// ScriptHelpNode.h - muffin/include/Game/ScriptHelpNode.h: a SimpleNode that runs Motivate
// scripts (CMBScript) and plays the animations they start. Layout from MBEngine.pdb. Also
// compiled into SteamboatApp.dll through SteamboatApp/ScriptHelpNode.cpp.
#ifndef SCRIPTHELPNODE_H
#define SCRIPTHELPNODE_H

#include "SimpleNode.h"

static const MBGUID MBIID_IMBScript = { 0x8efbf5d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBScript = { 0x8f021290, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

#include "IMBScript.h"

// what a script broadcasts on its channel (-6), and on -7 for the animations it starts
#include "MBScriptEvent.h"

class ScriptHelpNode : public SimpleNode {
public:
    ScriptHelpNode(IMBGameEngine *engine);
    virtual ~ScriptHelpNode(void);
    virtual void OnStart(void);
    virtual void OnEnd(void);
    virtual void OnPause(void);
    virtual void OnResume(void);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle(void);
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual bool StartScript(const char *name);          // vftable+0x94
    virtual bool StartScript(unsigned long id);          // vftable+0x90
    virtual void StopScript(const char *name);           // vftable+0x9c
    virtual void StopScript(unsigned long id);           // vftable+0x98
protected:
    virtual void StartListening(void);
    virtual void StopListening(void);
};

#endif
