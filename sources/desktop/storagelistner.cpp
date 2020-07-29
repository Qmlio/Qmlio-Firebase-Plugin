#include "storagelistner.h"
#include <QDebug>
using namespace qmlio::googleFirebase;
StorageListner::~StorageListner() {}

StorageListner::StorageListner(const StorageListner &other) {
  this->m_query = other.m_query;
}

StorageListner &StorageListner::operator=(const StorageListner &other) {
  Q_UNUSED(other)
  return *this;
}

StorageListner::StorageListner(StorageQuery *query) {
  qRegisterMetaType<StorageListner>();
  this->m_query = query;
}

void StorageListner::OnProgress(firebase::storage::Controller *control) {
  ControllerWrapper *c = new ControllerWrapper(control);
  qDebug() << "is c " << c << "  " << c->controller()->is_valid() << "  "
           << control->is_valid();
  emit progress(c);
}

void StorageListner::OnPaused(firebase::storage::Controller *control) {
  ControllerWrapper *c = new ControllerWrapper(control);
  emit paused(c);
}
