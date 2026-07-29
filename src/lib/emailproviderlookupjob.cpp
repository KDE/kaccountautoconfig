/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "emailproviderlookupjob_p.h"
#include <QNetworkReply>

#include "kaccountautoconfig_debug.h"

LookupEmailProviderJob::LookupEmailProviderJob(const QString &email, QNetworkAccessManager *networkAccessManager)
    : LookupProviderJobBase(email, networkAccessManager)
{
}

LookupEmailProviderJob::~LookupEmailProviderJob() = default;

void LookupEmailProviderJob::lookup()
{
    // Reset the lookup at every start to ensure the order is respected and avoid side effects
    m_currentEmailProvider = 0;
    tryNextEmailProvider();
}

void LookupEmailProviderJob::tryNextEmailProvider()
{
    constexpr auto emailProvidersUrls = std::to_array({
        QLatin1StringView("https://autoconfig.%{EMAILDOMAIN}/mail/config-v1.1.xml?emailaddress=%{EMAILADDRESS}"),
        QLatin1StringView("https://%{EMAILDOMAIN}/.well-known/autoconfig/mail/config-v1.1.xml"),
        QLatin1StringView("http://autoconfig.%{EMAILDOMAIN}/mail/config-v1.1.xml"),
    });

    if (m_currentEmailProvider >= emailProvidersUrls.size()) {
        // this job always returns ConfigurationNoFound as it tries next email provider if a parsing error during the lookup
        setError(InvalidConfiguration);
        setErrorText(QStringLiteral("Email provider lookup couldn't resolve any configuration"));
        emitResult();
        return;
    }

    const auto currentEmailProvider = emailProvidersUrls[m_currentEmailProvider++];
    const QUrl urlToCall(substitutePlaceholders(QString(currentEmailProvider)));
    qCDebug(KACCOUNTAUTOCONFIG_LOG) << "Looking Configuration into MailProvider with url" << urlToCall;
    handleServerRequest(urlToCall);
}

void LookupEmailProviderJob::handleServerRequest(const QUrl &url)
{
    const QNetworkRequest request(url);
    auto reply = m_networkAccessManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [reply, url, this] {
        // error will be handled by errorOccurred signal
        if (reply->error() != QNetworkReply::NoError) {
            return;
        }
        const auto data = parse(reply);
        if (!data) {
            qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Configuration invalid for url" << url;
            reply->deleteLater();
            tryNextEmailProvider();
            return;
        }
        m_configurationData = data.value();
        m_configurationSource = url.toString();
        emitResult();
        qCInfo(KACCOUNTAUTOCONFIG_LOG) << "Configuration found for url" << url;
        reply->deleteLater();
    });

    connect(reply, &QNetworkReply::errorOccurred, this, [reply, url, this] {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Configuration not found for url" << url << "with error code" << reply->errorString();
        reply->deleteLater();
        tryNextEmailProvider();
    });
}
