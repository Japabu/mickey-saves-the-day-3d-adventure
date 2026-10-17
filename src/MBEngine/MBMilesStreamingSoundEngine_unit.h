// MBMilesStreamingSoundEngine_unit.h - declarations for MBEngine.dll MBMilesStreamingSoundEngine.obj
#ifndef MBMILESSTREAMINGSOUNDENGINE_UNIT_H
#define MBMILESSTREAMINGSOUNDENGINE_UNIT_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); }

#include "../MBCom/MBGUID.h"

#include "../MBCom/IMBUnknown.h"

class IMBStreamLocator;

static const MBGUID MBIID_IMBSoundDescription = { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBSoundDescription.h"

// Miles Sound System (mss32.dll)
struct _STREAM;
typedef _STREAM *HSTREAM;
extern "C" {
__declspec(dllimport) void __stdcall AIL_set_stream_loop_count(HSTREAM stream, long count);
__declspec(dllimport) void __stdcall AIL_start_stream(HSTREAM stream);
__declspec(dllimport) void __stdcall AIL_pause_stream(HSTREAM stream, long onoff);
__declspec(dllimport) long __stdcall AIL_stream_status(HSTREAM stream);
__declspec(dllimport) void __stdcall AIL_set_stream_volume(HSTREAM stream, long volume);
__declspec(dllimport) void __stdcall AIL_close_stream(HSTREAM stream);
}

class MBFileStream;

#include "MBSoundEngine.h"

#include "MBMilesStreamingSoundEngine.h"

#endif
