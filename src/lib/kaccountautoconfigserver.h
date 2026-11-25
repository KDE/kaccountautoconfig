/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QDebug>
#include <QSharedDataPointer>
#include <QUrl>

#include <kaccountautoconfig_export.h>

class KAccountAutoconfigServerPrivate;

/*!
 * \class KAccountAutoconfigServer
 * \inmodule KAccountAutoconfig
 * \brief This class holds information about a server.
 *
 * All information needed to connect to a server is stored in this class, except for the email that is present in the Job.
 * Server should be understood as the RFC https://www.ietf.org/archive/id/draft-bucksch-mailmaint-pacc-00.html#name-servers
 */
class KACCOUNTAUTOCONFIG_EXPORT KAccountAutoconfigServer
{
    Q_GADGET

public:
    /*!
     * \enum KAccountAutoconfigServer::Protocol
     * \value IMAP IMAP protocol.
     * \value POP3 POP3 protocol.
     * \value SMTP SMTP protocol.
     * \value CalDAV CalDAV protocol.
     * \value CardDAV CardDAV protocol.
     */
    enum Protocol {
        IMAP,
        POP3,
        SMTP,
        CalDAV,
        CardDAV
    };
    Q_ENUM(Protocol);

    /*!
     * \enum KAccountAutoconfigServer::AuthenticationType
     * \value UnknownAuthentication Default value.
     * URL-based protocols
     * \value Basic Basic authentication
     * \value Digest Digest authentication
     * Common
     * \value OAuth2 OAuth2 authentication
     * TCP-based protocols
     * \value PasswordCleartext Password authentication over cleartext
     * \value PasswordEncrypted Password authentication over encrypted connection
     * \value NTLM NTLM authentication
     * \value GSSAPI GSSAPI authentication
     * \value TlsClientCert TLS client certificate authentication
     * \value ClientIpAddress Client IP address authentication
     */
    enum AuthenticationType {
        UnknownAuthentication,
        Basic,
        Digest,
        OAuth2,
        PasswordCleartext,
        PasswordEncrypted,
        NTLM,
        GSSAPI,
        TlsClientCert,
        ClientIpAddress
    };
    Q_ENUM(AuthenticationType);

    /*!
     * \enum KAccountAutoconfigServer::SocketType
     * \value UnknownSocketType Default value.
     * \value Plain Plain text socket
     * \value SSL SSL socket
     * \value STARTTLS STARTTLS socket
     */
    enum SocketType {
        UnknownSocketType,
        Plain,
        SSL,
        STARTTLS
    };
    Q_ENUM(SocketType);

    explicit KAccountAutoconfigServer(Protocol protocol);
    ~KAccountAutoconfigServer();

    /*!
     * Needed for QsharedDataPointer
     */
    KAccountAutoconfigServer(const KAccountAutoconfigServer &other);
    KAccountAutoconfigServer &operator=(const KAccountAutoconfigServer &other);
    KAccountAutoconfigServer(KAccountAutoconfigServer &&other) noexcept;
    KAccountAutoconfigServer &operator=(KAccountAutoconfigServer &&other) noexcept;
    bool operator==(const KAccountAutoconfigServer &other) const;
    /*!
     * The url to connect to the sever. Port include.
     */
    [[nodiscard]] QUrl url() const;
    void setUrl(const QUrl &newUrl);

    /*!
     * The protocol of the server, \sa Protocol
     */
    [[nodiscard]] Protocol protocol() const;

    /*!
     * Authentication method to use to connect to the sever, \sa AuthenticationType
     */
    [[nodiscard]] AuthenticationType authenticationType() const;
    void setAuthenticationType(AuthenticationType newAuthenticationType);

    /*!
     * Socket type for tcp-based protocol (IMAP, POP3, etc...)
     */
    [[nodiscard]] SocketType socketType() const;
    void setSocketType(SocketType newSocketType);

    /*!
     * Returns whether the data can be used to connect to the server
     */
    [[nodiscard]] bool isValid() const;

private:
    QSharedDataPointer<KAccountAutoconfigServerPrivate> d;
};

KACCOUNTAUTOCONFIG_EXPORT QDebug operator<<(QDebug debug, const KAccountAutoconfigServer &server);
