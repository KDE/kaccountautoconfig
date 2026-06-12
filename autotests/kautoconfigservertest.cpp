/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QTest>

#include <kaccountautoconfig/kautoconfigserver.h>

class KAutoconfigServerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void createEmpty()
    {
        KAutoconfigServer server(KAutoconfigServer::IMAP);
        // default value
        QCOMPARE(server.isValid(), false);
        QCOMPARE(server.status(), KAutoconfigServer::None);
        QCOMPARE(server.authenticationType(), KAutoconfigServer::UnknownAuthentication);
        QVERIFY(server.url().isEmpty());
        QCOMPARE(server.protocol(), KAutoconfigServer::IMAP);
    }

    void testIsValid_data()
    {
        QTest::addColumn<QUrl>("url");
        QTest::addColumn<KAutoconfigServer::Protocol>("protocol");
        QTest::addColumn<KAutoconfigServer::AuthenticationType>("auth");
        QTest::addColumn<KAutoconfigServer::Status>("status");
        QTest::addColumn<bool>("expectedValid");

        const auto validUrl = QUrl(QStringLiteral("imap://imap.example.com:993"));
        constexpr auto validProtocol = KAutoconfigServer::IMAP;
        constexpr auto validAuth = KAutoconfigServer::Basic;
        constexpr auto validStatus = KAutoconfigServer::LoginSuccess;

        QTest::newRow("all_valid") << validUrl << validProtocol << validAuth << validStatus << true;
        QTest::newRow("missing_url") << QUrl() << validProtocol << validAuth << validStatus << false;
        QTest::newRow("missing_authtype") << validUrl << validProtocol << KAutoconfigServer::AuthenticationType::UnknownAuthentication << validStatus << false;
        QTest::newRow("all_present_but_bad_status") << validUrl << validProtocol << validAuth << KAutoconfigServer::Status::BadCredentials << false;
    }

    void testIsValid()
    {
        QFETCH(QUrl, url);
        QFETCH(KAutoconfigServer::Protocol, protocol);
        QFETCH(KAutoconfigServer::AuthenticationType, auth);
        QFETCH(KAutoconfigServer::Status, status);
        QFETCH(bool, expectedValid);

        KAutoconfigServer server(protocol);

        server.setUrl(url);
        server.setAuthenticationType(auth);
        server.setStatus(status);

        QCOMPARE(server.isValid(), expectedValid);
    }
};

QTEST_MAIN(KAutoconfigServerTest)

#include "kautoconfigservertest.moc"
