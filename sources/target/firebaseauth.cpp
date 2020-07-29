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
#include <QAndroidJniObject>
#include <QDebug>
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QAndroidJniObject>
#define TASK_ID FirebaseUtils::Auth

namespace auth = ::firebase::auth;
using namespace qmlio::googleFirebase;

FirebaseAuth::FirebaseAuth(QObject *parent)
    : QObject(parent), m_auth(nullptr), m_complete(false), m_signedIn(false),
      m_errId(ErrorNone), m_action(ActionSignIn), m_user(QVariantMap()),
      mTaskManager(new TaskManager)

{

  requestCode = 20;
  firebaseUtils = FirebaseUtils::instance();
  reciever = new ResultReciever(this);
  connect(firebaseUtils, &FirebaseUtils::readyChanged, this,
          &FirebaseAuth::init);

  connect(mTaskManager, &TaskManager::complete, this,
          &FirebaseAuth::onFutureEvent);
}

FirebaseAuth::~FirebaseAuth() { qDebug() << " deleting auth "; }

void FirebaseAuth::registerUser(const QString &email, const QString &pass) {
  clearError();
  setComplete(false);
  m_action = ActionRegister;

  firebase::Future<auth::User *> future =
      m_auth->CreateUserWithEmailAndPassword(email.toUtf8().constData(),
                                             pass.toUtf8().constData());

  Task *helper = Task::createNewTask(TASK_ID, ActionRegister, future,
                                     QString("Auth.Register"));
  mTaskManager->addTask(helper);
}

void FirebaseAuth::signIn(const QString &email, const QString &pass) {

  m_action = ActionSignIn;
  clearError();

  if (email.isEmpty() && pass.isEmpty()) {
    setError(0, "email or password are empty ");
    return;
  }
  if (signedIn()) {

    signOut();
  }

  firebase::Future<auth::User *> future = m_auth->SignInWithEmailAndPassword(
      email.toUtf8().constData(), pass.toUtf8().constData());

  Task *helper = Task::createNewTask(TASK_ID, ActionSignIn, future,
                                     QString("Auth.SignIn"));
  mTaskManager->addTask(helper);
}

void FirebaseAuth::signInWithCredential(FirebaseAuth::Credential cred,
                                        const QString &email,
                                        const QString &password) {
  if (signedIn())
    signOut();
  switch (cred) {
  case Anonymously: {
    auto future = m_auth->SignInAnonymously();
    Task *tsk = Task::createNewTask(TASK_ID, ActionSignIn, future,
                                    "auth.signInAnonymous");
    mTaskManager->addTask(tsk);
  } break;
  case GoogleSignIn: {

    auto activity = QtAndroid::androidActivity();
    auto context = QtAndroid::androidContext();
    QAndroidJniObject contextWrapper("android/content/ContextWrapper",
                                     "(Landroid/content/Context;)V",
                                     context.object<jobject>());
    QString className = "";

    if (contextWrapper.isValid()) {
      QAndroidJniObject v =
          contextWrapper.callObjectMethod<jstring>("getPackageName");
      QString packegeName = v.toString();
      auto splits = packegeName.split(".");
      foreach (QString value, splits) { className.append(value + "/"); }
    }
    QString default_web_client;
    jint v = QAndroidJniObject::getStaticField<jint>(
        (className + "R" + "$string").toUtf8(), "default_web_client_id");
    QString g = QString::number(v);
    qDebug() << g << "   " << v;
    QAndroidJniObject web_client = QAndroidJniObject::fromString(g);

    auto jclazz = new QAndroidJniObject(
        "org/qmlio/QmlioGoogleCredential",
        "(Lorg/qtproject/qt5/android/bindings/QtActivity;"
        "Landroid/content/Context;I)V",
        activity.object<jobject>(), context.object<jobject>(), v);

    if (jclazz->isValid()) {

      reciever->setClass(jclazz);
      auto intent =
          jclazz->callObjectMethod("requestId", "()Landroid/content/Intent;");
      if (intent.isValid())
        QtAndroid::startActivity(intent, requestCode, reciever);
    }
  } break;
  case FaceBookSignIn: {

    break;
  }
  case Email: {
    signIn(email, password);
    break;
  }
  }
}
void FirebaseAuth::updateUser(const QString &displayName,
                              const QString &picture) {
  if (!m_auth->current_user()) {

    return;
  }
  if (!displayName.isEmpty() && !picture.isEmpty()) {

    auth::User::UserProfile profile;

    std::string d(displayName.toUtf8());
    std::string p(picture.toUtf8());
    profile.display_name = d.c_str();
    profile.photo_url = p.c_str();
    auto future = m_auth->current_user()->UpdateUserProfile(profile);
    Task *helper = Task::createNewTask(TASK_ID, UpdateUserCredentials, future,
                                       QString("auth.Update"));
    mTaskManager->addTask(helper);
  }
}

