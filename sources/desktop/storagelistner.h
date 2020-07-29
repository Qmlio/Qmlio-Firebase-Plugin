#ifndef STORAGELISTNER_H
#define STORAGELISTNER_H
#include "firebase/storage/controller.h"
#include "firebase/storage/listener.h"
#include "storagequery.h"
#include <QObject>
namespace qmlio {
namespace googleFirebase {
class StorageListner : public QObject, public firebase::storage::Listener {
  Q_OBJECT
public:
  virtual ~StorageListner() override;
  StorageListner(const StorageListner &other);
  StorageListner &operator=(const StorageListner &other);
  StorageListner(StorageQuery *query = nullptr);
  virtual void OnProgress(::firebase::storage::Controller *control) override;
  virtual void OnPaused(::firebase::storage::Controller *control) override;
signals:
  void paused(ControllerWrapper *controller);
  void progress(ControllerWrapper *contoroller);

private:
  StorageQuery *m_query;
};
} // namespace googleFirebase
} // namespace qmlio
Q_DECLARE_METATYPE(qmlio::googleFirebase::StorageListner);
#endif // STORAGELISTNER_H
