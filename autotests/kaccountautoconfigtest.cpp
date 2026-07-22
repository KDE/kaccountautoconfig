/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <kaccountautoconfig/kaccountautoconfigjob.h>

#include <QTest>

class KAccountAutoconfigTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testInit()
    {
        const auto job = new KAccountAutoconfigJob(this);
        Q_ASSERT(job->servers().size() == 0);
    }
};

QTEST_MAIN(KAccountAutoconfigTest)

#include "kaccountautoconfigtest.moc"
