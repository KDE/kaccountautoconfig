/*
 *  SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "lib/autoconfigparser_p.h"
#include <QTest>

using namespace Qt::StringLiterals;
using namespace AutoConfigParser;

class AutoConfigParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void test_successes_data()
    {
        QTest::addColumn<QString>("xmlName");
        QTest::addColumn<ClientConfig>("expectedConfig");

        // clang-format off

        QTest::newRow("example.xml") << QStringLiteral("example.xml") << ClientConfig{
            .version = "1.2"_L1,
            .emailProvider =
                {.id = "example.com"_L1,
                 .domains =
                     {
                         {.path = "example.com"_L1},
                         {.path = "example.net"_L1},
                     },
                 .displayName = "Google Workspace"_L1,
                 .displayShortName = "GMail"_L1,
                 .documentations =
                     {{.url = QUrl("https://www.example.com/help/mail/"_L1),
                       .descriptionByLanguage = {{"en"_L1, "Configure mail app for IMAP"_L1}, {"de"_L1, "Email mit IMAP konfigurieren"_L1}}}}},
            .serversByProtocol =
                {
                    {"imap"_L1,
                     QList{Server{
                         .type = "imap"_L1,
                         .authentications = {{.method = "password-cleartext"_L1}},
                         .username = "%EMAILADDRESS%"_L1,
                         .hostname = "imap.example.com"_L1,
                         .port = 993,
                         .socketType = "SSL"_L1,
                     }}},
                    {"pop3"_L1,
                     QList{Server{
                         .type = "pop3"_L1,
                         .authentications = {{.method = "password-cleartext"_L1}},
                         .username = "%EMAILADDRESS%"_L1,
                         .hostname = "pop.example.com"_L1,
                         .port = 995,
                         .socketType = "SSL"_L1,
                     }}},
                    {"smtp"_L1,
                     QList{Server{
                         .type = "smtp"_L1,
                         .authentications = {{.method = "password-cleartext"_L1}},
                         .username = "%EMAILADDRESS%"_L1,
                         .hostname = "smtp.googlemail.com"_L1,
                         .port = 587,
                         .socketType = "STARTTLS"_L1,
                     }}},
                    {"jmap"_L1,
                     QList{Server{
                         .type = "jmap"_L1,
                         .url = QUrl("https://jmap.example.com"_L1),
                         .authentications = {{.method = "OAuth2"_L1}},
                         .username = "%EMAILADDRESS%"_L1,
                     }}},
                    {"ews"_L1,
                     QList{Server{
                         .type = "ews"_L1,
                         .url = QUrl("https://mail.example.com/EWS/Exchange.asmx"_L1),
                         .authentications =
                             {
                                 {.system = "http"_L1, .method = "Basic"_L1},
                             },
                         .username = "%EMAILADDRESS%"_L1,
                     }}},
                    {"activesync"_L1,
                     QList{Server{
                         .type = "activesync"_L1,
                         .url = QUrl("https://mail.example.com/Microsoft-Server-ActiveSync"_L1),
                         .authentications = {{.method = "OAuth2"_L1}},
                         .username = "%EMAILADDRESS%"_L1,
                     }}},
                    {"carddav"_L1,
                     QList{Server{
                         .type = "carddav"_L1,
                         .url = QUrl("https://contacts.example.com/remote.php/dav"_L1),
                         .authentications =
                             {
                                 {.system = "http"_L1, .method = "Basic"_L1},
                             },
                         .username = "%EMAILADDRESS%"_L1,
                     }}},
                    {"caldav"_L1,
                     QList{Server{
                         .type = "caldav"_L1,
                         .url = QUrl("https://calendar.example.com/remote.php/dav"_L1),
                         .authentications =
                             {
                                 {.system = "http"_L1, .method = "Basic"_L1},
                             },
                         .username = "%EMAILADDRESS%"_L1,
                     }}},
                    {QString{"webdav"_L1},
                     QList{Server{
                         .type = "webdav"_L1,
                         .url = QUrl("https://share.example.com/remote.php/dav"_L1),
                         .authentications =
                             {
                                 {.system = "http"_L1, .method = "Basic"_L1},
                             },
                         .username = "%EMAILADDRESS%"_L1,
                     }}},
                    {"xmpp"_L1,
                     QList{
                         Server{
                             .type = "xmpp"_L1,
                             .url = QUrl("wss://example.com:5281/xmpp-websocket"_L1),
                             .authentications =
                                 {
                                     {.system = "http"_L1, .method = "Basic"_L1},
                                 },
                             .username = "%EMAILADDRESS%"_L1,
                         },
                         Server{
                             .type = "xmpp"_L1,
                             .authentications = {{.method = "password-cleartext"_L1}},
                             .username = "%EMAILADDRESS%"_L1,
                             .hostname = "xmpp.example.com"_L1,
                             .port = 5223,
                             .socketType = "TLS"_L1,
                         }
                     }},
                    {"opentalk"_L1,
                     QList{Server{
                         .type = "opentalk"_L1,
                         .url = QUrl("https://talk.example.com/login"_L1),
                         .authentications =
                             {
                                 {.method = "OAuth2"_L1},
                             },
                         .username = "%EMAILADDRESS%"_L1,
                     }}},
                },
            .oAuth2 =
                {
                    .authURL = QUrl(QLatin1StringView("https://login.example.com/auth")),
                    .tokenURL = QUrl(QLatin1StringView("https://login.example.com/token")),
                    .issuer = QLatin1StringView("login.example.com"),
                    .scope = QLatin1StringView("IMAP POP3 SMTP CalDAV CardDAV WebDAV offline_access"),
                    .clientID = QLatin1StringView("open"),
                    .clientSecret = QLatin1StringView("give-me-your-password"),
                },
            .clientConfigUpdate = QUrl(QLatin1StringView("https://www.example.com/config/mail.xml"))
        };

        QTest::newRow("multiple_authentication_methods.xml")
            << QStringLiteral("multiple_authentication_methods.xml")
            << ClientConfig{
                .version = "1.1"_L1,
                .emailProvider =
                    {
                        .id = "googlemail.com"_L1,
                        .domains =
                            {
                                {.path = "gmail.com"_L1},
                                {.path = "googlemail.com"_L1},
                                {.path = "google.com"_L1},
                                {.path = "jazztel.es"_L1},
                            },
                        .displayName = "Google Mail"_L1,
                        .displayShortName = "GMail"_L1,
                    },
                .serversByProtocol = {
                    {"imap"_L1,
                     QList{Server{
                         .type = "imap"_L1,
                         .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                         .username = "%EMAILADDRESS%"_L1,
                         .hostname = "imap.gmail.com"_L1,
                         .port = 993,
                         .socketType = "SSL"_L1,
                     }}},
                    {"pop3"_L1,
                     QList{Server{
                         .type = "pop3"_L1,
                         .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                         .username = "%EMAILADDRESS%"_L1,
                         .hostname = "pop.gmail.com"_L1,
                         .port = 995,
                         .socketType = "SSL"_L1,
                     }}},
                    {"smtp"_L1,
                     QList{Server{
                         .type = "smtp"_L1,
                         .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                         .username = "%EMAILADDRESS%"_L1,
                         .hostname = "smtp.gmail.com"_L1,
                         .port = 465,
                         .socketType = "SSL"_L1,
                     }}},
                }
            };

        QTest::newRow("special_urls.xml")<< QStringLiteral("special_urls.xml") << ClientConfig{
            .version="1.2"_L1,
            .serversByProtocol = {
                {"carddav"_L1,
                    QList{Server{
                        .type = "carddav"_L1,
                        .url = QUrl("https://contacts.example.com/remote.php/Distributed Authoring and Versioning!"_L1),
                        .authentications = {{.method = "basic"_L1}},
                        .username = "%EMAILADDRESS%"_L1
                }}},
                {"caldav"_L1,
                    QList{Server{
                        .type = "caldav"_L1,
                        .url = QUrl("https://calendar.example.com/remote.php/Distributed%20Authoring%20and%20Versioning%21"_L1),
                        .authentications = {{.method = "basic"_L1}},
                        .username = "%EMAILADDRESS%"_L1
                }}},
            }
        };

        // clang-format on
    }

    void test_successes()
    {
        QFETCH(QString, xmlName);
        QFETCH(ClientConfig, expectedConfig);
        QFile file(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(xmlName));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto actual = parse(&file);
        QVERIFY(actual);
        QVERIFY(*actual == expectedConfig);
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
        const auto res = parse(&file);
        QVERIFY(!res);
    }
};

QTEST_MAIN(AutoConfigParserTest);

#include "autoconfigparsertest.moc"
