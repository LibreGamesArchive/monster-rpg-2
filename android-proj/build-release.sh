#!/bin/bash

export ANDROID_NDK_TOOLCHAIN_ROOT=~/code/android-toolchain
if [ "x$1" = "xCLEAN" ] ; then
	V=1 ~/code/android-ndk-r7b/ndk-build clean
fi
V=1 ~/code/android-ndk-r7b/ndk-build

ant clean
ant release

