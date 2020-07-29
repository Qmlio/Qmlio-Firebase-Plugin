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
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <utils/platformutils.h>
#endif
using namespace qmlio::googleFirebase;
namespace busket = firebase::storage;
FirebaseStorage *FirebaseStorage::self = nullptr;
FirebaseStorage *FirebaseStorage::getInstance() {
  auto mSelf = self;
  if (!self)
    mSelf = {new FirebaseStorage};
  return mSelf;
}

FirebaseStorage::~FirebaseStorage() { qDebug() << "deleting storage"; }
bool FirebaseStorage::checkPermission() {
  bool granted = false;
  auto result = QtAndroid::checkPermission(
      QString("android.permission.WRITE_EXTERNAL_STORAGE"));
  if (result == QtAndroid::PermissionResult::Granted) {
    granted = true;
  }
  if (result == QtAndroid::PermissionResult::Denied) {
    QtAndroid::PermissionResultMap resultHash =
        QtAndroid::requestPermissionsSync(
            QStringList({"android.permission.WRITE_EXTERNAL_STORAGE"}));
    if (resultHash["android.permission.WRITE_EXTERNAL_STORAGE"] ==
        QtAndroid::PermissionResult::Denied) {
      granted = true;

    } else {
      granted = true;
    }
  }
  return granted;
}
bool FirebaseStorage::isReady() const { return m_ready; }

firebase::storage::StorageReference
FirebaseStorage::getNativeReference() const {
  return storage->GetReference();
}

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

firebase::storage::Storage *FirebaseStorage::getStorage() const {
  return storage;
}

FirebaseStorage::FirebaseStorage(QObject *parent)
    : QObject(parent), m_ready(false) {

  firebaseUtils = FirebaseUtils::instance();
  if (!firebaseUtils->isReady()) {
    connect(firebaseUtils, &FirebaseUtils::readyChanged, this,
            &FirebaseStorage::init);
  } else {
    init();
  }

  self = (this);
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
  } else {
    query->setParentStorage(getInstance());
    getInstance()->m_queries.append(query);
  }
}

void FirebaseStorage::init() {

  qDebug() << "initialising Storage Android";
  if (GooglePlayServices::getAvailability() ==
      GooglePlayServices::AvailabilityAvailable) {

    if (!m_ready) {

      firebase::InitResult result;
      storage = busket::Storage::GetInstance(firebaseUtils->firebaseNativeApp(),
                                             &result);

      auto f = getNativeReference();

      if (result != firebase::kInitResultSuccess) {
        initialised(false);
        setError(firebase::kInitResultFailedMissingDependency,
                 "failed to initalize firbase storage");
        m_ready = false;
      } else {
        emit initialised(true);
        m_ready = true;
      }
    }
  } else if (GooglePlayServices::getAvailability() ==
             GooglePlayServices::AvailabilityUnavailableUpdateRequired) {
    emit initialised(false);
    setError(GooglePlayServices::getAvailability(),
             "update Google play services");
  } else {
    emit initialised(false);
    setError(0, "Ununknown eroor has occured");
  }
}
