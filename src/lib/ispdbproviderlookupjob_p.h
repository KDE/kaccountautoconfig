/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once
#include "lookupproviderjobbase_p.h"

class QNetworkAccessManager;

/*!
 * \internal
 * \brief Lookup for configuration in ISPDB as described in RFC
 * https://benbucksch.github.io/autoconfig-spec/draft-ietf-mailmaint-autoconfig.html#name-central-database
 *
 * This job will just look directly in ISPDB.
 */
class IspdbProviderLookupJob : public LookupProviderJobBase
{
    Q_OBJECT

public:
    explicit IspdbProviderLookupJob(const QString &email, QNetworkAccessManager *networkAccessManager);
    ~IspdbProviderLookupJob() override;

private:
    void lookup() override;
};
