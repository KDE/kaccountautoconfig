/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mxproviderlookupjob_p.h"
#include <QDnsLookup>
#include <QNetworkReply>
#include <QUrl>
#include <libpsl.h>
#include <qloggingcategory.h>

#include "kaccountautoconfig_debug.h"
#include "kaccountautoconfigjob.h"

MXProviderLookupJob::MXProviderLookupJob(const QString &email, QNetworkAccessManager *networkAccessManager)
    : LookupProviderJobBase(email, networkAccessManager)
    , m_dnsLookup(new QDnsLookup(this))
{
    m_dnsLookup->setType(QDnsLookup::MX);
    m_dnsLookup->setName(m_emailDomain);
}

MXProviderLookupJob::~MXProviderLookupJob() = default;

void MXProviderLookupJob::handleDummyNameServer()
{
    auto stubDataFromEnv = qEnvironmentVariable(STUB_DATA_ENVIRONMENT_KEY).split(QLatin1Char(':'));
    const auto &mxFullDomain = stubDataFromEnv.first();
    const auto &mxTopLevelDomain = stubDataFromEnv.last();

    qCInfo(KACCOUNTAUTOCONFIG_LOG) << "Using dummy nameserver for DNS lookup, requests on" << mxFullDomain << "will answer" << mxTopLevelDomain;

    if (m_emailDomain != mxFullDomain) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Requested domain different from the dummy one provided in environment, erroring out";
        setError(KAccountAutoconfigJob::ConfigurationNotFound);
        setErrorText(QStringLiteral("DNS Lookup failed"));
        emitResult();
        return;
    }

    if (mxTopLevelDomain.isEmpty()) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Dummy DNS lookup returned empty result, erroring out";
        setError(KAccountAutoconfigJob::ConfigurationNotFound);
        setErrorText(QStringLiteral("No MX record found"));
        emitResult();
        return;
    }

    handleDomainAndTryProviders(mxTopLevelDomain);
}

void MXProviderLookupJob::lookup()
{
    m_currentProvider = 0;

    if (!qEnvironmentVariableIsEmpty(STUB_DATA_ENVIRONMENT_KEY)) {
        handleDummyNameServer();
        return;
    }

    connect(m_dnsLookup, &QDnsLookup::finished, this, [this] {
        handleDnsReply();
    });
    m_dnsLookup->lookup();
}

void MXProviderLookupJob::handleDnsReply()
{
    if (m_dnsLookup->error() != QDnsLookup::NoError) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Dns error" << m_dnsLookup->errorString();
        setError(KAccountAutoconfigJob::ConfigurationNotFound);
        setErrorText(QStringLiteral("DNS Lookup failed"));
        emitResult();
        return;
    }

    const auto records = m_dnsLookup->mailExchangeRecords();
    if (records.isEmpty()) {
        setError(KAccountAutoconfigJob::ConfigurationNotFound);
        setErrorText(QStringLiteral("No MX record found"));
        emitResult();
        return;
    }

    // QDnsLookup order records by preference
    // the rfc state that only the mx domain with highest preference should be used so take the first of the records
    const auto &record = records.first();
    handleDomainAndTryProviders(record.exchange());
}

void MXProviderLookupJob::handleDomainAndTryProviders(const QString &fullDomain)
{
    // Get TopLevelDomain
    const psl_ctx_t *psl = psl_builtin();
    const QByteArray fullDomainUt8 = fullDomain.toUtf8();
    const char *registrable = psl_registrable_domain(psl, fullDomainUt8.constData());
    const auto topLevelDomain = registrable ? QString::fromUtf8(registrable) : fullDomain;

    qCInfo(KACCOUNTAUTOCONFIG_LOG) << "MX found with top level domain" << topLevelDomain << "and full domain" << fullDomain;
    // update placeholder before looking up to MXProvider
    populatePlaceHolderSubstitutions(fullDomain, topLevelDomain);
    tryNextMXProvider();
}

void MXProviderLookupJob::populatePlaceHolderSubstitutions(const QString &mxFullDomain, const QString &mxTopLevelDomain)
{
    addPlaceHolderSubstitution(QStringLiteral("MXFULLDOMAIN"), mxFullDomain);
    addPlaceHolderSubstitution(QStringLiteral("MXBASEDOMAIN"), mxTopLevelDomain);
}

void MXProviderLookupJob::tryNextMXProvider()
{
    constexpr auto mxProvidersUrls = std::to_array({QLatin1StringView("https://autoconfig.%{MXFULLDOMAIN}/mail/config-v1.1.xml?emailaddress=%{EMAILADDRESS}"),
                                                    QLatin1StringView("https://autoconfig.%{MXBASEDOMAIN}/mail/config-v1.1.xml?emailaddress=%{EMAILADDRESS}"),
                                                    QLatin1StringView("https://v1.ispdb.net/%{MXFULLDOMAIN}"),
                                                    QLatin1StringView("https://v1.ispdb.net/%{MXBASEDOMAIN}")});

    if (m_currentProvider >= mxProvidersUrls.size()) {
        setError(KAccountAutoconfigJob::ConfigurationNotFound);
        setErrorText(QStringLiteral("MX provider lookup couldn't resolve any configuration"));
        emitResult();
        return;
    }

    const auto currentEmailProvider = mxProvidersUrls[m_currentProvider++];
    const QUrl urlToCall(substitutePlaceholders(QString(currentEmailProvider)));
    qCDebug(KACCOUNTAUTOCONFIG_LOG) << "Looking Configuration into mx provider with url" << urlToCall;
    handleServerRequest(urlToCall);
}

void MXProviderLookupJob::handleServerRequest(const QUrl &url)
{
    const QNetworkRequest request(url);
    auto reply = m_networkAccessManager->get(request);

    connect(reply, &QNetworkReply::finished, reply, [reply, url, this] {
        // error will be handled by errorOccurred signal
        if (reply->error() != QNetworkReply::NoError) {
            return;
        }
        const auto data = parse(reply);
        if (!data) {
            qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Configuration invalid for url" << url;
            reply->deleteLater();
            tryNextMXProvider();
            return;
        }
        m_configurationData = data.value();
        m_configurationSource = url.toString();
        emitResult();
        qCInfo(KACCOUNTAUTOCONFIG_LOG) << "Configuration found for url" << url;
        reply->deleteLater();
    });

    connect(reply, &QNetworkReply::errorOccurred, reply, [reply, url, this] {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Configuration not found for url" << url << "with error code" << reply->errorString();
        reply->deleteLater();
        tryNextMXProvider();
    });
}
