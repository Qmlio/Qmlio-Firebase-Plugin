#ifndef FIREBASEREFERENCE_H
#define FIREBASEREFERENCE_H

#include <QObject>
#include <firebase/target/firebasedatabase.h>
namespace qmlio {
namespace googleFirebase {

class DatabaseReference : public QObject,
                          firebase::database::DatabaseReference {
  Q_OBJECT
public:
  explicit DatabaseReference(QObject *parent = nullptr);

  qmlio::googleFirebase::DatabaseReference *orderByChild(const QString &path);
  qmlio::googleFirebase::DatabaseReference *orderByKey();
  qmlio::googleFirebase::DatabaseReference *orderByPriority();
  qmlio::googleFirebase::DatabaseReference *orderByValue();

  qmlio::googleFirebase::DatabaseReference *
  startAt(const QVariant &order_value);

  qmlio::googleFirebase::DatabaseReference *endAt(const QVariant &order_value);
  qmlio::googleFirebase::DatabaseReference *
  equalTo(const QVariant &order_value);
  qmlio::googleFirebase::DatabaseReference *limitToFirst(quint32 limit);

  qmlio::googleFirebase::DatabaseReference *limitToLast(quint32 limit);

  firebase::database::DatabaseReference reference() const;

  void assignRef(firebase::database::DatabaseReference &ref);

private:
  firebase::database::DatabaseReference mReference;
};
} // namespace googleFirebase

} // namespace qmlio
#endif // FIREBASEREFERENCE_H
