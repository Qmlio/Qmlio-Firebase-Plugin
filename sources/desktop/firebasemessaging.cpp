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

#include "firebasemessaging.h"
#include <QAuthenticator>
#include <QDebug>
#include <QNetworkReply>
#include <QUrl>
using namespace qmlio::googleFirebase;

FirebaseMessaging::FirebaseMessaging(QObject *parent) : QObject(parent) {
  qDebug() << "intialising Messaging Desktop";
}

void FirebaseMessaging::setError(const int &errorCode, const QString errormsg) {
  emit errorChanged(errorCode, errormsg);
}

FirebaseMessaging::~FirebaseMessaging() { qDebug() << "Deleting Messaging "; }

void FirebaseMessaging::send(const QVariant &) {}

void FirebaseMessaging::subscribeToTopic(const QString &) {}

QNetworkRequest *FirebaseMessaging::findRequest(QNetworkReply *) {
  QNetworkRequest *request = new QNetworkRequest;

  return request;
}

QString FirebaseMessaging::firebaseUrl() const { return mFirebaseUrl; }

void FirebaseMessaging::setFirebaseUrl(const QString &firebaseUrl) {
  if (firebaseUrl != mFirebaseUrl) {
    mFirebaseUrl = firebaseUrl;
    emit firebaseUrlChanged();
  }
}
