#ifndef AREA_H
#define AREA_H

#include "monster2.hpp"


struct Collision {
	int id1;
	int id2;
};


const int MAX_TILES = (BW/TILE_SIZE+2)*(BH/TILE_SIZE+2);


class Area {
public:
	// Shameless global members :(
	// If this is set the pan will quickly center on the player
	bool center_view;
	// set to -1 on obj(0)->stop
	int last_player_x, last_player_y;
	void auto_save_game(int step, bool ignoreCount = false);
	void auto_save_screenshot(int step, bool force=false);
	lua_State* getLuaState();
	void initLua();
	std::vector<Object*> &getObjects();
	Object* findObject(uint id);
	// objId is id of object checking for collisions
	bool isOccupied(uint objId, int x, int y, bool doCollide = true, bool check_objs = true, bool check_tiles = true);
	bool onAnyObject(uint objId, int x, int y);
	bool activate(uint id, int direction);
	void addObject(Object* obj);
	void removeObject(uint id);
	bool alreadyCollided(int o1, int o2);
	void drawObject(int index);
	void drawObject(Object *o);
	void startMusic();
	void getWaterAnimation(int x, int y, Animation **anim, int *depth);

	static const int MAX_AREA_NAME = 256;
	static const int TINT_TIME = 1500;
	
	uint    getWidth(void);
	uint    getHeight(void);
	void setWidth(int w);
	void setHeight(int h);
	std::string getName();
	Tile*   getTile(int x, int y);

	std::vector<Tile *> &getTiles(void);
	std::vector<int> &getAnimationNums(void);
	std::vector<Animation *> &getAnimations(void);
	Animation *getTileAnimation(int index);
	std::string getTerrain(void);
    	
	void setTile(int x, int y, Tile *t);
	void copyTile(int x, int y, Tile *t);
	
	Tile* loadTile(ALLEGRO_FILE *f);
	
	void save(std::string filename);
	
	void load(std::string filename);
	void update(int step);
	void draw(int bw = BW, int bh = BH);
	void tint(MCOLOR *color, bool reverse = false); // color is relative to the pixel in the texture

	void writeTile(int tile, gzFile f);

#ifdef EDITOR
	Area(int w, int h) throw (std::bad_alloc);	// create empty area for editor
	void saveBmp(std::string filename);
#endif
	
	void followPlayer(bool follow);
	void setFocus(int x, int y);
	int getFocusX(void);
	int getFocusY(void);
	int getOriginX(void);
	int getOriginY(void);
	void setOriginX(int ox);
	void setOriginY(int oy);

	void resetInput(void) {
		down = false;
	}
	
	void loadAnimation(int index, bool addIndex);

	Area(void);
	~Area(void);
	
private:
	void adjustPanX(void);
	void adjustPanY(void);
	void adjustPan(void);
	lua_State* luaState;
	std::vector<Collision*> collisions;
	std::vector<Object*> objects;
	bool tinting;
	int tintCount;
	MCOLOR targetTint;
	float tint_ratio;
	bool reverseTint;
	MBITMAP *bg;
#ifdef ALLEGRO4
	uint16_t tint_r, tint_g, tint_b;
#endif
        
	//int offsetx, offsety;
	//float offsetx, offsety;
	int sizex, sizey;
	std::string name;
    	
	void drawLayer(int layer, int bw, int bh);

	std::vector<Tile *> tiles;
	//std::vector<Object*> sObjects;	// sorted objects
	bool follow;
#ifdef EDITOR
	ALLEGRO_VERTEX verts[6*(240/16)*(160/16)*1000];
#else
	ALLEGRO_VERTEX verts[MAX_TILES*6*(TILE_LAYERS/2)];
#endif
	int num_quads;
	struct anim_data {
		int delay;
		int count;
		int current_frame;
		std::vector<int> tu;
		std::vector<int> tv;
	};
	std::map<int, anim_data> anim_info;
	bool down;
	double down_time;
	//bool moved;
	bool panned;
	int start_mx;
	int start_my;
	//int saved_start_mx;
	//int saved_start_my;
	int totalUpdates;
	Area *oldArea;
	float focusX, focusY;
	bool in_activate;
	std::vector<int> removedDuringActivate;

	int update_count;

public:
	int tm_w;
	int tm_h;
	MBITMAP *partial_tm;
	int tm_used;
	std::map<int, int> newmap;
	std::vector<int> tileAnimationNums;
	std::vector<Animation *> tileAnimations;
};


extern Area *area;
extern Area* oldArea;
extern std::string prevAreaName;


void startArea(std::string name);
std::string getTerrain(void);
void blitTile(int tile, MBITMAP* tilemap, MBITMAP* dest);

const char *file_date(const char *fn);

struct Node {
	Node *parent;
	Node *child;
	int x, y;
	int CostFromStart;
	int CostToGoal;
	int TotalCost;
	bool operator==(const Node &n) {
		return this->TotalCost == n.TotalCost;
	}
	bool operator<(const Node &n) {
		return this->TotalCost < n.TotalCost;
	}
	bool operator()(const Node *a, const Node *b) const {
		return a->TotalCost <= b->TotalCost;
	}
	Node(void) {
	}
	Node &operator=(const Node &rhs) {
		parent = rhs.parent;
		child = rhs.child;
		x = rhs.x;
		y = rhs.y;
		CostFromStart = rhs.CostFromStart;
		CostToGoal = rhs.CostToGoal;
		TotalCost = rhs.TotalCost;
		return *this;
	}
	Node(const Node &rhs) {
		parent = rhs.parent;
		child = rhs.child;
		x = rhs.x;
		y = rhs.y;
		CostFromStart = rhs.CostFromStart;
		CostToGoal = rhs.CostToGoal;
		TotalCost = rhs.TotalCost;
	}
};

void set_player_path(int x, int y);
void cleanup_astar();
void astar_stop(void);
Node *get_path_tail(void);
void save_memory(bool save_screenshot = true);
#if defined ALLEGRO_IPHONE || defined ALLEGRO_ANDROID
void auto_save();
void delete_file(const char *s);
#endif
extern long roaming;
extern bool dpad_panning;
extern bool pan_centered_x;
extern bool pan_centered_y;
extern short coord_map_x[4096];
extern short coord_map_y[4096];
//extern bool offs_centered_x;
//extern bool offs_centered_y;

extern int mapping[4096];

extern Node *path_head;
extern Node *path_tail;

void real_auto_save_screenshot(void);

#endif
