// PiccoloConsole.dll 0x100113e0-0x10012b9f: MfPeDrawingContext, the text
// drawing and geometry of the code editor.
#include "PiccoloConsole.h"

#define LEFT_MARGIN 20

// The glyphs of the margin: the current line arrow and the call stack arrow,
// relative to the right edge and the middle of the margin.
static POINT sCurrentLineArrow[8] = {
    { 0, 0 }, { -5, -5 }, { -5, -2 }, { -10, -2 }, { -10, 2 }, { -5, 2 }, { -5, 5 }, { 0, 0 }
};
static POINT sCallStackArrow[6] = {
    { 0, 0 }, { -3, -3 }, { -11, -3 }, { -11, 3 }, { -3, 3 }, { 0, 0 }
};

// 0x100113e0 ??0MfPeDrawingContext@@QAE@PAVMfPeCodeEditor@@@Z
MfPeDrawingContext::MfPeDrawingContext(MfPeCodeEditor *editor)
    : mEditor(editor), mHasCaret(0), mCaretInMargin(0), mCaretInSelection(0),
      mDragging(0), mCharWidth(0), mCharHeight(0), mLineSpacing(0), mTopLine(1), mLeftColumn(1),
      mMarginWidth(LEFT_MARGIN)
{
    mCaretPos.x = LEFT_MARGIN;
    mCaretPos.y = 0;
    mFont.CreatePointFont(100, "Courier");
    mMarginCursor = LoadCursor(hPiccoloConsoleResources, MAKEINTRESOURCE(105));
    mDropCursor = LoadCursor(hPiccoloConsoleResources, MAKEINTRESOURCE(107));
    mNoDropCursor = LoadCursor(hPiccoloConsoleResources, MAKEINTRESOURCE(106));
    mTextCursor = LoadCursor(NULL, IDC_IBEAM);
    mArrowCursor = LoadCursor(NULL, IDC_ARROW);
}

// 0x100114d0 ??_GMfPeDrawingContext@@UAEPAXI@Z

// 0x100114f0 ??1MfPeDrawingContext@@UAE@XZ
MfPeDrawingContext::~MfPeDrawingContext()
{
    DestroyCursor(mMarginCursor);
    DestroyCursor(mDropCursor);
    DestroyCursor(mNoDropCursor);
}

// 0x10011590 ?ResetScrollRanges@MfPeDrawingContext@@QAEXH@Z
void MfPeDrawingContext::ResetScrollRanges(int lines)
{
    if (lines <= 1)
        lines = 2;
    mTopLine = 1;
    mEditor->SetScrollRange(SB_VERT, 1, lines, FALSE);
    mEditor->SetScrollPos(SB_VERT, mTopLine);
    mLeftColumn = 1;
    mEditor->SetScrollRange(SB_HORZ, 1, 255, FALSE);
    mEditor->SetScrollPos(SB_HORZ, mLeftColumn);
}

// 0x10011600 ?SetLineCount@MfPeDrawingContext@@QAEXH@Z
void MfPeDrawingContext::SetLineCount(int lines)
{
    if (lines <= 1)
        lines = 2;
    mEditor->SetScrollRange(SB_VERT, 1, lines, FALSE);
    mEditor->SetScrollPos(SB_VERT, mTopLine);
}

// 0x10011640 ?CanScroll@MfPeDrawingContext@@QAEHIH@Z
int MfPeDrawingContext::CanScroll(UINT message, int direction)
{
    int minPos, maxPos;

    if (message == WM_VSCROLL) {
        mEditor->GetScrollRange(SB_VERT, &minPos, &maxPos);
        if (direction == 0 && mTopLine > minPos)
            return 1;
        if (direction == 1 && mTopLine < maxPos)
            return 1;
    } else if (message == WM_HSCROLL) {
        mEditor->GetScrollRange(SB_HORZ, &minPos, &maxPos);
        if (direction == 0 && mLeftColumn > minPos)
            return 1;
        if (direction == 1 && mLeftColumn < maxPos)
            return 1;
    }
    return 0;
}

// 0x10011700 ?SetupFont@MfPeDrawingContext@@QAEXPAVCDC@@@Z
void MfPeDrawingContext::SetupFont(CDC *dc)
{
    TEXTMETRIC tm;

    CFont *oldFont = dc->SelectObject(&mFont);
    dc->GetTextMetrics(&tm);
    mCharWidth = tm.tmAveCharWidth;
    mCharHeight = tm.tmHeight;
    mLineSpacing = 0;
    dc->SelectObject(oldFont);
}

