/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autoconfigparser_p.h"
#include "lookupproviderjobbase_p.h"

#include <QNetworkAccessManager>
#include <kmacroexpander.h>

LookupProviderJobBase::LookupProviderJobBase(const QString &email, QObject *parent)
    : LookupProviderJobBase(email, nullptr, parent)
{
}

LookupProviderJobBase::LookupProviderJobBase(const QString &email, QNetworkAccessManager *networkAccessManager)
    : LookupProviderJobBase(email, networkAccessManager, networkAccessManager)
{
}

LookupProviderJobBase::~LookupProviderJobBase() = default;

std::optional<AutoConfigParser::ClientConfig> LookupProviderJobBase::configuration() const
{
    return m_configurationData;
}

QString LookupProviderJobBase::source() const
{
    return m_configurationSource;
}

QString LookupProviderJobBase::substitutePlaceholders(const QString &in) const
{
    return KMacroExpander::expandMacros(in, m_placeHolderSubstitutions);
}

std::optional<AutoConfigParser::ClientConfig> LookupProviderJobBase::parse(QIODevice *data)
{
    return AutoConfigParser::parse(data);
}

LookupProviderJobBase::LookupProviderJobBase(const QString &email, QNetworkAccessManager *networkAccessManager, QObject *parent)
    : KJob(parent)
    , m_networkAccessManager(networkAccessManager)
    , m_email(email)
    , m_emailDomain(email.split(u'@').last())
{
    m_placeHolderSubstitutions.insert(QStringLiteral("EMAILDOMAIN"), m_emailDomain);
    m_placeHolderSubstitutions.insert(QStringLiteral("EMAILADDRESS"), m_email);
}

void LookupProviderJobBase::start()
{
    QMetaObject::invokeMethod(this, &LookupProviderJobBase::lookup, Qt::QueuedConnection);
}
