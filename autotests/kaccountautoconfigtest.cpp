/*
 *  SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QTest>

#include "lib/kaccountautoconfigJob.h"

class KAccountAutoconfigTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    static void testAutoconfigWithValidEmail()
    {
        auto job = new KAccountAutoconfigJob();
        job->start();
        Q_ASSERT(job->error() == 0);
    };
};

QTEST_MAIN(KAccountAutoconfigTest)

#include "kaccountautoconfigtest.moc"