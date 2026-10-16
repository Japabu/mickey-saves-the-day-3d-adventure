// PscDebugger.dll 0x10008430-0x10009885: the window of the variable dialog
// that shows a variable as a tree of names and values.
#include "PscDebugger.h"

// 0x10016d04
const char *pszVarWndClass = "PiccoloVarWnd";
// The width of the name column, kept between the dialogs (0x10016e48).
int pdbVarDisplayState = -1;
// 0x10016f8c
char pdbVarDisplayName[4];

// 0x10008430 ??0MfPdbVarDispWnd@@QAE@HPAVCWnd@@@Z
MfPdbVarDispWnd::MfPdbVarDispWnd(int nameWidth, CWnd *parent)
    : mNameWidth(125), mTopLine(1), mWheelDelta(0), mSplitting(0), mDialog((MfPdbViewVariable *)parent),
      mSelected(0)
{
    if (nameWidth > 0)
        mNameWidth = nameWidth;
    mFont.CreatePointFont(100, "Courier", NULL);
    LOGFONT lf;
    lf.lfHeight = 80;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_BOLD;
    lf.lfItalic = 0;
    lf.lfUnderline = 0;
    lf.lfStrikeOut = 0;
    lf.lfCharSet = 0;
    lf.lfOutPrecision = 0;
    lf.lfClipPrecision = 0;
    lf.lfQuality = 0;
    lf.lfPitchAndFamily = 0;
    strcpy(lf.lfFaceName, "System");
    mHeaderFont.CreatePointFontIndirect(&lf, NULL);
    mItems = new MfDblList;
    mRows = new MfDblList;
    {
        PscDebuggerContext context;
        mMinus.LoadBitmap(0x79);
        mPlus.LoadBitmap(0x7d);
    }
    mSplitCursor = LoadCursor(hPscDebugger, MAKEINTRESOURCE(0x7f));
    mArrow = LoadCursor(NULL, IDC_ARROW);
}

// 0x10008640 ??_GMfPdbVarDispWnd@@UAEPAXI@Z

// 0x10008660 ??1MfPdbVarDispWnd@@UAE@XZ
MfPdbVarDispWnd::~MfPdbVarDispWnd()
{
    MfPdbVarWndItem::VwiListItemInterface lii;
    mItems->Clear(lii);
    delete mItems;
    mRows->Clear();
    DestroyCursor(mSplitCursor);
}

// 0x10008770 ?GetItemRect@MfPdbVarDispWnd@@QAEHPAVMfPdbVarWndItem@@PAUtagRECT@@@Z
int MfPdbVarDispWnd::GetItemRect(MfPdbVarWndItem *item, LPRECT rect)
{
    int line = 0;
    MfDblListItem *pos;
    MfPdbVarWndItem *row = (MfPdbVarWndItem *)mRows->GetFirst(pos);
    while (row && item != row) {
        line += row->mLines;
        row = (MfPdbVarWndItem *)mRows->GetNext(pos);
    }
    if (!row)
        return 0;
    int visible = 0;
    CRect client;
    GetClientRect(&client);
    *rect = client;
    rect->top = (line - mTopLine + 1) * mLineHeight + client.top + mHeaderHeight;
    rect->bottom = item->mLines * mLineHeight + rect->top;
    client.top = mHeaderHeight;
    if (IntersectRect(&client, &client, rect))
        visible = 1;
    return visible;
}

// 0x10008840 ?CountLines@MfPdbVarDispWnd@@QAEHXZ
int MfPdbVarDispWnd::CountLines()
{
    int lines = 0;
    MfDblListItem *pos;
    for (MfPdbVarWndItem *row = (MfPdbVarWndItem *)mRows->GetFirst(pos); row;
         row = (MfPdbVarWndItem *)mRows->GetNext(pos))
        lines += row->mLines;
    return lines;
}

