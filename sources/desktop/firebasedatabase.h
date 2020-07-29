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

#ifndef FIREBASEDATABASE_H
#define FIREBASEDATABASE_H

#include "utils/firebaseutils.h"
#include <QObject>
#include <QQmlListProperty>
#include <QThread>

namespace qmlio {
namespace googleFirebase {
class DatabaseQuery;
class FirebaseDatabase : public QObject {
  Q_PROPERTY(bool persist READ persist WRITE setPersist NOTIFY persistChanged)
  Q_PROPERTY(QChar prefix READ prefix WRITE setPrefix NOTIFY prefixChanged)
  Q_PROPERTY(QQmlListProperty<qmlio::googleFirebase::DatabaseQuery> queries READ
                 queries)
  Q_OBJECT

public:
  ~FirebaseDatabase() override;
  static FirebaseDatabase *getInstance();
  FirebaseUtils *getService() const;
  void *getDatabase() const;
  bool persist() const;
  void setPersist(bool val);
  void setError(int errorCode, QString errorMsg, QString tag = "database");
  QChar prefix() const;
  QString generateKey(int size);
  void setPrefix(QChar p);
  enum Types {
    SetValue,
    UpdateValue,
    RemoveValue,
    GetValue

  };

  enum Priority {
    Idle = QThread::IdlePriority,
    Lowest = QThread::LowestPriority,
    Low = QThread::LowPriority,
    Normal = QThread::NormalPriority,
    High = QThread::HighPriority,
    Highest = QThread::HighestPriority,
    Critictal = QThread::TimeCriticalPriority,

  };
  Q_ENUM(Priority)
  static Task *createTask(Types t, firebase::FutureBase future,
                          QString tag = "database", int p = Normal);
  static QVariant fromQVariant(const QVariant &var);
  static QVariantList fromDatabaseSnapshot(const QVariant &snapshot);
  QJsonDocument static fromFirebaseDoc(const QVariant &snapshot);
  QQmlListProperty<DatabaseQuery> queries();

signals:
  void persistChanged();
  void prefixChanged();
  void errorChanged(int errorCode, QString errorMsg, QString tag);
  void databaseInitialized(bool ready);
  void beforeInitialised();

public slots:
  QString key();
  qint64 timeStamp();
  qreal truncate(double &d, unsigned numberOfDecimalsToKeep);
  QVariantList fromJsonObjectToArray(const QVariant &json);

protected slots:
  void init();
  void fillKey(QString &tank, int size);

protected:
  explicit FirebaseDatabase(QObject *parent = nullptr);

private:
  static void append_slice(QQmlListProperty<DatabaseQuery> *list,
                           DatabaseQuery *slice);

  bool dbReady, isPersistent;
  QList<DatabaseQuery *> m_queries;
  FirebaseUtils *service;
  QString userUId;
  QTimer mTimer;
  bool m_persist;
  QChar m_prefix;
  // bool requestPersist;
};
} // namespace googleFirebase
} // namespace qmlio
#endif // FIREBASEDATABASE_H
