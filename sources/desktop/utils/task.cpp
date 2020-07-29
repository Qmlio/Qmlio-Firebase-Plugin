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
#include <QEventLoop>
#include <QGuiApplication>
#include <QTimer>
using namespace qmlio::googleFirebase;

Task::Task(int id, int type, firebase::FutureBase future, QString tag,
           int prior) {

  this->id = id;
  this->future = future;
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
                          QString tag, int priority) {

  return new Task(id, type, future, tag, priority);
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
  wait();
  static QString j;
  QString alpha =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
  QDateTime l;
  srand(l.currentMSecsSinceEpoch());
  QString rString;
  int i = 0;
  while (i < 7) {
    int temp = rand() % 62;

    rString.append(alpha.at(temp));
    i++;
  }

  return rString;
}

void Task::wait() {

  QEventLoop loop;
  QTimer t;
  int i = 0;
  while (true) {
    if (i == 1)
      break;
    t.connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
    t.start(1);
    loop.exec();
    i++;
  }
}

firebase::FutureBase Task::getFuture() const { return this->future; }
Task::Task() {}
