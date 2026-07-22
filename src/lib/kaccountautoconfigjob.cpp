/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kaccountautoconfigjob.h"

#include "autoconfigparser_p.h"
#include "emailproviderlookupjob_p.h"
#include "ispdbproviderlookupjob_p.h"
#include "kautoconfigserver.h"
#include <QNetworkAccessManager>

enum ProviderIndex {
    EmailProvider = 0,
    ISPDBProvider,
    // Add more providers here as needed (e.g., MXProvider)
    ProviderCount // Always keep this last
};

class KAccountAutoconfigJobPrivate
{
public:
    explicit KAccountAutoconfigJobPrivate(KAccountAutoconfigJob *job, QNetworkAccessManager *networkAccessManager);

    QString m_email;
    QList<KAutoconfigServer> m_servers;
    LookupProviderJobBase *m_providerJob;
    QNetworkAccessManager *m_networkAccessManager;

    int m_currentProviderIndex = 0;
    LookupProviderJobBase *createProviderJob(int index);
    void tryNextProvider();
    void parseServers();
    static std::optional<KAutoconfigServer::Protocol> protocolFromString(const QString &protocolStr);
    static KAutoconfigServer::AuthenticationType authenticationFromString(const QString &authenticationStr);
    static KAutoconfigServer::SocketType socketTypeFromString(const QString &socketType);
    /*!
     * Create a url from the configuration for KAutoconfigServer.
     */
    static QUrl handleUrl(KAutoconfigServer::Protocol protocol, const AutoConfigParser::Server &server);

    KAccountAutoconfigJob *q;
};

KAccountAutoconfigJobPrivate::KAccountAutoconfigJobPrivate(KAccountAutoconfigJob *job, QNetworkAccessManager *networkAccessManager)
    : q(job)
    , m_networkAccessManager(networkAccessManager)
{
}

KAccountAutoconfigJob::KAccountAutoconfigJob(QObject *parent)
    : KCompositeJob(parent)
    , d(new KAccountAutoconfigJobPrivate(this, new QNetworkAccessManager(this)))
{
}

KAccountAutoconfigJob::KAccountAutoconfigJob(QNetworkAccessManager *networkAccessManager, QObject *parent)
    : KCompositeJob(parent)
    , d(new KAccountAutoconfigJobPrivate(this, networkAccessManager))
{
}

LookupProviderJobBase *KAccountAutoconfigJobPrivate::createProviderJob(int index)
{
    Q_ASSERT(index < ProviderCount);
    switch (index) {
    case EmailProvider:
        return new LookupEmailProviderJob(m_email, m_networkAccessManager);
    case ISPDBProvider:
        return new IspdbProviderLookupJob(m_email, m_networkAccessManager);
    default:
        Q_UNREACHABLE();
    }
}

void KAccountAutoconfigJobPrivate::tryNextProvider()
{
    if (m_currentProviderIndex >= ProviderCount) {
        q->setError(LookupProviderJobBase::ConfigurationNotFound);
        q->emitResult();
        return;
    }

    const auto job = createProviderJob(m_currentProviderIndex);
    q->addSubjob(job);
    job->start();
}

void KAccountAutoconfigJobPrivate::parseServers()
{
    const auto servers = m_providerJob->configuration().value().servers;

    for (const auto &server : servers) {
        const auto protocolHandled = protocolFromString(server.type);
        if (!protocolHandled) {
            continue;
        }
        // multiple auth is present in the configuration, create one server per auth. This keeps the orders
        for (const auto &auth : server.authentications) {
            KAutoconfigServer publicServer(protocolHandled.value());
            publicServer.setUrl(handleUrl(protocolHandled.value(), server));
            publicServer.setSocketType(socketTypeFromString(server.socketType));
            const auto authHandled = authenticationFromString(auth.method);
            publicServer.setAuthenticationType(authHandled);
            m_servers.emplace_back(publicServer);
        }
    }
}

KAccountAutoconfigJob::~KAccountAutoconfigJob() = default;

void KAccountAutoconfigJob::start()
{
    d->m_currentProviderIndex = 0;
    d->tryNextProvider();
}

QString KAccountAutoconfigJob::email() const
{
    return d->m_email;
}

void KAccountAutoconfigJob::setEmail(const QString &newEmail)
{
    d->m_email = newEmail;
}

QList<KAutoconfigServer> KAccountAutoconfigJob::servers() const
{
    return d->m_servers;
}

void KAccountAutoconfigJob::slotResult(KJob *job)
{
    if (job && job->error() == NoError) {
        d->m_providerJob = qobject_cast<LookupProviderJobBase *>(job);
        d->parseServers();
        emitResult();
        return;
    }

    d->m_currentProviderIndex++;
    d->tryNextProvider();
}

std::optional<KAutoconfigServer::Protocol> KAccountAutoconfigJobPrivate::protocolFromString(const QString &protocolStr)
{
    static const QHash<QString, KAutoconfigServer::Protocol> protocolMap{
        {QStringLiteral("imap"), KAutoconfigServer::IMAP},
        {QStringLiteral("pop3"), KAutoconfigServer::POP3},
        {QStringLiteral("smtp"), KAutoconfigServer::SMTP},
        {QStringLiteral("caldav"), KAutoconfigServer::CalDAV},
        {QStringLiteral("carddav"), KAutoconfigServer::CardDAV},
    };
    if (!protocolMap.contains(protocolStr.toLower())) {
        return std::nullopt;
    }
    return protocolMap.value(protocolStr.toLower());
}

KAutoconfigServer::AuthenticationType KAccountAutoconfigJobPrivate::authenticationFromString(const QString &authenticationStr)
{
    static const QHash<QString, KAutoconfigServer::AuthenticationType> authenticationMap{
        {QStringLiteral("password-cleartext"), KAutoconfigServer::AuthenticationType::PasswordCleartext},
        {QStringLiteral("password-encrypted"), KAutoconfigServer::AuthenticationType::PasswordEncrypted},
        {QStringLiteral("basic"), KAutoconfigServer::AuthenticationType::Basic},
        {QStringLiteral("digest"), KAutoconfigServer::AuthenticationType::Digest},
    };

    return authenticationMap.value(authenticationStr.toLower(), KAutoconfigServer::AuthenticationType::UnknownAuthentication);
}

KAutoconfigServer::SocketType KAccountAutoconfigJobPrivate::socketTypeFromString(const QString &socketType)
{
    static const QHash<QString, KAutoconfigServer::SocketType> socketTypeMap{
        {QStringLiteral("SSL"), KAutoconfigServer::SocketType::SSL},
        {QStringLiteral("STARTTLS"), KAutoconfigServer::SocketType::STARTTLS},
    };

    return socketTypeMap.value(socketType.toUpper(), KAutoconfigServer::SocketType::UnknownSocketType);
}

QUrl KAccountAutoconfigJobPrivate::handleUrl(KAutoconfigServer::Protocol protocol, const AutoConfigParser::Server &server)
{
    static const QList hostnameBasedProtocol{KAutoconfigServer::IMAP, KAutoconfigServer::POP3, KAutoconfigServer::SMTP};
    QUrl url(server.url);
    if (server.port != 0) {
        url.setPort(server.port);
    }
    if (!hostnameBasedProtocol.contains(protocol)) {
        return url;
    }
    // override host and url if hostnameBasedProtocol
    url.setScheme(server.type);
    if (protocol == KAutoconfigServer::POP3) {
        // according to https://www.iana.org/assignments/uri-schemes/uri-schemes.xhtml
        // pop3 should be pop
        url.setScheme(QStringLiteral("pop"));
    }
    url.setHost(server.hostname);
    return url;
}
