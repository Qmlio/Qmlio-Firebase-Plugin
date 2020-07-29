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
#ifndef FIREBASEUTILS_H
#define FIREBASEUTILS_H

#include "firebase/app.h"
#include "firebase/future.h"
#include "firebase/util.h"
#include "firebase/variant.h"
#include "task.h"
#include <QObject>
#include <QSharedPointer>
#include <QThreadPool>
#include <QTimer>
namespace qmlio {
namespace googleFirebase {
template <typename ResultType> const ResultType *result(const void *p) {
  return p == nullptr ? nullptr : static_cast<const ResultType *>(p);
}
class FirebaseUtils : public QObject {
  Q_OBJECT
public:
  enum Tasks { Auth, Database, Storage, Messaging, DyanmicLinks };

  ~FirebaseUtils();

  static void wait();
  firebase::App *firebaseNativeApp() const;
  static FirebaseUtils *instance();
  static QVariant fromFirebaseVariant(const firebase::Variant &v);
  static firebase::Variant fromQtVariant(const QVariant &v);
  inline bool isReady() const { return mReady; }

signals:
  void readyChanged();

protected:
  void init();
  static QSharedPointer<FirebaseUtils> self;

private:
  explicit FirebaseUtils(QObject *parent = nullptr);
  bool mReady;
  QTimer *_initTimer = nullptr;
  firebase::App *_firebaseApp = nullptr;

  Q_DISABLE_COPY(FirebaseUtils)
};
} // namespace googleFirebase
} // namespace qmlio
#endif // FIREBASEUTILS_H
