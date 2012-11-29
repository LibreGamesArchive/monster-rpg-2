#include "monster2.hpp"

#include "tftp_get.h"

#ifdef ALLEGRO_ANDROID
#define ASSERT ALLEGRO_ASSERT
extern "C" {
#include <allegro5/internal/aintern_android.h>
}
#include "sound-android.hpp"
#include <physfs.h>
#endif

static void destroyMusic(void);

static std::string shutdownMusicName = "";
static std::string shutdownAmbienceName = "";

bool sound_inited = false;
static int total_samples = 0;
static int curr_sample = 0;

static std::map<std::string, MSAMPLE> preloaded_samples;

static DWORD music = 0;
static DWORD ambience = 0;
std::string musicName = "";
std::string ambienceName = "";
static QWORD music_loop_start = 0;
static QWORD ambience_loop_start = 0;
static float musicVolume = 1.0f;
static float ambienceVolume = 1.0f;
	
static std::string preloaded_names[] = {
#ifdef LITE
	"bolt.ogg",
	"ice1.ogg",
	"fire1.ogg",
	"Cure.ogg",
	"Elixir.ogg",
	"Heal.ogg",
	"HolyWater.ogg",
	"Revive.ogg",
	"appear.ogg",
	"battle.ogg",
	"blip.ogg",
	"boss.ogg",
	"chest.ogg",
	"door.ogg",
	"enemy_die.ogg",
	"error.ogg",
	"fall.ogg",
	"freeze.ogg",
	"hit.ogg",
	"melee_woosh.ogg",
	"new_party_member.ogg",
	"nooskewl.ogg",
	"pistol.ogg",
	"select.ogg",
	"sleep.ogg",
	"swipe.ogg",
	"woosh.ogg",
#else
	"Bolt2.ogg",
	"Bolt3.ogg",
	"Fire2.ogg",
	"Fire3.ogg",
	"Ice3.ogg",
	"bolt.ogg",
	"fire1.ogg",
	"ice1.ogg",
	"Charm.ogg",
	"Cure.ogg",
	"Elixir.ogg",
	"Heal.ogg",
	"HolyWater.ogg",
	"Portal.ogg",
	"Quick.ogg",
	"Revive.ogg",
	"Slow.ogg",
	"Stun.ogg",
	"Vampire.ogg",
	"appear.ogg",
	"battle.ogg",
	"blip.ogg",
	"boss.ogg",
	"bow_draw.ogg",
	"bow_release_and_draw.ogg",
	"cartoon_fall.ogg",
	"chest.ogg",
	"ching.ogg",
	"door.ogg",
	"enemy_die.ogg",
	"enemy_explosion.ogg",
	"error.ogg",
	"explosion.ogg",
	"fall.ogg",
	"freeze.ogg",
	"hit.ogg",
	"ignite.ogg",
	"low_cackle.ogg",
	"melee_woosh.ogg",
	"new_party_member.ogg",
	"nooskewl.ogg",
	"pistol.ogg",
	"push.ogg",
	"select.ogg",
	"sleep.ogg",
	"slice.ogg",
	"splash.ogg",
	"staff_fly.ogg",
	"staff_poof.ogg",
	"swipe.ogg",
	"torpedo.ogg",
	"woosh.ogg",
#endif
	""
};

#ifdef ALLEGRO_ANDROID
static void CALLBACK my_close(void *user)
{
	ALLEGRO_FILE *f = (ALLEGRO_FILE *)user;
	al_fclose(f);
}
static QWORD CALLBACK my_len(void *user)
{
	ALLEGRO_FILE *f = (ALLEGRO_FILE *)user;
	QWORD sz = al_fsize(f);
	return sz;
}
static DWORD CALLBACK my_read(void *buf, DWORD length, void *user)
{
	ALLEGRO_FILE *f = (ALLEGRO_FILE *)user;
	DWORD r = al_fread(f, buf, length);
	return r;
}
static BOOL CALLBACK my_seek(QWORD offset, void *user)
{
	ALLEGRO_FILE *f = (ALLEGRO_FILE *)user;
	BOOL b = al_fseek(f, offset, ALLEGRO_SEEK_SET);
	return b;
}

