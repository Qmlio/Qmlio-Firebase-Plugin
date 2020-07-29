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
#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "task.h"
#include "worker.h"
#include <QObject>

namespace qmlio {
namespace googleFirebase {
class TaskManager : public QObject {
  Q_OBJECT
public:
  explicit TaskManager(QObject *parent = nullptr);
  ~TaskManager();
  void addTask(Task *tsk);
  int getMaxConcurrentThreads() const;
  void setMaxConcurrentThreads(int value);

signals:
  void complete(Task *helper);
  void pending(firebase::FutureBase future);
  void invalid();

protected:
  void waitForFuture(Task *tsk);
  void removeAll();
  void addWorker(Worker *);
  void hold(Worker *);

protected slots:
  int getWorkingLoad() const;
  void addWork(Worker *);
  void removeWorker(Worker *);
  void releaseWorker(int index);

private:
  QVector<Worker *> workers;
  QMap<int, Worker *> manager;
  int maxConcurrentThreads;
};
} // namespace googleFirebase
} // namespace qmlio
#endif // TASKMANAGER_H
