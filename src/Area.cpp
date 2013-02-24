#include "monster2.hpp"
#ifdef ALLEGRO_IPHONE
#import <Foundation/Foundation.h>
#endif

#include <sys/stat.h>

static bool compare_node_pointer(Node *n1, Node *n2)
{
	return n1->TotalCost < n2->TotalCost;
}

Area* area = 0;
long roaming = 0;
bool dpad_panning = false;

const float ORB_RADIUS = 40.0f;

#if !defined ALLEGRO_IPHONE
const char *file_date(const char *filename)
{
	static char buf[1000];
	struct stat s;

	stat(filename, &s);

#ifdef ALLEGRO_MACOSX
	strcpy(buf, ctime((const time_t *)&s.st_mtimespec.tv_sec));
#else
	strcpy(buf, ctime((const time_t *)&s.st_mtime));
#endif
	if (buf[strlen(buf)-1] == '\n')
		buf[strlen(buf)-1] = 0;

	return buf;
}
#endif

#ifdef ALLEGRO_IPHONE
const char *file_date(const char *filename)
{
	static char buf[1000];

	NSString *path = [
		[NSString alloc]
		initWithCString:filename encoding:NSUTF8StringEncoding];
	NSDictionary *dict = [
		[NSFileManager defaultManager]
		attributesOfItemAtPath:path error:NULL];
	NSDate *date = [dict objectForKey:NSFileModificationDate];
	NSString *dateStr = [date description];
	[dateStr getCString:buf maxLength:999 encoding:NSUTF8StringEncoding];
	[path release];
	return buf;
}

void delete_file(const char *s)
{
	NSString *f = [[NSString alloc] initWithCString:s encoding:NSUTF8StringEncoding];
	[[NSFileManager defaultManager] removeItemAtPath:f error:NULL];
	[f release];
}

static void shift_auto_saves()
{
	char src_c[1000];
	char dst_c[1000];
	for (int i = 8; i >= 0; i--) {
		strcpy(src_c, getUserResource("auto%d.save", i));
		strcpy(dst_c, getUserResource("auto%d.save", i+1));
		NSString *src = [[NSString alloc] initWithCString:src_c encoding:NSUTF8StringEncoding];
		NSString *dst = [[NSString alloc] initWithCString:dst_c encoding:NSUTF8StringEncoding];
		[[NSFileManager defaultManager] removeItemAtPath:dst error:NULL];
		[[NSFileManager defaultManager] moveItemAtPath:src toPath:dst error:NULL];
		[src release];
		[dst release];

		strcpy(src_c, getUserResource("auto%d.png", i));
		strcpy(dst_c, getUserResource("auto%d.png", i+1));
		src = [[NSString alloc] initWithCString:src_c encoding:NSUTF8StringEncoding];
		dst = [[NSString alloc] initWithCString:dst_c encoding:NSUTF8StringEncoding];
		[[NSFileManager defaultManager] removeItemAtPath:dst error:NULL];
		[[NSFileManager defaultManager] moveItemAtPath:src toPath:dst error:NULL];
		[src release];
		[dst release];

		strcpy(src_c, getUserResource("auto%d.bmp", i));
		strcpy(dst_c, getUserResource("auto%d.bmp", i+1));
		src = [[NSString alloc] initWithCString:src_c encoding:NSUTF8StringEncoding];
		dst = [[NSString alloc] initWithCString:dst_c encoding:NSUTF8StringEncoding];
		[[NSFileManager defaultManager] removeItemAtPath:dst error:NULL];
		[[NSFileManager defaultManager] moveItemAtPath:src toPath:dst error:NULL];
		[src release];
		[dst release];
	}
}
#else
static void shift_auto_saves()
{
	char src_c[1000];
	char dst_c[1000];
	for (int i = 8; i >= 0; i--) {
		strcpy(src_c, getUserResource("auto%d.save", i));
		strcpy(dst_c, getUserResource("auto%d.save", i+1));
		remove(dst_c);
		rename(src_c, dst_c);

		strcpy(src_c, getUserResource("auto%d.png", i));
		strcpy(dst_c, getUserResource("auto%d.png", i+1));
		remove(dst_c);
		rename(src_c, dst_c);

		strcpy(src_c, getUserResource("auto%d.bmp", i));
		strcpy(dst_c, getUserResource("auto%d.bmp", i+1));
		remove(dst_c);
		rename(src_c, dst_c);
	}
}
#endif

static void *save_auto_save_to_disk_thread(void *save_ss)
{
	bool save_screenshot = (save_ss != NULL);
	bool all_dead = true;
	for (int i = 0; i < MAX_PARTY; i++) {
		if (party[i] &&
		party[i]->getInfo().abilities.hp > 0) {
			all_dead = false;
			break;
		}
	}

	if (all_dead)
		return NULL;

	if (!memory_saved)
		return NULL;

	shift_auto_saves();
	gzFile f = gzopen(getUserResource("auto0.save"), "wb9");
	if (f) {
		for (int i = 0; i < memory_save_offset; i++) {
			gzputc(f, memory_save[i]);
		}
		gzclose(f);
	}
	if (screenshot && save_screenshot) {
#ifdef ALLEGRO_ANDROID
		al_set_standard_file_interface();
#endif
		al_save_bitmap(getUserResource("auto0.bmp"), screenshot->bitmap);
#ifdef ALLEGRO_ANDROID
		al_android_set_apk_file_interface();
#endif
	}

	return NULL;
}

void save_auto_save_to_disk()
{
	al_run_detached_thread(save_auto_save_to_disk_thread, (void *)1);
}

static int sign(float f)
{
	if (f < 0)
		return -1;
	else if (f > 0)
		return 1;
	else
		return 0;
}


// A* stuff

static int astar_next_x = -1, astar_next_y = -1;
static int astar_next_immediate_x = -1, astar_next_immediate_y = -1;

static bool astar_is_solid(int x, int y, int px, int py, int dest_x, int dest_y, bool check_objs = true)
{
	if (area->getName() == "Muttrace") {
		if (x == 20 && y == 23) {
			if (px == 20 && py == 23)
				return false;
			else if (dest_x == 20 && dest_y == 23)
				return false;
			else
				return true;
		}
	}

	int area_w = area->getWidth();
	int area_h = area->getHeight();

	if (x == px && y == py)
		return false;
	if ((dest_x == 0) && (x == 0) && (y != dest_y))
		return true;
	if ((dest_x == area_w-1) && (x == area_w-1) && (y != dest_y))
		return true;
	if ((dest_y == 0) && (y == 0) && (x != dest_x))
		return true;
	if ((dest_y == area_h-1) && (y == area_h-1) && (x != dest_x))
		return true;

	if (x < 0 || y < 0 || x >= area_w || y >= area_h || x < px-20 || x > px+20 || y < py-15 || y > py+15) // This sets limit (faster)
		return true;
	return area->isOccupied(0, x, y, false, check_objs, true);
}

std::list<Node *> as_open;
std::list<Node *> as_closed;

static std::list<Node *>::iterator astar_find(std::list<Node *> &nodes, int x, int y)
{
	std::list<Node *>::iterator it;
	for (it = nodes.begin(); 
		  it != nodes.end();
		  it++) {
		Node *current = *it;
		if (current->x == x &&
			 current->y == y) {
			return it;
		}
	}
	return nodes.end();
}

Node *path_tail = NULL;
Node *path_head = NULL;

void cleanup_astar(void)
{
	if (path_tail) {
		delete path_tail;
		path_tail = NULL;
	}

	std::list<Node *>::iterator it;
	
	for (it = as_open.begin(); it != as_open.end(); it++) {
		delete *it;
	}
	
	std::list<Node *>::iterator it2;

	for (it2 = as_closed.begin(); it2 != as_closed.end(); it2++) {
		delete *it2;
	}

	as_open.clear();
	as_closed.clear();
}