static void CALLBACK physfs_my_close(void *user)
{
	PHYSFS_File *f = (PHYSFS_File *)user;
	PHYSFS_close(f);
}
static QWORD CALLBACK physfs_my_len(void *user)
{
	PHYSFS_File *f = (PHYSFS_File *)user;
	PHYSFS_sint64 sz = PHYSFS_fileLength(f);
	return sz;
}
static DWORD CALLBACK physfs_my_read(void *buf, DWORD length, void *user)
{
	PHYSFS_File *f = (PHYSFS_File *)user;
	DWORD r = PHYSFS_read(f, buf, 1, length);
	return r;
}
static BOOL CALLBACK physfs_my_seek(QWORD offset, void *user)
{
	PHYSFS_File *f = (PHYSFS_File *)user;
	bool ret = PHYSFS_seek(f, offset);
	return ret;
}
#endif

#ifdef ALLEGRO_IPHONE
extern HPLUGIN BASSFLACplugin;
#else
static HPLUGIN BASSFLACplugin;
#endif

void initSound(void)
{
	sound_inited = true;
	
	for (int i = 0; preloaded_names[i] != ""; i++) {
		total_samples++;
	}

#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		initSound_oldandroid();
		return;
	}
#endif
	
#ifdef ALLEGRO_ANDROID
	fileprocs.close = my_close;
	fileprocs.length = my_len;
	fileprocs.read = my_read;
	fileprocs.seek = my_seek;

	physfs_fileprocs.close = physfs_my_close;
	physfs_fileprocs.length = physfs_my_len;
	physfs_fileprocs.read = physfs_my_read;
	physfs_fileprocs.seek = physfs_my_seek;
#endif

#ifdef ALLEGRO_RASPBERRYPI
	BASS_SetConfig(BASS_CONFIG_BUFFER, 3000);
	if (!BASS_Init(-1, 22050, 0, NULL, NULL)) {
#else
	if (!BASS_Init(-1, 44100, 0, NULL, NULL)) {
#endif
		int code = BASS_ErrorGetCode();
		ALLEGRO_DEBUG("BASS_Init failed (%d). Failing or falling back", code);
#ifdef ALLEGRO_ANDROID
		is_android_lessthan_2_3 = true;
		goto old;
#else
		return;
#endif
	}

#if defined ALLEGRO_WINDOWS
	BASSFLACplugin = BASS_PluginLoad("bassflac.dll", 0);
#elif defined LINUX_GENERIC
	char buf1[MAX_PATH];
	char buf2[MAX_PATH];
	getcwd(buf1, MAX_PATH);
	sprintf(buf2, "%s/libbassflac.so", buf1);
	BASSFLACplugin = BASS_PluginLoad(buf2, 0);
#elif defined IPHONE
	BASS_PluginLoad((const char *)&BASSFLACplugin, 0);
#elif defined ALLEGRO_MACOSX
	BASSFLACplugin = BASS_PluginLoad("libbassflac.dylib", 0);
#else
	BASSFLACplugin = BASS_PluginLoad("libbassflac.so", 0);
#endif

	if (!BASSFLACplugin) {
		ALLEGRO_DEBUG("Error loading FLAC plugin (%d)\n", BASS_ErrorGetCode());
	}
}

bool loadSamples(void (*cb)(int, int))
{
	preloaded_samples[preloaded_names[curr_sample]] =
		loadSample(preloaded_names[curr_sample]);
	(*cb)(curr_sample, total_samples);
	curr_sample++;
	if (curr_sample == total_samples) {
		curr_sample = 0;
		return true;
	}
	return false;
}


void destroySound(void)
{
	if (!sound_inited) return;

	std::map<std::string, MSAMPLE>::iterator it;

	for (it = preloaded_samples.begin(); it != preloaded_samples.end(); it++) {
		HSAMPLE s = (HSAMPLE)it->second;
		destroySample(s);
	}

	preloaded_samples.clear();

	destroyMusic();

	shutdownMusicName = musicName;
	shutdownAmbienceName = ambienceName;

#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		destroySound_oldandroid();
		return;
	}
#endif

	BASS_Free();
}

