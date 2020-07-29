#include "databaseference.h"
#include <QDebug>
using namespace qmlio::googleFirebase;
DatabaseReference::DatabaseReference(QObject *parent) : QObject(parent) {}

DatabaseReference *DatabaseReference::orderByChild(const QString &path) {
  if (!path.isEmpty())
    mReference.OrderByChild(path.toUtf8());

  return this;
}

DatabaseReference *DatabaseReference::orderByKey() {
  mReference.OrderByKey();
  return this;
}

DatabaseReference *DatabaseReference::orderByPriority() {
  mReference.OrderByPriority();
  return this;
}

DatabaseReference *DatabaseReference::orderByValue() {
  mReference.OrderByValue();
  return this;
}

DatabaseReference *DatabaseReference::startAt(const QVariant &order_value) {
  if (order_value.isValid()) {

    mReference.StartAt(FirebaseUtils::fromQtVariant(order_value));
  }
  return this;
}

DatabaseReference *DatabaseReference::endAt(const QVariant &order_value) {
  if (order_value.isValid()) {

    mReference.EndAt(FirebaseUtils::fromQtVariant(order_value));
  }
  return this;
}

DatabaseReference *DatabaseReference::equalTo(const QVariant &order_value) {
  qDebug() << "  " << order_value;
  if (order_value.isValid()) {

    mReference.EqualTo(FirebaseUtils::fromQtVariant(order_value));
  }
  return this;
}

DatabaseReference *DatabaseReference::limitToFirst(quint32 limit) {

  mReference.LimitToFirst(limit);
  return this;
}

DatabaseReference *DatabaseReference::limitToLast(quint32 limit) {

  qDebug() << "  " << mReference.url().c_str();
  mReference.LimitToLast(limit);

  return this;
}

firebase::database::DatabaseReference DatabaseReference::reference() const {

  return mReference;
}

void DatabaseReference::assignRef(firebase::database::DatabaseReference &ref) {
  if (ref.is_valid())
    mReference = ref;
}
