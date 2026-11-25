/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "lookupproviderjobbase_p.h"

#include <QDnsLookup>

class MXProviderLookupTest;
class KAccountAutoconfigTest;

/*!
 * \internal
 * \warning: If environment variable `KACCOUNTAUTOCONFIG_USE_DUMMY_NAMESERVER` is set and non-empty, use a stub DNS lookup implementation.
 */
class MXProviderLookupJob : public LookupProviderJobBase
{
    Q_OBJECT

    friend MXProviderLookupTest;
    friend KAccountAutoconfigTest;

public:
    constexpr static auto STUB_DATA_ENVIRONMENT_KEY = "KACCOUNTAUTOCONFIG_USE_DUMMY_NAMESERVER";

    explicit MXProviderLookupJob(const QString &email, QNetworkAccessManager *networkAccessManager);
    ~MXProviderLookupJob() override;

private:
    size_t m_currentProvider = 0;
    QDnsLookup *m_dnsLookup;

    void lookup() override;
    void handleDummyNameServer();
    void handleDnsReply();
    void handleDomainAndTryProviders(const QString &fullDomain);
    /*!
     * Add placeholder substitutions from LookupProviderJobBase.
     * DnsLookup will provide new value {MXFULLDOMAIN} and {MXBASEDOMAIN} to be substituted when lookup for configuration.
     */
    void populatePlaceHolderSubstitutions(const QString &mxFullDomain, const QString &mxTopLevelDomain);
    void tryNextMXProvider();
    void handleServerRequest(const QUrl &url);
};
