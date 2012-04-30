#include "monster2.hpp"

// FIXME
#define ASSERT ALLEGRO_ASSERT
#include <allegro5/internal/aintern_bitmap.h>
#include <allegro5/internal/aintern_display.h>
#include <allegro5/internal/aintern_opengl.h>

#include "tftp_get.h"

#ifdef ALLEGRO_ANDROID
extern "C" {
void openURL(const char *url);
}
#endif

TGUIWidget *mainWidget = NULL;
uint32_t runtime;
int runtime_ms = 0;
long runtime_start;

static bool dont_draw_now = false;

bool break_main_loop = false;
bool quit_game = false;

bool timer_on = false;
int timer_time = 0;

bool battle_must_win;

Player *player = NULL;

bool battle_won = false;
bool battle_lost = false;

volatile bool close_pressed = false;
volatile bool close_pressed_for_configure = false;
	
char *saveFilename = NULL;

bool was_in_map = false;

static bool fps_on = false;
static int fps;

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
static int bright_dir = 1;
#endif
float bright_ticker = 0;

std::vector<std::pair<int, bool> > forced_milestones;

bool global_can_save = true;

bool tutorial_started = false;
bool gonna_fade_in_red = false;

// FIXME:
void check_some_stuff_in_shooter(void);

int old_control_mode = -1;

void connect_airplay_controls(void)
{
#ifdef ALLEGRO_IPHONE
	old_control_mode = config.getDpadType();
	al_lock_mutex(dpad_mutex);
	getInput()->reset();
	config.setDpadType(DPAD_TOTAL_2);
	joystick_repeat_started[JOY_REPEAT_AXIS0] = false;
	joystick_repeat_started[JOY_REPEAT_AXIS1] = false;
	joystick_repeat_started[JOY_REPEAT_B1] = false;
	joystick_repeat_started[JOY_REPEAT_B2] = false;
	joystick_repeat_started[JOY_REPEAT_B3] = false;
	dpad_type = DPAD_TOTAL_2;
	al_unlock_mutex(dpad_mutex);
	dpad_on(false);
#endif
}

void disconnect_airplay_controls(void)
{
#ifdef ALLEGRO_IPHONE
	al_lock_mutex(dpad_mutex);
	getInput()->reset();
	config.setDpadType(old_control_mode);
	joystick_repeat_started[JOY_REPEAT_AXIS0] = false;
	joystick_repeat_started[JOY_REPEAT_AXIS1] = false;
	joystick_repeat_started[JOY_REPEAT_B1] = false;
	joystick_repeat_started[JOY_REPEAT_B2] = false;
	joystick_repeat_started[JOY_REPEAT_B3] = false;
	dpad_type = old_control_mode;
	al_unlock_mutex(dpad_mutex);
	if (old_control_mode == DPAD_TOTAL_1 || old_control_mode == DPAD_TOTAL_2) {
		dpad_on(false);
	}
	else {
		dpad_off(false);
	}
	old_control_mode = -1;
#endif
}

void connect_second_display(void)
{
#ifdef ALLEGRO_IPHONE
		
	int mvol = config.getMusicVolume();
	int svol = config.getSFXVolume();

	connect_airplay_controls();
	
	destroy_shaders();
	al_destroy_display(display);
	
	al_set_new_display_adapter(1);
	int flags = al_get_new_display_flags();
	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_USE_PROGRAMMABLE_PIPELINE | flags);
	al_set_new_display_option(ALLEGRO_AUTO_CONVERT_BITMAPS, 1, ALLEGRO_REQUIRE);
	display = al_create_display(1, 1);
	al_set_new_display_flags(flags);
	init_shaders();
	init2_shaders();

	al_set_new_display_adapter(0);
	ScreenDescriptor *sd = config.getWantedGraphicsMode();
	int flgs = al_get_new_display_flags();
	al_set_new_display_flags(flgs & ~ALLEGRO_USE_PROGRAMMABLE_PIPELINE);
	controller_display = al_create_display(sd->height, sd->width);
	al_set_new_display_flags(flgs);
	register_display(controller_display);
	//init_controller_shader();
	ALLEGRO_TRANSFORM scale;
	al_identity_transform(&scale);
	al_scale_transform(&scale, sd->height/960.0, sd->width/640.0);
	al_use_transform(&scale);
	int format = al_get_new_bitmap_format();
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGB_565);
	for (int i = 0; i < 8; i++) {
		blueblocks[i] = m_load_bitmap(getResource("media/blueblocks%d.png", i+1));
	}
	al_set_new_bitmap_format(format);
	game_font_second_display = al_load_ttf_font(getResource("DejaVuSans.ttf"), 10, 0);
	
	airplay_dpad = m_load_alpha_bitmap(getResource("media/airplay_pad.png"));
	white_button = m_load_alpha_bitmap(getResource("media/whitebutton.png"));
	black_button = m_load_alpha_bitmap(getResource("media/blackbutton.png"));
	airplay_logo = m_load_alpha_bitmap(getResource("media/m2_controller_logo.png"));
	
	m_set_target_bitmap(buffer);

	config.setMusicVolume(mvol);
	config.setSFXVolume(svol);
	setMusicVolume(1);

	airplay_connected = true;
