/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
 */

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
    const QString validEmail = QStringLiteral("test@kde.org");
    QByteArray validConfiguration;
    QByteArray validEmailProviderConfiguration;
    QByteArray validIspdbConfiguration;

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
        QTest::addColumn<QList<KAutoconfigServer>>("expectedServers");

        KAutoconfigServer emailProviderServer(KAutoconfigServer::IMAP);
        emailProviderServer.setUrl(QUrl("imap://email.provider.com:993"_L1));
        emailProviderServer.setAuthenticationType(KAutoconfigServer::PasswordCleartext);
        emailProviderServer.setSocketType(KAutoconfigServer::SSL);

        KAutoconfigServer ispdbProviderServer(KAutoconfigServer::IMAP);
        ispdbProviderServer.setUrl(QUrl("imap://ispdb.provider.com:993"_L1));
        ispdbProviderServer.setAuthenticationType(KAutoconfigServer::PasswordCleartext);
        ispdbProviderServer.setSocketType(KAutoconfigServer::SSL);

        KAutoconfigServer localProviderServer(KAutoconfigServer::IMAP);
        localProviderServer.setUrl(QUrl("imap://local.provider.com:993"_L1));
        localProviderServer.setAuthenticationType(KAutoconfigServer::PasswordCleartext);
        localProviderServer.setSocketType(KAutoconfigServer::SSL);

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
        ;
    }

    void testAccountAutoconfigProvider()
    {
        QFETCH(bool, searchLocalFile);
        QFETCH(Responses, responses);
        QFETCH(QList<KAutoconfigServer>, expectedServers);

        StubNetworkAccessManager stub;
        stub.addResponses(responses);

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
        QTest::addColumn<QList<KAutoconfigServer>>("expectedServers");
        // configuration with multiple imap,pop,stmp and multiple auth
        QFile validConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("multiple_protocol_and_authentication.xml")));
        QVERIFY(validConfigurationFile.open(QIODevice::ReadOnly));
        const QByteArray configuration = validConfigurationFile.readAll();

        // three pop servers, first on with two auth in the xml
        KAutoconfigServer firstPopServer(KAutoconfigServer::POP3);
        firstPopServer.setUrl(QUrl("pop://pop.example.com:995"_L1));
        firstPopServer.setAuthenticationType(KAutoconfigServer::PasswordCleartext);
        firstPopServer.setSocketType(KAutoconfigServer::SSL);

        KAutoconfigServer secondPopServer(KAutoconfigServer::POP3);
        secondPopServer.setUrl(QUrl("pop://pop.example.com:995"_L1));
        secondPopServer.setAuthenticationType(KAutoconfigServer::PasswordEncrypted);
        secondPopServer.setSocketType(KAutoconfigServer::SSL);

        KAutoconfigServer thirdPopServer(KAutoconfigServer::POP3);
        thirdPopServer.setUrl(QUrl("pop://pop.example.com:110"_L1));
        thirdPopServer.setAuthenticationType(KAutoconfigServer::PasswordCleartext);
        thirdPopServer.setSocketType(KAutoconfigServer::STARTTLS);

        // three imap servers, second with two auth in the xml
        KAutoconfigServer firstImapServer(KAutoconfigServer::IMAP);
        firstImapServer.setUrl(QUrl("imap://imap.example.com:993"_L1));
        firstImapServer.setAuthenticationType(KAutoconfigServer::PasswordCleartext);
        firstImapServer.setSocketType(KAutoconfigServer::SSL);

        KAutoconfigServer secondImapServer(KAutoconfigServer::IMAP);
        secondImapServer.setUrl(QUrl("imap://imap.example.com:993"_L1));
        secondImapServer.setAuthenticationType(KAutoconfigServer::PasswordEncrypted);
        secondImapServer.setSocketType(KAutoconfigServer::SSL);

        KAutoconfigServer thirdImapServer(KAutoconfigServer::IMAP);
        thirdImapServer.setUrl(QUrl("imap://imap.example.com:143"_L1));
        thirdImapServer.setAuthenticationType(KAutoconfigServer::PasswordCleartext);
        thirdImapServer.setSocketType(KAutoconfigServer::STARTTLS);

        // three smtp servers, second with two auth in the xml
        KAutoconfigServer firstSmtpServer(KAutoconfigServer::SMTP);
        firstSmtpServer.setUrl(QUrl("smtp://smtp.googlemail.com:465"_L1));
        firstSmtpServer.setAuthenticationType(KAutoconfigServer::PasswordCleartext);
        firstSmtpServer.setSocketType(KAutoconfigServer::SSL);

        KAutoconfigServer secondSmtpServer(KAutoconfigServer::SMTP);
        secondSmtpServer.setUrl(QUrl("smtp://smtp.googlemail.com:587"_L1));
        secondSmtpServer.setAuthenticationType(KAutoconfigServer::PasswordCleartext);
        secondSmtpServer.setSocketType(KAutoconfigServer::STARTTLS);

        KAutoconfigServer thirdSmtpServer(KAutoconfigServer::SMTP);
        thirdSmtpServer.setUrl(QUrl("smtp://smtp.googlemail.com:587"_L1));
        thirdSmtpServer.setAuthenticationType(KAutoconfigServer::PasswordEncrypted);
        thirdSmtpServer.setSocketType(KAutoconfigServer::STARTTLS);

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
        QFETCH(QList<KAutoconfigServer>, expectedServers);
        QFETCH(QByteArray, configuration);
        QCOMPARE(expectedServers.size(), 9);

        StubNetworkAccessManager stub;
        stub.addResponse({validWellKnownUrl, configuration});
        removeLocalConfigurationFile();

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
};

QTEST_MAIN(KAccountAutoconfigTest)

#include "kaccountautoconfigtest.moc"
