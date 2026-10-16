#pragma once
#include "MfManagerActivity.h"
#include "enums.h"
class MfActor;
class MfStream;
class MfString;

class __declspec(dllexport) MfRandomActivity : public MfManagerActivity {
public:
    enum { TYPE = 'RndA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfManagerActivity::IsType(type);
    }
    virtual ~MfRandomActivity();
    virtual MfString PrintName() const { return MfString("Random"); }
    virtual MfActivityStatus Update(unsigned long);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int WillFinish();
    virtual long StreamType() const;
    MfRandomActivity();
    MfRandomActivity(MfActor *, unsigned long, int);
    static long const STREAM_TYPE;
protected:
    void pickRandomActivity();
public:
    MfAiActivity *mCurrent;                                 // 0x70
    int mRepeat;                                            // 0x74 how many activities to run, -1 no limit
    int mCount;                                             // 0x78
};
