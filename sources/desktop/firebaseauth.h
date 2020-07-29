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

#include "firebase/app.h"
#include "utils/firebaseutils.h"
#include "utils/taskmanager.h"
#include <QObject>
#include <QVariantMap>
namespace qmlio {
namespace googleFirebase {

class ResultReciever;
class FirebaseAuth : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool running READ running NOTIFY runningChanged)
  Q_PROPERTY(bool signedIn READ signedIn NOTIFY signedInChanged)

public:
  enum Error { ErrorNone = 0, ErrorUnimplemented = -1, ErrorFailure = 1 };
  Q_ENUM(Error)
  static FirebaseAuth *getInstance();
  int getrequestCode() const { return requestCode; }
  ~FirebaseAuth();
  ::firebase::auth::Auth *getAuth() const { return m_auth; }

  enum Action {
    ActionRegister,
    ActionSendEmailVerification,
    ActionSignIn,
    ActionSignOut,
    ActionDeleteUser,
    UpdateUserCredentials,
    ActionResetEmail,
    ActionUpdateEmail,
    ActionUpdatePassword
  };
  Q_ENUM(Action)

  enum Credential {
    Anonymously,
    GoogleSignIn,
    FaceBookSignIn

  };
  Q_ENUM(Credential)

  TaskManager *taskManager() const;

signals:
  void signedInChanged();
  void runningChanged();
  void taskFinished(bool success, int actionId, const QVariantMap user);
  void completed(bool success, int actionId, const QVariantMap user);
  void passwordResetEmailSent();
  void credentialIdReceived();
  void errorChanged(int errId, const QString &errMsg);
  void initialised(bool ready);

public slots:

  void registerUser(const QString &email, const QString &pass);
  void signIn(const QString &email, const QString &pass);
  void signInWithCredential(Credential ced);
  void signOut();
  void updateUser(const QString &displayName, const QString &picture);
  void sendPasswordResetEmail(const QString &email);
  void deleteUser();
  void updateEmail(const QString &email);
  void updatePassword(const QString &password);

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
  ::firebase::auth::Auth *getUser(firebase::FutureBase *future);
  void initCurrentUser(::firebase::auth::Auth *auth);

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

class ResultReciever {

  FirebaseAuth *auth_;

public:
  virtual ~ResultReciever() {}
  ResultReciever(FirebaseAuth *auth_);
  virtual void handleActivityResult(int receiverRequestCode, int resultCode,
                                    const void *data);
  void setClass(const void *jclazz) { Q_UNUSED(jclazz) }
};
} // namespace googleFirebase
} // namespace qmlio
#endif // FIREBASEAUTH_H
