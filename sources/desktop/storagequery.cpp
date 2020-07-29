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
#include <QFile>
#define TASK_ID FirebaseUtils::Storage
using namespace qmlio::googleFirebase;
namespace cloudReference = ::firebase::storage;
qmlio::googleFirebase::StorageQuery::StorageQuery(QObject *parent)
    : QObject(parent), m_Resumed(false) {
  m_Storage = nullptr;
  m_priority = FirebaseStorage::Normal;
  mTaskManager = new TaskManager;
  connect(mTaskManager, &TaskManager::complete, this, &StorageQuery::finished);
}

StorageQuery::~StorageQuery() {}

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
void StorageQuery::finished(Task *task) { Q_UNUSED(task) }

void StorageQuery::initRef() {}

qmlio::googleFirebase::StorageQuery *StorageQuery::child(QString val) {

  m_pathHolder.append(val);
  return this;
}

QStringList StorageQuery::currentPath() const { return this->m_pathHolder; }

void StorageQuery::putFile(QVariant file) { Q_UNUSED(file) }

void StorageQuery::pause() {}

void StorageQuery::resume() {}

bool StorageQuery::isPaused() { return false; }

bool StorageQuery::isResumed() { return m_Resumed; }

void StorageQuery::getLink(QString filename) { Q_UNUSED(filename) }

void StorageQuery::downloadFile(QString path){Q_UNUSED(path)}

ControllerWrapper::ControllerWrapper(
    firebase::storage::Controller *controller) {
  qRegisterMetaType<ControllerWrapper>();
  Q_UNUSED(controller)
  qDebug() << "creating control " << this;
}

ControllerWrapper::~ControllerWrapper() {
  qDebug() << "deleting control " << this;
}
