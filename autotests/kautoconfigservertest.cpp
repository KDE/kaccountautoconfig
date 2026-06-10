/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "lib/kautoconfigserver.h"

#include <QTest>

class KAutoconfigServerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    static void createEmpty()
    {
        KAutoconfigServer server(KAutoconfigServer::Protocol::IMAP);
        // default value
        QCOMPARE(server.isValid(), false);
        QCOMPARE(server.status(), KAutoconfigServer::Status::None);
        QCOMPARE(server.authentificationType(), KAutoconfigServer::UnknownAuthentification);
        QCOMPARE(server.url(), QUrl());
        QCOMPARE(server.protocol(), KAutoconfigServer::IMAP);
    }

    static void testIsValid_data()
    {
        QTest::addColumn<QUrl>("url");
        QTest::addColumn<KAutoconfigServer::Protocol>("protocol");
        QTest::addColumn<KAutoconfigServer::AuthentificationType>("auth");
        QTest::addColumn<KAutoconfigServer::Status>("status");
        QTest::addColumn<bool>("expectedValid");

        const auto validUrl = QUrl(QStringLiteral("imap://imap.example.com:993"));
        constexpr auto validProtocol = KAutoconfigServer::Protocol::IMAP;
        constexpr auto validAuth = KAutoconfigServer::AuthentificationType::Basic;
        constexpr auto validStatus = KAutoconfigServer::Status::LoginSuccess;

        QTest::newRow("all_valid") << validUrl << validProtocol << validAuth << validStatus << true;
        QTest::newRow("missing_url") << QUrl() << validProtocol << validAuth << validStatus << false;
        QTest::newRow("missing_authtype") << validUrl << validProtocol << KAutoconfigServer::AuthentificationType::UnknownAuthentification << validStatus
                                          << false;
        QTest::newRow("all_present_but_bad_status") << validUrl << validProtocol << validAuth << KAutoconfigServer::Status::BadCredentials << false;
    }

    static void testIsValid()
    {
        QFETCH(QUrl, url);
        QFETCH(KAutoconfigServer::Protocol, protocol);
        QFETCH(KAutoconfigServer::AuthentificationType, auth);
        QFETCH(KAutoconfigServer::Status, status);
        QFETCH(bool, expectedValid);

        KAutoconfigServer server(KAutoconfigServer::Protocol::IMAP);

        server.setUrl(url);
        server.setAuthentificationType(auth);
        server.setStatus(status);

        QCOMPARE(server.isValid(), expectedValid);
    }
};

QTEST_MAIN(KAutoconfigServerTest)

#include "kautoconfigservertest.moc"