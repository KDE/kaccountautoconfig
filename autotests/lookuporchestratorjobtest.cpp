/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QSignalSpy>

#include "fakeserver/stubnetworkaccessmanager.h"

#include <QTest>

#include "lib/LookupProviderJobBase_p.h"
#include "lib/lookuporchestratorjob_p.h"

using Responses = StubNetworkAccessManager::Responses;

class LookupOrchestratorJobTest : public QObject
{
    Q_OBJECT

    const QUrl validWellKnownUrl = QUrl(QStringLiteral("https://kde.org/.well-known/autoconfig/mail/config-v1.1.xml"));
    const QUrl validIspdbUrl = QUrl(QStringLiteral("https://v1.ispdb.net/kde.org"));
    const QString validEmail = QStringLiteral("test@kde.org");
    QByteArray validConfiguration;

private Q_SLOTS:
    void initTestCase()
    {
        QFile validConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("example.xml")));
        QVERIFY(validConfigurationFile.open(QIODevice::ReadOnly));
        validConfiguration = validConfigurationFile.readAll();
    }

    void testLookupOrchestratorJob_data()
    {
        QTest::addColumn<Responses>("responses");
        QTest::addColumn<QUrl>("expectedSource");

        QTest::newRow("email_provider_valid") << Responses{{{validWellKnownUrl, validConfiguration}, {validIspdbUrl, validConfiguration}}} << validWellKnownUrl;
        QTest::newRow("ispdb_provider_valid") << Responses{{{validWellKnownUrl, validConfiguration, 404}, {validIspdbUrl, validConfiguration, 200}}}
                                              << validIspdbUrl;
    }

    /*!
     * Test provider orders
     */
    void testLookupOrchestratorJob()
    {
        QFETCH(Responses, responses);
        QFETCH(QUrl, expectedSource);

        StubNetworkAccessManager stub;
        stub.addResponses(responses);

        const auto job = new LookupOrchestratorJob(validEmail, &stub);
        QSignalSpy spyJob(job, &LookupOrchestratorJob::result);

        job->start();
        spyJob.wait();
        const auto lookupJob = job->providerJob();

        QCOMPARE(lookupJob->configuration().has_value(), true);
        QCOMPARE(lookupJob->source(), expectedSource.toString());
        QCOMPARE(lookupJob->error(), 0);
    };

    void testNoneValidProvider()
    {
        const Responses responses{{{validWellKnownUrl, validConfiguration, 404}, {validIspdbUrl, validConfiguration, 404}}};
        StubNetworkAccessManager stub;
        stub.addResponses(responses);
        const auto job = new LookupOrchestratorJob(validEmail, &stub);
        QSignalSpy spyJob(job, &LookupOrchestratorJob::result);
        job->start();
        spyJob.wait();

        const auto lookupJob = job->providerJob();

        QCOMPARE(job->error(), LookupProviderJobBase::InvalidConfiguration);
        QCOMPARE(lookupJob, nullptr);
    }
};

QTEST_MAIN(LookupOrchestratorJobTest)

#include "lookuporchestratorjobtest.moc"
