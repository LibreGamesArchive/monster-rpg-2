#include "monster2.hpp"

#ifdef ALLEGRO_ANDROID
#include <physfs.h>
#endif

#ifdef ALLEGRO_ANDROID
#include "java.h"
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
static float musicVolume = 1.0f;
static float ambienceVolume = 1.0f;
	
static std::string preloaded_names[] = {
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
	"juice.ogg",
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
	""
};

#ifdef ALLEGRO_ANDROID
BASS_FILEPROCS fileprocs;
BASS_FILEPROCS physfs_fileprocs;

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

void initSound(void)
{
	sound_inited = true;
	
	for (int i = 0; preloaded_names[i] != ""; i++) {
		total_samples++;
	}

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
	BASS_SetConfig(BASS_CONFIG_DEV_BUFFER, 250);
#endif
	if (!BASS_Init(-1, 44100, 0, NULL, NULL)) {
		int code = BASS_ErrorGetCode();
		debug_message("BASS_Init failed (%d). Failing or falling back", code);
	}

	return;
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

	BASS_Free();

	debug_message("Sound shutdown\n");
}

void playPreloadedSample(std::string name)
{
	if (!sound_inited) return;

	if (playBattlePreload(name))
		return;

	playSample(preloaded_samples[name]);
}

#ifdef ALLEGRO_ANDROID
static unsigned char *load_from_zip(std::string filename, int *ret_size, bool terminate_with_0, bool use_malloc)
{
	ALLEGRO_FILE *f = al_fopen(filename.c_str(), "rb");
	if (f == NULL) {
		return NULL;
	}
	long size = al_fsize(f);
	unsigned char *bytes;
	int extra = terminate_with_0;

	if (size < 0) {
		std::vector<char> v;
		int c;
		while ((c = al_fgetc(f)) != EOF) {
			v.push_back(c);
		}
		if (use_malloc) {
			bytes = (unsigned char *)malloc(v.size()+extra);
		}
		else {
			bytes = new unsigned char[v.size()+extra];
		}
		for (unsigned int i = 0; i < v.size(); i++) {
			bytes[i] = v[i];
		}
	}
	else {
		if (use_malloc) {
			bytes = (unsigned char *)malloc(size+extra);
		}
		else {
			bytes = new unsigned char[size+extra];
		}
		al_fread(f, bytes, size);
	}
	al_fclose(f);
	if (extra) {
		bytes[size] = 0;
	}

	if (ret_size)
		*ret_size = size + extra;

	return bytes;
}

static HSTREAM get_decode_stream(const char *name, unsigned char **buf)
{
	int sz;
	*buf = load_from_zip(
		name,
		&sz,
		false,
		true
	);
	if (*buf == NULL) {
		debug_message("buf == NULL");
		return 0;
	}
	HSTREAM stream = BASS_StreamCreateFile(
		true,
		*buf,
		0,
		sz,
		BASS_STREAM_DECODE
	);
	return stream;
}
#endif

MSAMPLE loadSample(std::string name)
{
	MSAMPLE s = 0;

	if (!sound_inited) return s;

#ifdef ALLEGRO_ANDROID
	unsigned char *buf0;
	HSTREAM stream = get_decode_stream(getResource("sfx/%s", name.c_str()), &buf0);
	if (stream == 0) {
		return 0;
	}
	#define SZ (256*1024)
	int total_read = 0;
	int bufsize = 0;
	DWORD read = 0;
	unsigned char *buf;
	do {
		if (bufsize == 0) {
			bufsize += SZ;
			buf = (unsigned char *)malloc(SZ);
		}
		else {
			bufsize += SZ;
			buf = (unsigned char *)realloc(buf, bufsize);
		}
		read = BASS_ChannelGetData(stream, buf+total_read, SZ);
		total_read += read;
	} while (read == SZ);
	#undef SZ
	BASS_CHANNELINFO info;
	BASS_ChannelGetInfo(stream, &info);
	BASS_StreamFree(stream);
	HSAMPLE samp = BASS_SampleCreate(
		total_read,
		info.freq,
		info.chans,
		4,
		BASS_SAMPLE_OVER_POS
	);
	BASS_SampleSetData(samp, buf);
	free(buf0);
	free(buf);
	return samp;
#else
	s = BASS_SampleLoad(false,
		getResource("sfx/%s", name.c_str()),
		0, 0, 8,
		BASS_SAMPLE_OVER_POS);
	if (s == 0) {
		native_error((std::string("Couldn't sfx/") + name + ".").c_str());
	}
#endif

	return s;
}

void destroySample(MSAMPLE sample)
{
	if (!sound_inited) return;

	BASS_SampleFree(sample);
}


void playSample(MSAMPLE sample, MSAMPLE_ID *unused)
{
	(void)unused;
	if (!sound_inited) return;

	HCHANNEL chan = BASS_SampleGetChannel(sample, false);
	float vol = (float)config.getSFXVolume()/255.0;
	BASS_ChannelSetAttribute(chan, BASS_ATTRIB_VOL, vol);
	BASS_ChannelPlay(chan, false);
}


