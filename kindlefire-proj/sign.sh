jarsigner -verbose -sigalg MD5withRSA -digestalg SHA1 -keystore my-release-key.keystore $1 alias_name
jarsigner -verify $1
/Users/trent/code/android-sdk-macosx/tools/zipalign -v 4 $1 $2
