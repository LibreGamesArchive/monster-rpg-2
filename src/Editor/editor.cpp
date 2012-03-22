#include "monster2.hpp"


#ifdef __linux__
#include <gtk/gtk.h>
#elseif defined ALLEGRO_WINDOWS
#else // osx
#include <Carbon/Carbon.h>
#endif


//M_KBDSTATE kbdstate;

const int MINI_ROWS = 10;
const int MINI_COLS = 8;

WgtScrollbar *vbarMain;
WgtScrollbar *hbarMain;
WgtScrollbar *vbarMini;
WgtScrollbar *hbarMini;
WgtScrollbar *vbarBig; // for big tile selector
WgtScrollbar *hbarBig; // for big tile selector

const int LAYER_SELECTION_DIALOG_W = 256;
const int LAYER_SELECTION_DIALOG_H = 150;
WgtFrame *layerFrame = NULL;
WgtLabel *layerLabel;
WgtRadioButton *layerRadios[TILE_LAYERS];
WgtButton *layerButton;
bool layerSelectionDialogActive = false;

WgtFrame *contextFrame;
WgtMenuItem *contextInsertRowBefore;
WgtMenuItem *contextInsertRowAfter;
WgtMenuItem *contextInsertColumnBefore;
WgtMenuItem *contextInsertColumnAfter;
WgtMenuItem *contextDeleteRow;
WgtMenuItem *contextDeleteColumn;
WgtMenuItem *contextInsert;
WgtMenuItem *contextDelete;
bool contextMenuOpen = false;
int contextFrameX, contextFrameY;
int ins_del_reason = 0;


int currentLayer = 0;
int currentTile = 0;
int clone_tile_x, clone_tile_y;
int clone_start_x, clone_start_y;

const int TARGET_FPS = 10;
const int TARGET_TICKS_PER_FRAME = 1000 / TARGET_FPS;

const int MAX_UNDO = 16;
std::vector< std::vector<Tile *> * > undoInfo;
std::vector< std::vector<Tile *> * > redoInfo;

int oldLayer = 0;

std::string filename = "";

BrushType myBrush = BRUSH;

WgtMenuBar *menuBar;

bool showSolids = true;

bool selecterActive = false;

bool insertFrameOpen = false;


bool macroRecording = false;
int currentMacro = 0;
int macroStartX = -1;
int macroStartY = -1;

const int NUM_MACROS = 10;

std::vector< std::vector<MacroInfo *> *> macros;


bool macroSelectDialogOpen = false;
const int MACRO_SELECT_W = 200;
const int MACRO_SELECT_H = 200;
WgtFrame *macroFrame = NULL;
WgtRadioButton *macroRadios[NUM_MACROS];
WgtLabel *macroLabel;
WgtButton *macroButton;


bool getSaveName(char *start)
{
#ifdef __linux__
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *dialog;
	bool ret = false;

	dialog = gtk_file_chooser_dialog_new ("Save File",
	      GTK_WINDOW(window),
	      GTK_FILE_CHOOSER_ACTION_SAVE,
	      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		gchar *filename =
			gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		strcpy(start, (char *)filename);
		g_free (filename);
		ret = true;
	}

	gtk_widget_destroy(dialog);
	gtk_widget_destroy(window);

	return ret;
#elseif defined ALLEGRO_WINDOWS
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
#ifdef ALLEGRO4
	ofn.hwndOwner = win_get_window();
#else
	ofn.hwndOwner = al_d3d_get_hwnd();
#endif
	ofn.lpstrFile = start;
	ofn.nMaxFile = 1000;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	bool ret = GetSaveFileName(&ofn);

	_chdir(start_cwd);

	return ret;
#else
	tguiPush();

	tguiSetParent(0);
	
	WgtCrappyWidget *w = new WgtCrappyWidget(0, 0, 200, 16, 1000, std::string(start), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._/", m_map_rgb(0, 0, 0), m_map_rgb(255, 255, 255), big_font);

	tguiAddWidget(w);

	while (1) {
		tguiDraw();
#ifdef ALLEGRO4
		blit(backbuffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
#endif
		m_flip_display();
		TGUIWidget *widget = tguiUpdate();
		if (widget == w) {
			strcpy(start, w->getText().c_str());
			break;
		}
	}

	tguiDeleteWidget(w);

	delete w;

	tguiPop();

	return true;

	/* This doesn't work...
	OSStatus s;
	NavDialogRef out;
	NavDialogCreationOptions dialogOptions;
	NavGetDefaultDialogCreationOptions(&dialogOptions);
	dialogOptions.modality = kWindowModalityAppModal;
	dialogOptions.saveFileName = CFStringCreateWithCString(NULL, start, kCFStringEncodingASCII);
	s = NavCreatePutFileDialog(&dialogOptions, 'area', 'AREA', NULL, NULL, &out);
	if (s == noErr) {
		s = NavDialogRun(out);
		NavUserAction action;
		action = NavDialogGetUserAction(out);
	printf("s=%d\n", (int)s);
		if (s == noErr && action != kNavUserActionCancel) {
			NavReplyRecord rec;
			NavDialogGetReply(out, &rec);
			CFStringGetCString(rec.saveFileName, start, 1000, kCFStringEncodingASCII);
			NavDisposeReply(&rec);
			NavDialogDispose(out);
			return true;
		}
		NavDialogDispose(out);
	}
	else
		printf("s=%d\n", (int)s);
	*/
	return false;
#endif
}


bool getOpenName(char *start)
{
#ifdef __linux__
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *dialog;
	bool ret = false;

	dialog = gtk_file_chooser_dialog_new ("Open File",
	      GTK_WINDOW(window),
	      GTK_FILE_CHOOSER_ACTION_OPEN,
	      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		gchar *filename =
			gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		strcpy(start, (char *)filename);
		g_free (filename);
		ret = true;
	}

	gtk_widget_destroy(dialog);
	gtk_widget_destroy(window);

	return ret;
#elif defined ALLEGRO_WINDOWS
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
#ifdef ALLEGRO4
	ofn.hwndOwner = win_get_window();
#else
	ofn.hwndOwner = al_d3d_get_hwnd();
#endif
	ofn.lpstrFile = start;
	ofn.nMaxFile = 1000;

	bool ret = GetOpenFileName(&ofn);

	_chdir(start_cwd);

	return ret;
#else
	return false;
#endif
}


static void setLayerSelectionDialogPosition(void)
{
	layerFrame->setX(0);
	layerFrame->setY(0);
	tguiCenterWidget(layerLabel, LAYER_SELECTION_DIALOG_W/2, 30);
	for (int i = 0; i < TILE_LAYERS; i++) {
		TGUIWidget *w = layerRadios[i];
		int row = i / 2;
		if (i % 2 == 0) {
			w->setX(10);
		}
		else {
			w->setX(LAYER_SELECTION_DIALOG_W/2+10);
		}
		w->setY(50+row*20);
	}
	tguiCenterWidget(layerButton, LAYER_SELECTION_DIALOG_W/2, LAYER_SELECTION_DIALOG_H-30);
}


void destroyTileVector(std::vector<Tile *> *v)
{
	for (unsigned int i = 0; i < v->size(); i++) {
		Tile *t = (*v)[i];
		delete t;
	}
	v->clear();
	delete v;
}


std::vector<Tile *> *createTileState(void)
{
	std::vector<Tile *> *v = new std::vector<Tile *>();
	
	int w = area->getWidth();
	int h = area->getHeight();

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Tile *t = area->getTile(x, y);
			int animNums[TILE_LAYERS];
			short tu[TILE_LAYERS];
			short tv[TILE_LAYERS];
			bool solid = t->isSolid();
			for (int i = 0; i < TILE_LAYERS; i++) {
				animNums[i] = t->getAnimationNum(i);
				if (animNums[i] >= 0) {
					int n = area->getAnimationNums()[animNums[i]];
					/* FIXME HARDCODED :(( */
					tu[i] = coord_map_x[n];
					tv[i] = coord_map_y[n];
				}
			}
			Tile *newTile = new Tile(animNums, solid, tu, tv);
			v->push_back(newTile);
		}
	}

	return v;
}

