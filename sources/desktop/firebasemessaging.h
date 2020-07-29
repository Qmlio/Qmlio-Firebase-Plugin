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

#ifndef FIREBASEMESSAGING_H
#define FIREBASEMESSAGING_H
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QObject>
namespace qmlio {
namespace googleFirebase {
class FirebaseMessaging : public QObject {
  Q_PROPERTY(QString firebaseUrl READ firebaseUrl WRITE setFirebaseUrl NOTIFY
                 firebaseUrlChanged)
  Q_OBJECT
public:
  explicit FirebaseMessaging(QObject *parent = nullptr);
  void setError(const int &errorCode, const QString errormsg);
  ~FirebaseMessaging();
  QString firebaseUrl() const;
  void setFirebaseUrl(const QString &firebaseUrl);

signals:
  void initialized(bool ready);
  void errorChanged(const int &errorCode, const QString errormsg);
  void tokenRecieved(const QString &token);
  void messageRecieved(const QVariantMap message);
  void messageSent(bool sent);
  void firebaseUrlChanged();

public slots:
  void send(const QVariant &data);
  void subscribeToTopic(const QString &topic);

protected:
  QNetworkRequest *findRequest(QNetworkReply *);

private:
  QString mFirebaseUrl;
  bool isReady;
  QNetworkAccessManager *manager;
};
} // namespace googleFirebase
} // namespace qmlio
#endif // FIREBASEMESSAGING_H