void FirebaseAuth::sendPasswordResetEmail(const QString &email) {
  if (!email.isEmpty()) {

    auto future = m_auth->SendPasswordResetEmail(email.toUtf8().constData());
    Task *helper = Task::createNewTask(TASK_ID, ActionResetEmail, future,
                                       QString("auth.Reset email"));
    mTaskManager->addTask(helper);
  }
}

void FirebaseAuth::deleteUser() {
  if (m_auth->current_user()) {

    auto future = m_auth->current_user()->Delete();

    Task *helper = Task::createNewTask(TASK_ID, ActionDeleteUser, future,
                                       QString("auth.deleteUser"));
    mTaskManager->addTask(helper);
  }
}

void FirebaseAuth::updateEmail(const QString &email) {
  if (signedIn() && m_auth->current_user() && !email.isEmpty()) {
    auto future =
        m_auth->current_user()->UpdateEmail(email.toUtf8().constData());
    Task *tsk = Task::createNewTask(TASK_ID, ActionUpdateEmail, future,
                                    QString("auth.UpdateEmail"));
    mTaskManager->addTask(tsk);
  }
}

void FirebaseAuth::updatePassword(const QString &password) {
  if (signedIn() && m_auth->current_user() && !password.isEmpty()) {
    auto future =
        m_auth->current_user()->UpdateEmail(password.toUtf8().constData());
    Task *tsk = Task::createNewTask(TASK_ID, ActionUpdatePassword, future,
                                    QString("auth.UpdatePassword"));
    mTaskManager->addTask(tsk);
  }
}

void FirebaseAuth::reload() {

  if (m_auth->current_user()) {

    auto future = m_auth->current_user()->Reload();
    Task *tsk = Task::createNewTask(TASK_ID, ActionRefresh, future,
                                    QString("auth.Reload"));
    mTaskManager->addTask(tsk);
  }
}
void FirebaseAuth::signOut() {

  m_action = ActionSignOut;

  m_auth->SignOut();

  clearError();

  setSignIn(false);
  emit taskFinished(true, m_action, m_user);
}

bool FirebaseAuth::signedIn() const { return m_signedIn; }
void FirebaseAuth::clearError() { setError(ErrorNone); }
void FirebaseAuth::setError(int errId, const QString &errMsg) {
  m_errId = errId;
  m_errMsg = errMsg;

  emit errorChanged(errId, errMsg);
}

auth::User *FirebaseAuth::getUser(firebase::FutureBase *future) {
  auth::User *user = result<auth::User *>(future->result_void())
                         ? *(result<auth::User *>(future->result_void()))
                         : nullptr;
  return user;
}

void FirebaseAuth::initCurrentUser(auth::User *user) {
  if (user) {
    m_user["displayName"] = m_auth->current_user()->display_name().c_str();
    m_user["uid"] = m_auth->current_user()->uid().c_str();
    m_user["email"] = m_auth->current_user()->email().c_str();
    m_user["email_verified"] = m_auth->current_user()->is_email_verified();
    m_user["photoUrl"] = m_auth->current_user()->photo_url().c_str();
    m_user["provider_id"] = m_auth->current_user()->provider_id().c_str();
  }
}

void FirebaseAuth::init() {
  qDebug() << "initialising Auth Mobile ";
  if (!firebaseUtils->isReady()) {
    return;
  }
  if (!m_auth) {
    firebase::InitResult result;
    m_auth = auth::Auth::GetAuth(firebaseUtils->firebaseNativeApp(), &result);

    if (result == firebase::kInitResultSuccess) {
      initialised(true);
      auto *user = m_auth->current_user();
      if (user != nullptr) {

        initCurrentUser(user);
        setSignIn(true);
        setComplete(true, Action::ActionSignIn);
      } else {

        signedInChanged(false);
      }

    } else {
      setError(firebase::kInitResultFailedMissingDependency,
               "Google Play Services Update");
      initialised(false);
      setComplete(false);
    }
  }
}
QString FirebaseAuth::email() const {
  if (signedIn()) {
    return QString::fromUtf8(m_auth->current_user()->email().c_str());
  }
  return QString();
}

QString FirebaseAuth::displayName() const {
  if (signedIn()) {
    return QString::fromUtf8(m_auth->current_user()->display_name().c_str());
  }
  return QString();
}

bool FirebaseAuth::emailVerified() const {
  if (signedIn()) {
    return m_auth->current_user()->is_email_verified();
  }
  return false;
}

