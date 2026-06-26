/*
    SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "autoconfigvalidator.h"
#include "kaccountautoconfig_debug.h"
#include <qloggingcategory.h>

constexpr auto tcpBasedProtocols = std::to_array({
    QLatin1StringView("imap"),
    QLatin1StringView("pop3"),
    QLatin1StringView("smtp"),
});

constexpr auto urlBasedProtocols = std::to_array({
    QLatin1StringView("jmap"),
    QLatin1StringView("carddav"),
    QLatin1StringView("caldav"),
});

bool isValid(const Server &server)
{
    if (server.type == nullptr) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: missing type attribute in server";
        return false;
    }
    if (std::find(tcpBasedProtocols.begin(), tcpBasedProtocols.end(), server.type) != tcpBasedProtocols.end()) {
        if (!server.hostname.isValid() || server.port == 0 || server.socketType.isEmpty() || server.authentications.isEmpty() || server.username.isEmpty()) {
            qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: missing or invalid attributes in" << server.type << "server";
            return false;
        }
    }
    if (std::find(urlBasedProtocols.begin(), urlBasedProtocols.end(), server.type) != urlBasedProtocols.end()) {
        if (!server.url.isValid() || server.url.isRelative() || server.authentications.isEmpty() || server.username.isEmpty()) {
            qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: missing or invalid attributes in" << server.type << "server";
            return false;
        }
    }
    for (const auto &authentication : server.authentications) {
        if (authentication.method.isEmpty()) {
            qCWarning(KACCOUNTAUTOCONFIG_LOG()) << "Malformed XML: empty authentication tag in" << server.type << "server";
            return false;
        }
    }
    // Completely ignore unsupported protocols
    return true;
}

bool isValid(const ClientConfig &clientConfig)
{
    // Don't check emailProvider validity, since all servers are in the clientConfig map
    for (const auto &type : clientConfig.serversByProtocol.keys()) {
        for (const auto &server : clientConfig.serversByProtocol[type]) {
            if (!isValid(server)) {
                return false;
            }
        }
    }
    return true;
}
