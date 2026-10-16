#pragma once
// ConfirmationBox.h: a modal yes/no box made of a background animation and two animated
// buttons; while it is shown it owns the mouse (ConfirmationBox.obj, declaration from
// SteamboatApp.pdb).
#include "SBInterfaces.h"

class ConfirmationBox : public IMBEventListener {
protected:
    unsigned long _mRefCnt;                             // +0x0004
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef();
    virtual unsigned long Release();
    // thrown by Create when a step fails: the source line and the error code
    struct exception {
        exception(long line, long error) : nLine(line), nError(error) {}
        long nLine;
        long nError;
    };
    ConfirmationBox();
    virtual ~ConfirmationBox();
    virtual long Create();
    virtual void Destroy();
    virtual void SetBackgroundName(MBString bgname);
    virtual void SetButtonNames(MBString yesname, MBString noname);
    virtual bool IsActive();
protected:
    virtual void ListenToEvent(long channel, long event, void *data);
private:
    MBComPtr<IMBMouseMonitor> mMyMouseMonitor;          // +0x0008
    MBComPtr<IMBMouseMonitor> mNodeMouseMonitor;        // +0x000c
    MBString mBackgroundName;                           // +0x0010
    MBString mYesName;                                  // +0x0020
    MBString mNoName;                                   // +0x0030
    IMBAnim *mBackground;                               // +0x0040
    ISBAnimButton *mYes;                                // +0x0044
    ISBAnimButton *mNo;                                 // +0x0048
    bool mActive;                                       // +0x004c
};
