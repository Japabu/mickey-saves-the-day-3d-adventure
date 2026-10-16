// PiccoloConsole.dll 0x10013300-0x10019f5f: MfPeEditorData, the text of an
// editor: its lines, the cursor and the selection, what the commands do to
// them, and the glyphs in the margin.
#include "PiccoloConsole.h"

// 0x10013300 _$E248
// 0x10013310 _$E244
// 0x10013320 _$E247
// 0x10013330 _$E245
MfDictionary MfPeEditorData::dictGlyphModules(0x35);

// 0x10013350 _$E253
// 0x10013360 _$E250
// 0x10013380 _$E252
// 0x10013390 _$E251
MfDblList MfPeEditorData::listEditors;

int nPwTabSize = 4;
int bPwSyntaxHighlight = 1;
int bPwAutoIndent = 1;

// 0x100133b0 ??0MfPeEditorData@@QAE@PAVMfPeDrawingContext@@@Z
MfPeEditorData::MfPeEditorData(MfPeDrawingContext *context)
    : mRefCount(1), mTabSize(nPwTabSize), mStringCursor(nPwTabSize), mCursorCallBacks(0), mUnixFormat(0),
      mSyntaxHighlight(bPwSyntaxHighlight), mAutoIndent(bPwAutoIndent)
{
    CreateLines();
    mCursorCallBacks = new MfDblList;
    context->mSelection = &mSelection;
}

// 0x10013480 ??_GMfPeEditorData@@UAEPAXI@Z

// 0x100134a0 ??1MfPeEditorData@@UAE@XZ
MfPeEditorData::~MfPeEditorData()
{
    delete mLines;
    delete mCursorCallBacks;
}

// 0x10013520 ?GetInterfaceName@MfPeEditorData@@UAEXPADHAAH1@Z
void MfPeEditorData::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "PiccoloEditor", size);
    major = 1;
    minor = 0;
}

// 0x10013550 ?AddRef@MfPeEditorData@@UAEHXZ
int MfPeEditorData::AddRef()
{
    return ++mRefCount;
}

// 0x10013560 ?Release@MfPeEditorData@@UAEHXZ
int MfPeEditorData::Release()
{
    if (--mRefCount == 0) {
        delete this;
        return 0;
    }
    return mRefCount;
}

// 0x10013580 ?GetInterface@MfPeEditorData@@UAEHHPAPAVMfAi@@@Z
int MfPeEditorData::GetInterface(int id, MfAi **ai)
{
    int found = 0;

    switch (id) {
    case 0:
        *ai = this;
        found = 1;
        break;
    case 1:
        *ai = CPiccoloWnd::LookupIWindows(this);
        if (*ai)
            found = 1;
        break;
    }
    return found;
}

// 0x100135c0 ?GetPosition@MfPeEditorData@@UAEXPAH0@Z
void MfPeEditorData::GetPosition(int *column, int *line)
{
    *column = mStringCursor.CharPos();
    *line = mCursor.y;
}

// 0x100135e0 ?GetModuleName@MfPeEditorData@@UAEXPAVMfString@@@Z
void MfPeEditorData::GetModuleName(MfString *module)
{
    *module = mModuleName;
}

// 0x10013600 ?IsTextEmpty@MfPeEditorData@@QAEHXZ
int MfPeEditorData::IsTextEmpty()
{
    int empty = 0;
    int count = mLines->GetCount();

    if (count == 0)
        empty = 1;
    else if (count == 1 && LineOf(mLines->FirstItem())->mText.Length() == 0)
        empty = 1;
    return empty;
}

// 0x10013630 ?GetLineText@MfPeEditorData@@UAEXHPAVMfString@@@Z
void MfPeEditorData::GetLineText(int line, MfString *text)
{
    *text = LineOf(GetLineItem(line))->mText;
}

// EQUIV 0x10013670 ?GetWordAtCursor@MfPeEditorData@@UAEXPAVMfString@@H@Z: reviewed: stack slots of the this copies only
void MfPeEditorData::GetWordAtCursor(MfString *word, int allLines)
{
    *word = NULL_STRING;
    MfStringList lines;
    GetRegionText(0, &lines, 0);
    if (lines.GetCount() == 0)
        return;
    MfDblListItem *item;
    if (allLines) {
        MfString *line = (MfString *)lines.First(item);
        *word << *line;
        for (line = (MfString *)MfDblList::Next(item); line; line = (MfString *)MfDblList::Next(item))
            *word << "\n" << *line;
    } else {
        *word << *(MfString *)lines.First(item);
    }
}

// EQUIV 0x10013820 ?CreateLines@MfPeEditorData@@AAEXXZ: reviewed: the order of independent member stores only
void MfPeEditorData::CreateLines()
{
    mLines = new MfPeTextLineList;
    mLines->Add(new MfPeTextLine(NULL_STRING));
    mCursorLine = mLines->FirstItem();
    mCursor.x = 1;
    mCursor.y = 1;
    mSelection.Reset();
    mSelection.mActive = 0;
}

// 0x10013990 ??0MfPeMarker@@QAE@XZ
// 0x100139a0 ??_GMfPeTextLine@@UAEPAXI@Z
// 0x100139c0 ??1MfPeTextLine@@UAE@XZ

// 0x10013a40 ?GetText@MfPeEditorData@@QAE?AVMfString@@XZ
MfString MfPeEditorData::GetText()
{
    MfString text;
    MfDblListItem *item = mLines->FirstItem();
    MfPeTextLine *line = LineOf(item);

    if (line)
        text << line->mText;
    line = (MfPeTextLine *)MfDblList::Next(item);
    MfString lineEnd;
    if (mUnixFormat)
        lineEnd = "\n";
    else
        lineEnd = "\r\n";
    while (line) {
        text << lineEnd << line->mText;
        line = LineOf(MfDblList::NextItem(item));
    }
    return text;
}

// TODO 0x10013b50 ?SetText@MfPeEditorData@@QAEXABVMfString@@PAVMfPeDrawingContext@@H@Z: 84% - the original moves each line pointer into a second register after the null test
void MfPeEditorData::SetText(const MfString &text, MfPeDrawingContext *context, int keepCursor)
{
    mLines->Clear();
    MfStringList lines;
    text.ConvertToLines(&lines);

    MfHPTimer timer;
    timer.Start();
    MfDblListItem *item;
    for (MfString *line = (MfString *)lines.First(item); line; line = (MfString *)MfDblList::Next(item))
        mLines->Add(new MfPeTextLine(*line));
    timer.Stop();

    context->ResetScrollRanges(mLines->GetCount());
    if (keepCursor) {
        SetCursor(mCursor.x, mCursor.y);
        context->ShowColumn(mCursor);
        context->ShowLine(mCursor, 0);
    } else {
        mCursorLine = mLines->FirstItem();
        LineOf(mCursorLine)->mText.CuBindToCursor(0, &mStringCursor);
        mCursor.x = 1;
        mCursor.y = 1;
        ShowCursor(context);
        mSelection.Reset();
        mSelection.mActive = 0;
    }
}

// TODO 0x10013d40 ?AppendText@MfPeEditorData@@QAEXABVMfString@@PAVMfPeDrawingContext@@@Z: 51% - the original calls MfArray's constructor out of line in the two new lines (VC5's per-function inline budget), this build inlines it
void MfPeEditorData::AppendText(const MfString &text, MfPeDrawingContext *context)
{
    int atEnd = 0;
    int count = mLines->GetCount();

    if (mCursor.y == count && mCursor.x >= mStringCursor.nExpandedLength)
        atEnd = 1;
    MfStringList lines;
    text.ConvertToLines(&lines);
    if (lines.GetCount() > 0) {
        MfDblListItem *item;
        MfString *line = (MfString *)lines.First(item);
        MfPeTextLine *last = LineOf(mLines->LastItem());
        if (last)
            last->mText << *line;
        else
            mLines->Add(new MfPeTextLine(*line));
        for (line = (MfString *)MfDblList::Next(item); line; line = (MfString *)MfDblList::Next(item))
            mLines->Add(new MfPeTextLine(*line));
        context->SetLineCount(mLines->GetCount());
        context->InvalidateFrom(count);
        context->Update();
        ShowCursor(context);
        if (atEnd)
            GotoTextEnd(context);
    }
}

// 0x10013f70 ?Clear@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::Clear(MfPeDrawingContext *context)
{
    SetText("", context, 0);
}

// 0x10013fe0 ?GetSelectionLine@MfPeEditorData@@QAEABVMfString@@XZ
const MfString &MfPeEditorData::GetSelectionLine()
{
    return LineOf(GetLineItem(mSelection.mStart.y))->mText;
}

// 0x10014000 ?IsAtTextEnd@MfPeEditorData@@QAEHXZ
int MfPeEditorData::IsAtTextEnd()
{
    MfPeTextLine *line = LineOf(mCursorLine);

    if (mCursor.y == mLines->GetCount() && mStringCursor.nCursPos >= line->mText.Length())
        return 1;
    return 0;
}

// 0x10014030 ?OnVScroll@MfPeEditorData@@QAEXIIPAVMfPeDrawingContext@@@Z
void MfPeEditorData::OnVScroll(UINT code, UINT position, MfPeDrawingContext *context)
{
    int lines = 0;

    switch (code) {
    case SB_TOP:
        lines = 1 - context->mTopLine;
        break;
    case SB_BOTTOM:
        lines = mLines->GetCount() - context->mTopLine;
        break;
    case SB_LINEUP:
        lines = -1;
        break;
    case SB_LINEDOWN:
        lines = 1;
        break;
    case SB_PAGEUP:
        lines = -context->mVisibleLines;
        break;
    case SB_PAGEDOWN:
        lines = context->mVisibleLines;
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        lines = position - context->mTopLine;
        break;
    }
    if (lines) {
        int top = context->mTopLine += lines;
        if (top < 1) {
            lines += 1 - top;
            context->mTopLine = 1;
        } else {
            int count = mLines->GetCount();
            if (top > count) {
                lines += count - top;
                context->mTopLine = count;
            }
        }
        if (lines)
            context->ScrollLines(-lines);
    }
}

// 0x100140e0 ?OnHScroll@MfPeEditorData@@QAEXIIPAVMfPeDrawingContext@@@Z
void MfPeEditorData::OnHScroll(UINT code, UINT position, MfPeDrawingContext *context)
{
    int columns = 0;

    switch (code) {
    case SB_LEFT:
        columns = 1 - context->mLeftColumn;
        break;
    case SB_RIGHT:
        columns = 255 - context->mLeftColumn;
        break;
    case SB_LINELEFT:
        columns = -1;
        break;
    case SB_LINERIGHT:
        columns = 1;
        break;
    case SB_PAGELEFT:
        columns = -(context->mFullColumns / 2);
        break;
    case SB_PAGERIGHT:
        columns = context->mFullColumns / 2;
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        columns = position - context->mLeftColumn;
        break;
    }
    if (columns) {
        int left = context->mLeftColumn += columns;
        if (left < 1) {
            columns += 1 - left;
            context->mLeftColumn = 1;
        } else if (left > 255) {
            columns += 255 - left;
            context->mLeftColumn = 255;
        }
        if (columns)
            context->ScrollColumns(-columns);
    }
}

// EQUIV 0x100141a0 ?Draw@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: reviewed: register choice only
void MfPeEditorData::Draw(MfPeDrawingContext *context)
{
    MfDblListItem *item = GetLineItem(context->mTopLine);
    if (item == 0)
        return;
    MfPeTextLine *next = (MfPeTextLine *)item->pData;
    int i = 0;
    while (next) {
        if (i >= context->mVisibleLines)
            return;
        MfPeTextLine *line = next;
        int number = context->mTopLine + i;
        int inComment = 0;
        line->DrawMargin(number, context);
        context->DrawLineEnd(number, line->Draw(number, context, mTabSize, mSyntaxHighlight));
        if (line->mSyntax.mFlags & (MfPeLineSyntax::COMMENT_START | MfPeLineSyntax::IN_COMMENT))
            inComment = 1;
        i++;
        next = (MfPeTextLine *)MfDblList::Next(item);
        if (next) {
            if (inComment == 1
                && ((~next->mSyntax.mFlags & MfPeLineSyntax::COMMENT_END) || (next->mSyntax.mFlags & MfPeLineSyntax::CHANGED)))
                next->mSyntax.mFlags = (next->mSyntax.mFlags & ~MfPeLineSyntax::CHANGED) | MfPeLineSyntax::IN_COMMENT;
            if (!inComment)
                next->mSyntax.mFlags &= ~(MfPeLineSyntax::IN_COMMENT | MfPeLineSyntax::COMMENT_END);
        }
    }
    if (i < context->mVisibleLines)
        context->EraseFrom(context->mTopLine + i);
}

