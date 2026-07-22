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
    QList<KAutoconfigServer> m_servers;

    KAccountAutoconfigJob *q;
};

KAccountAutoconfigJobPrivate::KAccountAutoconfigJobPrivate(KAccountAutoconfigJob *job)
    : q(job)
{
}

KAccountAutoconfigJob::KAccountAutoconfigJob(QObject *parent)
    : KCompositeJob(parent)
    , d(new KAccountAutoconfigJobPrivate(this))
{
}

KAccountAutoconfigJob::~KAccountAutoconfigJob() = default;

void KAccountAutoconfigJob::start()
{
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
