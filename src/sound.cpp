#include "monster2.hpp"
#include <allegro5/allegro_acodec.h>
#if defined IPHONE || defined ALLEGRO_MACOSX
#include "playaac.h"
#endif

static void destroyMusic(void);

std::string shutdownMusicName = "";
std::string shutdownAmbienceName = "";

//static bool aac_playing = false;

#if !defined KCM_AUDIO

bool sound_inited = false;
static int total_samples = 0;
static int curr_sample = 0;

std::map<std::string, MSAMPLE> preloaded_samples;

static std::string sample_name = "";
static MSAMPLE sample;

#ifdef ALLEGRO_IPHONE
static std::string preloaded_names[] = {
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
	""
};
#else
static std::string preloaded_names[] = {
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
	""
};
static HPLUGIN BASSFLACplugin;
#endif

#ifdef IPHONE
extern HPLUGIN BASSFLACplugin;
#endif

void initSound(void)
{
	sound_inited = true;

#if defined IPHONE || defined ALLEGRO_MACOSX
//	init_aac();
#endif
	
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
#elif defined IPHONE
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
		total_samples++;
	}
}

bool loadSamples(void (*cb)(int, int))
{
	preloaded_samples[preloaded_names[curr_sample]] =
		loadSample(preloaded_names[curr_sample]);
	(*cb)(curr_sample, total_samples);
	curr_sample++;
	if (curr_sample == total_samples)
		return true;
	return false;
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

#ifdef IPHONE
	name.replace(name.length()-3, 3, "wav");
#endif
	
	playSample(preloaded_samples[name]);
}


