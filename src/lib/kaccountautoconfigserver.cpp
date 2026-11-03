/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kaccountautoconfigserver.h"

class KAccountAutoconfigServerPrivate : public QSharedData
{
public:
    explicit KAccountAutoconfigServerPrivate(KAccountAutoconfigServer::Protocol protocol)
        : m_protocol(protocol) { };
    QUrl m_url;
    KAccountAutoconfigServer::Protocol m_protocol;
    KAccountAutoconfigServer::AuthenticationType m_authenticationType = KAccountAutoconfigServer::UnknownAuthentication;
    KAccountAutoconfigServer::SocketType m_socketType = KAccountAutoconfigServer::UnknownSocketType;
};

KAccountAutoconfigServer::KAccountAutoconfigServer(Protocol protocol)
    : d(new KAccountAutoconfigServerPrivate(protocol))
{
}

KAccountAutoconfigServer::~KAccountAutoconfigServer() = default;
KAccountAutoconfigServer::KAccountAutoconfigServer(const KAccountAutoconfigServer &other) = default;
KAccountAutoconfigServer &KAccountAutoconfigServer::operator=(const KAccountAutoconfigServer &other) = default;
KAccountAutoconfigServer::KAccountAutoconfigServer(KAccountAutoconfigServer &&other) noexcept = default;
KAccountAutoconfigServer &KAccountAutoconfigServer::operator=(KAccountAutoconfigServer &&other) noexcept = default;

bool KAccountAutoconfigServer::operator==(const KAccountAutoconfigServer &other) const
{
    return d->m_url == other.d->m_url && d->m_protocol == other.d->m_protocol && d->m_socketType == other.d->m_socketType
        && d->m_authenticationType == other.d->m_authenticationType;
};

QUrl KAccountAutoconfigServer::url() const
{
    return d->m_url;
}

void KAccountAutoconfigServer::setUrl(const QUrl &newUrl)
{
    d->m_url = newUrl;
}

KAccountAutoconfigServer::Protocol KAccountAutoconfigServer::protocol() const
{
    return d->m_protocol;
}

KAccountAutoconfigServer::AuthenticationType KAccountAutoconfigServer::authenticationType() const
{
    return d->m_authenticationType;
}

void KAccountAutoconfigServer::setAuthenticationType(AuthenticationType newAuthenticationType)
{
    d->m_authenticationType = newAuthenticationType;
}

KAccountAutoconfigServer::SocketType KAccountAutoconfigServer::socketType() const
{
    return d->m_socketType;
}

void KAccountAutoconfigServer::setSocketType(SocketType newSocketType)
{
    d->m_socketType = newSocketType;
}

bool KAccountAutoconfigServer::isValid() const
{
    return d->m_url.isValid() && d->m_authenticationType != UnknownAuthentication;
}

QDebug operator<<(QDebug debug, const KAccountAutoconfigServer &server)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "KAccountAutoconfigServer("
                    << "protocol=" << server.protocol() << ", url=" << server.url() << ", auth=" << server.authenticationType()
                    << ", socketType=" << server.socketType() << ", valid=" << server.isValid() << ')';
    return debug;
}
