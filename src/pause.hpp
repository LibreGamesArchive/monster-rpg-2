#ifndef PAUSE_HPP
#define PAUSE_HPP

bool pause(bool can_save, bool change_music_volume = true, std::string mapName = "");
void doMap(std::string start, std::string prefix = "map");
void doShop(std::string name, const char *imageName, int nItems,
	int *indexes, int *costs);
void into_the_sun(void);
void credits(void);
void choose_savestate(int *num, bool *existing, bool *isAuto);
void choose_savestate_old(std::string caption, bool paused, bool autosave, bool (*callback)(int num, bool exists, void *data), void *data);
int title_menu(void);
void showItemInfo(int index, bool preserve_buffer = false);
void showSaveStateInfo(const char *basename);
char *create_url(unsigned char *bytes, int len);
void save_url(const char *filename, const char *buf);
bool config_menu(bool start_on_fullscreen = false);
void pc_help(void);

#ifdef DEBUG
struct DEBUG_DATA {
	std::string area;
	int x, y;
	std::vector<std::pair<int, bool> > milestones;
};
void debug_start(DEBUG_DATA *d);
#endif

class MMap;

extern bool in_map;
extern MMap *mapWidget;
extern bool in_pause;

#endif

