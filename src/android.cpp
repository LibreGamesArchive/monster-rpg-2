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

// return true on success
bool get_clipboard(char *buf, int len)
{
	jstring s =
		(jstring)_jni_callObjectMethod(
			_al_android_get_jnienv(),
			_al_android_activity_object(),
			"getClipData",
			"()Ljava/lang/String;"
		);
	
	if (s == NULL)
		return false;
	
	const char *native = _al_android_get_jnienv()->GetStringUTFChars(s, 0);

	strncpy(buf, native, len);

	_al_android_get_jnienv()->ReleaseStringUTFChars(s, native);

	return true;
}

void set_clipboard(char *buf)
{
	jstring saveS = _al_android_get_jnienv()->NewStringUTF(buf);

	_jni_callVoidMethodV(
		_al_android_get_jnienv(),
		_al_android_activity_object(),
		"setClipData",
		"(Ljava/lang/String;)V",
		saveS
	);

	_al_android_get_jnienv()->DeleteLocalRef(saveS);
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
