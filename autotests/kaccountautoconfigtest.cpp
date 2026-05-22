/*
 *  SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kaccountautoconfig/kaccountautoconfigjob.h"

#include <QTest>

class KAccountAutoconfigTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testAutoconfigWithValidEmail()
    {
        auto job = new KAccountAutoconfigJob(this);
        job->start();
        Q_ASSERT(job->error() == 0);
    };
};

QTEST_MAIN(KAccountAutoconfigTest)

#include "kaccountautoconfigtest.moc"
