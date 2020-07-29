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
  utils = FirebaseUtils::instance();
  listener = new MessageListener;
  mTaskManager = new TaskManager;
  manager = new QNetworkAccessManager;
  connect(utils, &FirebaseUtils::readyChanged, this, [=]() {
    qDebug() << "intiailizing Messaging Android ";

    auto result =
        firebase::messaging::Initialize(*utils->firebaseNativeApp(), listener);
    this->completeIntialisation(result);
  });
  connect(mTaskManager, &TaskManager::complete, this,
          &FirebaseMessaging::finished);
  connect(listener, &MessageListener::tokenRecieved, this,
          [this](const QString &token) { this->tokenRecieved(token); });

  connect(listener, &MessageListener::messageRecieved, this,
          [=](const QVariantMap &data) { this->messageRecieved(data); });
}

void FirebaseMessaging::completeIntialisation(firebase::InitResult result) {
  switch (result) {
  case firebase::kInitResultSuccess: {
    isReady = true;

    break;
  }
  case firebase::kInitResultFailedMissingDependency: {
    setError(0, "Missing dependancy");
    isReady = false;
    break;
  }
  }
  emit initialized(isReady);
}

void FirebaseMessaging::setError(const int &errorCode, const QString errormsg) {
  emit errorChanged(errorCode, errormsg);
}

void FirebaseMessaging::connetToLisetner() {}

FirebaseMessaging::~FirebaseMessaging() { firebase::messaging::Terminate(); }

void FirebaseMessaging::send(const QVariant &data) {

  if (AUTH_KEY == "") {
    qWarning() << "FirebaseMessaging::send "
               << " Please set your Authorization key";
    return;
  }
  QNetworkRequest request;
  QByteArray key(AUTH_KEY);

  request.setUrl(QUrl("https://fcm.googleapis.com/fcm/send"));
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  request.setRawHeader("Authorization", key);

  Transaction *transction = new Transaction;
  transction->setData(QByteArray().append(data.toString()));
  transction->send(request);

  requestManager[TransactionManager::instances()] = transction;

  connect(transction, &TransactionManager::finished, this,
          &FirebaseMessaging::posted);
}

void FirebaseMessaging::subscribeToTopic(const QString &topic) {
  if (!topic.isEmpty()) {
    auto future = firebase::messaging::Subscribe(topic.toUtf8());
    Task *task = createNewTask(future);
    mTaskManager->addTask(task);
  }
}

void FirebaseMessaging::finished(Task *task) {
  if (task) {
    if (task->getFuture().error() == firebase::kFutureStatusComplete) {
    } else {
      qDebug() << "is error  " << task->getFuture().error_message();
    }
  }
}

void FirebaseMessaging::posted(bool b, TransactionManager *m) {

  remove(m);
  emit messageSent(b);
}

firebase::messaging::Message FirebaseMessaging::serialize(QVariantMap data) {
  return firebase::messaging::Message();
}

Task *FirebaseMessaging::createNewTask(firebase::FutureBase future) {
  return Task::createNewTask(FirebaseUtils::Messaging, 0, future,
                             "firebase::Messaging", QThread::NormalPriority);
}

QNetworkRequest *FirebaseMessaging::findRequest(QNetworkReply *reply) {
  QNetworkRequest *request = new QNetworkRequest;

  return request;
}

void FirebaseMessaging::remove(TransactionManager *reply) {
  Transaction *t = nullptr;
  int i = 0;
  QHashIterator<int, TransactionManager *> it(requestManager);
  while (it.hasNext()) {
    it.next();
    if (it.value() == reply) {
      t = dynamic_cast<Transaction *>(it.value());
      break;
    }
    i++;
  }

  if (t) {
    requestManager.remove(i);
    t->deleteLater();
  }
}

QString FirebaseMessaging::firebaseUrl() const { return mFirebaseUrl; }

void FirebaseMessaging::setFirebaseUrl(const QString &firebaseUrl) {
  if (firebaseUrl != mFirebaseUrl) {
    mFirebaseUrl = firebaseUrl;
    emit firebaseUrlChanged();
  }
}
