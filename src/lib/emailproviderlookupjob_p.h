/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "lookupproviderjobbase_p.h"

class QNetworkAccessManager;

/*!
 * \brief Lookup for configuration according to email provider as described in RFC
 * https://benbucksch.github.io/autoconfig-spec/draft-ietf-mailmaint-autoconfig.html#name-mail-provider
 *
 * This job will loop through email providers to find the configuration. The first valid configuration will stop the job.
 */
class LookupEmailProviderJob : public LookupProviderJobBase
{
    Q_OBJECT

public:
    explicit LookupEmailProviderJob(const QString &email, QNetworkAccessManager *networkAccessManager);
    ~LookupEmailProviderJob() override;

private Q_SLOTS:
    void tryNextEmailProvider();
    void handleServerRequest(const QUrl &url);

private:
    size_t m_currentEmailProvider = 0;
    void lookup() override;
};
