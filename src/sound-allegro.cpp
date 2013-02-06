#include "monster2.hpp"

#include "tftp_get.h"

static void destroyMusic(void);

static std::string shutdownMusicName = "";
static std::string shutdownAmbienceName = "";

bool sound_inited = false;
static int total_samples = 0;
static int curr_sample = 0;

static std::map<std::string, ALLEGRO_SAMPLE *> preloaded_samples;

static ALLEGRO_AUDIO_STREAM *music = 0;
static ALLEGRO_AUDIO_STREAM *ambience = 0;
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

void initSound(void)
{
	sound_inited = true;
	
	for (int i = 0; preloaded_names[i] != ""; i++) {
		total_samples++;
	}

	al_init_acodec_addon();

	al_install_audio();
	al_reserve_samples(32);
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

	std::map<std::string, ALLEGRO_SAMPLE *>::iterator it;

	for (it = preloaded_samples.begin(); it != preloaded_samples.end(); it++) {
		ALLEGRO_SAMPLE *s = (ALLEGRO_SAMPLE *)it->second;
		destroySample(s);
	}

	preloaded_samples.clear();

	destroyMusic();

	shutdownMusicName = musicName;
	shutdownAmbienceName = ambienceName;

	al_uninstall_audio();
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
	MSAMPLE s = 0;

	if (!sound_inited) return s;

	s = al_load_sample(getResource("sfx/%s", name.c_str()));
	if (s == 0) {
		native_error("Load error.", ((std::string("sfx/") + name).c_str()));
	}

	return s;
}

void destroySample(MSAMPLE sample)
{
	if (!sound_inited) return;

	al_destroy_sample(sample);
}


void playSample(MSAMPLE sample, MSAMPLE_ID *id)
{
	if (!sound_inited) return;

	al_play_sample(sample, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, id);
}


void loadPlayDestroy(std::string name)
{
	if (!sound_inited) return;

	playPreloadedSample(name);
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
	
	return getResource("music/%s", name.c_str());
}

void playMusic(std::string name, float vol, bool force)
{
	if (!sound_inited) return;

	if (!force && musicName == name)
		return;

	if (name != "")
		musicName = name;

	if (music) {
		al_destroy_audio_stream(music);
	}

	if (name == "" || config.getMusicVolume() == 0) {
		music = 0;
		return;
	}

	bool is_flac;
	name = check_music_name(name, &is_flac);

	music = al_load_audio_stream(name.c_str(), 4, 2048);
	if (music == 0) {
		native_error("Load error.", name.c_str());
	}

	setMusicVolume(vol);

	al_attach_audio_stream_to_mixer(music, al_get_default_mixer());

	al_set_audio_stream_playmode(music, ALLEGRO_PLAYMODE_LOOP);
}


void setMusicVolume(float volume)
{
	if (!sound_inited) return;

	musicVolume = volume;

	volume *= config.getMusicVolume()/255.0f;

	if (music) {
		al_set_audio_stream_gain(music, volume);
	}
}

void playAmbience(std::string name, float vol)
{
	if (!sound_inited) return;

	ambienceName = name;

	if (ambience) {
		al_destroy_audio_stream(ambience);
	}

	if (name == "" || config.getMusicVolume() == 0) {
		ambience = 0;
		return;
	}

	bool is_flac;
	name = check_music_name(name, &is_flac);

	ambience = al_load_audio_stream(name.c_str(), 4, 2048);
	if (ambience == 0) {
		native_error("Load error.", name.c_str());
	}

	setAmbienceVolume(vol);

	al_attach_audio_stream_to_mixer(ambience, al_get_default_mixer());

	al_set_audio_stream_playmode(ambience, ALLEGRO_PLAYMODE_LOOP);
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
		al_destroy_audio_stream(music);
		music = 0;
	}
	if (ambience) {
		al_destroy_audio_stream(ambience);
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

