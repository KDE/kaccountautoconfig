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

    static void testInit()
    {
        const auto email = QStringLiteral("test@example.com");
        const auto password = QStringLiteral("test");
        const auto job = new KAccountAutoconfigJob(email, password);
        // 6 protocols for the moment
        Q_ASSERT(job->servers().size() == 6);

        auto servers = job->servers();
        for (const auto &server : servers) {
            // default value for all servers
            QCOMPARE(server.status(), KAutoconfigServer::Status::None);
            QCOMPARE(server.authentificationType(), KAutoconfigServer::UnknownAuthentification);
        }
    }

    static void testGetServer()
    {
        const auto job = new KAccountAutoconfigJob(QStringLiteral("test@example.com"), QStringLiteral("test")););
        auto imapserver = job->getServer(KAutoconfigServer::Protocol::IMAP);
        QCOMPARE(imapserver.protocol(), KAutoconfigServer::Protocol::IMAP);
    }
};

QTEST_MAIN(KAccountAutoconfigTest)

#include "kaccountautoconfigtest.moc"