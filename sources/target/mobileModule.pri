message(Configuring mobile Module to Qmlio firebase plugin)

INCLUDEPATH += $$FIREBASE_SDK_PATH/include
INCLUDEPATH += $$PWD

DEFINES += QMLIO_FIREBASE_APP
contains(QMLIO_FIREBASE_CONFIG,"auth"){
message("Configuring Firebase auth")
DEFINES += QMLIO_FIREBASE_AUTH
}

contains(QMLIO_FIREBASE_CONFIG,"database"){
message("Configuring Firebase Database")
DEFINES += QMLIO_FIREBASE_DATABASE
}

contains(QMLIO_FIREBASE_CONFIG,"storage"){
message("Configuring Firebase storage")
DEFINES += QMLIO_FIREBASE_STORAGE
}

contains(QMLIO_FIREBASE_CONFIG,"messaging"){
message("Configuring Firebase messaging")
DEFINES += QMLIO_FIREBASE_MESSAGING
}

contains(QMLIO_FIREBASE_CONFIG,"dynamic_links"){
message("Configuring Firebase auth")
DEFINES += QMLIO_FIREBASE_DYNAMIC_LINKS
}

android {

message(" Configuring Android. ")


FIREBASE_SDK_LIBS_PREFIX = "firebase_"

QT += androidextras

FIREBASE_SDK_LIBS_PATH = $$FIREBASE_SDK_PATH/libs/android/$$ANDROID_TARGET_ARCH/gnustl
DEPENDPATH += $$FIREBASE_SDK_LIBS_PATH




# Auth
contains(DEFINES,QMLIO_FIREBASE_AUTH) {
    message( "Including  Firebase Auth for android" )

    PRE_TARGETDEPS += $$FIREBASE_SDK_LIBS_PATH/lib$${FIREBASE_SDK_LIBS_PREFIX}auth.a
    LIBS += -L$$FIREBASE_SDK_LIBS_PATH -l$${FIREBASE_SDK_LIBS_PREFIX}auth
}

# Database
contains(DEFINES,QMLIO_FIREBASE_DATABASE) {
    message( "Including  Firebase database for android" )

    PRE_TARGETDEPS += $$FIREBASE_SDK_LIBS_PATH/lib$${FIREBASE_SDK_LIBS_PREFIX}database.a
    LIBS += -L$$FIREBASE_SDK_LIBS_PATH -l$${FIREBASE_SDK_LIBS_PREFIX}database
}
# Storage
contains(DEFINES,QMLIO_FIREBASE_STORAGE) {
    message( "Including  Firebase storage for android" )

    PRE_TARGETDEPS += $$FIREBASE_SDK_LIBS_PATH/lib$${FIREBASE_SDK_LIBS_PREFIX}storage.a
    LIBS += -L$$FIREBASE_SDK_LIBS_PATH -l$${FIREBASE_SDK_LIBS_PREFIX}storage
}
# Messaging
contains(DEFINES,QMLIO_FIREBASE_STORAGE) {
    message( "Including  Firebase messaging for android" )

    PRE_TARGETDEPS += $$FIREBASE_SDK_LIBS_PATH/lib$${FIREBASE_SDK_LIBS_PREFIX}messaging.a
    LIBS += -L$$FIREBASE_SDK_LIBS_PATH -l$${FIREBASE_SDK_LIBS_PREFIX}messaging
}
# Dynamic links
contains(DEFINES,QMLIO_FIREBASE_DYNAMIC_LINKS) {
    message( "Including  Firebase Dynamic links for android" )

    PRE_TARGETDEPS += $$FIREBASE_SDK_LIBS_PATH/lib$${FIREBASE_SDK_LIBS_PREFIX}dynamic_links.a
    LIBS += -L$$FIREBASE_SDK_LIBS_PATH -l$${FIREBASE_SDK_LIBS_PREFIX}dynamic_links
}
# App
contains(DEFINES,QMLIO_FIREBASE_APP) {
    message( "Including  Firebase app for android" )

LIBS += -L$$FIREBASE_SDK_LIBS_PATH -l$${FIREBASE_SDK_LIBS_PREFIX}app


 }
}
HEADERS += \
   $$PWD/utils/platformutils.h \
   $$PWD/firebaseauth.h \
   $$PWD/utils/firebaseutils.h \
   $$PWD/utils/task.h \
   $$PWD/firebasestorage.h \
   $$PWD/storagequery.h \
   $$PWD/storagelistner.h \
   $$PWD/firebasedatabase.h \
   $$PWD/databasequery.h \
   $$PWD/databasechildlistner.h \
   $$PWD/utils/taskmanager.h \
    $$PWD/utils/worker.h \
  $$PWD/utils/transactionmanager.h \
   $$PWD/utils/transaction.h\
    $$PWD/firebasemessaging.h \
    $$PWD/messagelistener.h

SOURCES += $$PWD/utils/platformutils.cpp \
   $$PWD/firebaseauth.cpp \
   $$PWD/utils/firebaseutils.cpp \
   $$PWD/utils/task.cpp \
  $$PWD/utils/transactionmanager.cpp \
   $$PWD/utils/transaction.cpp \
   $$PWD/firebasestorage.cpp \
   $$PWD/storagequery.cpp \
   $$PWD/storagelistner.cpp \
   $$PWD/firebasedatabase.cpp \
   $$PWD/databasequery.cpp \
   $$PWD/databasechildlistner.cpp \
   $$PWD/utils/taskmanager.cpp \
    $$PWD/utils/worker.cpp \
    $$PWD/firebasemessaging.cpp \
    $$PWD/messagelistener.cpp