// 0x100142a0 ?UpdateSyntax@MfPeEditorData@@QAEXXZ
void MfPeEditorData::UpdateSyntax()
{
    if (!mSyntaxHighlight)
        return;
    MfDblListItem *item = mCursorLine;
    MfPeTextLine *line = LineOf(item);
    int inComment = 0;
    if (line->mSyntax.mFlags & (MfPeLineSyntax::COMMENT_START | MfPeLineSyntax::IN_COMMENT))
        inComment = 1;
    line = (MfPeTextLine *)MfDblList::Next(item);
    while (item) {
        if (~line->mSyntax.mFlags & MfPeLineSyntax::SCANNED) {
            if (inComment)
                line->mSyntax.mFlags |= MfPeLineSyntax::IN_COMMENT;
            line->Scan(mTabSize);
        }
        if (inComment == 1 && (~line->mSyntax.mFlags & MfPeLineSyntax::COMMENT_END))
            line->mSyntax.mFlags |= MfPeLineSyntax::IN_COMMENT;
        else if (!inComment && (line->mSyntax.mFlags & MfPeLineSyntax::IN_COMMENT))
            line->mSyntax.mFlags &= ~MfPeLineSyntax::IN_COMMENT;
        inComment = (line->mSyntax.mFlags & (MfPeLineSyntax::COMMENT_START | MfPeLineSyntax::IN_COMMENT)) != 0;
        line = (MfPeTextLine *)MfDblList::Next(item);
    }
}

// 0x10014350 ?Print@MfPeEditorData@@QAEXPAVMfPePrintingContext@@@Z
void MfPeEditorData::Print(MfPePrintingContext *context)
{
    MfDblListItem *item = mLines->FirstItem();
    MfPeTextLine *line = LineOf(item);
    MfPeTextLine *next;
    while (line) {
        int inComment = 0;
        line->Print(context, mTabSize, mSyntaxHighlight);
        if (line->mSyntax.mFlags & (MfPeLineSyntax::COMMENT_START | MfPeLineSyntax::IN_COMMENT))
            inComment = 1;
        next = (MfPeTextLine *)MfDblList::Next(item);
        line = next;
        if (next) {
            if (inComment == 1
                && ((~next->mSyntax.mFlags & MfPeLineSyntax::COMMENT_END) || (next->mSyntax.mFlags & MfPeLineSyntax::CHANGED)))
                next->mSyntax.mFlags = (next->mSyntax.mFlags & ~MfPeLineSyntax::CHANGED) | MfPeLineSyntax::IN_COMMENT;
            if (!inComment)
                next->mSyntax.mFlags &= ~(MfPeLineSyntax::IN_COMMENT | MfPeLineSyntax::COMMENT_END);
        }
    }
}

// EQUIV 0x100143e0 ?CursorUp@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: reviewed: register choice only
void MfPeEditorData::CursorUp(MfPeDrawingContext *context)
{
    if (mCursor.y < context->mTopLine)
        context->ScrollTo(WM_VSCROLL, mCursor.y);
    else if (mCursor.y > context->mTopLine + context->mFullLines)
        context->ScrollTo(WM_VSCROLL, mCursor.y - context->mFullLines);
    context->ShowColumn(mCursor);
    if (mCursor.y > 1) {
        mCursor.y--;
        MfDblList::PrevItem(mCursorLine);
        BindCursor();
        ShowCursor(context);
        if (mCursor.y < context->mTopLine)
            context->mEditor->SendMessage(WM_VSCROLL, SB_LINEUP, 0);
    }
}

// TODO 0x100144b0 ?CursorDown@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: 69% - the original lays the branches out longer (257 bytes instead of 247)
void MfPeEditorData::CursorDown(MfPeDrawingContext *context)
{
    int count = mLines->GetCount();

    if (mCursor.y < context->mTopLine)
        context->ScrollTo(WM_VSCROLL, mCursor.y + 1);
    else if (mCursor.y < count) {
        if (mCursor.y > context->mTopLine + context->mFullLines - 2)
            context->ScrollTo(WM_VSCROLL, mCursor.y - context->mFullLines + 2);
    } else if (mCursor.y == count && mCursor.y > context->mTopLine + context->mFullLines - 2)
        context->ScrollTo(WM_VSCROLL, mCursor.y - context->mFullLines + 1);
    context->ShowColumn(mCursor);
    if (mCursor.y < mLines->GetCount()) {
        mCursor.y++;
        MfDblList::NextItem(mCursorLine);
        BindCursor();
        ShowCursor(context);
        if (mCursor.y > context->mTopLine + context->mFullLines - 1)
            context->mEditor->SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
    }
}

// 0x100145c0 ?CursorLeft@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::CursorLeft(MfPeDrawingContext *context)
{
    context->ShowColumn(mCursor);
    context->ShowLine(mCursor, 0);
    if (mStringCursor.CursorPos() > 0) {
        mCursor.x = LineOf(mCursorLine)->mText.CuMoveLeft(&mStringCursor) + 1;
        ShowCursor(context);
        if (mCursor.x < context->mLeftColumn)
            context->ScrollTo(WM_HSCROLL, context->mLeftColumn - 10);
    }
}

// 0x10014640 ?CursorRight@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::CursorRight(MfPeDrawingContext *context)
{
    context->ShowColumn(mCursor);
    context->ShowLine(mCursor, 0);
    if (mCursor.x < 255) {
        mCursor.x = LineOf(mCursorLine)->mText.CuMoveRight(&mStringCursor) + 1;
        ShowCursor(context);
        if (mCursor.x > context->mLeftColumn + context->mFullColumns - 1)
            context->ScrollTo(WM_HSCROLL, context->mLeftColumn + 10);
    }
}

// TODO 0x100146d0 ?TabLeft@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: 58% - the original lays the branches out longer (164 bytes instead of 162)
void MfPeEditorData::TabLeft(MfPeDrawingContext *context)
{
    context->ShowLine(mCursor, 0);
    if (mStringCursor.CursorPos() > 0) {
        MfString &text = LineOf(mCursorLine)->mText;
        do
            mCursor.x = text.CuMoveLeft(&mStringCursor) + 1;
        while (mStringCursor.CursorPos() % mTabSize != 0
               && mStringCursor.CursorPos() >= mStringCursor.ExpandedSize());
        if (mStringCursor.CursorPos() < mStringCursor.ExpandedSize())
            mCursor.x = text.CuMoveRight(&mStringCursor) + 1;
        ShowCursor(context);
        if (mCursor.x < context->mLeftColumn)
            context->ScrollTo(WM_HSCROLL, context->mLeftColumn - 10);
    }
}

// 0x10014780 ?TabStopLeft@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::TabStopLeft(MfPeDrawingContext *context)
{
    context->ShowLine(mCursor, 0);
    if (mStringCursor.CursorPos() > 0) {
        MfString &text = LineOf(mCursorLine)->mText;
        do
            mCursor.x = text.CuMoveLeft(&mStringCursor) + 1;
        while (mStringCursor.CursorPos() % mTabSize != 0);
        ShowCursor(context);
        context->ShowColumn(mCursor);
    }
}

// 0x100147f0 ?TabStopRight@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::TabStopRight(MfPeDrawingContext *context)
{
    context->ShowLine(mCursor, 0);
    MfString &text = LineOf(mCursorLine)->mText;
    do
        mCursor.x = text.CuMoveRight(&mStringCursor) + 1;
    while (mStringCursor.CursorPos() % mTabSize != 0);
    ShowCursor(context);
    context->ShowColumn(mCursor);
}

// 0x10014850 ?CursorHome@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::CursorHome(MfPeDrawingContext *context)
{
    context->ShowLine(mCursor, 0);
    if (mStringCursor.CursorPos() > 0) {
        mCursor.x = LineOf(mCursorLine)->mText.CuBeginningOfString(&mStringCursor) + 1;
        ShowCursor(context);
    }
    context->ShowColumn(mCursor);
}

// 0x100148a0 ?CursorEnd@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::CursorEnd(MfPeDrawingContext *context)
{
    context->ShowLine(mCursor, 0);
    mCursor.x = LineOf(mCursorLine)->mText.CuEndOfString(&mStringCursor) + 1;
    ShowCursor(context);
    context->ShowColumn(mCursor);
}

// EQUIV 0x100148f0 ?PageUp@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: reviewed: register choice and the order of an independent load and store
void MfPeEditorData::PageUp(MfPeDrawingContext *context)
{
    context->ShowColumn(mCursor);
    context->ShowLine(mCursor, 0);
    int page = context->mFullLines - 1;
    int offset = mCursor.y - context->mTopLine;
    int i;
    for (i = 0; i < page && mCursorLine;) {
        i++;
        MfDblList::PrevItem(mCursorLine);
    }
    if (mCursorLine == 0) {
        i--;
        mCursorLine = mLines->FirstItem();
    }
    mCursor.y -= i;
    LineOf(mCursorLine)->mText.CuBindToCursor(mCursor.x - 1, &mStringCursor);
    ShowCursor(context);
    context->ScrollTo(WM_VSCROLL, mCursor.y - offset);
}

// EQUIV 0x100149b0 ?PageDown@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: reviewed: register choice and the order of an independent load and store
void MfPeEditorData::PageDown(MfPeDrawingContext *context)
{
    context->ShowColumn(mCursor);
    context->ShowLine(mCursor, 0);
    int page = context->mFullLines - 1;
    int offset = mCursor.y - context->mTopLine;
    int i;
    for (i = 0; i < page && mCursorLine;) {
        i++;
        MfDblList::NextItem(mCursorLine);
    }
    if (mCursorLine == 0) {
        i--;
        offset = 0;
        mCursorLine = mLines->LastItem();
    }
    mCursor.y += i;
    LineOf(mCursorLine)->mText.CuBindToCursor(mCursor.x - 1, &mStringCursor);
    ShowCursor(context);
    context->ScrollTo(WM_VSCROLL, mCursor.y - offset);
}

// EQUIV 0x10014a70 ?GotoTextStart@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: reviewed: register choice only
void MfPeEditorData::GotoTextStart(MfPeDrawingContext *context)
{
    mCursor.y = 1;
    mCursor.x = 1;
    MfPeTextLine *line = (MfPeTextLine *)mLines->First(mCursorLine);
    mCursor.x = line->mText.CuBindToCursor(mCursor.x - 1, &mStringCursor) + 1;
    ShowCursor(context);
    context->ShowColumn(mCursor);
    context->ShowLine(mCursor, 0);
}

// TODO 0x10014ae0 ?GotoTextEnd@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: 79% - the original lays the branches out longer (124 bytes instead of 121)
void MfPeEditorData::GotoTextEnd(MfPeDrawingContext *context)
{
    UpdateSyntax();
    mCursor.x = 1;
    mCursor.y = mLines->GetCount();
    MfString &text = ((MfPeTextLine *)mLines->Last(mCursorLine))->mText;
    mCursor.x = text.CountCharacters(text.Length(), mTabSize) + 1;
    mCursor.x = text.CuBindToCursor(mCursor.x - 1, &mStringCursor) + 1;
    ShowCursor(context);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
}

