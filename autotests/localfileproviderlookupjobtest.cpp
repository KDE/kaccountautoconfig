/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>

#include "lib/localfileproviderlookupjob_p.h"

/*!
 * Test the local file provider lookup job.
 * QStandardPaths::setTestModeEnabled is used for testing purposes.
 */
class LocalFileProviderLookupJobTest : public QObject
{
    Q_OBJECT
    const QString validEmail = QStringLiteral("test@valid.org");
    const QString invalidEmail = QStringLiteral("test@invalid.org");

    const QString validConfiguration = QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("example.xml"));
    const QString invalidConfiguration = QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("invalid.xml"));

private Q_SLOTS:
    void setDataFile(const QString &configuration)
    {
        // dest file should be in an "isp" directory, and the filename should be the same as the domain name of the email, according to the rfc,
        const QString validUserConfigPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QStringLiteral("/isp/valid.org.xml");

        // remove if exist
        QFile::remove(validUserConfigPath);

        QVERIFY(QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).mkpath(QStringLiteral("isp/")));
        QFile::copy(configuration, validUserConfigPath);
    }

    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
    }

    void testLocalFileProviderLookupJob()
    {
        setDataFile(validConfiguration);
        const auto job = new LocalFileProviderLookupJob(validEmail, this);
        QSignalSpy spyJob(job, &LocalFileProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(spyJob.count(), 1);
        QCOMPARE(job->error(), 0);
        QCOMPARE(job->configuration().has_value(), true);
    }

    void testLocalFileProviderInvalidConfigurationLookupJob()
    {
        setDataFile(invalidConfiguration);

        const auto job = new LocalFileProviderLookupJob(validEmail);
        QSignalSpy spyJob(job, &LocalFileProviderLookupJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->configuration().has_value(), false);
        QCOMPARE(job->source().isEmpty(), true);
        // invalid so search another path. At the end Configuration not found
        QCOMPARE(job->error(), LookupProviderJobBase::ConfigurationNotFound);
    }
};

QTEST_MAIN(LocalFileProviderLookupJobTest)

#include "localfileproviderlookupjobtest.moc"
