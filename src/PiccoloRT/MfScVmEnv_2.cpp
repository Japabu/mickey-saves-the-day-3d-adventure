#include "MfScVmEnv_2.h"
#include "MfScVmEnv.h"
#include "MfScVmFrame.h"
#include "MfScVmStack.h"

// 0x1001bc90 ?StoreFrame@MfScVmEnv@@QAEXPAVMfScVmFrame@@@Z
void MfScVmEnv::StoreFrame(MfScVmFrame *pFrame)
{
    FirstFrame(pFrame);
}

// 0x1001c1f0 ?ResetStackOverflow@MfScVmEnv@@QAEXXZ
void MfScVmEnv::ResetStackOverflow()
{
    m_stack->ResetGuardPage();
}
