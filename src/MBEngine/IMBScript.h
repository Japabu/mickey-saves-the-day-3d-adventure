// IMBScript.h - a running Motivate script.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBGUID, MBIID_IMBScript.
#ifndef MBENGINE_IMBSCRIPT_H
#define MBENGINE_IMBSCRIPT_H

class IMBStream;

class IMBScript : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBScript; }
    virtual bool Load(unsigned long, IMBStream *) = 0;
    virtual void Start(void) = 0;
    virtual void Stop(void) = 0;
    virtual void Pause(void) = 0;
    virtual void Resume(void) = 0;
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual bool SetScriptVariable(const char *, const char *) = 0; // vftable+0x24
    virtual bool SetScriptVariable(const char *, double) = 0;       // vftable+0x20
    virtual bool GetScriptVariable(const char *, const char **) = 0; // vftable+0x2c
    virtual bool GetScriptVariable(const char *, double *) = 0;      // vftable+0x28
    virtual bool IsStarted(void) const = 0;
    virtual bool IsDone(void) const = 0;
    virtual bool InBackground(void) const = 0;
    virtual long GetID(void) const = 0;
};

#endif
