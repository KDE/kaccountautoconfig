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

bool KAutoconfigServer::operator==(const KAutoconfigServer &other) const
{
    return d->m_url == other.d->m_url && d->m_protocol == other.d->m_protocol && d->m_socketType == other.d->m_socketType
        && d->m_authenticationType == other.d->m_authenticationType;
};

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
    return d->m_url.isValid() && d->m_authenticationType != UnknownAuthentication;
}

QDebug operator<<(QDebug debug, const KAutoconfigServer &server)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "KAutoconfigServer("
                    << "protocol=" << server.protocol() << ", url=" << server.url() << ", auth=" << server.authenticationType()
                    << ", socketType=" << server.socketType() << ", valid=" << server.isValid() << ')';
    return debug;
}
