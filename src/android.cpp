#include <allegro5/allegro.h>

#include <jni.h>
#include "java.h"

#include "monster2.hpp"

/*
static bool ok = false;
*/

extern "C" {
JNIEnv *_al_android_get_jnienv();
void __jni_checkException(JNIEnv *env, const char *file, const char *fname, int line);
jobject _al_android_activity_object();
}

#define _jni_checkException(env) __jni_checkException(env, __FILE__, __FUNCTION__, __LINE__)

#define _jni_call(env, rett, method, args...) ({ \
   rett ret = env->method(args); \
   _jni_checkException(env); \
   ret; \
})

#define _jni_callv(env, method, args...) ({ \
   env->method(args); \
   _jni_checkException(env); \
})

#define _jni_callVoidMethodV(env, obj, name, sig, args...) ({ \
   jclass class_id = _jni_call(env, jclass, GetObjectClass, obj); \
   \
   jmethodID method_id = _jni_call(env, jmethodID, GetMethodID, class_id, name, sig); \
   if(method_id == NULL) { \
   } else { \
      _jni_callv(env, CallVoidMethod, obj, method_id, ##args); \
   } \
   \
   _jni_callv(env, DeleteLocalRef, class_id); \
})

#define _jni_callBooleanMethodV(env, obj, name, sig, args...) ({ \
   jclass class_id = _jni_call(env, jclass, GetObjectClass, obj); \
   \
   jmethodID method_id = _jni_call(env, jmethodID, GetMethodID, class_id, name, sig); \
   \
   bool ret = false; \
   if(method_id == NULL) { \
   } \
   else { \
      ret = _jni_call(env, bool, CallBooleanMethod, obj, method_id, ##args); \
   } \
   \
   _jni_callv(env, DeleteLocalRef, class_id); \
   \
   ret; \
})

#define _jni_callIntMethodV(env, obj, name, sig, args...) ({ \
   jclass class_id = _jni_call(env, jclass, GetObjectClass, obj); \
   \
   jmethodID method_id = _jni_call(env, jmethodID, GetMethodID, class_id, name, sig); \
   \
   int ret = -1; \
   if(method_id == NULL) { \
      /*ALLEGRO_DEBUG("couldn't find method :(");*/ \
   } \
   else { \
      ret = _jni_call(env, int, CallIntMethod, obj, method_id, ##args); \
   } \
   \
   _jni_callv(env, DeleteLocalRef, class_id); \
   \
   ret; \
})

#define _jni_callIntMethod(env, obj, name) _jni_callIntMethodV(env, obj, name, "()I");

static jobject _jni_callObjectMethod(JNIEnv *env, jobject object, const char *name, const char *sig)
{
   jclass class_id = _jni_call(env, jclass, GetObjectClass, object);
   jmethodID method_id = _jni_call(env, jmethodID, GetMethodID, class_id, name, sig);
   jobject ret = _jni_call(env, jobject, CallObjectMethod, object, method_id);
   _jni_callv(env, DeleteLocalRef, class_id);
   
   return ret;
}

const char * get_sdcarddir()
{
	static char buf[2000];

	jstring s =
		(jstring)_jni_callObjectMethod(
			_al_android_get_jnienv(),
			_al_android_activity_object(),
   			"getSDCardPrivateDir",
			"()Ljava/lang/String;"
		);
	
	if (s == NULL)
		return "";
	
	const char *native = _al_android_get_jnienv()->GetStringUTFChars(s, 0);

	strcpy(buf, native);

	_al_android_get_jnienv()->ReleaseStringUTFChars(s, native);

	_al_android_get_jnienv()->DeleteLocalRef(s);

	return buf;
}

/*
void goHome()
{
	_jni_callVoidMethodV(
		_al_android_get_jnienv(),
		_al_android_activity_object(),
		"goHome",
		"()V"
	);

}
*/

