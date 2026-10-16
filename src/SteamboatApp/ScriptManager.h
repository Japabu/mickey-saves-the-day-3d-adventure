#pragma once
struct MBGUID;
// class ScriptManager {
// public:
//     virtual long QueryInterface(MBGUID const &, void * *);
//     virtual unsigned long AddRef();
//     virtual unsigned long Release();
//     virtual void ListenToEvent(long, long, void *);
//     ScriptManager();
//     ~ScriptManager();
//     unsigned long _mRefCnt;
// };
class ScriptManager {
public:
    virtual long QueryInterface(MBGUID const &, void * *);
    virtual unsigned long AddRef();
    virtual unsigned long Release();
    virtual void ListenToEvent(long, long, void *);
    ScriptManager();
    ~ScriptManager();
    unsigned long _mRefCnt;
};