// 0x10008880 ?SetVariable@MfPdbVarDispWnd@@QAEXPAVMfScDataHandle@@ABVMfString@@HH@Z
void MfPdbVarDispWnd::SetVariable(MfScDataHandle *value, const MfString &name, int found, int redraw)
{
    MfPdbVarWndItem::VwiListItemInterface lii;
    MfDblListItem *pos = mItems->Find((void *)&name, lii);
    if (pos) {
        MfPdbVarWndItem *old = (MfPdbVarWndItem *)mItems->Delete(pos);
        if (old)
            delete old;
    }
    MfPdbVarWndItem *item = new MfPdbVarWndItem(name, value, !found, this, 0, 0);
    mItems->Add(item);
    mRows->Add(item);
    if (redraw) {
        CRect rect;
        GetClientRect(&rect);
        InvalidateRect(&rect, FALSE);
    }
}

// 0x10008970 ?Clear@MfPdbVarDispWnd@@QAEXH@Z
void MfPdbVarDispWnd::Clear(int redraw)
{
    MfPdbVarWndItem::VwiListItemInterface lii;
    mItems->Clear(lii);
    mRows->Clear();
    mSelected = 0;
    if (redraw) {
        CRect rect;
        GetClientRect(&rect);
        InvalidateRect(&rect, FALSE);
    }
}

// 0x100089d0 ?GetMessageMap@MfPdbVarDispWnd@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbVarDispWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_VSCROLL()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEWHEEL()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_KEYDOWN()
    ON_WM_GETDLGCODE()
    ON_WM_CHAR()
END_MESSAGE_MAP()

// 0x100089e0 ?Initialize@MfPdbVarDispWnd@@SAXXZ
void MfPdbVarDispWnd::Initialize()
{
}

// 0x100089f0 ?Select@MfPdbVarDispWnd@@QAEXPAVMfDblListItem@@H@Z
void MfPdbVarDispWnd::Select(MfDblListItem *pos, int redraw)
{
    if (pos) {
        mSelected = pos;
        MfPdbVarWndItem *item = (MfPdbVarWndItem *)mRows->GetCurrent(mSelected);
        item->mFlags |= MfPdbVarWndItem::kSelected;
        if (redraw)
            item->Invalidate(this);
    }
}

// 0x10008a30 ?Deselect@MfPdbVarDispWnd@@QAEXPAVMfDblListItem@@H@Z
void MfPdbVarDispWnd::Deselect(MfDblListItem *, int redraw)
{
    if (mSelected) {
        MfPdbVarWndItem *item = (MfPdbVarWndItem *)mRows->GetCurrent(mSelected);
        item->mFlags &= ~MfPdbVarWndItem::kSelected;
        if (redraw)
            item->Invalidate(this);
    }
}

// 0x10008a70 ?PreCreateWindow@MfPdbVarDispWnd@@MAEHAAUtagCREATESTRUCTA@@@Z
BOOL MfPdbVarDispWnd::PreCreateWindow(CREATESTRUCT &cs)
{
    cs.lpszClass = pszVarWndClass;
    cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, NULL, (HBRUSH)COLOR_WINDOW, NULL);
    cs.style |= WS_VSCROLL;
    return CWnd::PreCreateWindow(cs);
}

// 0x10008ab0 ?PreTranslateMessage@MfPdbVarDispWnd@@MAEHPAUtagMSG@@@Z
BOOL MfPdbVarDispWnd::PreTranslateMessage(MSG *msg)
{
    return CWnd::PreTranslateMessage(msg);
}

