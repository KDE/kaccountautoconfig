/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once
#include <KCompositeJob>

class LookupProviderJobBase;
class QNetworkAccessManager;

/*!
 * \brief This job orchestrates provider jobs to find a valid configuration following the orders defined in the RFC
 * https://benbucksch.github.io/autoconfig-spec/draft-ietf-mailmaint-autoconfig.html
 *
 * Stops at the first provider that successfully retrieves and parses a configuration.
 */
class LookupOrchestratorJob : public KCompositeJob
{
    Q_OBJECT

public:
    explicit LookupOrchestratorJob(const QString &email, QNetworkAccessManager *networkAccessManager, QObject *parent = nullptr);
    ~LookupOrchestratorJob() override;

    void start() override;
    /*!
     * Return the lookup provider job that actually retrieves and parses a configuration.
     * Only valid if the job has started and finished without errors.
     */
    [[nodiscard]] LookupProviderJobBase *providerJob();

protected:
    void slotResult(KJob *job) override;

private:
    QString m_email;
    QNetworkAccessManager *m_networkAccessManager;
    int m_currentProviderIndex = 0;
    LookupProviderJobBase *m_providerJob;

    LookupProviderJobBase *createProviderJob(int index);
    void tryNextProvider();
};
