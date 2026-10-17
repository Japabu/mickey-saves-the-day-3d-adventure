#ifndef SCRIPTHELPNODE_H
#define SCRIPTHELPNODE_H
// ScriptHelpNode: a SimpleNode that runs Motivate scripts (declaration from SteamboatApp.pdb;
// the code is MBEngine's ScriptHelpNode.cpp, compiled into this binary by ScriptHelpNode.cpp).
#include "SimpleNode.h"

// own copy: MBEngine's ScriptHelpNode.h builds on its SimpleNode.h (see SimpleNode.h).
class ScriptHelpNode : public SimpleNode {
public:
    ScriptHelpNode(const ScriptHelpNode &);
    ScriptHelpNode(IMBGameEngine *);
    virtual ~ScriptHelpNode();                          // deleting destructor at vftable+0x20
    virtual void OnStart();                             // vftable+0x0c
    virtual void OnEnd();                               // vftable+0x18
    virtual void OnPause();                             // vftable+0x10
    virtual void OnResume();                            // vftable+0x14
    virtual void ListenToEvent(long, long, void *);     // vftable+0x0c (subobject +0x8), this adjust +0x8
    virtual void Idle();                                // vftable+0x0c (subobject +0x4), this adjust +0x4
    virtual bool StartScript(const char *);             // vftable+0x94
    virtual bool StartScript(unsigned long);            // vftable+0x90
    virtual void StopScript(const char *);              // vftable+0x9c
    virtual void StopScript(unsigned long);             // vftable+0x98
protected:
    virtual void StartListening();                      // vftable+0x88
    virtual void StopListening();                       // vftable+0x8c
};

#endif
