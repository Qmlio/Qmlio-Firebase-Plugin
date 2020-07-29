#include "taskmanager.h"
#include "firebase/future.h"
#include "task.h"
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#define LOG_TAG "TASK_MANAGER "
using namespace qmlio::googleFirebase;
TaskManager::TaskManager(QObject *parent)
    : QObject(parent), maxConcurrentThreads(4) {}

TaskManager::~TaskManager() { qDebug() << "deleting task manager " << this; }
void TaskManager::addTask(Task *tsk) {
  Worker *worker{new Worker};
  worker->setParent(this);
  worker->addTask([=]() mutable { waitForFuture(tsk); });
  addWorker(worker);
}

void TaskManager::waitForFuture(Task *tsk) {

  auto f = tsk->getFuture();
  auto *future = &f;

  QEventLoop loop;
  QTimer t;

  while (future->status() == firebase::kFutureStatusPending) {

    qDebug() << LOG_TAG << "::Proccessing task " << tsk->getTag();
    t.connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
    t.start(1000);
    loop.exec();
  }

  switch (future->status()) {
  case firebase::kFutureStatusComplete: {

    emit complete(tsk);
    break;
  }
  case firebase::kFutureStatusInvalid: {
    emit invalid();

    break;
  }
  case firebase::kFutureStatusPending: {
    emit pending(tsk->getFuture());
    break;
  }
  }
}

void TaskManager::addWorker(Worker *worker) {

  if (!worker)
    return;

  static int i = 0;
  if (getWorkingLoad() < getMaxConcurrentThreads()) {
    connect(worker, &Worker::workfinished, this, &TaskManager::removeWorker);
    connect(worker, &Worker::finished, worker, &QObject::deleteLater);
    worker->start();
    qDebug() << LOG_TAG << " appending task :: " << worker;

    workers.append(worker);

  } else {

    qDebug() << LOG_TAG << "Holding  task :: " << worker;

    manager[i] = worker;
    i++;
  }
}

int TaskManager::getWorkingLoad() const { return workers.size(); }

void TaskManager::addWork(Worker *) {}

void TaskManager::removeWorker(Worker *worker) {
  qDebug() << LOG_TAG << " removing task ::: " << worker;
  static int counter = 0;
  QVectorIterator<Worker *> i(workers);
  int index = 0;
  while (i.hasNext()) {

    if (i.next() == worker) {

      workers.remove(index);
    }
    index++;
  }

  if (manager.size() > 0) {

    for (int index = 0; index < manager.size(); index++) {
      if (index == 0) {
        qDebug() << LOG_TAG << "releasing  task ::: " << manager.value(counter);
        addWorker(manager.value(counter));
        releaseWorker(counter);

        counter++;
        break;
      }
    }
  }
}

void TaskManager::releaseWorker(int index) { manager.remove(index); }

void TaskManager::setMaxConcurrentThreads(int value) {
  maxConcurrentThreads = value;
}

int TaskManager::getMaxConcurrentThreads() const {
  return maxConcurrentThreads;
}
