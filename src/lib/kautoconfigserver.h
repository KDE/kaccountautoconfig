/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include <kaccountautoconfig_export.h>
#include <qshareddata.h>
#include <qurl.h>

class AutoconfigServerPrivate;

/*!
 * This class hold information about a server, aka an email server, dav server, etc...
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
        CardDAV
    };

    enum Status {
        /*!
         * None action hasn't been done yet.
         * Default value
         */
        None,
        /*!
         * The password, email are valid for the given url, protocol, and authentification type.
         */
        LoginSuccess,

        /*!
         * password or email are invalid with the url.
         */
        BadCredentials
    };

    enum AuthentificationType {
        /*!
         * Default value
         */
        UnknownAuthentification,
        Basic,
        Digest
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
    void setUrl(const QUrl &url);

    /*!
     * The protocol of the server, @see Protocol
     */
    [[nodiscard]] Protocol protocol() const;

    /*!
     * The status after a login test, @see Status
     */
    [[nodiscard]] Status status() const;
    void setStatus(Status status);

    /*!
     * Authentification method to use to connect to the sever, @see AuthentificationType
     */
    [[nodiscard]] AuthentificationType authentificationType() const;
    void setAuthentificationType(AuthentificationType authentificationType);

    /*!
     * If true, the data can be used to connect to the server.
     * Also means that all optional datas are set.
     */
    bool isValid() const;

private:
    QSharedDataPointer<AutoconfigServerPrivate> d;
};