// EQUIV 0x10014b60 ?WordLeft@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: reviewed: register choice only
void MfPeEditorData::WordLeft(MfPeDrawingContext *context)
{
    if (mStringCursor.CharPos() == 0 && mCursor.y > 1) {
        MfPeTextLine *line = mLines->Prev(mCursorLine);
        mCursor.y--;
        MfString &text = line->mText;
        text.CuBindToCursor(0, &mStringCursor);
        mCursor.x = text.CuEndOfString(&mStringCursor) + 1;
    }
    if (mStringCursor.CharPos() > 0) {
        MfString &text = LineOf(mCursorLine)->mText;
        char c = text.GetCharacter(mStringCursor.CharPos() - 1);
        if (isspace(c)) {
            WhiteSpaceScanner white;
            mCursor.x = text.CuScanBackward(&mStringCursor, white) + 1;
        }
        if (mStringCursor.CharPos() > 0)
            c = text.GetCharacter(mStringCursor.CharPos() - 1);
        if (isalnum(c) || c == '_') {
            IdentScanner ident;
            mCursor.x = text.CuScanBackward(&mStringCursor, ident) + 1;
        } else if (ispunct(c)) {
            PuncScanner punc;
            mCursor.x = text.CuScanBackward(&mStringCursor, punc) + 1;
        }
    }
    ShowCursor(context);
    context->ShowColumn(mCursor);
    context->ShowLine(mCursor, 0);
}

// TODO 0x10014c90 ?WordRight@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: 53% - the original lays the branches out longer (360 bytes instead of 352)
void MfPeEditorData::WordRight(MfPeDrawingContext *context)
{
    unsigned int column;

    if (mStringCursor.CursorPos() < mStringCursor.ExpandedSize()) {
        MfString &text = LineOf(mCursorLine)->mText;
        char c = text.GetCharacter(mStringCursor.CharPos());
        if (isspace(c)) {
            WhiteSpaceScanner white;
            column = text.CuScanForward(&mStringCursor, white);
        } else {
            if (isalnum(c) || c == '_') {
                IdentScanner ident;
                text.CuScanForward(&mStringCursor, ident);
            } else if (ispunct(c)) {
                PuncScanner punc;
                text.CuScanForward(&mStringCursor, punc);
            }
            WhiteSpaceScanner white;
            column = text.CuScanForward(&mStringCursor, white);
        }
    } else if (mCursor.y < mLines->GetCount()) {
        MfDblList::NextItem(mCursorLine);
        mCursor.y++;
        MfString &text = LineOf(mCursorLine)->mText;
        mCursor.x = text.CuBindToCursor(0, &mStringCursor) + 1;
        WhiteSpaceScanner white;
        column = text.CuScanForward(&mStringCursor, white);
    } else {
        column = LineOf(mCursorLine)->mText.CuEndOfString(&mStringCursor);
    }
    mCursor.x = column + 1;
    ShowCursor(context);
    context->ShowColumn(mCursor);
    context->ShowLine(mCursor, 0);
}

// EQUIV 0x10014e00 ?ScrollUp@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z: reviewed: the two summed members are loaded in the other order
void MfPeEditorData::ScrollUp(MfPeDrawingContext *context)
{
    context->ShowColumn(mCursor);
    context->ShowLine(mCursor, 0);
    context->mEditor->SendMessage(WM_VSCROLL, SB_LINEUP, 0);
    if (mCursor.y > 1 && mCursor.y > context->mFullLines + context->mTopLine - 1) {
        mCursor.y--;
        MfDblList::PrevItem(mCursorLine);
        BindCursor();
        ShowCursor(context);
    }
}

// 0x10014e90 ?ScrollDown@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::ScrollDown(MfPeDrawingContext *context)
{
    context->ShowColumn(mCursor);
    context->ShowLine(mCursor, 0);
    context->mEditor->SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
    if (mCursor.y < mLines->GetCount() && mCursor.y < context->mTopLine) {
        mCursor.y++;
        MfDblList::NextItem(mCursorLine);
        BindCursor();
        ShowCursor(context);
    }
}

// 0x10014f20 ?GetLineItem@MfPeEditorData@@QAEPAVMfDblListItem@@H@Z
MfDblListItem *MfPeEditorData::GetLineItem(int line)
{
    if (line < 1 || line > mLines->GetCount())
        return 0;
    MfDblListItem *item = mLines->FirstItem();
    for (int i = 1; i < line; i++)
        MfDblList::NextItem(item);
    return item;
}

// EQUIV 0x10014f50 ?SetCursor@MfPeEditorData@@QAEXHH@Z: reviewed: an independent store and register move swap places
void MfPeEditorData::SetCursor(int column, int line)
{
    if (line < 1)
        line = 1;
    else if (line > mLines->GetCount())
        line = mLines->GetCount();
    mCursor.x = column;
    mCursor.y = line;
    mCursorLine = GetLineItem(line);
    BindCursor();
}

// 0x10014fb0 ?MoveCursorTo@MfPeEditorData@@QAEXVCPoint@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::MoveCursorTo(CPoint point, MfPeDrawingContext *context)
{
    context->ClampPoint(point);
    mCursor = context->PointToPosition(point);
    if (mCursor.y > mLines->GetCount())
        mCursor.y = mLines->GetCount();
    mCursorLine = GetLineItem(mCursor.y);
    BindCursor();
    ShowCursor(context);
}

// 0x10015030 ?ShowCursor@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::ShowCursor(MfPeDrawingContext *context)
{
    context->SetCaret(mCursor);
    NotifyCursorChanged();
}

// 0x10015050 ?UpdateScrollRanges@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::UpdateScrollRanges(MfPeDrawingContext *context)
{
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
}

// 0x10015070 ?RegisterCursorChangedCallback@MfPeEditorData@@QAEXPAVMfPeCursorChanged@@@Z
void MfPeEditorData::RegisterCursorChangedCallback(MfPeCursorChanged *callBack)
{
    mCursorCallBacks->Add(callBack);
}

// 0x10015080 ?UnregisterCursorChangedCallback@MfPeEditorData@@QAEXPAVMfPeCursorChanged@@@Z
void MfPeEditorData::UnregisterCursorChangedCallback(MfPeCursorChanged *callBack)
{
    MfDblListItem *item = mCursorCallBacks->Find(callBack);
    mCursorCallBacks->Delete(item);
}

// 0x100150b0 ?NotifyCursorChanged@MfPeEditorData@@QAEXXZ
void MfPeEditorData::NotifyCursorChanged()
{
    MfDblListItem *item;
    MfPeCursorChanged *callBack = (MfPeCursorChanged *)mCursorCallBacks->GetFirst(item);
    while (callBack) {
        callBack->CursorChanged(mCursor.x, mCursor.y);
        callBack = (MfPeCursorChanged *)mCursorCallBacks->GetNext(item);
    }
}

// 0x100150f0 ?DragTo@MfPeEditorData@@QAEXVCPoint@@HPAVMfPeDrawingContext@@@Z
void MfPeEditorData::DragTo(CPoint point, int noDrop, MfPeDrawingContext *context)
{
    if (context->UpdateDragCursor(point, noDrop))
        MoveCursorTo(point, context);
}

// 0x10015130 ?EndDrag@MfPeEditorData@@QAEXVCPoint@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::EndDrag(CPoint point, MfPeDrawingContext *context)
{
    context->EndDrag(point);
    MoveCursorTo(point, context);
}

// 0x10015160 ?StartSelection@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::StartSelection(MfPeDrawingContext *context)
{
    if (!mSelection.mActive) {
        CPoint cursor;
        GetCursor(cursor);
        mSelection.mActive = 1;
        mSelection.Set(cursor, cursor);
        context->UpdateCaretForSelection();
    } else {
        mSelection.ResetPivotEnd();
    }
}

// TODO 0x100151b0 ?SelectLine@MfPeEditorData@@QAEXVCPoint@@PAVMfPeDrawingContext@@@Z: 70% - the original lays the branches out longer (245 bytes instead of 237)
void MfPeEditorData::SelectLine(CPoint point, MfPeDrawingContext *context)
{
    if (!mSelection.mActive) {
        CPoint start = context->PointToPosition(point);
        CPoint end;
        start.x = 1;
        end.x = 1;
        if (start.y < mLines->GetCount()) {
            end.y = start.y + 1;
        } else {
            MfPeTextLine *last = LineOf(mLines->LastItem());
            end.x = last->mText.CountCharacters(last->mText.Length(), mTabSize) + 1;
            start.y = end.y = mLines->GetCount();
        }
        mSelection.mActive = 1;
        mSelection.Set(start, end);
        mSelection.Invalidate(context);
        context->Update();
        context->UpdateCaretForSelection();
    } else {
        mSelection.ResetPivotEnd();
        MfPeTextLine *last = LineOf(mLines->LastItem());
        mSelection.AdvancePivotEnd(mLines->GetCount(), last->mText.CountCharacters(last->mText.Length(), mTabSize) + 1);
    }
}

// TODO 0x100152b0 ?SelectWord@MfPeEditorData@@QAEXVCPoint@@PAVMfPeDrawingContext@@@Z: 42% - the original lays the branches out longer (412 bytes instead of 405)
void MfPeEditorData::SelectWord(CPoint point, MfPeDrawingContext *context)
{
    CPoint first = mCursor;
    CPoint last = mCursor;
    int start = mStringCursor.CursorPos() + 1;
    int end = start;

    if (mStringCursor.CursorPos() < mStringCursor.ExpandedSize()) {
        MfString &text = LineOf(mCursorLine)->mText;
        char c = text.GetCharacter(mStringCursor.CharPos());
        if (isalnum(c) || c == '_') {
            IdentScanner ident;
            end = text.CuScanForward(&mStringCursor, ident) + 1;
            start = text.CuScanBackward(&mStringCursor, ident) + 1;
        } else if (ispunct(c)) {
            start = mStringCursor.CursorPos() + 1;
            end = start + 1;
        } else if (c == ' ' && mStringCursor.CharPos() > 0) {
            c = text.GetCharacter(mStringCursor.CharPos() - 1);
            if (isalnum(c) || c == '_') {
                end = mStringCursor.CursorPos() + 1;
                IdentScanner ident;
                start = text.CuScanBackward(&mStringCursor, ident) + 1;
            }
        } else if (c == '\t') {
            end = text.CuMoveRight(&mStringCursor) + 1;
            start = text.CuMoveLeft(&mStringCursor) + 1;
        }
    }
    first.x = start;
    last.x = end;
    mSelection.mActive = 1;
    mSelection.Set(first, last);
    if (mCursor.x != start) {
        mCursor.x = start;
        ShowCursor(context);
        context->ShowColumn(mCursor);
    }
    mSelection.Invalidate(context);
    context->Update();
    context->UpdateCaretForSelection();
}

// TODO 0x10015450 ?CollapseSelection@MfPeEditorData@@QAEXHPAVMfPeDrawingContext@@@Z: 87% - the original lays the branches out longer (171 bytes instead of 167)
void MfPeEditorData::CollapseSelection(int toStart, MfPeDrawingContext *context)
{
    if (mSelection.mActive == 1) {
        mSelection.mActive = 0;
        if (toStart == 1)
            mCursor = mSelection.mStart;
        else
            mCursor = mSelection.mEnd;
        context->ShowColumn(mCursor);
        context->ShowLine(mCursor, 0);
        mCursorLine = GetLineItem(mCursor.y);
        mSelection.Invalidate(context);
        context->Update();
        BindCursor();
        ShowCursor(context);
        context->UpdateCaretForSelection();
    }
}

// 0x10015500 ?ClearSelection@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::ClearSelection(MfPeDrawingContext *context)
{
    if (mSelection.mActive == 1) {
        mSelection.mActive = 0;
        mSelection.Invalidate(context);
        context->Update();
        context->UpdateCaretForSelection();
    }
}

// 0x10015530 ?ResetSelection@MfPeEditorData@@QAEXVCPoint@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::ResetSelection(CPoint point, MfPeDrawingContext *context)
{
    if (mSelection.mActive) {
        context->PointToPosition(point);
        mSelection.ResetPivotEnd();
    }
}

// 0x10015560 ?ExtendSelection@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::ExtendSelection(MfPeDrawingContext *context)
{
    if (mSelection.mActive) {
        CPoint cursor;
        GetCursor(cursor);
        mSelection.Extend(cursor, 1, context);
        context->UpdateCaretForSelection();
    }
}

