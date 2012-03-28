#include <allegro5/allegro.h>
#include <bass.h>

#include <jni.h>

ALLEGRO_DEBUG_CHANNEL("tmgCrap");

static JavaVM *javavm;
static JNIEnv *thread_env;
static jclass bpc;

typedef struct FuncCall
{
	bool processed;
	const char *funcname;
	const char *filename;
	int handle;
	float vol, pan;
} FuncCall;

#define FUNCBUF_SIZE 100
static FuncCall funcs[FUNCBUF_SIZE];
static int insertPos = 0;
static int processPos = 0;
static ALLEGRO_MUTEX *mutex;
static bool need_return = false;
static int the_return;
static bool inited = false;

static void push(const char *funcname, const char *filename, int handle, float vol, float pan)
{
	al_lock_mutex(mutex);

	FuncCall f;
	f.processed = false;
	f.funcname = funcname;
	f.filename = filename;
	f.handle = handle;
	f.vol = vol;
	f.pan = pan;
	funcs[insertPos++] = f;
	insertPos %= FUNCBUF_SIZE;

	need_return = true;

	al_unlock_mutex(mutex);

	while (need_return);
}

static int wait(const char *funcname, const char *filename, int handle, float vol, float pan)
{
	push(funcname, filename, handle, vol, pan);
	return the_return;
}

static void *thread_proc(void *arg)
{
   	JavaVMAttachArgs attach_args = { JNI_VERSION_1_4, "java_audio_thread", NULL };
      	(*javavm)->AttachCurrentThread(javavm, &thread_env, &attach_args);
	
	jmethodID meth_initSound = (*thread_env)->GetStaticMethodID(thread_env, bpc, "initSound", "()V");
	jmethodID meth_update = (*thread_env)->GetStaticMethodID(thread_env, bpc, "update", "()V");
	jmethodID meth_loadSample = (*thread_env)->GetStaticMethodID(thread_env, bpc, "loadSample", "(Ljava/lang/String;)I");
	jmethodID meth_loadSampleLoop = (*thread_env)->GetStaticMethodID(thread_env, bpc, "loadSampleLoop", "(Ljava/lang/String;)I");
	jmethodID meth_playSampleVolumePan = (*thread_env)->GetStaticMethodID(thread_env, bpc, "playSampleVolumePan", "(IFF)V");
	jmethodID meth_playSampleVolume = (*thread_env)->GetStaticMethodID(thread_env, bpc, "playSampleVolume", "(IF)V");
	jmethodID meth_playSample = (*thread_env)->GetStaticMethodID(thread_env, bpc, "playSample", "(I)V");
	jmethodID meth_stopSample = (*thread_env)->GetStaticMethodID(thread_env, bpc, "stopSample", "(I)V");
	jmethodID meth_loadMusic = (*thread_env)->GetStaticMethodID(thread_env, bpc, "loadMusic", "(Ljava/lang/String;)I");
	jmethodID meth_playMusic = (*thread_env)->GetStaticMethodID(thread_env, bpc, "playMusic", "(I)V");
	jmethodID meth_stopMusic = (*thread_env)->GetStaticMethodID(thread_env, bpc, "stopMusic", "(I)V");
	jmethodID meth_destroyMusic = (*thread_env)->GetStaticMethodID(thread_env, bpc, "destroyMusic", "(I)V");
	jmethodID meth_shutdownBASS = (*thread_env)->GetStaticMethodID(thread_env, bpc, "shutdownBASS", "()V");
	jmethodID meth_destroySample = (*thread_env)->GetStaticMethodID(thread_env, bpc, "destroySample", "(I)V");

	while (1) {
		al_lock_mutex(mutex);
		bool processed = funcs[processPos].processed;
		if (processed) {
			if (inited) {
				(*thread_env)->CallStaticVoidMethod(thread_env, bpc, meth_update);
			}
			al_unlock_mutex(mutex);
			if (inited) {
				al_rest(0.01);
			}
			continue;
		}
		const char *func = funcs[processPos].funcname;
		if (!strcmp(func, "initSound")) {
			(*thread_env)->CallStaticVoidMethod(thread_env, bpc, meth_initSound);
		}
		else if (!strcmp(func, "shutdownBASS")) {
			(*thread_env)->CallStaticVoidMethod(thread_env, bpc, meth_shutdownBASS);
		}
		else if (!strcmp(func, "loadSample")) {
			the_return = (*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_loadSample, (*thread_env)->NewStringUTF(thread_env, funcs[processPos].filename));
		}
		else if (!strcmp(func, "loadSampleLoop")) {
			the_return = (*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_loadSampleLoop, (*thread_env)->NewStringUTF(thread_env, funcs[processPos].filename));
		}
		else if (!strcmp(func, "loadMusic")) {
			the_return = (*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_loadMusic, (*thread_env)->NewStringUTF(thread_env, funcs[processPos].filename));
		}
		else if (!strcmp(func, "playSampleVolumePan")) {
			(*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_playSampleVolumePan, funcs[processPos].handle, funcs[processPos].vol, funcs[processPos].pan);
		}
		else if (!strcmp(func, "playSampleVolume")) {
			(*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_playSampleVolume, funcs[processPos].handle, funcs[processPos].vol);
		}
		else if (!strcmp(func, "playSample")) {
			(*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_playSample, funcs[processPos].handle);
		}
		else if (!strcmp(func, "stopSample")) {
			(*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_stopSample, funcs[processPos].handle);
		}
		else if (!strcmp(func, "playMusic")) {
			(*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_playMusic, funcs[processPos].handle);
		}
		else if (!strcmp(func, "stopMusic")) {
			(*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_stopMusic, funcs[processPos].handle);
		}
		else if (!strcmp(func, "destroyMusic")) {
			(*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_destroyMusic, funcs[processPos].handle);
		}
		else if (!strcmp(func, "destroySample")) {
			(*thread_env)->CallStaticIntMethod(thread_env, bpc, meth_destroySample, funcs[processPos].handle);
		}
		funcs[processPos].processed = true;
		processPos++;
		processPos %= FUNCBUF_SIZE;
		need_return = false;
		al_unlock_mutex(mutex);
	}

	return NULL;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	(void)reserved;

	javavm = vm;
	JNIEnv *java_env;
	(*javavm)->GetEnv(javavm, (void **)&java_env, JNI_VERSION_1_4);

	jclass cls = (*java_env)->FindClass(java_env, "org/liballeg/app/BassPump");
	bpc = (*java_env)->NewGlobalRef(java_env, cls);
	
	return JNI_VERSION_1_4;
}