void clearRedo(void)
{
	for (unsigned int i = 0; i < redoInfo.size(); i++) {
		std::vector<Tile *> *v = redoInfo[i];
		destroyTileVector(v);
	}
	redoInfo.clear();
}


void clearUndo(void)
{
	for (unsigned int i = 0; i < undoInfo.size(); i++) {
		std::vector<Tile *> *v = undoInfo[i];
		destroyTileVector(v);
	}
	undoInfo.clear();
}


void redo(void)
{
	if (redoInfo.size() <= 0)
		return;
	
	pushUndoInfo();

	std::vector<Tile *> *v = redoInfo[redoInfo.size()-1];
	redoInfo.pop_back();

	for (unsigned int i = 0; i < v->size(); i++) {
		int x = i % area->getWidth();
		int y = i / area->getWidth();
		area->copyTile(x, y, (*v)[i]);
	}
}


void pushRedoInfo(void)
{
	std::vector<Tile *> *v = createTileState();
	redoInfo.push_back(v);
}


void pushUndoInfo(void)
{
	/* Overflow */
	if ((int)undoInfo.size() >= MAX_UNDO) {
		std::vector<Tile *> *d = undoInfo[0];
		destroyTileVector(d);
		undoInfo.erase(undoInfo.begin());
	}

	std::vector<Tile *> *v = createTileState();
	undoInfo.push_back(v);
}


void popUndoInfo(void)
{
	if (undoInfo.size() <= 0)
		return;
	
	pushRedoInfo();

	std::vector<Tile *> *v = undoInfo[undoInfo.size()-1];
	undoInfo.pop_back();

	for (unsigned int i = 0; i < v->size(); i++) {
		int x = i % area->getWidth();
		int y = i / area->getWidth();
		area->copyTile(x, y, (*v)[i]);
	}

	destroyTileVector(v);
}


void brush(int i, int mx, int my)
{
	int x = mx / (TILE_SIZE*2) + (int)hbarMain->getValue();
	int y = (my - 52) / (TILE_SIZE*2) + (int)vbarMain->getValue();

	if (x < 0 || y < 0 ||
			x >= (int)area->getWidth() || y >= (int)area->getHeight())
		return;

	Tile *t = area->getTile(x, y);


	if (myBrush == CLONE) {
		if (clone_start_x < 0) {
			clone_start_x = x;
			clone_start_y = y;
		}
		int tx = clone_tile_x;
		int ty = clone_tile_y;
		tx = tx + (x - clone_start_x);
		ty = ty + (y - clone_start_y);
		currentTile = ty * 32 + tx;
		i = checkTile();
	}

	t->setAnimationNum(currentLayer, i);
	t->setTU(currentLayer, coord_map_x[currentTile]);
	t->setTV(currentLayer, coord_map_y[currentTile]);

	if (macroRecording) {
		if (macroStartX < 0) {
			macroStartX = x;
			macroStartY = y;
		}
		int mx = x - macroStartX;
		int my = y - macroStartY;
		int ml = currentLayer;
		int mt = i;
		std::vector<MacroInfo *> *v = macros[currentMacro];
		if (v->size() > 0) {
			MacroInfo *last = (*v)[v->size()-1];
			if (last->x == mx && last->y == my && last->layer == ml && last->tile == i)
				return;
		}
		MacroInfo *m = new MacroInfo;
		m->x = mx;
		m->y = my;
		m->layer = ml;
		m->tile = mt;
		v->push_back(m);
	}
}


void fill(int was, int i, int tx, int ty)
{
	int w = area->getWidth();
	int h = area->getHeight();
	Tile *c = area->getTile(tx, ty);

	if (c->getAnimationNum(currentLayer) == was) {
		c->setAnimationNum(currentLayer, i);
		if (ty > 0)
			fill(was, i, tx, ty-1);
		if (tx < (w-1))
			fill(was, i, tx+1, ty);
		if (tx > 0)
			fill(was, i, tx-1, ty);
		if (ty < (h-1))
			fill(was, i, tx, ty+1);
	}
}


int checkTile(void)
{
	std::vector<int> &tileAnimNums = area->getAnimationNums();
	std::vector<Animation *> &tileAnims = area->getAnimations();

	for (unsigned int i = 0; i < tileAnimNums.size(); i++) {
		if (tileAnimNums[i] == currentTile)
			return i;
	}
/*


	Frame *frame;
	Animation *animation;
	Image *image;

	int subx = coord_map_x[currentTile];
	int suby = coord_map_y[currentTile];
	MBITMAP *subbmp = al_create_sub_bitmap(tilemap, subx, suby, TILE_SIZE, TILE_SIZE);
	image = new Image();
	int flags = al_get_new_bitmap_flags();
	image->set(subbmp);
	al_set_new_bitmap_flags(flags);

	//blitTile(currentTile, tilemap, tile);
       	//image = new Image();
	//image->load(tile, 0, 0, TILE_SIZE, TILE_SIZE);
	//image->setTransparent(tileTransparent[currentTile]);
	frame = new Frame(image, 0);
	animation = new Animation("hmm", 255);
	animation->addFrame(frame);
	tileAnims.push_back(animation);

	tileAnimNums.push_back(currentTile);
*/

	area->loadAnimation(currentTile, true);

	return tileAnims.size()-1;
}