// 0x10011750 ?SetClientSize@MfPeDrawingContext@@QAEXHH@Z
void MfPeDrawingContext::SetClientSize(int width, int height)
{
    mWidth = width;
    mHeight = height;
    if ((HRGN)mUpdateRgn != NULL)
        mUpdateRgn.DeleteObject();
    mUpdateRgn.CreateRectRgn(0, 0, mWidth, mHeight);

    mVisibleLines = mFullLines = mHeight / LineHeight();
    if (mHeight % LineHeight())
        mVisibleLines++;
    mVisibleColumns = mFullColumns = (mWidth - mMarginWidth) / mCharWidth;
    if (mWidth % mCharWidth)
        mVisibleColumns++;
}

// EQUIV 0x10011800 ?PointToPosition@MfPeDrawingContext@@QAE?AVCPoint@@ABV2@@Z: reviewed: the two summed members are loaded in the other order
CPoint MfPeDrawingContext::PointToPosition(const CPoint &point)
{
    CPoint position;

    position.x = (point.x - mMarginWidth + 3) / mCharWidth + mLeftColumn;
    position.y = point.y / LineHeight() + mTopLine;
    return position;
}

// 0x10011840 ?PositionToPoint@MfPeDrawingContext@@QAE?AVCPoint@@ABV2@@Z
CPoint MfPeDrawingContext::PositionToPoint(const CPoint &position)
{
    CPoint point;

    point.x = (position.x - mLeftColumn) * mCharWidth + mMarginWidth;
    point.y = LineToY(position.y);
    return point;
}

// 0x10011880 ?ClampPoint@MfPeDrawingContext@@QAEXAAVCPoint@@@Z
void MfPeDrawingContext::ClampPoint(CPoint &point)
{
    if (point.x < mMarginWidth)
        point.x = mMarginWidth;
    else if (point.x >= mWidth)
        point.x = mWidth - 1;
    if (point.y < 0)
        point.y = 0;
    else if (point.y >= mHeight)
        point.y = mHeight - 1;
}

// 0x100118c0 ?SetDC@MfPeDrawingContext@@QAEXPAVCDC@@PBUtagRECT@@@Z
void MfPeDrawingContext::SetDC(CDC *dc, const RECT *clip)
{
    mDC = dc;
    if (clip)
        mClipRect.CopyRect(clip);
    else
        mClipRect.SetRect(0, 0, mWidth, mHeight);
    mDC->SetBkColor(GetSysColor(COLOR_WINDOW));
    mDC->SelectObject(&mFont);
    mDC->SetBkMode(OPAQUE);
}

// 0x10011930 ?InvalidateLine@MfPeDrawingContext@@QAEXH@Z
void MfPeDrawingContext::InvalidateLine(int line)
{
    CRect rect;

    GetTextRect(line, 0, mVisibleColumns, &rect);
    rect.left = mMarginWidth;
    rect.right = mWidth;
    InvalidateRect(mEditor->m_hWnd, &rect, FALSE);
}

// 0x10011980 ?InvalidateMargin@MfPeDrawingContext@@QAEXH@Z
void MfPeDrawingContext::InvalidateMargin(int line)
{
    CRect rect;

    GetMarginRect(line, &rect);
    InvalidateRect(mEditor->m_hWnd, &rect, FALSE);
}

// 0x100119c0 ?InvalidateMarginFrom@MfPeDrawingContext@@QAEXH@Z
void MfPeDrawingContext::InvalidateMarginFrom(int line)
{
    CRect rect;

    GetMarginRect(line, &rect);
    rect.bottom = mHeight;
    InvalidateRect(mEditor->m_hWnd, &rect, FALSE);
}

// EQUIV 0x10011a00 ?InvalidateFrom@MfPeDrawingContext@@QAEXH@Z: reviewed: the two summed members are loaded in the other order
void MfPeDrawingContext::InvalidateFrom(int line)
{
    CRect rect;

    rect.left = mMarginWidth;
    rect.right = mWidth;
    rect.top = LineToY(line);
    if (rect.top < 0)
        rect.top = 0;
    else if (rect.top > mHeight)
        rect.top = mHeight;
    rect.bottom = mHeight;
    InvalidateRect(mEditor->m_hWnd, &rect, FALSE);
}

