/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QTest>

#include <kaccountautoconfigserver.h>

class KAccountAutoconfigServerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void createEmpty()
    {
        KAccountAutoconfigServer server(KAccountAutoconfigServer::IMAP);
        // default value
        QCOMPARE(server.isValid(), false);
        QCOMPARE(server.authenticationType(), KAccountAutoconfigServer::UnknownAuthentication);
        QVERIFY(server.url().isEmpty());
        QCOMPARE(server.protocol(), KAccountAutoconfigServer::IMAP);
    }

    void testIsValid_data()
    {
        QTest::addColumn<QUrl>("url");
        QTest::addColumn<KAccountAutoconfigServer::Protocol>("protocol");
        QTest::addColumn<KAccountAutoconfigServer::AuthenticationType>("auth");
        QTest::addColumn<bool>("expectedValid");

        const auto validUrl = QUrl(QStringLiteral("imap://imap.example.com:993"));
        constexpr auto validProtocol = KAccountAutoconfigServer::IMAP;
        constexpr auto validAuth = KAccountAutoconfigServer::Basic;

        QTest::newRow("all_valid") << validUrl << validProtocol << validAuth << true;
        QTest::newRow("missing_url") << QUrl() << validProtocol << validAuth << false;
        QTest::newRow("missing_authtype") << validUrl << validProtocol << KAccountAutoconfigServer::AuthenticationType::UnknownAuthentication << false;
    }

    void testIsValid()
    {
        QFETCH(QUrl, url);
        QFETCH(KAccountAutoconfigServer::Protocol, protocol);
        QFETCH(KAccountAutoconfigServer::AuthenticationType, auth);
        QFETCH(bool, expectedValid);

        KAccountAutoconfigServer server(protocol);

        server.setUrl(url);
        server.setAuthenticationType(auth);

        QCOMPARE(server.isValid(), expectedValid);
    }
};

QTEST_MAIN(KAccountAutoconfigServerTest)

#include "kaccountautoconfigservertest.moc"
