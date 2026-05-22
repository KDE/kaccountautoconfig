/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KJob>
#include <kaccountautoconfig_export.h>

class KACCOUNTAUTOCONFIG_EXPORT KAccountAutoconfigJob : public KJob
{
    Q_OBJECT

public:
    ~KAccountAutoconfigJob() override;

    void start() override;
};
