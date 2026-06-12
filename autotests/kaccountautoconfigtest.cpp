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
        const auto email = QStringLiteral("test@example.com");
        const auto password = QStringLiteral("test");
        const auto job = new KAccountAutoconfigJob(this);
        // 6 protocols for the moment
        Q_ASSERT(job->servers().size() == 6);

        std::ranges::for_each(job->servers(), [](const auto &server) {
            // default value for all servers
            QCOMPARE(server.status(), KAutoconfigServer::Status::None);
            QCOMPARE(server.authenticationType(), KAutoconfigServer::UnknownAuthentication);
            QCOMPARE(server.socketType(), KAutoconfigServer::UnknownSocketType);
        });
    }

    void testGetServer()
    {
        const auto job = new KAccountAutoconfigJob(this);
        const auto imapServer = job->getServer(KAutoconfigServer::Protocol::IMAP);
        QCOMPARE(imapServer.protocol(), KAutoconfigServer::Protocol::IMAP);
        QCOMPARE(imapServer.status(), KAutoconfigServer::Status::None);
        QCOMPARE(imapServer.authenticationType(), KAutoconfigServer::UnknownAuthentication);
        QCOMPARE(imapServer.socketType(), KAutoconfigServer::UnknownSocketType);
    }
};

QTEST_MAIN(KAccountAutoconfigTest)

#include "kaccountautoconfigtest.moc"