// TODO 0x100155a0 ?ExtendLineSelection@MfPeEditorData@@QAEXVCPoint@@PAVMfPeDrawingContext@@@Z: 56% - the original lays the branches out longer (209 bytes instead of 201)
void MfPeEditorData::ExtendLineSelection(CPoint point, MfPeDrawingContext *context)
{
    point.x = context->mMarginWidth;
    CPoint position = context->PointToPosition(point);
    if (mSelection.LocatePivot(position) != MfPeRegion::BEFORE) {
        if (position.y < mLines->GetCount()) {
            point.y += context->LineHeight();
        } else {
            MfPeTextLine *last = LineOf(mLines->LastItem());
            position.x = last->mText.CountCharacters(last->mText.Length(), mTabSize) + 1;
            position.y = mLines->GetCount();
            point = context->PositionToPoint(position);
        }
    }
    MoveCursorTo(point, context);
    ExtendSelection(context);
}

// 0x10015680 ?ExtendWordSelection@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::ExtendWordSelection(MfPeDrawingContext *context)
{
    CPoint cursor;
    GetCursor(cursor);
    int where = mSelection.LocatePivot(cursor);
    if (where == MfPeRegion::BEFORE) {
        MfString &text = LineOf(mCursorLine)->mText;
        char c = text.GetCharacter(mStringCursor.CharPos());
        if (c == ' ' && mStringCursor.CharPos() > 0) {
            c = text.GetCharacter(mStringCursor.CharPos() - 1);
            if (isalnum(c) || c == '_')
                text.CuMoveLeft(&mStringCursor);
        }
        if (isalnum(c) || c == '_') {
            IdentScanner ident;
            text.CuScanBackward(&mStringCursor, ident);
        }
    } else if (where == MfPeRegion::AFTER) {
        MfString &text = LineOf(mCursorLine)->mText;
        char c = text.GetCharacter(mStringCursor.CharPos());
        if (isalnum(c) || c == '_') {
            IdentScanner ident;
            text.CuScanForward(&mStringCursor, ident);
        } else if (c != ' ') {
            text.CuMoveRight(&mStringCursor);
        }
    }
    mCursor.x = mStringCursor.CursorPos() + 1;
    ShowCursor(context);
    ExtendSelection(context);
}

// 0x100157c0 ?EndSelection@MfPeEditorData@@QAEXXZ
void MfPeEditorData::EndSelection()
{
    if (mSelection.mActive && mSelection.mStart.x == mSelection.mEnd.x && mSelection.mStart.y == mSelection.mEnd.y)
        mSelection.mActive = 0;
}

// 0x100157f0 ?IsSelectionPastLineEnd@MfPeEditorData@@QAEHXZ
int MfPeEditorData::IsSelectionPastLineEnd()
{
    int startColumn = mSelection.mStart.x;
    int endColumn = mSelection.mEnd.x;
    if (mSelection.mStart.y == mSelection.mEnd.y && startColumn > mStringCursor.ExpandedSize()
        && endColumn > mStringCursor.ExpandedSize())
        return 1;
    return 0;
}

// 0x10015820 ?InsertChar@MfPeEditorData@@QAEXDPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::InsertChar(char c, MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeCharInsertState *state = dynamic_cast<MfPeCharInsertState *>(editorState);

    if (mStringCursor.ExpandedSize() < 255) {
        CPoint cursor;
        GetCursor(cursor);
        context->ShowLine(cursor, 0);
        context->ShowColumn(cursor);
        MfPeTextLine *line = LineOf(mCursorLine);
        state->mCursor = cursor;
        state->mLength = line->mText.Length();
        line->InsertChar(c, &mStringCursor);
        mCursor.x = line->mText.CuMoveRight(&mStringCursor) + 1;
        context->InvalidateFrom(mCursor.y);
        context->Update();
        ShowCursor(context);
    }
}

// 0x10015900 ?UndoInsertChar@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::UndoInsertChar(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeCharInsertState *state = dynamic_cast<MfPeCharInsertState *>(editorState);

    SetCursor(state->mCursor.x, state->mCursor.y);
    context->ShowLine(mCursor, 0);
    MfPeTextLine *line = LineOf(mCursorLine);
    line->DeleteChar(1, &mStringCursor);
    if (line->mText.Length() > state->mLength) {
        line->mText.Truncate(state->mLength);
        mCursor.x = line->mText.CuBindToCursor(line->mText.Length(), &mStringCursor) + 1;
    }
    ShowCursor(context);
    context->ShowColumn(mCursor);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// EQUIV 0x100159d0 ?OverwriteChar@MfPeEditorData@@QAEXDPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: reviewed: the order of independent loads and stores only
void MfPeEditorData::OverwriteChar(char c, MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeCharOverwriteState *state = dynamic_cast<MfPeCharOverwriteState *>(editorState);

    if (mStringCursor.ExpandedSize() < 255) {
        CPoint cursor;
        GetCursor(cursor);
        context->ShowLine(cursor, 0);
        context->ShowColumn(cursor);
        MfPeTextLine *line = LineOf(mCursorLine);
        MfString &text = line->mText;
        state->mCursor = cursor;
        state->mLength = text.Length();
        line->InsertChar(c, &mStringCursor);
        mCursor.x = text.CuMoveRight(&mStringCursor) + 1;
        state->mOldChar = line->DeleteChar(1, &mStringCursor);
        context->InvalidateFrom(mCursor.y);
        context->Update();
        ShowCursor(context);
    }
}

// 0x10015ac0 ?UndoOverwriteChar@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::UndoOverwriteChar(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeCharOverwriteState *state = dynamic_cast<MfPeCharOverwriteState *>(editorState);

    SetCursor(state->mCursor.x, state->mCursor.y);
    context->ShowLine(mCursor, 0);
    MfPeTextLine *line = LineOf(mCursorLine);
    line->DeleteChar(1, &mStringCursor);
    line->InsertChar(state->mOldChar, &mStringCursor);
    if (line->mText.Length() > state->mLength) {
        line->mText.Truncate(state->mLength);
        mCursor.x = line->mText.CuBindToCursor(line->mText.Length(), &mStringCursor) + 1;
    }
    ShowCursor(context);
    context->ShowColumn(mCursor);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// 0x10015ba0 ?InsertString@MfPeEditorData@@QAEXPAVMfString@@PAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::InsertString(MfString *text, MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeInsertStringState *state = dynamic_cast<MfPeInsertStringState *>(editorState);

    GetCursor(state->mCursor);
    context->ShowLine(mCursor, 0);
    MfPeTextLine *line = LineOf(mCursorLine);
    MfString &lineText = line->mText;
    state->mLength = lineText.Length();
    line->FillTo(&mStringCursor);
    lineText = lineText.LeftString(mStringCursor.CharPos()) + *text
               + lineText.RightString(lineText.Length() - mStringCursor.CharPos());
    int position = mStringCursor.CharPos();
    lineText.CuBindToCursor(0, &mStringCursor);
    mCursor.x = lineText.CuSetCharPos(text->Length() + position, &mStringCursor) + 1;
    ShowCursor(context);
    context->ShowColumn(mCursor);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// TODO 0x10015d30 ?UndoInsertString@MfPeEditorData@@QAEXHPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: 64% - this build expands more inline or lays the branches out longer (546 bytes instead of 531)
void MfPeEditorData::UndoInsertString(int length, MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeInsertStringState *state = dynamic_cast<MfPeInsertStringState *>(editorState);

    SetCursor(state->mCursor.x, state->mCursor.y);
    context->ShowLine(mCursor, 0);
    MfString &text = LineOf(mCursorLine)->mText;
    text = text.LeftString(mStringCursor.CharPos())
           + text.RightString(text.Length() - length - mStringCursor.CharPos());
    if (text.Length() > state->mLength)
        text.Truncate(state->mLength);
    mCursor.x = text.CuBindToCursor(mCursor.x - 1, &mStringCursor) + 1;
    ShowCursor(context);
    context->ShowColumn(mCursor);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// 0x10015f50 ?DeleteChar@MfPeEditorData@@QAEXHPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::DeleteChar(int forward, MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDeleteCharState *state = dynamic_cast<MfPeDeleteCharState *>(editorState);

    GetCursor(state->mCursor);
    context->ShowLine(state->mCursor, 0);
    context->ShowColumn(state->mCursor);
    state->mForward = forward;
    MfPeTextLine *line = LineOf(mCursorLine);
    if (!forward)
        mCursor.x = line->mText.CuMoveLeft(&mStringCursor) + 1;
    state->mDeletedChar = line->DeleteChar(1, &mStringCursor);
    state->mCursor = mCursor;
    context->InvalidateFrom(mCursor.y);
    context->Update();
    ShowCursor(context);
}

// 0x10016020 ?UndoDeleteChar@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::UndoDeleteChar(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDeleteCharState *state = dynamic_cast<MfPeDeleteCharState *>(editorState);

    SetCursor(state->mCursor.x, state->mCursor.y);
    context->ShowLine(mCursor, 0);
    MfPeTextLine *line = LineOf(mCursorLine);
    line->InsertChar(state->mDeletedChar, &mStringCursor);
    if (state->mForward == 0)
        mCursor.x = line->mText.CuMoveRight(&mStringCursor) + 1;
    ShowCursor(context);
    context->ShowColumn(mCursor);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// EQUIV 0x100160d0 ?JoinLines@MfPeEditorData@@AAEXH@Z: reviewed: register and slot choice; the two width queries use the same lines and arguments in the same order
void MfPeEditorData::JoinLines(int forward)
{
    MfPeTextLine *line = mLines->Current(mCursorLine);

    if (forward) {
        MfDblListItem *item = mCursorLine;
        MfPeTextLine *next = mLines->Next(item);
        if (item == 0)
            return;
        if (line->mText.CountCharacters(line->mText.Length(), mTabSize)
                + next->mText.CountCharacters(next->mText.Length(), mTabSize)
            < 255) {
            line->FillTo(&mStringCursor);
            line->mText << next->mText;
            mLines->Delete(item);
            mCursor.x = line->mText.CuBindToCursor(mCursor.x - 1, &mStringCursor) + 1;
        }
    } else {
        MfDblListItem *item = mCursorLine;
        MfPeTextLine *prev = LineOf(MfDblList::PrevItem(item));
        if (item == 0)
            return;
        if (prev->mText.CountCharacters(prev->mText.Length(), mTabSize)
                + line->mText.CountCharacters(line->mText.Length(), mTabSize)
            < 255) {
            mCursor.y--;
            prev->mText.CuBindToCursor(0, &mStringCursor);
            mCursor.x = prev->mText.CuEndOfString(&mStringCursor) + 1;
            prev->mText << line->mText;
            mLines->Delete(mCursorLine);
            mCursorLine = item;
            mCursor.x = prev->mText.CuBindToCursor(mStringCursor.CursorPos(), &mStringCursor) + 1;
        }
    }
}

// 0x10016250 ?JoinLine@MfPeEditorData@@QAEXHPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::JoinLine(int forward, MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeJoinLineState *state = dynamic_cast<MfPeJoinLineState *>(editorState);
    MfPeTextLine *line;

    if (forward) {
        line = LineOf(mCursorLine);
        GetCursor(state->mCursor);
    } else {
        MfDblListItem *item = mCursorLine;
        line = LineOf(MfDblList::PrevItem(item));
        GetCursor(state->mCursor);
        state->mCursor.x = line->mText.CountCharacters(line->mText.Length(), mTabSize) + 1;
        state->mCursor.y--;
    }
    state->mForward = forward;
    state->mLength = line->mText.Length();
    JoinLines(forward);
    context->InvalidateFrom(mCursor.y);
    context->InvalidateMarginFrom(mCursor.y);
    context->Update();
    context->SetLineCount(mLines->GetCount());
    ShowCursor(context);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
}