#endif
}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
bool got_resume = false;
static void *wait_for_drawing_resume(void *arg)
{
	ALLEGRO_EVENT_QUEUE *queue = (ALLEGRO_EVENT_QUEUE *)arg;

	while (1) {
		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING) {
			got_resume = true;
			break;
		}
	}

	al_broadcast_cond(switch_cond);

	return NULL;
}

static bool should_pause_game(void)
{
	return (area && !battle && !player_scripted && !in_pause && !in_map);
}

static float backup_music_volume = 1.0f;
static float backup_ambience_volume = 1.0f;
#endif

// called from everywhere
bool is_close_pressed(void)
{
	// random tasks
	while (!al_event_queue_is_empty(events_minor)) {
		ALLEGRO_EVENT event;
		al_get_next_event(events_minor, &event);
#ifdef ALLEGRO_IPHONE
		if (event.type == ALLEGRO_EVENT_DISPLAY_CONNECTED) {
			create_airplay_mirror = true;
		}
		else if (event.type == ALLEGRO_EVENT_DISPLAY_DISCONNECTED) {
			delete_airplay_mirror = true;
		}
#endif

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
#ifdef ALLEGRO_IPHONE
			if (!sound_was_playing_at_program_start)
				iPodStop();
#endif
			close_pressed = true;
#ifdef ALLEGRO_IPHONE
			break;
#endif
		}
#if defined ALLEGRO_ANDROID
		if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
			do_acknowledge_resize = true;
		}
#endif
#ifdef ALLEGRO_IPHONE
		if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_OUT)
		{
			do_pause_game = should_pause_game();
			if (do_pause_game || in_pause) {
				backup_music_volume = 0.5;
				backup_ambience_volume = 0.5;
			}
			else {
				backup_music_volume = getMusicVolume();
				backup_ambience_volume = getAmbienceVolume();
			}
			setMusicVolume(0.0);
			setAmbienceVolume(0.0);
		}
		else if (event.type == ALLEGRO_EVENT_DISPLAY_SWITCH_IN)
		{
			setMusicVolume(backup_music_volume);
			setAmbienceVolume(backup_ambience_volume);
		}
#endif
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		if (event.type == ALLEGRO_EVENT_DISPLAY_HALT_DRAWING) {
			save_memory(false);
#if defined ALLEGRO_IPHONE
			if (!isMultitaskingSupported()) {
				if (!sound_was_playing_at_program_start)
					iPodStop();
				exit(0);
			}
			//sb_stop();
#elif defined ALLEGRO_ANDROID
			std::string old_music_name = musicName;
			std::string old_ambience_name = ambienceName;
			float old_music_volume = getMusicVolume();
			float old_ambience_volume = getAmbienceVolume();
			playMusic("");
			playAmbience("");
			_destroy_loaded_bitmaps();
			destroy_shaders();
#endif
			config.write();
			al_stop_timer(logic_timer);
			al_stop_timer(draw_timer);
			// halt
			al_acknowledge_drawing_halt(display);
			got_resume = false;
			al_run_detached_thread(
				wait_for_drawing_resume,
				events_minor
			);
			al_lock_mutex(switch_mutex);
			while (!got_resume) {
				al_wait_cond(switch_cond, switch_mutex);
			}
			ALLEGRO_DEBUG("after cond");
			al_unlock_mutex(switch_mutex);
			// resume
			al_acknowledge_drawing_resume(display);
#ifdef ALLEGRO_ANDROID
			_reload_loaded_bitmaps();
			init_shaders();
			init2_shaders();
			if (in_shooter) {
				shooter_restoring = true;
			}
#endif
      			glDisable(GL_DITHER);
			al_start_timer(logic_timer);
			al_start_timer(draw_timer);
			ALLEGRO_DEBUG("HERE1");
#ifdef ALLEGRO_ANDROID
			playMusic(old_music_name, old_music_volume, true);
			playAmbience(old_ambience_name, old_ambience_volume);
#endif
			ALLEGRO_DEBUG("HERE2");
		}
#endif
	}
#ifdef ALLEGRO_IPHONE
	double shake = al_iphone_get_last_shake_time();
	if (shake > allegro_iphone_shaken) {
		allegro_iphone_shaken = shake;
		if (config.getShakeAction() == CFG_SHAKE_CHANGE_SONG) {
			iPodNext();
		}
		else if (al_current_time() > next_shake) {
			iphone_shake_time = al_current_time();
			next_shake = al_current_time()+0.5;
		}
	}
#endif

	if (do_acknowledge_resize) {
		al_acknowledge_resize(display);
		do_acknowledge_resize = false;
	}

	if (reload_translation) {
		load_translation(get_language_name(config.getLanguage()).c_str());
		reload_translation = false;
		playPreloadedSample("blip.ogg");
	}

