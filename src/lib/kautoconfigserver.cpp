/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kautoconfigserver.h"

class KAutoconfigServerPrivate : public QSharedData
{
public:
    explicit KAutoconfigServerPrivate(KAutoconfigServer::Protocol protocol)
        : m_protocol(protocol) { };
    QUrl m_url;
    KAutoconfigServer::Protocol m_protocol;
    KAutoconfigServer::Status m_status = KAutoconfigServer::None;
    KAutoconfigServer::AuthenticationType m_authenticationType = KAutoconfigServer::UnknownAuthentication;
    KAutoconfigServer::SocketType m_socketType = KAutoconfigServer::UnknownSocketType;
};

KAutoconfigServer::KAutoconfigServer(Protocol protocol)
    : d(new KAutoconfigServerPrivate(protocol))
{
}

KAutoconfigServer::~KAutoconfigServer() = default;
KAutoconfigServer::KAutoconfigServer(const KAutoconfigServer &other) = default;
KAutoconfigServer &KAutoconfigServer::operator=(const KAutoconfigServer &other) = default;
KAutoconfigServer::KAutoconfigServer(KAutoconfigServer &&other) noexcept = default;
KAutoconfigServer &KAutoconfigServer::operator=(KAutoconfigServer &&other) noexcept = default;

QUrl KAutoconfigServer::url() const
{
    return d->m_url;
}

void KAutoconfigServer::setUrl(const QUrl &newUrl)
{
    d->m_url = newUrl;
}

KAutoconfigServer::Protocol KAutoconfigServer::protocol() const
{
    return d->m_protocol;
}

KAutoconfigServer::Status KAutoconfigServer::status() const
{
    return d->m_status;
}

void KAutoconfigServer::setStatus(Status newStatus)
{
    d->m_status = newStatus;
}

KAutoconfigServer::AuthenticationType KAutoconfigServer::authenticationType() const
{
    return d->m_authenticationType;
}

void KAutoconfigServer::setAuthenticationType(AuthenticationType newAuthenticationType)
{
    d->m_authenticationType = newAuthenticationType;
}

KAutoconfigServer::SocketType KAutoconfigServer::socketType() const
{
    return d->m_socketType;
}

void KAutoconfigServer::setSocketType(SocketType newSocketType)
{
    d->m_socketType = newSocketType;
}

bool KAutoconfigServer::isValid() const
{
    return d->m_status == LoginSuccess && d->m_url.isValid() && d->m_authenticationType != UnknownAuthentication;
}
