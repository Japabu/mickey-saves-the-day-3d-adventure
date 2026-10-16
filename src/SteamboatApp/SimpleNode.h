#ifndef SIMPLENODE_H
#define SIMPLENODE_H
// muffin/include/Game/SimpleNode.h: the engine's base node (declaration from SteamboatApp.pdb;
// the code is MBEngine's SimpleNode.cpp, compiled into this binary by SimpleNode.cpp).
#include "MBInterfaces.h"

class SimpleNode : public IMBNode, public IMBPeriodical, public IMBEventListener {
public:
    SimpleNode(const SimpleNode &);
    SimpleNode(IMBGameEngine *, bool);
    virtual ~SimpleNode();                              // deleting destructor at vftable+0x20
    static void __cdecl Startup(IMBGameEngine *);
    static void __cdecl Shutdown(IMBGameEngine *);
protected:
    unsigned long _mRefCnt;                             // +0x000c size 0x4
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj)
    {
        if (!obj)
            return -2;
        *obj = 0;
        IMBUnknown *p = 0;
        if (iid == MBIID_IMBUnknown)
            p = (IMBNode *)this;
        if (iid == MBIID_IMBNode)
            p = (IMBNode *)this;
        if (iid == MBIID_IMBPeriodical)
            p = (IMBPeriodical *)this;
        if (iid == MBIID_IMBEventListener)
            p = (IMBEventListener *)this;
        if (p) {
            p->AddRef();
            *obj = p;
            return 0;
        }
        return -3;
    }
    virtual unsigned long AddRef() { return ++_mRefCnt; }
    virtual unsigned long Release()
    {
        unsigned long r = --_mRefCnt;
        if (!_mRefCnt)
            delete this;
        return r;
    }
    virtual void OnStart();                             // vftable+0x0c
    virtual void OnEnd();                               // vftable+0x18
    virtual void OnPause();                             // vftable+0x10
    virtual void OnResume();                            // vftable+0x14
    virtual void OnQuit();                              // vftable+0x1c
    virtual void OnPreRender(MBComPtr<IMBScreen>);      // vftable+0x24
    virtual void OnPostRender(MBComPtr<IMBScreen>);     // vftable+0x28
    virtual void OnPostFlip(MBComPtr<IMBScreen>);       // vftable+0x2c
    virtual void OnKeyDown(MBKeyboardEvent &);          // vftable+0x30
    virtual void OnKeyUp(MBKeyboardEvent &);            // vftable+0x34
    virtual void OnLMouseDown(MBMouseEvent &);          // vftable+0x38
    virtual void OnLMouseUp(MBMouseEvent &);            // vftable+0x3c
    virtual void OnLMouseDblClk(MBMouseEvent &);        // vftable+0x40
    virtual void OnMMouseDown(MBMouseEvent &);          // vftable+0x44
    virtual void OnMMouseUp(MBMouseEvent &);            // vftable+0x48
    virtual void OnMMouseDblClk(MBMouseEvent &);        // vftable+0x4c
    virtual void OnRMouseDown(MBMouseEvent &);          // vftable+0x50
    virtual void OnRMouseUp(MBMouseEvent &);            // vftable+0x54
    virtual void OnRMouseDblClk(MBMouseEvent &);        // vftable+0x58
    virtual void OnMouseMove(MBMouseEvent &);           // vftable+0x5c
    virtual void OnAnimSeqStarted(unsigned long, unsigned long); // vftable+0x60
    virtual void OnAnimSeqEnded(unsigned long, unsigned long); // vftable+0x64
    virtual void OnAnimNotify(unsigned long, unsigned long, unsigned long); // vftable+0x68
    virtual void OnSoundStarted(long, unsigned long);   // vftable+0x6c
    virtual void OnSoundEnded(long, unsigned long);     // vftable+0x70
    virtual void ListenToEvent(long, long, void *);     // vftable+0x0c (subobject +0x8), this adjust +0x8
    virtual void Idle();                                // vftable+0x0c (subobject +0x4), this adjust +0x4
    virtual void HideCursor();                          // vftable+0x74
    virtual void ShowCursor();                          // vftable+0x78
    virtual void SetCursor(MBComPtr<IMBAnim>, short, short); // vftable+0x7c
    virtual void SetCursorLocation(MBPoint);            // vftable+0x80
    void SetResourceDirectories(const MBString &, const MBString &);
    void AddDatabase(const char *);
    void RemoveDatabase(const char *);
    MBComPtr<IMBStream> GetStream(unsigned long, unsigned long);
    MBComPtr<IMBAnim> CreateAnim(long);
    MBComPtr<IMBAnim> CreateAnim(const char *);
    MBComPtr<IMBAnim> CloneAnim(const MBComPtr<IMBAnim> &);
    MBComPtr<IMBFont> CreateFont(long);
    MBComPtr<IMBFont> CreateFont(const char *);
    MBComPtr<IMBFont> CreateUnicodeFont(long);
    MBComPtr<IMBFont> CreateUnicodeFont(const char *);
    MBComPtr<IMBFont> CreateSystemFont(const char *, const int, const MBColor &, const int);
    void StartSound(const char *);
    virtual void LoadStringTable(const char *);         // vftable+0x84
    void GetString(unsigned long, MBString16 &);
    void GetNamedString(const char *, MBString16 &);
    void SetLocale(MBLocale &);
protected:
    IMBGameEngine *mEngine;                             // +0x0010 size 0x4
    bool mRouteMessages;                                // +0x0014 size 0x1
    MBComPtr<IMBWorld> mWorld;                          // +0x0018 size 0x4
    MBComPtr<IMBView> mView;                            // +0x001c size 0x4
    MBComPtr<IMBMouseMonitor> mMouseMonitor;            // +0x0020 size 0x4
    void PlaceCursor();
    MBComPtr<IMBAnim> mCursor;                          // +0x0024 size 0x4
    virtual void StartListening();                      // vftable+0x88
    virtual void StopListening();                       // vftable+0x8c
    static IMBStringTable *sStringTable;
    static MBLocale sLocale;
};

#endif
