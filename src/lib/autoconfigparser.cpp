/*
    SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "autoconfigparser_p.h"
#include "autoconfigvalidator.h"
#include "kaccountautoconfig_debug.h"
#include <QXmlStreamReader>

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
                qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: unexpected duplicate url tag in server";
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
                qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: unexpected duplicate username tag in server";
                return {};
            }
            server.username = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("hostname")) {
            if (!server.hostname.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: unexpected duplicate hostname tag in server";
                return {};
            }
            server.hostname = QUrl();
            server.hostname.setHost(xml.readElementText());
        } else if (xml.name() == QLatin1StringView("port")) {
            if (server.port != 0) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: unexpected duplicate port tag in server";
                return {};
            }
            server.port = xml.readElementText().toInt();
        } else if (xml.name() == QLatin1StringView("socketType")) {
            if (!server.socketType.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: unexpected duplicate socketType tag in server";
                return {};
            }
            server.socketType = xml.readElementText();
        } else {
            xml.skipCurrentElement();
        }
    }
    return server;
}

std::optional<EmailProvider> parseEmailProvider(QXmlStreamReader &xml, QHash<QString, QList<Server>> &serversByProtocol)
{
    auto emailProvider = EmailProvider();
    if (xml.attributes().hasAttribute(QLatin1StringView("id"))) {
        emailProvider.id = xml.attributes().value(QLatin1StringView("id")).toString();
    }
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("domain")) {
            emailProvider.domains.append(xml.readElementText());
        } else if (xml.name() == QLatin1StringView("displayName")) {
            if (!emailProvider.displayName.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: unexpected duplicate displayName tag in emailProvider";
                return {};
            }
            emailProvider.displayName = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("displayShortName")) {
            if (!emailProvider.displayShortName.isEmpty()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: unexpected duplicate displayShortName tag in emailProvider";
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
            serversByProtocol[protocol].append(*res);
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
    });
    auto clientConfig = ClientConfig();
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("emailProvider")) {
            const auto res = parseEmailProvider(xml, clientConfig.serversByProtocol);
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
                clientConfig.serversByProtocol[protocol].append(*res);
            } else {
                xml.skipCurrentElement();
            }
        } else {
            xml.skipCurrentElement();
        }
    }
    return clientConfig;
}

std::optional<ClientConfig> doParse(QIODevice *doc)
{
    QXmlStreamReader xml;
    xml.setDevice(doc);

    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("clientConfig")) {
            auto res = parseClientConfig(xml);
            if (xml.hasError()) {
                qCWarning(KACCOUNTAUTOCONFIG_LOG) << "XML parse error:" << xml.errorString() << "at line" << xml.lineNumber() << "column" << xml.columnNumber();
                return {};
            }
            if (!res || !isValid(res.value())) {
                return std::nullopt;
            }
            return res;
        }
        xml.skipCurrentElement();
    }

    if (xml.hasError()) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "XML parse error:" << xml.errorString() << "at line" << xml.lineNumber() << "column" << xml.columnNumber();
    } else {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Malformed XML: there is no <clientConfig> node";
    }
    return {};
}
