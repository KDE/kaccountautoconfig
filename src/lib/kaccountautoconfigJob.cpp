/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kaccountautoconfigJob.h"

#include "kautoconfigserver.h"

#include <qmap.h>

class KAccountAutoconfigJobPrivate
{
public:
    explicit KAccountAutoconfigJobPrivate(KAccountAutoconfigJob *job);

    QString m_email;
    QString m_password;
    QMap<KAutoconfigServer::Protocol, KAutoconfigServer> m_servers;

    KAccountAutoconfigJob *q;
};

KAccountAutoconfigJobPrivate::KAccountAutoconfigJobPrivate(KAccountAutoconfigJob *job)
    : q(job)
{
    // by default, create empty server for all protocol.
    const QList protocols = {KAutoconfigServer::Protocol::JMAP,
                             KAutoconfigServer::Protocol::IMAP,
                             KAutoconfigServer::Protocol::POP3,
                             KAutoconfigServer::Protocol::SMTP,
                             KAutoconfigServer::Protocol::CalDAV,
                             KAutoconfigServer::Protocol::CardDAV};

    for (const auto protocol : protocols) {
        KAutoconfigServer server(protocol);
        m_servers.insert(protocol, server);
    }
}

KAccountAutoconfigJob::KAccountAutoconfigJob(const QString &email, const QString &password, QObject *parent)
    : KJob(parent)
    , d(new KAccountAutoconfigJobPrivate(this))
{
    d->m_password = password;
    d->m_email = email;
}
KAccountAutoconfigJob::~KAccountAutoconfigJob() = default;

void KAccountAutoconfigJob::start()
{
    emitResult();
}

QString KAccountAutoconfigJob::password() const
{
    return d->m_password;
}
QString KAccountAutoconfigJob::email() const
{
    return d->m_email;
}
QList<KAutoconfigServer> KAccountAutoconfigJob::servers() const
{
    return d->m_servers.values();
}
KAutoconfigServer KAccountAutoconfigJob::getServer(const KAutoconfigServer::Protocol protocol) const
{
    return d->m_servers.find(protocol).value();
}
