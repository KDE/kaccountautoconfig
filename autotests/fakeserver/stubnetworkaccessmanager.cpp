/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "stubnetworkaccessmanager.h"

#include <QBuffer>
#include <QNetworkReply>
#include <qendian.h>

/*!
 * Needed as ctor in QNetworkReply is protected
 */
class StubNetworkReply : public QNetworkReply
{
public:
    explicit StubNetworkReply(const QByteArray &data, int statusCode, QObject *parent);

    void abort() override;

protected:
    qint64 readData(char *data, qint64 maxLen) override;

private:
    QByteArray m_data;
    QBuffer m_buffer;
};

StubNetworkReply::StubNetworkReply(const QByteArray &data, int statusCode, QObject *parent)
    : QNetworkReply(parent)
    , m_data(data)
    , m_buffer(&m_data)
{
    // QBuffer need to be open before reading it.
    m_buffer.open(ReadOnly);
    setOpenMode(ReadOnly);
    setAttribute(QNetworkRequest::HttpStatusCodeAttribute, statusCode);
    setFinished(true);

    // to simulate real-life error handling by networkReply, we must emit errorOccurred in subclass
    if (statusCode != 200) {
        setError(UnknownNetworkError, QStringLiteral("Protocol failure"));
        QMetaObject::invokeMethod(this, &QNetworkReply::errorOccurred, Qt::QueuedConnection, error());
    }
    QMetaObject::invokeMethod(this, &QNetworkReply::finished, Qt::QueuedConnection);
}

/*!
 * Virtual, nothing to do
 */
void StubNetworkReply::abort()
{
}

/*!
 * Virtual, simple data copy
 */
qint64 StubNetworkReply::readData(char *data, qint64 maxLen)
{
    return m_buffer.read(data, maxLen);
}

void StubNetworkAccessManager::addResponses(const Responses &responses)
{
    for (const auto &response : responses) {
        addResponse(response);
    }
}

void StubNetworkAccessManager::addResponse(const Response &response)
{
    m_responses.insert(response.url.toString(), response);
}

QNetworkReply *StubNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData)
{
    const QString url = req.url().toString();
    if (m_responses.contains(url)) {
        const auto r = m_responses.find(url);
        return new StubNetworkReply(r->data, r->statusCode, this);
    }
    return new StubNetworkReply({}, 404, this);
}
