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

#include "firebasestorage.h"
#include "storagequery.h"
#include <QDebug>
#include <QSharedPointer>
using namespace qmlio::googleFirebase;
namespace busket = firebase::storage;
FirebaseStorage *self = nullptr;
FirebaseStorage *FirebaseStorage::getInstance() {
  if (!self)
    self = {new FirebaseStorage};
  return self;
}

FirebaseStorage::~FirebaseStorage() {

  self = nullptr;
  qDebug() << "in destructor";
}
bool FirebaseStorage::checkPermission() { return false; }
bool FirebaseStorage::isReady() const { return m_ready; }
#ifdef Q_OS_ANDROID
firebase::storage::StorageReference
FirebaseStorage::getNativeReference() const {
  return storage->GetReference();
}
firebase::storage::Storage *FirebaseStorage::getStorage() const {
  return this->storage;
}
#else
void FirebaseStorage::getNativeReference() const {}
void *FirebaseStorage::getStorage() const { return 0; }
#endif
QQmlListProperty<StorageQuery> FirebaseStorage::queries() {
  return QQmlListProperty<StorageQuery>(
      this, this, &FirebaseStorage::append_Query, &FirebaseStorage::queryCount,
      &FirebaseStorage::query, &FirebaseStorage::clearqueries);
}

int FirebaseStorage::queryCount() const { return m_queries.count(); }

StorageQuery *FirebaseStorage::query(int index) const {
  return this->m_queries.at(index);
}

void FirebaseStorage::clear() { this->m_queries.clear(); }

int FirebaseStorage::queryCount(QQmlListProperty<StorageQuery> *mList) {
  return reinterpret_cast<FirebaseStorage *>(mList->data)->queryCount();
}

StorageQuery *FirebaseStorage::query(QQmlListProperty<StorageQuery> *mList,
                                     int mIndex) {
  return reinterpret_cast<FirebaseStorage *>(mList->data)->query(mIndex);
}

void FirebaseStorage::clearqueries(QQmlListProperty<StorageQuery> *mList) {
  reinterpret_cast<FirebaseStorage *>(mList->data)->clear();
}

FirebaseStorage::FirebaseStorage(QObject *parent)
    : QObject(parent), m_ready(false) {

  connect(FirebaseUtils::getInstance().get(), &FirebaseUtils::readyChanged,
          this, &FirebaseStorage::init);

  if (!self)
    self = this;
}

void FirebaseStorage::setError(int errorCode, QString errorMsg, QString tag) {
  emit errorChanged(errorCode, errorMsg, tag);
}

void FirebaseStorage::append_Query(QQmlListProperty<StorageQuery> *list,
                                   StorageQuery *query) {
  FirebaseStorage *storage = qobject_cast<FirebaseStorage *>(list->object);
  if (storage) {
    query->setParentStorage(storage);
    storage->m_queries.append(query);
  }
}

void FirebaseStorage::init() { qDebug() << "Initializing Storage Desktop   "; }
