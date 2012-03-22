#ifndef EDITOR_HPP
#define EDITOR_HPP

void pushUndoInfo(void);
void brush(int i, int mx, int my);
void fill(int was, int i, int tx, int ty);
int checkTile(void);
void clearRedo(void);

extern WgtFrame *contextFrame;
extern WgtMenuItem *contextInsertRowBefore;
extern WgtMenuItem *contextInsertRowAfter;
extern WgtMenuItem *contextInsertColumnBefore;
extern WgtMenuItem *contextInsertColumnAfter;
extern WgtMenuItem *contextDeleteRow;
extern WgtMenuItem *contextDeleteColumn;
extern WgtMenuItem *contextInsert;
extern WgtMenuItem *contextDelete;
extern bool contextMenuOpen;
extern int contextFrameX, contextFrameY;

enum BrushType {
	BRUSH = 0,
	FILL,
	SOLIDS,
	CLEAR,
	MACRO,
	CLONE
};

struct MacroInfo {
	int x, y, layer, tile;
};


extern BrushType myBrush;
extern int currentLayer;
extern WgtMenuBar *menuBar;
extern int currentTile;
extern int startTile;
extern bool showSolids;
extern WgtScrollbar *vbarMain;
extern WgtScrollbar *hbarMain;
extern std::vector< std::vector<MacroInfo *> *> macros;
extern int currentMacro;
extern int clone_tile_x, clone_tile_y;
extern int clone_start_x, clone_start_y;


#endif