// 0x10008ac0 ?OnCreate@MfPdbVarDispWnd@@IAEHPAUtagCREATESTRUCTA@@@Z
int MfPdbVarDispWnd::OnCreate(LPCREATESTRUCT cs)
{
    if (CWnd::OnCreate(cs) == -1)
        return -1;
    CDC *dc = GetDC();
    TEXTMETRIC tm;
    CFont *old = dc->SelectObject(&mHeaderFont);
    dc->GetTextMetrics(&tm);
    mHeaderHeight = tm.tmHeight + 4;
    dc->SelectObject(old);
    old = dc->SelectObject(&mFont);
    dc->GetTextMetrics(&tm);
    mLineHeight = tm.tmHeight + 4;
    mCharWidth = tm.tmAveCharWidth;
    dc->SelectObject(old);
    ReleaseDC(dc);
    SetScrollRange(SB_VERT, mTopLine, 1, FALSE);
    SetScrollPos(SB_VERT, mTopLine, TRUE);
    return 0;
}

// 0x10008b90 ?OnPaint@MfPdbVarDispWnd@@IAEXXZ
void MfPdbVarDispWnd::OnPaint()
{
    CPaintDC dc(this);
    COLORREF oldBackground = dc.GetBkColor();
    CRect client;
    GetClientRect(&client);
    dc.SetBkColor(GetSysColor(COLOR_WINDOW));
    dc.Draw3dRect(0, 0, mNameWidth, mHeaderHeight, 0xffffff, 0);
    dc.Draw3dRect(mNameWidth, 0, client.Width() - mNameWidth, mHeaderHeight, 0xffffff, 0);
    dc.SetBkColor(GetSysColor(COLOR_WINDOW));
    HGDIOBJ pen = GetStockObject(BLACK_PEN);
    if (pen)
        ::SelectObject(dc.m_hDC, pen);
    CFont *oldFont = dc.SelectObject(&mHeaderFont);
    dc.SetTextColor(0);
    dc.SetBkMode(TRANSPARENT);
    ::TextOut(dc.m_hDC, 4, 2, "Name", 4);
    ::TextOut(dc.m_hDC, mNameWidth + 4, 2, "Value", 5);
    dc.SetBkMode(OPAQUE);
    dc.SelectObject(oldFont);
    oldFont = dc.SelectObject(&mFont);
    CRect rect = client;
    MfDblListItem *pos;
    for (MfPdbVarWndItem *item = (MfPdbVarWndItem *)mRows->GetFirst(pos); item;
         item = (MfPdbVarWndItem *)mRows->GetNext(pos))
        item->Draw(this, &dc, 0, &rect);
    rect.top = rect.bottom;
    rect.bottom = client.bottom;
    dc.FillSolidRect(&rect, dc.GetBkColor());
    dc.SelectObject(oldFont);
}

// 0x10008d90 ?OnSize@MfPdbVarDispWnd@@IAEXIHH@Z
void MfPdbVarDispWnd::OnSize(UINT type, int cx, int cy)
{
    CWnd::OnSize(type, cx, cy);
}

