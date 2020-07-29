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
#ifndef FIREBASEAUTH_H
#define FIREBASEAUTH_H

#include "firebase/auth.h"
#include "utils/firebaseutils.h"
#include <QAndroidActivityResultReceiver>
#include <QObject>
#include <QVariantMap>
#include <utils/taskmanager.h>

namespace qmlio {

namespace googleFirebase {

class ResultReciever;

class FirebaseAuth : public QObject {

  Q_OBJECT

  Q_PROPERTY(bool running READ running NOTIFY runningChanged)
  Q_PROPERTY(bool signedIn READ signedIn NOTIFY signedInChanged)

public:
  enum Error {
    ErrorNone = firebase::auth::kAuthErrorNone,
    ErrorUnimplemented = firebase::auth::kAuthErrorUnimplemented,
    ErrorFailure = firebase::auth::kAuthErrorFailure
  };
  Q_ENUM(Error)

  /// @brief returns a shared pointer to this instance
  static FirebaseAuth *getInstance();

  /// @brief returns the request code used in authenticating
  int getrequestCode() const { return requestCode; }

  /// @brief destructor
  ~FirebaseAuth();

  /// @brief returns an auth instance used in this class
  firebase::auth::Auth *getAuth() const { return m_auth; }

  TaskManager *taskManager() const { return this->mTaskManager; }

  enum Action {
    ActionRegister,
    ActionSendEmailVerification,
    ActionSignIn,
    ActionSignOut,
    ActionDeleteUser,
    UpdateUserCredentials,
    ActionResetEmail,
    ActionUpdateEmail,
    ActionUpdatePassword,
    ActionRefresh,
  };
  Q_ENUM(Action)

  enum Credential {
    Anonymously,
    GoogleSignIn,
    FaceBookSignIn,
    Email

  };
  Q_ENUM(Credential)

signals:
  void signedInChanged(bool isSignedIn);
  void runningChanged();
  void taskFinished(bool success, int actionId, const QVariantMap user);
  void completed(bool success, int actionId, const QVariantMap user);
  void passwordResetEmailSent();
  void credentialIdReceived();
  void initialised(bool ready);
  void errorChanged(int errId, const QString &errMsg);

public slots:

  void registerUser(const QString &email, const QString &pass);
  void signIn(const QString &email, const QString &pass);
  void signInWithCredential(Credential ced, const QString &email = QString(""),
                            const QString &password = QString(""));
  void signOut();
  void updateUser(const QString &displayName, const QString &picture);
  void sendPasswordResetEmail(const QString &email);
  void deleteUser();
  void updateEmail(const QString &email);
  void updatePassword(const QString &password);
  void reload();

  bool signedIn() const;
  bool running() const;
  int errorId() const;
  QString errorMsg() const;

  QString email() const;
  QString displayName() const;
  bool emailVerified() const;
  QString photoUrl() const;
  QString uid() const;

protected slots:
  void init();
  void onFutureEvent(Task *helper);

protected:
  explicit FirebaseAuth(QObject *parent = nullptr);
  void clearError();
  void setComplete(bool complete);
  void setComplete(bool complete, Action act);
  void setSignIn(bool value);
  void setError(int errId, const QString &errMsg = QString(""));
  ::firebase::auth::User *getUser(firebase::FutureBase *future);
  void initCurrentUser(::firebase::auth::User *user);

private:
  firebase::auth::Auth *m_auth;
  ResultReciever *reciever;
  int requestCode;
  bool m_complete;
  bool m_signedIn;
  int m_errId;
  QString m_errMsg;
  int m_action;
  QVariantMap m_user;
  FirebaseUtils *firebaseUtils;
  TaskManager *mTaskManager;
  Q_DISABLE_COPY(FirebaseAuth)
};

class ResultReciever : public QAndroidActivityResultReceiver {
  QAndroidJniObject *jclazz;
  FirebaseAuth *auth_;

public:
  virtual ~ResultReciever() override;
  ResultReciever(FirebaseAuth *auth_);
  virtual void handleActivityResult(int receiverRequestCode, int resultCode,
                                    const QAndroidJniObject &data) override;
  void setClass(QAndroidJniObject *jclazz) {
    if (jclazz)
      this->jclazz = jclazz;
  }
};
} // namespace googleFirebase
} // namespace qmlio
#endif // FIREBASEAUTH_H
