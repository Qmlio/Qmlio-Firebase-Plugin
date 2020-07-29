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

#include "databasequery.h"
#include <QDebug>
#include <QJsonDocument>

#define TASK_ID FirebaseUtils::Database
using namespace qmlio::googleFirebase;
DatabaseQuery::DatabaseQuery(QObject *parent)
    : QObject(parent), mPushedkey(""), mTaskManager(new TaskManager),
      listner(new DatabaseChildListner), isInitialised(false) {

  connect(mTaskManager, &TaskManager::complete, this, &DatabaseQuery::finished);

  connect(listner, &DatabaseChildListner::childAdded, this,
          &DatabaseQuery::OnChildAdded);

  connect(listner, &DatabaseChildListner::childChanged, this,
          &DatabaseQuery::OnChildChanged);
  connect(listner, &DatabaseChildListner::childMoved, this,
          &DatabaseQuery::OnChildMoved);
  connect(listner, &DatabaseChildListner::valueChanged, this,
          &DatabaseQuery::OnValueChanged);
  connect(listner, &DatabaseChildListner::childRemoved, this,
          &DatabaseQuery::OnChildRemoved);
  mTaskManager->setMaxConcurrentThreads(1);
}

DatabaseQuery::~DatabaseQuery() {}

void DatabaseQuery::setTag(const QString &tg) {
  if (tg != m_tag) {
    m_tag = tg;
    emit tagChanged();
  }
}

FirebaseDatabase *DatabaseQuery::database() const { return this->m_Databse; }

void DatabaseQuery::setDatabase(FirebaseDatabase *store) {

  if (store && store != m_Databse) {
    this->m_Databse = store;
    connect(this->m_Databse, &FirebaseDatabase::databaseInitialized, this,
            &DatabaseQuery::attachNotifiers);
    emit databaseChanged();
  }
}

int DatabaseQuery::priority() const { return this->m_priority; }

void DatabaseQuery::setPriority(int p) {
  if (p != m_priority) {
    this->m_priority = p;
    emit priorityChanged();
  }
}

qmlio::googleFirebase::DatabaseQuery *DatabaseQuery::child(QString str) {
  this->m_path.append(str);
  return this;
}

QStringList DatabaseQuery::currentPath() const { return this->m_path; }

void DatabaseQuery::setValue(QVariant value) {

  if (value.isValid()) {
    if (!mPushedkey.isEmpty()) {

      m_Reference = m_Reference.Child(mPushedkey.toUtf8().constData());
      mPushedkey.clear();
    }

    initializeReference();
    auto future = m_Reference.SetValue(FirebaseUtils::fromQtVariant(value));
    auto tsk = FirebaseDatabase::createTask(FirebaseDatabase::SetValue, future,
                                            m_tag, m_priority);

    currentKey = tsk->key();

    mTaskManager->addTask(tsk);
  }
}

void DatabaseQuery::setValueAsMap(QVariantMap value) {
  if (!value.isEmpty()) {

    if (!mPushedkey.isEmpty()) {

      m_Reference = m_Reference.Child(mPushedkey.toUtf8().constData());
      mPushedkey.clear();
    }
    auto v = value.toStdMap();
    auto c = firebase::Variant(FirebaseUtils::fromQtVariant(value));

    initializeReference();
    auto future = m_Reference.SetValue(
        firebase::Variant(FirebaseUtils::fromQtVariant(value)));
    auto tsk = FirebaseDatabase::createTask(FirebaseDatabase::SetValue, future,
                                            m_tag, m_priority);

    currentKey = tsk->key();

    mTaskManager->addTask(tsk);
  }
}

void DatabaseQuery::getValue(bool isQuery) {
  isInitialised = false;

  if (!isQuery) {

    initializeReference();
    if (m_Reference.is_valid()) {

      auto future = m_Reference.GetValue();
      auto tsk = FirebaseDatabase::createTask(FirebaseDatabase::GetValue,
                                              future, m_tag, m_priority);
      currentKey = tsk->key();
      mTaskManager->addTask(tsk);

    } else {
      m_Databse->setError(firebase::database::kErrorUnknownError,
                          "trying to operate on an empty Database::Reference");
    }
  } else {

    auto future = m_query.GetValue();
    auto tsk = FirebaseDatabase::createTask(FirebaseDatabase::GetValue, future,
                                            m_tag, m_priority);
    currentKey = tsk->key();
    mTaskManager->addTask(tsk);
  }
}

DatabaseQuery *DatabaseQuery::pushChild() {
  mPushedkey = m_Reference.PushChild().key();

  return this;
}

void DatabaseQuery::removeValue() {
  this->initializeReference();
  if (m_Reference.is_valid()) {

    auto future = m_Reference.RemoveValue();
    auto tsk = FirebaseDatabase::createTask(FirebaseDatabase::RemoveValue,
                                            future, m_tag, m_priority);
    currentKey = tsk->key();
    mTaskManager->addTask(tsk);
  }
}