bool isAndroidConsole()
{
#if defined OUYA
	return true;
#endif

	static bool first = true;
	static bool is_console;

	if (!first) {
		return is_console;
	}

	first = false;
	is_console = false;

	JNIEnv *env = _al_android_get_jnienv();
	jobject object = _al_android_activity_object();

	jclass class_id = _jni_call(env, jclass, GetObjectClass, object);
	jmethodID method_id = _jni_call(env, jmethodID, GetMethodID, class_id, "getModel", "()Ljava/lang/String;");
	jstring s = (jstring)_jni_call(env, jobject, CallObjectMethod, object, method_id);

	if (s == NULL)
		return false;
	
	const char *native = _al_android_get_jnienv()->GetStringUTFChars(s, 0);

	if (strstr(native, "OUYA")) {
		is_console = true;
	}
	else if (!strncmp(native, "AFTB", 4) || !strncmp(native, "AFTM", 4)) {
		jmethodID method_id2 = _jni_call(env, jmethodID, GetMethodID, class_id, "getManufacturer", "()Ljava/lang/String;");
		jstring s2 = (jstring)_jni_call(env, jobject, CallObjectMethod, object, method_id2);
		const char *native2 = _al_android_get_jnienv()->GetStringUTFChars(s2, 0);
		if (!strcmp(native2, "Amazon")) {
			is_console = true;
		}
		_al_android_get_jnienv()->ReleaseStringUTFChars(s2, native2);
		_jni_callv(env, DeleteLocalRef, s2);
	}

	_al_android_get_jnienv()->ReleaseStringUTFChars(s, native);
	_jni_callv(env, DeleteLocalRef, s);
	_jni_callv(env, DeleteLocalRef, class_id);

	return is_console;
}

bool gamepadConnected()
{
	bool ret = false;

	/* FIXME: 1 here means ignore accelerometer */
	if (al_is_joystick_installed() && al_get_num_joysticks() > 1) {
		ret = true;
	}

	if (!ret) {
		ret = isAndroidConsole();
	}

	return ret;
}

void logString(const char *s)
{
	jstring S = _al_android_get_jnienv()->NewStringUTF(s);

	_jni_callVoidMethodV(
		_al_android_get_jnienv(),
		_al_android_activity_object(),
		"logString",
		"(Ljava/lang/String;)V",
		S
	);
	
	_al_android_get_jnienv()->DeleteLocalRef(S);
}

