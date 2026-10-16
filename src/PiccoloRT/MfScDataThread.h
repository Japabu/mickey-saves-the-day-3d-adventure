#pragma once
#include "MfScDataObject.h"
#include "MfThread.h"
class MfScVirtualMachine;

// The script object of a script thread: runs a function with its arguments
// on a new MfScThread.
class MfScDataThread : public MfScDataObject {
public:
    // the work the thread does: calls the function on its own VM
    class MfScDataThreadWorker : public MfThreadWork {
    public:
        MfScDataThreadWorker(MfScDataHandle *self, MfScDataHandle *function, MfScDataHandle *args);
        virtual ~MfScDataThreadWorker();
        virtual void Work();

        MfScDataHandle *mSelf;          // +0x04
        MfScDataHandle *mFunction;      // +0x08
        MfScDataHandle *mArgs;          // +0x0c
        int mRunning;                   // +0x10
        MfScVirtualMachine *mVM;        // +0x14
    };

    MfScDataThread();
    virtual ~MfScDataThread();
    // starts a thread that calls function(args)
    void Start(MfScDataHandle *self, MfScDataHandle *function, MfScDataHandle *args);

    MfScDataThreadWorker *mWorker;      // +0x14
};