static Node *astar(int start_x, int start_y, int dest_x, int dest_y)
{
	cleanup_astar(); // no leak
	
	if (astar_is_solid(dest_x, dest_y, start_x, start_y, dest_x, dest_y, false)) {
		astar_stop();
		return NULL;
	}
	
#define AS_ESTIMATE(sx, sy, dx, dy) \
(abs(sx-dx) + abs(sy-dy))
	
	const int offsets[4][2] = {
		{  0, -1 },            
		{ -1,  0 },             {  1,  0 },
		{  0,  1 }            
	};

	int count = 0;

	Node *StartNode = new Node();
	count++;
	StartNode->x = start_x;
	StartNode->y = start_y;
	StartNode->CostFromStart = 0;
	StartNode->CostToGoal = AS_ESTIMATE(start_x, start_y, dest_x, dest_y);
	StartNode->TotalCost = StartNode->CostToGoal;
	StartNode->parent = NULL;
	as_open.insert(as_open.begin(), StartNode);

	while (as_open.size()) {
		Node *n = *(as_open.begin());
		as_open.erase(as_open.begin());
		
		if (n->x == dest_x && n->y == dest_y) {
			return n;
		}
		else {
			for (int i = 0; i < 4; i++) {
				int x = n->x + offsets[i][0];
				int y = n->y + offsets[i][1];
			
				if (!(x == dest_x && y == dest_y) && astar_is_solid(x, y, start_x, start_y, dest_x, dest_y)) {
					Node *NewNode = new Node;
					NewNode->x = x;
					NewNode->y = y;
					as_closed.push_back(NewNode);

					continue;
				}
				
				// cost always 1
				int NewCost = n->CostFromStart + 1;
				
				bool in_open = false;
				bool in_closed = false;
				
				std::list<Node *>::iterator open_it;
				open_it = astar_find(as_open, x, y);
				if (open_it != as_open.end()) {
					in_open = true;
				}
				
				std::list<Node *>::iterator closed_it;
				closed_it = astar_find(as_closed, x, y);
				if (closed_it != as_closed.end()) {
					in_closed = true;
				}

				int costFromStart;
				if (in_open)
					costFromStart = (*open_it)->CostFromStart;
				else if (in_closed)
					costFromStart = (*closed_it)->CostFromStart;
				
				if ((in_open || in_closed) &&
					costFromStart <= NewCost) {
					continue;
				}
				else {
					Node *NewNode = new Node();
					count++;
					NewNode->x = x;
					NewNode->y = y;
					NewNode->parent = n;
					NewNode->CostFromStart = NewCost;
					NewNode->CostToGoal =
					AS_ESTIMATE(
									NewNode->x, NewNode->y,
									dest_x, dest_y
									);
					NewNode->TotalCost =
					NewNode->CostFromStart +
					NewNode->CostToGoal;
					
					in_open = in_closed = false;
					
					open_it =
					astar_find(
								  as_open,
								  NewNode->x,
								  NewNode->y
								  );
					if (open_it != as_open.end()) {
						in_open = true;
					}
					
					closed_it =
					astar_find(
								  as_closed,
								  NewNode->x,
								  NewNode->y
								  );
					if (closed_it != as_closed.end()) {
						in_closed = true;
					}
					
					if (in_closed) {
						delete *closed_it;
						as_closed.erase(closed_it);
						count--;
					}
					if (in_open) {
						delete NewNode;
						count--;
					}
					else {
						as_open.insert(as_open.begin(), NewNode);
						as_open.sort(compare_node_pointer);
					}
				}
			}
		}
		as_closed.push_back(n);
	}
	
	return NULL;
	
#undef AS_ESTIMATE
}

Node *get_path_tail(void)
{
	return path_tail;
}

static bool was_a_click = false;

void set_player_path(int x, int y)
{
	if (speechDialog) {
		return;
	}

	Player *p = party[heroSpot];
	if (p) {
		Object *o = p->getObject();
		if (o) {
			x = (x + area->getOriginX()) / TILE_SIZE;
			y = (y + area->getOriginY()) / TILE_SIZE;
			bool activated = false;
			Input *i = o->getInput();
			if ((!battle) && i && abs(x-o->getX()) <= 1 && abs(y-o->getY()) <= 1) {
				int dx = x - o->getX();
				int dy = y - o->getY();
				const int dirs[3][3] = {
					{  -1,  DIRECTION_NORTH,  -1 },
					{ DIRECTION_WEST,  -1, DIRECTION_EAST },
					{  -1,  DIRECTION_SOUTH,  -1 }
				};
				int d;
				if (dx < -1 || dy < -1 || dx > 1 || dy > 1) {
					d = -1;
				}
				else {
					d = dirs[dy+1][dx+1];
				}
				if (d != -1) {
					was_a_click = true;
					activated = area->activate(0, d);
					was_a_click = false;
					if (activated) {
						o->stop();
						i->set(false, false, false, false, false, false, false);
						path_head = NULL;
						astar_stop();
						released = true; // HACK!
						return;
					}
				}
			}
			path_tail = astar(o->getX(), o->getY(), x, y);

			if (path_tail) {
				Node *tmp = path_tail;
				tmp->child = NULL;
				while (tmp->x != o->getX() || tmp->y != o->getY()) {
					tmp->parent->child = tmp;
					tmp = tmp->parent;
				}
				path_head = tmp;
			}
			else {
				path_head = NULL;
				astar_stop();
			}
		}
	}
}

void astar_stop(void)
{
	Input *i = getInput();
	if (i) {
		//i->set(false, false, false, false);
		// FIXME: was above. TODO: Make sure this change doesn't break anything
		i->set(false, false, false, false, false, false, false);
	}
	path_head = NULL;
	Player *p = party[heroSpot];
	if (p) {
		Object *o = p->getObject();
		if (o) {
			o->stop();
		}
	}
	astar_next_x = astar_next_y = -1;
	astar_next_immediate_x = astar_next_immediate_y = -1;
}

// game only methods
void Area::drawObject(int index)
{
	objects[index]->draw();
	
	int posx, posy, offsx, offsy, dimx, dimy;
	objects[index]->getPosition(&posx, &posy);
	objects[index]->getOffset(&offsx, &offsy);
	objects[index]->getDimensions(&dimx, &dimy);

	if (objects[index]->isMoving()) {
		int tx1, ty1, tx2, ty2;
		int x1 = posx*TILE_SIZE;
		int y1 = posy*TILE_SIZE;
		Animation *anim1, *anim2;
		int depth1;
		tx1 = x1/TILE_SIZE;
		ty1 = y1/TILE_SIZE;
		getWaterAnimation(tx1, ty1, &anim1, &depth1);
		int depth2;
		tx2 = tx1+sign(offsx);
		ty2 = ty1+sign(offsy);
		getWaterAnimation(tx2, ty2, &anim2, &depth2);
		if (anim1 != NULL) {
			bool draw = false;
			int depth;
			int y;
			if (offsy >= 0 && offsy < depth1) {
				depth = depth1 - offsy;
				y = TILE_SIZE-depth;
				draw = true;
			}
			else if (offsy < 0) {
				depth = TILE_SIZE + offsy;
				if (depth > depth1) depth = depth1;
				y = (TILE_SIZE+offsy) - depth;
				draw = true;
			}
			if (draw) {
				MBITMAP *bmp = anim1->getCurrentFrame()->getImage()->getBitmap();
				m_draw_bitmap_region(bmp, 0, y, TILE_SIZE, depth, tx1*TILE_SIZE-getOriginX(), ty1*TILE_SIZE+y-getOriginY(), 0);
			}
		}
		if (anim2 != NULL) {
			bool draw = false;
			int depth;
			int y;
			if (offsy > 0) {
				depth = offsy;
				if (depth > depth2) depth = depth2;
				y = offsy - depth;
				draw = true;
			}
			else if (offsy < -(TILE_SIZE-depth2)) {
				depth = depth2 - (TILE_SIZE+offsy);
				y = TILE_SIZE - depth;
				draw = true;
			}
			else if (offsy == 0) {
				depth = depth2;
				y = TILE_SIZE-depth2;
				draw = true;
			}
			if (draw) {
				MBITMAP *bmp = anim2->getCurrentFrame()->getImage()->getBitmap();
				m_draw_bitmap_region(bmp, 0, y, TILE_SIZE, depth, tx2*TILE_SIZE-getOriginX(), ty2*TILE_SIZE+y-getOriginY(), 0);
			}
		}
	}
	else {
		int x = posx;
		int y = posy;
		Animation *anim;
		int depth;
		getWaterAnimation(x, y, &anim, &depth);
		if (anim != NULL) {
			MBITMAP *bmp = anim->getCurrentFrame()->getImage()->getBitmap();
			int dx, dy;
			dx = (int)((x * TILE_SIZE) - getOriginX());
			dy = (int)((y * TILE_SIZE) - getOriginY() - (dimy - TILE_SIZE));
			dy += dimy;
			dy -= depth;
			if (tinting) {
				if (use_programmable_pipeline) {
				al_set_shader(display, tinter);
				al_set_shader_sampler(tinter, "tex", bmp->bitmap, 0);
				al_set_shader_float(tinter, "ratio", tint_ratio);
				al_set_shader_float(tinter, "r", targetTint.r);
				al_set_shader_float(tinter, "g", targetTint.g);
				al_set_shader_float(tinter, "b", targetTint.b);
				al_use_shader(tinter, true);
				m_draw_bitmap_region(bmp, 0, TILE_SIZE-depth, TILE_SIZE,
					depth, dx, dy, 0);
				al_use_shader(tinter, false);
				al_set_shader(display, default_shader);
				}
				else
				{
				m_save_blender();
				float d, r, g, b;
				d = targetTint.r - 1;
				r = 1+(tint_ratio*d);
				d = targetTint.g - 1;
				g = 1+(tint_ratio*d);
				d = targetTint.b - 1;
				b = 1+(tint_ratio*d);
				m_set_blender(ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA, al_map_rgb_f(r, g, b));
				m_draw_bitmap_region(bmp, 0, TILE_SIZE-depth, TILE_SIZE,
					depth, dx, dy, 0);
				m_restore_blender();
				}
			}
			else {
				m_draw_bitmap_region(bmp, 0, TILE_SIZE-depth, TILE_SIZE,
					depth, dx, dy, 0);
			}
		}
	}

	objects[index]->drawUpper();
}



