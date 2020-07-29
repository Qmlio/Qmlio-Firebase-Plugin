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
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "transactionmanager.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
class Transaction : public TransactionManager {
  Q_OBJECT
public:
  explicit Transaction(QObject *parent = nullptr);
  ~Transaction() override;

  bool lHas(QNetworkRequest);
  QByteArray getData() const;
  void setData(const QByteArray &value);
  void rentry(Type, const QNetworkRequest &);
  void send(const QNetworkRequest &request, Type type = POST);
  bool getCancelTranscation() const;

protected:
  void transactionComplete(QNetworkReply *) override;
  void wait();

private:
  QNetworkRequest reply;
  QByteArray data;
  bool cancelTranscation;
};

#endif // TRANSACTION_H
