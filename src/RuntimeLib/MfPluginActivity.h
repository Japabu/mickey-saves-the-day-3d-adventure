// MfPluginActivity: an atomic activity driven by an application plugin
// (MfAiActivitySpec). RuntimeLib's own class (unit 1004c230-1004c5e0, RTTI
// name; vtables 100b47fc, 100b473c, 100b4724).
#ifndef RUNTIMELIB_MFPLUGINACTIVITY_H
#define RUNTIMELIB_MFPLUGINACTIVITY_H
#include "MfAtomicActivity.h"
class MfAiActivitySpec;
class MfStream;

class MfPluginActivity : public MfAtomicActivity {
public:
    enum { TYPE = 'PluA' };
    MfPluginActivity() {}
    MfPluginActivity(MfActor *actor, unsigned long startTime, const MfAiActivitySpec *spec);
    virtual int Type() { return TYPE; }
    // not an atomic activity to the type test
    virtual int IsType(int type)
    {
        if (type == TYPE)
            return 1;
        return MfActivity::IsType(type);
    }
    virtual ~MfPluginActivity();
    virtual MfActivityStatus Update(unsigned long time);
    virtual MfAiActivitySpec *GetSpec() { return mSpec; }
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    static long const STREAM_TYPE;

    MfAiActivitySpec *mSpec;                                // 0xac
};

#endif
