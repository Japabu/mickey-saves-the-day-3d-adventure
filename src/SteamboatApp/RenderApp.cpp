#include <windows.h>
#include <string.h>
#include "RenderApp.h"
#include "MBNew.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x10034ce0 ??0RenderApp@@QAE@XZ
RenderApp::RenderApp()
{
}

// 0x10034d00 ?SwitchServer@RenderApp@@UAEHABVMfString@@@Z
int RenderApp::SwitchServer(const MfString &serverName)
{
    return 1;
}

// 0x10034d20 ?SendPacket@RenderApp@@UAEXHPAXH@Z
void RenderApp::SendPacket(int hid, void *data, int length)
{
}

// 0x10034d80 ??1RenderApp@@UAE@XZ
RenderApp::~RenderApp()
{
}

// 0x10034da0 ?GetInterfaceName@RenderApp@@UAEXPADHAAH1@Z
void RenderApp::GetInterfaceName(char *nameBuffer, int maxNameSize, int &major, int &minor)
{
    strncpy(nameBuffer, "MfAiApp", maxNameSize - 1);
    nameBuffer[maxNameSize - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x10034d40 ??_GRenderApp@@UAEPAXI@Z

// 0x10034df0 ?AddRef@RenderApp@@UAEHXZ
int RenderApp::AddRef()
{
    return 1;
}

// 0x10034e10 ?Release@RenderApp@@UAEHXZ
int RenderApp::Release()
{
    return 1;
}

// 0x10034e30 ?GetClock@RenderApp@@UAEKXZ
unsigned long RenderApp::GetClock()
{
    return GetTickCount();
}

// 0x10034e50 _$E24
// 0x10034e90 _$E20
// 0x10034eb0 _$E23
