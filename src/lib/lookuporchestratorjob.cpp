/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "lookuporchestratorjob_p.h"

#include "emailproviderlookupjob_p.h"
#include "ispdbproviderlookupjob_p.h"

enum ProviderIndex {
    EmailProvider = 0,
    ISPDBProvider,
    // Add more providers here as needed (e.g., MXProvider)
    ProviderCount // Always keep this last
};

LookupOrchestratorJob::LookupOrchestratorJob(const QString &email, QNetworkAccessManager *networkAccessManager, QObject *parent)
    : KCompositeJob(parent)
    , m_email(email)
    , m_networkAccessManager(networkAccessManager)
    , m_providerJob(nullptr)
{
}

LookupOrchestratorJob::~LookupOrchestratorJob() = default;

void LookupOrchestratorJob::start()
{
    m_currentProviderIndex = 0;
    tryNextProvider();
}

LookupProviderJobBase *LookupOrchestratorJob::providerJob()
{
    return m_providerJob;
}

LookupProviderJobBase *LookupOrchestratorJob::createProviderJob(int index)
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

void LookupOrchestratorJob::tryNextProvider()
{
    if (m_currentProviderIndex >= ProviderCount) {
        setError(LookupProviderJobBase::InvalidConfiguration);
        emitResult();
        return;
    }

    const auto job = createProviderJob(m_currentProviderIndex);

    addSubjob(job);
    job->start();
}

void LookupOrchestratorJob::slotResult(KJob *job)
{
    if (job && job->error() == NoError) {
        m_providerJob = qobject_cast<LookupProviderJobBase *>(job);
        emitResult();
        return;
    }

    m_currentProviderIndex++;
    tryNextProvider();
}
