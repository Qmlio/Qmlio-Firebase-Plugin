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
#ifndef TASK_H
#define TASK_H
#include "firebase/future.h"
#include <QObject>
#include <QString>
#include <QThread>
namespace qmlio {
namespace googleFirebase {
class Task {
public:
  Task(int id, int type, firebase::FutureBase future, QString tag = "",
       int prior = 0, bool usestdFuture = false);
  Task();

  ~Task();

  static Task *createNewTask(int id, int type, firebase::FutureBase future,
                             QString tag = "", int priority = 0,
                             bool usestdFuture = false);

  void setId(int id);
  void setType(int type);
  void setTag(QString tag);
  void setFuture(firebase::FutureBase future);
  void setFuture();
  void setPriorty(int prior);

  int getId() const;
  int getType() const;
  QString getTag() const;
  QString key() const;
  int priority() const;
  firebase::FutureBase getFuture() const;
  QThread::Priority getTask_Priority() const;
  void setTask_Priority(const QThread::Priority &value);

  firebase::Future<std::string> getStdfuture() const;
  void setStdfuture(const firebase::Future<std::string> &stdfuture);

  bool getUseStdFuture() const;
  void setUseStdFuture(bool value);

protected:
  QString generateKey();

private:
  int id;
  int type;
  bool useStdFuture;
  firebase::FutureBase future;
  firebase::Future<std::string> mStdfuture;
  QString tag;
  int m_Priority;
  QString m_key;
  QString temp;
  QThread::Priority task_Priority;
};

} // namespace googleFirebase
} // namespace qmlio
Q_DECLARE_METATYPE(qmlio::googleFirebase::Task)
#endif // TASK_H
