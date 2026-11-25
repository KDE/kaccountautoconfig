/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KCompositeJob>

#include "kaccountautoconfigserver.h"

#include <kaccountautoconfig_export.h>

class QNetworkAccessManager;
class KAccountAutoconfigJobPrivate;

/*!
 * \class KAccountAutoconfigJob
 * \inmodule KAccountAutoconfig
 * \brief This class provides a job that, given an email, retrieves a list of KAccountAutoconfigServer.
 *
 * This job performs lookups from various providers, stopping at the first successful one, and returns all servers defined in the configuration xml that was
 * fetched. The providers are as defined in the RFC
 * https://www.ietf.org/archive/id/draft-ietf-mailmaint-autoconfig-04.html#name-configuration-retrieval-by-mail-clients
 */
class KACCOUNTAUTOCONFIG_EXPORT KAccountAutoconfigJob : public KCompositeJob
{
    Q_OBJECT

    friend class KAccountAutoconfigJobPrivate;

public:
    /*!
     * \enum KAccountAutoconfigJob::Error
     * \value ConfigurationNotFound No valid configuration was found from any providers.
     */
    enum Error {
        ConfigurationNotFound = UserDefinedError,
    };

    explicit KAccountAutoconfigJob(QObject *parent = nullptr);
    /*!
     * If needed, provide a custom network access manager to use in the sub-jobs.
     * Most likely this should only be used for testing purposes and you likely should use the other constructor.
     * \a networkAccessManager the network access manager to use
     * \a parent the parent object
     */
    explicit KAccountAutoconfigJob(QNetworkAccessManager *networkAccessManager, QObject *parent = nullptr);

    ~KAccountAutoconfigJob() override;

    /*!
     * Starts iterating over different providers and stops when a valid configuration is found.
     * When the job is done, the KJob::result() signal is emitted and the results are available through the servers() method.
     */
    void start() override;

    /*!
     * Accessor to the current email used for lookup.
     */
    [[nodiscard]] QString email() const;
    /*!
     * Provides a new email to perform lookup on to \a newEmail
     */
    void setEmail(const QString &newEmail);

    /*!
     * \brief Get all servers ordered by priority.
     *
     * The client should carefully handle the order.
     * For example, a POP3 server should be preferred over an IMAP server if the POP3 server is first in the list.
     * Note also that multiple servers can be returned for a given protocol and the order matters also in this case.
     */
    [[nodiscard]] QList<KAccountAutoconfigServer> servers() const;

private:
    void slotResult(KJob *job) override;

    std::unique_ptr<KAccountAutoconfigJobPrivate> d;
};
