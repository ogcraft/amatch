#!/bin/sh

#export ANDROID_NDK_ROOT=$HOME/src/android-ndk-r8c

rm -rf src/amatch_generated
mkdir -p src/amatch_generated

swig -java -package amatch_generated -includeall -verbose -outdir src/amatch_generated -c++ -I/usr/local/include -I/System/Library/Frameworks/JavaVM.framework/Headers -I./jni -o jni/amatch_interface.cpp amatch_interface.i

$ANDROID_NDK_ROOT/ndk-build TARGET_PLATFORM=android-9 V=1




