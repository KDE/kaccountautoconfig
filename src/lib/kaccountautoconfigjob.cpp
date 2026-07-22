/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kaccountautoconfigjob.h"

#include "autoconfigparser_p.h"
#include "autoconfigserverhelper_p.h"
#include "emailproviderlookupjob_p.h"
#include "ispdbproviderlookupjob_p.h"
#include "kautoconfigserver.h"
#include "localfileproviderlookupjob_p.h"
#include <QNetworkAccessManager>

#include "mxproviderlookupjob_p.h"

using namespace AutoconfigServerHelper;

enum ProviderIndex {
    LocalProvider = 0,
    EmailProvider,
    ISPDBProvider,
    MXProvider,
    // Add more providers here as needed (e.g., MXProvider)
    ProviderCount // Always keep this last
};

class KAccountAutoconfigJobPrivate
{
public:
    explicit KAccountAutoconfigJobPrivate(KAccountAutoconfigJob *job, QNetworkAccessManager *networkAccessManager);

    KAccountAutoconfigJob *q;

    QString m_email;
    QList<KAutoconfigServer> m_servers;
    LookupProviderJobBase *m_providerJob;
    QNetworkAccessManager *m_networkAccessManager;

    int m_currentProviderIndex = 0;
    LookupProviderJobBase *createProviderJob(int index);
    void tryNextProvider();
    void parseServers();
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
    case LocalProvider:
        return new LocalFileProviderLookupJob(m_email, q);
    case EmailProvider:
        return new LookupEmailProviderJob(m_email, m_networkAccessManager);
    case ISPDBProvider:
        return new IspdbProviderLookupJob(m_email, m_networkAccessManager);
    case MXProvider:
        return new MXProviderLookupJob(m_email, m_networkAccessManager);
    default:
        Q_UNREACHABLE();
    }
}

void KAccountAutoconfigJobPrivate::tryNextProvider()
{
    if (m_currentProviderIndex >= ProviderCount) {
        q->setError(LookupProviderJobBase::InvalidConfiguration);
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
        // do not create a server for unsupported protocols
        if (!protocolHandled) {
            continue;
        }
        // multiple auth is present in the configuration, create one server per auth. This keeps the orders
        for (const auto &[system, method] : server.authentications) {
            m_servers.append(createServerFromParserEntry(server, protocolHandled.value(), method));
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
        qWarning() << "Got configuration from provider" << d->m_currentProviderIndex;
        d->m_providerJob = qobject_cast<LookupProviderJobBase *>(job);
        d->parseServers();
        emitResult();
        return;
    }

    removeSubjob(job);
    d->m_currentProviderIndex++;
    d->tryNextProvider();
}
