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
#ifndef FIREBASESTORAGE_H
#define FIREBASESTORAGE_H

#include "firebase/storage.h"
#include "utils/firebaseutils.h"
#include <QObject>
#include <QQmlListProperty>
#include <QSharedPointer>
#include <QThread>

namespace qmlio {
namespace googleFirebase {
class StorageQuery;
class FirebaseStorage : public QObject {
  Q_OBJECT
  Q_PROPERTY(QQmlListProperty<qmlio::googleFirebase::StorageQuery> queries READ
                 queries)
public:
  bool checkPermission();
  static FirebaseStorage *getInstance();
  ~FirebaseStorage() override;

  enum { Upload, Download, Link };
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

  bool isReady() const;
  firebase::storage::StorageReference getNativeReference() const;
  QQmlListProperty<StorageQuery> queries();
  int queryCount() const;
  StorageQuery *query(int index) const;
  void clear();
  firebase::storage::Storage *getStorage() const;
  void setError(int errorCode, QString errorMsg, QString tag = "storage");

signals:
  void errorChanged(int errorCode, QString errorMsg, QString tag);
  void initialised(bool initialised);

protected:
  explicit FirebaseStorage(QObject *parent = nullptr);

protected slots:
  void init();

private:
  static void append_Query(QQmlListProperty<StorageQuery> *list,
                           StorageQuery *slice);
  static int queryCount(QQmlListProperty<StorageQuery> *);
  static StorageQuery *query(QQmlListProperty<StorageQuery> *, int);
  static void clearqueries(QQmlListProperty<StorageQuery> *);

  static FirebaseStorage *self;
  bool m_ready;
  FirebaseUtils *firebaseUtils;
  QList<StorageQuery *> m_queries;
  firebase::storage::Storage *storage;
};
} // namespace googleFirebase
} // namespace qmlio
#endif // FIREBASESTORAGE_H