#ifdef ALLEGRO_IPHONE
	if (create_airplay_mirror)
	{
		create_airplay_mirror = false;
		connect_second_display();
	}
	else if (delete_airplay_mirror)
	{
		delete_airplay_mirror = false;
		if (controller_display) {
			int mvol = config.getMusicVolume();
			int svol = config.getSFXVolume();

			disconnect_airplay_controls();
			
			al_destroy_font(game_font_second_display);
			for (int i = 0; i < 8; i++) {
				m_destroy_bitmap(blueblocks[i]);
			}
			m_destroy_bitmap(airplay_dpad);
			m_destroy_bitmap(white_button);
			m_destroy_bitmap(black_button);
			m_destroy_bitmap(airplay_logo);
			//destroy_controller_shader();
			al_destroy_display(controller_display);
			controller_display = NULL;
			
			destroy_shaders();
			al_destroy_display(display);
			
			al_set_new_display_adapter(0);
			al_set_new_display_option(ALLEGRO_AUTO_CONVERT_BITMAPS, 1, ALLEGRO_REQUIRE);
			ScreenDescriptor *sd = config.getWantedGraphicsMode();
			display = al_create_display(sd->height, sd->width);
			register_display(display);
			init_shaders();
			init2_shaders();
	
			_reload_loaded_bitmaps();
			
			m_set_target_bitmap(buffer);
			
			config.setMusicVolume(mvol);
			config.setSFXVolume(svol);
			setMusicVolume(1);
			
			airplay_connected = false;
		}
	}
#endif

#ifdef A5_D3D
	if (should_reset) {
		big_depth_surface->Release();
		_destroy_loaded_bitmaps();
		al_stop_timer(logic_timer);
		al_stop_timer(draw_timer);
		main_halted = true;
		while (d3d_halted) {
			m_rest(0.01);
		}
		init_big_depth_surface();
		_reload_loaded_bitmaps();
		if (in_shooter) {
			shooter_restoring = true;
		}
		al_start_timer(logic_timer);
		al_start_timer(draw_timer);
		main_halted = false;
		should_reset = false;
	}
#endif
	
	if (do_toggle_fullscreen) {
		do_toggle_fullscreen = false;
		toggle_fullscreen();
	}
	
	return close_pressed;
}


void do_close(bool quit)
{
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
	if (mapWidget) {
		mapWidget->auto_save(0, true);
	}
	else if (area && !shouldDoMap) {
		area->auto_save_game(0, true);
	}
	save_memory(true);
	config.write();
	if (quit)
		throw QuitError();
#else
	if (close_pressed_for_configure) {
		fadeOut(black);
		close_pressed_for_configure = false;
		close_pressed = false;
		config_menu();
	}
	else if (prompt("Really exit?", "", 0, 0)) {
		if (saveFilename) saveTime(saveFilename);
		config.write();
		al_set_target_bitmap(al_get_backbuffer(display));
		m_clear(al_map_rgb(0, 0, 0));
		m_flip_display();
		m_clear(al_map_rgb(0, 0, 0));
		m_flip_display();
#ifdef ALLEGRO_WINDOWS
		throw QuitError();
#else
		destroy();
		exit(0);
#endif
	}
#endif
}


static bool playerCanLevel(std::string name)
{
	if (name == "Guard")
		return false;

	return true;
}


