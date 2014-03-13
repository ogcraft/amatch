set -x

APPNAME=AmatchTest
MODE=release
KEYSTORE=$APPNAME.keystore

UNSIGNEDAPK=$APPNAME-$MODE-unsigned.apk
SIGNEDAPK=$APPNAME-$MODE-signed.apk

ant $MODE

if [ ! -f $KEYSTORE ]
then
	keytool -genkey -validity 10000 -dname "CN=AndroidDebug, O=Android, C=US" -keystore $KEYSTORE -storepass android -keypass android -alias androiddebugkey -keyalg RSA -v -keysize 2048
fi

jarsigner -sigalg SHA1withRSA -digestalg SHA1 -keystore $KEYSTORE -storepass android -keypass android -signedjar bin/$SIGNEDAPK bin/$UNSIGNEDAPK androiddebugkey

#adb install bin/$SIGNEDAPK 
