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
#include "storagequery.h"
#include "storagelistner.h"
#include "utils/task.h"
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QGuiApplication>
#include <QTimer>
#define TASK_ID FirebaseUtils::Storage
using namespace qmlio::googleFirebase;
namespace cloudReference = ::firebase::storage;

qmlio::googleFirebase::StorageQuery::StorageQuery(QObject *parent)
    : QObject(parent), m_Resumed(false), isBusy(false) {
  m_Storage = nullptr;
  m_priority = FirebaseStorage::Normal;
  mTaskManager = new TaskManager;
  listner = new StorageListner(this);
  connect(mTaskManager, &TaskManager::complete, this, &StorageQuery::finished);
  connect(listner, &StorageListner::progress, this,
          [=](ControllerWrapper *control) {
            emit transferedSizeChanged(
                control->controller()->bytes_transferred());
            emit totalSizeChanged(control->controller()->total_byte_count());
            delete control;
          });
  connect(listner, &StorageListner::paused, this,
          [=](ControllerWrapper *control) {
            if (control->controller()->is_paused())
              emit paused(control->controller()->bytes_transferred());

            delete control;
          });
}

StorageQuery::~StorageQuery() {

  if (getIsBusy()) {
    cancel();
    delete mTaskManager;
  }
}

void StorageQuery::setTag(const QString &tg) {
  if (tg != m_tag) {
    m_tag = tg;
    emit tagChanged();
  }
}

FirebaseStorage *StorageQuery::parentStorage() const { return this->m_Storage; }

void StorageQuery::setParentStorage(FirebaseStorage *store) {
  if (store && store != m_Storage) {
    m_Storage = store;
    emit parentStorageChanged();
  }
}

FirebaseStorage::Priority StorageQuery::priority() const {
  return this->m_priority;
}

void StorageQuery::setPriority(FirebaseStorage::Priority p) {
  if (p != m_priority) {
    this->m_priority = p;
    emit priorityChanged();
  }
}
void StorageQuery::finished(Task *task) {

  if (!task)
    return;

  if (task->getId() == FirebaseUtils::Storage) {
    setIsBusy(false);

    auto future = task->getFuture();
    if (future.error() == firebase::storage::kErrorNone) {

      if (task->key() == currentKey) {

        switch (task->getType()) {
        case FirebaseStorage::Upload: {
          emit uploadComplete();

        } break;
        case FirebaseStorage::Download: {
          emit downloadComplete(downloadPath);
        } break;
        case FirebaseStorage::Link: {

          if (task->getUseStdFuture()) {

            auto df = task->getStdfuture();
            if (df.status() == firebase::kFutureStatusComplete) {

              if (df.result())
                emit linkGenerated(QString::fromUtf8(df.result()->c_str()));
              else {
                m_Storage->setError(0, QString("Unknown error has occured."),
                                    tag());
              }
            }
          }
        } break;
        }

        delete task;
      }

    } else
      m_Storage->setError(future.error(), QString(future.error_message()),
                          tag());
  }
}

void StorageQuery::initRef() {
  ref = m_Storage->getNativeReference();
  foreach (auto _path, m_pathHolder) {

    ref = ref.Child(_path.toStdString().c_str());
  }
  if (m_pathHolder.size() > 0)
    m_pathHolder.clear();
}

bool StorageQuery::getIsBusy() const { return isBusy; }

void StorageQuery::setIsBusy(bool value) { isBusy = value; }
qmlio::googleFirebase::StorageQuery *StorageQuery::child(QString val) {

  m_pathHolder.append(val);
  return this;
}

QStringList StorageQuery::currentPath() const { return this->m_pathHolder; }
void StorageQuery::putFile(QVariant file) {
  if (getIsBusy()) {
    m_Storage->setError(0, m_tag + " is currently busy ");
    return;
  }
  if (m_pathHolder.size() > 0 && m_Storage) {
    initRef();

    bool g = m_Storage->checkPermission();
    QFile m_file(file.toString());
    QDir f(file.toString());
    if (!f.exists() && !g) {

      m_Storage->setError(firebase::storage::kErrorUnknown, "file is invalid ");
      return;
    }

    auto future = ref.PutFile(file.toString().toUtf8(), listner, &m_controller);

    Task *task = Task::createNewTask(TASK_ID, FirebaseStorage::Upload, future,
                                     m_tag, m_priority);

    currentKey = task->key();
    mTaskManager->addTask(task);

    setIsBusy(true);
  }
}

void StorageQuery::pause() {
  if (m_controller.is_valid()) {
    m_controller.Pause();
    m_Resumed = false;
  }
}

void StorageQuery::resume() {
  if (m_controller.is_valid() && m_controller.is_paused()) {
    m_Resumed = m_controller.Resume();
    emit resumed(m_controller.bytes_transferred());
  }
}

void StorageQuery::cancel() {
  if (this->m_controller.is_valid()) {
    m_controller.Cancel();
    emit cancelled();
  }
}

bool StorageQuery::isPaused() { return m_controller.is_paused(); }

bool StorageQuery::isResumed() { return m_Resumed; }

void StorageQuery::getLink(QString filename) {
  if (filename.isEmpty()) {
    m_Storage->setError(0, "file name is empty ", tag());
    return;
  }

  if (m_pathHolder.size() > 0) {

    qDebug() << "Mupanf   " << currentPath();
    initRef();

    firebase::Future<std::string> link =
        ref.Child(filename.toUtf8()).GetDownloadUrl();

    Task *tsk = Task::createNewTask(TASK_ID, FirebaseStorage::Link, link, tag(),
                                    m_priority, true);
    currentKey = tsk->key();
    mTaskManager->addTask(tsk);
  }
}

void StorageQuery::downloadFile(QString path) {
  if (!path.isEmpty()) {

    initRef();
    auto file = ref.GetFile(path.toUtf8(), listner, &m_controller);
    Task *tsk = Task::createNewTask(TASK_ID, FirebaseStorage::Download, file,
                                    tag(), m_priority);
    currentKey = tsk->key();
    mTaskManager->addTask(tsk);

    downloadPath = path;
  }
}

ControllerWrapper::ControllerWrapper(
    firebase::storage::Controller *controller) {
  qRegisterMetaType<ControllerWrapper>();
  this->m_controller = *controller;
  qDebug() << "creating control " << this;
}

firebase::storage::Controller *ControllerWrapper::controller() {
  return &m_controller;
}

ControllerWrapper::~ControllerWrapper() {
  qDebug() << "deleting control " << this;
}
