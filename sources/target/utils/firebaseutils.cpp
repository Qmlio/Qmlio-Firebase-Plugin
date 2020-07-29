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
#include "firebaseutils.h"
#include "platformutils.h"
#include <QDebug>
#include <QEventLoop>
#include <QGuiApplication>
#include <QTimer>
#include <QVariant>
using namespace qmlio::googleFirebase;
QSharedPointer<FirebaseUtils> FirebaseUtils::self = nullptr;
FirebaseUtils::FirebaseUtils(QObject *parent) : QObject(parent), mReady(false) {
  _initTimer = new QTimer(this);
  FirebaseUtils::self = QSharedPointer<FirebaseUtils>(this);

  _initTimer->setSingleShot(false);
  connect(_initTimer, &QTimer::timeout, this, &FirebaseUtils::init);
  _initTimer->start(1000);
}
firebase::App *FirebaseUtils::firebaseNativeApp() const { return _firebaseApp; }

void FirebaseUtils::wait() {

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

FirebaseUtils::~FirebaseUtils() { qDebug() << " destroying Util " << this; }

FirebaseUtils *FirebaseUtils::instance() {

  static auto s = FirebaseUtils::self.data();
  if (!self)
    s = new FirebaseUtils;

  qDebug() << " Requesting Util :::::   " << s;
  return s;
}

QVariant FirebaseUtils::fromFirebaseVariant(const firebase::Variant &v) {
  switch (v.type()) {
  case firebase::Variant::kTypeNull:
    return QVariant();
  case firebase::Variant::kTypeInt64:
    return QVariant(v.int64_value());
  case firebase::Variant::kTypeDouble:
    return QVariant(v.double_value());
  case firebase::Variant::kTypeBool:
    return QVariant(v.bool_value());
  case firebase::Variant::kTypeStaticString:
    return QVariant(QString::fromUtf8(v.string_value()));
  case firebase::Variant::kTypeMutableString:
    return QVariant(QString::fromUtf8(v.mutable_string().c_str()));
  case firebase::Variant::kTypeMap: {
    std::map<firebase::Variant, firebase::Variant> srcMap = v.map();
    QVariantMap targetMap;
    for (std::map<firebase::Variant, firebase::Variant>::const_iterator it =
             srcMap.begin();
         it != srcMap.end(); ++it) {
      firebase::Variant key = it->first;
      if (key.type() == firebase::Variant::kTypeStaticString ||
          key.type() == firebase::Variant::kTypeMutableString) {
        targetMap[QString::fromUtf8(key.string_value())] =
            fromFirebaseVariant(it->second);
      } else {
        qDebug() << "FirebaseUtils::fromFirebaseVariant:"
                 << "FirebaseUtils does not support non string keys";
        qDebug() << "FirebaseUtils::fromFirebaseVariant:"
                 << "Got key of type:" << key.TypeName(v.type());
        return QVariant();
      }
    }
    return QVariant(targetMap);
  }
  case firebase::Variant::kTypeVector: {
    std::vector<firebase::Variant> srcLst = v.vector();
    QVariantList targetLst;
    for (std::vector<firebase::Variant>::const_iterator it = srcLst.begin();
         it != srcLst.end(); ++it) {
      targetLst << fromFirebaseVariant(*it);
    }
    return QVariant(targetLst);
  }
  default: {
    qDebug() << "QtFirebaseService::fromFirebaseVariant type:"
             << v.TypeName(v.type()) << " not supported";
  }
  }
  return QVariant();
}

firebase::Variant FirebaseUtils::fromQtVariant(const QVariant &v) {
  switch (v.type()) {
  case QVariant::Bool: {
    return firebase::Variant(v.toBool());
  }
  case QVariant::Int: {
    return firebase::Variant(static_cast<int64_t>(v.toInt()));
  }
  case QVariant::UInt: {
    return firebase::Variant(static_cast<int64_t>(v.toUInt()));
  }
  case QVariant::LongLong: {
    return firebase::Variant(static_cast<int64_t>(v.toLongLong()));
  }
  case QVariant::ULongLong: {
    return firebase::Variant(static_cast<int64_t>(v.toULongLong()));
  }
  case QVariant::Double: {
    return firebase::Variant(v.toDouble());
  }
  case QVariant::String: {
    std::string str(v.toString().toUtf8().constData());
    return firebase::Variant(str);
  }
  case QVariant::ByteArray: {
    std::string str(v.toByteArray().constData());
    return firebase::Variant(str);
  }
  case QVariant::Map: {
    QVariantMap srcMap = v.toMap();
    std::map<std::string, firebase::Variant> targetMap;
    for (QVariantMap::const_iterator it = srcMap.begin(); it != srcMap.end();
         ++it) {
      targetMap[it.key().toUtf8().constData()] = fromQtVariant(it.value());
    }
    return firebase::Variant(targetMap);
  }
  case QVariant::List: {
    QVariantList srcLst = v.toList();
    std::vector<firebase::Variant> targetLst;
    for (QVariantList::const_iterator it = srcLst.begin(); it != srcLst.end();
         ++it) {
      targetLst.push_back(fromQtVariant(*it));
    }
    return firebase::Variant(targetLst);
  }
  default: {
    qDebug() << "QtFirebaseService::fromQtVariant type:" << v.typeName()
             << "not supported";
  }
  }

  return firebase::Variant();
}

void FirebaseUtils::init() {
  qDebug() << "Initialising Util Android " << this;
  if (!PlatformUtils::getNativeWindow()) {

    return;
  }

  if (!mReady) {

#if defined(Q_OS_ANDROID)

    jobject activity = PlatformUtils::getNativeWindow();

    QAndroidJniEnvironment env;

    _firebaseApp = firebase::App::Create(firebase::AppOptions(), env, activity);

#else

    _firebaseApp = firebase::App::Create(firebase::AppOptions());

#endif
    mReady = true;
    _initTimer->stop();

    emit readyChanged();
  }
}
