#ifndef DATABASEQUERY_H
#define DATABASEQUERY_H

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

#include "utils/task.h"
#include "utils/taskmanager.h"

#include <QObject>
#include <QSharedPointer>
#include <QStringList>
#include <firebasedatabase.h>

namespace qmlio {
namespace googleFirebase {

class DatabaseQuery : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged)
  Q_PROPERTY(FirebaseDatabase *database READ database WRITE setDatabase NOTIFY
                 databaseChanged)
  Q_PROPERTY(
      int priority READ priority WRITE setPriority NOTIFY priorityChanged)
public:
  explicit DatabaseQuery(QObject *parent = nullptr);

  ~DatabaseQuery();
  inline QString tag() const { return m_tag; }

  void setTag(const QString &tg);
  FirebaseDatabase *database() const;
  void setDatabase(FirebaseDatabase *store);
  int priority() const;
  void setPriority(int p);

signals:
  void tagChanged();
  void databaseChanged();
  void priorityChanged();
  void completed(int type, QByteArray value);
  void childAdded(const QVariant value, const QString previousChild);
  void childChanged(const QVariant value, const QString previousChild);
  void childMoved(const QVariant value, const QString previousChild);
  void childRemoved(const QVariant value);
  void valueChanged(const QVariant value);
  void databaseReady(bool isReady);

public slots:
  qmlio::googleFirebase::DatabaseQuery *child(QString str);
  QStringList currentPath() const;
  void setValue(QVariant value);
  void getValue(bool isQuery = false);
  qmlio::googleFirebase::DatabaseQuery *pushChild();
  void removeValue();
  QString key() const;
  void setValueAsMap(QVariantMap map);

  void addChildListner();
  void addValueListner();

  qmlio::googleFirebase::DatabaseQuery *orderByChild(const QString &path);
  qmlio::googleFirebase::DatabaseQuery *orderByKey();
  qmlio::googleFirebase::DatabaseQuery *orderByPriority();
  qmlio::googleFirebase::DatabaseQuery *orderByValue();

  qmlio::googleFirebase::DatabaseQuery *startAt(const QVariant &order_value,
                                                QString key = "null");

  qmlio::googleFirebase::DatabaseQuery *endAt(const QVariant &order_value);
  qmlio::googleFirebase::DatabaseQuery *equalTo(const QVariant &order_value);
  qmlio::googleFirebase::DatabaseQuery *limitToFirst(quint32 limit);
  qmlio::googleFirebase::DatabaseQuery *limitToLast(quint32 limit);

protected slots:

  void initializeReference();
  void finished(Task *tsk);
  void OnChildAdded(const QVariant &value, const QString &previous_sibling);
  void OnChildChanged(const QVariant &value, const QString &previous_sibling);
  void OnChildMoved(const QVariant &value, const QString &previous_sibling);
  void OnChildRemoved(const QVariant &value);
  void OnValueChanged(const QVariant &value);
  bool isDatabaseValid();
  void attachNotifiers(bool ready);

private:
  FirebaseDatabase *m_Databse;
  QString m_tag;
  QString currentKey;
  TaskManager *mTaskManager;
  QStringList m_path;
  int m_priority;
  QString mPushedkey;
  bool isInitialised;
};
} // namespace googleFirebase
} // namespace qmlio
#endif // DATABASEQUERY_H
