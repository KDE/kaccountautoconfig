/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once
#include "lookupproviderjobbase_p.h"

class LocalFileProviderLookupJob : public LookupProviderJobBase
{
    Q_OBJECT

public:
    explicit LocalFileProviderLookupJob(const QString &email, QObject *parent = nullptr);
    ~LocalFileProviderLookupJob() override;

private:
    QString m_path;
    /*!
     * string substitution for a local file path.
     */
    QHash<QString, QString> m_pathSubstitutions;
    void lookup() override;
    int lookupFile(const QString &path);
};
