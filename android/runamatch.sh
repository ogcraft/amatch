$ANDROID_NDK_ROOT/ndk-build TARGET_PLATFORM=android-9 V=1 && adb push ./libs/armeabi-v7a/amatch /data/local/tmp && adb shell /data/local/tmp/amatch
