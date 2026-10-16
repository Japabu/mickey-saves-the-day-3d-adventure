// PiccoloRT.dll 0x10008540-0x10008993: the script thread object, with the
// compiler-generated members of MfScThread and MfScVirtualMachine it emits.
#include "MfScDataThread.h"
#include "MfScThread.h"
#include "MfScDataHandle.h"
#include "MfScVirtualMachine.h"

// 0x10008540 ??0MfScThread@@QAE@ABV0@@Z
// 0x10008590 ??4MfScThread@@QAEAAV0@ABV0@@Z
// 0x100085d0 ??_FMfScThread@@QAEXXZ
// 0x100085e0 ??_EMfScThread@@UAEPAXI@Z
// 0x10008640 ??0MfScVirtualMachine@@QAE@ABV0@@Z
// 0x10008670 ??_EMfScVirtualMachine@@UAEPAXI@Z

// 0x100086c0 ??0MfScDataThread@@QAE@XZ
MfScDataThread::MfScDataThread()
    : mWorker(0)
{
}

// 0x10008700 ??1MfScDataThread@@UAE@XZ
MfScDataThread::~MfScDataThread()
{
    delete mWorker;
}

// 0x10008760 ?Start@MfScDataThread@@QAEXPAVMfScDataHandle@@00@Z
void MfScDataThread::Start(MfScDataHandle *self, MfScDataHandle *function, MfScDataHandle *args)
{
    MfScThread *thread = new MfScThread(0);
    mWorker = new MfScDataThreadWorker(self, function, args);
    thread->AssignWork(mWorker);
}

// 0x10008810 ??0MfScDataThreadWorker@MfScDataThread@@QAE@PAVMfScDataHandle@@00@Z
MfScDataThread::MfScDataThreadWorker::MfScDataThreadWorker(MfScDataHandle *self, MfScDataHandle *function,
                                                           MfScDataHandle *args)
    : mSelf(self), mFunction(function), mArgs(args), mRunning(0)
{
    self->AddRef();
    mFunction->AddRef();
    if (mArgs)
        mArgs->AddRef();
    mVM = new MfScVirtualMachine;
}

// 0x100088d0 ??1MfScDataThreadWorker@MfScDataThread@@UAE@XZ
MfScDataThread::MfScDataThreadWorker::~MfScDataThreadWorker()
{
    delete mVM;
}

// 0x10008920 ?Work@MfScDataThreadWorker@MfScDataThread@@UAEXXZ
void MfScDataThread::MfScDataThreadWorker::Work()
{
    mRunning = 1;
    if (mArgs)
        mArgs->AddRef();
    MfScDataHandle *result = mVM->ExecuteFunction(mFunction, 1, mArgs);
    if (result)
        result->Release();
    mRunning = 0;
    mFunction->Release();
    if (mArgs)
        mArgs->Release();
    mSelf->Release();
}