void Area::drawObject(Object *o)
{
	for (int i = 0; i < (int)objects.size(); i++) {
		if (objects[i] == o) {
			drawObject(i);
			break;
		}
	}
}

bool Area::activate(uint id, int direction)
{
	in_activate = true;

	bool activated = false;

	Object* o = findObject(id);

	int ox = o->getX();
	int oy = o->getY();

	int x1, y1;

	switch (direction) {
		case DIRECTION_NORTH:
			x1 = ox;
			y1 = oy - 1;
			break;
		case DIRECTION_EAST:
			y1 = oy;
			x1 = ox + 1;
			break;
		case DIRECTION_SOUTH:
			x1 = ox;
			y1 = oy + 1;
			break;
		case DIRECTION_WEST:
		default:
			y1 = oy;
			x1 = ox - 1;
			break;
	}

	if (!speechDialog) {
		for (unsigned int i = 0; i < objects.size(); i++) {
			if (objects[i]->getId() == id)
				continue;
			Object* obj = objects[i];
			std::vector<int *> &v = obj->getOccupied();
			for (int j = 0; j < (int)v.size(); j++) {
				if (v[j][0] == x1 && v[j][1] == y1) {
					activated = true;
					setObjectDirection(o, direction);
					getInput()->waitForReleaseOr(4, 5000);
					callLua(luaState, "activate", "ii>", id, obj->getId());
					if (was_a_click) {
						callLua(luaState, "collide", "ii>", id, obj->getId());
					}
				}
			}
		}
	}

	in_activate = false;

	return activated;
}

bool Area::alreadyCollided(int id1, int id2)
{
	for (unsigned int i = 0; i < collisions.size(); i++) {
		Collision* c = collisions[i];
		if ((c->id1 == id1 && c->id2 == id2) ||
			(c->id1 == id2 && c->id2 == id1))
				return true;
	}
	return false;
}

/*
 * Add an object to an area. Player should always be object 1.
 */
void Area::addObject(Object* obj)
{
	objects.push_back(obj);
}

/*
 * Remove an object from the object vectors and delete it.
 */
void Area::removeObject(uint id)
{
	if (in_activate) {
		removedDuringActivate.push_back(id);
		return;
	}

	for (unsigned int i = 0; i < objects.size(); i++) {
		if (objects[i]->getId() == id) {
			delete objects[i];
			std::vector<Object*>::iterator it = objects.begin() + i;
			objects.erase(it);
			break;
		}
	}
}


/*
 * Locate an object by id
 */
Object* Area::findObject(uint id)
{
	for (unsigned int i = 0; i < objects.size(); i++) {
		Object *o = objects[i];
		if (o->getId() == id)
			return o;
	}

	return NULL;
}

bool Area::onAnyObject(uint objId, int posx, int posy)
{
	// Check for objects
	for (uint i = 0; i < objects.size(); i++) {
		Object *o = objects[i];
		if (o->getId() == objId)
			continue;
		std::vector<int *> &objOccupied = o->getOccupied();
		for (uint j = 0; j < objOccupied.size(); j++) {
			int *v = objOccupied[j];
			if (v[0] == posx && v[1] == posy) {
				return true;
			}
		}
	}

	return false;
}


bool Area::isOccupied(uint objId, int posx, int posy, bool doCollide, bool check_objects, bool check_tiles)
{
	if (!findObject(objId)->isSolid())
		return false;

	// check bounds
	int xx = posx;
	int yy = posy;
	if (xx < 0 || yy < 0 || xx >= sizex || yy >= sizey) {
		return true;
	}

	if (check_tiles) {
		// Check area for solids
		Tile *tile = tiles[posx+posy*sizex];
		if (tile->isSolid())
			return true;
	}
	
	if (check_objects) {
		// Check for objects
		for (uint i = 0; i < objects.size(); i++) {
			Object *o = objects[i];
			if (o->getId() == objId || !o->isSolid())
				continue;
			std::vector<int *> &objOccupied = o->getOccupied();
			for (uint j = 0; j < objOccupied.size(); j++) {
				int *v = objOccupied[j];
				if (v[0] == posx && v[1] == posy) {
					if (doCollide && !alreadyCollided(o->getId(), objId)) {
						Collision *c = new Collision;
						c->id1 = o->getId();
						c->id2 = objId;
						collisions.push_back(c);
					}
					if (o->isSpecialWalkable())
						return false;
					return true;
				}
			}
		}
	}

	return false;
}

