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

#include "firebaseauth.h"
#include <QDebug>
#define TASK_ID FirebaseUtils::Auth

namespace auth = ::firebase::auth;
using namespace qmlio::googleFirebase;
FirebaseAuth::FirebaseAuth(QObject *parent)
    : QObject(parent), m_auth(nullptr), m_complete(false), m_signedIn(false),
      m_errId(ErrorNone), m_action(ActionSignIn),

      m_user(QVariantMap()), mTaskManager(new TaskManager)

{

  requestCode = 20;
  firebaseUtils = FirebaseUtils::getInstance().get();

  connect(firebaseUtils, &FirebaseUtils::readyChanged, this,
          &FirebaseAuth::init);

  connect(mTaskManager, &TaskManager::complete, this,
          &FirebaseAuth::onFutureEvent);
}

FirebaseAuth::~FirebaseAuth() {}

void FirebaseAuth::registerUser(const QString &email, const QString &pass) {
  clearError();
  setComplete(false);
  m_action = ActionRegister;
  Q_UNUSED(email)
  Q_UNUSED(pass)
}

void FirebaseAuth::signIn(const QString &email, const QString &pass) {

  m_action = ActionSignIn;
  clearError();
  setComplete(false);
  if (email.isEmpty() || pass.isEmpty())
    return;
  if (signedIn()) {

    signOut();
  }
  Q_UNUSED(email)
  Q_UNUSED(pass)
}

void FirebaseAuth::signInWithCredential(FirebaseAuth::Credential cred) {
  Q_UNUSED(cred)
}
void FirebaseAuth::updateUser(const QString &displayName,
                              const QString &picture) {
  if (!signedIn())
    return;
  Q_UNUSED(displayName)
  Q_UNUSED(picture)
}

void FirebaseAuth::sendPasswordResetEmail(const QString &email) {
  Q_UNUSED(email)
}

void FirebaseAuth::deleteUser() {}

void FirebaseAuth::updateEmail(const QString &email) { Q_UNUSED(email) }

void FirebaseAuth::updatePassword(const QString &password) {

  Q_UNUSED(password)
}
void FirebaseAuth::signOut() {

  m_action = ActionSignOut;
  clearError();
  setSignIn(false);
  emit completed(true, m_action, m_user);
}

bool FirebaseAuth::signedIn() const { return m_signedIn; }
void FirebaseAuth::clearError() { setError(ErrorNone); }
void FirebaseAuth::setError(int errId, const QString &errMsg) {
  m_errId = errId;
  m_errMsg = errMsg;
}

TaskManager *FirebaseAuth::taskManager() const { return mTaskManager; }

auth::Auth *FirebaseAuth::getUser(firebase::FutureBase *) { return nullptr; }

void FirebaseAuth::initCurrentUser(auth::Auth *user) { Q_UNUSED(user) }

void FirebaseAuth::init() { qDebug() << " Initializing auth desktop  "; }
QString FirebaseAuth::email() const { return QString(); }

QString FirebaseAuth::displayName() const { return QString(); }

bool FirebaseAuth::emailVerified() const { return false; }

QString FirebaseAuth::photoUrl() const { return QString(); }

QString FirebaseAuth::uid() const { return QString(); }

void FirebaseAuth::onFutureEvent(Task *helper) { Q_UNUSED(helper) }
void FirebaseAuth::setComplete(bool complete) {

  emit taskFinished(complete, m_action, m_user);
}
void FirebaseAuth::setComplete(bool complete, Action act) {

  emit taskFinished(complete, act, m_user);
}

void FirebaseAuth::setSignIn(bool value) {
  if (m_signedIn != value) {
    m_signedIn = value;
    emit signedInChanged();
    if (!value) {
      if (m_user.size() > 0)
        m_user.clear();
    }
  }
}
bool FirebaseAuth::running() const { return !m_complete; }

int FirebaseAuth::errorId() const { return m_errId; }

QString FirebaseAuth::errorMsg() const { return m_errMsg; }
FirebaseAuth *FirebaseAuth::getInstance() {

  static QSharedPointer<FirebaseAuth> self(new FirebaseAuth);
  return self.data();
}
ResultReciever::ResultReciever(FirebaseAuth *auth_) { this->auth_ = auth_; }
void ResultReciever::handleActivityResult(int, int, const void *) {}
