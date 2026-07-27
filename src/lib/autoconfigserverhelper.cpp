/*
    SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autoconfigserverhelper_p.h"

namespace AutoconfigServerHelper
{
KAutoconfigServer::AuthenticationType authenticationFromString(const QString &authenticationStr)
{
    static const QHash<QString, KAutoconfigServer::AuthenticationType> authenticationMap{
        {QStringLiteral("password-cleartext"), KAutoconfigServer::AuthenticationType::PasswordCleartext},
        {QStringLiteral("password-encrypted"), KAutoconfigServer::AuthenticationType::PasswordEncrypted},
        {QStringLiteral("basic"), KAutoconfigServer::AuthenticationType::Basic},
        {QStringLiteral("digest"), KAutoconfigServer::AuthenticationType::Digest},
    };

    return authenticationMap.value(authenticationStr.toLower(), KAutoconfigServer::AuthenticationType::UnknownAuthentication);
}

KAutoconfigServer::SocketType socketTypeFromString(const QString &socketTypeStr)
{
    static const QHash<QString, KAutoconfigServer::SocketType> socketTypeMap{
        {QStringLiteral("SSL"), KAutoconfigServer::SocketType::SSL},
        {QStringLiteral("STARTTLS"), KAutoconfigServer::SocketType::STARTTLS},
    };

    return socketTypeMap.value(socketTypeStr.toUpper(), KAutoconfigServer::SocketType::UnknownSocketType);
}

QUrl createUrlFromParserEntry(KAutoconfigServer::Protocol protocol, const AutoConfigParser::Server &server)
{
    static const QList hostnameBasedProtocol{KAutoconfigServer::IMAP, KAutoconfigServer::POP3, KAutoconfigServer::SMTP};
    QUrl url(server.url);
    if (server.port != 0) {
        url.setPort(server.port);
    }
    if (!hostnameBasedProtocol.contains(protocol)) {
        return url;
    }
    // override host and url if hostnameBasedProtocol
    url.setScheme(server.type);
    if (protocol == KAutoconfigServer::POP3) {
        // according to https://www.iana.org/assignments/uri-schemes/uri-schemes.xhtml
        // pop3 should be pop
        url.setScheme(QStringLiteral("pop"));
    }
    url.setHost(server.hostname);
    return url;
}

KAutoconfigServer createServerFromParserEntry(const AutoConfigParser::Server &server, KAutoconfigServer::Protocol protocol, const QString &authenticationType)
{
    KAutoconfigServer publicServer(protocol);
    publicServer.setUrl(createUrlFromParserEntry(protocol, server));
    publicServer.setSocketType(socketTypeFromString(server.socketType));
    publicServer.setAuthenticationType(authenticationFromString(authenticationType));
    return publicServer;
}

std::optional<KAutoconfigServer::Protocol> protocolFromString(const QString &protocolStr)
{
    static const QHash<QString, KAutoconfigServer::Protocol> protocolMap{
        {QStringLiteral("imap"), KAutoconfigServer::IMAP},
        {QStringLiteral("pop3"), KAutoconfigServer::POP3},
        {QStringLiteral("smtp"), KAutoconfigServer::SMTP},
        {QStringLiteral("caldav"), KAutoconfigServer::CalDAV},
        {QStringLiteral("carddav"), KAutoconfigServer::CardDAV},
    };
    if (!protocolMap.contains(protocolStr.toLower())) {
        return std::nullopt;
    }
    return protocolMap.value(protocolStr.toLower());
}
}
