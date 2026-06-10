/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "kautoconfigserver.h"

#include <KJob>
#include <kaccountautoconfig_export.h>

class KAccountAutoconfigJobPrivate;

class KACCOUNTAUTOCONFIG_EXPORT KAccountAutoconfigJob : public KJob
{
    Q_OBJECT

public:
    explicit KAccountAutoconfigJob(const QString &email, const QString &password, QObject *parent = nullptr);

    ~KAccountAutoconfigJob() override;

    void start() override;

    [[nodiscard]] QString password() const;
    [[nodiscard]] QString email() const;

    /*!
     * Get all servers retrieved by the job.
     * By design the job can only have one server per protocol @see KAutoconfigServer::Protocol.
     * @return
     */
    [[nodiscard]] QList<KAutoconfigServer> servers() const;

    /*!
     * Retrieve the server for the given protocol.
     */
    KAutoconfigServer getServer(KAutoconfigServer::Protocol protocol) const;

private:
    std::unique_ptr<KAccountAutoconfigJobPrivate> d;
};