void Area::initLua()
{
	debug_message("in initLua for Area\n");
	luaState = lua_open();

	openLuaLibs(luaState);

	registerCFunctions(luaState);

	runGlobalScript(luaState);

	unsigned char *bytes;
	int file_size;

	ALLEGRO_DEBUG("going to load global area script");

	debug_message("Loading global area script...\n");
	bytes = slurp_file(getResource("area_scripts/global.%s", getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error("Load error.", "area_scripts/global.lua");
	ALLEGRO_DEBUG("slurped %d bytes", file_size);
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		throw ReadError();
	}
	delete[] bytes;
	ALLEGRO_DEBUG("loaded it");

	debug_message("Running global area script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}

	ALLEGRO_DEBUG("gonna load the other one");

	debug_message("Loading area script (%s)...\n", name.c_str());
	bytes = slurp_file(getResource("area_scripts/%s.%s", name.c_str(), getScriptExtension().c_str()), &file_size);
	if (!bytes) native_error("Load Error.", ((std::string("area_scripts/") + name + ".lua").c_str()));
	if (luaL_loadbuffer(luaState, (char *)bytes, file_size, "chunk")) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ReadError();
	}
	delete[] bytes;

	ALLEGRO_DEBUG("loaded the other one!");

	debug_message("Running area script...\n");
	if (lua_pcall(luaState, 0, 0, 0)) {
		dumpLuaStack(luaState);
		lua_close(luaState);
		throw ScriptError();
	}

	debug_message("Calling 'start'...\n");

	callLua(luaState, "start", ">");

	startMusic();

	adjustPan();


	ALLEGRO_DEBUG("initLua done!");
}

void Area::startMusic(void)
{
	playAmbience("");

	std::string mname;
	std::string aname;

	lua_getglobal(luaState, "music");
	if (!lua_isnil(luaState, -1)) {
		mname = std::string(lua_tostring(luaState, -1));
	}
	else
		mname = "";

	lua_pop(luaState, 1);

	lua_getglobal(luaState, "ambience");
	if (!lua_isnil(luaState, -1)) {
		aname = std::string(lua_tostring(luaState, -1));
	}
	else
		aname = "";

	lua_pop(luaState, 1);

	playMusic(mname);

	if (aname != "") {
		playAmbience(aname);
	}
	
	setMusicVolume(1);
	setAmbienceVolume(1);
}


std::vector<Object*> &Area::getObjects()
{
	return objects;
}


lua_State* Area::getLuaState()
{
	return luaState;
}


void Area::tint(MCOLOR *target, bool reverse)
{
	tinting = true;
	tintCount = 0;
	reverseTint = reverse;

	targetTint = *target;
}

void startArea(std::string name)
{
	char resName[1000];
	snprintf(resName, 1000, "areas/%s.area", name.c_str());
	
	area = new Area();
	if (!area->load(getResource(resName))) {
		native_error("Load Error.", resName);
	}

	area->addObject(party[heroSpot]->getObject());

	area->initLua();
	
	Input *i;
	if ((i = getInput())) {
		clear_input_events();
		path_head = NULL;
		Player *p = party[heroSpot];
		if (p) {
			Object *o = p->getObject();
			if (o) {
				o->stop();
			}
		}
	}
}

std::string Area::getTerrain(void)
{
	int x = player->getObject()->getX();
	int y = player->getObject()->getY();

	std::list<XMLData *> &nodes = terrain->getNodes();
	std::list<XMLData *>::iterator it;

	for (it = nodes.begin(); it != nodes.end(); it++) {
		XMLData *node = *it;
		std::string name = node->getName();
		XMLData *nodeX = node->find("x");
		XMLData *nodeY = node->find("y");
		XMLData *nodeW = node->find("width");
		XMLData *nodeH = node->find("height");
		int xi = atoi(nodeX->getValue().c_str());
		int yi = atoi(nodeY->getValue().c_str());
		int wi = atoi(nodeW->getValue().c_str());
		int hi = atoi(nodeH->getValue().c_str());
		Tile *tile = tiles[x+y*sizex];
		for (int i = 0; i < TILE_LAYERS; i++) {
			int tx, ty;
			int animNum = tile->getAnimationNum(i);
			if (animNum < 0)
				continue;
			int num = tileAnimationNums[animNum];
			if (num < 0) {
				continue;
			}
			tx = num % (512/TILE_SIZE);
			ty = num / (512/TILE_SIZE);
			if (tx >= xi && tx < (xi+wi) && ty >= yi && ty < (yi+hi)) {
				return name;
			}
		}
	}

	return "";
}


void Area::getWaterAnimation(int x, int y, Animation **anim, int *depth)
{
	int vecSize = (int)waterData.size();
	for (int j = 0; j < vecSize; j++) {
		Tile *tile = tiles[x+y*sizex];
		bool found = false;
		int animNum;
		int waterTileNum = waterData[j].tilenum;
		int i;
		for (i = 0; i < TILE_LAYERS; i++) {
			animNum = tile->getAnimationNum(i);
			if (animNum < 0)
				continue;
			int num = tileAnimationNums[animNum];
			if (num < 0) {
				continue;
			}
			if (num == waterTileNum) {
				found = true;
				break;
			}
		}
		if (found) {
			// found already true ^
			for (; i < TILE_LAYERS/2; i++) {
				if (tile->getAnimationNum(i) >= 0 &&
						tileAnimationNums[tile->getAnimationNum(i)] != waterTileNum) {
					found = false;
					break;
				}
			}
			if (found) {
				*anim = getTileAnimation(animNum);
				*depth = waterData[j].depth;
				return;
			}
		}
	}

	*anim = NULL;
	*depth = 0;
}


void Area::drawLayer(int i, int bw, int bh)
{
	int startTilex = getOriginX() / TILE_SIZE;
	int startTiley = getOriginY() / TILE_SIZE;

	int startOffsetx = (-(int)getOriginX()) % TILE_SIZE;
	int startOffsety = (-(int)getOriginY()) % TILE_SIZE;

	int ox, oy;
	int tx, ty;
	
	// Draw tiles

	ALLEGRO_COLOR c;
	if (!use_programmable_pipeline && tinting) {
		float d, r, g, b;
		d = targetTint.r - 1;
		r = 1+(tint_ratio*d);
		d = targetTint.g - 1;
		g = 1+(tint_ratio*d);
		d = targetTint.b - 1;
		b = 1+(tint_ratio*d);
		c = al_map_rgb_f(r, g, b);
	}
	else {
		c = white;
	}

	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);

	for (ty = startTiley, oy = startOffsety;
 	     ty < sizey && oy < bh; ty++, oy += TILE_SIZE) {
		for (tx = startTilex, ox = startOffsetx;
		     tx < sizex && ox < bw; tx++, ox += TILE_SIZE) {
		     	if (tx < 0 || ty < 0 || tx >= sizex || ty >= sizey)
				continue;
			Tile *t = tiles[tx+ty*sizex];
			int n = t->getAnimationNum(i);
#ifdef EDITOR
			if (n >= 0 && n < (int)tileAnimations.size() && tileAnimationNums[n]) {
				int mapped = tileAnimationNums[n];
#else
			if (n >= 0 && n < (int)tileAnimations.size() && tileAnimationNums[n]) {
				int mapped = newmap[tileAnimationNums[n]];
#endif
				std::map<int, anim_data>::iterator it  = anim_info.find(mapped);
				float tu;
				float tv;
				if (it != anim_info.end()) {
					/* Animated tile */
					anim_data &ad = it->second;
					tu = ad.tu[ad.current_frame];
					tv = ad.tv[ad.current_frame];
				}
				else {
					tu = t->getTU(i);
					tv = t->getTV(i);
				}

				const int extra = 16;

				ALLEGRO_VERTEX *v = &verts[num_quads*6];
				v->x = ox;
				v->y = oy;
				v->u = tu;
				v->v = tv;
				v->color = c;
				v = &verts[num_quads*6+1];
				v->x = ox;
				v->y = oy+TILE_SIZE;
				v->u = tu;
				v->v = tv+extra;
				v->color = c;
				v = &verts[num_quads*6+2];
				v->x = ox+TILE_SIZE;
				v->y = oy+TILE_SIZE;
				v->u = tu+extra;
				v->v = tv+extra;
				v->color = c;
				v = &verts[num_quads*6+3];
				v->x = ox;
				v->y = oy;
				v->u = tu;
				v->v = tv;
				v->color = c;
				v = &verts[num_quads*6+4];
				v->x = ox+TILE_SIZE;
				v->y = oy+TILE_SIZE;
				v->u = tu+extra;
				v->v = tv+extra;
				v->color = c;
				v = &verts[num_quads*6+5];
				v->x = ox+TILE_SIZE;
				v->y = oy;
				v->u = tu+extra;
				v->v = tv;
				v->color = c;
				num_quads++;
			}
		}
	}

#ifdef EDITOR // draw solids
	if (i == (TILE_LAYERS-1) && showSolids) {
		for (ty = startTiley, oy = startOffsety;
		     ty < sizey && oy < bh; ty++, oy += TILE_SIZE) {
			for (tx = startTilex, ox = startOffsetx;
			     tx < sizex && ox < bw; tx++, ox += TILE_SIZE) {
					Tile *t = tiles[tx+ty*sizex];
					if (t->isSolid()) {
						int rx1 = ox;
						int ry1 = oy;
						int rx2 = ox + TILE_SIZE;
						int ry2 = oy + TILE_SIZE;
						MCOLOR color;
						color = m_map_rgb(255, 255, 0);
						m_draw_line(rx1, ry1, rx2, ry2,
							color);
						m_draw_line(rx1, ry2, rx2, ry1,
							color);
					}
			}
		}
	}
#endif // EDITOR
}


static void insertObject(Object *o, std::vector<Object *>& sorted)
{
	std::vector<Object *>::iterator it = sorted.begin();

	for (; it != sorted.end(); it++) {
		Object *obj = *it;
		if (obj->getY() > o->getY())
			break;
	}

	sorted.insert(it, o);
}

void Area::draw(int bw, int bh)
{
	num_quads = 0;

	if (bg) {
		int bgox = (float)getOriginX() / (sizex*TILE_SIZE) * (m_get_bitmap_width(bg)-BW);
		int bgoy = (float)getOriginY() / (sizey*TILE_SIZE) * (m_get_bitmap_height(bg)-BH);
		m_save_blender();
		m_set_blender(ALLEGRO_ONE, ALLEGRO_ZERO, white);
		m_draw_bitmap_region(bg, bgox, bgoy, BW, BH, 0, 0, 0);
		m_restore_blender();
	}

	if (tinting) {
		m_save_blender();
		float d, r, g, b;
		d = targetTint.r - 1;
		r = 1+(tint_ratio*d);
		d = targetTint.g - 1;
		g = 1+(tint_ratio*d);
		d = targetTint.b - 1;
		b = 1+(tint_ratio*d);
		m_set_blender(ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA, al_map_rgb_f(r, g, b));
		for (int j = 0; j < TILE_LAYERS/2; j++) {
			drawLayer(j, bw, bh);
		}
		
		m_restore_blender();
	}
	else {
		for (int i = 0; i < TILE_LAYERS/2; i++) {
			drawLayer(i, bw, bh);
		}
	}
	
#ifndef NO_SHADERS
	if (num_quads > 0) {
		if (tinting && use_programmable_pipeline) {
			al_set_shader(display, tinter);
			al_set_shader_float(tinter, "ratio", tint_ratio);
			al_set_shader_float(tinter, "r", targetTint.r);
			al_set_shader_float(tinter, "g", targetTint.g);
			al_set_shader_float(tinter, "b", targetTint.b);
			al_use_shader(tinter, true);
		}
		m_draw_prim(verts, 0, partial_tm, 0, num_quads*6,
			ALLEGRO_PRIM_TRIANGLE_LIST);
		if (tinting && use_programmable_pipeline) {
			al_use_shader(tinter, false);
			al_set_shader(display, default_shader);
		}
	}

	num_quads = 0;
#endif
		
	// draw the lower objects

	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
		
	if (manChooser && !battle) tguiDraw();

	std::vector<Object *> sorted_objects;

	for (unsigned int i = 0; i < objects.size(); i++) {
		insertObject(objects[i], sorted_objects);
	}

	for (unsigned int i = 0; i < sorted_objects.size(); i++) {
		int p = i;
		if (sorted_objects[i] == objects[0]) {
			i++;
			while (i < objects.size() && sorted_objects[i]->getY() == objects[0]->getY()) {
				i++;
			}
			Object *tmp = sorted_objects[i-1];
			sorted_objects[i-1] = sorted_objects[p];
			sorted_objects[p] = tmp;
			break;
		}
	}

	/* Draw low objects */
	for (unsigned int i = 0; i < sorted_objects.size(); i++) {
		if (sorted_objects[i]->isLow() && !sorted_objects[i]->isHidden()) {
			drawObject(sorted_objects[i]);
		}
	}

	/* Draw regular objects */
	for (unsigned int i = 0; i < sorted_objects.size(); i++) {
		if (!sorted_objects[i]->isHigh() && !sorted_objects[i]->isLow() && !sorted_objects[i]->isHidden()) {
			drawObject(sorted_objects[i]);
		}
	}


	if (tinting) {
		m_save_blender();
		float d, r, g, b;
		d = targetTint.r - 1;
		r = 1+(tint_ratio*d);
		d = targetTint.g - 1;
		g = 1+(tint_ratio*d);
		d = targetTint.b - 1;
		b = 1+(tint_ratio*d);
		m_set_blender(ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA, al_map_rgb_f(r, g, b));
		for (int j = TILE_LAYERS/2; j < TILE_LAYERS; j++) {
			drawLayer(j, bw, bh);
		}
		m_restore_blender();
	}
	else {
		for (int i = TILE_LAYERS/2; i < TILE_LAYERS; i++) {
			drawLayer(i, bw, bh);
		}
	}

#ifndef NO_SHADERS
	if (num_quads > 0) {
		if (tinting && use_programmable_pipeline) {
			al_set_shader(display, tinter);
			al_set_shader_float(tinter, "ratio", tint_ratio);
			al_set_shader_float(tinter, "r", targetTint.r);
			al_set_shader_float(tinter, "g", targetTint.g);
			al_set_shader_float(tinter, "b", targetTint.b);
			al_use_shader(tinter, true);
		}
		m_draw_prim(verts, 0, partial_tm, 0, num_quads*6,
			ALLEGRO_PRIM_TRIANGLE_LIST);
		if (tinting && use_programmable_pipeline) {
			al_use_shader(tinter, false);
			al_set_shader(display, default_shader);
		}
	}
#endif

	m_set_blender(M_ONE, M_INVERSE_ALPHA, white);

	// Draw high and flying objects
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (objects[i]->isHigh() && !objects[i]->isHidden())
			objects[i]->draw();
	}
#ifndef EDITOR
	if (name == "darkside") {
		if (gameInfo.milestones[MS_GOT_ORB]) {
			int player_posx, player_posy;
			int player_offsx, player_offsy;
			party[heroSpot]->getObject()->getPosition(&player_posx, &player_posy);
			party[heroSpot]->getObject()->getOffset(&player_offsx, &player_offsy);
			int px = ((player_posx * TILE_SIZE) + player_offsx + TILE_SIZE/2);
			int py = ((player_posy * TILE_SIZE) + player_offsy - TILE_SIZE/2) + TILE_SIZE;
			px -= getOriginX();
			py -= getOriginY();
			int x1, y1, x2, y2;
			x1 = px - ORB_RADIUS;
			y1 = py - ORB_RADIUS;
			x2 = px + ORB_RADIUS;
			y2 = py + ORB_RADIUS;
			// draw rects
			m_draw_rectangle(x1, y1-BH, x1-BW, y2+BH, black, M_FILLED);
			m_draw_rectangle(x2, y1-BH, x2+BW, y2+BH, black, M_FILLED);
			m_draw_rectangle(x1, y1, x2, y1-BH, black, M_FILLED);
			m_draw_rectangle(x1, y2, x2, y2+BH, black, M_FILLED);
			// draw circle
			m_draw_bitmap(orb_bmp, x1, y1, 0);
			al_set_target_backbuffer(display);
		}
		else {
			m_draw_rectangle(0, 0, BW, BH, black, M_FILLED);
		}
	
		// draw lit
		for (int i = 0; i < (int)sorted_objects.size(); i++) {
			Object *o = sorted_objects[i];
			if (o->getAnimationSet()->getSubName() == "lit") {
				drawObject(o);
			}
		}
		drawObject(objects[0]);
	}
	
	sorted_objects.clear();
#endif
}

