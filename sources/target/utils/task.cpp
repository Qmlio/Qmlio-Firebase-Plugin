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
#include "task.h"
#include "firebaseutils.h"
#include <QDateTime>
#include <QDebug>
using namespace qmlio::googleFirebase;

Task::Task(int id, int type, firebase::FutureBase future, QString tag,
           int prior, bool usestdFuture)
    : useStdFuture(usestdFuture) {

  this->id = id;

  if (usestdFuture) {
    auto hmStdfuture = static_cast<firebase::Future<std::string> *>(&future);
    this->mStdfuture = (*hmStdfuture);
  } else {
    this->future = future;
  }
  this->tag = tag;
  this->type = type;
  m_Priority = prior;
  this->m_key = generateKey();

  qRegisterMetaType<Task>();
}
Task::~Task() { qDebug() << "deleting task " << tag; }

/// @brief Creates a new query task
/// @param[in] id module type being queried
/// @param[in] type action this task is performing
/// @param[in] future a firebase future to be executed
/// @param[in] tag a name to this task main used for debuging purposes
/// @param[in] priority thread priority to be used
/// @return a new Task
Task *Task::createNewTask(int id, int type, firebase::FutureBase future,
                          QString tag, int priority, bool usestdFuture) {

  return new Task(id, type, future, tag, priority, usestdFuture);
}

void Task::setId(int id) { this->id = id; }

void Task::setType(int type) { this->type = type; }

void Task::setTag(QString tag) { this->tag = tag; }

void Task::setFuture(firebase::FutureBase future) { this->future = future; }

int Task::getId() const { return this->id; }

int Task::getType() const { return this->type; }

QString Task::getTag() const { return this->tag; }

QString Task::key() const { return this->m_key; }

int Task::priority() const { return this->m_Priority; }

QString Task::generateKey() {
  FirebaseUtils::wait();
  static QString j;
  QString alpha =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
  QDateTime l;
  srand(static_cast<unsigned>(l.currentMSecsSinceEpoch()));
  QString rString;
  int i = 0;
  while (i < 7) {
    int temp = rand() % 62;

    rString.append(alpha.at(temp));
    i++;
  }

  return rString;
}

bool Task::getUseStdFuture() const { return useStdFuture; }

void Task::setUseStdFuture(bool value) { useStdFuture = value; }

firebase::Future<std::string> Task::getStdfuture() const { return mStdfuture; }

void Task::setStdfuture(const firebase::Future<std::string> &stdfuture) {
  mStdfuture = stdfuture;
}

QThread::Priority Task::getTask_Priority() const { return task_Priority; }

void Task::setTask_Priority(const QThread::Priority &value) {
  task_Priority = value;
}

firebase::FutureBase Task::getFuture() const {
  return (!useStdFuture ? this->future : this->mStdfuture);
}
Task::Task() {}
