#include "monster2.hpp"
#include <allegro5/allegro_acodec.h>

std::string shutdownMusicName = "";
std::string shutdownAmbienceName = "";

bool sound_inited = false;

std::map<std::string, MSAMPLE> preloaded_samples;

static std::string sample_name = "";
static MSAMPLE sample;

#ifdef ALLEGRO_IPHONE
static HPLUGIN BASSFLACplugin;
#else
extern HPLUGIN BASSFLACplugin;
#endif

static DWORD music = 0;
static DWORD ambience = 0;
std::string musicName = "";
std::string ambienceName = "";
static QWORD music_loop_start = 0;
static QWORD ambience_loop_start = 0;
static float musicVolume = 1.0f;
static float ambienceVolume = 1.0f;


static std::string preloaded_names[] =
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
#ifdef LITE
	"Cure.wav",
	"Darkness1.wav",
	"Elixir.wav",
	"Heal.wav",
	"HolyWater.wav",
	"Revive.wav",
	"Wave.wav",
	"Weep.wav",
	"Whirlpool.wav",
	"appear.wav",
	"battle.wav",
	"blip.wav",
	"bolt.wav",
	"boss.wav",
	"chest.wav",
	"door.wav",
	"enemy_die.wav",
	"error.wav",
	"fall.wav",
	"fire1.wav",
	"hit.wav",
	"ice1.wav",
	"jump.wav",
	"melee_woosh.wav",
	"new_party_member.wav",
	"nooskewl.wav",
	"select.wav",
	"slime.wav",
	"spin.wav",
	"suck.wav",
	"swipe.wav",
	"woosh.wav",
#else
	"Acorns.wav",
	"Arc.wav",
	"Banana.wav",
	"Beam.wav",
	"BoF.wav",
	"Bolt2.wav",
	"Bolt3.wav",
	"Charm.wav",
	"Cure.wav",
	"Daisy.wav",
	"Darkness1.wav",
	"Darkness2.wav",
	"Darkness3.wav",
	"Elixir.wav",
	"Fire2.wav",
	"Fire3.wav",
	"Fireball.wav",
	"Heal.wav",
	"HolyWater.wav",
	"Ice3.wav",
	"Laser.wav",
	"Machine_Gun.wav",
	"Meow.wav",
	"Mmm.wav",
	"Orbit.wav",
	"Portal.wav",
	"Puke.wav",
	"Punch.wav",
	"Quick.wav",
	"Rend.wav",
	"Revive.wav",
	"Slow.wav",
	"Spray.wav",
	"Stomp.wav",
	"Stone.wav",
	"Stun.wav",
	"Swallow.wav",
	"Thud.wav",
	"TouchofDeath.wav",
	"Twister.wav",
	"UFO.wav",
	"Vampire.wav",
	"Wave.wav",
	"Web.wav",
	"Weep.wav",
	"Whip.wav",
	"Whirlpool.wav",
	"appear.wav",
	"battle.wav",
	"blip.wav",
	"bolt.wav",
	"boss.wav",
	"bow_draw.wav",
	"bow_release_and_draw.wav",
	"buzz.wav",
	"cartoon_fall.wav",
	"chest.wav",
	"ching.wav",
	"chomp.wav",
	"door.wav",
	"drain.wav",
	"enemy_die.wav",
	"enemy_explosion.wav",
	"error.wav",
	"explosion.wav",
	"fall.wav",
	"fire1.wav",
	"freeze.wav",
	"high_cackle.wav",
	"hit.wav",
	"ice1.wav",
	"ignite.wav",
	"jump.wav",
	"low_cackle.wav",
	"melee_woosh.wav",
	"new_party_member.wav",
	"nooskewl.wav",
	"pistol.wav",
	"push.wav",
	"rocket_launch.wav",
	"select.wav",
	"sleep.wav",
	"slice.wav",
	"slime.wav",
	"spin.wav",
	"splash.wav",
	"staff_fly.wav",
	"staff_poof.wav",
	"suck.wav",
	"swipe.wav",
	"torpedo.wav",
	"woosh.wav",
#endif
#else
#ifdef LITE
	"Cure.ogg",
	"Darkness1.ogg",
	"Elixir.ogg",
	"Heal.ogg",
	"HolyWater.ogg",
	"Revive.ogg",
	"Wave.ogg",
	"Weep.ogg",
	"Whirlpool.ogg",
	"appear.ogg",
	"battle.ogg",
	"blip.ogg",
	"bolt.ogg",
	"boss.ogg",
	"chest.ogg",
	"door.ogg",
	"enemy_die.ogg",
	"error.ogg",
	"fall.ogg",
	"fire1.ogg",
	"hit.ogg",
	"ice1.ogg",
	"jump.ogg",
	"melee_woosh.ogg",
	"new_party_member.ogg",
	"nooskewl.ogg",
	"select.ogg",
	"slime.ogg",
	"spin.ogg",
	"suck.ogg",
	"swipe.ogg",
	"woosh.ogg",
