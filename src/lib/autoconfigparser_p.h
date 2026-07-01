/*
    SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QHash>
#include <QUrl>
#include <QXmlStreamReader>
#include <kaccountautoconfig_export.h>

namespace AutoConfigParser
{
struct Documentation {
    QUrl url;
    QHash<QString, QString> descriptionByLanguage;

    bool operator==(const Documentation &other) const = default;
};

struct Authentication {
    QString system;
    QString method;

    bool operator==(const Authentication &other) const = default;
};

struct Server {
    QString type;
    QUrl url;
    QList<Authentication> authentications;
    QString username;
    QString hostname;
    int port;
    QString socketType;

    bool operator==(const Server &other) const = default;
};

struct EmailProvider {
    QString id;

    QStringList domains;
    QString displayName;
    QString displayShortName;

    QList<Documentation> documentations;

    bool operator==(const EmailProvider &other) const = default;
};

struct ClientConfig {
    EmailProvider emailProvider;
    QHash<QString, QList<Server>> serversByProtocol;

    bool operator==(const ClientConfig &other) const = default;
};

KACCOUNTAUTOCONFIG_EXPORT std::optional<ClientConfig> parse(QIODevice *doc);
};

Q_DECLARE_METATYPE(AutoConfigParser::ClientConfig)