// 0x10011a70 ?Update@MfPeDrawingContext@@QAEXXZ
void MfPeDrawingContext::Update()
{
    UpdateWindow(mEditor->m_hWnd);
}

// 0x10011a80 ?GetTextRect@MfPeDrawingContext@@QAEXHHHPAUtagRECT@@@Z
void MfPeDrawingContext::GetTextRect(int line, int firstColumn, int lastColumn, RECT *rect)
{
    int top = LineToY(line);
    int bottom = top + LineHeight();
    if (top < 0)
        top = 0;
    if (top > mHeight)
        top = mHeight;
    if (bottom < 0)
        bottom = 0;
    if (bottom > mHeight)
        bottom = mHeight;

    int left = firstColumn - mLeftColumn + mMarginWidth;
    int right = left + (lastColumn - firstColumn) * mCharWidth;
    if (left < mMarginWidth)
        left = mMarginWidth;
    if (left > mWidth)
        left = mWidth;
    if (right < mMarginWidth)
        right = mMarginWidth;
    if (right > mWidth)
        right = mWidth;
    if (right < left)
        right = left;
    SetRect(rect, left, top, right, bottom);
}

// EQUIV 0x10011b10 ?GetMarginRect@MfPeDrawingContext@@QAEXHPAUtagRECT@@@Z: reviewed: the two summed members are loaded in the other order
void MfPeDrawingContext::GetMarginRect(int line, RECT *rect)
{
    int top = LineToY(line);
    SetRect(rect, 0, top, mMarginWidth, top + LineHeight());
}

// 0x10011b40 ?IsTextVisible@MfPeDrawingContext@@QAEHHH@Z
int MfPeDrawingContext::IsTextVisible(int line, int column)
{
    CRect rect;

    int row = line - mTopLine;
    if (row >= 0 && row < mVisibleLines && mLeftColumn <= column) {
        GetTextRect(line, 0, column + 1, &rect);
        return IntersectRect(&rect, &rect, &mClipRect);
    }
    return 0;
}

// 0x10011ba0 ?IsMarginVisible@MfPeDrawingContext@@QAEHH@Z
int MfPeDrawingContext::IsMarginVisible(int line)
{
    CRect rect;

    int row = line - mTopLine;
    if (row >= 0 && row < mVisibleLines) {
        GetMarginRect(line, &rect);
        return IntersectRect(&rect, &rect, &mClipRect);
    }
    return 0;
}

// EQUIV 0x10011bf0 ?DrawString@MfPeDrawingContext@@QAEXABVMfString@@KHH@Z: reviewed: register choice and the order of independent loads and stores; same calls and arguments
void MfPeDrawingContext::DrawString(const MfString &text, COLORREF color, int column, int line)
{
    COLORREF background = mDC->GetBkColor();
    int skip = column - mLeftColumn;
    MfString visible;

    if (skip < 0) {
        column = mLeftColumn;
        visible = text.RightString(text.Length() + skip);
    } else {
        visible = text;
    }

    int y = LineToY(line);
    int x = (column - mLeftColumn) * mCharWidth + mMarginWidth;
    if (mSelection->mActive) {
        int offset = 0;
        int done = 0;
        int remaining = visible.Length();
        MfString part;
        while (!done) {
            int length = remaining;
            int selected = mSelection->GetRun(line, column, &length);
            if (length < visible.Length())
                part = visible.MidString(offset, length);
            else
                part = visible;
            COLORREF back;
            if (selected == 1) {
                mDC->SetTextColor(RGB(255, 255, 255));
                back = RGB(0, 0, 0);
            } else {
                mDC->SetTextColor(color);
                back = background;
            }
            mDC->SetBkColor(back);
            if (part.Length() > 0) {
                mDC->TextOut(x, y, part, part.Length());
                offset += part.Length();
                column += part.Length();
                x += part.Length() * mCharWidth;
            }
            remaining -= length;
            mDC->SetBkColor(background);
            if (length == visible.Length() || length == 0 || part.Length() == 0)
                done = 1;
        }
    } else {
        mDC->SetBkColor(background);
        mDC->SetTextColor(color);
        mDC->TextOut(x, y, visible, visible.Length());
    }
}