void playPreloadedSample(std::string name)
{
	if (!sound_inited) return;

	if (playBattlePreload(name))
		return;

	playSample(preloaded_samples[name]);
}

MSAMPLE loadSample(std::string name)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		return loadSample_oldandroid(name);
	}
#endif

	MSAMPLE s = 0;

	if (!sound_inited) return s;

#ifdef ALLEGRO_ANDROID
	ALLEGRO_PATH *p = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	char fn[1000];
	sprintf(fn, "%s/unpack/sfx/%s", al_path_cstr(p, '/'), name.c_str());
	al_destroy_path(p);

	s = BASS_SampleLoad(false,
		fn,
		0, 0, 8,
		BASS_SAMPLE_MONO|BASS_SAMPLE_OVER_POS);
	
	if (s == 0) {
		ALLEGRO_DEBUG("s=0 error code %d", BASS_ErrorGetCode());
	}
#else
	s = BASS_SampleLoad(false,
		getResource("sfx/%s", name.c_str()),
		0, 0, 8,
		BASS_SAMPLE_OVER_POS);
#endif

	return s;
}

void destroySample(MSAMPLE sample)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		destroySample_oldandroid(sample);
		return;
	}
#endif
	if (!sound_inited) return;

	BASS_SampleFree(sample);
}


void playSample(MSAMPLE sample, MSAMPLE_ID *unused)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		playSample_oldandroid(sample, unused);
		return;
	}
#endif
	(void)unused;
	if (!sound_inited) return;

	HCHANNEL chan = BASS_SampleGetChannel(sample, false);
	float vol = (float)config.getSFXVolume()/255.0;
	BASS_ChannelSetAttribute(chan, BASS_ATTRIB_VOL, vol);
	BASS_ChannelPlay(chan, false);
}


void loadPlayDestroy(std::string name)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		loadPlayDestroy_oldandroid(name);
		return;
	}
#endif
	if (!sound_inited) return;

	playPreloadedSample(name);
}

static void CALLBACK MusicSyncProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if (!BASS_ChannelSetPosition(channel, music_loop_start, BASS_POS_BYTE))
		BASS_ChannelSetPosition(channel, 0, BASS_POS_BYTE);
}

std::string check_music_name(std::string name, bool *is_flac)
{
	if (hqm_get_status(NULL) == HQM_STATUS_COMPLETE) {
		std::string::size_type p = name.rfind(".");
		if (p != std::string::npos) {
			name = name.substr(0, p) + ".flac";
			name = getUserResource((std::string("flacs/") + name).c_str());
			*is_flac = true;
			return name;
		}
	}

	*is_flac = false;
#ifdef ALLEGRO_ANDROID
	if (!is_android_lessthan_2_3) {
		static char n[1000];
		sprintf(n, "assets/data/music/%s", name.c_str());
		return n;
	}
#endif
	
	return getResource("music/%s", name.c_str());
}

void playMusic(std::string name, float vol, bool force)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		playMusic_oldandroid(name, vol, force);
		return;
	}
#endif
	if (!sound_inited) return;

	if (!force && musicName == name)
		return;

	if (name != "")
		musicName = name;

	if (music) {
		BASS_StreamFree(music);
	}

	if (name == "" || config.getMusicVolume() == 0) {
		music = 0;
		return;
	}

	bool is_flac;
	name = check_music_name(name, &is_flac);

#ifdef ALLEGRO_ANDROID
	if (is_flac) {
		al_set_standard_file_interface();
		ALLEGRO_FILE *f = al_fopen(name.c_str(), "rb");
		al_android_set_apk_file_interface();
		music = BASS_StreamCreateFileUser(
			STREAMFILE_NOBUFFER,
			BASS_SAMPLE_LOOP,
			&fileprocs,
			(void *)f
		);
	}
	else {
		debug_message("name='%s'", name.c_str());
		PHYSFS_File *f = PHYSFS_openRead(name.c_str());
		music = BASS_StreamCreateFileUser(
			STREAMFILE_NOBUFFER,
			BASS_SAMPLE_LOOP,
			&physfs_fileprocs,
			(void *)f
		);
	}

	if (music == 0)
		ALLEGRO_DEBUG("music == 0 error code=%d", BASS_ErrorGetCode());
