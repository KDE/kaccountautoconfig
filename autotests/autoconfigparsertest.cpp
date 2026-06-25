/*
 *  SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "lib/autoconfigparser_p.h"
#include <QTest>

class AutoConfigParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_email()
    {
        QFile file(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("gandi.xml")));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto res = doParse(&file);
        QVERIFY(res);
        const auto &config = *res;
        QVERIFY(config.emailProvider.displayName == QStringLiteral("Gandi Mail"));
    }

    void test_all()
    {
        QFile file(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("example.xml")));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto res = doParse(&file);
        QVERIFY(res);
        const auto &config = *res;
        QVERIFY(config.emailProvider.displayName == QStringLiteral("Google Workspace"));
    }

    void test_invalid()
    {
        QFile file(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("invalid.xml")));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const auto res = doParse(&file);
        QVERIFY(!res);
    }
};

QTEST_MAIN(AutoConfigParserTest);

#include "autoconfigparsertest.moc"
