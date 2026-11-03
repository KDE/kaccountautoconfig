/*
  SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "autoconfigparser_p.h"
#include "kaccountautoconfigserver.h"

namespace AutoconfigServerHelper
{
KAccountAutoconfigServer
createServerFromParserEntry(const AutoConfigParser::Server &server, KAccountAutoconfigServer::Protocol protocol, const QString &authenticationType);
std::optional<KAccountAutoconfigServer::Protocol> protocolFromString(const QString &protocolStr);
}