#ifdef EDITOR
void Area::saveBmp(std::string filename)
{
	int pixw = sizex * TILE_SIZE;
	int pixh = sizey * TILE_SIZE;

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags | ALLEGRO_MEMORY_BITMAP);
	MBITMAP *mem = m_create_bitmap(pixw, pixh); // check
	al_set_new_bitmap_flags(flags);

	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	m_set_target_bitmap(mem);
	m_clear(al_map_rgb(0, 0, 0));
	draw(pixw, pixh);
	al_set_target_bitmap(target);

	al_save_bitmap(filename.c_str(), mem->bitmap);

	m_destroy_bitmap(mem);
}
#endif

Tile* Area::getTile(int x, int y)
{
	return tiles[y*sizex+x];
}

void Area::setTile(int x, int y, Tile *t)
{
	tiles[y*sizex+x] = t;
}

void Area::copyTile(int x, int y, Tile *t)
{
       Tile *areaTile = tiles[x+y*sizex];

       for (int i = 0; i < TILE_LAYERS; i++) {
               areaTile->setAnimationNum(i, t->getAnimationNum(i));
       }

       areaTile->setSolid(t->isSolid());
}

static int ss_save_counter = 10000;
static int mem_save_counter = 10000;

void real_auto_save_game_to_memory(bool save_ss)
{
	mem_save_counter = 0;
	ss_save_counter = 0;

	had_battle = false;
	memory_save_offset = 0;
	using_memory_save = true;
	saveGame(NULL);
	using_memory_save = false;
	memory_saved = true;

	if (save_ss) {
		int dx, dy, dw, dh;
		get_screen_offset_size(&dx, &dy, &dw, &dh);
		m_draw_scaled_backbuffer(dx, dy, dw, dh, 0, 0, BW/2, BH/2, screenshot);
	}
	else {
		ALLEGRO_BITMAP *old = al_get_target_bitmap();
		m_set_target_bitmap(screenshot);
		al_clear_to_color(blue);
		al_set_target_bitmap(old);
	}
}
	