// 0x10016350 ?UndoJoinLine@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::UndoJoinLine(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeJoinLineState *state = dynamic_cast<MfPeJoinLineState *>(editorState);

    SetCursor(state->mCursor.x, state->mCursor.y);
    int line = mCursor.y;
    MfDblListItem *item = mCursorLine;
    SplitLine();
    if (state->mForward) {
        MfString &text = LineOf(item)->mText;
        if (text.Length() > state->mLength)
            text.Truncate(state->mLength);
        mCursorLine = item;
        mCursor.y = line;
        text.CuBindToCursor(0, &mStringCursor);
        mCursor.x = text.CuEndOfString(&mStringCursor) + 1;
    } else {
        mCursor.x = LineOf(mCursorLine)->mText.CuBindToCursor(0, &mStringCursor) + 1;
    }
    context->InvalidateFrom(line);
    context->Update();
    context->SetLineCount(mLines->GetCount());
    ShowCursor(context);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
}

// 0x10016460 ?SplitLine@MfPeEditorData@@AAEXXZ
void MfPeEditorData::SplitLine()
{
    MfPeTextLine *line = LineOf(mCursorLine);
    MfPeTextLine *newLine = new MfPeTextLine;

    if (mStringCursor.CursorPos() < mStringCursor.ExpandedSize()) {
        newLine->mText = line->mText.RightString(line->mText.Length() - mStringCursor.CharPos());
        line->mText = line->mText.LeftString(mStringCursor.CharPos());
    }
    mLines->Insert(newLine, MfDblList::LP_AFTER, mCursorLine);
    mCursor.y++;
    MfDblList::NextItem(mCursorLine);
    mCursor.x = newLine->mText.CuBindToCursor(0, &mStringCursor) + 1;
}

// TODO 0x100165a0 ?NewLine@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: 44% - this build expands more inline or lays the branches out longer (422 bytes instead of 413)
void MfPeEditorData::NewLine(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeNewLineState *state = dynamic_cast<MfPeNewLineState *>(editorState);

    GetCursor(state->mCursor);
    CPoint cursor = state->mCursor;
    if (mStringCursor.CursorPos() >= mStringCursor.ExpandedSize()) {
        MfString &text = LineOf(mCursorLine)->mText;
        state->mCursor.x = text.CountCharacters(text.Length(), mTabSize) + 1;
    }
    context->ShowLine(mCursor, 0);
    SplitLine();
    if (mAutoIndent && LineOf(mCursorLine)->mText == NULL_STRING) {
        MfDblListItem *item = mCursorLine;
        MfPeTextLine *prev = LineOf(MfDblList::PrevItem(item));
        if (prev) {
            MfStringCursor indent(mTabSize);
            prev->mText.CuBindToCursor(0, &indent);
            MfPeTextLine *line = LineOf(mCursorLine);
            unsigned int column;
            if (prev->mText.Length() != 0) {
                WhiteSpaceScanner white;
                column = prev->mText.CuScanForward(&indent, white);
            } else {
                column = cursor.x - 1;
            }
            mCursor.x = line->mText.CuSetCursorPos(column, &mStringCursor) + 1;
        }
    }
    context->InvalidateFrom(mCursor.y - 1);
    context->InvalidateMarginFrom(mCursor.y - 1);
    context->Update();
    context->SetLineCount(mLines->GetCount());
    ShowCursor(context);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
}

// 0x10016740 ?UndoNewLine@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::UndoNewLine(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeNewLineState *state = dynamic_cast<MfPeNewLineState *>(editorState);

    SetCursor(state->mCursor.x, state->mCursor.y);
    JoinLines(1);
    context->InvalidateFrom(mCursor.y);
    context->Update();
    context->SetLineCount(mLines->GetCount());
    ShowCursor(context);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
}

// TODO 0x100167c0 ?DeleteRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: 86% - the original lays the branches out longer (243 bytes instead of 239)
void MfPeEditorData::DeleteRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDeleteRegionState *state = dynamic_cast<MfPeDeleteRegionState *>(editorState);

    state->mText.Clear();
    state->mCursor = mSelection.mStart;
    if (HasSelection()) {
        GotoSelectionStart(context);
        GetRegionText(1, &state->mText, 0);
        BindCursor();
        ShowCursor(context);
        mSelection.Reset();
        mSelection.mActive = 0;
        context->SetLineCount(mLines->GetCount());
        context->ShowLine(mCursor, 0);
        context->ShowColumn(mCursor);
        context->InvalidateFrom(mCursor.y);
        context->Update();
        context->UpdateCaretForSelection();
    }
}

// EQUIV 0x100168c0 ?CopyRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: reviewed: stack slot of the this copy only
void MfPeEditorData::CopyRegion(MfPeEditorState *, MfPeDrawingContext *context)
{
    if (HasSelection()) {
        MfStringList text;
        GetRegionText(0, &text, 0);
        CopyToClipboard(&text, context);
    }
}

// EQUIV 0x100169b0 ?CutRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: reviewed: an independent store and load swap places
void MfPeEditorData::CutRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDeleteRegionState *state = dynamic_cast<MfPeDeleteRegionState *>(editorState);

    state->mText.Clear();
    state->mCursor = mSelection.mStart;
    if (HasSelection()) {
        GotoSelectionStart(context);
        context->ShowLine(mCursor, 0);
        context->ShowColumn(mCursor);
        GetRegionText(1, &state->mText, 0);
        CopyToClipboard(&state->mText, context);
        BindCursor();
        ShowCursor(context);
        mSelection.Reset();
        mSelection.mActive = 0;
        context->SetLineCount(mLines->GetCount());
        context->InvalidateFrom(mCursor.y);
        context->Update();
        context->UpdateCaretForSelection();
    }
}

// 0x10016ab0 ?UndoDeleteRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::UndoDeleteRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDeleteRegionState *state = dynamic_cast<MfPeDeleteRegionState *>(editorState);

    SetCursor(state->mCursor.x, state->mCursor.y);
    InsertLines(&state->mText);
    context->SetLineCount(mLines->GetCount());
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    ShowCursor(context);
    context->InvalidateFrom(context->mTopLine);
    context->Update();
}

// EQUIV 0x10016b40 ?PasteRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: reviewed: stack slot of the this copy only
void MfPeEditorData::PasteRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPePasteRegionState *state = dynamic_cast<MfPePasteRegionState *>(editorState);

    state->mLength = LineOf(mCursorLine)->mText.Length();
    MfStringList text;
    PasteFromClipboard(&text, context);
    GetCursor(state->mStart);
    InsertLines(&text);
    GetCursor(state->mEnd);
    context->SetLineCount(mLines->GetCount());
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    ShowCursor(context);
    context->InvalidateFrom(context->mTopLine);
    context->Update();
}

// TODO 0x10016ca0 ?UndoPasteRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: 26% - the original expands MfString::CopyOnWrite differently (VC5's per-function inline budget)
void MfPeEditorData::UndoPasteRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPePasteRegionState *state = dynamic_cast<MfPePasteRegionState *>(editorState);

    mSelection.Set(state->mStart, state->mStart);
    mSelection.Extend(state->mEnd, 0, context);
    GotoSelectionStart(context);
    context->ShowLine(mCursor, 0);
    MfStringList text;
    GetRegionText(1, &text, 0);
    MfString &line = LineOf(mCursorLine)->mText;
    unsigned int column;
    if (line.Length() > state->mLength) {
        line.Truncate(state->mLength);
        line.CuBindToCursor(0, &mStringCursor);
        column = line.CuEndOfString(&mStringCursor);
    } else {
        column = line.CuBindToCursor(mCursor.x - 1, &mStringCursor);
    }
    mCursor.x = column + 1;
    ShowCursor(context);
    mSelection.Reset();
    mSelection.mActive = 0;
    context->InvalidateFrom(mCursor.y);
    context->Update();
    context->SetLineCount(mLines->GetCount());
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    context->UpdateCaretForSelection();
}

// TODO 0x10016e70 ?DragRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: 45% - same calls on each path (checked: both copies run CuBindToCursor/CuSetCharPos/CuSetCursorPos through tails shared with the single-line case, merged at different points); the original keeps count and the selection end in registers (0x28 bytes of locals, 0x34 here); using mSelection.mEnd directly instead of the local gives 39%
void MfPeEditorData::DragRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDragRegionState *state = dynamic_cast<MfPeDragRegionState *>(editorState);
    int charPos = mStringCursor.CharPos();
    int beyond = 0;

    if (mStringCursor.CursorPos() >= mStringCursor.ExpandedSize())
        beyond = mStringCursor.CursorPos() - mStringCursor.ExpandedSize();
    MfStringList text;
    MfPeRegionText taken;
    GetRegionText(1, &text, &taken);
    state->mFrom = mSelection.mStart;
    int count = text.GetCount();
    CPoint end = mSelection.mEnd;
    if (count > 1) {
        if (mCursor.y >= end.y) {
            MfDblListItem *firstItem = text.FirstItem();
            MfString *first = firstItem ? (MfString *)firstItem->pData : 0;
            if (mCursor.y == end.y) {
                first->CountCharacters(first->Length(), mTabSize);
                mCursor.y += 1 - count;
                mCursorLine = GetLineItem(mCursor.y);
                MfString &line = LineOf(mCursorLine)->mText;
                line.CuBindToCursor(0, &mStringCursor);
                mCursor.x = line.CuSetCharPos(taken.mFirstLength - taken.mFirstTaken - taken.mLastTaken + charPos,
                                              &mStringCursor) + 1;
                mCursor.x = line.CuSetCursorPos(beyond + mCursor.x - 1, &mStringCursor) + 1;
            } else {
                mCursor.y = mCursor.y - count + 1;
                BindCursor();
            }
        }
    } else if (count == 1 && mCursor.y == end.y && mCursor.x > end.x) {
        MfString &line = LineOf(mCursorLine)->mText;
        line.CuBindToCursor(0, &mStringCursor);
        mCursor.x = line.CuSetCharPos(charPos - taken.mFirstTaken, &mStringCursor) + 1;
        mCursor.x = line.CuSetCursorPos(beyond + mCursor.x - 1, &mStringCursor) + 1;
    }
    state->mLength = LineOf(mCursorLine)->mText.Length();
    GetCursor(state->mTo);
    InsertLines(&text);
    GetCursor(state->mEnd);
    mSelection.mActive = 1;
    mSelection.Set(state->mTo, state->mTo);
    mSelection.Extend(state->mEnd, 0, context);
    context->SetLineCount(mLines->GetCount());
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    ShowCursor(context);
    context->InvalidateFrom(context->mTopLine);
    context->Update();
}

// TODO 0x10017160 ?UndoDragRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: 57% - the original expands MfString::CopyOnWrite differently (VC5's per-function inline budget)
void MfPeEditorData::UndoDragRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDragRegionState *state = dynamic_cast<MfPeDragRegionState *>(editorState);

    mSelection.Set(state->mTo, state->mTo);
    mSelection.Extend(state->mEnd, 0, context);
    MfStringList text;
    GetRegionText(1, &text, 0);
    SetCursor(state->mTo.x, state->mTo.y);
    MfPeTextLine *line = LineOf(mCursorLine);
    if (line->mText.Length() > state->mLength)
        line->mText.Truncate(state->mLength);
    SetCursor(state->mFrom.x, state->mFrom.y);
    InsertLines(&text);
    context->SetLineCount(mLines->GetCount());
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    ShowCursor(context);
    context->InvalidateFrom(context->mTopLine);
    context->Update();
}

// TODO 0x10017370 ?SpansLines@MfPeEditorData@@QAEHXZ: 83% - the original lays the branches out longer (96 bytes instead of 93)
int MfPeEditorData::SpansLines()
{
    int spans = 0;
    int startLine = mSelection.mStart.y;
    int endColumn = mSelection.mEnd.x;
    int endLine = mSelection.mEnd.y;
    MfPeTextLine *last = LineOf(mLines->LastItem());
    int width = last->mText.CountCharacters(last->mText.Length(), mTabSize);

    if (startLine != endLine)
        return 1;
    if (endLine == mLines->GetCount() && endColumn >= width)
        return 1;
    return spans;
}

