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

#include "firebasedatabase.h"
#include "databasequery.h"
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <cmath>
#include <memory.h>
#include <random>
#define ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"

using namespace qmlio::googleFirebase;

FirebaseDatabase::FirebaseDatabase(QObject *parent)
    : QObject(parent), m_persist(false), m_prefix('Q'), dbReady(false) {

  service = FirebaseUtils::getInstance().get();
  connect(&mTimer, &QTimer::timeout, this, [=] {
    if (dbReady) {

      mTimer.stop();
    }
  });
  //  mTimer.start(500);
  connect(service, &FirebaseUtils::readyChanged, this, &FirebaseDatabase::init);
}

FirebaseDatabase::~FirebaseDatabase() {}

FirebaseDatabase *FirebaseDatabase::getInstance() {
  static QSharedPointer<FirebaseDatabase> self(new FirebaseDatabase);
  return self.data();
}

FirebaseUtils *FirebaseDatabase::getService() const { return this->service; }

void *FirebaseDatabase::getDatabase() const { return nullptr; }

bool FirebaseDatabase::persist() const { return this->m_persist; }

void FirebaseDatabase::setPersist(bool val) {
  if (val != m_persist) {
    mTimer.start(500);
    emit persistChanged();
  }
}

void FirebaseDatabase::setError(int errorCode, QString errorMsg, QString tag) {
  emit errorChanged(errorCode, errorMsg, tag);
}

QChar FirebaseDatabase::prefix() const { return this->m_prefix; }

QString FirebaseDatabase::generateKey(int size) {

  QString rString;
  fillKey(rString, size);
  return rString;
}
void FirebaseDatabase::fillKey(QString &tank, int size) {
  srand(static_cast<unsigned>(QDateTime().currentMSecsSinceEpoch()));
  QString rString;
  int i = 0;
  while (i < size) {
    int temp = rand() % 62;
    tank.append(ALPHA[temp]);
    i++;
  }
}

void FirebaseDatabase::setPrefix(QChar p) {
  if (p != m_prefix) {
    this->m_prefix = p;
    emit prefixChanged();
  }
}

Task *FirebaseDatabase::createTask(FirebaseDatabase::Types _type,
                                   firebase::FutureBase future, QString tag,
                                   int priority) {

  Task *tsk = Task::createNewTask(FirebaseUtils::Database, _type, future, tag,
                                  priority);
  return tsk;
}

QVariant FirebaseDatabase::fromQVariant(const QVariant &) { return QVariant(); }

QVariantList FirebaseDatabase::fromDatabaseSnapshot(const QVariant &t) {

  return QVariantList();
}

QJsonDocument FirebaseDatabase::fromFirebaseDoc(const QVariant &) {

  return QJsonDocument();
}

QQmlListProperty<DatabaseQuery> FirebaseDatabase::queries() {
  return QQmlListProperty<DatabaseQuery>(this, nullptr,
                                         &FirebaseDatabase::append_slice,
                                         nullptr, nullptr, nullptr);
}

QString FirebaseDatabase::key() {

  QString rString;
  fillKey(rString, 22);
  FirebaseUtils::wait();
  return rString.insert(0, m_prefix);
}

qint64 FirebaseDatabase::timeStamp() {
  QDateTime t;
  return t.currentMSecsSinceEpoch();
}

qreal FirebaseDatabase::truncate(double &d, unsigned numberOfDecimalsToKeep) {
  float d1 = roundf(static_cast<float>(d) * powf(10, numberOfDecimalsToKeep)) /
             powf(10, numberOfDecimalsToKeep);

  return static_cast<qreal>(d1);
}

QVariantList FirebaseDatabase::fromJsonObjectToArray(const QVariant &json) {
  QVariantList imagesArr;
  auto map1 = json.toMap();
  QMapIterator<QString, QVariant> it(map1);

  while (it.hasNext()) {
    it.next();
    imagesArr.append(it.value());
  }

  return imagesArr;
}

void FirebaseDatabase::init() {
  qDebug() << "initializing database desktop " << this;
}

void FirebaseDatabase::append_slice(QQmlListProperty<DatabaseQuery> *list,
                                    DatabaseQuery *query) {
  FirebaseDatabase *database = qobject_cast<FirebaseDatabase *>(list->object);
  if (database) {

    query->setDatabase(database);
    database->m_queries.append(query);
  }
}
