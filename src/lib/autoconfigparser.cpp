/*
    SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "autoconfigparser_p.h"
#include "kaccountautoconfig_debug.h"
#include <QXmlStreamReader>

using namespace AutoConfigParser;

constexpr auto tcpBasedProtocols = std::to_array({
    QLatin1StringView("imap"),
    QLatin1StringView("pop3"),
    QLatin1StringView("smtp"),
});

constexpr auto urlBasedProtocols = std::to_array({
    QLatin1StringView("jmap"),
    QLatin1StringView("carddav"),
    QLatin1StringView("caldav"),
});

bool isAuthenticationValid(const Authentication &authentication)
{
    return !authentication.method.isEmpty();
}

bool isServerValid(const Server &server)
{
    if (server.type.isEmpty()) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: missing type attribute in server";
        return false;
    }
    if (std::ranges::find(tcpBasedProtocols, server.type) != tcpBasedProtocols.end()) {
        QUrl hostNameUrl;
        hostNameUrl.setHost(server.hostname);
        if (!hostNameUrl.isValid() || server.port == 0 || server.socketType.isEmpty() || server.authentications.isEmpty() || server.username.isEmpty()) {
            qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: missing or invalid attributes in" << server.type << "server";
            return false;
        }
    }
    if (std::ranges::find(urlBasedProtocols, server.type) != urlBasedProtocols.end()) {
        if (!server.url.isValid() || server.url.isRelative() || server.authentications.isEmpty() || server.username.isEmpty()) {
            qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: missing or invalid attributes in" << server.type << "server";
            return false;
        }
    }
    if (!std::ranges::all_of(server.authentications, isAuthenticationValid)) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: empty authentication tag in" << server.type << "server";
        return false;
    }
    // Completely ignore unsupported protocols
    return true;
}

bool isClientConfigValid(const ClientConfig &clientConfig)
{
    // Don't check emailProvider validity, since all servers are in the clientConfig map
    return std::ranges::all_of(clientConfig.servers, isServerValid);
}

Documentation parseDocumentation(QXmlStreamReader &xml)
{
    auto documentation = Documentation();
    if (xml.attributes().hasAttribute(QLatin1StringView("url"))) {
        documentation.url = QUrl(xml.attributes().value(QLatin1StringView("url")).toString());
    }
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("descr") && xml.attributes().hasAttribute(QLatin1StringView("lang"))) {
            const auto lang = xml.attributes().value(QLatin1StringView("lang")).toString();
            documentation.descriptionByLanguage[lang] = xml.readElementText();
        } else {
            xml.skipCurrentElement();
        }
    }
    return documentation;
}

std::optional<Server> parseServer(QXmlStreamReader &xml)
{
    auto server = Server();
    if (xml.attributes().hasAttribute(QLatin1StringView("type"))) {
        server.type = xml.attributes().value(QLatin1StringView("type")).toString();
    }
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("url")) {
            if (!server.url.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate url tag in server";
                return {};
            }
            server.url = QUrl(xml.readElementText());
        } else if (xml.name() == QLatin1StringView("authentication")) {
            auto authentication = Authentication();
            if (xml.attributes().hasAttribute(QLatin1StringView("system"))) {
                authentication.system = xml.attributes().value(QLatin1StringView("system")).toString();
            }
            authentication.method = xml.readElementText();
            server.authentications.append(authentication);
        } else if (xml.name() == QLatin1StringView("username")) {
            if (!server.username.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate username tag in server";
                return {};
            }
            server.username = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("hostname")) {
            if (!server.hostname.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate hostname tag in server";
                return {};
            }
            server.hostname = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("port")) {
            if (server.port != 0) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate port tag in server";
                return {};
            }
            server.port = xml.readElementText().toInt();
        } else if (xml.name() == QLatin1StringView("socketType")) {
            if (!server.socketType.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate socketType tag in server";
                return {};
            }
            server.socketType = xml.readElementText();
        } else {
            xml.skipCurrentElement();
        }
    }
    return server;
}

std::optional<OAuth2> parseOAuth2(QXmlStreamReader &xml)
{
    auto oAuth2 = OAuth2();
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("authURL")) {
            if (!oAuth2.authURL.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate authURL tag in oAuth2";
                return std::nullopt;
            }
            oAuth2.authURL = QUrl(xml.readElementText());
        } else if (xml.name() == QLatin1StringView("tokenURL")) {
            if (!oAuth2.tokenURL.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate tokenURL tag in oAuth2";
                return std::nullopt;
            }
            oAuth2.tokenURL = QUrl(xml.readElementText());
        } else if (xml.name() == QLatin1StringView("issuer")) {
            if (!oAuth2.issuer.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate issuer tag in oAuth2";
                return std::nullopt;
            }
            oAuth2.issuer = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("scope")) {
            if (!oAuth2.scope.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate scope tag in oAuth2";
                return std::nullopt;
            }
            oAuth2.scope = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("clientID")) {
            if (!oAuth2.clientID.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate clientID tag in oAuth2";
                return std::nullopt;
            }
            oAuth2.clientID = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("clientSecret")) {
            if (!oAuth2.clientSecret.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate clientSecret tag in oAuth2";
                return std::nullopt;
            }
            oAuth2.clientSecret = xml.readElementText();
        } else {
            xml.skipCurrentElement();
        }
    }
    return oAuth2;
}

Domain parseDomain(QXmlStreamReader &xml)
{
    auto domain = Domain();
    if (xml.attributes().hasAttribute(QLatin1StringView("purpose"))) {
        domain.purpose = xml.attributes().value(QLatin1StringView("purpose")).toString();
    }
    domain.path = xml.readElementText();
    return domain;
}

std::optional<EmailProvider> parseEmailProvider(QXmlStreamReader &xml, QList<Server> &servers)
{
    auto emailProvider = EmailProvider();
    if (xml.attributes().hasAttribute(QLatin1StringView("id"))) {
        emailProvider.id = xml.attributes().value(QLatin1StringView("id")).toString();
    }
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("domain")) {
            emailProvider.domains.append(parseDomain(xml));
        } else if (xml.name() == QLatin1StringView("displayName")) {
            if (!emailProvider.displayName.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate displayName tag in emailProvider";
                return {};
            }
            emailProvider.displayName = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("displayShortName")) {
            if (!emailProvider.displayShortName.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: unexpected duplicate displayShortName tag in emailProvider";
                return {};
            }
            emailProvider.displayShortName = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("documentation")) {
            emailProvider.documentations.append(parseDocumentation(xml));
        } else if (xml.name() == QLatin1StringView("incomingServer") || xml.name() == QLatin1StringView("outgoingServer")) {
            const auto protocol = xml.attributes().value(QLatin1StringView("type")).toString();
            auto res = parseServer(xml);
            if (!res) {
                return {};
            }
            servers.append(*res);
        } else {
            xml.skipCurrentElement();
        }
    }
    return emailProvider;
}

std::optional<ClientConfig> parseClientConfig(QXmlStreamReader &xml)
{
    constexpr auto serviceTags = std::to_array({
        QLatin1StringView("calendar"),
        QLatin1StringView("addressbook"),
        QLatin1StringView("fileShare"),
        QLatin1StringView("chatServer"),
        QLatin1StringView("videoConference"),
        QLatin1StringView("setupServer"),

    });
    auto clientConfig = ClientConfig();
    if (xml.attributes().hasAttribute(QLatin1StringView("version"))) {
        clientConfig.version = xml.attributes().value(QLatin1StringView("version")).toString();
    }
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("emailProvider")) {
            const auto res = parseEmailProvider(xml, clientConfig.servers);
            if (!res) {
                return {};
            }
            clientConfig.emailProvider = *res;
        } else if (std::ranges::find(serviceTags, xml.name().toString()) != serviceTags.end()) {
            if (xml.attributes().hasAttribute(QLatin1StringView("type"))) {
                const auto protocol = xml.attributes().value(QLatin1StringView("type")).toString();
                auto res = parseServer(xml);
                if (!res) {
                    return {};
                }
                clientConfig.servers.append(*res);
            } else {
                xml.skipCurrentElement();
            }
        } else if (xml.name() == QLatin1StringView("oAuth2")) {
            auto res = parseOAuth2(xml);
            if (!res.has_value()) {
                return std::nullopt;
            }
            clientConfig.oAuth2 = res.value();
        } else if (xml.name() == QLatin1StringView("clientConfigUpdate")) {
            if (xml.attributes().hasAttribute(QLatin1StringView("url"))) {
                clientConfig.clientConfigUpdate = QUrl(xml.attributes().value(QLatin1StringView("url")).toString());
            }
        } else {
            xml.skipCurrentElement();
        }
    }
    return clientConfig;
}

std::optional<ClientConfig> AutoConfigParser::parse(QIODevice *doc)
{
    QXmlStreamReader xml;
    xml.setDevice(doc);

    while (xml.readNextStartElement() && xml.name() != QLatin1StringView("clientConfig")) {
        xml.skipCurrentElement();
    }

    if (xml.hasError()) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "XML parse error:" << xml.errorString() << "at line" << xml.lineNumber() << "column" << xml.columnNumber();
        return {};
    }
    if (xml.name() != QLatin1StringView("clientConfig")) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: there is no <clientConfig> node";
        return {};
    }

    auto res = parseClientConfig(xml);
    if (xml.hasError()) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "XML parse error:" << xml.errorString() << "at line" << xml.lineNumber() << "column" << xml.columnNumber();
        return {};
    }
    if (!res || !isClientConfigValid(*res)) {
        return {};
    }
    return res;
}