QString FirebaseAuth::photoUrl() const {
  if (signedIn()) {
    return QString::fromUtf8(m_auth->current_user()->photo_url().c_str());
  }
  return QString();
}

QString FirebaseAuth::uid() const {
  if (signedIn()) {
    return QString::fromUtf8(m_auth->current_user()->uid().c_str());
  }
  return QString();
}

void FirebaseAuth::onFutureEvent(Task *helper) {
  if (helper->getId() == FirebaseUtils::Auth) {

    qDebug() << "::executing Auth task " << helper->getTag();
    auto g = helper->getFuture();
    auto *future = &g;
    if (future->error() == auth::kAuthErrorNone) {
      switch (helper->getType()) {

      case ActionRegister: {
        setComplete(true, ActionRegister);
        auth::User *user = result<auth::User *>(future->result_void())
                               ? *(result<auth::User *>(future->result_void()))
                               : nullptr;
        if (user != nullptr) {

          auto future = user->SendEmailVerification();

          Task *tsk =
              Task::createNewTask(TASK_ID, ActionSendEmailVerification, future,
                                  QString("auth.emailVerification"));

          mTaskManager->addTask(tsk);
        }
        break;
      }
      case ActionSendEmailVerification: {

        setComplete(true, ActionSendEmailVerification);
        break;
      }
      case ActionSignIn: {

        auth::User *user = getUser(future);
        if (user != nullptr) {

          initCurrentUser(user);
          setSignIn(true);
          setComplete(true, ActionSignIn);
        }
        break;
      }

      case UpdateUserCredentials: {

        reload();

        break;
      }
      case ActionResetEmail: {

        setComplete(true, ActionResetEmail);
        break;
      }
      case ActionDeleteUser: {
        setSignIn(false);
        setComplete(true, ActionDeleteUser);
        break;
      }
      case ActionUpdateEmail: {
        if (m_auth->current_user()) {
          reload();
        }
        break;
      }
      case ActionUpdatePassword: {
        if (m_auth->current_user()) {
          reload();
        }
        break;
      }
      case ActionRefresh: {
        initCurrentUser(m_auth->current_user());
        setComplete(true, static_cast<Action>(helper->getType()));
        break;
      }
      }

    } else {
      setError(future->error(), future->error_message());
      emit completed(false, helper->getId(), m_user);
    }

    delete helper; // delete task
  }
}
void FirebaseAuth::setComplete(bool complete) {
  emit taskFinished(complete, m_action, m_user);
}
void FirebaseAuth::setComplete(bool complete, Action act) {

  emit taskFinished(complete, act, m_user);
}

void FirebaseAuth::setSignIn(bool value) {

  if (m_signedIn != value) {
    m_signedIn = value;

    if (!value) {
      if (m_user.size() > 0)
        m_user.clear();
    }
    signedInChanged(value);
  }
}
bool FirebaseAuth::running() const { return !m_complete; }

int FirebaseAuth::errorId() const { return m_errId; }

QString FirebaseAuth::errorMsg() const { return m_errMsg; }
FirebaseAuth *FirebaseAuth::getInstance() {

  static QSharedPointer<FirebaseAuth> self(new FirebaseAuth);
  return self.data();
}

ResultReciever::~ResultReciever() {

  qDebug() << "deleting result receiver " << this;
}

ResultReciever::ResultReciever(FirebaseAuth *auth_) {
  qDebug() << "creating result receiver " << this;
  this->auth_ = auth_;
}

void ResultReciever::handleActivityResult(int receiverRequestCode,
                                          int resultCode,
                                          const QAndroidJniObject &data) {

  jint RESULT_OK = QAndroidJniObject::getStaticField<jint>(
      "android/app/Activity", "RESULT_OK");

  if (resultCode == RESULT_OK) {
    // qDebug() << "in result ";
    if (receiverRequestCode == auth_->getrequestCode()) {
      auto id = jclazz->callObjectMethod(
          "getTokenId", "(Landroid/content/Intent;)Ljava/lang/String;",
          data.object<jobject>());

      if (!id.toString().isEmpty()) {
        if (auth_->getAuth()) {
          emit auth_->credentialIdReceived();
          std::string iD(id.toString().toUtf8());
          firebase::auth::Credential cred =
              firebase::auth::GoogleAuthProvider::GetCredential(iD.c_str(),
                                                                nullptr);
          auto future = auth_->getAuth()->SignInWithCredential(cred);
          Task *tsk = Task::createNewTask(TASK_ID, FirebaseAuth::ActionSignIn,
                                          future, "auth.googleSignIn");
          auth_->taskManager()->addTask(tsk);
        }
      }
    }
  }
}
