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

void openURL(const char *url)
{
	jstring urlS = _al_android_get_jnienv()->NewStringUTF(url);

	_jni_callVoidMethodV(
		_al_android_get_jnienv(),
		_al_android_activity_object(),
		"openURL",
		"(Ljava/lang/String;)V",
		urlS
	);
	
	_al_android_get_jnienv()->DeleteLocalRef(urlS);

	/*
	bool done = false;
	while (!done) {
		done = _jni_callBooleanMethodV(
			_al_android_get_jnienv(),
			_al_android_activity_object(),
			"browserDone",
			"()Z"
		);
		if (!done) {
			al_rest(0.01);
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}
		}
	}

	al_rest(0.5);
	clear_input_events();
	getInput()->set(false, false, false, false, false, false, false);
	al_flush_event_queue(input_event_queue);
	*/
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

bool wifiConnected()
{
	if (isOuya()) {
		return true;
	}

	bool ret = _jni_callBooleanMethodV(
		_al_android_get_jnienv(),
		_al_android_activity_object(),
		"wifiConnected",
		"()Z"
	);

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

bool isOuya()
{
	static bool first = true;
	static bool is_ouya;

	if (!first) {
		return is_ouya;
	}

	first = false;
	is_ouya = false;

	JNIEnv *env = _al_android_get_jnienv();
	jobject object = _al_android_activity_object();

	jclass class_id = _jni_call(env, jclass, GetObjectClass, object);
	jmethodID method_id = _jni_call(env, jmethodID, GetMethodID, class_id, "getModel", "()Ljava/lang/String;");
	jstring s = (jstring)_jni_call(env, jobject, CallObjectMethod, object, method_id);

	if (s == NULL)
		return false;
	
	const char *native = _al_android_get_jnienv()->GetStringUTFChars(s, 0);

	if (strstr(native, "OUYA")) {
		is_ouya = true;
	}

	_al_android_get_jnienv()->ReleaseStringUTFChars(s, native);
	_jni_callv(env, DeleteLocalRef, s);
	_jni_callv(env, DeleteLocalRef, class_id);

	return is_ouya;
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

#ifdef OUYA
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
