// CMBWinDraw.cpp - MBEngine.dll CMBWinDraw.obj
#include "CMBWinDraw.h"

// 0x1005b350 ?MakeCMBWinDraw@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBWinDraw(void)
{
    IMBUnknown *p = new CMBWinDraw;
    if (p)
        p->AddRef();
    return p;
}

// 0x1005b380 ??0CMBWinDraw@@QAE@XZ
CMBWinDraw::CMBWinDraw(void)
    : m_hDC(0), m_hBrush(0), m_hOldBrush(0), m_hPen(0), m_hOldPen(0)
{
    _mRefCnt = 0;
}

// 0x1005b3a0 ?QueryInterface@CMBWinDraw@@UAEJABUMBGUID@@PAPAX@Z
long CMBWinDraw::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBDraw)
        p = this;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1005b470 ?AddRef@CMBWinDraw@@UAEKXZ
unsigned long CMBWinDraw::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1005b480 ?Release@CMBWinDraw@@UAEKXZ
unsigned long CMBWinDraw::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1005b4b0 ??1CMBWinDraw@@QAE@XZ
CMBWinDraw::~CMBWinDraw(void)
{
    EndDrawing();
}

// 0x1005b4e0 ?BeginDrawing@CMBWinDraw@@UAEJPAVIMBUnknown@@@Z
long CMBWinDraw::BeginDrawing(IMBUnknown *target)
{
    mWinDCPtr = target;
    if (mWinDCPtr) {
        if (mWinDCPtr->GetDC(&m_hDC, 2))
            return 0;
        mWinDCPtr.Release();
    }
    return -1;
}

// 0x1005b580 ?EndDrawing@CMBWinDraw@@UAEJXZ
long CMBWinDraw::EndDrawing(void)
{
    if (m_hPen)
        ReleasePen();
    if (m_hBrush)
        ReleaseBrush();
    if (m_hDC && mWinDCPtr) {
        mWinDCPtr->ReleaseDC(m_hDC);
        mWinDCPtr.Release();
    }
    return 0;
}

// 0x1005b5d0 ?SetPenInfo@CMBWinDraw@@UAEJKABVMBColor@@@Z
long CMBWinDraw::SetPenInfo(unsigned long penSize, const MBColor &penColor)
{
    CreatePen(penSize, penColor);
    return 0;
}

// 0x1005b5f0 ?SetPenMode@CMBWinDraw@@UAEJK@Z
long CMBWinDraw::SetPenMode(unsigned long penMode)
{
    return -1;
}

// 0x1005b600 ?SetFillColor@CMBWinDraw@@UAEJABVMBColor@@@Z
long CMBWinDraw::SetFillColor(const MBColor &fillColor)
{
    CreateBrush(fillColor);
    return 0;
}

// 0x1005b610 ?FrameRect@CMBWinDraw@@UAEJABVMBRect@@@Z
long CMBWinDraw::FrameRect(const MBRect &rect)
{
    if (m_hDC) {
        MoveToEx(m_hDC, rect.left, rect.top, 0);
        LineTo(m_hDC, rect.right, rect.top);
        LineTo(m_hDC, rect.right, rect.bottom);
        LineTo(m_hDC, rect.left, rect.bottom);
        LineTo(m_hDC, rect.left, rect.top);
    }
    return 0;
}

// 0x1005b680 ?FillRect@CMBWinDraw@@UAEJABVMBRect@@@Z
long CMBWinDraw::FillRect(const MBRect &rect)
{
    if (m_hDC && m_hBrush) {
        tagRECT winRect;
        winRect.left = rect.left;
        winRect.top = rect.top;
        winRect.right = rect.right;
        winRect.bottom = rect.bottom;
        ::FillRect(m_hDC, &winRect, m_hBrush);
    }
    return 0;
}

// 0x1005b890 ?FrameEllipse@CMBWinDraw@@UAEJABVMBRect@@@Z
long CMBWinDraw::FrameEllipse(const MBRect &boundingRect)
{
    if (m_hDC) {
        long midX = boundingRect.left + (boundingRect.right - boundingRect.left) / 2;
        Arc(m_hDC, boundingRect.left, boundingRect.top, boundingRect.right, boundingRect.bottom,
            midX, boundingRect.top, midX, boundingRect.top);
    }
    return 0;
}