#else
	"Acorns.ogg",
	"Arc.ogg",
	"Banana.ogg",
	"Beam.ogg",
	"BoF.ogg",
	"Bolt2.ogg",
	"Bolt3.ogg",
	"Charm.ogg",
	"Cure.ogg",
	"Daisy.ogg",
	"Darkness1.ogg",
	"Darkness2.ogg",
	"Darkness3.ogg",
	"Elixir.ogg",
	"Fire2.ogg",
	"Fire3.ogg",
	"Fireball.ogg",
	"Heal.ogg",
	"HolyWater.ogg",
	"Ice3.ogg",
	"Laser.ogg",
	"Machine_Gun.ogg",
	"Meow.ogg",
	"Mmm.ogg",
	"Orbit.ogg",
	"Portal.ogg",
	"Puke.ogg",
	"Punch.ogg",
	"Quick.ogg",
	"Rend.ogg",
	"Revive.ogg",
	"Slow.ogg",
	"Spray.ogg",
	"Stomp.ogg",
	"Stone.ogg",
	"Stun.ogg",
	"Swallow.ogg",
	"Thud.ogg",
	"TouchofDeath.ogg",
	"Twister.ogg",
	"UFO.ogg",
	"Vampire.ogg",
	"Wave.ogg",
	"Web.ogg",
	"Weep.ogg",
	"Whip.ogg",
	"Whirlpool.ogg",
	"appear.ogg",
	"battle.ogg",
	"blip.ogg",
	"bolt.ogg",
	"boss.ogg",
	"bow_draw.ogg",
	"bow_release_and_draw.ogg",
	"buzz.ogg",
	"cartoon_fall.ogg",
	"chest.ogg",
	"ching.ogg",
	"chomp.ogg",
	"door.ogg",
	"drain.ogg",
	"enemy_die.ogg",
	"enemy_explosion.ogg",
	"error.ogg",
	"explosion.ogg",
	"fall.ogg",
	"fire1.ogg",
	"freeze.ogg",
	"high_cackle.ogg",
	"hit.ogg",
	"ice1.ogg",
	"ignite.ogg",
	"jump.ogg",
	"low_cackle.ogg",
	"melee_woosh.ogg",
	"new_party_member.ogg",
	"nooskewl.ogg",
	"pistol.ogg",
	"push.ogg",
	"rocket_launch.ogg",
	"select.ogg",
	"sleep.ogg",
	"slice.ogg",
	"slime.ogg",
	"spin.ogg",
	"splash.ogg",
	"staff_fly.ogg",
	"staff_poof.ogg",
	"suck.ogg",
	"swipe.ogg",
	"torpedo.ogg",
	"woosh.ogg",
#endif
#endif
	""
};

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
	sample_name = "";
	
	BASS_PluginFree((HPLUGIN)BASSFLACplugin);
}

void initSound(void)
{
	sound_inited = true;

#ifdef __linux__XXX
	if (!BASS_Init(-1, 44100, BASS_DEVICE_DMIX, NULL, NULL)) {
#else
	if (!BASS_Init(-1, 44100, 0, NULL, NULL)) {
#endif
		sound_inited = false;
		return;
	}

#if defined ALLEGRO_WINDOWS
	BASSFLACplugin = BASS_PluginLoad("bassflac.dll", 0);
#elif defined LINUX_GENERIC
	char buf1[MAX_PATH];
	char buf2[MAX_PATH];
	getcwd(buf1, MAX_PATH);
	sprintf(buf2, "%s/libbassflac.so", buf1);
	BASSFLACplugin = BASS_PluginLoad(buf2, 0);
#elif defined ALLEGRO_IPHONE
	BASS_PluginLoad((const char *)&BASSFLACplugin, 0);
#elif defined ALLEGRO_MACOSX
	BASSFLACplugin = BASS_PluginLoad("libbassflac.dylib", 0);
#else
	BASSFLACplugin = BASS_PluginLoad("libbassflac.so", 0);
#endif

	if (!BASSFLACplugin) {
		printf("Error loading FLAC plugin (%d)\n", BASS_ErrorGetCode());
	}

	for (int i = 0; preloaded_names[i] != ""; i++) {
		preloaded_samples[preloaded_names[i]] =
			loadSample(preloaded_names[i]);
	}
}


void destroySound(void)
{
	if (!sound_inited) return;

	std::map<std::string,  MSAMPLE>::iterator it;

	for (it = preloaded_samples.begin(); it != preloaded_samples.end(); it++) {
		HSAMPLE s = (HSAMPLE)it->second;
		BASS_SampleFree(s);
	}

	preloaded_samples.clear();

	if (sample)
		BASS_SampleFree(sample);
		sample = 0;

	destroyMusic();

	shutdownMusicName = musicName;
	shutdownAmbienceName = ambienceName;

	BASS_Free();
}


void playPreloadedSample(std::string name)
{
	if (!sound_inited) return;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	name.replace(name.length()-3, 3, "wav");
#endif
	
	playSample(preloaded_samples[name]);
}


MSAMPLE loadSample(std::string name)
{
	MSAMPLE s = 0;

	if (!sound_inited) return s;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	name.replace(name.length()-3, 3, "wav");
#endif

	s = BASS_SampleLoad(false,
		getResource("sfx/%s", name.c_str()),
		0, 0, 8,
		BASS_SAMPLE_OVER_POS);

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

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	name.replace(name.length()-3, 3, "wav");
#endif

	if (sample != 0 && sample_name != name) {
		sample_name = name;
		BASS_SampleFree(sample);
		sample = loadSample(name);
	}
	else if (sample == 0) {
		sample = loadSample(name);
	}

	if (sample)
		playSample(sample);
}

void stopAllSamples(void)
{
}

static void CALLBACK MusicSyncProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if (!BASS_ChannelSetPosition(channel, music_loop_start, BASS_POS_BYTE))
		BASS_ChannelSetPosition(channel, 0, BASS_POS_BYTE);
}

