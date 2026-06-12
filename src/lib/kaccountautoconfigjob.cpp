/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kaccountautoconfigjob.h"
#include "kautoconfigserver.h"

class KAccountAutoconfigJobPrivate
{
public:
    explicit KAccountAutoconfigJobPrivate(KAccountAutoconfigJob *job);

    QString m_email;
    QString m_password;
    QList<KAutoconfigServer> m_servers;

    KAccountAutoconfigJob *q;
};

KAccountAutoconfigJobPrivate::KAccountAutoconfigJobPrivate(KAccountAutoconfigJob *job)
    : q(job)
{
    m_servers.reserve(KAutoconfigServer::Protocol::Last);
    for (int i = 0; i < KAutoconfigServer::Protocol::Last; ++i) {
        KAutoconfigServer server(static_cast<KAutoconfigServer::Protocol>(i));
        m_servers.emplace_back(server);
    }
}

KAccountAutoconfigJob::KAccountAutoconfigJob(QObject *parent)
    : KJob(parent)
    , d(new KAccountAutoconfigJobPrivate(this))
{
}

KAccountAutoconfigJob::~KAccountAutoconfigJob() = default;

void KAccountAutoconfigJob::start()
{
}

QString KAccountAutoconfigJob::password() const
{
    return d->m_password;
}

void KAccountAutoconfigJob::setPassword(const QString &newPassword)
{
    d->m_password = newPassword;
}

QString KAccountAutoconfigJob::email() const
{
    return d->m_email;
}

void KAccountAutoconfigJob::setEmail(const QString &newEmail)
{
    d->m_email = newEmail;
}

QList<KAutoconfigServer> KAccountAutoconfigJob::servers() const
{
    return d->m_servers;
}

KAutoconfigServer KAccountAutoconfigJob::getServer(KAutoconfigServer::Protocol protocol) const
{
    const auto it = std::ranges::find_if(d->m_servers, [&](const auto &server) {
        return server.protocol() == protocol;
    });
    // all servers are init by default
    Q_ASSERT(it != d->m_servers.end());
    return *it;
}
