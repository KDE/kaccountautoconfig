/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KCompositeJob>

#include "kautoconfigserver.h"

#include <kaccountautoconfig_export.h>

class QNetworkAccessManager;
class KAccountAutoconfigJobPrivate;

class KACCOUNTAUTOCONFIG_EXPORT KAccountAutoconfigJob : public KCompositeJob
{
    Q_OBJECT

    friend class KAccountAutoconfigJobPrivate;

public:
    explicit KAccountAutoconfigJob(QObject *parent = nullptr);
    explicit KAccountAutoconfigJob(QNetworkAccessManager *networkAccessManager, QObject *parent = nullptr);

    ~KAccountAutoconfigJob() override;

    void start() override;

    [[nodiscard]] QString email() const;
    void setEmail(const QString &newEmail);

    /*!
     * \brief Get all servers ordered by priority.
     *
     * The client should carefully handle the order.
     * For example, a POP3 server should be preferred over an IMAP server if the POP3 server is first in the list.
     * Note also that multiple servers can be returned for a given protocol and the order matters also in this case.
     */
    [[nodiscard]] QList<KAutoconfigServer> servers() const;

private:
    void slotResult(KJob *job) override;

    std::unique_ptr<KAccountAutoconfigJobPrivate> d;
};
