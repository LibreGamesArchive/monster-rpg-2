# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liballegro_monolith-prebuilt
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/liballegro_monolith.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libbass-prebuilt
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libbass.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_ALLOW_UNDEFINED_SYMBOLS := false
LOCAL_MODULE := monsterrpg2
LOCAL_SRC_FILES := \
	../../../src/3d.cpp\
	../../../src/android.cpp\
	../../../src/Animation.cpp\
	../../../src/AnimationSet.cpp\
	../../../src/Area.cpp\
	../../../src/atlas.cpp\
	../../../src/atlas_accessor.cpp\
	../../../src/battle.cpp\
	../../../src/CombatActions.cpp\
	../../../src/Combatant.cpp\
	../../../src/CombatEntity.cpp\
	../../../src/CombatEnemy.cpp\
	../../../src/CombatPlayer.cpp\
	../../../src/Configuration.cpp\
	../../../src/debug.cpp\
	../../../src/equipment.cpp\
	../../../src/Frame.cpp\
	../../../src/GenericEffect.cpp\
	../../../src/graphics.cpp\
	../../../src/Image.cpp\
	../../../src/Input.cpp\
	../../../src/inventory.cpp\
	../../../src/io.cpp\
	../../../src/Items.cpp\
	../../../src/lander.cpp\
	../../../src/monster2.cpp\
	../../../src/my_load_bitmap.cpp\
	../../../src/Object.cpp\
	../../../src/pause.cpp\
	../../../src/Player.cpp\
	../../../src/redundant.cpp\
	../../../src/script.cpp\
	../../../src/shooter.cpp\
	../../../src/sound.cpp\
	../../../src/Spells.cpp\
	../../../src/tgui.cpp\
	../../../src/Tile.cpp\
	../../../src/translate.cpp\
	../../../src/util.cpp\
	../../../src/widgets.cpp\
	../../../src/xml.cpp\
	../../../src/init.cpp

LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -Wall -O2 -I$(ANDROID_DIR)/$(TARGET_ARCH_ABI)/include -I../../include -DALLEGRO_UNSTABLE -DOUYA $(NO_DRM)

LOCAL_SHARED_LIBRARIES := liballegro_monolith-prebuilt libbass-prebuilt
LOCAL_LDLIBS := -L$(ANDROID_DIR)/$(TARGET_ARCH_ABI)/lib -llog -lGLESv1_CM -lstdc++ -lphysfs -llua5.2 -lz

include $(BUILD_SHARED_LIBRARY)
