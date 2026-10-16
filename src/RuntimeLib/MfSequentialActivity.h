#pragma once
#include "MfManagerActivity.h"
#include "enums.h"
class MfActor;
class MfStream;
class MfString;

class __declspec(dllexport) MfSequentialActivity : public MfManagerActivity {
public:
    enum { TYPE = 'SeqA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfManagerActivity::IsType(type);
    }
    virtual ~MfSequentialActivity();
    virtual MfString PrintName() const { return MfString("Sequential"); }
    virtual MfActivityStatus Update(unsigned long);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int WillFinish();
    virtual long StreamType() const;
    MfSequentialActivity();
    MfSequentialActivity(MfActor *, unsigned long, int);
    static long const STREAM_TYPE;
    MfDblListItem *mPos;                                    // 0x70 the current subactivity's item
    MfAiActivity *mCurrent;                                 // 0x74
    int mRepeat;                                            // 0x78 how often to run the sequence, -1 always
    int mCount;                                             // 0x7c
};