void run(void)
{
	// FIXME!.
	//levelUp(party[heroSpot], 10);

	// Fix because Eny used to be only CLASS_WARRIOR, some save
	// states are missing CLASS_ENY
	party[heroSpot]->getInfo().characterClass |= CLASS_ENY;

	runtime_start = tguiCurrentTimeMillis();
	timer_on = false;
	timer_time = 0;

	int counter = 0;
	int frames = 0;

	while (!break_main_loop) {
		// apply healall cheat
		if (healall) {
			healall = false;
			if (battle) {
				for (int i = 0; i < 4; i++) {
					CombatPlayer *p = battle->findPlayer(i);
					if (p) {
						CombatantInfo &i = p->getInfo();
						i.abilities.hp = i.abilities.maxhp;
						i.abilities.mp = i.abilities.maxmp;
					}
				}
			}
			else {
				for (int i = 0; i < 4; i++) {
					Player *p = party[i];
					if (p) {
						CombatantInfo &i = p->getInfo();
						i.abilities.hp = i.abilities.maxhp;
						i.abilities.mp = i.abilities.maxmp;
					}
				}
			}
		}

		al_wait_cond(wait_cond, wait_mutex);
		// Logic
		int tmp_counter = logic_counter;
		logic_counter = 0;
		if (tmp_counter > 10)
			tmp_counter = 1;
		while  (tmp_counter > 0) {
			next_input_event_ready = true;

			counter += LOGIC_MILLIS;
			if (fps_on && counter > 2000) {
				fps = (int)((float)frames/((float)counter/1000.0f));
				counter = frames = 0;
			}

			tmp_counter--;
			if (is_close_pressed()) {
				do_close();
				close_pressed = false;
			}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			if (do_pause_game)
			{
				do_pause_game = false;
				bool ret = pause(true, false);
				if (!ret) {
					fadeOut(black);
					return;
				}
			}
			
			const float MAX_BRIGHT = 0.8f;
			bright_ticker += MAX_BRIGHT/1000.0*bright_dir*LOGIC_MILLIS; // go from normal to white in 1000ms
			if (bright_dir == 1 && bright_ticker >= MAX_BRIGHT) {
				bright_ticker = MAX_BRIGHT;
				bright_dir = -bright_dir;
			}
			else if (bright_dir == -1 && bright_ticker <= 0) {
				bright_ticker = 0;
				bright_dir = -bright_dir;
			}
#endif
			
			// FIXME:
			long now = tguiCurrentTimeMillis();
			runtime_ms += now - runtime_start;
			runtime_start = now;
			while (runtime_ms >= 1000) {
				runtime++;
				runtime_ms -= 1000;
			}
			if (!battle && party[heroSpot] && party[heroSpot]->getName() == "Eny") {
				// seems like the best spot for this
				bool poisoned = false;
				for (int i = 0; i < MAX_PARTY; i++) {
					Player *p = party[i];
					if (!p)
						continue;
					CombatantInfo &info = p->getInfo();
					if (info.abilities.hp < 1)
						continue;
					if (info.condition == CONDITION_POISONED) {
						poisoned = true;
						break;
					}
				}
				party[heroSpot]->getObject()->setPoisoned(poisoned);
			}
			if (timer_on) {
				timer_time -= LOGIC_MILLIS;
				if (timer_time < 0) {
					timer_time = 0;
					if (saveFilename) saveTime(saveFilename);
					ALLEGRO_DEBUG("HOO");

					if (speechDialog) {
						dpad_on();
						delete speechDialog;
						speechDialog = NULL;
					}
					ALLEGRO_DEBUG("HOO");

					if (!battle)
							fadeOut(m_map_rgb(255, 0, 0));
					ALLEGRO_DEBUG("HOO");
					m_set_target_bitmap(buffer);
					ALLEGRO_DEBUG("HOO");
					m_clear(m_map_rgb(255, 0, 0));
					ALLEGRO_DEBUG("HOO");

					if (battle) {
						delete battle;
						battle = NULL;
					}
					ALLEGRO_DEBUG("HOO");

					anotherDoDialogue("You were not quick enough to stop the Golems.\n", false, true);
					ALLEGRO_DEBUG("HOO");

					return;
				}
			}
			// update gui
			mainWidget = tguiUpdate();
			if (manChooser && mainWidget == manChooser) {
				tguiDeleteWidget(manChooser);
			}
			if (speechDialog && mainWidget == speechDialog) {
				tguiDeleteWidget(speechDialog);
				delete speechDialog;
				speechDialog = NULL;
				dpad_on();
				// FIXME: make sure this works
				// dont want tapping away a dialog to make
				// you move once it's closed
				// HERE
				tguiMakeFresh();
			}
			// update battle
			if (battle) {
			
				std::vector<int> levels;
				for (int i = 0; i < MAX_PARTY; i++) {
					if (party[i]) {
						levels.push_back(getLevel(party[i]->getInfo().experience));
					}
					else {
						levels.push_back(-1);
					}
				}
				BattleResult result = battle->update(LOGIC_MILLIS);
				if (result != BATTLE_CONTINUE) {
					if (use_dpad) {
						getInput()->set(false, false, false, false, false, false, false);
					}

					had_battle = true;
					astar_stop();
					if (!timer_on && !manChooser &&
					result != BATTLE_ENEMY_WIN) {
						area->startMusic();
						setMusicVolume(1);
						setAmbienceVolume(1);
					}
					if (result == BATTLE_ENEMY_WIN) {
						battle_won = false;
						battle_lost = true;
					}
					else if (result == BATTLE_PLAYER_RUN) {
						battle_won = false;
						battle_lost = false;
					}
					else if (result == BATTLE_PLAYER_WIN) {
						battle_won = true;
						battle_lost = false;
						for (int i = 0; i < MAX_PARTY; i++) {
							if (party[i] && playerCanLevel(party[i]->getName())) {
								int newLevel = getLevel(party[i]->getInfo().experience);
								for (int l = levels[i]; l < newLevel; l++) {
									while (levelUp(party[i]))
										;
								}
							}
						}
					}
					delete battle;
					battle = NULL;
				
/*
#ifdef LITE
					if (battle->getName() == "Monster") {
						notify("You have reached the end of", "the lite version. Please purchase", "the full version to continue!");
						return;
					}
#endif
*/
					if (result == BATTLE_ENEMY_WIN && battle_must_win) {
						m_rest(5);
						if (saveFilename) saveTime(saveFilename);
						return;
					}
					else if (result == BATTLE_ENEMY_WIN) {
						for (int i = 0; i < MAX_PARTY; i++) {
							Player *p = party[i];
							if (p) {
								if (p->getInfo().abilities.hp <= 0)
									p->getInfo().abilities.hp = 1;
							}
						}
					}
					if (use_dpad) {
						getInput()->setDirection(battleStartDirection);
					}

				}
				levels.clear();
			}
			else if (area) {
				Area *oldArea = area;
				std::string oldAreaName = area->getName();
				area->update(LOGIC_MILLIS);
				
				/* FIXME: Hardcoded (esp. "5") */
				if (area->getName() == "tutorial") {
					if (getNumberFromScript(area->getLuaState(), "stage") == 5) {
						tutorial_started = false;
						return;
					}
				}
				
				// players could be dead
				bool all_dead = true;
				for (int i = 0; i < MAX_PARTY; i++) {
					if (party[i] &&
					party[i]->getInfo().abilities.hp > 0) {
						all_dead = false;
						break;
					}
				}

				bool was = was_in_map;
				was_in_map = false;

				if (all_dead) {
					//FIXME
					if (saveFilename) saveTime(saveFilename);
					return;
				}

				bool did_change_areas = false;

				if (!was && (area->getName() != oldAreaName)) {
					delete oldArea;
					oldArea = NULL;
					area->getObjects()[0]->stop();
					did_change_areas = true;
				}
				
				if (was || area->getName() != oldAreaName)
					dont_draw_now = true;

				if (break_main_loop) {
					break_main_loop = false;
					return;
				}

				if (was || (area->getName() != oldAreaName)) {
					if (oldArea)
						delete oldArea;
					area->getObjects()[0]->stop();
					did_change_areas = true;
				}

				if (did_change_areas) {
					area_panned_x = -BW/2;
					area_panned_y = -BH/2;
				}

				// pause
				if (area && party[heroSpot] && party[heroSpot]->getName() == "Eny" && party[heroSpot]->getObject()->getInput()->isPlayerControlled() &&
						!speechDialog && !path_head) {
					InputDescriptor ie = getInput()->getDescriptor();
					if (ie.button2 || iphone_shaken(0.1)) {
						if (use_dpad) {
							InputDescriptor in = getInput()->getDescriptor();
							while (in.button1) {
								in = getInput()->getDescriptor();
							}
						}
						clear_input_events();
						if (area->getName() == "tutorial") {
							if (prompt("Really exit", "tutorial?", 0, 1)) {
								tutorial_started = false;

								return;
							}
						}
						else {
							iphone_clear_shaken();
							int posx, posy;
							party[heroSpot]->getObject()->getPosition(&posx, &posy);
							bool can_save = true;
							fadeOut(black);
							bool ret = pause(can_save);
							if (!ret) {
								fadeOut(black);
								return;
							}
							
							m_set_target_bitmap(buffer);
							m_clear(black);
							area->draw();
							fadeIn(black);

							runtime_start = tguiCurrentTimeMillis();
						}
					}
				}
			}
		}

		if (break_main_loop) {
			break_main_loop = false;
			return;
		}

		if (draw_counter > 0 && !dont_draw_now) {
			// FIXME:
			frames++;
			draw_counter = 0;
			m_set_target_bitmap(buffer);
			
			/* draw the Area */
			if (battle) {
				battle->draw();
			}
			else if (area) {
				if (gonna_fade_in_red)
					m_clear(m_map_rgb(255, 0, 0));
				else {
					m_clear(black);
					area->draw();
				}
			}
			if (timer_on) {
				int minutes = (timer_time/1000) / 60;
				int seconds = (timer_time/1000) % 60;
				char text[10];
				sprintf(text, "%d:%02d", minutes, seconds);
				int tw = m_text_length(huge_font, "5:55") + 10;
				int th = m_text_height(huge_font);
				mTextout(huge_font, text, BW-(tw/2)-10, th/2+5,
					white, black,
					WGT_TEXT_DROP_SHADOW, true);
			}
			// Draw the GUI
			if (!manChooser || battle)
				tguiDraw();
			if (fps_on) {
				char msg[100];
				sprintf(msg, "%d", fps);
				mTextout(game_font, msg, 0, 0,
					white, black,
					WGT_TEXT_BORDER, false);
			}

			drawBufferToScreen();
			m_flip_display();
		}
		
		dont_draw_now = false;

		if (!battle && party[heroSpot] && party[heroSpot]->getName() == "Eny" && party[heroSpot]->getObject()->getPoisoned() && !gameInfo.milestones[MS_FIRST_POISON]) {
			gameInfo.milestones[MS_FIRST_POISON] = true;
			anotherDoDialogue("Oh, no! Someone is poisoned. They'll lose health every turn until they're healed.\n", false, true);
		}

		//m_rest(0.001);

#ifdef ALLEGRO4
	if (gameInfo.milestones[MS_CELL_SCENE]) {
		if (key[KEY_S] && !battle) {
			if (prompt("Really save? This can", "cause problems!!", 0, 0)) {
				if (saveFilename) {
					char tmp[1000];
					strcpy(tmp, saveFilename);
					saveGame(tmp, "");
				}
				else
					notify("", "Not saved!", "");
			}
		}
	}
      if (key[KEY_F1]) {
         rest(150);
         int v = config.getMusicVolume();
         if (v <= 26) v = 0;
         else v = v - 26; 
         config.setMusicVolume(v);
         setMusicVolume(getMusicVolume());
         setAmbienceVolume(getAmbienceVolume());
      }
      if (key[KEY_F2]) {
         rest(150);
         int v = config.getMusicVolume();
         if (v >= 230) v = 255;
         else v = v + 26; 
         config.setMusicVolume(v);
         setMusicVolume(getMusicVolume());
         setAmbienceVolume(getAmbienceVolume());
      }
      if (key[KEY_F3]) {
         rest(150);
         int v = config.getSFXVolume();
         if (v <= 26) v = 0;
         else v = v - 26; 
         config.setSFXVolume(v);
      }
      if (key[KEY_F4]) {
         rest(150);
         int v = config.getSFXVolume();
         if (v >= 230) v = 255;
         else v = v + 26; 
         config.setSFXVolume(v);
      }
#else

#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
		ALLEGRO_KEYBOARD_STATE state;
		al_get_keyboard_state(&state);
#elif defined IPHONELKFDJSLKFJDSKFLDS
	ALLEGRO_JOYSTICK_STATE joystate;
	al_get_joystick_state(al_get_joystick(0), &joystate);
	if (gameInfo.milestones[MS_CELL_SCENE]) {
		if (joystate.button[7] && !battle) {
			if (prompt("Really save? This can", "cause problems!!", 0, 0)) {
				char tmp[1000];
				strcpy(tmp, saveFilename);
				saveGame(tmp, "");
			}
		}
	}
      if (joystate.button[9]) {
         m_rest(0.15);
         int v = config.getMusicVolume();
         if (v <= 26) v = 0;
         else v = v - 26; 
         config.setMusicVolume(v);
         setMusicVolume(getMusicVolume());
         setAmbienceVolume(getAmbienceVolume());
      }
      if (joystate.button[8]) {
         m_rest(0.15);
         int v = config.getMusicVolume();
         if (v >= 230) v = 255;
         else v = v + 26; 
         config.setMusicVolume(v);
         setMusicVolume(getMusicVolume());
         setAmbienceVolume(getAmbienceVolume());
      }
      if (joystate.button[4]) {
         m_rest(0.15);
         int v = config.getSFXVolume();
         if (v <= 26) v = 0;
         else v = v - 26; 
         config.setSFXVolume(v);
      }
      if (joystate.button[5]) {
         m_rest(0.15);
         int v = config.getSFXVolume();
         if (v >= 230) v = 255;
         else v = v + 26; 
         config.setSFXVolume(v);
      }
      else if (joystate.button[6]) {
      	fps_on = !fps_on;
	m_rest(0.15);
      }
#endif

#endif
	}

	break_main_loop = false;
}

