#!/bin/sh
set -x 

PACKAGE_PATH=com/google/corp/productivity/specialprojects/android/samples/fft
PACKAGE=com.google.corp.productivity.specialprojects.android.samples.fft
MAIN_CLASS=AnalyzeActivity
APP_NAME=AnalyzeSpectrumAnalyzer

AAPT_PATH=$ANDROID_SDK_ROOT/platform-tools/aapt
DX_PATH=$ANDROID_SDK_ROOT/platform-tools/dx
ANDROID_JAR=$ANDROID_SDK_ROOT/platforms/android-17/android.jar
ADB=$ANDROID_SDK_ROOT/platform-tools/adb
DEV_HOME=$PWD

$AAPT_PATH package -f -m -S $DEV_HOME/res -J $DEV_HOME/src -M $DEV_HOME/AndroidManifest.xml -I $ANDROID_JAR

javac -d $DEV_HOME/obj -cp $ANDROID_JAR -sourcepath $DEV_HOME/src $DEV_HOME/src/${PACKAGE_PATH}/*.java

$DX_PATH --dex --output=$DEV_HOME/bin/classes.dex $DEV_HOME/obj

$AAPT_PATH package -f -M $DEV_HOME/AndroidManifest.xml -S $DEV_HOME/res -I $ANDROID_JAR -F $DEV_HOME/bin/$APP_NAME.unsigned.apk $DEV_HOME/bin

 keytool -genkey -validity 10000 -dname "CN=AndroidDebug, O=Android, C=US" -keystore $DEV_HOME/$APP_NAME.keystore -storepass android -keypass android -alias androiddebugkey -keyalg RSA -v -keysize 2048
 jarsigner -sigalg SHA1withRSA -digestalg SHA1 -keystore $DEV_HOME/$APP_NAME.keystore -storepass android -keypass android -signedjar $DEV_HOME/bin/$APP_NAME.signed.apk $DEV_HOME/bin/$APP_NAME.unsigned.apk androiddebugkey
 