void loadPlayDestroy(std::string name)
{
	if (!sound_inited) return;

	playPreloadedSample(name);
}

static void CALLBACK MusicSyncProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if (!BASS_ChannelSetPosition(channel, 0, BASS_POS_BYTE))
		BASS_ChannelSetPosition(channel, 0, BASS_POS_BYTE);
}

std::string check_music_name(std::string name, bool *is_flac)
{
	*is_flac = false;

/*
#ifdef ALLEGRO_ANDROID
	static char buffer[1000];
	sprintf(buffer, "assets/data/music/%s", name.c_str());
	return buffer;
#else
*/
	return getResource("music/%s", name.c_str());
//#endif
}

void playMusic(std::string name, float vol, bool force)
{
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
		al_set_physfs_file_interface();
		music = BASS_StreamCreateFileUser(
			STREAMFILE_NOBUFFER,
			BASS_SAMPLE_LOOP,
			&fileprocs,
			(void *)f
		);
	}
	else {
		PHYSFS_File *f = PHYSFS_openRead(name.c_str());
		music = BASS_StreamCreateFileUser(
			STREAMFILE_NOBUFFER,
			BASS_SAMPLE_LOOP,
			&physfs_fileprocs,
			(void *)f
		);
	}
#else
	music = BASS_StreamCreateFile(false,
		name.c_str(),
		0, 0, 0);
#endif
	
	if (music == 0) {
	   native_error("Load error.", name.c_str());
	}

	BASS_ChannelSetSync(music, BASS_SYNC_END | BASS_SYNC_MIXTIME,
		0, MusicSyncProc, 0);

	setMusicVolume(vol);
	BASS_ChannelPlay(music, FALSE);
}


void setMusicVolume(float volume)
{
	if (!sound_inited) return;

	musicVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

	if (music) {
		BASS_ChannelSetAttribute(music, BASS_ATTRIB_VOL, volume);
	}
}

void playAmbience(std::string name, float vol)
{
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
		al_set_physfs_file_interface();
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
#else
	ambience = BASS_StreamCreateFile(false,
		name.c_str(),
		0, 0, 0);
#endif
	
	if (ambience == 0) {
		native_error("Load error.", name.c_str());
	}

	BASS_ChannelSetSync(ambience, BASS_SYNC_END | BASS_SYNC_MIXTIME,
		0, MusicSyncProc, 0);

	setAmbienceVolume(vol);
	BASS_ChannelPlay(ambience, FALSE);
}

void setAmbienceVolume(float volume)
{
	if (!sound_inited) return;

	ambienceVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

	BASS_ChannelSetAttribute(ambience, BASS_ATTRIB_VOL, volume);
}

float getMusicVolume(void)
{
   return musicVolume;
}

float getAmbienceVolume(void)
{
   return ambienceVolume;
}

void unmuteMusic(void)
{
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
}

void unmuteAmbience(void)
{
	playAmbience(ambienceName);
}

void restartMusic(void)
{
	playMusic(shutdownMusicName, 1.0, true);
}


void restartAmbience(void)
{
	playAmbience(shutdownAmbienceName);
}

void setStreamVolume(MSAMPLE stream, float volume)
{
	if (!sound_inited) return;

	volume *= config.getSFXVolume()/255.0f;

	if (stream) {
		BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume);
	}
}

MSAMPLE streamSample(std::string name, float vol)
{
	MSAMPLE samp;

	if (!sound_inited) return 0;

	bool is_flac;
	name = check_music_name(name, &is_flac);

#ifdef ALLEGRO_ANDROID
	if (is_flac) {
		al_set_standard_file_interface();
		ALLEGRO_FILE *f = al_fopen(name.c_str(), "rb");
		al_set_physfs_file_interface();
		samp = BASS_StreamCreateFileUser(
			STREAMFILE_NOBUFFER,
			BASS_SAMPLE_LOOP,
			&fileprocs,
			(void *)f
		);
	}
	else {
		PHYSFS_File *f = PHYSFS_openRead(name.c_str());
		samp = BASS_StreamCreateFileUser(
			STREAMFILE_NOBUFFER,
			BASS_SAMPLE_LOOP,
			&physfs_fileprocs,
			(void *)f
		);
	}
#else
	samp = BASS_StreamCreateFile(false,
		name.c_str(),
		0, 0, 0);
#endif
	
	if (samp == 0) {
	   native_error("Load error.", name.c_str());
	}

	BASS_ChannelSetSync(samp, BASS_SYNC_END | BASS_SYNC_MIXTIME,
		0, MusicSyncProc, 0);

	setStreamVolume(samp, vol);
	BASS_ChannelPlay(samp, FALSE);

	return samp;
}

void destroyStream(MSAMPLE stream)
{
	if (stream) {
		BASS_StreamFree(stream);
	}
}

