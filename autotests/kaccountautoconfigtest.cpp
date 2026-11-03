/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "fakeserver/dnsstubhelper.h"

#include <QSignalSpy>
#include <QStandardPaths>

#include <kaccountautoconfig/kaccountautoconfigjob.h>

#include <QTest>

#include "fakeserver/stubnetworkaccessmanager.h"

using namespace Qt::StringLiterals;
using Responses = StubNetworkAccessManager::Responses;

class KAccountAutoconfigTest : public QObject
{
    Q_OBJECT

    const QUrl validWellKnownUrl = QUrl(QStringLiteral("https://kde.org/.well-known/autoconfig/mail/config-v1.1.xml"));
    const QUrl validIspdbUrl = QUrl(QStringLiteral("https://v1.ispdb.net/kde.org"));

    // to make a difference with ispdb provider, dns lookup will return example.com and by doing so MX provider should search for example.com
    const QString dnsLookupValid = QStringLiteral("test.example.com");
    const QUrl validMXProviderUrl = QUrl(QStringLiteral("https://v1.ispdb.net/example.com"));

    const QString validEmail = QStringLiteral("test@kde.org");
    const QString validDomain = validEmail.split(u'@').last();

    QByteArray validConfiguration;
    QByteArray validEmailProviderConfiguration;
    QByteArray validIspdbConfiguration;
    QByteArray validMXConfiguration;

private Q_SLOTS:
    void testInit()
    {
        const auto job = new KAccountAutoconfigJob(this);
        Q_ASSERT(job->servers().size() == 0);
    }

