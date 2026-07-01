/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include <QNetworkAccessManager>

/*!
 * \brief A very simple stub server to simulate http requests.
 *
 * The test should add responses before using it. If url in the response is not found, the request will return an error 404.
 * An example:
        StubNetworkAccessManager stub{};
        stub.addResponse(QStringLiteral("https://example.com"), "some data");

        // reply will have http status code 200
        auto replyOk = stub.get(new QNetworkRequest("https://example.com"));
        // will return 404
        auto replyKO = stub.get(new QNetworkRequest("https://false.com"))

        // If http status code is different from 200, the signal errorOccurred will be emitted.
 */
class StubNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    struct Response {
        QUrl url;
        QByteArray data;
        int statusCode = 200;
    };

    typedef QList<Response> Responses;

    void addResponses(const Responses &responses);
    void addResponse(const Response &response);

protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData) override;

private:
    QHash<QString, Response> m_responses;
};