// EQUIV 0x10008da0 ?OnKeyDown@MfPdbVarDispWnd@@IAEXIII@Z: reviewed: register choice and operand order of one integer sum only (the original computes mLines + line - visible + 1 in ecx with add/sub/inc and ors in 4 as a dword, this one in eax via lea and or al, 4); same value, calls and branches
void MfPdbVarDispWnd::OnKeyDown(UINT ch, UINT, UINT)
{
    int shift = GetKeyState(VK_SHIFT) < 0;
    switch (ch) {
    case VK_UP: {
        if (!mSelected)
            mRows->GetFirst(mSelected);
        if (!mSelected)
            break;
        Deselect(mSelected, 1);
        mRows->GetPrev(mSelected);
        if (!mSelected)
            mRows->GetFirst(mSelected);
        MfPdbVarWndItem *item = (MfPdbVarWndItem *)mRows->GetCurrent(mSelected);
        CRect rect;
        GetItemRect(item, &rect);
        if (rect.top < mHeaderHeight) {
            MfDblListItem *pos;
            MfPdbVarWndItem *row = (MfPdbVarWndItem *)mRows->GetFirst(pos);
            int line = 0;
            while (pos && row != item) {
                line += row->mLines;
                row = (MfPdbVarWndItem *)mRows->GetNext(pos);
            }
            PostMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, line + 1));
        }
        Select(mSelected, 1);
        break;
    }
    case VK_DOWN: {
        if (!mSelected)
            mRows->GetFirst(mSelected);
        if (!mSelected)
            break;
        Deselect(mSelected, 1);
        mRows->GetNext(mSelected);
        if (!mSelected)
            mRows->GetLast(mSelected);
        MfPdbVarWndItem *item = (MfPdbVarWndItem *)mRows->GetCurrent(mSelected);
        CRect rect;
        GetItemRect(item, &rect);
        CRect client;
        GetClientRect(&client);
        if (rect.bottom > client.bottom) {
            MfDblListItem *pos;
            MfPdbVarWndItem *row = (MfPdbVarWndItem *)mRows->GetFirst(pos);
            int line = 0;
            while (pos && row != item) {
                line += row->mLines;
                row = (MfPdbVarWndItem *)mRows->GetNext(pos);
            }
            int visible = (client.bottom - mHeaderHeight - client.top) / mLineHeight;
            int top = item->mLines + line;
            top = top - visible + 1;
            PostMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, top));
        }
        Select(mSelected, 1);
        break;
    }
    case VK_ESCAPE:
        if (mDialog)
            mDialog->EndDialog(0);
        break;
    case VK_TAB:
        if (mDialog) {
            if (shift)
                mDialog->FocusPrevious(this);
            else
                mDialog->FocusNext(this);
        }
        break;
    }
}

// 0x10009020 ?OnChar@MfPdbVarDispWnd@@IAEXIII@Z
void MfPdbVarDispWnd::OnChar(UINT ch, UINT, UINT)
{
    int changed = 0;
    switch (ch) {
    case '+':
        if (mSelected)
            changed = ExpandItem(mSelected);
        break;
    case '-':
        if (mSelected)
            changed = CollapseItem(mSelected);
        break;
    }
    if (changed) {
        UpdateScrollRange();
        CRect rect;
        GetClientRect(&rect);
        InvalidateRect(&rect, FALSE);
    }
}

// 0x10009090 ?ExpandItem@MfPdbVarDispWnd@@QAEHPAVMfDblListItem@@@Z
int MfPdbVarDispWnd::ExpandItem(MfDblListItem *pos)
{
    int expanded = 0;
    MfPdbVarWndItem *item = (MfPdbVarWndItem *)mRows->GetCurrent(pos);
    if (item->mFlags & MfPdbVarWndItem::kCollapsed) {
        item->Expand(this);
        MfDblList *children = item->mChildren;
        MfDblListItem *childPos;
        for (MfPdbVarWndItem *child = (MfPdbVarWndItem *)children->GetFirst(childPos); child;
             child = (MfPdbVarWndItem *)children->GetNext(childPos)) {
            mRows->Insert(child, MfDblList::LP_AFTER, pos);
            mRows->GetNext(pos);
        }
        expanded = 1;
    }
    return expanded;
}

// 0x10009110 ?CollapseItem@MfPdbVarDispWnd@@QAEHPAVMfDblListItem@@@Z
int MfPdbVarDispWnd::CollapseItem(MfDblListItem *pos)
{
    int collapsed = 0;
    MfPdbVarWndItem *item = (MfPdbVarWndItem *)mRows->GetCurrent(pos);
    if (item->mFlags & MfPdbVarWndItem::kExpanded) {
        MfPdbVarWndItem *row = (MfPdbVarWndItem *)mRows->GetNext(pos);
        while (row && row->mDepth > item->mDepth) {
            mRows->Delete(pos);
            row = (MfPdbVarWndItem *)mRows->GetCurrent(pos);
        }
        item->Collapse();
        collapsed = 1;
    }
    return collapsed;
}