QString DatabaseQuery::key() const { return this->mPushedkey; }

void DatabaseQuery::addChildListner() { m_query.AddChildListener(listner); }

void DatabaseQuery::addValueListner() {

  if (!m_path.isEmpty()) {
    initializeReference();

    m_Reference.AddValueListener(listner);
  } else {

    m_query.AddValueListener(listner);
  }
}

DatabaseQuery *DatabaseQuery::orderByChild(const QString &path) {
  if (!path.isEmpty()) {
    initializeReference();

    m_query = m_Reference.OrderByChild(path.toUtf8());

    isInitialised = true;
  }
  return this;
}

DatabaseQuery *DatabaseQuery::orderByKey() {
  if (!isInitialised)
    initializeReference();
  m_query = m_Reference.OrderByKey();
  return this;
}

DatabaseQuery *DatabaseQuery::orderByPriority() {
  if (!isInitialised)
    initializeReference();
  m_query = m_Reference.OrderByPriority();
  return this;
}

DatabaseQuery *DatabaseQuery::orderByValue() {
  if (!isInitialised)
    initializeReference();
  m_query = m_Reference.OrderByValue();

  return this;
}

DatabaseQuery *DatabaseQuery::startAt(const QVariant &order_value,
                                      QString key) {
  if (order_value.isValid()) {

    if (key == "null") {

      m_query = m_query.StartAt(FirebaseUtils::fromQtVariant(order_value));
    } else {

      m_query = m_query.StartAt(FirebaseUtils::fromQtVariant(order_value),
                                key.toUtf8());
    }
  }
  return this;
}

DatabaseQuery *DatabaseQuery::endAt(const QVariant &order_value) {
  if (order_value.isValid()) {

    m_query = m_query.EndAt(FirebaseUtils::fromQtVariant(order_value));
  }
  return this;
}

DatabaseQuery *DatabaseQuery::equalTo(const QVariant &order_value) {

  if (order_value.isValid()) {

    m_query = m_query.EqualTo(FirebaseUtils::fromQtVariant(order_value));
  }
  return this;
}

DatabaseQuery *DatabaseQuery::limitToFirst(quint32 limit) {
  if (!isInitialised)
    initializeReference();
  m_query = m_Reference.LimitToFirst(limit);
  return this;
}

DatabaseQuery *DatabaseQuery::limitToLast(quint32 limit) {
  if (!isInitialised)
    initializeReference();

  m_query = m_Reference.LimitToLast(limit);

  return this;
}

void DatabaseQuery::initializeReference() {

  if (m_path.size() <= 0)
    return;
  if (m_Databse->getDatabase()) {
    m_Reference = m_Databse->getDatabase()->GetReference();
    foreach (QString path, m_path)
      m_Reference = m_Reference.Child(path.toUtf8());
  }

  if (m_path.size())
    m_path.clear();
}

void DatabaseQuery::finished(Task *tsk) {

  if (tsk->getId() == TASK_ID) {
    auto future = tsk->getFuture();
    if (future.error() == firebase::database::kErrorNone) {

      if (tsk->key() == currentKey) {
        switch (tsk->getType()) {
        case FirebaseDatabase::SetValue: {
          emit completed(FirebaseDatabase::SetValue, QByteArray());

        } break;
        case FirebaseDatabase::GetValue: {
          auto _future =
              result<firebase::database::DataSnapshot>(future.result_void());
          if (_future)
            emit completed(
                FirebaseDatabase::GetValue,
                FirebaseDatabase::fromFirebaseDoc(*_future).toJson());

        } break;
        case FirebaseDatabase::RemoveValue: {

          emit completed(FirebaseDatabase::RemoveValue, QByteArray());
        } break;
        }

        delete tsk;
      }

    } else {
      m_Databse->setError(future.error(), future.error_message());
    }
  }
}

void DatabaseQuery::OnChildAdded(const QVariant &value,
                                 const QString &previous_sibling) {
  emit childAdded(value, previous_sibling);
}

void DatabaseQuery::OnChildChanged(const QVariant &value,
                                   const QString &previous_sibling) {
  emit childChanged(value, previous_sibling);
}

void DatabaseQuery::OnChildMoved(const QVariant &value,
                                 const QString &previous_sibling) {
  emit childMoved(value, previous_sibling);
}

void DatabaseQuery::OnChildRemoved(const QVariant &value) {
  emit childRemoved(value);
}

void DatabaseQuery::OnValueChanged(const QVariant &value) {
  emit valueChanged(value);
}

void DatabaseQuery::attachNotifiers(bool ready) { emit databaseReady(ready); }
