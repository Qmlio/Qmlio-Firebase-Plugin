message(Welcome to Qmlio firebase plugin)

isEmpty(FIREBASE_SDK_PATH){
error("Firebase sdk path  is invalid, plugin may fail to build. ")
}
else{
message("Using firebase_sdk_path ($$FIREBASE_SDK_PATH)")
}



android|ios {
    include(target/mobileModule.pri)
} else {
    include(desktop/desktopModule.pri)
}
