// MfManagerActivity: an activity that runs subactivities (the base of the
// overlay, sequential and random activities); exported.
#ifndef RUNTIMELIB_MFMANAGERACTIVITY_H
#define RUNTIMELIB_MFMANAGERACTIVITY_H
#include "MfActivity.h"
#include "MfDblList.h"
#include "enums.h"
#include "RuntimeLib_api.h"
class MfActor;
class MfAiActivity;
class MfAiActor;
class MfDblList;
class MfLiveActor;
class MfStream;
class MfString;

class RUNTIME_API MfManagerActivity : public MfActivity {
public:
    enum { TYPE = 'MgrA' };
    virtual int Type() { return TYPE; }
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfActivity::IsType(type);
    }
    virtual ~MfManagerActivity();
    virtual void SetUniqueID(int);
    virtual MfString PrintName() const;
    virtual void PrepareForExecution();
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual int AddActivity(MfAiActivity const *);
    virtual int AnimateSubtree(MfString const &);
    virtual int HasMotion();
    virtual int IgnoreSubtree(MfString const &);
    virtual int IsManagerActivity();
    virtual int RemoveHoldActivity(MfAiActor const *);
    virtual int RemoveSubactivity(MfAiActivity const *);
    virtual int RemoveSubactivity(int);
    virtual long StreamType() const;
    virtual void GetSubactivities(MfDblList *);
    virtual void Reset(unsigned long);
    virtual void SetDuration(unsigned long);
    MfActivity * FindType(int);
    MfDblList * GetSubactivities();
    MfManagerActivity();
    MfManagerActivity(MfActor *, unsigned long);
    int Contains(MfActivity *);
    int RemoveSubactivity(MfAiActivity const *, int);
    int RemoveSubactivity(int, int);
    int SubContains(MfManagerActivity *);
    void RemoveActivityThatRefersToThisActor(MfLiveActor *);
    void RemoveSubactivities();
    static long const STREAM_TYPE;

    MfDblList mSubactivities;                               // 0x60
};

#endif
