// sound.cpp - improvement over the original (port/edits/improvements.py hooks it in):
// the background music follows the options panel's music volume.
#include "../../platform/port.h"

void PortMusicStream(void *stream) { port_music_stream(stream); }
