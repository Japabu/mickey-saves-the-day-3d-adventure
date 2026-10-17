// sound.cpp - improvement over the original (port/edits/improvements.py hooks it in):
// the options panel's volumes for the game's sound channels (platform/win32/options.cpp).
#include "../../platform/port.h"

float PortSoundGain(int channel) { return port_sound_gain(channel); }
unsigned PortSoundGainVersion() { return port_sound_gain_version(); }
