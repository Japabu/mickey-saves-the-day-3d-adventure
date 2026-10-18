// sound.cpp - improvement over the original (port/edits/improvements.py hooks it in):
// the options panel's volumes for the game's sound channels (platform/win32/options.cpp),
// and the game's sounds by name for the panel's samples (platform/media/miles.cpp).
#include "MBSoundUtils.h"
#include "IMBStream.h"
#include "../../platform/port.h"
#include <stdlib.h>

unsigned PortSoundGainVersion() { return port_sound_gain_version(); }
void PortNextSoundChannel(int channel) { port_sound_next_channel(channel); }

// The bytes of a sound of the loaded resource databases ('wav ' by its name's hash, as
// MBPlaySound finds it), malloc'd; 0 when there is none.
extern "C" int port_read_sound(const char *name, unsigned char **data, unsigned long *size)
{
    *data = 0;
    *size = 0;
    MBComPtr<IMBResourceStreamLocator> locator(MBIID_CMBResourceStreamLocator);
    if (!locator)
        return 0;
    locator->SetResource(MBHashString(name), 'wav ');
    IMBStream *stream = 0;
    locator->GetStream(&stream);
    if (!stream)
        return 0;
    unsigned long n = stream->GetStreamSize();
    unsigned char *bytes = n ? (unsigned char *)malloc(n) : 0;
    if (bytes) {
        unsigned int got = (unsigned int)n;  // the game's 32-bit unsigned long
        stream->Read(bytes, got);
    }
    stream->Release();
    *data = bytes;
    *size = bytes ? n : 0;
    return bytes != 0;
}
