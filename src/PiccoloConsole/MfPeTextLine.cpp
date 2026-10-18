// PiccoloConsole.dll 0x1001aef0-0x1001bbbf: MfPeTextLine, one line of an
// editor: drawing and printing it in the colours of its tokens, and the
// syntax colouring that the scanner (a flex scanner, one at a time) finds.
#include "PiccoloConsole.h"

// The scanner of the syntax colouring (pelex.cpp).
extern "C" void PeLexRestart(void *file);
extern "C" void PeLexSetInput(MfPeLexInput *input);
extern "C" int PeLex();

// The colours of the token kinds, and the print styles.
static COLORREF colors[10] = { 0, 0xff, 0, 0x840000, 0xff0000, 0, 0x8200, 0xff00ff, 0xffffff, 0 };
static int styles[10] = { 0, 2, 0, 0, 0, 0, 1, 0, 0, 0 };

// 0x1001aef0 _$E247
// 0x1001af00 _$E244
// 0x1001af20 _$E246
// 0x1001af30 _$E245
static MfLock lexLock;

// 0x1001af40 ?Init@MfPeLexInput@@QAEXABVMfString@@PAVMfPeLineSyntax@@@Z
void MfPeLexInput::Init(const MfString &text, MfPeLineSyntax *syntax)
{
    mSyntax = syntax;
    mText = text;
    mColumn = 0;
    mRead = 0;
}

// 0x1001af70 ?Skip@MfPeLexInput@@QAEXPADH@Z
void MfPeLexInput::Skip(char *, int length)
{
    mColumn += length;
}

// 0x1001af80 ?NextColumn@MfPeLexInput@@QAEXXZ
void MfPeLexInput::NextColumn()
{
    mColumn++;
}

// 0x1001af90 ?Read@MfPeLexInput@@QAEHPADH@Z
int MfPeLexInput::Read(char *buffer, int size)
{
    int left = mText.Length() - mRead;
    if (left <= 0)
        return 0;
    if (size >= left)
        size = left;
    memcpy(buffer, (const char *)mText + mRead, size);
    mRead += size;
    return size;
}

// 0x1001aff0 ?Token@MfPeLexInput@@QAEXDH@Z
void MfPeLexInput::Token(char kind, int length)
{
    mSyntax->Add(MfPeMarker(mColumn - length, length, kind));
}

// 0x1001b050 ?Scan@MfPeTextLine@@QAEXH@Z
void MfPeTextLine::Scan(int tabSize)
{
    MfString text(mText);
    text.ExpandTabs(tabSize);
    ScanText(text, tabSize);
}

// TODO 0x1001b0d0 ?ScanText@MfPeTextLine@@QAEXAAVMfString@@H@Z: 78% - same logic (checked the constants: VC5 here turns the tab/blank choice into sub/neg/sbb/and 0x17/add 9 where the original branches to push 9 or 0x20, and clears COMMENT_START|CHANGED with a byte and al, 0xed where the original uses a dword and); the comment handling is laid out differently and the unwind states differ
void MfPeTextLine::ScanText(MfString &text, int tabSize)
{
    lexLock.Lock(-1);
    MfString original;
    mSyntax.mCount = 0;
    if (text == NULL_STRING) {
        mSyntax.mFlags &= ~(MfPeLineSyntax::COMMENT_START | MfPeLineSyntax::COMMENT_END);
        lexLock.Unlock();
        return;
    }
    if ((mSyntax.mFlags & (MfPeLineSyntax::IN_COMMENT | MfPeLineSyntax::COMMENT_END))
        && (~mSyntax.mFlags & MfPeLineSyntax::CHANGED)) {
        int end = text.ContainsSubString(MfString("*/"), 0);
        if (end == -1) {
            mSyntax.Add(MfPeMarker(0, text.CountCharacters(text.Length(), tabSize), 6));
            mSyntax.mFlags = (mSyntax.mFlags & ~MfPeLineSyntax::COMMENT_END) | MfPeLineSyntax::SCANNED
                             | MfPeLineSyntax::IN_COMMENT;
            lexLock.Unlock();
            return;
        }
        mSyntax.mFlags = (mSyntax.mFlags & ~MfPeLineSyntax::IN_COMMENT) | MfPeLineSyntax::COMMENT_END;
        end += 2;
        mSyntax.Add(MfPeMarker(0, text.CountCharacters(end, tabSize), 6));
        MfString blank = MfStringSize(end);
        for (int i = 0; i < end; i++)
            blank << (text.GetCharacter(i) == '\t' ? '\t' : ' ');
        original = text;
        text = NULL_STRING;
        text << blank << original.RightString(original.Length() - end);
    } else {
        mSyntax.mFlags &= ~(MfPeLineSyntax::COMMENT_START | MfPeLineSyntax::CHANGED);
    }
    MfPeLexInput input;
    input.Init(text, &mSyntax);
    PeLexRestart(0);
    PeLexSetInput(&input);
    PeLex();
    if (original != NULL_STRING)
        mText = original;
    mSyntax.mFlags |= MfPeLineSyntax::SCANNED;
    lexLock.Unlock();
}

// 0x1001b3b0 ??1MfPeLexInput@@QAE@XZ