MSAMPLE loadSample(std::string name)
{
	MSAMPLE s = 0;

	if (!sound_inited) return s;

#ifdef IPHONE
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

#ifdef IPHONE
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

#else

bool sound_inited = false;

std::map<std::string, MSAMPLE> preloaded_samples;

static std::string sample_name = "";
static MSAMPLE sample = { 0, };
static ALLEGRO_SAMPLE_ID sample_id;

static std::string preloaded_names[] = {
#ifdef IPHONE
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

void initSound(void)
{
//	double start = al_current_time();

	sound_inited = true;
	
// FIXME!
	//if (!al_install_audio(ALLEGRO_AUDIO_DRIVER_AUTODETECT)) {
	//if (!al_install_audio(ALLEGRO_AUDIO_DRIVER_OPENAL)) {
	if (!al_install_audio()) {
		debug_message("Init sound failed\n");
		sound_inited = false;
		return;
	}

	al_reserve_samples(8);

//	al_set_mixer_quality(al_get_default_mixer(), ALLEGRO_MIXER_QUALITY_POINT);

	//al_init_ogg_vorbis_addon();

	//al_set_mixer_frequency(al_get_default_mixer(), 11025);

	al_init_acodec_addon();

	#if defined IPHONE || defined ALLEGRO_MACOSX
	init_aac();
	#endif


	for (int i = 0; preloaded_names[i] != ""; i++) {
		preloaded_samples[preloaded_names[i]] =
			loadSample(preloaded_names[i]);
	}

//	double end = al_current_time();
//	double elapsed = end - start;
//	printf("initSound took %g seconds\n", elapsed);
}


void destroySound(void)
{
	if (!sound_inited) return;

	destroyMusic();

	//stopAllSamples();


	sample_name = "";


	std::map<std::string,  MSAMPLE>::iterator it;

	for (it = preloaded_samples.begin(); it != preloaded_samples.end(); it++) {
		MSAMPLE s = (MSAMPLE)it->second;
		//if (s.played)
		//	al_stop_sample(&s.id);
		al_destroy_sample(s);
	}

	preloaded_samples.clear();

	if (sample) {
		//if (sample.played)
		//	al_stop_sample(&sample.id);
		al_destroy_sample(sample);
		sample = 0;
	}

	//al_restore_default_mixer();

	al_uninstall_audio();
}


void playPreloadedSample(std::string name)
{
	if (!sound_inited) return;

#ifdef IPHONE
	name.replace(name.length()-3, 3, "wav");
#endif
	playSample(preloaded_samples[name]);
}


MSAMPLE loadSample(std::string name)
{
	if (!sound_inited) return NULL;

#ifdef IPHONE
	name.replace(name.length()-3, 3, "wav");
#endif
	MSAMPLE s = al_load_sample(getResource("sfx/%s", name.c_str()));

	return s;
}


void destroySample(MSAMPLE sample)
{
	if (!sound_inited) return;

	//if (sample.played)
	//	al_stop_sample(&sample.id);
	al_destroy_sample(sample);
}


void playSample(MSAMPLE sample, MSAMPLE_ID *id)
{
	if (!sound_inited) return;

	float vol = (float)config.getSFXVolume()/255.0;

	//if (sample.played)
	//	al_stop_sample(&sample.id);

	al_play_sample(sample, vol, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, id);
}


void loadPlayDestroy(std::string name)
{
	if (!sound_inited) return;

#ifdef IPHONE
	name.replace(name.length()-3, 3, "wav");
#endif
	/* First check if it's already loaded */
	if (preloaded_samples.find(name) != preloaded_samples.end()) {
		playPreloadedSample(name);
		return;
	}

	if (sample != 0) {
		al_stop_sample(&sample_id);
		if (sample_name != name) {
			sample_name = name;
			al_destroy_sample(sample);
			sample = loadSample(name);
		}
	}
	else {
		sample_name = name;
		sample = loadSample(name);
	}

	if (sample) {
		float vol = (float)config.getSFXVolume()/255;
		al_play_sample(sample, vol, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, &sample_id);
	}
}

void stopAllSamples(void)
{
	al_stop_samples();
}
#endif
    

#if !defined KCM_AUDIO
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
#include "playaac.h"
#endif
    

static DWORD music = 0;
static DWORD ambience = 0;
std::string musicName = "";
std::string ambienceName = "";
static QWORD music_loop_start = 0;
static QWORD ambience_loop_start = 0;
static float musicVolume = 1.0f;
static float ambienceVolume = 1.0f;

static void CALLBACK MusicSyncProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if (!BASS_ChannelSetPosition(channel, music_loop_start, BASS_POS_BYTE))
		BASS_ChannelSetPosition(channel, 0, BASS_POS_BYTE);
}

void playMusic(std::string name, float vol, bool force)
{
	if (!sound_inited) return;

	if (!force && musicName == name)
		return;

	if (name != "")
		musicName = name;

#if defined ALLEGRO_IPHONE_XX || defined ALLEGRO_MACOSX_XX
	if (aac_playing) {
		play_aac(NULL);
		aac_playing = false;
		music = NULL;
	}
	else
#endif
	if (music) {
		BASS_StreamFree(music);
	}

	if (name == "" || config.getMusicVolume() == 0) {
		music = 0;
		return;
	}

#if defined ALLEGRO_IPHONE_XX || defined ALLEGRO_MACOSX_XX
	if (name.find(".caf", 0) != std::string::npos) {
		play_aac(getResource("music/%s", name.c_str()));
		aac_playing = true;
	}
	else {
#endif
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
		
		music_loop_start = 0;

		BASS_ChannelSetSync(music, BASS_SYNC_END | BASS_SYNC_MIXTIME,
			0, MusicSyncProc, 0);

		setMusicVolume(vol);
		BASS_ChannelPlay(music, FALSE);
#if defined ALLEGRO_IPHONE_XX || defined ALLEGRO_MACOSX_XX
	}
#endif
}


void setMusicVolume(float volume)
{
	if (!sound_inited) return;

	musicVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

#if defined ALLEGRO_IPHONE_XX || defined ALLEGRO_MACOSX_XX
	if (aac_playing) {
		set_aac_volume(volume);
	}
	else 
#endif
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

static void destroyMusic(void)
{
#if defined ALLEGRO_IPHONE_XX || defined ALLEGRO_MACOSX_XX
	if (aac_playing) {
		play_aac(NULL);
		aac_playing = false;
		music = NULL;
	}
	else
#endif
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

void unmuteMusic(void)
{
    playMusic(musicName, 1.0, true);
}

void unmuteAmbience(void)
{
    playAmbience(ambienceName);
}
    
    
	
#else
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
//#include "playaac.h"
#endif
    
static ALLEGRO_AUDIO_STREAM *music = 0;
static ALLEGRO_AUDIO_STREAM *ambience = 0;
std::string musicName = "";
std::string ambienceName = "";
static float musicVolume = 1.0f;
static float ambienceVolume = 1.0f;

void playMusic(std::string name, float vol, bool force)
{
	if (!sound_inited) return;

	if (!force && musicName == name)
		return;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	if (aac_playing) {
		play_aac(NULL);
		aac_playing = false;
		music = NULL;
	}
	else
#endif
	if (music) {
		al_detach_audio_stream(music);
		al_set_audio_stream_playing(music, false);
		al_destroy_audio_stream(music);
	}
	
	if (name != "")
		musicName = name;
	
	if (name == "" || config.getMusicVolume() == 0) {
		music = 0;
		return;
	}
	
	//music = al_stream_from_file(getResource("music/%s", name.c_str()), 4, 4096);
	//if (name.find(".aac", 0) != std::string::npos) {

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	if (name.find(".caf", 0) != std::string::npos) {
		play_aac(getResource("music/%s", name.c_str()));
		aac_playing = true;
	}
	else {
#else
		size_t ext_pos = name.find(".caf", 0);
		if (ext_pos != std::string::npos) {
			name = name.substr(0, ext_pos) + ".flac";
		}
		ext_pos = name.find(".ogg", 0);
		if (ext_pos != std::string::npos) {
			name = name.substr(0, ext_pos) + ".flac";
		}
#endif
		music = al_load_audio_stream(getResource("music/%s", name.c_str()), 4, 1024*8);
		
		if (!music) {
			return;
		}
		
		al_set_audio_stream_playmode(music, ALLEGRO_PLAYMODE_LOOP);

		al_attach_audio_stream_to_mixer(music, al_get_default_mixer());
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	}
#endif

	setMusicVolume(vol);
}


void setMusicVolume(float volume)
{
	if (!sound_inited) return;

	musicVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	if (aac_playing) {
		set_aac_volume(volume);
	}
	else 
#endif
	if (music) {
		al_set_audio_stream_gain(music, volume);
	}
}


void playAmbience(std::string name)
{
	if (!sound_inited) return;
	
	ambienceName = name;

	if (ambience) {
		//al_detach_stream(ambience);
		al_set_audio_stream_playing(ambience, false);
		al_destroy_audio_stream(ambience);
	}

	if (name == "" || config.getMusicVolume() == 0) {
		ambience = 0;
		return;
	}

#if !defined IPHONE && !defined ALLEGRO_MACOSX
	size_t ext_pos = name.find(".caf", 0);
	if (ext_pos != std::string::npos) {
		name = name.substr(0, ext_pos) + ".flac";
	}
	ext_pos = name.find(".ogg", 0);
	if (ext_pos != std::string::npos) {
		name = name.substr(0, ext_pos) + ".flac";
	}
#endif

	ambience = al_load_audio_stream(getResource("music/%s", name.c_str()), 4, 1024*8);
	
	al_set_audio_stream_playmode(ambience, ALLEGRO_PLAYMODE_LOOP);

	al_attach_audio_stream_to_mixer(ambience, al_get_default_mixer());
		
	setAmbienceVolume(1);
}

void setAmbienceVolume(float volume)
{
	if (!sound_inited) return;

	ambienceVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

	if (ambience) {
		al_set_audio_stream_gain(ambience, volume);
	}
}

static void destroyMusic(void)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_MACOSX
	if (aac_playing) {
		play_aac(NULL);
		aac_playing = false;
		music = NULL;
	}
	else
#endif
	if (music) {
		al_set_audio_stream_playing(music, false);
		al_destroy_audio_stream(music);
		music = 0;
	}
	if (ambience) {
		al_set_audio_stream_playing(ambience, false);
		al_destroy_audio_stream(ambience);
		ambience = 0;
	}
	shutdownMusicName = musicName;
	shutdownAmbienceName = ambienceName;
	
	musicName = "";
	ambienceName = "";
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

void unmuteAmbience(void)
{
	playAmbience(ambienceName);
}
#endif

void restartMusic(void)
{
	playMusic(shutdownMusicName, 1.0, true);
}


void restartAmbience(void)
{
	playAmbience(shutdownAmbienceName);
}

