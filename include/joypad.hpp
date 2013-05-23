#ifndef NO_JOYPAD

#ifndef _JOYPAD_HPP
#define _JOYPAD_HPP

#include "monster2.hpp"

void find_joypads();
void stop_finding_joypads();
bool is_32_or_later();
void init_joypad();
InputDescriptor get_joypad_state();
bool joypad_connected();
void reset_joypad_state();

#endif

#endif // NO_JOYPAD