#ifndef EDITOR
#ifdef ALLEGRO_ANDROID
int main(void)
#else
int main(int argc, char *argv[])
#endif
{
#if defined ALLEGRO_WINDOWS && defined A5_OGL
	LPTSTR cmdline = GetCommandLine();
	argc = 1;
	for (int i = 0; cmdline[i]; i++) {
		if (cmdline[i] == ' ') {
			while  (cmdline[i+1] == ' ')
				i++;
			argc++;
		}
	}
	argv = new char *[argc];
	char buf[256];
	int k = 0;
	for (int i = 0; i < argc; i++) {
		int j = 0;
		while  (1) {
			buf[j++] = cmdline[k++];
			if (cmdline[k] == ' ' || cmdline[k] == 0) {
				while  (cmdline[k] == ' ' && cmdline[k+1] == ' ')
					k++;
				k++;
				buf[j] = 0;
				break;
			}
		}
		argv[i] = strdup(buf);
	}
#endif

	try { // QuitError try

	int n;

#ifndef ALLEGRO_ANDROID
	if ((n = check_arg(argc, argv, "-adapter")) != -1) {
		config.setAdapter(atoi(argv[n+1]));
	}
	if (check_arg(argc, argv, "-show-fps") != -1) {
		fps_on = true;
	}
#else
	int argc = 0;
	char **argv = NULL;
#endif

	if (!init(&argc, &argv)) {
		printf("An error occurred during initialization.\n");
		remove(getUserResource("launch_config"));
		return 1;
	}

	ALLEGRO_DEBUG("returned from init");

#ifndef ALLEGRO_ANDROID
	int c = argc;
	char **p = argv;
	while ((n = check_arg(c, p, "-ms")) != -1) {
		int num = atoi(p[n+1]);
		bool value = atoi(p[n+2]);
		std::pair<int, bool> x;
		x.first = num;
		x.second = value;
		forced_milestones.push_back(x);
		c -= n+2;
		p = &p[n+2];
		printf("ms %d %d\n", num, value);
	}
#endif

	// FIXME!
	// Easiest way to restore a save state after deleting the app
#ifdef DEBUG_XXX
	#include "savestate.h"
	//#ifdef OVERWRITE_SAVE
	FILE *f = fopen(getUserResource("auto9.save"), "wb");
	fwrite(savedata, save_state_size, 1, f);
	fclose(f);
	//#endif
#endif


	// Setup HQM (High Quality Music) download path
	hqm_set_download_path(getUserResource("flacs"));


	ALLEGRO_DEBUG("format=%d\n", al_get_new_bitmap_format());
	MBITMAP *nooskewl = m_load_bitmap(getResource("media/nooskewl.png"));
	ALLEGRO_DEBUG("HERE\n");

#ifndef ALLEGRO_ANDROID
	if ((n = check_arg(argc, argv, "-stick")) != -1) {
		int stick = atoi(argv[n+1]);
		config.setStick(stick);
	}
	if ((n = check_arg(argc, argv, "-axis")) != -1) {
		int axis = atoi(argv[n+1]);
		config.setAxis(axis);
	}
	if ((n = check_arg(argc, argv, "-360")) != -1) {
		config.setXbox360(true);
	}
#endif
	
	ALLEGRO_DEBUG("huh1");
	al_set_target_backbuffer(display);
	ALLEGRO_DEBUG("huh2");
	m_clear(al_map_rgb(0, 0, 0));
	ALLEGRO_DEBUG("huh3");
	m_flip_display();
	ALLEGRO_DEBUG("huh4");

	debug_message("loaded nooskewl bmp\n");
	ALLEGRO_DEBUG("buffer format=%d\n", al_get_bitmap_format(buffer->bitmap));
	m_set_target_bitmap(buffer);
	ALLEGRO_DEBUG("HERE2\n");
	debug_message("set target to buffer\n");
	debug_message("cleared buffer\n");
	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
	debug_message("drawing nooskewl bitmap\n");
	m_draw_bitmap(nooskewl, BW/2-m_get_bitmap_width(nooskewl)/2,
		BH/2-m_get_bitmap_height(nooskewl)/2, 0);
	debug_message("drew nooskewl bitmap to buffer, going to transition\n");
	bool cancelled = transitionIn(true, false);
	if (!cancelled) {
		m_set_target_bitmap(buffer);
		m_clear(black);
		m_draw_bitmap(nooskewl, BW/2-m_get_bitmap_width(nooskewl)/2,
			BH/2-m_get_bitmap_height(nooskewl)/2, 0);
		drawBufferToScreen();
		m_flip_display();
		
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			if (is_close_pressed())
				throw QuitError();
#endif
			
		//loadPlayDestroy("nooskewl.ogg");
		m_rest(1.5);
		transitionOut(false);
	}
	else {
		m_rest(1);
	}
	m_destroy_bitmap(nooskewl);

	debug_message("After logo\n");

	#ifdef DEBUG_XXX
	DEBUG_DATA d;
	char *xS;
	char *yS;
	if (prompt("warp?", "", 0, 0)) {
		debug_start(&d);
		xS = strdup(my_itoa(d.x));
		yS = strdup(my_itoa(d.y));
		argc = 5;
		argv = new char *[6];
		argv[0] = "MoRPG2";
		argv[1] = "-warp";
		argv[2] = (char *)d.area.c_str();
		argv[3] = xS;
		argv[4] = yS;
		argv[5] = NULL;
		for (int i = 0; i < (int)d.milestones.size(); i++) {
			forced_milestones.push_back(d.milestones[i]);
		}
	}
	#endif


	
   
#if defined ALLEGRO_IPHONE
	if (al_get_num_video_adapters() > 1) {
		connect_second_display();
	}
#endif


	// FIXME
	//volcano_scene();
	//do_lander(); 
	//archery(false);
	//shooter(false);
	//credits();

	while (!quit_game) {
		playAmbience("");
		playMusic("title.ogg");

		debug_message("After playmusic/ambience\n");
		
		debug_message("After getinputupdate\n");
		
		clear_input_events();

		int choice = 0;
		
#if !defined ALLEGRO_IPHONE && !defined ALLEGRO_ANDROID
		if (al_filename_exists(getUserResource("start_in_config"))) {
			remove(getUserResource("start_in_config"));
			bool result = config_menu(true);
			if (result == true)
				choice = 0xDEAD;
		}
		if (choice != 0xDEAD)
#endif
		choice = title_menu();
		
		m_push_target_bitmap();
		m_set_target_bitmap(buffer);
		m_clear(m_map_rgb(0, 0, 0));
		m_pop_target_bitmap();

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		int remap[4] = { 0, 1, 3, 4 };
#else
		int remap[4] = { 1, 3, 5, 0 };
#endif
		if (choice != 0xDEAD && choice != 0xBEEF) {
			choice = remap[choice];
		}

		debug_message("after title_menu\n");

		m_rest(0.25);
		
		if (choice == 0) {
			try {
				if (!loadGame(getUserResource("auto0.save"))) {
					Area *oldArea = area;
					area->update(1);
					if (area != oldArea) {
						delete oldArea;
						was_in_map = false;
					}
					else {
						m_set_target_bitmap(buffer);
						area->draw();
						transitionIn();
					}
				}
				player = party[heroSpot];
			}
			catch (ReadError e) {
				notify("No auto-save", "yet recorded...", "");
				continue;
			}
		}
		else if (choice == 1) {
			int num;
			bool exists;
			bool isAuto;

			choose_savestate(&num, &exists, &isAuto);

			m_set_target_bitmap(buffer);
			m_clear(black);
			drawBufferToScreen();
			m_flip_display();

			if (num < 0) {
				continue;
			}
			else if (exists) {
				try {
					debug_message("loading game\n");
					if (!loadGame(getUserResource("%s%d.save", isAuto ? "auto" : "", num))) {
						Area *oldArea = area;
						area->update(1);
						if (area != oldArea) {
							delete oldArea;
							was_in_map = false;
						}
						else {
							m_set_target_bitmap(buffer);
							area->draw();
							transitionIn();
						}
					}
					player = party[heroSpot];
					debug_message("loaded\n");
				}
				catch (ReadError e) {
					notify("Error loading...", "", "");
					continue;
				}
			}
			else {
				debug_message("creating player\n");
				player = new Player("Eny"); // auto put in party
				player->setObject(new Object());
				player->getObject()->setPerson(true);
				player->getObject()->setAnimationSet(std::string(getResource("objects/Eny.png")));
				al_lock_mutex(input_mutex);
				player->getObject()->setInput(getInput());
				al_unlock_mutex(input_mutex);
				heroSpot = 0;
				debug_message("calling startNewGame\n");
				party[heroSpot] = player;
				startNewGame();
				if (saveFilename) free(saveFilename);
				saveFilename = NULL;
				//saveFilename = strdup(getUserResource("%d.save", num));
				runtime = 0;
				gold = 0;
				runtime_ms = 0;
				runtime_start = 0;
				debug_message("started new game\n");
			}

			int n;
			if ((n = check_arg(argc, argv, "-warp")) != -1) {
				if (area) delete area;
				startArea(argv[n+1]);
				// FIXME! xcode bug!
				//int x = atoi(argv[n+2]);
				//int y = atoi(argv[n+3]);
				int x = 5;
				int y = 5;
				party[heroSpot]->getObject()->setPosition(x, y);
			}
		}
		else if (choice == 2) {
		// FIXME!
		#if 0
			if (do_config()) {
				notify("You must restart the program", "for the changes to take effect", "... Exiting.");
				break;
			}
			continue;
		#endif
		}
		else if (choice == 5) {
			pc_help();
			continue;
		}
		else if (choice == 3) {
			player = new Player("Eny"); // auto put in party
			player->setObject(new Object());
			player->getObject()->setPerson(true);
			player->getObject()->setAnimationSet(getResource("objects/Eny.png"));
			al_lock_mutex(input_mutex);
			player->getObject()->setInput(getInput());
			al_unlock_mutex(input_mutex);
			heroSpot = 0;
			party[heroSpot] = player;
			tutorial_started = true;
			startNewGame("tutorial");
		}
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
		else if (choice == 4) {
			save_memory(true);
			config.write();
#ifdef LITEXX
			openFeedbackSite();
#else
#ifdef ALLEGRO_ANDROID
			openURL("http://www.monster-rpg.com");
			exit(0);
#else
			openRatingSite();
#endif
#endif
		}
		#endif
		else if (choice == 0xBEEF) {
			config.write();
			throw QuitError();
		}
		else if (choice == 0xDEAD) {
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
			do_close(true);
#elif defined ALLEGRO_MACOSX
			destroy();
			//char const *exe = [[[NSBundle mainBundle] executablePath] fileSystemRepresentation];
			//int r = execl(exe, exe, NULL);
			/*
			if (fork()) {
				exit(0);
			}
			*/
			/*
			CFURLRef url = CFBundleCopyExecutableURL(CFBundleGetMainBundle());
			FSRef fsref;
			CFURLGetFSRef(url, &fsref);
			char exe[1024]; 
			FSRefMakePath(&fsref, (UInt8*)exe, 1024);
			execl(exe, exe, NULL);
			*/
#else
#ifndef _MSC_VER // FIXME!
			char exe[1024];
			sprintf(exe, "%s", argv[0]);
			execl(exe, exe, NULL);
#endif
#endif
		}
		else {
			break;
		}

		run();

		save_memory(true);

		/* tguiDeleteWidget(manChooser)? */
		if (manChooser) {
			delete manChooser;
			manChooser = NULL;
			dpad_on();
		}
		al_lock_mutex(input_mutex);
		if (player_scripted) {
			player_scripted = false;
			dpad_on();
		}
		astar_stop();
		al_unlock_mutex(input_mutex);


		if (area) {
			delete area;
	//		dpad_on();
		}
		area = NULL;

		al_lock_mutex(input_mutex);
		for (int i = 0; i < MAX_PARTY; i++) {
			if (party[i]) {
				Object *o = party[i]->getObject();
				if (o)
					delete o;
				delete party[i];
				party[i] = NULL;
			}
		}
		player = NULL;
		resetIds();
		initInput();
		al_unlock_mutex(input_mutex);
	}
		
	} catch (QuitError e) {
		al_set_target_bitmap(al_get_backbuffer(display));
		m_clear(al_map_rgb(0, 0, 0));
		m_flip_display();
		m_clear(al_map_rgb(0, 0, 0));
		m_flip_display();
	}

#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
done:
#endif

#ifndef ALLEGRO_WINDOWS
	al_unlock_mutex(wait_mutex);
#endif

#if !defined ALLEGRO_IPHONE
	destroy();
#endif

#if defined ALLEGRO_WINDOWS && defined A5_OGL
	for (int i = 0; i < argc; i++) {
		debug_message("deleting argv[%d]\n", i);
		free(argv[i]);
	}
	debug_message("delete[]ing argv\n");
	delete[] argv;
#endif

	debug_message("done\n");

	return 0;
}
#endif
