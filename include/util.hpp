#ifndef UTIL_HPP
#define UTIL_HPP


const char *getUserResource(const char *fmt, ...);
#ifdef __cplusplus
extern "C" {
#endif
const char *getResource(const char *fmt, ...);
#ifdef __cplusplus
}
#endif
void saveBlender(void);
void restoreBlender(void);
bool pointInBox(int px, int py, int x1, int y1, int x2, int y2);
const char *my_itoa(int i);
int countOccurances(const char *s, char c);
const char *findOccurance(const char *text, char c, int num);
int check_arg(int argc, char **argv, const char *arg);
void native_error(const char *msg, const char *msg2 = NULL);
bool isVowel(char c);

enum IPHONE_LINE_DIR {
	IPHONE_LINE_DIR_NORTH,
	IPHONE_LINE_DIR_EAST,
	IPHONE_LINE_DIR_SOUTH,
	IPHONE_LINE_DIR_WEST
};

const int IPHONE_LINE_MIN = 50;
extern double iphone_line_times[4];
extern double iphone_shake_time;
bool iphone_line(IPHONE_LINE_DIR dir, double since);
bool iphone_shaken(double since);
void iphone_clear_line(IPHONE_LINE_DIR dir);
void iphone_clear_shaken(void);

extern int myArgc;
extern char **myArgv;
extern unsigned long last_shake_check;


#endif

