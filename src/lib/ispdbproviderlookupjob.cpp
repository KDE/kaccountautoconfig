/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ispdbproviderlookupjob_p.h"
#include <QNetworkReply>
#include <QNetworkRequest>

#include "kaccountautoconfig_debug.h"

constexpr auto ispdbProviderUrl = QLatin1StringView("https://v1.ispdb.net/%{EMAILDOMAIN}");

IspdbProviderLookupJob::IspdbProviderLookupJob(const QString &email, QNetworkAccessManager *networkAccessManager)
    : LookupProviderJobBase(email, networkAccessManager)
{
}

IspdbProviderLookupJob::~IspdbProviderLookupJob() = default;

void IspdbProviderLookupJob::lookup()
{
    QUrl url(substitutePlaceholders(QString(ispdbProviderUrl)));
    qCInfo(KACCOUNTAUTOCONFIG_LOG) << "Looking Configuration into ispdb with url" << url;

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
            setErrorText(QStringLiteral("ISPDB provider lookup configuration is invalid"));
            setError(ConfigurationParsingError);
            emitResult();
            return;
        }
        m_configurationData = data.value();
        m_configurationSource = url.toString();
        emitResult();
        qCInfo(KACCOUNTAUTOCONFIG_LOG) << "Configuration found for url" << url;
        reply->deleteLater();
    });

    connect(reply, &QNetworkReply::errorOccurred, this, [reply, url, this] {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Configuration not found for url" << url << "with error" << reply->errorString();
        reply->deleteLater();
        setError(ConfigurationNotFound);
        setErrorText(QStringLiteral("ISPDB provider lookup couldn't find configuration"));
        emitResult();
    });
}
