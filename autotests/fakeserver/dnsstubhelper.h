/*
 *  SPDX-FileCopyrightText: 2026 Kenny Lorin <klo@enioka.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "mxproviderlookupjob_p.h"

#include <qlatin1stringview.h>
#include <qtestcase.h>

class DnsStubHelper
{
public:
    DnsStubHelper(const QString &searchDomain, const QString &answerDomain)
        : m_searchDomain(searchDomain)
        , m_answerDomain(answerDomain)
    {
        setStubData();
    }

    ~DnsStubHelper()
    {
        clearStubData();
    }

private:
    const QString m_searchDomain;
    const QString m_answerDomain;

    /*!
     * Sets the environment variable for the stub to read the data to return from
     */
    void setStubData()
    {
        const auto data = QLatin1String("%1:%2").arg(m_searchDomain, m_answerDomain);
        QVERIFY(qputenv(MXProviderLookupJob::STUB_DATA_ENVIRONMENT_KEY, data.toStdString()));
    }

    void clearStubData()
    {
        qunsetenv(MXProviderLookupJob::STUB_DATA_ENVIRONMENT_KEY);
    }
};