// 0x10009180 ?UpdateScrollRange@MfPdbVarDispWnd@@QAEXXZ
void MfPdbVarDispWnd::UpdateScrollRange()
{
    CRect rect;
    GetClientRect(&rect);
    int visible = (rect.bottom - mHeaderHeight - rect.top) / mLineHeight + 1;
    int lines = CountLines();
    if (lines >= visible)
        SetScrollRange(SB_VERT, 1, lines, TRUE);
    else
        SetScrollRange(SB_VERT, 1, 1, TRUE);
    if (mTopLine > lines)
        mTopLine = lines;
    else if (mTopLine < 1)
        mTopLine = 1;
}

// 0x100091f0 ?OnLButtonDown@MfPdbVarDispWnd@@IAEXIVCPoint@@@Z
void MfPdbVarDispWnd::OnLButtonDown(UINT, CPoint point)
{
    SetFocus();
    if (point.y < mHeaderHeight) {
        if (point.x >= mNameWidth - 1 && point.x <= mNameWidth + 1) {
            mSplitting = 1;
            SetCapture();
            DrawSplitter(point.x, point.y, 0, 0);
            SetSplitCursor(point.x, point.y);
        }
        return;
    }
    int redraw = 0;
    MfDblListItem *pos;
    MfPdbVarWndItem *item = (MfPdbVarWndItem *)mRows->GetFirst(pos);
    for (int done = 0; item && !done; item = (MfPdbVarWndItem *)mRows->GetNext(pos)) {
        int hit = item->HitTest(point, this);
        if (hit == MfPdbVarWndItem::kHitExpand) {
            Deselect(mSelected, done);
            mSelected = pos;
            Select(pos, done);
            ExpandItem(mSelected);
            done = 1;
            redraw = done;
        } else if (hit == MfPdbVarWndItem::kHitCollapse) {
            Deselect(mSelected, 0);
            mSelected = pos;
            Select(pos, 0);
            CollapseItem(mSelected);
            done = 1;
            redraw = done;
        } else if (hit) {
            Deselect(mSelected, 1);
            mSelected = pos;
            Select(pos, 1);
            done = 1;
        }
    }
    UpdateScrollRange();
    if (redraw) {
        CRect rect;
        GetClientRect(&rect);
        InvalidateRect(&rect, FALSE);
    }
}

// 0x10009380 ?OnLButtonUp@MfPdbVarDispWnd@@IAEXIVCPoint@@@Z
void MfPdbVarDispWnd::OnLButtonUp(UINT, CPoint point)
{
    if (mSplitting) {
        mSplitting = 0;
        ReleaseCapture();
        DrawSplitter(point.x, point.y, 1, 1);
        SetSplitCursor(point.x, point.y);
        mNameWidth = point.x;
        CRect rect;
        GetClientRect(&rect);
        int bottom = rect.bottom;
        rect.bottom = mHeaderHeight;
        InvalidateRect(&rect, TRUE);
        rect.bottom = bottom;
        rect.top = mHeaderHeight;
        InvalidateRect(&rect, FALSE);
    }
}

// EQUIV 0x10009410 ?OnVScroll@MfPdbVarDispWnd@@IAEXIIPAVCScrollBar@@@Z: reviewed: register choice only (the original reads mTopLine from memory again for the final compare, this one keeps the first load in eax); same calls, arguments and branches
void MfPdbVarDispWnd::OnVScroll(UINT code, UINT pos, CScrollBar *)
{
    int delta = 0;
    CRect rect;
    GetClientRect(&rect);
    int visible = (rect.bottom - mHeaderHeight - rect.top) / mLineHeight + 1;
    int lines = CountLines();
    switch (code) {
    case SB_TOP:
        delta = 1 - mTopLine;
        break;
    case SB_BOTTOM:
        delta = lines - mTopLine;
        break;
    case SB_LINEUP:
        delta = -1;
        break;
    case SB_LINEDOWN:
        delta = 1;
        break;
    case SB_PAGEUP:
        delta = -(visible / 2);
        break;
    case SB_PAGEDOWN:
        delta = visible / 2;
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        delta = pos - mTopLine;
        break;
    }
    delta += mTopLine;
    if (delta < 1)
        delta = 1;
    else if (delta > lines)
        delta = lines;
    if (delta != mTopLine) {
        mTopLine = delta;
        SetScrollPos(SB_VERT, delta, TRUE);
        InvalidateRect(&rect, FALSE);
    }
}