void bass_initSound(void)
{
	mutex = al_create_mutex();

	int i;
	for (i = 0; i < FUNCBUF_SIZE; i++) {
		funcs[i].processed = true;
	}

	al_run_detached_thread(thread_proc, NULL);

	push("initSound", NULL, 0, 0, 0);
	inited = true;
}

HSAMPLE bass_loadSample(const char *name)
{
	return (HSAMPLE)wait("loadSample", name, 0, 0, 0);
}

HSAMPLE bass_loadSampleLoop(const char *name)
{
	return (HSAMPLE)wait("loadSampleLoop", name, 0, 0, 0);
}

void bass_playSampleVolumePan(HSAMPLE s, float vol, float pan)
{
	push("playSampleVolumePan", NULL, s, vol, pan);
}

void bass_playSampleVolume(HSAMPLE s, float vol)
{
	push("playSampleVolume", NULL, s, vol, 0);
}

void bass_playSample(HSAMPLE s)
{
	push("playSample", NULL, s, 0, 0);
}

void bass_stopSample(HSAMPLE s)
{
	push("stopSample", NULL, s, 0, 0);
}

HMUSIC bass_loadMusic(const char *name)
{
	return (HMUSIC)wait("loadMusic", name, 0, 0, 0);
}

void bass_playMusic(HMUSIC music)
{
	push("playMusic", NULL, music, 0, 0);
}

void bass_stopMusic(HMUSIC music)
{
	push("stopMusic", NULL, music, 0, 0);
}

void bass_destroyMusic(HMUSIC music)
{
	push("destroyMusic", NULL, music, 0, 0);
}

void bass_shutdownBASS(void)
{
	push("shutdownBASS", NULL, 0, 0, 0);
}

void bass_destroySample(HSAMPLE s)
{
	push("destroySample", NULL, s, 0, 0);
}

