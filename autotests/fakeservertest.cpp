/*
   SPDX-FileCopyrightText: 2026 Arnaud Chirat <aci@enioka.com>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QSignalSpy>

#include "fakeserver/stubnetworkaccessmanager.h"

#include <QTest>

class FakeServerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testFakeServer()
    {
        QUrl url(QStringLiteral("https://example.com"));
        StubNetworkAccessManager stub;
        stub.addResponse({url, "some data"});
        QSignalSpy spyManagerFinished(&stub, &StubNetworkAccessManager::finished);

        // request with an url present in the stub
        const auto reply = stub.get(QNetworkRequest(QUrl(url)));
        QSignalSpy spyReplyFinished(reply, &QNetworkReply::finished);
        QSignalSpy spyReplyErrorOccurred(reply, &QNetworkReply::errorOccurred);

        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QCOMPARE(statusCode, 200);
        QCOMPARE(reply->readAll(), QByteArrayLiteral("some data"));

        // check signals
        QTRY_COMPARE(spyReplyFinished.count(), 1);
        QTRY_COMPARE(spyReplyErrorOccurred.count(), 0);

        reply->deleteLater();

        // request with an url not present in the stub
        const auto replyKo = stub.get(QNetworkRequest(QUrl(QStringLiteral("https://false.com"))));
        QSignalSpy spyReplyKoFinished(replyKo, &QNetworkReply::finished);
        QSignalSpy spyReplyKoErrorOccurred(replyKo, &QNetworkReply::errorOccurred);

        const auto statusCodeKO = replyKo->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QCOMPARE(statusCodeKO, 404);
        QCOMPARE(replyKo->readAll(), QByteArrayLiteral(""));
        // check signals
        QTRY_COMPARE(spyReplyKoFinished.count(), 1);
        QTRY_COMPARE(spyReplyKoErrorOccurred.count(), 1);

        replyKo->deleteLater();

        // check manager signal, one finished per call
        QTRY_COMPARE(spyManagerFinished.count(), 2);
    }
};

QTEST_MAIN(FakeServerTest)

#include "fakeservertest.moc"
