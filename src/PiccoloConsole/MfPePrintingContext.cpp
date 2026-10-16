// PiccoloConsole.dll 0x10012ba0-0x100132ff: MfPePrintingContext, printing
// the text of the code editor.
#include "PiccoloConsole.h"

// 0x10012ba0 ??0MfPePrintingContext@@QAE@PAVMfPeCodeEditor@@@Z
MfPePrintingContext::MfPePrintingContext(MfPeCodeEditor *editor)
    : mEditor(editor)
{
}

// 0x10012c10 ??_GMfPePrintingContext@@UAEPAXI@Z
// 0x10012c30 ??1MfPePrintingContext@@UAE@XZ

// EQUIV 0x10012d00 ?Begin@MfPePrintingContext@@QAEHPAVCDC@@ABVMfString@@@Z: reviewed: register choice (the DC and font pointers use eax/ecx/edx swapped), the line/column/page stores scheduled before the first idiv instead of after it, and the margin doubling computed in the other register; same calls, arguments, stores and branches
int MfPePrintingContext::Begin(CDC *dc, const MfString &title)
{
    TEXTMETRIC tm;
    DOCINFO info;

    mTitle = title;
    mDC = dc;
    mTitleFont.CreatePointFont(80, "Courier New", mDC);
    mFont.CreatePointFont(100, "Courier New", mDC);
    mItalicFont.CreatePointFont(100, "Courier New Italic", mDC);
    mBoldFont.CreatePointFont(100, "Courier New Bold", mDC);
    int pixelsX = GetDeviceCaps(mDC->m_hAttribDC, LOGPIXELSX);
    int pixelsY = GetDeviceCaps(mDC->m_hAttribDC, LOGPIXELSY);
    mMarginX = pixelsX / 2;
    mMarginY = pixelsY / 2;
    CFont *oldFont = mDC->SelectObject(&mFont);
    mDC->GetTextMetrics(&tm);
    mCharWidth = tm.tmAveCharWidth;
    mCharHeight = tm.tmHeight;
    int width = GetDeviceCaps(mDC->m_hAttribDC, HORZRES);
    int height = GetDeviceCaps(mDC->m_hAttribDC, VERTRES);
    mLine = 1;
    mColumn = 1;
    mPage = 1;
    mColumnsPerLine = (width - 2 * mMarginX) / mCharWidth;
    mLinesPerPage = (height - 2 * mMarginY) / mCharHeight;

    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.lpszDocName = mTitle;
    if (mDC->StartDoc(&info) > 0) {
        mPrinting = 1;
        StartNewPage();
    } else {
        mPrinting = 0;
        mFont.DeleteObject();
        mItalicFont.DeleteObject();
        mBoldFont.DeleteObject();
        mTitleFont.DeleteObject();
    }
    mDC->SelectObject(oldFont);
    return 1;
}

// 0x10012eb0 ?End@MfPePrintingContext@@QAEXXZ
void MfPePrintingContext::End()
{
    if (mPrinting)
        mDC->EndDoc();
    mPrinting = 0;
    mFont.DeleteObject();
    mItalicFont.DeleteObject();
    mBoldFont.DeleteObject();
    mTitleFont.DeleteObject();
}

// TODO 0x10012ef0 ?Print@MfPePrintingContext@@QAEXABVMfString@@H@Z: 59% - the original keeps the constant 1 in ebx (EH state, style == 1); the loop is entered with a jump
void MfPePrintingContext::Print(const MfString &text, int style)
{
    if (mPrinting) {
        MfString line(text);
        if (style == 0)
            mDC->SelectObject(&mFont);
        else if (style == 1)
            mDC->SelectObject(&mItalicFont);
        else if (style == 2)
            mDC->SelectObject(&mBoldFont);
        else
            mDC->SelectObject(&mFont);
        while (mColumn + line.Length() - 1 > mColumnsPerLine) {
            MfString rest = line.RightString(mColumn + line.Length() - mColumnsPerLine - 1);
            line.Truncate(mColumnsPerLine - mColumn + 1);
            mDC->TextOut((mColumn - 1) * mCharWidth + mMarginX, (mLine - 1) * mCharHeight + mMarginY,
                         line, line.Length());
            mColumn += line.Length();
            line = rest;
            NewLine();
        }
        mDC->TextOut((mColumn - 1) * mCharWidth + mMarginX, (mLine - 1) * mCharHeight + mMarginY,
                     line, line.Length());
        mColumn += line.Length();
    }
}

// 0x10013120 ?NewLine@MfPePrintingContext@@QAEXXZ
void MfPePrintingContext::NewLine()
{
    mColumn = 1;
    mLine++;
    if (mLine > mLinesPerPage) {
        mLine = 1;
        mPage++;
        if (mDC->EndPage() <= 0) {
            mPrinting = 0;
            mDC->AbortDoc();
        } else {
            StartNewPage();
        }
    }
}

// TODO 0x10013180 ?StartNewPage@MfPePrintingContext@@QAEXXZ: 44% - frame is 4 bytes larger; the title y position is computed before the text pointer
void MfPePrintingContext::StartNewPage()
{
    if (mPrinting) {
        mDC->StartPage();
        CFont *oldFont = mDC->SelectObject(&mTitleFont);
        CSize size = mDC->GetTextExtent(mTitle, mTitle.Length());
        int width = mDC->GetDeviceCaps(HORZRES);
        mDC->TextOut((width - size.cx) / 2, (mMarginY - size.cy) / 2, mTitle, mTitle.Length());
        MfString page(mPage);
        CSize pageSize = mDC->GetTextExtent(page, page.Length());
        int height = mDC->GetDeviceCaps(VERTRES);
        mDC->TextOut((width - pageSize.cx) / 2, (mMarginY - pageSize.cy) / 2 + height - mMarginY, page,
                     page.Length());
        mDC->SelectObject(oldFont);
    }
}
