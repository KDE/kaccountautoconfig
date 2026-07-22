/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include <KJob>

#include <qhash.h>

#include "autoconfigparser_p.h"

class QNetworkAccessManager;
class QUrl;
/*!
 *\brief Base class for every lookup provider job.
 *
 * Every provider implementation should implement the lookup() method.
 * The provider is also responsible for knowing when its job is done, meaning it MUST handle the parsing response
 */
class LookupProviderJobBase : public KJob
{
    Q_OBJECT

public:
    enum Error {
        /*!
         * The job could not find the configuration or parse the configuration
         */
        InvalidConfiguration = UserDefinedError,
    };

    explicit LookupProviderJobBase(const QString &email, QNetworkAccessManager *networkAccessManager);
    explicit LookupProviderJobBase(const QString &email, QObject *parent = nullptr);
    ~LookupProviderJobBase() override;

    [[nodiscard]] std::optional<AutoConfigParser::ClientConfig> configuration() const;
    [[nodiscard]] QString source() const;
    void start() override;

protected:
    void addPlaceHolderSubstitution(const QString &placeholder, const QString &value);

    std::optional<AutoConfigParser::ClientConfig> m_configurationData;
    QString m_configurationSource;
    QString m_email;
    QString m_emailDomain;

    QNetworkAccessManager *const m_networkAccessManager;

    /*!
     * Substitute placeholders in the given QString string with actual values
     * based on the email address and domain stored in this job.
     *
     * By convention, %{EMAILADDRESS} will be replaced by the full email address,
     * and %{EMAILDOMAIN} will be replaced by the email domain.
     */
    [[nodiscard]] QString substitutePlaceholders(const QString &in) const;

    static std::optional<AutoConfigParser::ClientConfig> parse(QIODevice *data);

    virtual void lookup() = 0;

private:
    LookupProviderJobBase(const QString &email, QNetworkAccessManager *networkAccessManager, QObject *parent);

    QHash<QString, QString> m_placeHolderSubstitutions;
};