// TODO 0x100173d0 ?CanUndent@MfPeEditorData@@QAEHXZ: 55% - this build expands more inline or lays the branches out longer (154 bytes instead of 140)
int MfPeEditorData::CanUndent()
{
    int found = 0;
    int number = mSelection.mStart.y;
    int last = mSelection.mEnd.y;

    if (!(number == mLines->GetCount() && mSelection.mEnd.x > 1))
        last--;
    MfDblListItem *item = GetLineItem(number);
    MfPeTextLine *line = LineOf(item);
    for (; number <= last && !found; number++) {
        if (isspace(line->mText.GetCharacter(0)))
            found = 1;
        line = (MfPeTextLine *)MfDblList::Next(item);
    }
    return found;
}

// TODO 0x10017460 ?SelectWholeLines@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@H@Z: 46% - the original lays the branches out longer (265 bytes instead of 245)
void MfPeEditorData::SelectWholeLines(MfPeDrawingContext *context, int update)
{
    CPoint start = mSelection.mStart;
    CPoint end = mSelection.mEnd;

    if (start.x > 1)
        start.x = 1;
    if (end.x > 1) {
        if (end.y < mLines->GetCount()) {
            end.y++;
            end.x = 1;
        } else {
            MfStringCursor cursor(mTabSize);
            MfString &text = LineOf(mLines->LastItem())->mText;
            text.CuBindToCursor(0, &cursor);
            end.x = text.CuEndOfString(&cursor) + 1;
        }
    }
    SetCursor(start.x, start.y);
    ShowCursor(context);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    mSelection.Set(start, start);
    mSelection.Extend(end, 0, context);
    if (update) {
        context->InvalidateFrom(mCursor.y);
        context->Update();
    }
}

// 0x10017570 ?IndentRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::IndentRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeIndentRegionState *state = dynamic_cast<MfPeIndentRegionState *>(editorState);

    state->mStart = mSelection.mStart;
    state->mEnd = mSelection.mEnd;
    int last = state->mEnd.y;
    if (!(last == mLines->GetCount() && state->mEnd.x > 1))
        last--;
    MfDblListItem *item = mCursorLine;
    MfPeTextLine *line = LineOf(item);
    for (int number = state->mStart.y; number <= last; number++) {
        line->mText = MfString("\t") + line->mText;
        line = (MfPeTextLine *)MfDblList::Next(item);
    }
    BindCursor();
    ShowCursor(context);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// 0x100176c0 ?UndoIndentRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::UndoIndentRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeIndentRegionState *state = dynamic_cast<MfPeIndentRegionState *>(editorState);

    mSelection.mActive = 0;
    mSelection.Set(state->mStart, state->mEnd);
    mSelection.Extend(state->mEnd, 0, context);
    SetCursor(state->mStart.x, state->mStart.y);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    int last = state->mEnd.y;
    if (!(last == mLines->GetCount() && state->mEnd.x > 1))
        last--;
    MfDblListItem *item = mCursorLine;
    MfPeTextLine *line = LineOf(item);
    for (int number = state->mStart.y; number <= last; number++) {
        line->mText = line->mText.RightString(line->mText.Length() - 1);
        line = (MfPeTextLine *)MfDblList::Next(item);
    }
    BindCursor();
    ShowCursor(context);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// EQUIV 0x10017850 ?UndentRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: reviewed: register and slot choice; one vtable load is scheduled above two unrelated member stores
void MfPeEditorData::UndentRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeUndentRegionState *state = dynamic_cast<MfPeUndentRegionState *>(editorState);

    state->mStart = mSelection.mStart;
    state->mEnd = mSelection.mEnd;
    state->mRemoved.Clear();
    int last = state->mEnd.y;
    if (!(last == mLines->GetCount() && state->mEnd.x > 1))
        last--;
    MfDblListItem *item = mCursorLine;
    MfPeTextLine *line = LineOf(item);
    for (int number = state->mStart.y; number <= last; number++) {
        MfString &text = line->mText;
        MfStringCursor cursor(mTabSize);
        text.CuBindToCursor(0, &cursor);
        while (cursor.CursorPos() < (unsigned int)mTabSize && isspace(text.GetCharacter(cursor.CharPos())))
            text.CuMoveRight(&cursor);
        state->mRemoved.Add(text.LeftString(cursor.CharPos()));
        text = text.RightString(text.Length() - cursor.CharPos());
        line = (MfPeTextLine *)MfDblList::Next(item);
    }
    BindCursor();
    ShowCursor(context);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// TODO 0x10017a40 ?UndoUndentRegion@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: 57% - this build expands more inline or lays the branches out longer (419 bytes instead of 411)
