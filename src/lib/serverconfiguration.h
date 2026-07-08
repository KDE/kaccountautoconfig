/*
    SPDX-FileCopyrightText: 2026 Alan Thouvenin <ath@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QHash>
#include <QUrl>

struct Documentation {
    QUrl url;
    QHash<QString, QString> descriptionByLanguage;
};

struct Authentication {
    QString system;
    QString method;
};

struct Server {
    QString type;
    QUrl url;
    QList<Authentication> authentications;
    QString username;
    QUrl hostname;
    int port;
    QString socketType;
};

struct EmailProvider {
    QString id;

    QStringList domains;
    QString displayName;
    QString displayShortName;

    QList<Documentation> documentations;
};

struct ClientConfig {
    EmailProvider emailProvider;
    QHash<QString, QList<Server>> serversByProtocol;
};