// EQUIV 0x10011e80 ?DrawLineEnd@MfPeDrawingContext@@QAEXHH@Z: reviewed: the two summed members are loaded in the other order
void MfPeDrawingContext::DrawLineEnd(int line, int column)
{
    COLORREF background = mDC->GetBkColor();
    int done = 0;
    int current = column + 1;
    int length;
    int remaining = mVisibleColumns;
    CRect rect;

    while (!done) {
        length = remaining;
        int selected = mSelection->GetRun(line, current, &length);
        if (current < mLeftColumn) {
            int hidden = mLeftColumn - current;
            length -= hidden;
            current += hidden;
        }
        rect.top = LineToY(line);
        rect.left = (current - mLeftColumn) * mCharWidth + mMarginWidth;
        rect.right = mWidth;
        rect.bottom = rect.top + LineHeight();
        mDC->FillSolidRect(&rect, selected ? RGB(0, 0, 0) : background);
        mDC->SetBkColor(background);
        remaining -= length;
        current += length;
        if (remaining <= 0)
            done = 1;
    }
}

// 0x10011f70 ?EraseMargin@MfPeDrawingContext@@QAEXH@Z
void MfPeDrawingContext::EraseMargin(int line)
{
    CRect rect;

    GetMarginRect(line, &rect);
    COLORREF background = mDC->GetBkColor();
    mDC->FillSolidRect(&rect, background);
    mDC->SetBkColor(background);
}

// TODO 0x10011fc0 ?DrawGlyphs@MfPeDrawingContext@@QAEXHH@Z: 89% - the Ellipse right edge is computed as left - top + bottom - 1 and the CGdiObject vtable is kept in ebp from the start
void MfPeDrawingContext::DrawGlyphs(int line, int glyphs)
{
    CRect rect;
    POINT points[8];

    GetMarginRect(line, &rect);
    if (glyphs & 2) {
        CPen pen(PS_SOLID, 1, RGB(0x84, 0, 0));
        CBrush brush(mDC->GetBkColor());
        CPen *oldPen = mDC->SelectObject(&pen);
        CBrush *oldBrush = mDC->SelectObject(&brush);
        mDC->Ellipse(rect.left + 1, rect.top + 1, rect.left + 1 + (rect.bottom - 1 - (rect.top + 1)), rect.bottom - 1);
        mDC->SelectObject(oldPen);
        mDC->SelectObject(oldBrush);
    }
    if (glyphs & 1) {
        CPen pen(PS_SOLID, 1, RGB(255, 255, 255));
        CBrush brush(RGB(0x84, 0, 0));
        CPen *oldPen = mDC->SelectObject(&pen);
        CBrush *oldBrush = mDC->SelectObject(&brush);
        mDC->Ellipse(rect.left + 1, rect.top + 1, rect.left + 1 + (rect.bottom - 1 - (rect.top + 1)), rect.bottom - 1);
        mDC->SelectObject(oldPen);
        mDC->SelectObject(oldBrush);
    }
    if (glyphs & 4) {
        int x = rect.right - 1;
        int y = (rect.top + rect.bottom) / 2;
        for (int i = 0; i < 8; i++) {
            points[i].x = sCurrentLineArrow[i].x + x;
            points[i].y = sCurrentLineArrow[i].y + y;
        }
        CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
        CBrush brush(RGB(255, 255, 0));
        CPen *oldPen = mDC->SelectObject(&pen);
        CBrush *oldBrush = mDC->SelectObject(&brush);
        mDC->Polygon(points, 8);
        mDC->SelectObject(oldPen);
        mDC->SelectObject(oldBrush);
    }
    if (glyphs & 8) {
        int x = rect.right - 1;
        int y = (rect.top + rect.bottom) / 2;
        for (int i = 0; i < 6; i++) {
            points[i].x = sCallStackArrow[i].x + x;
            points[i].y = sCallStackArrow[i].y + y;
        }
        CPen pen(PS_SOLID, 1, RGB(255, 255, 255));
        CBrush brush(RGB(0, 0, 0x84));
        CPen *oldPen = mDC->SelectObject(&pen);
        CBrush *oldBrush = mDC->SelectObject(&brush);
        mDC->Polygon(points, 6);
        mDC->SelectObject(oldPen);
        mDC->SelectObject(oldBrush);
    }
}

// EQUIV 0x10012390 ?EraseFrom@MfPeDrawingContext@@QAEXH@Z: reviewed: the two summed members are loaded in the other order
void MfPeDrawingContext::EraseFrom(int line)
{
    CRect rect;

    rect.right = mWidth;
    rect.left = 0;
    rect.top = LineToY(line);
    if (rect.top < 0)
        rect.top = 0;
    else if (rect.top > mHeight)
        rect.top = mHeight;
    rect.bottom = mHeight;
    mDC->FillSolidRect(&rect, mDC->GetBkColor());
}

