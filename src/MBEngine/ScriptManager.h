// ScriptManager.h - MBEngine's copy: keeps a reference to every running script (by ID) so that StopScript can find it and the scripts are stopped when the game quits.
// Declared first by the includer (still one copy per unit in wave 1): IMBUnknown, MBComPtr, MBGUID, MBIID_IMBEventListener, MBIID_IMBEventManager, MBIID_IMBScript, MBMultimap.
#ifndef MBENGINE_SCRIPTMANAGER_H
#define MBENGINE_SCRIPTMANAGER_H

#include "IMBEventListener.h"
#include "IMBEventManager.h"
#include "IMBScript.h"
#include "MBScriptEvent.h"

class ScriptManager : public IMBEventListener {
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    ScriptManager(void)
    {
        _mRefCnt = 1;
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (broadcaster) {
            broadcaster->AddListener(-6, this);
            broadcaster->AddListener(-3, this);
        }
    }
    ~ScriptManager(void)
    {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (broadcaster) {
            broadcaster->RemoveListener(-6, this);
            broadcaster->RemoveListener(-3, this);
        }
        MBMultimap<unsigned long, IMBScript *, std::less<unsigned long> >::iterator it;
        for (it = scripts.begin(); it != scripts.end(); it++)
            (*it).second->Release();
    }
    void AddScript(MBScriptEvent *event)
    {
        event->script->AddRef();
        scripts.insert(MBMultimap<unsigned long, IMBScript *, std::less<unsigned long> >::value_type(
            event->scriptID, event->script));
    }
    void RemoveScript(MBScriptEvent *event)
    {
        MBMultimap<unsigned long, IMBScript *, std::less<unsigned long> >::iterator it =
            scripts.lower_bound(event->scriptID);
        MBMultimap<unsigned long, IMBScript *, std::less<unsigned long> >::iterator end =
            scripts.upper_bound(event->scriptID);
        for (; it != end; it++) {
            if ((*it).second == event->script) {
                (*it).second->Release();
                scripts.erase(it);
                return;
            }
        }
    }
    IMBScript *FindScript(unsigned long id)
    {
        MBMultimap<unsigned long, IMBScript *, std::less<unsigned long> >::iterator it = scripts.find(id);
        if (it != scripts.end())
            return (*it).second;
        return 0;
    }
    virtual void ListenToEvent(long channel, long event, void *data);
    MBMultimap<unsigned long, IMBScript *, std::less<unsigned long> > scripts;
};

#endif
