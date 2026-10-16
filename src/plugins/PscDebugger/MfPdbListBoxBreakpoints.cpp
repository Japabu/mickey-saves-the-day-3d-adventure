// PscDebugger.dll 0x10007500-0x10007c85: the list of breakpoints in the
// breakpoint dialog.
#include "PscDebugger.h"

// Size of the breakpoint glyphs.
enum { kGlyphSize = 11 };

// 0x10007500 ??0MfPdbListBoxBreakpoints@@QAE@XZ
MfPdbListBoxBreakpoints::MfPdbListBoxBreakpoints()
    : mItemHeight(-1)
{
    PscDebuggerContext context;
    mEnabled.LoadBitmap(0x73);
    mDisabled.LoadBitmap(0x74);
}

// 0x100075e0 ??_GMfPdbListBoxBreakpoints@@UAEPAXI@Z

// 0x10007600 ??1MfPdbListBoxBreakpoints@@UAE@XZ
MfPdbListBoxBreakpoints::~MfPdbListBoxBreakpoints()
{
}

// 0x10007690 ??_GCObject@@UAEPAXI@Z
// 0x100076b0 ??1CObject@@UAE@XZ

// 0x100076c0 ?DrawGlyph@MfPdbListBoxBreakpoints@@QAEXPAVCDC@@PBUtagRECT@@PAVMfPdbSetBreakpoint@@@Z
void MfPdbListBoxBreakpoints::DrawGlyph(CDC *dc, LPCRECT rect, MfPdbSetBreakpoint *bp)
{
    CDC memDC;
    memDC.CreateCompatibleDC(dc);
    CBitmap *old;
    if (bp->mEnabled)
        old = memDC.SelectObject(&mEnabled);
    else
        old = memDC.SelectObject(&mDisabled);
    int x = rect->left + (mItemHeight - kGlyphSize) / 2;
    int y = rect->top + (mItemHeight - kGlyphSize) / 2;
    dc->BitBlt(x, y, kGlyphSize, kGlyphSize, &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(old);
}

// 0x100077d0 ?_GetBaseMessageMap@MfPdbListBoxBreakpoints@@KGPBUAFX_MSGMAP@@XZ
// 0x100077e0 ?GetMessageMap@MfPdbListBoxBreakpoints@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbListBoxBreakpoints, CListBox)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// 0x100077f0 ?CompareItem@MfPdbListBoxBreakpoints@@UAEHPAUtagCOMPAREITEMSTRUCT@@@Z
// 0x10007800 ?MeasureItem@MfPdbListBoxBreakpoints@@UAEXPAUtagMEASUREITEMSTRUCT@@@Z

// 0x10007810 ?DrawItem@MfPdbListBoxBreakpoints@@UAEXPAUtagDRAWITEMSTRUCT@@@Z
void MfPdbListBoxBreakpoints::DrawItem(LPDRAWITEMSTRUCT dis)
{
    if (mItemHeight < 0)
        mItemHeight = dis->rcItem.bottom - dis->rcItem.top;
    CDC *dc = CDC::FromHandle(dis->hDC);
    MfPdbSetBreakpoint *bp = (MfPdbSetBreakpoint *)GetItemDataPtr(dis->itemID);
    if (dis->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) {
        CRect rect(&dis->rcItem);
        MfString text(bp->mLine);
        CString module;
        GetText(dis->itemID, module);
        text << ", " << module;
        if (dis->itemAction & ODA_DRAWENTIRE)
            DrawGlyph(dc, &rect, bp);
        rect.left += mItemHeight;
        COLORREF background = dc->GetBkColor();
        COLORREF foreground = dc->GetTextColor();
        if (dis->itemState & ODS_SELECTED) {
            CBrush brush(::GetSysColor(COLOR_HIGHLIGHT));
            dc->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
            dc->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
            dc->FillRect(&rect, &brush);
        } else if (dis->itemAction & ODA_SELECT) {
            CBrush brush(background);
            dc->FillRect(&rect, &brush);
        }
        dc->TextOut(rect.left, rect.top, text, text.Length());
        if (dis->itemState & ODS_SELECTED) {
            dc->SetTextColor(foreground);
            dc->SetBkColor(background);
        }
    }
    if (dis->itemAction & ODA_FOCUS) {
        CRect rect(&dis->rcItem);
        rect.left += mItemHeight;
        dc->DrawFocusRect(&rect);
    }
}

// 0x10007a80 ?OnLButtonDown@MfPdbListBoxBreakpoints@@IAEXIVCPoint@@@Z
void MfPdbListBoxBreakpoints::OnLButtonDown(UINT flags, CPoint point)
{
    if (point.x < mItemHeight) {
        int top = GetTopIndex();
        int count = GetCount();
        if (count == LB_ERR || top == LB_ERR)
            return;
        int index = point.y / mItemHeight + top;
        if (index < count) {
            MfPdbSetBreakpoint *bp = (MfPdbSetBreakpoint *)GetItemDataPtr(index);
            pdbDebugger->mBreakpoints->ToggleEnabled(bp);
            CRect rect;
            GetItemRect(index, &rect);
            CDC *dc = GetDC();
            DrawGlyph(dc, &rect, bp);
            ReleaseDC(dc);
        }
    } else {
        CListBox::OnLButtonDown(flags, point);
    }
}

// 0x10007b60 ??_GCGdiObject@@UAEPAXI@Z
// 0x10007b80 ??1CGdiObject@@UAE@XZ
// 0x10007bd0 ??1CBrush@@UAE@XZ
// 0x10007c20 ??_GCBitmap@@UAEPAXI@Z
// 0x10007c40 ??1CBitmap@@UAE@XZ
