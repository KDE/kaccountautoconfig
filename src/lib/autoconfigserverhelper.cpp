/*
    SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autoconfigserverhelper_p.h"

namespace AutoconfigServerHelper
{
KAccountAutoconfigServer::AuthenticationType authenticationFromString(const QString &authenticationStr)
{
    static const QHash<QString, KAccountAutoconfigServer::AuthenticationType> authenticationMap{
        {QStringLiteral("password-cleartext"), KAccountAutoconfigServer::AuthenticationType::PasswordCleartext},
        {QStringLiteral("password-encrypted"), KAccountAutoconfigServer::AuthenticationType::PasswordEncrypted},
        {QStringLiteral("basic"), KAccountAutoconfigServer::AuthenticationType::Basic},
        {QStringLiteral("digest"), KAccountAutoconfigServer::AuthenticationType::Digest},
        {QStringLiteral("oauth2"), KAccountAutoconfigServer::AuthenticationType::OAuth2},
        {QStringLiteral("oauth"), KAccountAutoconfigServer::AuthenticationType::OAuth2},
        {QStringLiteral("ntlm"), KAccountAutoconfigServer::AuthenticationType::NTLM},
        {QStringLiteral("gssapi"), KAccountAutoconfigServer::AuthenticationType::GSSAPI},
        {QStringLiteral("tls-client-cert"), KAccountAutoconfigServer::AuthenticationType::TlsClientCert},
        {QStringLiteral("client-ip-address"), KAccountAutoconfigServer::AuthenticationType::ClientIpAddress},
    };

    return authenticationMap.value(authenticationStr.toLower(), KAccountAutoconfigServer::AuthenticationType::UnknownAuthentication);
}

KAccountAutoconfigServer::SocketType socketTypeFromString(const QString &socketTypeStr)
{
    static const QHash<QString, KAccountAutoconfigServer::SocketType> socketTypeMap{
        {QStringLiteral("PLAIN"), KAccountAutoconfigServer::SocketType::Plain},
        {QStringLiteral("SSL"), KAccountAutoconfigServer::SocketType::SSL},
        {QStringLiteral("STARTTLS"), KAccountAutoconfigServer::SocketType::STARTTLS},
    };

    return socketTypeMap.value(socketTypeStr.toUpper(), KAccountAutoconfigServer::SocketType::UnknownSocketType);
}

QUrl createUrlFromParserEntry(KAccountAutoconfigServer::Protocol protocol, const AutoConfigParser::Server &server)
{
    static const QList hostnameBasedProtocol{KAccountAutoconfigServer::IMAP, KAccountAutoconfigServer::POP3, KAccountAutoconfigServer::SMTP};
    QUrl url(server.url);
    if (server.port != 0) {
        url.setPort(server.port);
    }
    if (!hostnameBasedProtocol.contains(protocol)) {
        return url;
    }
    // override host and url if hostnameBasedProtocol
    url.setScheme(server.type);
    if (protocol == KAccountAutoconfigServer::POP3) {
        // according to https://www.iana.org/assignments/uri-schemes/uri-schemes.xhtml
        // pop3 should be pop
        url.setScheme(QStringLiteral("pop"));
    }
    url.setHost(server.hostname);
    return url;
}

KAccountAutoconfigServer
createServerFromParserEntry(const AutoConfigParser::Server &server, KAccountAutoconfigServer::Protocol protocol, const QString &authenticationType)
{
    KAccountAutoconfigServer publicServer(protocol);
    publicServer.setUrl(createUrlFromParserEntry(protocol, server));
    publicServer.setSocketType(socketTypeFromString(server.socketType));
    publicServer.setAuthenticationType(authenticationFromString(authenticationType));
    return publicServer;
}

std::optional<KAccountAutoconfigServer::Protocol> protocolFromString(const QString &protocolStr)
{
    static const QHash<QString, KAccountAutoconfigServer::Protocol> protocolMap{
        {QStringLiteral("imap"), KAccountAutoconfigServer::IMAP},
        {QStringLiteral("pop3"), KAccountAutoconfigServer::POP3},
        {QStringLiteral("smtp"), KAccountAutoconfigServer::SMTP},
        {QStringLiteral("caldav"), KAccountAutoconfigServer::CalDAV},
        {QStringLiteral("carddav"), KAccountAutoconfigServer::CardDAV},
    };
    if (!protocolMap.contains(protocolStr.toLower())) {
        return std::nullopt;
    }
    return protocolMap.value(protocolStr.toLower());
}
}
