/*
  SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "autoconfigparser_p.h"
#include "kautoconfigserver.h"

namespace AutoconfigServerHelper
{
KAutoconfigServer createServerFromParserEntry(const AutoConfigParser::Server &server, KAutoconfigServer::Protocol protocol, const QString &authenticationType);
std::optional<KAutoconfigServer::Protocol> protocolFromString(const QString &protocolStr);
}
