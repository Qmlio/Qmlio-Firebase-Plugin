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
#ifndef STORAGEQUERY_H
#define STORAGEQUERY_H

#include "firebase/storage.h"
#include "firebasestorage.h"
#include "utils/taskmanager.h"
#include <QObject>
namespace qmlio {
namespace googleFirebase {

struct ControllerWrapper {
  ControllerWrapper(firebase::storage::Controller *controller = nullptr);
  firebase::storage::Controller *controller();
  ~ControllerWrapper();

private:
  firebase::storage::Controller m_controller;
};

class StorageListner;
class StorageQuery : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged)
  Q_PROPERTY(FirebaseStorage *parentStorage READ parentStorage WRITE
                 setParentStorage NOTIFY parentStorageChanged)
  Q_PROPERTY(FirebaseStorage::Priority priority READ priority WRITE setPriority
                 NOTIFY priorityChanged)
public:
  explicit StorageQuery(QObject *parent = nullptr);
  ~StorageQuery() override;
  QString tag() const { return m_tag; }

  void setTag(const QString &tg);
  FirebaseStorage *parentStorage() const;
  void setParentStorage(FirebaseStorage *store);
  FirebaseStorage::Priority priority() const;
  void setPriority(FirebaseStorage::Priority p);
  bool getIsBusy() const;
  void setIsBusy(bool value);

signals:
  void tagChanged();
  void transferedSizeChanged(qint64 size);
  void totalSizeChanged(qint64 size);
  void resumed(qint64 size);
  void paused(qint64 size);
  void cancelled();
  void parentStorageChanged();
  void uploadComplete();
  void complete();
  void downloadComplete(QString path);
  void linkGenerated(QString link);
  void priorityChanged();

public slots:
  qmlio::googleFirebase::StorageQuery *child(QString val);
  QStringList currentPath() const;
  void putFile(QVariant file);

  void pause();
  void resume();
  void cancel();
  bool isPaused();
  bool isResumed();
  void getLink(QString filename);
  void downloadFile(QString path);

protected slots:
  void finished(Task *task);
  void initRef();

private:
  QStringList m_pathHolder;
  firebase::storage::Controller m_controller;
  ::firebase::storage::StorageReference ref;
  FirebaseStorage::Priority m_priority;
  FirebaseStorage *m_Storage;
  StorageListner *listner;
  bool m_Resumed;
  bool isBusy;
  QString currentKey;
  QString m_tag;
  QString downloadPath;
  TaskManager *mTaskManager;
};

} // namespace googleFirebase
} // namespace qmlio
Q_DECLARE_METATYPE(qmlio::googleFirebase::ControllerWrapper);
#endif // STORAGEQUERY_H