#else
	music = BASS_StreamCreateFile(false,
		name.c_str(),
		0, 0, 0);
#endif
	
	music_loop_start = 0;

	BASS_ChannelSetSync(music, BASS_SYNC_END | BASS_SYNC_MIXTIME,
		0, MusicSyncProc, 0);

	BASS_ChannelPlay(music, FALSE);
	setMusicVolume(vol);
}


void setMusicVolume(float volume)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		setMusicVolume_oldandroid(volume);
		return;
	}
#endif
	if (!sound_inited) return;

	musicVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

	if (music) {
		BASS_ChannelSetAttribute(music, BASS_ATTRIB_VOL, volume);
	}
}

static void CALLBACK AmbienceSyncProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if (!BASS_ChannelSetPosition(channel, ambience_loop_start, BASS_POS_BYTE))
		BASS_ChannelSetPosition(channel, 0, BASS_POS_BYTE);
}

void playAmbience(std::string name, float vol)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		playAmbience_oldandroid(name, vol);
		return;
	}
#endif
	if (!sound_inited) return;

	ambienceName = name;

	if (ambience) {
		BASS_StreamFree(ambience);
	}

	if (name == "" || config.getMusicVolume() == 0) {
		ambience = 0;
		return;
	}

	bool is_flac;
	name = check_music_name(name, &is_flac);

#ifdef ALLEGRO_ANDROID
	if (is_flac) {
		al_set_standard_file_interface();
		ALLEGRO_FILE *f = al_fopen(name.c_str(), "rb");
		al_android_set_apk_file_interface();
		ambience = BASS_StreamCreateFileUser(
			STREAMFILE_NOBUFFER,
			BASS_SAMPLE_LOOP,
			&fileprocs,
			(void *)f
		);
	}
	else {
		PHYSFS_File *f = PHYSFS_openRead(name.c_str());
		ambience = BASS_StreamCreateFileUser(
			STREAMFILE_NOBUFFER,
			BASS_SAMPLE_LOOP,
			&physfs_fileprocs,
			(void *)f
		);
	}

	if (ambience == 0)
		ALLEGRO_DEBUG("ambience == 0 error code=%d", BASS_ErrorGetCode());
#else
	ambience = BASS_StreamCreateFile(false,
		name.c_str(),
		0, 0, 0);
#endif
	
	ambience_loop_start = 0;
	BASS_ChannelSetSync(ambience, BASS_SYNC_END | BASS_SYNC_MIXTIME,
		0, AmbienceSyncProc, 0);

	BASS_ChannelPlay(ambience, FALSE);
	setMusicVolume(vol);
}

void setAmbienceVolume(float volume)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		setAmbienceVolume_oldandroid(volume);
		return;
	}
#endif
	if (!sound_inited) return;

	ambienceVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

	BASS_ChannelSetAttribute(ambience, BASS_ATTRIB_VOL, volume);
}

float getMusicVolume(void)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		return getMusicVolume_oldandroid();
	}
#endif
   return musicVolume;
}

float getAmbienceVolume(void)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		return getAmbienceVolume_oldandroid();
	}
#endif
   return ambienceVolume;
}

void unmuteMusic(void)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		unmuteMusic_oldandroid();
		return;
	}
#endif
	playMusic(musicName, 1.0, true);
}

static void destroyMusic(void)
{
	if (music) {
		BASS_StreamFree(music);
		music = 0;
	}
	if (ambience) {
		BASS_StreamFree(ambience);
		ambience = 0;
	}
	
	musicName = "";

	BASS_PluginFree((HPLUGIN)BASSFLACplugin);
}

void unmuteAmbience(void)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		unmuteAmbience_oldandroid();
		return;
	}
#endif
	playAmbience(ambienceName);
}

void restartMusic(void)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		restartMusic_oldandroid();
		return;
	}
#endif
	playMusic(shutdownMusicName, 1.0, true);
}


void restartAmbience(void)
{
#ifdef ALLEGRO_ANDROID
	if (is_android_lessthan_2_3) {
		restartAmbience_oldandroid();
		return;
	}
#endif
	playAmbience(shutdownAmbienceName);
}

