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
#ifndef DATABASECHILDLISTNER_H
#define DATABASECHILDLISTNER_H

#include <QObject>
#include <QVariant>
#include <firebase/database.h>

namespace qmlio {
namespace googleFirebase {
class DatabaseChildListner : public QObject,
                             public firebase::database::ChildListener,
                             public firebase::database::ValueListener {
  Q_OBJECT
public:
  explicit DatabaseChildListner(QObject *parent = nullptr);
  DatabaseChildListner(const DatabaseChildListner &other);
  DatabaseChildListner &operator=(const DatabaseChildListner &other);

  void OnChildAdded(const firebase::database::DataSnapshot &snapshot,
                    const char *previous_sibling) override;
  void OnChildChanged(const firebase::database::DataSnapshot &snapshot,
                      const char *previous_sibling) override;
  void OnChildMoved(const firebase::database::DataSnapshot &snapshot,
                    const char *previous_sibling) override;
  void
  OnChildRemoved(const firebase::database::DataSnapshot &snapshot) override;
  void OnCancelled(const firebase::database::Error &error_code,
                   const char *error_message) override;
  void
  OnValueChanged(const firebase::database::DataSnapshot &snapshot) override;

signals:
  void childAdded(const QVariant &value, const QString &previous_sibling);
  void childChanged(const QVariant &value, const QString &previous_sibling);
  void childMoved(const QVariant &value, const QString &previous_sibling);
  void childRemoved(const QVariant &value);
  void cancelled(const firebase::database::Error &error_code,
                 const char *error_message);
  void valueChanged(const QVariant &value);
};
} // namespace googleFirebase
} // namespace qmlio
Q_DECLARE_METATYPE(qmlio::googleFirebase::DatabaseChildListner)
#endif // DATABASECHILDLISTNER_H
