/*
 *  SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "lib/autoconfigparser_p.h"
#include <QTest>

class AutoConfigParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void test_all()
    {
        QFile file(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QLatin1StringView("example.xml")));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto res = doParse(&file);
        QVERIFY(res);
        const auto &config = *res;

        QCOMPARE(config.emailProvider.id, QLatin1StringView("example.com"));
        QCOMPARE(config.emailProvider.domains[0], QLatin1StringView("example.com"));
        QCOMPARE(config.emailProvider.domains[1], QLatin1StringView("example.net"));
        QCOMPARE(config.emailProvider.displayName, QLatin1StringView("Google Workspace"));
        QCOMPARE(config.emailProvider.displayShortName, QLatin1StringView("GMail"));

        QCOMPARE(config.serversByProtocol[QLatin1StringView("imap")][0].hostname.host(), QLatin1StringView("imap.example.com"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("imap")][0].port, 993);
        QCOMPARE(config.serversByProtocol[QLatin1StringView("imap")][0].socketType, QLatin1StringView("SSL"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("imap")][0].authentications[0].method, QLatin1StringView("password-cleartext"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("imap")][0].username, QLatin1StringView("%EMAILADDRESS%"));

        QCOMPARE(config.serversByProtocol[QLatin1StringView("pop3")][0].hostname.host(), QLatin1StringView("pop.example.com"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("pop3")][0].port, 995);
        QCOMPARE(config.serversByProtocol[QLatin1StringView("pop3")][0].socketType, QLatin1StringView("SSL"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("pop3")][0].authentications[0].method, QLatin1StringView("password-cleartext"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("pop3")][0].username, QLatin1StringView("%EMAILADDRESS%"));

        QCOMPARE(config.serversByProtocol[QLatin1StringView("smtp")][0].hostname.host(), QLatin1StringView("smtp.googlemail.com"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("smtp")][0].port, 587);
        QCOMPARE(config.serversByProtocol[QLatin1StringView("smtp")][0].socketType, QLatin1StringView("STARTTLS"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("smtp")][0].authentications[0].method, QLatin1StringView("password-cleartext"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("smtp")][0].username, QLatin1StringView("%EMAILADDRESS%"));

        QCOMPARE(config.serversByProtocol[QLatin1StringView("jmap")][0].url.toDisplayString(), QLatin1StringView("https://jmap.example.com"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("jmap")][0].authentications[0].method, QLatin1StringView("OAuth2"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("jmap")][0].authentications[1].method, QLatin1StringView("basic"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("jmap")][0].username, QLatin1StringView("%EMAILADDRESS%"));

        QCOMPARE(config.serversByProtocol[QLatin1StringView("carddav")][0].url.toDisplayString(),
                 QLatin1StringView("https://contacts.example.com/remote.php/dav"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("carddav")][0].authentications[0].method, QLatin1StringView("basic"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("carddav")][0].username, QLatin1StringView("%EMAILADDRESS%"));

        QCOMPARE(config.serversByProtocol[QLatin1StringView("caldav")][0].url.toDisplayString(),
                 QLatin1StringView("https://calendar.example.com/remote.php/dav"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("caldav")][0].authentications[0].method, QLatin1StringView("basic"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("caldav")][0].username, QLatin1StringView("%EMAILADDRESS%"));
    }

    void test_multiple_authentications()
    {
        QFile file(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QLatin1StringView("gmail.xml")));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto res = doParse(&file);
        QVERIFY(res);
        const auto &config = *res;

        QCOMPARE(config.emailProvider.id, QLatin1StringView("googlemail.com"));
        QCOMPARE(config.emailProvider.domains[0], QLatin1StringView("gmail.com"));
        QCOMPARE(config.emailProvider.domains[1], QLatin1StringView("googlemail.com"));
        QCOMPARE(config.emailProvider.domains[2], QLatin1StringView("google.com"));
        QCOMPARE(config.emailProvider.domains[3], QLatin1StringView("jazztel.es"));
        QCOMPARE(config.emailProvider.displayName, QLatin1StringView("Google Mail"));
        QCOMPARE(config.emailProvider.displayShortName, QLatin1StringView("GMail"));

        QCOMPARE(config.serversByProtocol[QLatin1StringView("imap")][0].authentications[0].method, QLatin1StringView("OAuth2"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("imap")][0].authentications[1].method, QLatin1StringView("password-cleartext"));

        QCOMPARE(config.serversByProtocol[QLatin1StringView("pop3")][0].authentications[0].method, QLatin1StringView("OAuth2"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("pop3")][0].authentications[1].method, QLatin1StringView("password-cleartext"));

        QCOMPARE(config.serversByProtocol[QLatin1StringView("smtp")][0].authentications[0].method, QLatin1StringView("OAuth2"));
        QCOMPARE(config.serversByProtocol[QLatin1StringView("smtp")][0].authentications[1].method, QLatin1StringView("password-cleartext"));
    }

    void test_failures_data()
    {
        QTest::addColumn<QString>("xmlName");
        QTest::newRow("invalid.xml") << QStringLiteral("invalid.xml");
        QTest::newRow("offtopic.xml") << QStringLiteral("offtopic.xml");

        QTest::newRow("missing_authentication.xml") << QStringLiteral("missing_authentication.xml");
        QTest::newRow("missing_hostname.xml") << QStringLiteral("missing_hostname.xml");
        QTest::newRow("missing_port.xml") << QStringLiteral("missing_port.xml");
        QTest::newRow("missing_sockettype.xml") << QStringLiteral("missing_sockettype.xml");
        QTest::newRow("missing_type.xml") << QStringLiteral("missing_type.xml");
        QTest::newRow("missing_url.xml") << QStringLiteral("missing_url.xml");
        QTest::newRow("missing_username.xml") << QStringLiteral("missing_username.xml");

        QTest::newRow("duplicate_hostname.xml") << QStringLiteral("duplicate_hostname.xml");
        QTest::newRow("duplicate_port.xml") << QStringLiteral("duplicate_port.xml");
        QTest::newRow("duplicate_sockettype.xml") << QStringLiteral("duplicate_sockettype.xml");
        QTest::newRow("duplicate_url.xml") << QStringLiteral("duplicate_url.xml");
        QTest::newRow("duplicate_username.xml") << QStringLiteral("duplicate_username.xml");

        QTest::newRow("invalid_port.xml") << QStringLiteral("invalid_port.xml");
        QTest::newRow("invalid_hostname.xml") << QStringLiteral("invalid_hostname.xml");
        QTest::newRow("invalid_url.xml") << QStringLiteral("invalid_url.xml");
    }

    void test_failures()
    {
        QFETCH(QString, xmlName);
        QFile file(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(xmlName));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto res = doParse(&file);
        QVERIFY(!res);
    }
};

QTEST_MAIN(AutoConfigParserTest);

#include "autoconfigparsertest.moc"
