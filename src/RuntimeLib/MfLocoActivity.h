#pragma once
#include "MfAtomicActivity.h"
#include "enums.h"
class MfActor;
class MfAiPath2;
class MfPath2;
class MfSkeletonNode;
class MfSkill;
class MfStream;
class MfString;

class __declspec(dllexport) MfLocoActivity : public MfAtomicActivity {
public:
    enum { TYPE = 'LocA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfAtomicActivity::IsType(type);
    }
    virtual ~MfLocoActivity();
    virtual MfString PrintName() const { return MfString("Loco"); }
    virtual void setSkill(MfSkill *);
protected:
    virtual void _InitBubbleModel(MfSkill *, MfActor *, int);
public:
protected:
    virtual void _CheckBubble(unsigned long &);
public:
    virtual MfActivityStatus Update(unsigned long);
    virtual MfAiPath2 * GetCopy2DPath();
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int HasMotion() { return 1; }
    virtual int WillFinish();
    virtual long StreamType() const;
    virtual void Reset(unsigned long);
    MfLocoActivity();
    MfLocoActivity(MfActor *, unsigned long, MfSkill *, MfPath2 *, int, int, int, int, float, float, MfSkeletonNode *, int, int, int, int);
    MfPath2 *getPath() const { return mPath; }
    static long const STREAM_TYPE;
protected:
    int _ConformToFloor(unsigned long);
public:
    int mMode;                                              // 0xac 1, 2: request options 0x200, 0x400
    int mCheckCollision;                                    // 0xb0
    int mWithChildren;                                      // 0xb4 bubbles of the child actors too
    int mBackwards;                                         // 0xb8
    int mbc;                                                // 0xbc
    int mLimitCycles;                                       // 0xc0
    int mCycles;                                            // 0xc4
    MfPath2 *mPath;                                         // 0xc8
    float mPathPos;                                         // 0xcc point index along mPath, -1 before the start
    float md0;                                              // 0xd0
    float md4;                                              // 0xd4
    MfSkeletonNode *mNode;                                  // 0xd8
    int mFirstUpdate;                                       // 0xdc
    int mMatchGait;                                         // 0xe0 start at the skill's best matching frame
    MfTransform3 mLastTransform;                            // 0xe4 of the actor's skeleton
    int mCycleCount;                                        // 0x100
};