// 0x10012400 ?CreateEditCaret@MfPeDrawingContext@@QAEXXZ
void MfPeDrawingContext::CreateEditCaret()
{
    CreateCaret(mEditor->m_hWnd, NULL, 2, mCharHeight);
    CWnd::SetCaretPos(mCaretPos);
    mHasCaret = 1;
    if (mCaretInSelection)
        mCaretInSelection = 0;
    ShowCaret(mEditor->m_hWnd);
    UpdateCaretForSelection();
    UpdateCaretForMargin();
}

// 0x10012460 ?DestroyEditCaret@MfPeDrawingContext@@QAEXXZ
void MfPeDrawingContext::DestroyEditCaret()
{
    CPoint position;

    HideCaret(mEditor->m_hWnd);
    GetCaretPos(&position);
    mCaretPos = position;
    DestroyCaret();
    mHasCaret = 0;
    mCaretInMargin = 0;
}

// EQUIV 0x100124a0 ?SetCaret@MfPeDrawingContext@@QAEXABVCPoint@@@Z: reviewed: the two factors of the scroll offset are computed in the other order (same product); register choice
void MfPeDrawingContext::SetCaret(const CPoint &position)
{
    mCaretPos.x = (position.x - mLeftColumn) * mCharWidth + mMarginWidth;
    mCaretPos.y = LineToY(position.y);
    if (mHasCaret) {
        CWnd::SetCaretPos(mCaretPos);
        UpdateCaretForSelection();
        UpdateCaretForMargin();
    }
}

// 0x10012500 ?UpdateCaretForSelection@MfPeDrawingContext@@QAEXXZ
void MfPeDrawingContext::UpdateCaretForSelection()
{
    int empty = mSelection->IsEmpty();
    if (mCaretInSelection == 0 && mSelection->mActive && !empty) {
        HideCaret(mEditor->m_hWnd);
        mCaretInSelection = 1;
    } else if (mCaretInSelection == 1 && (!mSelection->mActive || empty)) {
        ShowCaret(mEditor->m_hWnd);
        mCaretInSelection = 0;
    }
}

// 0x10012580 ?UpdateCaretForMargin@MfPeDrawingContext@@QAEXXZ
void MfPeDrawingContext::UpdateCaretForMargin()
{
    if (mHasCaret) {
        CPoint position;
        GetCaretPos(&position);
        mCaretPos = position;
        if (mCaretInMargin == 0 && mCaretPos.x >= 0 && mCaretPos.x < mMarginWidth) {
            HideCaret(mEditor->m_hWnd);
            mCaretInMargin = 1;
        } else if (mCaretInMargin == 1 && mCaretPos.x >= mMarginWidth) {
            ShowCaret(mEditor->m_hWnd);
            mCaretInMargin = 0;
        }
    }
}

// EQUIV 0x10012600 ?ScrollLines@MfPeDrawingContext@@QAEXH@Z: reviewed: the two summed members are loaded in the other order
void MfPeDrawingContext::ScrollLines(int lines)
{
    mEditor->ScrollWindow(0, lines * LineHeight());
    mEditor->GetUpdateRgn(&mUpdateRgn);
    mEditor->ValidateRgn(&mUpdateRgn);
    mEditor->InvalidateRgn(&mUpdateRgn, FALSE);
    mEditor->SetScrollPos(SB_VERT, mTopLine);
    mEditor->UpdateWindow();
}

// 0x10012690 ?ScrollColumns@MfPeDrawingContext@@QAEXH@Z
void MfPeDrawingContext::ScrollColumns(int columns)
{
    CRect rect(mMarginWidth, 0, mWidth, mHeight);

    mEditor->ScrollWindow(columns * mCharWidth, 0, NULL, &rect);
    mEditor->GetUpdateRgn(&mUpdateRgn);
    mEditor->ValidateRgn(&mUpdateRgn);
    mEditor->InvalidateRgn(&mUpdateRgn, FALSE);
    mEditor->SetScrollPos(SB_HORZ, mLeftColumn);
    mEditor->UpdateWindow();
    UpdateCaretForMargin();
}