void Area::auto_save_game_to_memory(int step, bool ignoreCount, bool save_ss)
{
	if (!battle && !player_scripted && !manChooser && !timer_on && name != "tutorial" && oldArea == area && global_can_save) {
		mem_save_counter += step;
		ss_save_counter += step;
		Object *o = party[heroSpot]->getObject();
		if (
		
			(ignoreCount || mem_save_counter >= 10000 || had_battle || totalUpdates < 5)


			// FIXME: is this bad news?
			&& !onAnyObject(o->getId(), o->getX(), o->getY())
		)
		{
			real_auto_save_game_to_memory(save_ss);
		}
	}
}

bool pan_centered_x = false;
bool pan_centered_y = false;

void Area::adjustPanX(void)
{
	bool smallX = sizex*TILE_SIZE <= BW;
	if (!smallX) {
		if (getOriginX() < 0) {
			area_panned_x += -getOriginX();
		}
		else if (getOriginX()+BW > sizex*TILE_SIZE) {
			area_panned_x -= ((getOriginX()+BW) - (sizex*TILE_SIZE));
		}
	}
	else {
		area_panned_x = (-(BW-sizex*TILE_SIZE)/2) - getFocusX();
	}
}

void Area::adjustPanY(void)
{
	bool smallY = sizey*TILE_SIZE <= BH;
	if (!smallY) {
		if (getOriginY() < 0) {
			area_panned_y += -getOriginY();
		}
		else if (getOriginY()+BH > sizey*TILE_SIZE) {
			area_panned_y -= ((getOriginY()+BH) - (sizey*TILE_SIZE));
		}
	}
	else {
		area_panned_y = (-(BH-sizey*TILE_SIZE)/2) - getFocusY();
	}
}

void Area::adjustPan(void)
{
	adjustPanX();
	adjustPanY();
}
		
void Area::update(int step)
{
	std::vector<int> toDelete;

	roaming = tguiCurrentTimeMillis();

	shouldDoMap = false;

	totalUpdates++;

	for (unsigned int i = 0; i < collisions.size(); i++) {
		callLua(luaState, "collide", "ii>", collisions[i]->id1,
				collisions[i]->id2);
		delete collisions[i];
	}
	collisions.clear();

	oldArea = area;
	callLua(luaState, "update", "i>", step);

	for (unsigned int i = 0; i < objects.size(); i++) {
		if (!objects[i]->update(this, step)) {
			toDelete.push_back(i);
		}
		if (!speechDialog && !dpad_panning) {
			try {
				Input *input = objects[i]->getInput();
				if (input) {
					InputDescriptor id = input->getDescriptor();
					if (id.button1) {
						if (activate(objects[i]->getId(), id.direction)) {
						}
						else if (!player_scripted && !battle && !manChooser
								&& this == area && !speechDialog) {
							dpad_panning = true;
						}
					}
				}
			}
			catch (...) {
				continue;
			}
		}
	}

	for (int i = 0; i < (int)toDelete.size(); i++) {
		int index = toDelete[i] - i;
		delete objects[index];
		objects.erase(objects.begin() + index);
	}

	if (this == area) {
		if (update_count == 0) {
			update_count = 1;
		}
		else {
			//auto_save_game_to_memory(step);
		}
	}
	else
		return;

	bool adjusted_pan = false;

	if (!player_scripted && !battle && !manChooser
			&& this == area && !speechDialog) {
		if (!use_dpad) {
			if (!released && !down) {
				down = true;
				al_lock_mutex(click_mutex);
				start_mx = click_x;
				start_my = click_y;
				current_mouse_x = start_mx;
				current_mouse_y = start_my;
				al_unlock_mutex(click_mutex);
				down_time = al_current_time();
			}
			else if (down) {
				al_lock_mutex(click_mutex);
				int cx = current_mouse_x;
				int cy = current_mouse_y;
				al_unlock_mutex(click_mutex);
				if ((abs(cx-start_mx) > 5 || abs(cy-start_my) > 5) && cx >= 0 && !path_head) {
					// panning
					area_panned_x += (start_mx - cx);
					area_panned_y += (start_my - cy);
					if (area_panned_x < -(BW-TILE_SIZE/2)) {
						area_panned_x = -(BW-TILE_SIZE/2);
					}
					else if (area_panned_x > -TILE_SIZE/2) {
						area_panned_x = -TILE_SIZE/2;
					}
					if (area_panned_y < -(BH-TILE_SIZE/2)) {
						area_panned_y = -(BH-TILE_SIZE/2);
					}
					else if (area_panned_y > -TILE_SIZE/2) {
						area_panned_y = -TILE_SIZE/2;
					}
					start_mx = cx;
					start_my = cy;
					panned = true;
					adjusted_pan = true;
				}
				if (released) {
					down = false;
					if (!panned) {
						int tx = (start_mx + area->getOriginX()) / TILE_SIZE;
						int ty = (start_my + area->getOriginY()) / TILE_SIZE;
						callLua(luaState, "activate_any", "ii>b", tx, ty);
						bool used;
						if (lua_isboolean(luaState, -1)) {
							used = lua_toboolean(luaState, -1);
						}
						else {
							used = false;
						}
						lua_pop(luaState, 1);
						if (!used) {
							if (!path_head) {
								set_player_path(start_mx, start_my);
								astar_next_x = -1;
								astar_next_immediate_x = -1;
							}
							else {
								astar_next_immediate_x = start_mx;
								astar_next_immediate_y = start_my;
							}
						}
					}
					panned = false;
				}
				else {
					if (al_current_time() > down_time+0.2 && !panned) {
						astar_next_x = cx;
						astar_next_y = cy;
						down_time = al_current_time();
					}
				}
			}
		}
		if (dpad_panning) {
			InputDescriptor id = getInput()->getDescriptor();
			for (int i = 0; i < (int)(0.2 * step); i++) {
				if (id.up) {
					area_panned_y--;
					adjusted_pan = true;
					if (area_panned_y < -(BH-TILE_SIZE/2)) {
						area_panned_y = -(BH-TILE_SIZE/2);
					}
				}
				else if (id.down) {
					area_panned_y++;
					adjusted_pan = true;
					if (area_panned_y > -TILE_SIZE/2) {
						area_panned_y = -TILE_SIZE/2;
					}
				}
				if (id.left) {
					area_panned_x--;
					adjusted_pan = true;
					if (area_panned_x < -(BW-TILE_SIZE/2)) {
						area_panned_x = -(BW-TILE_SIZE/2);
					}
				}
				else if (id.right) {
					area_panned_x++;
					adjusted_pan = true;
					if (area_panned_x > -TILE_SIZE/2) {
						area_panned_x = -TILE_SIZE/2;
					}
				}
			}
		}

		if (adjusted_pan) {
			adjustPan();
		}

	#define GET_DIRECTIONS(path_head) \
		if (path_head->x > px) {  \
			l = false;  \
			r = true;  \
		} \
		else if (path_head->x < px) {  \
			l = true;  \
			r = false;  \
		} \
		else {  \
			l = false;  \
			r = false;  \
		} \
		if (path_head->y > py) {  \
			u = false;  \
			d = true;  \
		} \
		else if (path_head->y < py) {  \
			u = true;  \
			d = false;  \
		} \
		else {  \
			u = false;  \
			d = false;  \
		}
			
		if (path_head) {
			Player *p = party[heroSpot];
			if (p) {
				Object *o = p->getObject();
				Input *i = o->getInput();
				if (o && i) {
					bool l, r, u, d;
					int px = o->getX();
					int py = o->getY();
					GET_DIRECTIONS(path_head)
					bool blocked = false;
					if (l == false && r == false && u == false && d == false) {
						if (astar_next_immediate_x >= 0) {
							set_player_path(
								astar_next_immediate_x, astar_next_immediate_y
							);
							astar_next_x = -1;
							astar_next_immediate_x = -1;
							if (path_head && !path_head->child) {
								o->stop();
								i->set(false, false, false, false);
								path_head = NULL;
							}
						}
						else if (!path_head->child) {
							if (astar_next_x >= 0) {
								set_player_path(
									astar_next_x, astar_next_y
								);
								astar_next_x = -1;
								astar_next_immediate_x = -1;
							}
							if (path_head && !path_head->child) {
								o->stop();
								i->set(false, false, false, false);
								path_head = NULL;
							}
						}
						if (path_head && path_head->child) {
							path_head = path_head->child;
							GET_DIRECTIONS(path_head)
							InputDescriptor id = i->getDescriptor();
							int nextdir;
							if (l) nextdir = DIRECTION_WEST;
							else if (r) nextdir = DIRECTION_EAST;
							else if (u) nextdir = DIRECTION_NORTH;
							else nextdir = DIRECTION_SOUTH;
							if (id.direction != nextdir) {
								o->stop();
							}
							if (area->isOccupied(0, path_head->x, path_head->y, true, true)) {
								// something got in the way
								o->stop();
								i->set(false, false, false, false);
								path_head = NULL;
								blocked = true;
							}
						}
					}
					if (path_head && !blocked && !player_scripted) {
						i->set(l, r, u, d);
					}
				}
			}
		}
	}

	// delete for real anything deleted in an activate() callback
	for (int sz = (int)removedDuringActivate.size(); sz > 0; sz--) {
		removeObject(removedDuringActivate[0]);
		removedDuringActivate.erase(removedDuringActivate.begin());
	}

	if (shouldDoMap) {
		draw(BW, BH);
		if (mapPrefix == "<none>")
			doMap(mapStartPlace);
		else
			doMap(mapStartPlace, mapPrefix);
	}

	bool smallX = sizex*TILE_SIZE <= BW;
	bool smallY = sizey*TILE_SIZE <= BH;
	bool changed;
	
	if (center_view && !smallX) {
		for (int i = 0; i < (int)(0.2*step); i++) {
			changed = false;
			if (area_panned_x > -BW/2) {
				if (getOriginX()-1 >= 0) {
					area_panned_x -= 1;
					changed = true;
				}
			}
			else if (area_panned_x < -BW/2) {
				if (getOriginX()+BW+1 <= sizex*TILE_SIZE) {
					area_panned_x += 1;
					changed = true;
				}
			}
			if (!changed) {
				adjustPanX();
			}
		}
	}
	else if (follow && party[heroSpot] && party[heroSpot]->getName() == "Eny"
			 && oldArea == area) {
		adjustPanX();
	}

	if (center_view && !smallY) {
		for (int i = 0; i < (int)(0.2*step); i++) {
			changed = false;
			if (area_panned_y > -BH/2) {
				if (getOriginY()-1 >= 0) {
					area_panned_y -= 1;
					changed = true;
				}
			}
			else if (area_panned_y < -BH/2) {
				if (getOriginY()+BH+1 <= sizey*TILE_SIZE) {
					area_panned_y += 1;
					changed = true;
				}
			}
			if (!changed) {
				adjustPanY();
			}
		}
	}
	else if (follow && party[heroSpot] && party[heroSpot]->getName() == "Eny"
			 && oldArea == area) {
		adjustPanY();
	}

	// update animated tiles

	std::map<int, anim_data>::iterator it;
	for (it = anim_info.begin(); it != anim_info.end(); it++) {
		anim_data &a = it->second;
		a.count += step;
		if (a.count > a.delay) {
			a.count -= a.delay;
			a.current_frame++;
			a.current_frame = (a.current_frame % (int)a.tu.size());
		}
	}
	
	for (unsigned int i = 0; i < tileAnimations.size(); i++) {
		tileAnimations[i]->update(step);
	}

	if (tinting) {
		tintCount += step;
		tint_ratio = (float)tintCount / TINT_TIME;
		if (tint_ratio >= 1.0f) {
			tint_ratio = 0.999999f;
		}
		if (reverseTint) {
			tint_ratio = 1 - tint_ratio;
			if (tint_ratio < 0.01) {
				tinting = false;
			}
		}
	}
}

