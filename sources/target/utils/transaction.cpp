/* Copyright (c) 2020 Qmlio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "transaction.h"
#include <QEventLoop>
#include <QHashIterator>
#include <QTimer>
void Transaction::wait() {
  QEventLoop loop;
  QTimer::singleShot(5000, &loop, &QEventLoop::quit);
  loop.exec();
}
Transaction::Transaction(QObject *parent)
    : TransactionManager(parent), cancelTranscation(false) {}

Transaction::~Transaction() { qDebug() << "deleting transaction " << this; }

void Transaction::transactionComplete(QNetworkReply *reply) {

  if (reply->error() != QNetworkReply::NoError)
    rentry(POST, reply->request());

  else
    emit finished(true, this, reply);
}

QByteArray Transaction::getData() const { return data; }

void Transaction::setData(const QByteArray &value) { data = value; }

void Transaction::rentry(Type type, const QNetworkRequest &reply) {
  static int trials = 0;
  if (trials == 500) {
    qWarning() << "tried enough times quiting ";
    cancelTranscation = true;
    finished(false, this, nullptr);
  }
  this->wait();

  if (!cancelTranscation) {
    qWarning() << "trying again";
    doTransaction(type, reply, data);
  }
  trials++;
}

void Transaction::send(const QNetworkRequest &request, Type type) {
  doTransaction(type, request, data);
}

bool Transaction::getCancelTranscation() const { return cancelTranscation; }
