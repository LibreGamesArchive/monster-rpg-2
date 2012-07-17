#ifndef _JOYPAD_HPP
#define _JOYPAD_HPP

#include "monster2.hpp"

void find_joypads(void);
bool is_32_or_later(void);
void init_joypad(void);
InputDescriptor get_joypad_state(void);
bool joypad_connected(void);

#endif