void insertRow(int where)
{
	int w = area->getWidth();
	int h = area->getHeight();

	area->setHeight(++h);

	std::vector<Tile *> newTiles;

	for (int i = 0; i < w; i++) {
		int animNums[TILE_LAYERS];
		short tu[TILE_LAYERS] = { 0, };
		short tv[TILE_LAYERS] = { 0, };
		for (int j = 0; j < TILE_LAYERS; j++)
			animNums[j] = -1;
		Tile *t = new Tile(animNums, false, tu, tv);
		newTiles.push_back(t);
	}

	std::vector<Tile *> &tiles = area->getTiles();

	tiles.insert(tiles.begin()+(w*where),
		newTiles.begin(), newTiles.end());
}


void insertColumn(int where)
{
	int w = area->getWidth();
	int h = area->getHeight();

	std::vector<Tile *> &tiles = area->getTiles();
	std::vector<Tile *>::iterator it = tiles.begin() + where;

	for (int i = 0; i < h; i++) {
		int animNums[TILE_LAYERS];
		short tu[TILE_LAYERS] = { 0, };
		short tv[TILE_LAYERS] = { 0, };
		for (int j = 0; j < TILE_LAYERS; j++)
			animNums[j] = -1;
		Tile *t = new Tile(animNums, false, tu, tv);
		it = tiles.insert(it, t);
		it += w+1;
	}

	area->setWidth(++w);
}


void deleteRow(int r)
{
	int w = area->getWidth();
	int h = area->getHeight();

	std::vector<Tile *> &tiles = area->getTiles();
	std::vector<Tile *>::iterator it = tiles.begin() + w*r;

	for (int i = 0; i < w; i++) {
		Tile *t = *it;
		delete t;
		it = tiles.erase(it);
	}

	area->setHeight(--h);
}


void deleteColumn(int c)
{
	int w = area->getWidth();
	int h = area->getHeight();

	std::vector<Tile *> &tiles = area->getTiles();
	std::vector<Tile *>::iterator it = tiles.begin() + c;

	for (int i = 0; i < h; i++) {
		Tile *t = *it;
		delete t;
		it = tiles.erase(it);
		it += (w-1);
	}

	area->setWidth(--w);
}


void resetVbarParams(void)
{
	int extra;
	int h = area->getHeight();

	if (h <= HT) {
		extra = 0;
	}
	else {
		extra = h-HT;
	}

	vbarMain->setParams(0, extra, HT);
}


void resetHbarParams(void)
{
	int extra;
	int w = area->getWidth();

	if (w <= WT) {
		extra = 0;
	}
	else {
		extra = w-WT;
	}

	hbarMain->setParams(0, extra, WT);
}


void resetTileBarParams(void)
{
	int tr = m_get_bitmap_height(tilemap) / TILE_SIZE;
	int tc = m_get_bitmap_width(tilemap) / TILE_SIZE;

	vbarMini->setParams(0, tr-MINI_ROWS, MINI_ROWS);
	hbarMini->setParams(0, tc-MINI_COLS, MINI_COLS);

	vbarMini->setValue(0);
	hbarMini->setValue(0);
	
	int numMiniTileSelectorRows = numTiles / (512 / 16);
	int numExtraRows = numMiniTileSelectorRows - (256 / 16);

	vbarBig->setParams(0, numExtraRows, (256 / 16));

}


