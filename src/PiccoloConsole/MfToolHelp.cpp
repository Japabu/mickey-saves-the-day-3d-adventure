// PiccoloConsole.dll 0x1000ca50-0x1000cdcf: MfToolHelp, the window that shows
// the full title of an editor window when it does not fit.
#include "PiccoloConsole.h"

// 0x1000ca50 ??0MfToolHelp@@QAE@XZ
MfToolHelp::MfToolHelp()
{
    OSVERSIONINFO version;
    NONCLIENTMETRICS metrics;

    mFont = new CFont;
    version.dwOSVersionInfoSize = sizeof(version);
    GetVersionEx(&version);
    if (version.dwMajorVersion >= 4) {
        metrics.cbSize = sizeof(metrics);
        SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);
        mFont->CreateFontIndirect(&metrics.lfSmCaptionFont);
    } else {
        mFont->CreateFont(8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "ms sans serif");
    }
}

// 0x1000cb60 ??_GMfToolHelp@@UAEPAXI@Z

// 0x1000cb80 ??1MfToolHelp@@UAE@XZ
MfToolHelp::~MfToolHelp()
{
    delete mFont;
}

// 0x1000cbf0 ?FitText@MfToolHelp@@QAEXXZ
void MfToolHelp::FitText()
{
    CRect rect;

    GetWindowRect(&rect);
    CDC *dc = GetDC();
    CFont *oldFont = dc->SelectObject(mFont);
    CSize size = dc->GetTextExtent(mText, mText.Length());
    dc->SelectObject(oldFont);
    ReleaseDC(dc);
    rect.right = rect.left + size.cx + 10;
    CRect work;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &work, 0);
    if (rect.right >= work.Width()) {
        rect.right = work.Width() - 1;
        rect.left = rect.right - size.cx - 10;
    }
    MoveWindow(&rect);
}

// 0x1000ccd0 ?GetMessageMap@MfToolHelp@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfToolHelp, CWnd)
    ON_WM_PAINT()
END_MESSAGE_MAP()

// 0x1000cce0 ?PreCreateWindow@MfToolHelp@@MAEHAAUtagCREATESTRUCTA@@@Z
BOOL MfToolHelp::PreCreateWindow(CREATESTRUCT &cs)
{
    COLORREF color = GetSysColor(COLOR_INFOBK);
    cs.style = WS_POPUP | WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_OWNDC, NULL, CreateSolidBrush(color), NULL);
    return CWnd::PreCreateWindow(cs);
}

// 0x1000cd20 ?OnPaint@MfToolHelp@@IAEXXZ
void MfToolHelp::OnPaint()
{
    CPaintDC dc(this);

    CFont *oldFont = dc.SelectObject(mFont);
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(GetSysColor(COLOR_INFOTEXT));
    dc.TextOut(4, 2, mText, mText.Length());
    dc.SelectObject(oldFont);
}
