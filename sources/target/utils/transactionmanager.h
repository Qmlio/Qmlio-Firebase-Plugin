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
#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QNetworkRequest>
#include <QObject>

class TransactionManager : public QObject {
  Q_OBJECT

public:
  explicit TransactionManager(QObject *parent = nullptr);
  enum States {
    COMPLETE = 0,
    TIMEOUT = QNetworkReply::TimeoutError,
    CONNECTIONLOST = QNetworkReply::HostNotFoundError,
    UNKNOWNERROR = QNetworkReply::UnknownNetworkError,
    INVALID = UNKNOWNERROR + 1
  };
  inline static int instances() { return objs; }

  enum Type { POST, GET };
  QNetworkAccessManager *getManager() const;
  void doTransaction(Type type, const QNetworkRequest request,
                     const QByteArray &data = QByteArray());

protected:
  virtual void transactionComplete(QNetworkReply *) = 0;

signals:
  void finished(bool, TransactionManager *, QNetworkReply *);

private:
  QNetworkAccessManager *manager;
  static int objs;
};

#endif // TRANSACTIONMANAGER_H