// EQUIV 0x1001b3c0 ?Draw@MfPeTextLine@@QAEHHPAVMfPeDrawingContext@@HH@Z: reviewed: register choice (the keyword-colour table load and the column use eax/ecx/edx swapped, so one mov takes the 6-byte form), the run length computed as (length + column) - start instead of (length - start) + column on ints, and the EH state store and the result load in the other order; same calls, arguments and stores
int MfPeTextLine::Draw(int line, MfPeDrawingContext *context, int tabSize, int highlight)
{
    MfString text(mText);
    text.ExpandTabs(tabSize);
    if (highlight)
        ScanText(text, tabSize);
    else
        mSyntax.mCount = 0;
    if (context->IsTextVisible(line, text.Length())) {
        if (mSyntax.mCount > 0) {
            MfString piece;
            int column = 0;
            for (int i = 0; i < mSyntax.mCount; i++) {
                MfPeMarker marker = mSyntax.Get(i);
                if (column < marker.mColumn) {
                    piece = text.MidString(column, marker.mColumn - column);
                    context->DrawString(piece, colors[0], column + 1, line);
                    column += piece.Length();
                }
                piece = text.MidString(column, marker.mLength + marker.mColumn - column);
                context->DrawString(piece, colors[marker.mKind], column + 1, line);
                column += piece.Length();
            }
            if (column < text.Length()) {
                piece = text.MidString(column, text.Length() - column);
                context->DrawString(piece, colors[0], column + 1, line);
            }
        } else {
            context->DrawString(text, colors[0], 1, line);
        }
    }
    return text.Length();
}

// TODO 0x1001b620 ?Get@MfPeLineSyntax@@QAE?AVMfPeMarker@@I@Z: 87% - the original calls MfPeMarker's vector deleting destructor, this build expands delete[] inline

// 0x1001b6f0 ?DrawMargin@MfPeTextLine@@QAEXHPAVMfPeDrawingContext@@@Z
void MfPeTextLine::DrawMargin(int line, MfPeDrawingContext *context)
{
    if (context->IsMarginVisible(line)) {
        context->EraseMargin(line);
        context->DrawGlyphs(line, mGlyphs);
    }
}

// EQUIV 0x1001b730 ?Print@MfPeTextLine@@QAEXPAVMfPePrintingContext@@HH@Z: reviewed: register choice only (the colour-table loads and the string addresses use eax/ecx/edx swapped, so three movs take the 6-byte form) and the order of an independent load and add; same calls, pushed values and EH states
void MfPeTextLine::Print(MfPePrintingContext *context, int tabSize, int highlight)
{
    MfString text(mText);
    text.ExpandTabs(tabSize);
    if (highlight)
        ScanText(text, tabSize);
    else
        mSyntax.mCount = 0;
    if (mSyntax.mCount > 0) {
        MfString piece;
        int column = 0;
        for (int i = 0; i < mSyntax.mCount; i++) {
            MfPeMarker marker = mSyntax.Get(i);
            if (column < marker.mColumn) {
                piece = text.MidString(column, marker.mColumn - column);
                context->Print(piece, styles[0]);
                column += piece.Length();
            }
            piece = text.MidString(column, marker.mLength + marker.mColumn - column);
            context->Print(piece, styles[marker.mKind]);
            column += piece.Length();
        }
        if (column < text.Length()) {
            piece = text.MidString(column, text.Length() - column);
            context->Print(piece, styles[0]);
        }
    } else {
        context->Print(text, styles[0]);
    }
    context->NewLine();
}

// EQUIV 0x1001b960 ?FillTo@MfPeTextLine@@QAEXPAVMfStringCursor@@@Z: reviewed: the two tab-column divisions are computed in the other order (pure arithmetic); ebp saved later
void MfPeTextLine::FillTo(MfStringCursor *cursor)
{
    unsigned int position = cursor->CursorPos();
    unsigned int length = cursor->ExpandedSize();
    if (position >= length) {
        unsigned int tabSize = cursor->TabSize();
        int lengthTabs = length / tabSize;
        int positionTabs = position / tabSize;
        int tabs = positionTabs - lengthTabs;
        int spaces;
        if (tabs > 0)
            spaces = position % tabSize;
        else
            spaces = position - length;
        int i;
        for (i = tabs; i > 0; i--)
            mText << '\t';
        for (i = spaces; i > 0; i--)
            mText << ' ';
        mText.CuBindToCursor(cursor->CursorPos(), cursor);
    }
}

// EQUIV 0x1001b9f0 ?InsertChar@MfPeTextLine@@QAEXDPAVMfStringCursor@@@Z: reviewed: same calls in the same order with the same EH states; the left and right parts swap their stack slots, and the right part is passed by the eax that RightString returns (its own address) instead of a lea of the slot; the unwind funclets are listed in another order
void MfPeTextLine::InsertChar(char c, MfStringCursor *cursor)
{
    FillTo(cursor);
    if (cursor->CursorPos() >= cursor->ExpandedSize())
        mText << c;
    else {
        MfString left = mText.LeftString(cursor->CharPos());
        mText = left << c << mText.RightString(mText.Length() - cursor->CharPos());
    }
    mText.CuBindToCursor(cursor->CursorPos(), cursor);
}

// 0x1001bad0 ?DeleteChar@MfPeTextLine@@QAEDHPAVMfStringCursor@@@Z
char MfPeTextLine::DeleteChar(int forward, MfStringCursor *cursor)
{
    char c = 0;
    unsigned int position = cursor->CharPos();
    if (position <= (unsigned int)mText.Length()) {
        if (!forward)
            position--;
        MfString left = mText.LeftString(position);
        c = mText.GetCharacter(position);
        mText = left + mText.RightString(mText.Length() - position - 1);
        mText.CuBindToCursor(cursor->CursorPos(), cursor);
    }
    return c;
}
