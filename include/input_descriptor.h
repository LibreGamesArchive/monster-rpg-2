#ifndef INPUT_DESCRIPTOR_H
#define INPUT_DESCRIPTOR_H

#ifdef __cplusplus_XXX
enum Direction {
	DIRECTION_NONE = -1,
	DIRECTION_NORTH,
	DIRECTION_EAST,
	DIRECTION_SOUTH,
	DIRECTION_WEST
};
#else
#define DIRECTION_NONE -1
#define DIRECTION_NORTH 0
#define DIRECTION_EAST  1
#define DIRECTION_SOUTH 2
#define DIRECTION_WEST  3
#endif

struct InputDescriptor {
	bool left;
	bool right;
	bool up;
	bool down;
	bool button1;
	bool button2;
	bool button3;
	int direction;
};

#endif
