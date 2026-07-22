/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "fakeserver/stubnetworkaccessmanager.h"

#include <QSignalSpy>
#include <QTest>

#include "ispdbproviderlookupjob_p.h"

using Responses = StubNetworkAccessManager::Responses;

class IspdbProviderLookupJobTest : public QObject
{
    Q_OBJECT

    const QUrl validIspdbProviderUrl = QUrl(QStringLiteral("https://v1.ispdb.net/kde.org"));
    const QUrl invalidIspdbProviderUrl = QUrl(QStringLiteral("https://v1.ispdb.net/false.org"));
    // it's the domain that matters
    const QString validEmail = QStringLiteral("test@kde.org");
    const QString invalidEmail = QStringLiteral("test@invalid.org");

    QByteArray validConfiguration;
    QByteArray invalidConfiguration;

private Q_SLOTS:
    void initTestCase()
    {
        QFile validConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("example.xml")));
        QVERIFY(validConfigurationFile.open(QIODevice::ReadOnly));
        validConfiguration = validConfigurationFile.readAll();

        QFile invalidConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("invalid.xml")));
        QVERIFY(invalidConfigurationFile.open(QIODevice::ReadOnly));
        invalidConfiguration = invalidConfigurationFile.readAll();
    }

    void testRetrieveConfigurationFromIspdbProvider()
    {
        StubNetworkAccessManager stub;
        stub.addResponse({validIspdbProviderUrl, validConfiguration, 200});

        const auto job = new IspdbProviderLookupJob(validEmail, &stub);
        QSignalSpy spyJob(job, &IspdbProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->configuration().has_value(), true);
        QCOMPARE(job->source(), validIspdbProviderUrl.toString());
    }

    /*!
     * When a network error occurs, the job should return an error and the configuration should be empty.
     */
    void testNetworkError()
    {
        StubNetworkAccessManager stub;
        stub.addResponse({validIspdbProviderUrl, validConfiguration, 404});
        const auto job = new IspdbProviderLookupJob(validEmail, &stub);
        QSignalSpy spyJob(job, &IspdbProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->configuration().has_value(), false);
        QCOMPARE(job->source().isEmpty(), true);
        QCOMPARE(job->error(), LookupProviderJobBase::InvalidConfiguration);
    }

    /*!
     * When the configuration is invalid, the job should return an error and the configuration should be empty.
     */
    void testInvalidConfigurationError()
    {
        StubNetworkAccessManager stub;
        stub.addResponse({validIspdbProviderUrl, invalidConfiguration, 200});
        const auto job = new IspdbProviderLookupJob(validEmail, &stub);
        QSignalSpy spyJob(job, &IspdbProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->configuration().has_value(), false);
        QCOMPARE(job->source().isEmpty(), true);
        QCOMPARE(job->error(), LookupProviderJobBase::InvalidConfiguration);
    }

    /*!
     * When the configuration is invalid, the job should return an error and the configuration should be empty.
     */
    void testInvalidEmailError()
    {
        StubNetworkAccessManager stub;
        stub.addResponse({validIspdbProviderUrl, validConfiguration, 200});
        const auto job = new IspdbProviderLookupJob(invalidEmail, &stub);
        QSignalSpy spyJob(job, &IspdbProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->configuration().has_value(), false);
        QCOMPARE(job->source().isEmpty(), true);
        QCOMPARE(job->error(), LookupProviderJobBase::InvalidConfiguration);
    }
};

QTEST_MAIN(IspdbProviderLookupJobTest)

#include "ispdbproviderlookupjobtest.moc"
