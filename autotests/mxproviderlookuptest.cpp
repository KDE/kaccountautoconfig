/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "fakeserver/dnsstubhelper.h"

#include <QSignalSpy>
#include <QTest>

#include "fakeserver/stubnetworkaccessmanager.h"
#include "mxproviderlookupjob_p.h"

using UrlResponses = StubNetworkAccessManager::Responses;

class MXProviderLookupTest : public QObject
{
    Q_OBJECT
    const QString validEmail = QStringLiteral("test@kde.org");
    const QString validDomain = validEmail.split(u'@').last();
    // MXLookup will extract example.com
    const QString validDomainFromDns = QStringLiteral("test.example.com");

    // dns lookup for validEmail will be valiDomainFromDns, to test full domain extraction search on example.com and test.example.com
    const QUrl validAutoConfigUrlFullDomain = QUrl(QStringLiteral("https://autoconfig.test.example.com/mail/config-v1.1.xml?emailaddress=test@kde.org"));
    const QUrl validAutoConfigUrlTopLevelDomain = QUrl(QStringLiteral("https://autoconfig.example.com/mail/config-v1.1.xml?emailaddress=test@kde.org"));
    const QUrl validIspdbProviderUrlFullDomain = QUrl(QStringLiteral("https://v1.ispdb.net/test.example.com"));
    const QUrl validIspdbProviderUrlTopLevelDomain = QUrl(QStringLiteral("https://v1.ispdb.net/example.com"));

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

    void testValidMXConfiguration_data()
    {
        QTest::addColumn<UrlResponses>("urlResponses");
        QTest::addColumn<QUrl>("expectedSource");

        QTest::newRow("valid_autoconfig_full_domain") << UrlResponses{{{validAutoConfigUrlFullDomain, validConfiguration, 200},
                                                                       {validAutoConfigUrlTopLevelDomain, validConfiguration, 200},
                                                                       {validIspdbProviderUrlFullDomain, validConfiguration, 200},
                                                                       {validIspdbProviderUrlTopLevelDomain, validConfiguration, 200}}}
                                                      << validAutoConfigUrlFullDomain;

        QTest::newRow("valid_autoconfig_top_level_domain") << UrlResponses{{{validAutoConfigUrlFullDomain, validConfiguration, 404},
                                                                            {validAutoConfigUrlTopLevelDomain, validConfiguration, 200},
                                                                            {validIspdbProviderUrlFullDomain, validConfiguration, 200},
                                                                            {validIspdbProviderUrlTopLevelDomain, validConfiguration, 200}}}
                                                           << validAutoConfigUrlTopLevelDomain;

        QTest::newRow("valid_ispdb_full_domain") << UrlResponses{{{validAutoConfigUrlFullDomain, validConfiguration, 404},
                                                                  {validAutoConfigUrlTopLevelDomain, invalidConfiguration, 200},
                                                                  {validIspdbProviderUrlFullDomain, validConfiguration, 200},
                                                                  {validIspdbProviderUrlTopLevelDomain, validConfiguration, 200}}}
                                                 << validIspdbProviderUrlFullDomain;

        QTest::newRow("valid_ispdb_top_level_domain") << UrlResponses{{{validAutoConfigUrlFullDomain, validConfiguration, 404},
                                                                       {validAutoConfigUrlTopLevelDomain, invalidConfiguration, 200},
                                                                       {validIspdbProviderUrlFullDomain, validConfiguration, 404},
                                                                       {validIspdbProviderUrlTopLevelDomain, validConfiguration, 200}}}
                                                      << validIspdbProviderUrlTopLevelDomain;
    }

    /*!
     * Test at least on valid MXLookup with error combination
     */
    void testValidMXConfiguration()
    {
        QFETCH(UrlResponses, urlResponses);
        QFETCH(QUrl, expectedSource);

        StubNetworkAccessManager stubNetwork;
        stubNetwork.addResponses(urlResponses);

        DnsStubHelper dnsStubData(validDomain, validDomainFromDns);
        const auto job = new MXProviderLookupJob(validEmail, &stubNetwork);
        QSignalSpy spyJob(job, &MXProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->error(), 0);
        QCOMPARE(job->configuration().has_value(), true);
        QCOMPARE(job->source(), expectedSource.toString());
    }

    void testMXDnsPublicSuffix()
    {
        const auto validSourceWithPublicSuffix = QStringLiteral("https://v1.ispdb.net/example.com.fr");
        StubNetworkAccessManager stub;
        stub.addResponse({QUrl(validSourceWithPublicSuffix), validConfiguration, 200});

        DnsStubHelper dnsStubData(validDomain, QStringLiteral("test.example.com.fr"));
        const auto job = new MXProviderLookupJob(validEmail, &stub);
        QSignalSpy spyJob(job, &MXProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->error(), 0);
        QCOMPARE(job->configuration().has_value(), true);
        QCOMPARE(job->source(), validSourceWithPublicSuffix);
    }

    /*!
     * Test DNSLookup Error
     */
    void testInvalidMailMXConfiguration()
    {
        StubNetworkAccessManager stubnetwork;
        stubnetwork.addResponse({validIspdbProviderUrlTopLevelDomain, validConfiguration, 200});

        DnsStubHelper dnsStubData(validDomain, validDomainFromDns);
        const auto job = new MXProviderLookupJob(invalidEmail, &stubnetwork);
        QSignalSpy spyJob(job, &MXProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->error(), LookupProviderJobBase::Error::InvalidConfiguration);
        QCOMPARE(job->configuration().has_value(), false);
        QCOMPARE(job->source().isEmpty(), true);
        QCOMPARE(job->errorText(), QStringLiteral("DNS Lookup failed"));
    }

    /*!
     * Test DNSLookup returning no MX record
     */
    void testNoMXRecord()
    {
        StubNetworkAccessManager stubnetwork;
        stubnetwork.addResponse({validIspdbProviderUrlTopLevelDomain, validConfiguration, 200});

        DnsStubHelper dnsStubData(validDomain, {});
        const auto job = new MXProviderLookupJob(validEmail, &stubnetwork);
        QSignalSpy spyJob(job, &MXProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->error(), LookupProviderJobBase::Error::InvalidConfiguration);
        QCOMPARE(job->configuration().has_value(), false);
        QCOMPARE(job->source().isEmpty(), true);
        QCOMPARE(job->errorText(), QStringLiteral("No MX record found"));
    }

    /*!
     * test to make sure everything works when calling real DNS and real provider.
     * Skip it, to be used only locally
     */
    void testWithRealImplementation()
    {
        QSKIP("Skip it, to be used only locally");
        QNetworkAccessManager networkManager;
        const auto job = new MXProviderLookupJob(QStringLiteral("test@gmail.com"), &networkManager);
        QSignalSpy spyJob(job, &MXProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->error(), 0);
        QCOMPARE(job->configuration().has_value(), true);
        // the actual dns lookup for gmail is google.com
        QCOMPARE(job->source(), QUrl(QStringLiteral("https://v1.ispdb.net/google.com")).toString());
    }
};

QTEST_MAIN(MXProviderLookupTest)

#include "mxproviderlookuptest.moc"