int main(int argc, char *argv[])
{
	debug_message("Initializing...\n");

#ifdef __linux__
	gtk_init(&argc, &argv);
#endif
	int n;
	if ((n = check_arg(argc, argv, "-debug")) != -1) {
		config.setShowDebugMessages(true);
	}
	

	if (!init(&argc, &argv)) {
		debug_message("Initialization failed.\n");
		return 1;
	}

	big_font = al_load_font("data/big_font.png", 0, 0);

	for (int i = 0; i < NUM_MACROS; i++) {
		std::vector<MacroInfo *> *v = new std::vector<MacroInfo *>();
		macros.push_back(v);
	}

#ifdef ALLEGRO4
	backbuffer = create_bitmap_ex(16, SCREEN_W, SCREEN_H);
#endif

	debug_message("Initialization successful.\n");

#ifdef ALLEGRO4
	show_mouse(screen);
#else
	al_show_mouse_cursor(display);
#endif

	if (argc > 1) {
		filename = std::string(argv[1]);
		area = new Area();
		area->followPlayer(false);
		area->load(filename);
	}
	else {
		area = new Area(40, 30);
		area->followPlayer(false);
	}

	area->setOriginX(0);
	area->setOriginY(0);

	MCOLOR tab_color, dark_grey;

	tab_color = m_map_rgb(255, 255, 255);
	dark_grey = m_map_rgb(180, 190, 200);

	vbarMain = new WgtScrollbar(true, BW*2, 52, 16, BH*2,
			0, 0, 0, tab_color, dark_grey);
	hbarMain = new WgtScrollbar(false, 0, 52+BH*2, BW*2, 16,
			0, 0, 0, tab_color, dark_grey);
	resetVbarParams();
	resetHbarParams();

	vbarMini = new WgtScrollbar(true, BW*2+16+(MINI_COLS*TILE_SIZE*2), 52+TILE_SIZE*2,
			16, MINI_ROWS*TILE_SIZE*2,
			0, 0, 0, tab_color, dark_grey);

	hbarMini = new WgtScrollbar(false, BW*2+16, 52+((MINI_ROWS+1)*TILE_SIZE*2),
			TILE_SIZE*2*MINI_COLS, 16,
			0, 0, 0, tab_color, dark_grey);

	vbarBig = new WgtScrollbar(true, 0, 0, 16, 256,
			0, 0, 0, tab_color, dark_grey);
	hbarBig = new WgtScrollbar(false, 0, 0, 512, 16,
			0, 0, 0, tab_color, dark_grey);
	
	resetTileBarParams();

	tguiAddWidget(vbarMain);
	tguiAddWidget(hbarMain);
	tguiAddWidget(vbarMini);
	tguiAddWidget(hbarMini);

	WgtAreaEditor *areaWidget = new WgtAreaEditor(0, 52, BW*2, BH*2);
	WgtTileSelector *tileSelector =
		new WgtTileSelector(BW*2+16, 52+TILE_SIZE*2,
		MINI_COLS*(TILE_SIZE*2), MINI_ROWS*TILE_SIZE*2, 2, vbarMini, hbarMini);

	WgtFrame *bigSelectorFrame = new WgtFrame(0, 0, 600, 400, true,
		true, dark_grey, vbarBig);
	WgtTileSelector *bigSelector = new WgtTileSelector(0, 0, 512, 256,
		1, vbarBig, hbarBig);
	
	MCOLOR grey, blue;
	grey = m_map_rgb(255, 255, 255);
	blue = m_map_rgb(40, 80, 100);

	Menu *fileMenu = new Menu("File");
	Menu *editMenu = new Menu("Edit");
	Menu *toolsMenu = new Menu("Tools");
	Menu *layersMenu = new Menu("Layers");
	Menu *tilesMenu = new Menu("Tiles");
	Menu *macroMenu = new Menu("Macro");

	WgtMenuItem *fileNew = new WgtMenuItem(0, 17, 200, 16, "New...",
		black, grey, white, blue, big_font);
	WgtMenuItem *fileOpen = new WgtMenuItem(0, 33, 200, 16, "Open...",
		black, grey, white, blue, big_font);
	WgtMenuItem *fileSave = new WgtMenuItem(0, 49, 200, 16, "Save",
		black, grey, white, blue, big_font);
	WgtMenuItem *fileSaveAs = new WgtMenuItem(0, 65, 200, 16, "Save as...",
		black, grey, white, blue, big_font);
	WgtMenuItem *fileExit = new WgtMenuItem(0, 81, 200, 16, "Exit",
		black, grey, white, blue, big_font);
#ifdef ALLEGRO4
	WgtMenuItem *fileSaveBmp = new WgtMenuItem(0, 97, 200, 16, "Save bmp",
		black, grey, white, blue, big_font);
#endif

	WgtMenuItem *editUndo = new WgtMenuItem(0, 17, 200, 16, "Undo",
		black, grey, white, blue, big_font);
	WgtMenuItem *editRedo = new WgtMenuItem(0, 33, 200, 16, "Redo",
		black, grey, white, blue, big_font);

	WgtMenuItem *toolsBrush = new WgtMenuItem(0, 17, 200, 16, "Brush",
		black, grey, white, blue, big_font);
	WgtMenuItem *toolsFill = new WgtMenuItem(0, 33, 200, 16, "Fill",
		black, grey, white, blue, big_font);
	WgtMenuItem *toolsSolids = new WgtMenuItem(0, 49, 200, 16, "Solids",
		black, grey, white, blue, big_font);
	WgtMenuItem *toolsClear = new WgtMenuItem(0, 65, 200, 16, "Clear",
		black, grey, white, blue, big_font);
	WgtMenuItem *toolsMacro = new WgtMenuItem(0, 81, 200, 16, "Macro",
		black, grey, white, blue, big_font);
	WgtMenuItem *toolsClone = new WgtMenuItem(0, 97, 200, 16, "Clone",
		black, grey, white, blue, big_font);

	WgtMenuItem *layersSelectActive = new WgtMenuItem(0, 17, 200, 16, "Select Active...",
		black, grey, white, blue, big_font);

	WgtMenuItem *tilesReload = new WgtMenuItem(0, 17, 200, 16, "Reload",
		black, grey, white, blue, big_font);
	WgtMenuItem *tilesToggleSolids = new WgtMenuItem(0, 33, 200, 16, "Toggle Solids",
		black, grey, white, blue, big_font);
	WgtMenuItem *tilesSelecter = new WgtMenuItem(0, 49, 200, 16, "Selecter",
		black, grey, white, blue, big_font);
	WgtMenuItem *tilesFlipH = new WgtMenuItem(0, 65, 200, 16, "Flip Horiz.",
		black, grey, white, blue, big_font);


	WgtMenuItem *macroSelect = new WgtMenuItem(0, 17, 200, 16, "Select Active...",
		black, grey, white, blue, big_font);
	WgtMenuItem *macroRecord = new WgtMenuItem(0, 33, 200, 16, "Record",
		black, grey, white, blue, big_font);
	WgtMenuItem *macroEnd = new WgtMenuItem(0, 49, 200, 16, "End",
		black, grey, white, blue, big_font);


	fileMenu->addItem(fileNew);
	fileMenu->addItem(fileOpen);
	fileMenu->addItem(fileSave);
	fileMenu->addItem(fileSaveAs);
	fileMenu->addItem(fileExit);
#ifdef ALLEGRO4
	fileMenu->addItem(fileSaveBmp);
#endif

	editMenu->addItem(editUndo);
	editMenu->addItem(editRedo);

	toolsMenu->addItem(toolsBrush);
	toolsMenu->addItem(toolsFill);
	toolsMenu->addItem(toolsSolids);
	toolsMenu->addItem(toolsClear);
	toolsMenu->addItem(toolsMacro);
	toolsMenu->addItem(toolsClone);

	layersMenu->addItem(layersSelectActive);

	tilesMenu->addItem(tilesReload);
	tilesMenu->addItem(tilesToggleSolids);
	tilesMenu->addItem(tilesSelecter);
	tilesMenu->addItem(tilesFlipH);

	macroMenu->addItem(macroSelect);
	macroMenu->addItem(macroRecord);
	macroMenu->addItem(macroEnd);

	std::vector<Menu *> menus;
	menus.push_back(fileMenu);
	menus.push_back(editMenu);
	menus.push_back(toolsMenu);
	menus.push_back(layersMenu);
	menus.push_back(tilesMenu);
	menus.push_back(macroMenu);

	std::vector<int> menuBarHotkeys;

	menuBarHotkeys.push_back(tguiCreateHotkey(TGUI_KEYFLAG_ALT,
		ALLEGRO_KEY_F));
	menuBarHotkeys.push_back(tguiCreateHotkey(TGUI_KEYFLAG_ALT,
		ALLEGRO_KEY_E));
	menuBarHotkeys.push_back(tguiCreateHotkey(TGUI_KEYFLAG_ALT,
		ALLEGRO_KEY_T));
	menuBarHotkeys.push_back(tguiCreateHotkey(TGUI_KEYFLAG_ALT,
		ALLEGRO_KEY_L));
	menuBarHotkeys.push_back(tguiCreateHotkey(TGUI_KEYFLAG_ALT,
		ALLEGRO_KEY_I));

	menuBar = new WgtMenuBar(0, 0, 800, 16, menus, black,
		grey, big_font, &menuBarHotkeys);

	tguiAddWidget(menuBar);

	
	layerButton = new WgtButton(0, 0, -1, -1, "Ok",
			black, grey, big_font);
	layerFrame = new WgtFrame(0, 0, LAYER_SELECTION_DIALOG_W,
			LAYER_SELECTION_DIALOG_H, true, true, dark_grey,
			layerButton);
	layerLabel = new  WgtLabel(0, 0, "Choose Active Layer:",
			white, black, big_font);
	for (int i = 0; i < TILE_LAYERS; i++) {
		char label[10];
		sprintf(label, "%d", i+1);
		const std::string labelS = std::string(label);
		layerRadios[i] = new WgtRadioButton(0, 0, labelS, white,
			black, big_font,
			i == 0 ? NULL : layerRadios[i-1]);
	}
	layerRadios[0]->setChecked(true);

	
	contextInsertRowBefore = new WgtMenuItem(20, 32, 160, 16,
		"Ins. Row Before", black, grey, white, blue,
		big_font);
	contextFrame = new WgtFrame(0, 0, CONTEXT_MENU_W, CONTEXT_MENU_H,
		true, true, dark_grey, contextInsertRowBefore);
	contextInsertRowAfter = new WgtMenuItem(20, 48, 160, 16,
		"Ins. Row After", black, grey, white, blue,
		big_font);
	contextInsertColumnBefore = new WgtMenuItem(20, 64, 160, 16,
		"Ins. Col. Before", black, grey, white, blue,
		big_font);
	contextInsertColumnAfter = new WgtMenuItem(20, 80, 160, 16,
		"Ins. Col. After", black, grey, white, blue,
		big_font);
	contextDeleteRow = new WgtMenuItem(20, 96, 160, 16,
		"Delete Row", black, grey, white, blue,
		big_font);
	contextDeleteColumn = new WgtMenuItem(20, 112, 160, 16,
		"Delete Column", black, grey, white, blue,
		big_font);
	contextInsert = new WgtMenuItem(20, 128, 160, 16,
	 	"Insert...", black, grey, white, blue, big_font);
	contextDelete = new WgtMenuItem(20, 144, 160, 16,
	 	"Delete...", black, grey, white, blue, big_font);


	WgtButton *newButton = new WgtButton(0, 0, -1, -1,
		"Ok", black, grey, big_font);
	WgtFrame *newFrame = new WgtFrame(0, 0, 200, 135, true, true,
		dark_grey, newButton);
	WgtLabel *newLabel = new WgtLabel(0, 0, "New area:", white,
		black, big_font);
	tguiCenterWidget(newLabel, 100, 30);
	WgtLabel *newWidthLabel = new WgtLabel(10, 45, "Width:",
		white, black, big_font);
	WgtLabel *newHeightLabel = new WgtLabel(10, 65, "Height:",
		white, black, big_font);
	WgtTextField *widthGetter = new WgtTextField(130, 45, 60, 20,
		4, "40", "0123456789", black, white, big_font);
	WgtTextField *heightGetter = new WgtTextField(130, 65, 60, 20,
		4, "30", "0123456789", black, white, big_font);
	tguiCenterWidget(newButton, 100, 100);

	WgtButton *verifyNo = new WgtButton(0, 0, -1, -1, "No",
		black, grey, big_font);
	tguiCenterWidget(verifyNo, 200/4*3, 70);
	WgtFrame *verifyFrame = new WgtFrame(0, 0, 200, 100, true, true,
		dark_grey, verifyNo);
	WgtLabel *verifyLabel = new WgtLabel(0, 0, "Really exit?", white,
		black, big_font);
	tguiCenterWidget(verifyLabel, 100, 32);
	WgtButton *verifyYes = new WgtButton(0, 0, -1, -1, "Yes",
		black, grey, big_font);
	tguiCenterWidget(verifyYes, 200/4, 70);

	
	WgtLabel *insertLabel = new WgtLabel(0, 0, "Insert/Delete...", white, black,
		big_font);
	tguiCenterWidget(insertLabel, 100, 25);
	WgtRadioButton *insertRowBefore = new WgtRadioButton(10, 50, "Row before",
		white, black, big_font, NULL);
	WgtRadioButton *insertRowAfter = new WgtRadioButton(10, 70, "Row after",
		white, black, big_font, insertRowBefore);
	WgtRadioButton *insertColumnBefore = new WgtRadioButton(10, 90, "Column before",
		white, black, big_font, insertRowAfter);
	WgtRadioButton *insertColumnAfter = new WgtRadioButton(10, 110, "Column after",
		white, black, big_font, insertColumnBefore);
	WgtLabel *insertCountLabel = new WgtLabel(10, 130, "Count:", white,
		black, big_font);
	WgtTextField *insertCount = new WgtTextField(70, 130, 60, 20,
		4, "0", "0123456789", black, white, big_font);
	WgtButton *insertOk = new WgtButton(0, 0, -1, -1, "Ok",
		black, grey, big_font);
	tguiCenterWidget(insertOk, 100, 175);
	WgtFrame *insertFrame = new WgtFrame(0, 0, 200, 220, true, true,
		dark_grey, insertRowBefore);

	
	macroButton = new WgtButton(0, 0, -1, -1, "Ok",
		black, grey, big_font);
	tguiCenterWidget(macroButton, MACRO_SELECT_W/2, MACRO_SELECT_H-20);
	macroFrame = new WgtFrame(0, 0, MACRO_SELECT_W, MACRO_SELECT_H,
		true, true, dark_grey, macroButton);
	macroLabel = new WgtLabel(0, 0, "Select macro...", white,
		black, big_font);
	tguiCenterWidget(macroLabel, MACRO_SELECT_W/2, 30);
	for (int i = 0; i < NUM_MACROS; i++) {
		char label[10];
		sprintf(label, "%d", i+1);
		const std::string labelS = std::string(label);
		int x, y;
		if (i % 2) {
			x = MACRO_SELECT_W/2+10;
		}
		else {
			x = 10;
		}
		y = (i/2) * 20 + 45;
		macroRadios[i] = new WgtRadioButton(x, y, labelS, white,
			black, big_font,
			i == 0 ? NULL : macroRadios[i-1]);
	}
	layerRadios[0]->setChecked(true);


	tguiAddWidget(areaWidget);
	tguiAddWidget(tileSelector);


	MBITMAP *brushIcons[6];
	brushIcons[0] = m_load_bitmap(getResource("brush.png"));
	brushIcons[1] = m_load_bitmap(getResource("bucket.png"));
	brushIcons[2] = m_load_bitmap(getResource("solids.png"));
	brushIcons[3] = m_load_bitmap(getResource("clear.png"));
	brushIcons[4] = m_load_bitmap(getResource("macro.png"));
	brushIcons[5] = m_load_bitmap(getResource("clone.png"));

	for (;;) {
		long start = tguiCurrentTimeMillis();
		m_set_target_bitmap(buffer);
		MCOLOR color = m_map_rgb(0, 0, 0);
		m_clear(color);
		/* draw the Area */
		m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
		area->draw();
#ifdef ALLEGRO4
		m_set_target_bitmap(backbuffer);
#else
		m_set_target_bitmap(al_get_backbuffer(display));
#endif
		color = m_map_rgb(255, 0, 255);
		m_clear(color);
		m_set_blender(M_ONE, M_ZERO, white);
		m_draw_scaled_bitmap(buffer, 0, 0, BW, BH, 0, 52,
			BW*2, BH*2, 0);
		if ((int)area->getWidth() < WT || (int)area->getHeight() < HT) {
			m_draw_rectangle(-1, 52,
				area->getWidth()*(TILE_SIZE*2),
				52+area->getHeight()*(TILE_SIZE*2),
				white, 0);
		}
		m_set_blender(M_ONE, M_INVERSE_ALPHA, white);
		// Draw the current tile
		blitTile(currentTile, tilemap, tile);
		m_draw_scaled_bitmap(tile, 0, 0, TILE_SIZE, TILE_SIZE,
				BW*2+16, 52, TILE_SIZE*2, TILE_SIZE*2, 0);
		// Draw the GUI
		tguiDraw();
		// Draw the brush icon
		m_draw_bitmap(brushIcons[myBrush], 800-32, 16, 0);
		// Draw the coordinates of the tile under the mouse
		char text[100];
		int mx = tguiGetMouseX();
		int my = tguiGetMouseY();
		int tx, ty;
		if (mx > (BW*2) || my < 52 || my > (52+BH*2))
			tx = ty = 0;
		else {
			tx = mx / (TILE_SIZE*2) + (int)hbarMain->getValue();
			ty = (my - 52) / (TILE_SIZE*2) + (int)vbarMain->getValue();
		}
		if (tx >= 0 && ty >= 0 && tx < (int)area->getWidth() && ty < (int)area->getHeight())
			sprintf(text, "%d,%d", tx, ty);
		else
			sprintf(text, "n/a");
		m_textout(big_font, text, 0, 480, white);
#ifdef ALLEGRO4
		blit(backbuffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
#endif
		m_flip_display();
		for (;;) {
			// Logic
			TGUIWidget *widget = tguiUpdate();
			if (widget == fileExit) {
				menuBar->hideMenus();
				tguiSetParent(NULL);
				tguiAddWidget(verifyFrame);
				tguiSetParent(verifyFrame);
				tguiAddWidget(verifyLabel);
				tguiAddWidget(verifyYes);
				tguiAddWidget(verifyNo);
				tguiTranslateWidget(verifyFrame,
					-verifyFrame->getX(),
					-verifyFrame->getY());
				tguiTranslateWidget(verifyFrame,
					(800-verifyFrame->getWidth())/2,
					(600-verifyFrame->getHeight())/2);
				tguiSetFocus(verifyNo);
			}
			else if (widget == layersSelectActive) {
				menuBar->hideMenus();
				if (!layerSelectionDialogActive) {
					setLayerSelectionDialogPosition();
					tguiSetParent(NULL);
					tguiAddWidget(layerFrame);
					tguiSetParent(layerFrame);
					tguiAddWidget(layerLabel);
					for (int i = 0; i < TILE_LAYERS; i++) {
						tguiAddWidget(layerRadios[i]);
					}
					tguiAddWidget(layerButton);
					tguiSetParent(NULL);
					tguiSetFocus(layerButton);
					tguiTranslateWidget(layerFrame, (800-LAYER_SELECTION_DIALOG_W)/2, (600-LAYER_SELECTION_DIALOG_H)/2);
					layerSelectionDialogActive = true;
					oldLayer = currentLayer;
				}
			}
			else if (widget && widget == layerButton) {
				tguiDeleteWidget(layerFrame);
				for (int i = 0; i < TILE_LAYERS; i++) {
					WgtRadioButton *w = layerRadios[i];
					if (w->isChecked()) {
						currentLayer = i;
						break;
					}
				}
				layerSelectionDialogActive = false;
			}
			else if (widget == editUndo) {
				popUndoInfo();
			}
			else if (widget == editRedo) {
				redo();
			}
			else if (widget == layerFrame) {
				tguiDeleteWidget(layerFrame);
				currentLayer = oldLayer;
				layerRadios[currentLayer]->setChecked(true);
				layerSelectionDialogActive = false;
			}
			else if (widget == contextInsertRowBefore) {
				int y = (contextFrameY - 52) / (TILE_SIZE*2)
					+ (int)vbarMain->getValue();
				tguiDeleteWidget(contextFrame);
				contextMenuOpen = false;
				insertRow(y);
				resetVbarParams();
				clearUndo();
				clearRedo();
			}
			else if (widget == contextInsertRowAfter) {
				int y = (contextFrameY - 52) / (TILE_SIZE*2)
					+ (int)vbarMain->getValue();
				tguiDeleteWidget(contextFrame);
				contextMenuOpen = false;
				insertRow(y+1);
				resetVbarParams();
				clearUndo();
				clearRedo();
			}
			else if (widget == contextInsertColumnBefore) {
				int x = (contextFrameX / (TILE_SIZE*2)) +
					(int)hbarMain->getValue();
				tguiDeleteWidget(contextFrame);
				contextMenuOpen = false;
				insertColumn(x);
				resetHbarParams();
				clearUndo();
				clearRedo();
			}
			else if (widget == contextInsertColumnAfter) {
				int x = (contextFrameX / (TILE_SIZE*2)) +
					(int)hbarMain->getValue();
				tguiDeleteWidget(contextFrame);
				contextMenuOpen = false;
				insertColumn(x+1);
				resetHbarParams();
				clearUndo();
				clearRedo();
			}
			else if (widget == contextDeleteRow) {
				int y = (contextFrameY - 52) / (TILE_SIZE*2)
					+ (int)vbarMain->getValue();
				tguiDeleteWidget(contextFrame);
				contextMenuOpen = false;
				deleteRow(y);
				resetVbarParams();
				vbarMain->setValue(0);
				clearUndo();
				clearRedo();
			}
			else if (widget == contextDeleteColumn) {
				int x = (contextFrameX / (TILE_SIZE*2)) +
					(int)hbarMain->getValue();
				tguiDeleteWidget(contextFrame);
				contextMenuOpen = false;
				deleteColumn(x);
				resetHbarParams();
				vbarMain->setValue(0);
				clearUndo();
				clearRedo();
			}
			else if (widget == contextFrame) {
				tguiDeleteWidget(contextFrame);
				contextMenuOpen = false;
			}
			if (widget == contextInsert) {
				tguiDeleteWidget(contextFrame);
				contextMenuOpen = false;
				if (!insertFrameOpen) {
					ins_del_reason = 0;
					tguiSetParent(NULL);
					tguiAddWidget(insertFrame);
					tguiSetParent(insertFrame);
					tguiAddWidget(insertLabel);
					tguiAddWidget(insertRowBefore);
					tguiAddWidget(insertRowAfter);
					tguiAddWidget(insertColumnBefore);
					tguiAddWidget(insertColumnAfter);
					tguiAddWidget(insertCountLabel);
					tguiAddWidget(insertCount);
					tguiAddWidget(insertOk);
					tguiTranslateWidget(insertFrame,
						-insertFrame->getX(),
						-insertFrame->getY());
					tguiTranslateWidget(insertFrame,
						(800-insertFrame->getWidth())/2,
						(600-insertFrame->getHeight())/2);
					tguiSetFocus(insertRowBefore);
					insertFrameOpen = true;
				}
			}
			else if (widget == contextDelete) {
				tguiDeleteWidget(contextFrame);
				contextMenuOpen = false;
				if (!insertFrameOpen) {
					ins_del_reason = 1;
					tguiSetParent(NULL);
					tguiAddWidget(insertFrame);
					tguiSetParent(insertFrame);
					tguiAddWidget(insertLabel);
					tguiAddWidget(insertRowBefore);
					tguiAddWidget(insertRowAfter);
					tguiAddWidget(insertColumnBefore);
					tguiAddWidget(insertColumnAfter);
					tguiAddWidget(insertCountLabel);
					tguiAddWidget(insertCount);
					tguiAddWidget(insertOk);
					tguiTranslateWidget(insertFrame,
						-insertFrame->getX(),
						-insertFrame->getY());
					tguiTranslateWidget(insertFrame,
						(800-insertFrame->getWidth())/2,
						(600-insertFrame->getHeight())/2);
					tguiSetFocus(insertRowBefore);
					insertFrameOpen = true;
				}
			}
			else if (widget == insertOk) {
				int x = (contextFrameX / (TILE_SIZE*2)) +
					(int)hbarMain->getValue();
				int y = (contextFrameY - 52) / (TILE_SIZE*2)
					+ (int)vbarMain->getValue();
				int count = atoi(insertCount->getText().c_str());
				for (int i = 0; i < count; i++) {
					if (ins_del_reason == 0) {
						if (insertRowBefore->isChecked()) {
							insertRow(y);
						}
						else if (insertRowAfter->isChecked()) {
							insertRow(y+1);
						}
						else if (insertColumnBefore->isChecked()) {
							insertColumn(x);
						}
						else if (insertColumnAfter->isChecked()) {
							insertColumn(x+1);
						}
					}
					else {
						if (insertRowBefore->isChecked()) {
							deleteRow(y);
						}
						else if (insertRowAfter->isChecked()) {
							deleteRow(y+1);
						}
						else if (insertColumnBefore->isChecked()) {
							deleteColumn(x);
						}
						else if (insertColumnAfter->isChecked()) {
							deleteColumn(x+1);
						}
					}
				}
				tguiDeleteWidget(insertFrame);
				insertFrameOpen = false;
				resetHbarParams();
				resetVbarParams();
				vbarMain->setValue(0);
				hbarMain->setValue(0);
				clearUndo();
				clearRedo();
			}
			else if (widget == insertFrame) {
				tguiDeleteWidget(insertFrame);
				insertFrameOpen = false;
			}
			else if (widget == fileSaveAs) {
				menuBar->hideMenus();
				char s[1000];
				s[0] = 0;
				if (getSaveName(s)) {
					filename = std::string(s);
					area->save(filename);
				}
			}
			else if (widget == fileOpen) {
				menuBar->hideMenus();
				char s[1000];
				s[0] = 0;
				if (getOpenName(s)) {
					delete area;
					filename = std::string(s);
					area = new Area();
					area->followPlayer(false);
					area->load(filename);
					resetVbarParams();
					resetHbarParams();
					vbarMain->setValue(0);
					hbarMain->setValue(0);
				}
			}
			else if (widget == fileSave) {
				menuBar->hideMenus();
				if (filename != "") {
					area->save(filename);
				}
				else {
					char s[1000];
					s[0] = 0;
					if (getSaveName(s)) {
						filename = std::string(s);
						area->save(filename);
					}
				}
			}
#ifdef ALLEGRO4
			else if (widget == fileSaveBmp) {
				char s[1000];
				s[0] = 0;
				if (getSaveName(s)) {
					std::string str = std::string(s);
					area->saveBmp(str);
				}
			}
#endif
			else if (widget == fileNew) {
				menuBar->hideMenus();
				tguiSetParent(0);
				tguiAddWidget(newFrame);
				tguiSetParent(newFrame);
				tguiAddWidget(newLabel);
				tguiAddWidget(newWidthLabel);
				tguiAddWidget(newHeightLabel);
				tguiAddWidget(widthGetter);
				tguiAddWidget(heightGetter);
				tguiAddWidget(newButton);
				tguiSetFocus(newButton);
				tguiTranslateWidget(newFrame,
					-newFrame->getX(),
					-newFrame->getY());
				tguiTranslateWidget(newFrame,
					(800-newFrame->getWidth())/2,
					(600-newFrame->getHeight())/2);
			}
			else if (widget == toolsBrush) {
				menuBar->hideMenus();
				myBrush = BRUSH;
			}
			else if (widget == toolsFill) {
				menuBar->hideMenus();
				myBrush = FILL;
			}
			else if (widget == toolsSolids) {
				menuBar->hideMenus();
				myBrush = SOLIDS;
			}
			else if (widget == toolsClear) {
				menuBar->hideMenus();
				myBrush = CLEAR;
			}
			else if (widget == toolsMacro) {
				menuBar->hideMenus();
				myBrush = MACRO;
			}
			else if (widget == toolsClone) {
				menuBar->hideMenus();
				myBrush = CLONE;
				clone_tile_x = currentTile % 32;
				clone_tile_y = currentTile / 32;
				clone_start_x = -1;
				clone_start_y = -1;
			}
			else if (widget == tilesReload) {
				menuBar->hideMenus();
				destroyTilemap();
				loadTilemap();
				area->reloadAnimations();
				resetTileBarParams();
			}
			else if (widget == tilesFlipH) {
				menuBar->hideMenus();
				for (int x = 0; x < (int)area->getWidth()/2; x++) {
					for (int y = 0; y < (int)area->getHeight(); y++) {
						Tile *tmp = area->getTile(x, y);
						area->setTile(x, y, area->getTile(area->getWidth()-x-1, y));
						area->setTile(area->getWidth()-x-1, y, tmp);
					}
				}
			}
			else if (widget == newButton) {
				tguiDeleteWidget(newFrame);
				std::string wS = widthGetter->getText();
				std::string hS = heightGetter->getText();
				int w = atoi(wS.c_str());
				int h = atoi(hS.c_str());
				if (w > 0 && h > 0 && w < 9999 && h < 9999) {
					delete area;
					area = new Area(w, h);
					area->followPlayer(false);
					filename[0] = 0;
					resetVbarParams();
					resetHbarParams();
					vbarMain->setValue(0);
					hbarMain->setValue(0);
				}
			}
			else if (widget == newFrame) {
				tguiDeleteWidget(newFrame);
			}
			else if (widget == verifyNo || widget == verifyFrame) {
				tguiDeleteWidget(verifyFrame);
			}
			else if (widget == verifyYes) {
				goto done;
			}
			else if (widget == tilesToggleSolids) {
				showSolids = !showSolids;
			}
			else if (widget == tilesSelecter) {
				if (!selecterActive) {
					selecterActive = true;
					bigSelectorFrame->setX(0);
					bigSelectorFrame->setY(0);
					tguiCenterWidget(bigSelector, 300, 200);
					vbarBig->setX(bigSelector->getX()+512);
					vbarBig->setY(bigSelector->getY());
					hbarBig->setX(bigSelector->getX());
					hbarBig->setY(bigSelector->getY()+bigSelector->getHeight());
					printf("%d %d %d %d\n", bigSelector->getX(), bigSelector->getY(), vbarBig->getX(), vbarBig->getY());
					tguiSetParent(0);
					tguiAddWidget(bigSelectorFrame);
					tguiSetParent(bigSelectorFrame);
					tguiAddWidget(bigSelector);
					tguiAddWidget(vbarBig);
					tguiTranslateWidget(bigSelectorFrame,
						(800-600)/2, (600-400)/2);
				}
			}
			else if (widget == bigSelectorFrame) {
				tguiDeleteWidget(bigSelectorFrame);
				selecterActive = false;
			}
			else if (widget == macroSelect && !macroSelectDialogOpen) {
				macroSelectDialogOpen = true;
				menuBar->hideMenus();
				tguiSetParent(0);
				tguiAddWidget(macroFrame);
				tguiSetParent(macroFrame);
				tguiAddWidget(macroLabel);
				for (int i = 0; i < NUM_MACROS; i++) {
					tguiAddWidget(macroRadios[i]);
				}
				tguiAddWidget(macroButton);
				tguiSetFocus(macroRadios[0]);
				tguiTranslateWidget(macroFrame,
					-macroFrame->getX(),
					-macroFrame->getY());
				tguiTranslateWidget(macroFrame,
					(800-macroFrame->getWidth())/2,
					(600-macroFrame->getHeight())/2);
			}
			else if (widget == macroFrame) {
				tguiDeleteWidget(macroFrame);
				macroSelectDialogOpen = false;
			}
			else if (widget == macroButton) {
				tguiDeleteWidget(macroFrame);
				macroSelectDialogOpen = false;
				for (int i = 0; i < NUM_MACROS; i++) {
					if (macroRadios[i]->isChecked()) {
						currentMacro = i;
						break;
					}
				}
			}
			else if (widget == macroRecord) {
				menuBar->hideMenus();
				macroStartX = macroStartY = -1;
				std::vector<MacroInfo *> *v = macros[currentMacro];
				for (unsigned int i = 0; i < v->size(); i++) {
					MacroInfo *m = (*v)[i];
					delete m;
				}
				v->clear();
				macroRecording = true;
			}
			else if (widget == macroEnd) {
				menuBar->hideMenus();
				macroRecording = false;
				std::vector<MacroInfo *> *v = macros[currentMacro];
				printf("recorded:");
				for (unsigned int i = 0; i < v->size(); i++) {
					MacroInfo *m = (*v)[i];
					printf("\t%d %d %d %d\n", m->x, m->y, m->layer, m->tile);
				}
			}
			//al_get_keyboard_state(&kbdstate);
			// FIXME:
			/*
			if (al_key_down(&kbdstate, ALLEGRO_KEY_ALT) &&
					al_key_down(&kbdstate, ALLEGRO_KEY_F)) {
				tguiSetFocus(fileNew);
			}
			*/
			// Handle area scrolling
			int oy = (int)vbarMain->getValue() * 16;
			int ox = (int)hbarMain->getValue() * 16;
			//area->setOffset(ox, oy);
			area->setOriginX(ox);
			area->setOriginY(oy);

			m_rest(0.001);
			long elapsed = tguiCurrentTimeMillis() - start;
			long remaining = TARGET_TICKS_PER_FRAME - elapsed;
			if (remaining <= 0)
				break;
		}
	}

done:
	delete area;

	return 0;
}

