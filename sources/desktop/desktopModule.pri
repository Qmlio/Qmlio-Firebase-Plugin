message(Configuring Desktop Module)

isEmpty(FIREBASE_SDK_PATH){
message("Firebase sdk path  is invalid, Qmlio plugin may fail to build. ")
}
else{
message("Using firebase_sdk_path ($$FIREBASE_SDK_PATH)")
}

LIBS += -LD:/firebase_cpp_sdk/libs/windows/VS2015/MD/x86/Release/firebase_auth.lib


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

INCLUDEPATH += $$PWD
INCLUDEPATH += $$FIREBASE_SDK_PATH/include

HEADERS += \
   $$PWD/firebaseauth.h \
   $$PWD/utils/firebaseutils.h \
   $$PWD/utils/task.h \
   $$PWD/firebasestorage.h \
   $$PWD/storagequery.h \
   $$PWD/firebasedatabase.h \
   $$PWD/databasequery.h \
   $$PWD/utils/taskmanager.h \
   $$PWD/firebasemessaging.h \
   $$PWD/utils/worker.h

SOURCES += \
   $$PWD/firebaseauth.cpp \
   $$PWD/utils/firebaseutils.cpp \
   $$PWD/utils/task.cpp \
   $$PWD/firebasestorage.cpp \
   $$PWD/storagequery.cpp \
   $$PWD/firebasedatabase.cpp \
   $$PWD/databasequery.cpp \
   $$PWD/utils/taskmanager.cpp \
   $$PWD/firebasemessaging.cpp\
   $$PWD/utils/worker.cpp


