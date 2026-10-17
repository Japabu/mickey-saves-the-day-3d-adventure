// MfOverlayActivity: a manager activity that runs its subactivities side by
// side; exported.
#ifndef RUNTIMELIB_MFOVERLAYACTIVITY_H
#define RUNTIMELIB_MFOVERLAYACTIVITY_H
#include "MfManagerActivity.h"
#include "enums.h"
#include "RuntimeLib_api.h"
class MfActor;
class MfStream;
class MfString;

class RUNTIME_API MfOverlayActivity : public MfManagerActivity {
public:
    enum { TYPE = 'OvlA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfManagerActivity::IsType(type);
    }
    virtual ~MfOverlayActivity();
    virtual MfString PrintName() const;
    virtual MfActivityStatus Update(unsigned long);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int WillFinish();
    virtual long StreamType() const;
    MfOverlayActivity();
    MfOverlayActivity(MfActor *, unsigned long);
    static long const STREAM_TYPE;
    int mKeepRunning;                                       // 0x70 never finishes by itself
};

#endif
