/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "emailproviderlookupjob_p.h"
#include "fakeserver/stubnetworkaccessmanager.h"

#include <QSignalSpy>
#include <QTest>

using Responses = StubNetworkAccessManager::Responses;

class EmailProviderLookupJobTest : public QObject
{
    Q_OBJECT

    const QUrl validAutoConfigUrl = QUrl(QStringLiteral("https://autoconfig.kde.org/mail/config-v1.1.xml?emailaddress=test@kde.org"));
    const QUrl validWellKnownUrl = QUrl(QStringLiteral("https://kde.org/.well-known/autoconfig/mail/config-v1.1.xml"));
    // legacy http
    const QUrl validAutoconfigLegacyUrl = QUrl(QStringLiteral("http://autoconfig.kde.org/mail/config-v1.1.xml"));
    // invalid because email domain from validEmail doesn't match
    const QUrl genericFalseUrl = QUrl(QStringLiteral("https://autoconfig.false.com/mail/config-v1.1.xml"));

    // it's the domain that matters
    const QString validEmail = QStringLiteral("test@kde.org");
    const QString invalidEmail = QStringLiteral("test@invalid.org");

    QByteArray validConfiguration;
    QByteArray invalidConfiguration;

private Q_SLOTS:
    void initTestCase()
    {
        QFile validConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("example.xml")));
        QVERIFY(validConfigurationFile.open(QIODevice::ReadOnly));
        validConfiguration = validConfigurationFile.readAll();

        QFile invalidConfigurationFile(QDir(QString::fromLatin1(TEST_DATA_DIR)).filePath(QStringLiteral("invalid.xml")));
        QVERIFY(invalidConfigurationFile.open(QIODevice::ReadOnly));
        invalidConfiguration = invalidConfigurationFile.readAll();
    }

    /*!
     * Simple test to make sure the job returns a valid configuration from a valid url
     */
    void testSimpleValidConfigurationFromMailProvider()
    {
        const StubNetworkAccessManager::Response response{validAutoConfigUrl, validConfiguration, 200};
        StubNetworkAccessManager networkStub;
        networkStub.addResponse(response);

        const auto job = new LookupEmailProviderJob(validEmail, &networkStub);
        QSignalSpy spyJob(job, &LookupEmailProviderJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->error(), 0);
        QCOMPARE(job->configuration().has_value(), true);
        QCOMPARE(job->source(), validAutoConfigUrl.toString());
    }

    void testValidConfigurationFromMailProviderWithErrorCombination_data()
    {
        QTest::addColumn<Responses>("responses");
        QTest::addColumn<QUrl>("expectedSource");

        // clang-format off
        QTest::newRow("network_error_on_autoconfig_invalid_configuration_on_well_known_legacy_url_ok")
            << Responses{
                {
                    {validAutoConfigUrl, validConfiguration, 404},
                    {validWellKnownUrl, invalidConfiguration, 200},
                    {validAutoconfigLegacyUrl, validConfiguration, 200}
                }
            }
            << validAutoconfigLegacyUrl;

        QTest::newRow("invalid_configuration_on_autoconfig_network_error_on_well_known_legacy_url_ok")
            << Responses{
                {
                    {validAutoConfigUrl, invalidConfiguration, 200},
                    {validWellKnownUrl, validConfiguration, 404},
                    {validAutoconfigLegacyUrl, validConfiguration, 200}
                }
            }
            << validAutoconfigLegacyUrl;
        QTest::newRow("invalid_configuration_on_autoconfig_well_known_ok_legacy_url_ok") << Responses{
                {
                    {validAutoConfigUrl, invalidConfiguration, 200},
                    {validWellKnownUrl, validConfiguration, 200},
                    {validAutoconfigLegacyUrl, validConfiguration, 200}
                }
            }
            << validWellKnownUrl;
        // clang-format on
    }

    /*!
     * Test error combination with at least one valid
     */
    void testValidConfigurationFromMailProviderWithErrorCombination()
    {
        QFETCH(Responses, responses);
        QFETCH(QUrl, expectedSource);
        StubNetworkAccessManager networkStub;
        networkStub.addResponses(responses);

        const auto job = new LookupEmailProviderJob(validEmail, &networkStub);
        QSignalSpy spyJob(job, &LookupEmailProviderJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->configuration().has_value(), true);
        QCOMPARE(job->source(), expectedSource.toString());
        QCOMPARE(job->error(), 0);
    }

    /*!
     * invalid configuration on all urls should return an error
     */
    void testInvalidConfigurationFromMailProvider()
    {
        const Responses responses{
            {{validAutoConfigUrl, invalidConfiguration}, {validWellKnownUrl, invalidConfiguration}, {validAutoconfigLegacyUrl, invalidConfiguration}}};

        StubNetworkAccessManager networkStub;
        networkStub.addResponses(responses);

        const auto job = new LookupEmailProviderJob(validEmail, &networkStub);
        QSignalSpy spyJob(job, &LookupEmailProviderJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->configuration().has_value(), false);
        QCOMPARE(job->source().isEmpty(), true);
        QCOMPARE(job->error(), LookupProviderJobBase::InvalidConfiguration);
    }

    /*!
     * Error network on all urls should return an error
     */
    void testNetworkErrorFromMailProvider()
    {
        const Responses responses{
            {{validAutoConfigUrl, validConfiguration, 404}, {validWellKnownUrl, validConfiguration, 404}, {validAutoconfigLegacyUrl, validConfiguration, 404}}};

        StubNetworkAccessManager networkStub;
        networkStub.addResponses(responses);

        const auto job = new LookupEmailProviderJob(validEmail, &networkStub);
        QSignalSpy spyJob(job, &LookupEmailProviderJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->configuration().has_value(), false);
        QCOMPARE(job->source().isEmpty(), true);
        QCOMPARE(job->error(), LookupProviderJobBase::InvalidConfiguration);
    }

    /*!
     * invalid email on all urls should return an error
     */
    void testInvalidEmail()
    {
        const Responses responses{
            {{validAutoConfigUrl, validConfiguration}, {validWellKnownUrl, validConfiguration}, {validAutoconfigLegacyUrl, validConfiguration}}};
        StubNetworkAccessManager networkStub;
        networkStub.addResponses(responses);

        const auto job = new LookupEmailProviderJob(invalidEmail, &networkStub);
        QSignalSpy spyJob(job, &LookupEmailProviderJob::result);

        job->start();
        spyJob.wait();
        QCOMPARE(job->configuration().has_value(), false);
        QCOMPARE(job->source().isEmpty(), true);
        QCOMPARE(job->error(), LookupProviderJobBase::InvalidConfiguration);
    }
};

QTEST_MAIN(EmailProviderLookupJobTest)

#include "emailproviderlookupjobtest.moc"
