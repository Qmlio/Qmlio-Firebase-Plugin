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
#include "utils/platformutils.h"
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
    : QObject(parent), m_persist(false), m_prefix("Q"), dbReady(false) {
  service = FirebaseUtils::instance();
  connect(&mTimer, &QTimer::timeout, this, [=] {
    if (dbReady) {

      database->set_persistence_enabled(this->m_persist);
      emit persistChanged();
      mTimer.stop();
    } else {
    }
  });

  connect(service, &FirebaseUtils::readyChanged, this, &FirebaseDatabase::init);
}

int FirebaseDatabase::queryCount(QQmlListProperty<DatabaseQuery> *mList) {
  return reinterpret_cast<FirebaseDatabase *>(mList->data)->queryCount();
}

DatabaseQuery *FirebaseDatabase::query(QQmlListProperty<DatabaseQuery> *mList,
                                       int mIndex) {
  return reinterpret_cast<FirebaseDatabase *>(mList->data)->query(mIndex);
}

void FirebaseDatabase::clearqueries(QQmlListProperty<DatabaseQuery> *mList) {
  reinterpret_cast<FirebaseDatabase *>(mList->data)->clear();
}

FirebaseDatabase::~FirebaseDatabase() {
  // qDebug() << "deleting database " << this;
  database = nullptr;
}

FirebaseDatabase *FirebaseDatabase::getInstance() {

  static QSharedPointer<FirebaseDatabase> self(new FirebaseDatabase);
  return self.data();
}

firebase::database::Database *FirebaseDatabase::getDatabase() const {
  return this->database;
}

bool FirebaseDatabase::persist() const { return this->m_persist; }

void FirebaseDatabase::setPersist(bool val) {
  if (val != m_persist) {
    m_persist = val;
    // qDebug() << "setting  " << m_persist;
    // mTimer.start(500);
  }
}

void FirebaseDatabase::setError(int errorCode, QString errorMsg, QString tag) {
  emit errorChanged(errorCode, errorMsg, tag);
}

QString FirebaseDatabase::prefix() const { return this->m_prefix; }

QString FirebaseDatabase::generateKey(int size) {

  QString rString;
  fillKey(rString, size);
  return rString;
}

void FirebaseDatabase::setPrefix(QString p) {
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

firebase::Variant FirebaseDatabase::fromQVariant(const QVariant &var) {
  QJsonDocument doc = QJsonDocument::fromJson(var.toString().toUtf8());
  QVariant v(doc.object().toVariantMap());
  firebase::Variant fVariant = FirebaseUtils::fromQtVariant(v);
  return fVariant;
}

QVariantList FirebaseDatabase::fromDatabaseSnapshot(
    const firebase::Future<firebase::database::DataSnapshot> &snap) {
  QVariantList children;
  size_t f = snap.result()->children_count();
  for (size_t i = 0; i < f; i++) {
    auto reference =
        fromFirebaseDoc(snap.result()->children().at(i)).toVariant();
    children.append(reference);
  }
  return children;
}

QJsonDocument FirebaseDatabase::fromFirebaseDoc(
    const firebase::database::DataSnapshot &snapshot) {
  QVariant result = FirebaseUtils::fromFirebaseVariant(snapshot.value());
  QJsonDocument details = QJsonDocument::fromVariant(result);
  return details;
}

QQmlListProperty<DatabaseQuery> FirebaseDatabase::queries() {

  return QQmlListProperty<DatabaseQuery>(
      this, this, &FirebaseDatabase::append_Query,
      &FirebaseDatabase::queryCount, &FirebaseDatabase::query,
      &FirebaseDatabase::clearqueries);
}

int FirebaseDatabase::queryCount() const { return m_queries.count(); }

DatabaseQuery *FirebaseDatabase::query(int index) const {
  return this->m_queries.at(index);
}

void FirebaseDatabase::clear() { this->m_queries.clear(); }

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
  qDebug() << "initialising database Android " << this;
  if (!PlatformUtils::getNativeWindow()) {
    emit databaseInitialized(false);
    return;
  }

  beforeInitialised();
  if (GooglePlayServices::getAvailability() ==
      GooglePlayServices::AvailabilityAvailable) {

    firebase::InitResult result;
    database = firebase::database::Database::GetInstance(
        service->firebaseNativeApp(), &result);

    if (result == firebase::kInitResultSuccess) {
      dbReady = true;

      database->set_persistence_enabled(m_persist);

      emit databaseInitialized(true);

    } else if (result != firebase::kInitResultSuccess) {
      setError(firebase::kInitResultFailedMissingDependency,
               "Please update your google play services");
      emit databaseInitialized(false);
      dbReady = false;
    }
  }
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

void FirebaseDatabase::append_Query(QQmlListProperty<DatabaseQuery> *list,
                                    DatabaseQuery *query) {
  FirebaseDatabase *database = qobject_cast<FirebaseDatabase *>(list->object);
  if (database) {

    query->setDatabase(getInstance());
    database->m_queries.append(query);
  }
}