#ifdef __cplusplus
extern "C" {
#endif

static float backup_music_volume, backup_ambience_volume;

JNIEXPORT void JNICALL Java_ca_nooskewl_monsterrpg2_MyBroadcastReceiver_pauseSound
  (JNIEnv *env, jobject obj)
{
	backup_music_volume = getMusicVolume();
	backup_ambience_volume = getAmbienceVolume();
	setMusicVolume(0.0);
	setAmbienceVolume(0.0);
}

JNIEXPORT void JNICALL Java_ca_nooskewl_monsterrpg2_MyBroadcastReceiver_resumeSound
  (JNIEnv *env, jobject obj)
{
	setMusicVolume(backup_music_volume);
	setAmbienceVolume(backup_ambience_volume);
}

JNIEXPORT void JNICALL Java_com_nooskewl_monsterrpg2_MyBroadcastReceiver_pauseSound
  (JNIEnv *env, jobject obj)
{
	backup_music_volume = getMusicVolume();
	backup_ambience_volume = getAmbienceVolume();
	setMusicVolume(0.0);
	setAmbienceVolume(0.0);
}

JNIEXPORT void JNICALL Java_com_nooskewl_monsterrpg2_MyBroadcastReceiver_resumeSound
  (JNIEnv *env, jobject obj)
{
	setMusicVolume(backup_music_volume);
	setAmbienceVolume(backup_ambience_volume);
}

#define NUM_ACHIEVEMENTS 30

const char *achievement_ids[] = {
	/* codes from Google Play Developer Console */
	"CgkIyobkjZsMEAIQAQ",
	"CgkIyobkjZsMEAIQAg",
	"CgkIyobkjZsMEAIQAw",
	"CgkIyobkjZsMEAIQBA",
	"CgkIyobkjZsMEAIQBQ",
	"CgkIyobkjZsMEAIQBg",
	"CgkIyobkjZsMEAIQBw",
	"CgkIyobkjZsMEAIQCA",
	"CgkIyobkjZsMEAIQCQ",
	"CgkIyobkjZsMEAIQCg",
	"CgkIyobkjZsMEAIQCw",
	"CgkIyobkjZsMEAIQDA",
	"CgkIyobkjZsMEAIQDQ",
	"CgkIyobkjZsMEAIQDg",
	"CgkIyobkjZsMEAIQDw",
	"CgkIyobkjZsMEAIQEA",
	"CgkIyobkjZsMEAIQEg",
	"CgkIyobkjZsMEAIQEw",
	"CgkIyobkjZsMEAIQFA",
	"CgkIyobkjZsMEAIQFQ",
	"CgkIyobkjZsMEAIQFg",
	"CgkIyobkjZsMEAIQFw",
	"CgkIyobkjZsMEAIQGA",
	"CgkIyobkjZsMEAIQGQ",
	"CgkIyobkjZsMEAIQGg",
	"CgkIyobkjZsMEAIQGw",
	"CgkIyobkjZsMEAIQHA",
	"CgkIyobkjZsMEAIQHQ",
	"CgkIyobkjZsMEAIQHg",
	"CgkIyobkjZsMEAIQHw"
};

void achieve(const char *id)
{
	JNIEnv *env = _al_android_get_jnienv();

	const char *s = NULL;

	for (int i = 0; i < NUM_ACHIEVEMENTS; i++) {
		if (!strcmp(achievement_ids[i], id)) {
			s = achievement_ids[i];
			break;
		}
	}

	if (s == NULL) {
		return;
	}

	jstring S = env->NewStringUTF(s);

	_jni_callVoidMethodV(
		env,
		_al_android_activity_object(),
		"unlock_achievement",
		"(Ljava/lang/String;)V",
		S
	);

	env->DeleteLocalRef(S);
}

void do_milestone(int num, bool visual)
{
	(void)visual;

	int ach[NUM_ACHIEVEMENTS] = {
		3,
		15,
		20,
		26,
		30,
		40,
		43,
		48,
		56,
		59,
		65,
		67,
		74,
		76,
		89,
		87,
		96,
		102,
		98,
		123,
		180,
		135,
		149,
		153,
		154,
		167,
		168,
		171,
		176,
		177
	};

	for (int i = 0; i < NUM_ACHIEVEMENTS; i++) {
		if (num+1 == ach[i]) {
			achieve(achievement_ids[i]);
			return;
		}
	}
}

void init_play_services()
{
	_jni_callVoidMethodV(
		_al_android_get_jnienv(),
		_al_android_activity_object(),
		"init_play_services",
		"()V"
	);
}
#ifdef __cplusplus
}
#endif

#if defined OUYA
int isPurchased()
{
	int ret = _jni_callIntMethod(
		_al_android_get_jnienv(),
		_al_android_activity_object(),
		"isPurchased"
	);

	return ret;
}

void queryPurchased()
{
	_jni_callVoidMethodV(
		_al_android_get_jnienv(),
		_al_android_activity_object(),
		"queryPurchased",
		"()V"
	);
}

void doIAP()
{
	_jni_callVoidMethodV(
		_al_android_get_jnienv(),
		_al_android_activity_object(),
		"doIAP",
		"()V"
	);
}

int checkPurchased()
{
	queryPurchased();

	int purchased = -1;

	do {
		purchased = isPurchased();
		if (purchased == -1) {
			al_rest(0.01);
		}
	} while (purchased == -1);

	return purchased;
}
#endif
