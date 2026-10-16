#pragma once
#include "MfAtomicActivity.h"
#include "enums.h"
class MfActor;
class MfPath3;
class MfSkill;
class MfStream;

class __declspec(dllexport) Mf3dPathActivity : public MfAtomicActivity {
public:
    enum { TYPE = '3dpA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfAtomicActivity::IsType(type);
    }
    virtual ~Mf3dPathActivity();
    virtual void setSkill(MfSkill *);
protected:
    virtual void _CheckBubble(unsigned long &);
public:
    virtual MfActivityStatus Update(unsigned long);
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int HasMotion() { return 1; }
    virtual int WillFinish();
    virtual long StreamType() const;
    virtual void Reset(unsigned long);
    Mf3dPathActivity();
    Mf3dPathActivity(MfActor *, unsigned long, MfSkill *, MfPath3 *, int, int);
    static long const STREAM_TYPE;
    MfPath3 *mPath;                                         // 0xac
    float mPathPos;                                         // 0xb0 -1 before the start
    int mb4;                                                // 0xb4 request option 0x1000
    int mBackwards;                                         // 0xb8
    int mbc;                                                // 0xbc request option 1
    int mc0;                                                // 0xc0 request option 0x10000
    int mc4;                                                // 0xc4
    int mc8;                                                // 0xc8
    int mFirstUpdate;                                       // 0xcc
    int mMatchGait;                                         // 0xd0 start at the skill's best matching frame
    MfTransform3 mLastTransform;                            // 0xd4 of the actor's skeleton
};
