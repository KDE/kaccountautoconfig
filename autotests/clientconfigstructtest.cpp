/*
 *  SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "autoconfigparser_p.h"
#include <QTest>

using namespace Qt::StringLiterals;
using namespace AutoConfigParser;

class ClientConfigStructTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    // clang adds big indentations that make the code less readable when using this struct syntax
    // clang-format off

    void test_documentation_equal_operator()
    {
        const auto controlDocumentation = Documentation{
            .url = QUrl("https://example.com/doc"_L1),
            .descriptionByLanguage = {
                {"en"_L1, "Configuration"_L1},
                {"fr"_L1, "Configuration"_L1},
            }
        };
        QCOMPARE(
            (Documentation{
                .url = QUrl("https://example.com/doc"_L1),
                .descriptionByLanguage =
                    {
                        {"en"_L1, "Configuration"_L1},
                        {"fr"_L1, "Configuration"_L1},
                    }
            }),
            controlDocumentation
        );

        // Different URL
        QCOMPARE_NE(
            (Documentation{
                .url = QUrl("https://example.com/docs"_L1),
                .descriptionByLanguage =
                    {
                        {"en"_L1, "Configuration"_L1},
                        {"fr"_L1, "Configuration"_L1},
                    }
            }),
            controlDocumentation
        );

        // Different language
        QCOMPARE_NE(
            (Documentation{
                .url = QUrl("https://example.com/doc"_L1),
                .descriptionByLanguage =
                    {
                        {"es"_L1, "Configuration"_L1},
                        {"fr"_L1, "Configuration"_L1},
                    }
            }),
            controlDocumentation
        );

        // Different description
        QCOMPARE_NE(
            (Documentation{
                .url = QUrl("https://example.com/doc"_L1),
                .descriptionByLanguage =
                    {
                        {"en"_L1, "AutoConfig"_L1},
                        {"fr"_L1, "Configuration"_L1},
                    }
            }),
            controlDocumentation
        );

        // Bigger map
        QCOMPARE_NE(
            (Documentation{
                .url = QUrl("https://example.com/doc"_L1),
                .descriptionByLanguage =
                    {
                        {"en"_L1, "Configuration"_L1},
                        {"fr"_L1, "Configuration"_L1},
                        {"es"_L1, "Configuration"_L1},
                    }
            }),
            controlDocumentation
        );
    }

    void test_authentication_equal_operator()
    {
        const auto controlAuthentication = Authentication{.system = "http"_L1, .method = "Basic"_L1};

        QCOMPARE((Authentication{.system = "http"_L1, .method = "Basic"_L1}), controlAuthentication);
        QCOMPARE_NE((Authentication{.method = "Basic"_L1}), controlAuthentication);
        QCOMPARE_NE((Authentication{.system = "sasl"_L1, .method = "Basic"_L1}), controlAuthentication);
        QCOMPARE_NE((Authentication{.system = "http"_L1, .method = "OAuth2"_L1}), controlAuthentication);
    }

    void test_server_equal_operator()
    {
        const auto controlServer = Server{
            .type = "imap"_L1,
            .authentications = {{.method = "password-cleartext"_L1}},
            .username = "%EMAILADDRESS%"_L1,
            .hostname = "imap.example.com"_L1,
            .port = 993,
            .socketType = "SSL"_L1,
        };

        QCOMPARE(
            (Server{
                .type = "imap"_L1,
                .authentications = {{.method = "password-cleartext"_L1}},
                .username = "%EMAILADDRESS%"_L1,
                .hostname = "imap.example.com"_L1,
                .port = 993,
                .socketType = "SSL"_L1,
            }),
            controlServer
        );

        // Type
        QCOMPARE_NE(
            (Server{
                .type = "unk"_L1,
                .authentications = {{.method = "password-cleartext"_L1}},
                .username = "%EMAILADDRESS%"_L1,
                .hostname = "imap.example.com"_L1,
                .port = 993,
                .socketType = "SSL"_L1,
            }),
            controlServer
        );

        // Authent
        QCOMPARE_NE(
            (Server{
                .type = "imap"_L1,
                .authentications = {},
                .username = "%EMAILADDRESS%"_L1,
                .hostname = "imap.example.com"_L1,
                .port = 993,
                .socketType = "SSL"_L1,
            }),
            controlServer
        );

        // Username
        QCOMPARE_NE(
            (Server{
                .type = "imap"_L1,
                .authentications = {{.method = "password-cleartext"_L1}},
                .username = "Konqi"_L1,
                .hostname = "imap.example.com"_L1,
                .port = 993,
                .socketType = "SSL"_L1,
            }),
            controlServer
        );

        // Hostname
        QCOMPARE_NE(
            (Server{
                .type = "imap"_L1,
                .authentications = {{.method = "password-cleartext"_L1}},
                .username = "%EMAILADDRESS%"_L1,
                .hostname = "example.com"_L1,
                .port = 993,
                .socketType = "SSL"_L1,
            }),
            controlServer
        );

        // Port
        QCOMPARE_NE(
            (Server{
                .type = "imap"_L1,
                .authentications = {{.method = "password-cleartext"_L1}},
                .username = "%EMAILADDRESS%"_L1,
                .hostname = "imap.example.com"_L1,
                .port = 995,
                .socketType = "SSL"_L1,
            }),
            controlServer
        );

        // SocketType
        QCOMPARE_NE(
            (Server{
                .type = "imap"_L1,
                .authentications = {{.method = "password-cleartext"_L1}},
                .username = "%EMAILADDRESS%"_L1,
                .hostname = "imap.example.com"_L1,
                .port = 993,
                .socketType = "STARTTLS"_L1,
            }),
            controlServer
        );
    }

    void test_emailProvider_equal_operator()
    {
        const auto controlEmailProvider = EmailProvider{
            .id = "example.com"_L1,
            .domains =
                {
                    {.path = "example.com"_L1},
                    {.path = "example.net"_L1},
                },
            .displayName = "Google Workspace"_L1,
            .displayShortName = "GMail"_L1,
            .documentations = {
                {.url = QUrl("https://www.example.com/help/mail/"_L1),
                 .descriptionByLanguage = {{"en"_L1, "Configure mail app for IMAP"_L1}, {"de"_L1, "Email mit IMAP konfigurieren"_L1}}}
            }
        };

        QCOMPARE(
            (EmailProvider{
                .id = "example.com"_L1,
                .domains =
                    {
                        {.path = "example.com"_L1},
                        {.path = "example.net"_L1},
                    },
                .displayName = "Google Workspace"_L1,
                .displayShortName = "GMail"_L1,
                .documentations =
                    {{.url = QUrl("https://www.example.com/help/mail/"_L1),
                      .descriptionByLanguage = {{"en"_L1, "Configure mail app for IMAP"_L1}, {"de"_L1, "Email mit IMAP konfigurieren"_L1}}}}
            }),
            controlEmailProvider
        );

        // id
        QCOMPARE_NE(
            (EmailProvider{
                .id = "example.net"_L1,
                .domains =
                    {
                        {.path = "example.com"_L1},
                        {.path = "example.net"_L1},
                    },
                .displayName = "Google Workspace"_L1,
                .displayShortName = "GMail"_L1,
                .documentations =
                    {{.url = QUrl("https://www.example.com/help/mail/"_L1),
                      .descriptionByLanguage = {{"en"_L1, "Configure mail app for IMAP"_L1}, {"de"_L1, "Email mit IMAP konfigurieren"_L1}}}}
            }),
            controlEmailProvider
        );

        // domains
        QCOMPARE_NE(
            (EmailProvider{
                .id = "example.com"_L1,
                .domains =
                    {
                        {.path = "example.com"_L1},
                        {.path = "example.net"_L1},
                        {.path = "example.org"_L1},
                    },
                .displayName = "Google Workspace"_L1,
                .displayShortName = "GMail"_L1,
                .documentations =
                    {{.url = QUrl("https://www.example.com/help/mail/"_L1),
                      .descriptionByLanguage = {{"en"_L1, "Configure mail app for IMAP"_L1}, {"de"_L1, "Email mit IMAP konfigurieren"_L1}}}}
            }),
            controlEmailProvider
        );

        // displayName
        QCOMPARE_NE(
            (EmailProvider{
                .id = "example.com"_L1,
                .domains =
                    {
                        {.path = "example.com"_L1},
                        {.path = "example.net"_L1},
                    },
                .displayName = "Google Work space"_L1,
                .displayShortName = "GMail"_L1,
                .documentations =
                    {{.url = QUrl("https://www.example.com/help/mail/"_L1),
                      .descriptionByLanguage = {{"en"_L1, "Configure mail app for IMAP"_L1}, {"de"_L1, "Email mit IMAP konfigurieren"_L1}}}}
            }),
            controlEmailProvider
        );

        // displayShortName
        QCOMPARE_NE(
            (EmailProvider{
                .id = "example.com"_L1,
                .domains =
                    {
                        {.path = "example.com"_L1},
                        {.path = "example.net"_L1},
                    },
                .displayName = "Google Workspace"_L1,
                .displayShortName = "eMail"_L1,
                .documentations =
                    {{.url = QUrl("https://www.example.com/help/mail/"_L1),
                      .descriptionByLanguage = {{"en"_L1, "Configure mail app for IMAP"_L1}, {"de"_L1, "Email mit IMAP konfigurieren"_L1}}}}
            }),
            controlEmailProvider
        );

        // documentations
        QCOMPARE_NE(
            (EmailProvider{
                .id = "example.com"_L1,
                .domains =
                    {
                        {.path = "example.com"_L1},
                        {.path = "example.net"_L1},
                    },
                .displayName = "Google Workspace"_L1,
                .displayShortName = "GMail"_L1,
                .documentations = {}
            }),
            controlEmailProvider
        );
    }

    void test_clientConfig_equal_operator()
    {
        const auto controlClientConfig = ClientConfig{
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
            .servers = {
                 {
                     .type = "imap"_L1,
                     .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                     .username = "%EMAILADDRESS%"_L1,
                     .hostname = "imap.gmail.com"_L1,
                     .port = 993,
                     .socketType = "SSL"_L1,
                 },
                 {
                     .type = "smtp"_L1,
                     .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                     .username = "%EMAILADDRESS%"_L1,
                     .hostname = "smtp.gmail.com"_L1,
                     .port = 465,
                     .socketType = "SSL"_L1,
                 },
            }
        };

        QCOMPARE(
            (ClientConfig{
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
                .servers =
                    {
                         {
                             .type = "imap"_L1,
                             .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                             .username = "%EMAILADDRESS%"_L1,
                             .hostname = "imap.gmail.com"_L1,
                             .port = 993,
                             .socketType = "SSL"_L1,
                         },
                         {
                             .type = "smtp"_L1,
                             .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                             .username = "%EMAILADDRESS%"_L1,
                             .hostname = "smtp.gmail.com"_L1,
                             .port = 465,
                             .socketType = "SSL"_L1,
                         },
                    }
            }),
            controlClientConfig
        );

        // EmailProvider
        QCOMPARE_NE(
            (ClientConfig{
                .emailProvider =
                    {
                        .id = "gmail.com"_L1,
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
                .servers =
                    {
                         {
                             .type = "imap"_L1,
                             .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                             .username = "%EMAILADDRESS%"_L1,
                             .hostname = "imap.gmail.com"_L1,
                             .port = 993,
                             .socketType = "SSL"_L1,
                         },
                         {
                             .type = "smtp"_L1,
                             .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                             .username = "%EMAILADDRESS%"_L1,
                             .hostname = "smtp.gmail.com"_L1,
                             .port = 465,
                             .socketType = "SSL"_L1,
                         },
                    }
            }),
            controlClientConfig
        );

        // servers different value
        QCOMPARE_NE(
            (ClientConfig{
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
                .servers =
                    {
                         {
                             .type = "pop3"_L1,
                             .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                             .username = "%EMAILADDRESS%"_L1,
                             .hostname = "imap.gmail.com"_L1,
                             .port = 993,
                             .socketType = "SSL"_L1,
                         },
                         {
                             .type = "smtp"_L1,
                             .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                             .username = "%EMAILADDRESS%"_L1,
                             .hostname = "smtp.gmail.com"_L1,
                             .port = 465,
                             .socketType = "SSL"_L1,
                         },
                    }
            }),
            controlClientConfig
        );

        // servers bigger map
        QCOMPARE_NE(
            (ClientConfig{
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
                .servers =
                    {
                        {},
                         {
                             .type = "imap"_L1,
                             .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                             .username = "%EMAILADDRESS%"_L1,
                             .hostname = "imap.gmail.com"_L1,
                             .port = 993,
                             .socketType = "SSL"_L1,
                         },
                         {
                             .type = "smtp"_L1,
                             .authentications = {{.method = "OAuth2"_L1}, {.method = "password-cleartext"_L1}},
                             .username = "%EMAILADDRESS%"_L1,
                             .hostname = "smtp.gmail.com"_L1,
                             .port = 465,
                             .socketType = "SSL"_L1,
                         },
                    }
            }),
            controlClientConfig
        );
    }

    // clang-format off
};

QTEST_MAIN(ClientConfigStructTest)

#include "clientconfigstructtest.moc"
