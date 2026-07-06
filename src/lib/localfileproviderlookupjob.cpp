/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "localfileproviderlookupjob_p.h"

#include <QUrl>
#include <qfile.h>
#include <qfileinfo.h>
#include <qstandardpaths.h>

#include "kaccountautoconfig_debug.h"

LocalFileProviderLookupJob::LocalFileProviderLookupJob(const QString &email, QObject *parent)
    : LookupProviderJobBase(email, parent)
{
}

LocalFileProviderLookupJob::~LocalFileProviderLookupJob() = default;

void LocalFileProviderLookupJob::lookup()
{
    // configuration file should be in a isp directory according to the rfc
    const auto subPath = QStringLiteral("/isp/") + m_emailDomain + QStringLiteral(".xml");
    int lastError = NoError;

    for (const auto &dir : QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)) {
        qCInfo(KACCOUNTAUTOCONFIG_LOG) << "Looking for configuration files in:" << dir;
        lastError = lookupFile(dir + subPath);
        if (lastError == NoError) {
            emitResult();
            return;
        }
    }
    setError(lastError);
    emitResult();
}

int LocalFileProviderLookupJob::lookupFile(const QString &path)
{
    qCInfo(KACCOUNTAUTOCONFIG_LOG) << "Looking Configuration into local file with path" << path;
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        return ConfigurationNotFound;
    }
    qCInfo(KACCOUNTAUTOCONFIG_LOG) << "Configuration found for path" << path;
    const auto data = parse(&file);
    if (!data) {
        qCWarning(KACCOUNTAUTOCONFIG_LOG) << "Configuration invalid for path" << path;
        return ConfigurationParsingError;
    }
    m_configurationData = data.value();
    m_configurationSource = path;
    qCInfo(KACCOUNTAUTOCONFIG_LOG) << "Configuration parsed for path" << path;
    return NoError;
}
