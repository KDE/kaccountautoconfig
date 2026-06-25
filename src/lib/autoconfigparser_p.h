/*
    SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "serverconfiguration.h"
#include <kaccountautoconfig_export.h>
#include <qiodevice.h>

KACCOUNTAUTOCONFIG_EXPORT std::optional<ClientConfig> doParse(QIODevice *doc);