void MfPeEditorData::UndoUndentRegion(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeUndentRegionState *state = dynamic_cast<MfPeUndentRegionState *>(editorState);

    mSelection.mActive = 0;
    mSelection.Set(state->mStart, state->mEnd);
    mSelection.Extend(state->mEnd, 0, context);
    SetCursor(state->mStart.x, state->mStart.y);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    int last = state->mEnd.y;
    if (!(last == mLines->GetCount() && state->mEnd.x > 1))
        last--;
    MfDblListItem *item = mCursorLine;
    MfPeTextLine *line = LineOf(item);
    MfDblListItem *removedItem = state->mRemoved.FirstItem();
    MfString *removed = removedItem ? (MfString *)removedItem->pData : 0;
    for (int number = state->mStart.y; number <= last; number++) {
        line->mText = *removed + line->mText;
        line = (MfPeTextLine *)MfDblList::Next(item);
        removed = (MfString *)MfDblList::Next(removedItem);
    }
    BindCursor();
    ShowCursor(context);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// TODO 0x10017be0 ?DeleteBackWord@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: 46% - this build expands more inline or lays the branches out longer (872 bytes instead of 871)
void MfPeEditorData::DeleteBackWord(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDeleteBackWordState *state = dynamic_cast<MfPeDeleteBackWordState *>(editorState);

    if (mStringCursor.CursorPos() >= mStringCursor.ExpandedSize())
        mCursor.x = LineOf(mCursorLine)->mText.CuEndOfString(&mStringCursor) + 1;
    state->mFrom = mCursor;
    int end = mStringCursor.CharPos();
    MfPeTextLine *line = LineOf(mCursorLine);
    MfPeTextLine *current = line;
    char c = 0;
    if (end)
        c = line->mText.GetCharacter(end - 1);
    int start = end;
    int joined = 0;
    if (c) {
        if (isspace(c)) {
            WhiteSpaceScanner white;
            line->mText.CuScanBackward(&mStringCursor, white);
            if (mStringCursor.CharPos() > 0)
                c = line->mText.GetCharacter(mStringCursor.CharPos() - 1);
            else
                joined = 1;
        }
        if (isalnum(c) || c == '_') {
            IdentScanner ident;
            line->mText.CuScanBackward(&mStringCursor, ident);
        } else if (ispunct(c)) {
            line->mText.CuMoveLeft(&mStringCursor);
        }
        if (mStringCursor.CharPos() != end) {
            MfString &text = line->mText;
            state->mWord = text.MidString(mStringCursor.CharPos(), end - mStringCursor.CharPos());
            text = text.LeftString(mStringCursor.CharPos()) + text.RightString(text.Length() - end);
        }
        start = mStringCursor.CharPos();
    } else if (end == 0) {
        joined = 1;
    }
    if (joined) {
        MfDblListItem *prevItem = mCursorLine;
        MfPeTextLine *prev = LineOf(MfDblList::PrevItem(prevItem));
        if (prevItem
            && prev->mText.CountCharacters(prev->mText.Length(), mTabSize)
                       + line->mText.CountCharacters(line->mText.Length(), mTabSize)
                   < 255) {
            mCursor.y--;
            int length = prev->mText.Length();
            prev->mText << line->mText;
            mLines->Delete(mCursorLine);
            mCursorLine = prevItem;
            current = prev;
            start = length;
        }
    }
    MfString &text = current->mText;
    text.CuBindToCursor(0, &mStringCursor);
    mCursor.x = text.CuSetCharPos(start, &mStringCursor) + 1;
    state->mTo = mCursor;
    state->mJoined = joined;
    ShowCursor(context);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// 0x10017f50 ?UndoDeleteBackWord@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z
void MfPeEditorData::UndoDeleteBackWord(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDeleteBackWordState *state = dynamic_cast<MfPeDeleteBackWordState *>(editorState);

    SetCursor(state->mTo.x, state->mTo.y);
    if (state->mJoined)
        SplitLine();
    MfPeTextLine *line = LineOf(mCursorLine);
    if (state->mWord != NULL_STRING) {
        MfString &text = line->mText;
        text = text.LeftString(mStringCursor.CharPos()) + state->mWord
               + text.RightString(text.Length() - mStringCursor.CharPos());
    }
    mCursor.x = line->mText.CuBindToCursor(state->mFrom.x - 1, &mStringCursor) + 1;
    ShowCursor(context);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    context->InvalidateFrom(state->mTo.y);
    context->Update();
}

// TODO 0x100180f0 ?DeleteForwardWord@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: 65% - this build expands more inline or lays the branches out longer (804 bytes instead of 791)
void MfPeEditorData::DeleteForwardWord(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDeleteForwardWordState *state = dynamic_cast<MfPeDeleteForwardWordState *>(editorState);

    if (mStringCursor.CursorPos() >= mStringCursor.ExpandedSize())
        mCursor.x = LineOf(mCursorLine)->mText.CuEndOfString(&mStringCursor) + 1;
    state->mCursor = mCursor;
    int start = mStringCursor.CharPos();
    MfString &text = LineOf(mCursorLine)->mText;
    char c = text.GetCharacter(start);
    int joined = 0;
    if (c) {
        int atSpace = 0;
        if (isalnum(c) || c == '_') {
            IdentScanner ident;
            text.CuScanForward(&mStringCursor, ident);
        } else if (ispunct(c)) {
            text.CuMoveRight(&mStringCursor);
        } else if (isspace(c)) {
            atSpace = 1;
        }
        if (isspace(text.GetCharacter(mStringCursor.CharPos()))) {
            WhiteSpaceScanner white;
            text.CuScanForward(&mStringCursor, white);
        }
        if (atSpace && mStringCursor.CharPos() == text.Length())
            joined = 1;
        if (mStringCursor.CharPos() != start) {
            state->mWord = text.MidString(start, mStringCursor.CharPos() - start);
            text = text.LeftString(start) + text.RightString(text.Length() - mStringCursor.CharPos());
        }
    } else if (start == text.Length()) {
        joined = 1;
    }
    if (joined) {
        MfDblListItem *nextItem = mCursorLine;
        MfPeTextLine *next = LineOf(MfDblList::NextItem(nextItem));
        if (nextItem
            && next->mText.CountCharacters(next->mText.Length(), mTabSize)
                       + text.CountCharacters(text.Length(), mTabSize)
                   < 255) {
            text << next->mText;
            mLines->Delete(nextItem);
        }
    }
    text.CuBindToCursor(0, &mStringCursor);
    mCursor.x = text.CuSetCharPos(start, &mStringCursor) + 1;
    state->mJoined = joined;
    ShowCursor(context);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    context->InvalidateFrom(mCursor.y);
    context->Update();
}

// TODO 0x10018410 ?UndoDeleteForwardWord@MfPeEditorData@@QAEXPAVMfPeEditorState@@PAVMfPeDrawingContext@@@Z: 68% - register allocation and instruction order differ
void MfPeEditorData::UndoDeleteForwardWord(MfPeEditorState *editorState, MfPeDrawingContext *context)
{
    MfPeDeleteForwardWordState *state = dynamic_cast<MfPeDeleteForwardWordState *>(editorState);

    SetCursor(state->mCursor.x, state->mCursor.y);
    if (state->mJoined) {
        SplitLine();
        MfDblList::PrevItem(mCursorLine);
        MfString &text = LineOf(mCursorLine)->mText;
        mCursor.y--;
        text.CuBindToCursor(0, &mStringCursor);
        mCursor.x = text.CuEndOfString(&mStringCursor) + 1;
    }
    MfPeTextLine *line = LineOf(mCursorLine);
    if (state->mWord != NULL_STRING) {
        MfString &text = line->mText;
        text = text.LeftString(mStringCursor.CharPos()) + state->mWord
               + text.RightString(text.Length() - mStringCursor.CharPos());
    }
    mCursor.x = line->mText.CuBindToCursor(state->mCursor.x - 1, &mStringCursor) + 1;
    ShowCursor(context);
    context->ShowLine(mCursor, 0);
    context->ShowColumn(mCursor);
    context->InvalidateFrom(state->mCursor.y);
    context->Update();
}

// 0x100185f0 ?GotoSelectionStart@MfPeEditorData@@AAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::GotoSelectionStart(MfPeDrawingContext *context)
{
    int line = mSelection.mStart.y;
    int column = mSelection.mStart.x;
    MfDblListItem *item = mCursorLine;

    if (mCursor.y != line)
        item = GetLineItem(line);
    MfPeTextLine *start = LineOf(item);
    mCursor = CPoint(column, line);
    mCursor.x = start->mText.CuBindToCursor(mCursor.x - 1, &mStringCursor) + 1;
    ShowCursor(context);
    mCursorLine = item;
}

// TODO 0x10018650 ?GetRegionText@MfPeEditorData@@AAEXHPAVMfStringList@@PAUMfPeRegionText@@@Z: 40% - calls, their order and the EH states now agree (the single-line removal keeps the left part in a named string that lives until after Scan); the original uses edi for this and keeps fewer values in stack slots (0x2c bytes of locals here 0x3c)
void MfPeEditorData::GetRegionText(int remove, MfStringList *text, MfPeRegionText *taken)
{
    if (mSelection.mStart.x == mSelection.mEnd.x && mSelection.mStart.y == mSelection.mEnd.y)
        return;
    CPoint start = mSelection.mStart;
    CPoint end = mSelection.mEnd;
    MfDblListItem *item = mCursorLine;
    if (mCursor.y != start.y)
        item = GetLineItem(start.y);
    MfPeTextLine *first = mLines->Current(item);
    text->Clear();
    if (start.y == end.y) {
        MfStringCursor cursor(mTabSize);
        MfString &line = first->mText;
        line.CuBindToCursor(start.x - 1, &cursor);
        if (cursor.CursorPos() >= cursor.ExpandedSize())
            return;
        int from = cursor.CharPos();
        line.CuSetCursorPos(end.x - 1, &cursor);
        int to = cursor.CharPos();
        int length = to - from;
        text->Add(line.MidString(from, length));
        if (taken) {
            taken->mLastTaken = length;
            taken->mLastLength = line.Length();
            taken->mFirstLength = line.Length();
            taken->mFirstTaken = length;
        }
        if (!remove)
            return;
        MfString left = line.LeftString(from);
        line = left + line.RightString(line.Length() - to);
        if (mSyntaxHighlight)
            first->Scan(mTabSize);
        return;
    }
    MfStringCursor cursor(mTabSize);
    MfString &firstText = first->mText;
    firstText.CuBindToCursor(start.x - 1, &cursor);
    if (taken) {
        taken->mFirstLength = firstText.Length();
        taken->mFirstTaken = cursor.CursorPos() < cursor.ExpandedSize() ? firstText.Length() - cursor.CharPos() : 0;
    }
    if (cursor.CursorPos() < cursor.ExpandedSize() || cursor.ExpandedSize() == 0) {
        text->Add(firstText.RightString(firstText.Length() - cursor.CharPos()));
        if (remove)
            firstText = firstText.LeftString(cursor.CharPos());
    }
    MfDblList::NextItem(item);
    for (int number = start.y + 1; number < end.y; number++) {
        MfDblListItem *next = item;
        MfDblList::NextItem(next);
        text->Add(LineOf(item)->mText);
        if (remove)
            mLines->Delete(item);
        item = next;
    }
    MfString &lastText = LineOf(item)->mText;
    lastText.CuBindToCursor(end.x - 1, &cursor);
    if (taken) {
        taken->mLastTaken = cursor.CharPos();
        taken->mLastLength = lastText.Length();
    }
    text->Add(lastText.LeftString(cursor.CharPos()));
    if (!remove)
        return;
    firstText << lastText.RightString(lastText.Length() - cursor.CharPos());
    mLines->Delete(item);
    if (mSyntaxHighlight)
        first->Scan(mTabSize);
}

// TODO 0x10018a50 ?InsertLines@MfPeEditorData@@AAEXPAVMfStringList@@@Z: 58% - the original calls MfArray's constructor out of line (VC5's per-function inline budget), this build inlines it
void MfPeEditorData::InsertLines(MfStringList *text)
{
    if (text->GetCount() == 0)
        return;
    MfPeTextLine *line = mLines->Current(mCursorLine);
    MfString *lineText = &line->mText;
    MfString rest = lineText->RightString(lineText->Length() - mStringCursor.CharPos());
    *lineText = lineText->LeftString(mStringCursor.CharPos());
    if (mStringCursor.CursorPos() >= mStringCursor.ExpandedSize())
        line->FillTo(&mStringCursor);
    MfDblListItem *item = text->FirstItem();
    MfString *insert = item ? (MfString *)item->pData : 0;
    *lineText << *insert;
    int inComment = 0;
    if (mSyntaxHighlight) {
        line->Scan(mTabSize);
        if (line->mSyntax.mFlags & (MfPeLineSyntax::COMMENT_START | MfPeLineSyntax::IN_COMMENT))
            inComment = 1;
    }
    int column;
    if (text->GetCount() == 1) {
        *lineText << rest;
        column = insert->Length() + mStringCursor.CharPos();
    } else if (text->GetCount() > 1) {
        for (insert = (MfString *)MfDblList::Next(item); insert; insert = (MfString *)MfDblList::Next(item)) {
            mLines->Insert(new MfPeTextLine(*insert), MfDblList::LP_AFTER, mCursorLine);
            MfDblList::NextItem(mCursorLine);
            line = LineOf(mCursorLine);
            mCursor.y++;
            if (mSyntaxHighlight) {
                if (inComment)
                    line->mSyntax.mFlags |= MfPeLineSyntax::IN_COMMENT;
                line->Scan(mTabSize);
                inComment = (line->mSyntax.mFlags & (MfPeLineSyntax::COMMENT_START | MfPeLineSyntax::IN_COMMENT)) != 0;
            }
        }
        line = LineOf(mCursorLine);
        lineText = &line->mText;
        column = lineText->Length();
        *lineText << rest;
        if (mSyntaxHighlight && inComment)
            line->mSyntax.mFlags |= MfPeLineSyntax::IN_COMMENT;
    }
    lineText->CuBindToCursor(0, &mStringCursor);
    mCursor.x = lineText->CuSetCharPos(column, &mStringCursor) + 1;
}

// TODO 0x10018cf0 ?CopyToClipboard@MfPeEditorData@@AAEXPAVMfStringList@@PAVMfPeDrawingContext@@@Z: 70% - the original lays the branches out longer (314 bytes instead of 296)
void MfPeEditorData::CopyToClipboard(MfStringList *text, MfPeDrawingContext *context)
{
    if (!context->OpenClipboard(1))
        return;
    unsigned int size = 0;
    MfDblListItem *item = text->FirstItem();
    for (MfString *line = item ? (MfString *)item->pData : 0; line; line = (MfString *)MfDblList::Next(item))
        size += line->Length();
    if (text->GetCount() > 0)
        size += text->GetCount() * 2 - 2;
    if (size > 0) {
        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, size + 1);
        char *buffer = (char *)GlobalLock(memory);
        int i = 0;
        item = text->FirstItem();
        for (MfString *line = item ? (MfString *)item->pData : 0; line; line = (MfString *)MfDblList::Next(item)) {
            int length = line->Length();
            memcpy(buffer, (char *)*line, length);
            buffer += length;
            if (i < text->GetCount() - 1) {
                memcpy(buffer, "\r\n", 2);
                buffer += 2;
            }
            i++;
        }
        *buffer = 0;
        GlobalUnlock(memory);
        SetClipboardData(CF_TEXT, memory);
    }
    context->CloseClipboard();
}

// EQUIV 0x10018e30 ?PasteFromClipboard@MfPeEditorData@@AAEXPAVMfStringList@@PAVMfPeDrawingContext@@@Z: reviewed: the string's this comes from the constructor's return value instead of its frame address (the same object)
void MfPeEditorData::PasteFromClipboard(MfStringList *text, MfPeDrawingContext *context)
{
    if (IsClipboardFormatAvailable(CF_TEXT) && context->OpenClipboard(0)) {
        HANDLE data = GetClipboardData(CF_TEXT);
        if (data) {
            char *buffer = (char *)GlobalLock(data);
            if (buffer)
                MfString(buffer).ConvertToLines(text);
            GlobalUnlock(data);
        }
        context->CloseClipboard();
    }
}

// TODO 0x10018ee0 ?Find@MfPeEditorData@@QAEHABVMfString@@PAVMfPeDrawingContext@@HH@Z: 59% - the original calls MfPeRegion::IsEmpty out of line and keeps the line and column in other registers
int MfPeEditorData::Find(const MfString &text, MfPeDrawingContext *context, int backwards, int wrap)
{
    int found = 0;
    if (text == NULL_STRING)
        return 0;

    MfDblListItem *item;
    int number;
    int column;
    if (mSelection.mActive && !mSelection.IsEmpty()) {
        if (backwards) {
            column = mSelection.mStart.x;
            number = mSelection.mStart.y;
        } else {
            column = mSelection.mEnd.x;
            number = mSelection.mEnd.y;
        }
        item = GetLineItem(number);
    } else {
        item = mCursorLine;
        number = mCursor.y;
        column = mStringCursor.CursorPos() + 1;
    }
    MfDblListItem *startItem = item;
    if (backwards)
        column--;
    MfStringCursor cursor(mTabSize);
    MfString *line = &LineOf(item)->mText;
    line->CuBindToCursor(column - 1, &cursor);
    int done = 0;
    int wrapped = 0;
    do {
        int position;
        if (backwards)
            position = line->ReverseCaseContainsSubString(text, cursor.CharPos());
        else
            position = line->CaseContainsSubString(text, cursor.CharPos());
        if (position != -1 && (backwards == 0 || (backwards == 1 && (unsigned int)position <= (unsigned int)cursor.CharPos()))) {
            line->CuBindToCursor(0, &mStringCursor);
            mCursor.x = line->CuSetCharPos(position, &mStringCursor) + 1;
            mCursor.y = number;
            mCursorLine = item;
            mSelection.mActive = 0;
            mSelection.Invalidate(context);
            context->Update();
            ShowCursor(context);
            if (mCursor.y < context->mTopLine || mCursor.y > context->mTopLine + context->mFullLines - 1)
                context->ShowLine(mCursor, 1 - context->mFullLines / 2);
            context->ShowColumn(mCursor);
            mSelection.Invalidate(context);
            mSelection.Set(mCursor, CPoint(line->CuSetCharPos(text.Length() + position, &cursor) + 1, number));
            mSelection.mActive = 1;
            context->InvalidateLine(mCursor.y);
            context->UpdateCaretForSelection();
            context->Update();
            done = 1;
            found = 1;
        } else {
            if (wrapped == 1 && item == startItem)
                return 0;
            MfPeTextLine *next;
            if (backwards) {
                next = LineOf(MfDblList::PrevItem(item));
                number--;
            } else {
                next = LineOf(MfDblList::NextItem(item));
                number++;
            }
            if (next == 0) {
                if (!wrap)
                    return 0;
                context->ShowMessage("Passed the end of the buffer");
                if (backwards) {
                    item = mLines->LastItem();
                    next = LineOf(item);
                    number = mLines->GetCount();
                } else {
                    item = mLines->FirstItem();
                    next = LineOf(item);
                    number = 1;
                }
                wrapped = 1;
            }
            if (next == 0)
                return 0;
            line = &next->mText;
            line->CuBindToCursor(0, &cursor);
            if (backwards)
                line->CuEndOfString(&cursor);
        }
    } while (!done);
    return found;
}

// TODO 0x10019290 ?IsEmpty@MfPeRegion@@QBEHXZ: an inline that this build expands everywhere; the original's Find calls it out of line

// EQUIV 0x100192b0 ?GotoLine@MfPeEditorData@@QAEXHHPAVMfPeDrawingContext@@@Z: reviewed: an independent store and address computation swap places
void MfPeEditorData::GotoLine(int line, int mark, MfPeDrawingContext *context)
{
    if (line < 1 || line > mLines->GetCount())
        return;
    mCursorLine = GetLineItem(line);
    MfPeTextLine *current = LineOf(mCursorLine);
    mCursor.x = current->mText.CuBindToCursor(0, &mStringCursor) + 1;
    mCursor.y = line;
    mSelection.mActive = 0;
    mSelection.Invalidate(context);
    context->Update();
    ShowCursor(context);
    if (mCursor.y < context->mTopLine || mCursor.y > context->mTopLine + context->mFullLines - 1)
        context->ShowLine(mCursor, 1 - context->mFullLines / 2);
    context->ShowColumn(mCursor);
    if (mark) {
        ClearGlyph(8, context);
        current->mGlyphs |= 8;
        context->InvalidateMargin(line);
    }
}

// 0x10019390 ?SetGlyph@MfPeEditorData@@QAEXHHPAVMfPeDrawingContext@@@Z
void MfPeEditorData::SetGlyph(int line, int glyphs, MfPeDrawingContext *context)
{
    if (line < 1 || line > mLines->GetCount())
        return;
    LineOf(GetLineItem(line))->mGlyphs |= glyphs;
    context->InvalidateMargin(line);
}

// 0x100193d0 ?UnsetGlyph@MfPeEditorData@@QAEXHHPAVMfPeDrawingContext@@@Z
void MfPeEditorData::UnsetGlyph(int line, int glyphs, MfPeDrawingContext *context)
{
    if (line < 1 || line > mLines->GetCount())
        return;
    LineOf(GetLineItem(line))->mGlyphs &= ~glyphs;
    context->InvalidateMargin(line);
}

// 0x10019410 ?ClearGlyphsOnLine@MfPeEditorData@@QAEXHPAVMfPeDrawingContext@@@Z
void MfPeEditorData::ClearGlyphsOnLine(int line, MfPeDrawingContext *context)
{
    if (line < 1 || line > mLines->GetCount())
        return;
    LineOf(GetLineItem(line))->mGlyphs = 0;
    context->InvalidateMargin(line);
}

// 0x10019450 ?ClearGlyphs@MfPeEditorData@@QAEXPAVMfPeDrawingContext@@@Z
void MfPeEditorData::ClearGlyphs(MfPeDrawingContext *context)
{
    MfDblListItem *item = mLines->FirstItem();
    int number = 1;
    for (MfPeTextLine *line = LineOf(item); line; line = (MfPeTextLine *)MfDblList::Next(item)) {
        line->mGlyphs = 0;
        context->InvalidateMargin(number);
        number++;
    }
}

// 0x100194a0 ?ClearGlyph@MfPeEditorData@@QAEXHPAVMfPeDrawingContext@@@Z
void MfPeEditorData::ClearGlyph(int glyphs, MfPeDrawingContext *context)
{
    MfDblListItem *item = mLines->FirstItem();
    int number = 1;
    for (MfPeTextLine *line = LineOf(item); line; line = (MfPeTextLine *)MfDblList::Next(item)) {
        line->mGlyphs &= ~glyphs;
        context->InvalidateMargin(number);
        number++;
    }
}

// TODO 0x10019500 ?SetModuleName@MfPeEditorData@@QAEXABVMfString@@PAVMfPeDrawingContext@@@Z: 78% - the original expands ConvertToLower's CopyOnWrite differently (VC5's per-function inline budget)
void MfPeEditorData::SetModuleName(const MfString &module, MfPeDrawingContext *context)
{
    MfString name(module);
    name.ConvertToLower();
    GlyphModule *glyphs = (GlyphModule *)dictGlyphModules.Find(name);
    int number = 1;
    MfDblListItem *item = mLines->FirstItem();
    for (MfPeTextLine *line = LineOf(item); line; line = (MfPeTextLine *)MfDblList::Next(item)) {
        line->mGlyphs = 0;
        if (glyphs)
            line->mGlyphs |= glyphs->Get(number);
        context->InvalidateMargin(number);
        number++;
    }
}

// 0x10019640 ?AddGlobalGlyph@MfPeEditorData@@SAXABVMfString@@HH@Z
void MfPeEditorData::AddGlobalGlyph(const MfString &module, int line, int glyphs)
{
    MfString name(module);
    name.ConvertToLower();
    GlyphModule *glyphModule = (GlyphModule *)dictGlyphModules.Find(name);
    if (glyphModule == 0) {
        glyphModule = new GlyphModule(name);
        dictGlyphModules.Add(name, glyphModule);
    }
    glyphModule->Add(line, glyphs);
}

// 0x100197c0 ?RemoveGlobalGlyph@MfPeEditorData@@SAXABVMfString@@HH@Z
void MfPeEditorData::RemoveGlobalGlyph(const MfString &module, int line, int glyphs)
{
    MfString name(module);
    name.ConvertToLower();
    GlyphModule *glyphModule = (GlyphModule *)dictGlyphModules.Find(name);
    if (glyphModule)
        glyphModule->Remove(line, glyphs);
}

// 0x10019900 ?ClearGlobalGlyphs@MfPeEditorData@@SAXXZ
void MfPeEditorData::ClearGlobalGlyphs()
{
    LiiExemplar lii;
    GlyphModule deleter(lii);
    dictGlyphModules.ClearItems(&deleter);
}

// 0x10019960 ?CallBack@GlyphModuleIterator@MfPeEditorData@@UAEHPAX00@Z
int MfPeEditorData::GlyphModuleIterator::CallBack(void *, void *module, void *)
{
    ((GlyphModule *)module)->Clear(mGlyphs);
    return 1;
}

// 0x10019980 ?ClearGlobalGlyph@MfPeEditorData@@SAXH@Z
void MfPeEditorData::ClearGlobalGlyph(int glyphs)
{
    GlyphModuleIterator clear(glyphs);
    dictGlyphModules.Iterate(clear);
}

// 0x100199d0 ??_GGlyphModuleIterator@MfPeEditorData@@UAEPAXI@Z
// 0x100199f0 ??1GlyphModuleIterator@MfPeEditorData@@UAE@XZ

// EQUIV 0x10019a00 ?SetTabSize@MfPeEditorData@@QAEXHPAVMfPeDrawingContext@@@Z: reviewed: an independent store and load swap places
void MfPeEditorData::SetTabSize(int tabSize, MfPeDrawingContext *context)
{
    mTabSize = tabSize;
    mCursor.x = LineOf(mCursorLine)->mText.CuSetTabSize(tabSize, &mStringCursor) + 1;
    ShowCursor(context);
}

// 0x10019a40 ?SetAutoIndent@MfPeEditorData@@QAEXHPAVMfPeDrawingContext@@@Z
void MfPeEditorData::SetAutoIndent(int autoIndent, MfPeDrawingContext *)
{
    mAutoIndent = autoIndent;
}

// 0x10019a50 ?TestChar@IdentScanner@MfPeEditorData@@UAEHD@Z
int MfPeEditorData::IdentScanner::TestChar(char c)
{
    return isalpha(c) || isdigit(c) || c == '_';
}

// 0x10019a90 ?TestChar@PuncScanner@MfPeEditorData@@UAEHD@Z
int MfPeEditorData::PuncScanner::TestChar(char c)
{
    return ispunct(c) != 0;
}

// 0x10019ab0 ?TestChar@WhiteSpaceScanner@MfPeEditorData@@UAEHD@Z
int MfPeEditorData::WhiteSpaceScanner::TestChar(char c)
{
    return isspace(c) != 0;
}

// 0x10019ad0 ??0GlyphModule@MfPeEditorData@@QAE@ABVMfString@@@Z
MfPeEditorData::GlyphModule::GlyphModule(const MfString &module)
    : mModule(module)
{
}

// 0x10019b40 ??_GGlyphModule@MfPeEditorData@@UAEPAXI@Z

// 0x10019b60 ??0GlyphModule@MfPeEditorData@@QAE@VLiiExemplar@@@Z
MfPeEditorData::GlyphModule::GlyphModule(LiiExemplar)
{
}

// TODO 0x10019bd0 ??1GlyphModule@MfPeEditorData@@UAE@XZ: 82% - one EH state number differs: the original has one more unwind state
MfPeEditorData::GlyphModule::~GlyphModule()
{
    if (mGlyphs.GetCount() > 0) {
        GlyphInfo deleter;
        mGlyphs.Clear(deleter);
    }
}

// 0x10019c60 ??1GlyphInfo@GlyphModule@MfPeEditorData@@UAE@XZ
// 0x10019c70 ??_GGlyphInfo@GlyphModule@MfPeEditorData@@UAEPAXI@Z

// 0x10019c90 ?CallBack@GlyphModule@MfPeEditorData@@UAEHPAX@Z
int MfPeEditorData::GlyphModule::CallBack(void *module)
{
    delete (GlyphModule *)module;
    return 1;
}

// The same code as GlyphModule::CallBack.
int MfPeEditorData::GlyphModule::GlyphInfo::CallBack(void *info)
{
    delete (GlyphInfo *)info;
    return 1;
}

// 0x10019cb0 ?Find@GlyphModule@MfPeEditorData@@QAEPAVGlyphInfo@12@HPAPAVMfDblListItem@@@Z
MfPeEditorData::GlyphModule::GlyphInfo *MfPeEditorData::GlyphModule::Find(int line, MfDblListItem **position)
{
    MfDblListItem *item;
    for (GlyphInfo *info = (GlyphInfo *)mGlyphs.GetFirst(item); info; info = (GlyphInfo *)mGlyphs.GetNext(item)) {
        if (info->mLine == line) {
            if (position)
                *position = item;
            return info;
        }
    }
    return 0;
}

// 0x10019d10 ?Add@GlyphModule@MfPeEditorData@@QAEXHH@Z
void MfPeEditorData::GlyphModule::Add(int line, int glyphs)
{
    GlyphInfo *info = Find(line, 0);
    if (info == 0)
        mGlyphs.Add(new GlyphInfo(line, glyphs));
    else
        info->mGlyphs |= glyphs;
}

// 0x10019d80 ?Remove@GlyphModule@MfPeEditorData@@QAEXHH@Z
void MfPeEditorData::GlyphModule::Remove(int line, int glyphs)
{
    MfDblListItem *item;
    GlyphInfo *info = Find(line, &item);
    if (info && (info->mGlyphs &= ~glyphs) == 0) {
        mGlyphs.Delete(item);
        delete info;
    }
}

// 0x10019dd0 ?Get@GlyphModule@MfPeEditorData@@QAEHH@Z
int MfPeEditorData::GlyphModule::Get(int line)
{
    GlyphInfo *info = Find(line, 0);
    return info ? info->mGlyphs : 0;
}

// 0x10019df0 ?Clear@GlyphModule@MfPeEditorData@@QAEXH@Z
void MfPeEditorData::GlyphModule::Clear(int glyphs)
{
    MfDblListItem *item;
    GlyphInfo *info = (GlyphInfo *)mGlyphs.GetFirst(item);
    while (info) {
        info->mGlyphs &= ~glyphs;
        if (info->mGlyphs == 0) {
            mGlyphs.Delete(item);
            info = (GlyphInfo *)mGlyphs.GetCurrent(item);
        } else {
            info = (GlyphInfo *)mGlyphs.GetNext(item);
        }
    }
}

// 0x10019e40 ??0?$MfArray@VMfPeMarker@@@@QAE@XZ
// 0x10019ec0 ??_G?$MfArray@VMfPeMarker@@@@UAEPAXI@Z
// 0x10019f10 ??_EMfPeMarker@@QAEPAXI@Z
