// PiccoloRT.dll 0x1001b890-0x1001b8f8: MfScVmFrame.
#include "MfScVmFrame.h"
#include "MfScDataHandle.h"

// 0x1001b890 ??0MfScVmFrame@@QAE@XZ
MfScVmFrame::MfScVmFrame()
    : mIP(0), mObject(0), mFunction(0), mConstants(0), mFrame(0)
{
}

// 0x1001b8b0 ??1MfScVmFrame@@QAE@XZ
MfScVmFrame::~MfScVmFrame()
{
    ReleaseHandles();
}

// 0x1001b8c0 ?ReleaseHandles@MfScVmFrame@@QAEXXZ
void MfScVmFrame::ReleaseHandles()
{
    if (mObject)
        mObject->Release();
    if (mFunction)
        mFunction->Release();
    mIP = 0;
    mObject = 0;
    mFunction = 0;
    mConstants = 0;
    mFrame = 0;
}