void Area::writeTile(int tile, gzFile f)
{
	for (int i = 0; i < TILE_LAYERS; i++) {
		iputl(tiles[tile]->getAnimationNum(i), f);
	}
	my_pack_putc((char)tiles[tile]->isSolid(), f);
}

Tile* Area::loadTile(ALLEGRO_FILE *f)
{
	Tile* tile = 0;

	try {
		int anims[TILE_LAYERS];
		short tu[TILE_LAYERS];
		short tv[TILE_LAYERS];
		for (int i = 0; i < TILE_LAYERS; i++) {
			anims[i] = al_fread32le(f);
			if (anims[i] >= 0) {
				int n = newmap[tileAnimationNums[anims[i]]];
				tu[i] = (n % tm_w) * TILE_SIZE;
				tv[i] = (n / tm_w) * TILE_SIZE;
			}
		}
		bool solid = al_fgetc(f);
		tile = new Tile(anims, solid, tu, tv);
	}
	catch (...) {
		if (tile)
			delete tile;
		throw ReadError();
	}

	return tile;
}

void Area::save(std::string filename)
{
	gzFile f = NULL;
	f = gzopen(filename.c_str(), "wb");
	if (!f)
		throw WriteError();

	try {
		iputl(sizex, f);
		iputl(sizey, f);
		iputl(tileAnimationNums.size(), f);
		for (unsigned int i = 0; i < tileAnimationNums.size(); i++) {
			int x = tileAnimationNums[i];
			iputl(x, f);
		}
		for (int i = 0; i < sizex*sizey; i++)
			writeTile(i, f);
	}
	catch (WriteError) {
		gzclose(f);
		throw WriteError();
	}

	gzclose(f);
}

std::string Area::getName()
{
	return name;
}

std::vector<int> &Area::getAnimationNums(void)
{
	return tileAnimationNums;
}

std::vector<Animation *> &Area::getAnimations(void)
{
	return tileAnimations;
}

Animation *Area::getTileAnimation(int index)
{
	return tileAnimations[index];
}

unsigned int Area::getWidth(void)
{
	return (uint)sizex;
}

void Area::setWidth(int w)
{
	sizex = w;
}

void Area::setHeight(int h)
{
	sizey = h;
}

unsigned int Area::getHeight(void)
{
	return (uint)sizey;
}

