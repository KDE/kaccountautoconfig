/*
    SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "autoconfigparser_p.h"
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

Server parseServer(QXmlStreamReader &xml)
{
    auto server = Server();
    if (xml.attributes().hasAttribute(QLatin1StringView("type"))) {
        server.type = xml.attributes().value(QLatin1StringView("type")).toString();
    }
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("url")) {
            server.url = QUrl(xml.readElementText());
        } else if (xml.name() == QLatin1StringView("authentication")) {
            auto authentication = Authentication();
            if (xml.attributes().hasAttribute(QLatin1StringView("system"))) {
                authentication.system = xml.attributes().value(QLatin1StringView("system")).toString();
            }
            authentication.method = xml.readElementText();
            server.authentications.append(authentication);
        } else if (xml.name() == QLatin1StringView("username")) {
            server.username = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("hostname")) {
            server.hostname = QUrl();
            server.hostname.setHost(xml.readElementText());
        } else if (xml.name() == QLatin1StringView("port")) {
            server.port = xml.readElementText().toInt();
        } else if (xml.name() == QLatin1StringView("socketType")) {
            server.socketType = xml.readElementText();
        } else {
            xml.skipCurrentElement();
        }
    }
    return server;
}

EmailProvider parseEmailProvider(QXmlStreamReader &xml, QHash<QString, QList<Server>> &serversByProtocol)
{
    auto emailProvider = EmailProvider();
    if (xml.attributes().hasAttribute(QLatin1StringView("id"))) {
        emailProvider.id = xml.attributes().value(QLatin1StringView("id")).toString();
    }
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("domain")) {
            emailProvider.domains.append(xml.readElementText());
        } else if (xml.name() == QLatin1StringView("displayName")) {
            emailProvider.displayName = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("displayShortName")) {
            emailProvider.displayShortName = xml.readElementText();
        } else if (xml.name() == QLatin1StringView("documentation")) {
            emailProvider.documentations.append(parseDocumentation(xml));
        } else if (xml.name() == QLatin1StringView("incomingServer") || xml.name() == QLatin1StringView("outgoingServer")) {
            serversByProtocol[xml.attributes().value(QLatin1StringView("type")).toString()].append(parseServer(xml));
        } else {
            xml.skipCurrentElement();
        }
    }
    return emailProvider;
}

ClientConfig parseClientConfig(QXmlStreamReader &xml)
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
            clientConfig.emailProvider = parseEmailProvider(xml, clientConfig.serversByProtocol);
        } else if (std::ranges::find(serviceTags, xml.name().toString()) != serviceTags.end()) {
            if (xml.attributes().hasAttribute(QLatin1StringView("type"))) {
                clientConfig.serversByProtocol[xml.attributes().value(QLatin1StringView("type")).toString()].append(parseServer(xml));
            } else {
                xml.skipCurrentElement();
            }
        } else {
            xml.skipCurrentElement();
        }
    }
    return clientConfig;
}

ClientConfig doParse(QIODevice *doc)
{
    QXmlStreamReader xml;
    xml.setDevice(doc);
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1StringView("clientConfig")) {
            return parseClientConfig(xml);
        }
        xml.skipCurrentElement();
    }
    // If we get here it means there haven't been any clientConfig element
    return {};
}
