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
#ifndef STORAGELISTNER_H
#define STORAGELISTNER_H
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