void playMusic(std::string name, bool setLoopStart, unsigned int loopStart, bool force)
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

	size_t ext_pos = name.find(".caf", 0);
	if (ext_pos != std::string::npos) {
		name = name.substr(0, ext_pos) + ".flac";
	}
	ext_pos = name.find(".ogg", 0);
	if (ext_pos != std::string::npos) {
		name = name.substr(0, ext_pos) + ".flac";
	}
	music = BASS_StreamCreateFile(false,
		getResource("music/%s", name.c_str()),
		0, 0, 0);
	
	if (setLoopStart) {
		music_loop_start = BASS_ChannelSeconds2Bytes(music, loopStart/1000.0);
	}
	else {
		music_loop_start = 0;
	}

	BASS_ChannelSetSync(music, BASS_SYNC_END | BASS_SYNC_MIXTIME,
		0, MusicSyncProc, 0);

	setMusicVolume(1);
	BASS_ChannelPlay(music, FALSE);
}


void playMusicVolumeOff(std::string name)
{
	if (!sound_inited) return;

	if (musicName == name)
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

	size_t ext_pos = name.find(".caf", 0);
	if (ext_pos != std::string::npos) {
		name = name.substr(0, ext_pos) + ".flac";
	}
	ext_pos = name.find(".ogg", 0);
	if (ext_pos != std::string::npos) {
		name = name.substr(0, ext_pos) + ".flac";
	}
	music = BASS_StreamCreateFile(false,
		getResource("music/%s", name.c_str()),
		0, 0, 0);
	
	BASS_ChannelSetSync(music, BASS_SYNC_END | BASS_SYNC_MIXTIME,
		0, MusicSyncProc, 0);

	setMusicVolume(0);
	BASS_ChannelPlay(music, FALSE);
}


void setMusicVolume(float volume)
{
	if (!sound_inited) return;

	musicVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

	if (music)
		BASS_ChannelSetAttribute(music, BASS_ATTRIB_VOL, volume);
}

static void CALLBACK AmbienceSyncProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if (!BASS_ChannelSetPosition(channel, ambience_loop_start, BASS_POS_BYTE))
		BASS_ChannelSetPosition(channel, 0, BASS_POS_BYTE);
}


void playAmbience(std::string name)
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

	size_t ext_pos = name.find(".caf", 0);
	if (ext_pos != std::string::npos) {
		name = name.substr(0, ext_pos) + ".flac";
	}
	ext_pos = name.find(".ogg", 0);
	if (ext_pos != std::string::npos) {
		name = name.substr(0, ext_pos) + ".flac";
	}
	ambience = BASS_StreamCreateFile(false,
		getResource("music/%s", name.c_str()),
		0, 0, 0);
	
	ambience_loop_start = 0;
	BASS_ChannelSetSync(ambience, BASS_SYNC_END | BASS_SYNC_MIXTIME,
		0, AmbienceSyncProc, 0);

	setMusicVolume(1);
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
    playMusic(musicName, false, 0, true);
}

void unmuteAmbience(void)
{
    playAmbience(ambienceName);
}

void restartMusic(void)
{
	playMusic(shutdownMusicName, false, 0, true);
}


void restartAmbience(void)
{
	playAmbience(shutdownAmbienceName);
}

