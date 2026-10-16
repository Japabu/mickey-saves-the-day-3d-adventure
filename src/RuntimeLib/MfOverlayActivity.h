#pragma once
#include "MfManagerActivity.h"
#include "enums.h"
class MfActor;
class MfStream;
class MfString;

class __declspec(dllexport) MfOverlayActivity : public MfManagerActivity {
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
