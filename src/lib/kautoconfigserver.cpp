/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kautoconfigserver.h"

class AutoconfigServerPrivate : public QSharedData
{
public:
    QUrl m_url;
    KAutoconfigServer::Protocol m_protocol;
    KAutoconfigServer::Status m_status = KAutoconfigServer::None;
    KAutoconfigServer::AuthentificationType m_authentificationType = KAutoconfigServer::UnknownAuthentification;
};

KAutoconfigServer::KAutoconfigServer(const Protocol protocol)
    : d(new AutoconfigServerPrivate)
{
    d->m_protocol = protocol;
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

void KAutoconfigServer::setUrl(const QUrl &url)
{
    d->m_url = url;
}

KAutoconfigServer::Protocol KAutoconfigServer::protocol() const
{
    return d->m_protocol;
}

KAutoconfigServer::Status KAutoconfigServer::status() const
{
    return d->m_status;
}
void KAutoconfigServer::setStatus(const Status status)
{
    d->m_status = status;
}

KAutoconfigServer::AuthentificationType KAutoconfigServer::authentificationType() const
{
    return d->m_authentificationType;
}
void KAutoconfigServer::setAuthentificationType(const AuthentificationType authentificationType)
{
    d->m_authentificationType = authentificationType;
}
bool KAutoconfigServer::isValid() const
{
    return d->m_status == LoginSuccess && d->m_url.isValid() && d->m_authentificationType != UnknownAuthentification;
}
