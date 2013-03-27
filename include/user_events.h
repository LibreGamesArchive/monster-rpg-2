#ifndef USER_EVENTS_H
#define USER_EVENTS_H

enum MyUserEvents {
	USER_KEY_DOWN = ALLEGRO_GET_EVENT_TYPE('M','R','P','G'),
	USER_KEY_UP,
	USER_KEY_CHAR,
	USER_STOP
};

extern ALLEGRO_EVENT_SOURCE user_event_source;

#endif
