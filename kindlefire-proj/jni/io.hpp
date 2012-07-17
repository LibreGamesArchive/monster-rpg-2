#ifndef IO_H
#define IO_H


#include "monster2.hpp"

struct SaveStateInfo
{
	int exp;
	int gold;
	int time;
};


extern void iputl(long l, gzFile f);
extern long igetl(gzFile f);
extern void my_pack_putc(int c, gzFile f);
extern int my_pack_getc(gzFile f);
extern bool readMilestones(bool* ms, int num, gzFile f);
extern bool WriteMilestones(bool* ms, int num, gzFile f);
extern const char *readString(gzFile f);
extern void writeString(const char *s, gzFile f);
extern void saveGame(const char* filename, std::string mapArea = "");
extern bool loadGame(const char* filename);
extern void saveTime(char *filename);
void getSaveStateInfo(int num, SaveStateInfo& info, bool autosave);

extern unsigned char memory_save[20000];
extern int memory_save_offset;
extern bool using_memory_save;
extern bool memory_saved;

#endif