void Area::loadAnimation(int index, bool addIndex)
{
	// FIXME hardcoded
	if (index < 0 || index >= ((512/16)*(2048/16))/*>= numTiles*/)
		return;

	char n[100];
	sprintf(n, "%d", index);
	std::string number(n);

	XMLData *data = tilemap_data->find(number);

	XMLData *a = trans_data->find(number);
	int alpha;
	if (a) {
		alpha = atoi(a->find("alpha")->getValue().c_str());
		debug_message("Alpha tile %d\n", index);
	}
	else
		alpha = 255;

	int delay;
	if (data) {
		XMLData *delay_xml = data->find("delay");
		std::string value = delay_xml->getValue();
		delay = atoi(value.c_str());
	}
	else
		delay = 0;

	Frame *frame;
	Animation *animation;
	Image *image;
	int subx = index % (512/TILE_SIZE);
	int suby = index / (512/TILE_SIZE);

	ALLEGRO_STATE st;
	al_store_state(&st, ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	char nm[100];
#ifdef ALLEGRO_ANDROID
	sprintf(nm, "tiles/%d-%d.png", subx, suby);
	MBITMAP *tmp = m_load_bitmap(getResource(nm));
#else
	sprintf(nm, "%d-%d.png", subx, suby);
	al_set_physfs_file_interface();
	MBITMAP *tmp = m_load_bitmap(nm);
	al_set_standard_file_interface();
	al_save_bitmap("test.png", tmp->bitmap);
#endif
	int xx = tm_used % tm_w;
	int yy = tm_used / tm_w;
	tm_used++;
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	m_set_target_bitmap(partial_tm);
	m_draw_bitmap(tmp, xx*TILE_SIZE, yy*TILE_SIZE, 0);
	MBITMAP *subbmp = m_create_sub_bitmap(partial_tm, xx*TILE_SIZE, yy*TILE_SIZE, TILE_SIZE, TILE_SIZE); // check
	m_destroy_bitmap(tmp);
	al_restore_state(&st);
	image = new Image();
	int flags = al_get_new_bitmap_flags();
	image->set(subbmp);
	al_set_new_bitmap_flags(flags);
	frame = new Frame(image, delay);
	animation = new Animation("hmm", alpha);
	animation->addFrame(frame);
	tileAnimations.push_back(animation);

	if (addIndex) {
		newmap[index] = tm_used-1;
		tileAnimationNums.push_back(index);
	}
	
	if (!data) {
		return;
	}

	// animated tile

	anim_data ad;
	ad.delay = delay;
	ad.count = 0;
	ad.current_frame = 0;
	std::vector<int> tu;
	std::vector<int> tv;

	tu.push_back(xx*TILE_SIZE);
	tv.push_back(yy*TILE_SIZE);

	int i = 2;

	for (;;) {
		sprintf(n, "%d", i);
		std::string iS(n);
		XMLData *index_xml = data->find(iS);
		if (!index_xml)
			break;
		std::string index_string = index_xml->getValue();
		int index = atoi(index_string.c_str());
		subx = index % (512/TILE_SIZE);
		suby = index / (512/TILE_SIZE);


		ALLEGRO_STATE st;
		al_store_state(&st, ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);

#ifdef ALLEGRO_ANDROID
		sprintf(nm, "tiles/%d-%d.png", subx, suby);
		MBITMAP *tmp = m_load_bitmap(getResource(nm));
#else
		sprintf(nm, "%d-%d.png", subx, suby);
		al_set_physfs_file_interface();
		MBITMAP *tmp = m_load_bitmap(nm);
		al_set_standard_file_interface();
#endif
		int xx = tm_used % tm_w;
		int yy = tm_used / tm_w;
		tm_used++;
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		m_set_target_bitmap(partial_tm);
		m_draw_bitmap(tmp, xx*TILE_SIZE, yy*TILE_SIZE, 0);
		MBITMAP *subbmp = m_create_sub_bitmap(partial_tm, xx*TILE_SIZE, yy*TILE_SIZE, TILE_SIZE, TILE_SIZE); // check
		m_destroy_bitmap(tmp);
		al_restore_state(&st);
		image = new Image();
		flags = al_get_new_bitmap_flags();
		image->set(subbmp);
		al_set_new_bitmap_flags(flags);
		frame = new Frame(image, delay);
		animation->addFrame(frame);
		tu.push_back(xx*TILE_SIZE);
		tv.push_back(yy*TILE_SIZE);

		i++;
	}

	ad.tu = tu;
	ad.tv = tv;
#ifdef EDITOR
	anim_info[index] = ad;
#else
	anim_info[newmap[index]] = ad;
#endif
}

std::vector<Tile *> &Area::getTiles(void)
{
	return tiles;
}

bool Area::load(std::string filename)
{
	ALLEGRO_FILE *f = al_fopen(filename.c_str(), "rb");
	if (!f) {
		return false;
	}
	
	sizex = al_fread32le(f);
	sizey = al_fread32le(f);
		
	int ntiles = 0;
	tm_w = 0;
	tm_h = 0;
	tm_used = 0;
	
	// Count number of tiles used
	int n = al_fread32le(f);
	for (int i = 0; i < n; i++) {
		int tileIndex = al_fread32le(f);

		char num[100];
		sprintf(num, "%d", tileIndex);
		std::string number(num);

		XMLData *data = tilemap_data->find(number);
		
		ntiles++;

		if (data) {
			int j = 2;
			for (;;) {
				char name[10];
				sprintf(name, "%d", j);
				XMLData *tmp = data->find(name);
				if (!tmp)
					break;
				ntiles++;
				j++;
			}
		}
	}

	int len = sqrtf(ntiles);
	len++;

	tm_w = len;
	tm_h = len;

	int flgs = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flgs & ~ALLEGRO_NO_PRESERVE_TEXTURE);
	partial_tm = m_create_alpha_bitmap(tm_w*TILE_SIZE, tm_h*TILE_SIZE); // check
	al_set_new_bitmap_flags(flgs);

	char tmp[MAX_AREA_NAME];
	ALLEGRO_PATH *path = al_create_path(filename.c_str());
	strcpy(tmp, al_get_path_basename(path));
	name = std::string(tmp);
	al_destroy_path(path);

	bg = NULL;

	// Can be null, it's ok
	const char *_tmp = getResource("media/%s_bg.png", name.c_str());
	ALLEGRO_FILE *_f = al_fopen(_tmp, "rb");
	if (_f) {
		al_fclose(_f);
		bg = m_load_bitmap(_tmp);
	}

	luaState = 0;

	al_fseek(f, 8, SEEK_SET);
	
#ifndef ALLEGRO_ANDROID
	PHYSFS_addToSearchPath(getResource("tiles.zip"), 1);
#endif

	try {
		// Read animation indexes
		int n = (int)al_fread32le(f);
		for (int i = 0; i < n; i++) {
			int tileIndex = (int)al_fread32le(f);
			loadAnimation(tileIndex, true);
		}
		
		int i;

		for (i = 0; i < sizex*sizey; i++) {
			tiles.push_back(loadTile(f));
		}
	}
	catch (...) {
		if ( sizex < 0 || sizey < 0) {
			for (unsigned int i = 0; i < tiles.size(); i++) {
				delete tiles[i];
			}
			tiles.clear();
		}
	}

#ifndef ALLEGRO_ANDROID
	PHYSFS_removeFromSearchPath(getResource("tiles.zip"));
#endif

	al_fclose(f);

	return true;
}

void Area::followPlayer(bool f)
{
	follow = f;
	if (follow) {
		adjustPan();
	}
}



#ifdef EDITOR


Area::Area(int w, int h) throw (std::bad_alloc) 
{
	name = "";

	sizex = w;
	sizey = h;
    
	try {
		int anims[TILE_LAYERS];
		for (int i = 0; i < TILE_LAYERS; i++)
			anims[i] = -1;

		short tu[TILE_LAYERS] = { 0, };
		short tv[TILE_LAYERS] = { 0, };

		for (int i = 0; i < (int)sizex; i++) {
			Tile *t;
			t = new Tile(anims, false, tu, tv);
			if (!t)
				throw std::bad_alloc();
			tiles.push_back(t);
		}
	}
	catch (std::bad_alloc e) {
		for (unsigned int i = 0; i < tiles.size(); i++) {
			delete tiles[i];
		}
		tiles.clear();
		throw std::bad_alloc();
	}
}
#endif

void Area::setFocus(int x, int y)
{
	focusX = x;
	focusY = y;
}

int Area::getFocusX(void)
{
	if (follow) {
		int pox, poy;
		Object *o = area->findObject(0);
		o->getOffset(&pox, &poy);
		int px = o->getX()*TILE_SIZE+TILE_SIZE/2+pox;
		return px;
	}
	else {
		return focusX;
	}
}


int Area::getFocusY(void)
{
	if (follow) {
		int pox, poy;
		Object *o = area->findObject(0);
		o->getOffset(&pox, &poy);
		int py = o->getY()*TILE_SIZE+TILE_SIZE/2+poy;
		return py;
	}
	else {
		return focusY;
	}
}

int Area::getOriginX(void)
{
	return getFocusX() + area_panned_x;
}

int Area::getOriginY(void)
{
	return getFocusY() + area_panned_y;
}

void Area::setOriginX(int ox)
{
	area_panned_x = 0;
	focusX = ox;
}

void Area::setOriginY(int oy)
{
	area_panned_y = 0;
	focusY = oy;
}

Area::Area(void)
{
	tinting = false;
	memset(&targetTint, 0, sizeof(targetTint));
	follow = true;
	last_player_x = -1;
	last_player_y = -1;
	mem_save_counter = 10000;
	ss_save_counter = 10000;
	down = false;
	panned = false;
	start_mx = 0;
	start_my = 0;
	astar_next_x = -1;
	astar_next_immediate_x = -1;
	area_panned_x = -BW/2;
	area_panned_y = -BH/2;
	center_view = false;
	totalUpdates = 0;
	oldArea = 0;
	in_activate = false;
	dpad_on();
	pan_centered_x = false;
	pan_centered_y = false;
	for (int i = 0; i < MAX_TILES*6*(TILE_LAYERS/2); i++)
		verts[i].z = 0;
	
	update_count = 0;
}



Area::~Area()
{
	if (partial_tm)
		m_destroy_bitmap(partial_tm);

	if (luaState)
		callLua(luaState, "stop", ">");

	if (luaState)
		lua_close(luaState);

	for (unsigned int i = 0; i < tiles.size(); i++) {
		delete tiles[i];
	}
	tiles.clear();

	for (unsigned int i = 0; i < tileAnimations.size(); i++) {
		Animation *a = tileAnimations[i];
		delete a;
	}

	for (unsigned int i = 1; i < objects.size(); i++) {
		delete objects[i];
	}
	objects.clear();

	for (unsigned int i = 0; i < collisions.size(); i++) {
		delete collisions[i];
	}
	collisions.clear();

	if (bg)
		m_destroy_bitmap(bg);
	dpad_off();
}
