/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QSharedDataPointer>
#include <QUrl>

#include <kaccountautoconfig_export.h>

class KAutoconfigServerPrivate;

/*!
 * \class KAutoconfigServer
 * \inmodule KAutoconfigAccount
 * \brief This class holds information about a server.
 *
 * All information needed to connect to a server is stored in this class, except for the password and the email that are present in the Job.
 * Server should be understood as the RFC https://www.ietf.org/archive/id/draft-bucksch-mailmaint-pacc-00.html#name-servers
 */
class KACCOUNTAUTOCONFIG_EXPORT KAutoconfigServer
{
public:
    enum Protocol {
        JMAP,
        IMAP,
        POP3,
        SMTP,
        CalDAV,
        CardDAV,
        /*!
         * Used to check if KAccountAutoconfigJob implements all protocols (at least with the default value).
         * Should always be the last element and never specifies values for this enum.
         */
        Last
    };

    enum AuthenticationType {
        /*!
         * Default value
         */
        UnknownAuthentication,
        Basic,
        Digest
    };

    enum SocketType {
        /*!
         * Default value
         */
        UnknownSocketType,
        Plain,
        SSL,
        STARTTLS
    };

    explicit KAutoconfigServer(Protocol protocol);
    ~KAutoconfigServer();

    /*!
     * Needed for QsharedDataPointer
     */
    KAutoconfigServer(const KAutoconfigServer &other);
    KAutoconfigServer &operator=(const KAutoconfigServer &other);
    KAutoconfigServer(KAutoconfigServer &&other) noexcept;
    KAutoconfigServer &operator=(KAutoconfigServer &&other) noexcept;

    /*!
     * The url to connect to the sever. Port include.
     */
    [[nodiscard]] QUrl url() const;
    void setUrl(const QUrl &newUrl);

    /*!
     * The protocol of the server, @see Protocol
     */
    [[nodiscard]] Protocol protocol() const;

    /*!
     * Authentication method to use to connect to the sever, @see AuthenticationType
     */
    [[nodiscard]] AuthenticationType authenticationType() const;
    void setAuthenticationType(AuthenticationType newAuthenticationType);

    /*!
     * Socket type for tcp-based protocol (IMAP, POP3, etc...)
     */
    [[nodiscard]] SocketType socketType() const;
    void setSocketType(SocketType newSocketType);

    /*!
     * If true, the data can be used to connect to the server.
     */
    [[nodiscard]] bool isValid() const;

private:
    QSharedDataPointer<KAutoconfigServerPrivate> d;
};