// 0x10012750 ?ShowLine@MfPeDrawingContext@@QAEXABVCPoint@@H@Z
void MfPeDrawingContext::ShowLine(const CPoint &position, int offset)
{
    if (position.y >= mTopLine && !offset) {
        if (position.y > mTopLine + mFullLines - 1)
            ScrollTo(WM_VSCROLL, position.y - mFullLines + 1);
    } else {
        ScrollTo(WM_VSCROLL, position.y + offset);
    }
}

// 0x100127c0 ?ShowColumn@MfPeDrawingContext@@QAEXABVCPoint@@@Z
void MfPeDrawingContext::ShowColumn(const CPoint &position)
{
    if (position.x < mLeftColumn) {
        ScrollTo(WM_HSCROLL, position.x - 10);
    } else {
        int lastColumn = mLeftColumn + mFullColumns - 1;
        if (lastColumn > 0 && position.x > lastColumn)
            ScrollTo(WM_HSCROLL, position.x - mFullColumns + 10);
    }
}

// 0x10012830 ?SetTextCursor@MfPeDrawingContext@@QAEXXZ
void MfPeDrawingContext::SetTextCursor()
{
    SetCursor(mTextCursor);
}

// 0x10012840 ?UpdateCursor@MfPeDrawingContext@@QAEXVCPoint@@@Z
void MfPeDrawingContext::UpdateCursor(CPoint point)
{
    if (point.x < mMarginWidth)
        SetCursor(mMarginCursor);
    else if (mSelection->mActive && !mSelection->IsEmpty() && IsInSelection(point))
        SetCursor(mArrowCursor);
    else
        SetCursor(mTextCursor);
}

// 0x100128b0 ?SetCursorType@MfPeDrawingContext@@QAEXH@Z
void MfPeDrawingContext::SetCursorType(int type)
{
    switch (type) {
    case 0:
        SetCursor(mTextCursor);
        break;
    case 1:
        SetCursor(mMarginCursor);
        break;
    case 2:
        SetCursor(mArrowCursor);
        break;
    }
}

// 0x100128f0 ?UpdateDragCursor@MfPeDrawingContext@@QAEHVCPoint@@H@Z
int MfPeDrawingContext::UpdateDragCursor(CPoint point, int noDrop)
{
    CRect client;

    if (!mDragging) {
        mDragging = 1;
        HideCaret(mEditor->m_hWnd);
        DestroyCaret();
        CreateCaret(mEditor->m_hWnd, (HBITMAP)1, 2, mCharHeight);
        ShowCaret(mEditor->m_hWnd);
    }
    mEditor->GetClientRect(&client);
    if (client.PtInRect(point) && !noDrop) {
        SetCursor(mDropCursor);
        return 1;
    }
    SetCursor(mNoDropCursor);
    return 0;
}

// 0x100129a0 ?EndDrag@MfPeDrawingContext@@QAEXVCPoint@@@Z
void MfPeDrawingContext::EndDrag(CPoint point)
{
    mDragging = 0;
    UpdateCursor(point);
    HideCaret(mEditor->m_hWnd);
    DestroyCaret();
    CreateEditCaret();
}

// 0x100129e0 ?IsOverSelection@MfPeDrawingContext@@QAEHVCPoint@@@Z
int MfPeDrawingContext::IsOverSelection(CPoint point)
{
    if (mSelection->mActive && IsInSelection(point))
        return 1;
    return 0;
}

// 0x10012a10 ?IsInSelection@MfPeDrawingContext@@QAEHVCPoint@@@Z
int MfPeDrawingContext::IsInSelection(CPoint point)
{
    CPoint position = PointToPosition(point);
    return mSelection->Locate(position.x, position.y, 1) == 5;
}

// 0x10012a50 ?OpenClipboard@MfPeDrawingContext@@QAEHH@Z
int MfPeDrawingContext::OpenClipboard(int empty)
{
    if (!mEditor->OpenClipboard())
        return 0;
    if (empty)
        return EmptyClipboard();
    return 1;
}

// 0x10012a80 ?CloseClipboard@MfPeDrawingContext@@QAEHXZ
int MfPeDrawingContext::CloseClipboard()
{
    return ::CloseClipboard();
}

// 0x10012a90 ??1CPen@@UAE@XZ
// 0x10012ae0 ??1CBrush@@UAE@XZ
// 0x10012b30 ??_GCRgn@@UAEPAXI@Z
// 0x10012b50 ??1CRgn@@UAE@XZ