    void setLocalConfigurationFile()
    {
        // dest file should be in an "isp" directory, and the filename should be the same as the domain name of the email, according to the rfc,
        const QString validUserConfigPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QStringLiteral("/isp/kde.org.xml");

        // remove if exist
        QFile::remove(validUserConfigPath);

        QVERIFY(QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).mkpath(QStringLiteral("isp/")));
        QFile::copy(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("local_file_provider.xml")), validUserConfigPath);
    }

    void removeLocalConfigurationFile()
    {
        // dest file should be in an "isp" directory, and the filename should be the same as the domain name of the email, according to the rfc,
        const QString validUserConfigPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QStringLiteral("/isp/kde.org.xml");

        // remove if exist
        QFile::remove(validUserConfigPath);
    }

    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);

        QFile validConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("example.xml")));
        QVERIFY(validConfigurationFile.open(QIODevice::ReadOnly));
        validConfiguration = validConfigurationFile.readAll();

        QFile validEmailProviderConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("email_provider.xml")));
        QVERIFY(validEmailProviderConfigurationFile.open(QIODevice::ReadOnly));
        validEmailProviderConfiguration = validEmailProviderConfigurationFile.readAll();

        QFile validIspdbProviderConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("ispdb_provider.xml")));
        QVERIFY(validIspdbProviderConfigurationFile.open(QIODevice::ReadOnly));
        validIspdbConfiguration = validIspdbProviderConfigurationFile.readAll();

        QFile validMxProviderConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("ispdb_provider.xml")));
        QVERIFY(validMxProviderConfigurationFile.open(QIODevice::ReadOnly));
        validMXConfiguration = validMxProviderConfigurationFile.readAll();
    }

    /*!
     * Test the provider lookup mechanism.
     * Data varies per provider, allowing implicit validation of the source
     * without exposing unnecessary functions in the public API.
     */
    void testAccountAutoconfigProvider_data()
    {
        QTest::addColumn<bool>("searchLocalFile");
        QTest::addColumn<Responses>("responses");
        QTest::addColumn<QList<KAccountAutoconfigServer>>("expectedServers");

        KAccountAutoconfigServer emailProviderServer(KAccountAutoconfigServer::IMAP);
        emailProviderServer.setUrl(QUrl("imap://email.provider.com:993"_L1));
        emailProviderServer.setAuthenticationType(KAccountAutoconfigServer::PasswordCleartext);
        emailProviderServer.setSocketType(KAccountAutoconfigServer::SSL);

        KAccountAutoconfigServer ispdbProviderServer(KAccountAutoconfigServer::IMAP);
        ispdbProviderServer.setUrl(QUrl("imap://ispdb.provider.com:993"_L1));
        ispdbProviderServer.setAuthenticationType(KAccountAutoconfigServer::PasswordCleartext);
        ispdbProviderServer.setSocketType(KAccountAutoconfigServer::SSL);

        KAccountAutoconfigServer localProviderServer(KAccountAutoconfigServer::IMAP);
        localProviderServer.setUrl(QUrl("imap://local.provider.com:993"_L1));
        localProviderServer.setAuthenticationType(KAccountAutoconfigServer::PasswordCleartext);
        localProviderServer.setSocketType(KAccountAutoconfigServer::SSL);

        KAccountAutoconfigServer MXProviderServer(KAccountAutoconfigServer::IMAP);
        MXProviderServer.setUrl(QUrl("imap://mx.provider.com:993"_L1));
        MXProviderServer.setAuthenticationType(KAccountAutoconfigServer::PasswordCleartext);
        MXProviderServer.setSocketType(KAccountAutoconfigServer::SSL);

        // the orders should be the same as in the XML
        QTest::newRow("email_provider_valid") << false
                                              << Responses{{{validWellKnownUrl, validEmailProviderConfiguration}, {validIspdbUrl, validIspdbConfiguration}}}
                                              << QList{emailProviderServer};
        QTest::newRow("ispdb_provider_valid") << false
                                              << Responses{{{validWellKnownUrl, validEmailProviderConfiguration, 404},
                                                            {validIspdbUrl, validIspdbConfiguration, 200}}}
                                              << QList{ispdbProviderServer};

        QTest::newRow("local_file_provider_valid") << true
                                                   << Responses{{{validWellKnownUrl, validEmailProviderConfiguration, 404},
                                                                 {validIspdbUrl, validIspdbConfiguration, 404}}}
                                                   << QList{localProviderServer};

        QTest::newRow("mx_provider_valid") << false
                                           << Responses{{{validWellKnownUrl, validEmailProviderConfiguration, 404},
                                                         {validIspdbUrl, validIspdbConfiguration, 404},
                                                         {validMXProviderUrl, validMXConfiguration, 200}}}
                                           << QList{ispdbProviderServer};
    }

    void testAccountAutoconfigProvider()
    {
        QFETCH(bool, searchLocalFile);
        QFETCH(Responses, responses);
        QFETCH(QList<KAccountAutoconfigServer>, expectedServers);

        StubNetworkAccessManager stub;
        stub.addResponses(responses);
        DnsStubHelper dnsStubData(validDomain, dnsLookupValid);

        if (searchLocalFile) {
            setLocalConfigurationFile();
        } else {
            removeLocalConfigurationFile();
        }

        const auto job = new KAccountAutoconfigJob(&stub, this);
        job->setEmail(validEmail);
        QSignalSpy spyJob(job, &KAccountAutoconfigJob::result);

        job->start();
        spyJob.wait();

        QCOMPARE(job->error(), 0);

        const auto servers = job->servers();
        QCOMPARE(servers, expectedServers);

        // check that all servers are valid
        QVERIFY(std::ranges::all_of(servers, [](const auto &server) {
            return server.isValid();
        }));
    }

    void testAccountAutoconfigMultipleProtocolAndAuth_data()
    {
        QTest::addColumn<QByteArray>("configuration");
        QTest::addColumn<QList<KAccountAutoconfigServer>>("expectedServers");
        // configuration with multiple imap,pop,stmp and multiple auth
        QFile validConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("multiple_protocol_and_authentication.xml")));
        QVERIFY(validConfigurationFile.open(QIODevice::ReadOnly));
        const QByteArray configuration = validConfigurationFile.readAll();

        // three pop servers, first on with two auth in the xml
        KAccountAutoconfigServer firstPopServer(KAccountAutoconfigServer::POP3);
        firstPopServer.setUrl(QUrl("pop://pop.example.com:995"_L1));
        firstPopServer.setAuthenticationType(KAccountAutoconfigServer::PasswordCleartext);
        firstPopServer.setSocketType(KAccountAutoconfigServer::SSL);

        KAccountAutoconfigServer secondPopServer(KAccountAutoconfigServer::POP3);
        secondPopServer.setUrl(QUrl("pop://pop.example.com:995"_L1));
        secondPopServer.setAuthenticationType(KAccountAutoconfigServer::PasswordEncrypted);
        secondPopServer.setSocketType(KAccountAutoconfigServer::SSL);

        KAccountAutoconfigServer thirdPopServer(KAccountAutoconfigServer::POP3);
        thirdPopServer.setUrl(QUrl("pop://pop.example.com:110"_L1));
        thirdPopServer.setAuthenticationType(KAccountAutoconfigServer::PasswordCleartext);
        thirdPopServer.setSocketType(KAccountAutoconfigServer::STARTTLS);

        // three imap servers, second with two auth in the xml
        KAccountAutoconfigServer firstImapServer(KAccountAutoconfigServer::IMAP);
        firstImapServer.setUrl(QUrl("imap://imap.example.com:993"_L1));
        firstImapServer.setAuthenticationType(KAccountAutoconfigServer::PasswordCleartext);
        firstImapServer.setSocketType(KAccountAutoconfigServer::SSL);

        KAccountAutoconfigServer secondImapServer(KAccountAutoconfigServer::IMAP);
        secondImapServer.setUrl(QUrl("imap://imap.example.com:993"_L1));
        secondImapServer.setAuthenticationType(KAccountAutoconfigServer::PasswordEncrypted);
        secondImapServer.setSocketType(KAccountAutoconfigServer::SSL);

        KAccountAutoconfigServer thirdImapServer(KAccountAutoconfigServer::IMAP);
        thirdImapServer.setUrl(QUrl("imap://imap.example.com:143"_L1));
        thirdImapServer.setAuthenticationType(KAccountAutoconfigServer::PasswordCleartext);
        thirdImapServer.setSocketType(KAccountAutoconfigServer::STARTTLS);

        // three smtp servers, second with two auth in the xml
        KAccountAutoconfigServer firstSmtpServer(KAccountAutoconfigServer::SMTP);
        firstSmtpServer.setUrl(QUrl("smtp://smtp.googlemail.com:465"_L1));
        firstSmtpServer.setAuthenticationType(KAccountAutoconfigServer::PasswordCleartext);
        firstSmtpServer.setSocketType(KAccountAutoconfigServer::SSL);

        KAccountAutoconfigServer secondSmtpServer(KAccountAutoconfigServer::SMTP);
        secondSmtpServer.setUrl(QUrl("smtp://smtp.googlemail.com:587"_L1));
        secondSmtpServer.setAuthenticationType(KAccountAutoconfigServer::PasswordCleartext);
        secondSmtpServer.setSocketType(KAccountAutoconfigServer::STARTTLS);

        KAccountAutoconfigServer thirdSmtpServer(KAccountAutoconfigServer::SMTP);
        thirdSmtpServer.setUrl(QUrl("smtp://smtp.googlemail.com:587"_L1));
        thirdSmtpServer.setAuthenticationType(KAccountAutoconfigServer::PasswordEncrypted);
        thirdSmtpServer.setSocketType(KAccountAutoconfigServer::STARTTLS);

        // the orders should be the same as in the XML
        const QList validServers = {firstPopServer,
                                    secondPopServer,
                                    thirdPopServer,
                                    firstImapServer,
                                    secondImapServer,
                                    firstSmtpServer,
                                    thirdImapServer,
                                    secondSmtpServer,
                                    thirdSmtpServer};

        QTest::newRow("servers_order_test") << configuration << validServers;
    }

    void testAccountAutoconfigMultipleProtocolAndAuth()
    {
        QFETCH(QList<KAccountAutoconfigServer>, expectedServers);
        QFETCH(QByteArray, configuration);
        QCOMPARE(expectedServers.size(), 9);

        StubNetworkAccessManager stub;
        stub.addResponse({validWellKnownUrl, configuration});
        removeLocalConfigurationFile();
        DnsStubHelper dnsStubData({}, {});

        const auto job = new KAccountAutoconfigJob(&stub, this);
        job->setEmail(validEmail);
        QSignalSpy spyJob(job, &KAccountAutoconfigJob::result);

        job->start();
        spyJob.wait();

        QCOMPARE(job->error(), 0);

        const auto servers = job->servers();
        QCOMPARE(servers, expectedServers);

        // check that all servers are valid
        QVERIFY(std::ranges::all_of(servers, [](const auto &server) {
            return server.isValid();
        }));
    }

    void testAllProvidersFail()
    {
        StubNetworkAccessManager stub;
        stub.addResponses({{validWellKnownUrl, validConfiguration, 404}, {validIspdbUrl, validConfiguration, 404}});
        removeLocalConfigurationFile();
        DnsStubHelper dnsStubData({}, {});

        const auto job = new KAccountAutoconfigJob(&stub, this);
        job->setEmail(validEmail);
        QSignalSpy spyJob(job, &KAccountAutoconfigJob::result);

        job->start();
        spyJob.wait();

        QCOMPARE(job->error(), KAccountAutoconfigJob::Error::ConfigurationNotFound);
        QCOMPARE(job->servers().size(), 0);
    }
};

QTEST_MAIN(KAccountAutoconfigTest)

#include "kaccountautoconfigtest.moc"
