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
#include <QJsonDocument>
#define TASK_ID FirebaseUtils::Database
#include <QDebug>
using namespace qmlio::googleFirebase;
DatabaseQuery::DatabaseQuery(QObject *parent)
    : QObject(parent), mPushedkey(""), mTaskManager(new TaskManager),
      isInitialised(false) {

  connect(mTaskManager, &TaskManager::complete, this, &DatabaseQuery::finished);
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

void DatabaseQuery::setValue(QVariant value) { Q_UNUSED(value) }
void DatabaseQuery::setValueAsMap(QVariantMap value) { Q_UNUSED(value) }
void DatabaseQuery::getValue(bool isQuery){Q_UNUSED(isQuery)

}

DatabaseQuery *DatabaseQuery::pushChild() {
  return this;
}

void DatabaseQuery::removeValue() {}

QString DatabaseQuery::key() const { return this->mPushedkey; }

void DatabaseQuery::addChildListner() {}

void DatabaseQuery::addValueListner() {}

DatabaseQuery *DatabaseQuery::orderByChild(const QString &path) {
  Q_UNUSED(path)
  return this;
}

DatabaseQuery *DatabaseQuery::orderByKey() { return this; }

DatabaseQuery *DatabaseQuery::orderByPriority() { return this; }

DatabaseQuery *DatabaseQuery::orderByValue() { return this; }

DatabaseQuery *DatabaseQuery::startAt(const QVariant &order_value,
                                      QString key) {
  Q_UNUSED(order_value)
  Q_UNUSED(key)
  return this;
}

DatabaseQuery *DatabaseQuery::endAt(const QVariant &order_value) {
  Q_UNUSED(order_value)

  return this;
}

DatabaseQuery *DatabaseQuery::equalTo(const QVariant &order_value) {
  Q_UNUSED(order_value)

  return this;
}

DatabaseQuery *DatabaseQuery::limitToFirst(quint32 limit) {
  Q_UNUSED(limit)
  return this;
}

DatabaseQuery *DatabaseQuery::limitToLast(quint32 limit) {
  Q_UNUSED(limit)
  return this;
}

void DatabaseQuery::initializeReference() {}

void DatabaseQuery::finished(Task *tsk) { Q_UNUSED(tsk) }

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

bool DatabaseQuery::isDatabaseValid() {
  if (database())
    return true;
  else
    return false;
}

void DatabaseQuery::attachNotifiers(bool ready) { emit databaseReady(ready); }
