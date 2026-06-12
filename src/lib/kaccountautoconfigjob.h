/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KJob>

#include "kautoconfigserver.h"

#include <kaccountautoconfig_export.h>

class KAccountAutoconfigJobPrivate;

class KACCOUNTAUTOCONFIG_EXPORT KAccountAutoconfigJob : public KJob
{
    Q_OBJECT

public:
    explicit KAccountAutoconfigJob(QObject *parent = nullptr);

    ~KAccountAutoconfigJob() override;

    void start() override;

    [[nodiscard]] QString password() const;
    void setPassword(const QString &newPassword);

    [[nodiscard]] QString email() const;
    void setEmail(const QString &newEmail);

    /*!
     * \brief Get all servers.
     *
     * Note that by design, The job maintains one server for each protocol. Initially, all servers are created with Status::None, indicating no action has been
     * performed yet.
     */
    [[nodiscard]] QList<KAutoconfigServer> servers() const;

    /*!
     * Retrieve the server for the given protocol.
     */
    KAutoconfigServer getServer(KAutoconfigServer::Protocol protocol) const;

private:
    std::unique_ptr<KAccountAutoconfigJobPrivate> d;
};
