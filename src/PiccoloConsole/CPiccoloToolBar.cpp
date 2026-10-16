// PiccoloConsole.dll 0x100092f0-0x1000942f: the splitter and the tool bar of
// the editor windows.
#include "PiccoloConsole.h"

// 0x100092f0 ?GetActivePane@CPiccoloSplitter@@UAEPAVCWnd@@PAH0@Z
CWnd *CPiccoloSplitter::GetActivePane(int *row, int *col)
{
    CWnd *view = GetFocus();
    if (view && !IsChildPane(view, row, col))
        view = NULL;
    return view;
}

// 0x10009330 ?OnCommand@CPiccoloSplitter@@MAEHIJ@Z
BOOL CPiccoloSplitter::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return CWnd::OnCommand(wParam, lParam) != 0;
}

// 0x10009350 ?OnNotify@CPiccoloSplitter@@MAEHIJPAJ@Z
BOOL CPiccoloSplitter::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *result)
{
    return CWnd::OnNotify(wParam, lParam, result) != 0;
}

// 0x10009370 ?OnWndMsg@CPiccoloSplitter@@MAEHIIJPAJ@Z
BOOL CPiccoloSplitter::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *result)
{
    *result = 0;
    if (message == WM_MOUSEWHEEL)
        return FALSE;
    return CWnd::OnWndMsg(message, wParam, lParam, result);
}

// 0x100093a0 ?_GetBaseMessageMap@CPiccoloToolBar@@KGPBUAFX_MSGMAP@@XZ
// 0x100093b0 ?GetMessageMap@CPiccoloToolBar@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(CPiccoloToolBar, CToolBar)
    ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

// 0x100093c0 ?WindowProc@CPiccoloToolBar@@MAEJIIJ@Z
LRESULT CPiccoloToolBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_IDLEUPDATECMDUI)
        return OnIdleUpdateCmdUI(wParam, lParam);
    return CControlBar::WindowProc(message, wParam, lParam);
}

// 0x100093f0 ?OnIdleUpdateCmdUI@CPiccoloToolBar@@IAEJIJ@Z
LRESULT CPiccoloToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
    CPiccoloWnd *wnd = (CPiccoloWnd *)GetParent();
    wnd->mUpdatingCmdUI = 1;
    LRESULT result = CControlBar::OnIdleUpdateCmdUI(wParam, lParam);
    wnd->mUpdatingCmdUI = 0;
    return result;
}