// 0x10009500 ?SetSplitCursor@MfPdbVarDispWnd@@QAEXHH@Z
void MfPdbVarDispWnd::SetSplitCursor(int x, int y)
{
    if (y >= 0 && y < mHeaderHeight && x >= mNameWidth - 1 && x <= mNameWidth + 1)
        SetCursor(mSplitCursor);
    else
        SetCursor(mArrow);
}

// 0x10009550 ?DrawSplitter@MfPdbVarDispWnd@@QAEXHHHH@Z
void MfPdbVarDispWnd::DrawSplitter(int x, int y, int erase, int keep)
{
    CDC *dc = GetDC();
    CRect rect;
    GetClientRect(&rect);
    CPen pen(PS_SOLID, 2, 0xffffff);
    int oldRop = dc->SetROP2(R2_XORPEN);
    CPen *oldPen = dc->SelectObject(&pen);
    if (erase) {
        dc->MoveTo(mSplitX, rect.top);
        dc->LineTo(mSplitX, rect.bottom);
    }
    if (!keep) {
        dc->MoveTo(x, rect.top);
        dc->LineTo(x, rect.bottom);
    }
    dc->SetROP2(oldRop);
    dc->SelectObject(oldPen);
    ReleaseDC(dc);
    mSplitX = x;
    mSplitY = y;
}

// 0x10009680 ?OnMouseMove@MfPdbVarDispWnd@@IAEXIVCPoint@@@Z
void MfPdbVarDispWnd::OnMouseMove(UINT, CPoint point)
{
    if (mSplitting) {
        DrawSplitter(point.x, point.y, 1, 0);
        SetCursor(mSplitCursor);
    } else {
        SetSplitCursor(point.x, point.y);
    }
}

// 0x100096d0 ?OnMouseWheel@MfPdbVarDispWnd@@IAEHIFVCPoint@@@Z
BOOL MfPdbVarDispWnd::OnMouseWheel(UINT, short delta, CPoint)
{
    mWheelDelta += delta;
    int lines = -mWheelDelta * 2 / WHEEL_DELTA;
    if (lines) {
        mWheelDelta += lines * (WHEEL_DELTA / 2);
        PostMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, GetScrollPos(SB_VERT) + lines));
    }
    return TRUE;
}

// 0x10009740 ?OnSetFocus@MfPdbVarDispWnd@@IAEXPAVCWnd@@@Z
void MfPdbVarDispWnd::OnSetFocus(CWnd *old)
{
    if (mDialog) {
        CWnd *ok = mDialog->GetDlgItem(IDOK);
        if (old && ok->m_hWnd == old->m_hWnd) {
            mDialog->FocusPrevious(this);
            return;
        }
    }
    if (!mSelected)
        mRows->GetFirst(mSelected);
    if (!mSelected)
        return;
    mRows->GetCurrent(mSelected);
    Select(mSelected, 1);
}

// 0x100097b0 ?OnKillFocus@MfPdbVarDispWnd@@IAEXPAVCWnd@@@Z
void MfPdbVarDispWnd::OnKillFocus(CWnd *)
{
    if (mSelected)
        Deselect(mSelected, 1);
}

// The same bytes as MfPdbViewVariableEdit::OnGetDlgCode (0x1000b3c0), into
// which the linker folded it.
UINT MfPdbVarDispWnd::OnGetDlgCode()
{
    return DLGC_WANTALLKEYS | DLGC_WANTCHARS;
}

// 0x100097d0 ??1CPen@@UAE@XZ
// 0x10009820 ??_GCFont@@UAEPAXI@Z
// 0x10009840 ??1CFont@@UAE@XZ