// 0x1005bb60 ?FillEllipse@CMBWinDraw@@UAEJABVMBRect@@@Z
long CMBWinDraw::FillEllipse(const MBRect &boundingRect)
{
    if (m_hDC && m_hBrush) {
        HGDIOBJ oldPen = SelectObject(m_hDC, ::CreatePen(PS_NULL, 0, 0));
        Ellipse(m_hDC, boundingRect.left, boundingRect.top, boundingRect.right, boundingRect.bottom);
        DeleteObject(SelectObject(m_hDC, oldPen));
    }
    return 0;
}

// 0x1005bbd0 ?CreateBrush@CMBWinDraw@@QAEXABVMBColor@@@Z
void CMBWinDraw::CreateBrush(const MBColor &color)
{
    ReleaseBrush();
    m_hBrush = CreateSolidBrush(RGB(color.GetRed(), color.GetGreen(), color.GetBlue()));
    if (m_hDC)
        m_hOldBrush = (HBRUSH)SelectObject(m_hDC, m_hBrush);
}

// 0x1005bc20 ?ReleaseBrush@CMBWinDraw@@QAEXXZ
void CMBWinDraw::ReleaseBrush(void)
{
    if (m_hBrush) {
        if (m_hDC && m_hOldBrush)
            SelectObject(m_hDC, m_hOldBrush);
        DeleteObject(m_hBrush);
    }
    m_hOldBrush = 0;
    m_hBrush = 0;
}

// 0x1005bc60 ?CreatePen@CMBWinDraw@@QAEXKABVMBColor@@@Z
void CMBWinDraw::CreatePen(unsigned long penSize, const MBColor &color)
{
    ReleasePen();
    m_hPen = ::CreatePen(PS_SOLID, penSize, RGB(color.GetRed(), color.GetGreen(), color.GetBlue()));
    if (m_hDC)
        m_hOldPen = (HPEN)SelectObject(m_hDC, m_hPen);
}

// 0x1005bcc0 ?ReleasePen@CMBWinDraw@@QAEXXZ
void CMBWinDraw::ReleasePen(void)
{
    if (m_hPen) {
        if (m_hDC && m_hOldPen)
            SelectObject(m_hDC, m_hOldPen);
        DeleteObject(m_hPen);
    }
    m_hOldPen = 0;
    m_hPen = 0;
}


// Matched by this code:
// 0x1005b6d0 _$E2
// 0x1005b6e0 _$E1
// 0x1005b7b0 _$E5
// 0x1005b7c0 _$E4
// 0x1005b8e0 _$E8
// 0x1005b8f0 _$E7
// 0x1005b9c0 _$E11
// 0x1005b9d0 _$E10
// 0x1005ba90 _$E14
// 0x1005baa0 _$E13
// 0x1005bd00 _$E17
// 0x1005bd10 _$E16
// 0x1005bdc0 _$E20
// 0x1005bdd0 _$E19
// 0x1005be80 _$E23
// 0x1005be90 _$E22
// 0x1005bf40 _$E26
// 0x1005bf50 _$E25
// 0x1005c010 ?Release@?$MBComPtr@VIMBWinDC@@@@QAEXXZ
// 0x1005c030 _$E29
// 0x1005c040 _$E28
// 0x1005c100 _$E32
// 0x1005c110 _$E31
// 0x1005c1c0 _$E35
// 0x1005c1d0 _$E34
// 0x1005c280 _$E38
// 0x1005c290 _$E37
// 0x1005c340 _$E41
// 0x1005c350 _$E40
// 0x1005c400 _$E44
// 0x1005c410 _$E43
// 0x1005c4c0 _$E47
// 0x1005c4d0 _$E46
// 0x1005c580 ?AddRef@?$MBComPtr@VIMBWinDC@@@@QAEXXZ
