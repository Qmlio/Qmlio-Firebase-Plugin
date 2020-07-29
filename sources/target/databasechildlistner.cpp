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
#include "databasechildlistner.h"
#include <QDebug>
#include <utils/firebaseutils.h>
using namespace qmlio::googleFirebase;
DatabaseChildListner::DatabaseChildListner(QObject *parent) : QObject(parent) {
  qRegisterMetaType<DatabaseChildListner>();
}

DatabaseChildListner::DatabaseChildListner(const DatabaseChildListner &other) {
  qRegisterMetaType<DatabaseChildListner>();
  Q_UNUSED(other)
}

DatabaseChildListner &DatabaseChildListner::
operator=(const DatabaseChildListner &other) {
  qRegisterMetaType<DatabaseChildListner>();
  Q_UNUSED(other);
  return *this;
}

void DatabaseChildListner::OnChildAdded(
    const firebase::database::DataSnapshot &snapshot,
    const char *previous_sibling) {

  auto v = FirebaseUtils::fromFirebaseVariant(snapshot.value());

  emit childAdded(v, QString::fromUtf8(previous_sibling));
}

void DatabaseChildListner::OnChildChanged(
    const firebase::database::DataSnapshot &snapshot,
    const char *previous_sibling) {
  auto v = FirebaseUtils::fromFirebaseVariant(snapshot.value());
  emit childChanged(v, QString::fromUtf8(previous_sibling));
}

void DatabaseChildListner::OnChildMoved(
    const firebase::database::DataSnapshot &snapshot,
    const char *previous_sibling) {
  auto v = FirebaseUtils::fromFirebaseVariant(snapshot.value());
  emit childMoved(v, QString::fromUtf8(previous_sibling));
}

void DatabaseChildListner::OnChildRemoved(
    const firebase::database::DataSnapshot &snapshot) {
  auto v = FirebaseUtils::fromFirebaseVariant(snapshot.value());
  emit childRemoved(v);
}

void DatabaseChildListner::OnCancelled(
    const firebase::database::Error &error_code, const char *error_message) {
  Q_UNUSED(error_code);
  Q_UNUSED(error_message);
}

void DatabaseChildListner::OnValueChanged(
    const firebase::database::DataSnapshot &snapshot) {

  auto v = FirebaseUtils::fromFirebaseVariant(snapshot.value());
  emit valueChanged(v);
}
