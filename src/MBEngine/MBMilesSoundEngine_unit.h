// MBMilesSoundEngine_unit.h - declarations for MBEngine.dll MBMilesSoundEngine.obj
#ifndef MBMILESSOUNDENGINE_UNIT_H
#define MBMILESSOUNDENGINE_UNIT_H

#include <string> // evidence: original TU has $E19/$E20 for std::ctype<unsigned short>::id

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { if (p) MBfree_release(p); }

#include "../MBCom/MBGUID.h"

// header-defined interface IDs (PDB ldata 0x1008ed38..0x1008ed88)
#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBSoundDescription =
    { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundManager =
    { 0x79ee6f80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream =
    { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStreamLocator =
    { 0x89cc0fe0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };


extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

#include "IMBStreamSeekValues.h"
#include "IMBStreamError.h"

#include "IMBStream.h"

// own copy: carries the sUnknownStreamLocatorStatic $E stand-in (see NOTES.md)
class IMBStreamLocator : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStreamLocator; }
    virtual long GetStream(IMBStream **) = 0;
    virtual unsigned long GetID(void) const = 0;
    // stand-in: the original's ctype id $E20/$E19 are one past the <string> baseline ($E19/$E18);
    // the obj's PDB data has no guard/static, and every STL unit including the IMBStreamLocator
    // header has the +1, so one static data member declaration there takes it (name unknown;
    // never defined or used)
    static const unsigned long sUnknownStreamLocatorStatic;
};

#include "IMBSoundDescription.h"

#include "MBSoundData.h"

#include "IMBSoundManager.h"

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    ~MBComPtr(void) {
        if (mInterface) mInterface->Release();
        mInterface = 0;
    }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    void Release(void); // 0x10018a60, emitted in another object
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p); // 0x10018ac0, emitted in another object
    T *mInterface;
};

// Miles Sound System (mss32.dll)
struct _SAMPLE;
typedef _SAMPLE *HSAMPLE;
struct _DLSFILEID;
struct AUDIO_TYPE {
    const unsigned long *data;
    long size;
    long type;
    void *handle;
    long status;
    void *next;
    long speed;
    long volume;
    long extravol;
    float rlevel;
    float rrtime;
    float rdtime;
    _DLSFILEID *dlsid;
    void *dlsmem;
    void *dlsmemunc;
    long milliseconds;
    long length;
    long userdata;
};
typedef AUDIO_TYPE *HAUDIO;
extern "C" {
__declspec(dllimport) HAUDIO __stdcall AIL_quick_load_mem(const void *mem, unsigned long size);
__declspec(dllimport) long __stdcall AIL_quick_play(HAUDIO audio, unsigned long loop_count);
__declspec(dllimport) void __stdcall AIL_quick_halt(HAUDIO audio);
__declspec(dllimport) long __stdcall AIL_quick_status(HAUDIO audio);
__declspec(dllimport) void __stdcall AIL_quick_unload(HAUDIO audio);
__declspec(dllimport) void __stdcall AIL_quick_set_volume(HAUDIO audio, long volume, long extravol);
__declspec(dllimport) void __stdcall AIL_quick_set_speed(HAUDIO audio, long speed);
__declspec(dllimport) long __stdcall AIL_quick_ms_position(HAUDIO audio);
__declspec(dllimport) void __stdcall AIL_quick_set_ms_position(HAUDIO audio, long milliseconds);
__declspec(dllimport) unsigned long __stdcall AIL_sample_playback_rate(HSAMPLE S);
__declspec(dllimport) long __stdcall AIL_sample_loop_count(HSAMPLE S);
}

#include "MBSoundEngine.h"

#include "MBMilesSoundEngine.h"

#endif
