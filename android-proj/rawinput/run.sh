#!/bin/bash

export ANDROID_NDK_TOOLCHAIN_ROOT=~/code/android-toolchain

ant clean
ant debug

echo install
/Users/trent/code/android-sdk-macosx/platform-tools/adb -d install -r bin/RawInputIME-debug.apk

