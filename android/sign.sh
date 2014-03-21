#!/bin/sh
set -x 

PACKAGE=com.ogcraft.amatchtest
APP_NAME=AmatchTest
MODE=release
DEV_HOME=$PWD

rm $DEV_HOME/$APP_NAME.keystore

keytool -genkey -validity 10000 -dname "CN=AndroidDebug, O=Android, C=US" -keystore $DEV_HOME/$APP_NAME.keystore -storepass android -keypass android -alias androiddebugkey -keyalg RSA -v -keysize 2048
jarsigner -sigalg SHA1withRSA -digestalg SHA1 -keystore $DEV_HOME/$APP_NAME.keystore -storepass android -keypass android -signedjar $DEV_HOME/bin/$APP_NAME.signed.apk $DEV_HOME/bin/$APP_NAME-$MODE-unsigned.apk androiddebugkey
 
