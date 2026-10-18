// PiccoloConsole.dll 0x1000ce70-0x10010c6f: MfPeCodeEditor, the code editor
// window; the text, the drawing and the commands are in their own classes.
#include "PiccoloConsole.h"

// 0x1000ce70 _$E248
// 0x1000ce80 _$E244
// 0x1000cea0 _$E247
// 0x1000ceb0 _$E245
MfDblList MfPeCodeEditor::listGlobalKeyboardHandlers;

// 0x1000ced0 ?CreateObject@MfPeCodeEditor@@SGPAVCObject@@XZ
// 0x1000cf30 ?_GetBaseClass@MfPeCodeEditor@@KGPAUCRuntimeClass@@XZ
// 0x1000cf40 ?GetRuntimeClass@MfPeCodeEditor@@UBEPAUCRuntimeClass@@XZ
IMPLEMENT_DYNCREATE(MfPeCodeEditor, CWnd)

// 0x1000cf50 ??0MfPeCodeEditor@@QAE@XZ
MfPeCodeEditor::MfPeCodeEditor()
    : mReadOnly(0), mOverwrite(0), mPiccoloWnd(0), mMouseDown(0), mMarginSelect(0), mWordSelect(0),
      mDblClickSelectsLine(0), mDblClickHandler(0), mDragPending(0), mWheelDelta(0), mSized(0),
      mUndoItem(0), mRedoItem(0), mSavedItem(0), mFastTimer(0), mSlowTimer(0), mSetFocusTimer(0)
{
    mDrawing = new MfPeDrawingContext(this);
    mPrinting = new MfPePrintingContext(this);
    mData = new MfPeEditorData(mDrawing);
    mCommands = new MfPeCommandList;
    mBuffModCallBacks = new MfDblList;
    mKeyboardHandlers = new MfDblList;
    mSetFocusHandlers = new MfDblList;
}

// 0x1000d130 ??_EMfPeCodeEditor@@UAEPAXI@Z

// 0x1000d190 ??1MfPeCodeEditor@@UAE@XZ
MfPeCodeEditor::~MfPeCodeEditor()
{
    delete mData;
    delete mDrawing;
    delete mPrinting;
    delete mCommands;
    delete mBuffModCallBacks;
    delete mKeyboardHandlers;
    delete mSetFocusHandlers;
}

// 0x1000d260 ?GetTextData@MfPeCodeEditor@@QAE?AVMfString@@XZ
MfString MfPeCodeEditor::GetTextData()
{
    return mData->GetText();
}

// 0x1000d280 ?SetTextData@MfPeCodeEditor@@QAEXABVMfString@@H@Z
void MfPeCodeEditor::SetTextData(const MfString &text, int keepCursor)
{
    mData->SetText(text, mDrawing, keepCursor);
    mData->SetModuleName(mModuleName, mDrawing);
    mCommands->Clear();
    mSavedItem = 0;
    mUndoItem = 0;
    mRedoItem = 0;
    Invalidate(FALSE);
    NotifyBuffMod();
}

// 0x1000d2e0 ?SetModuleName@MfPeCodeEditor@@QAEXABVMfString@@@Z
void MfPeCodeEditor::SetModuleName(const MfString &module)
{
    mModuleName = module;
    mData->mModuleName = mModuleName;
    mData->SetModuleName(mModuleName, mDrawing);
}

// 0x1000d320 ?GetModuleName@MfPeCodeEditor@@QBE?AVMfString@@XZ
MfString MfPeCodeEditor::GetModuleName() const
{
    return mModuleName;
}

// 0x1000d350 ?AppendText@MfPeCodeEditor@@QAEXABVMfString@@@Z
void MfPeCodeEditor::AppendText(const MfString &text)
{
    mData->AppendText(text, mDrawing);
    NotifyBuffMod();
}

// 0x1000d370 ?Clear@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::Clear()
{
    mData->Clear(mDrawing);
    Invalidate(FALSE);
    UpdateWindow();
    NotifyBuffMod();
}

// 0x1000d3a0 ?SetUnixFormat@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::SetUnixFormat()
{
    mData->mUnixFormat = 1;
}

// 0x1000d3b0 ?SetDosFormat@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::SetDosFormat()
{
    mData->mUnixFormat = 0;
}

// 0x1000d3c0 ?EnableSyntaxHighlight@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::EnableSyntaxHighlight()
{
    mData->mSyntaxHighlight = 1;
    Invalidate(FALSE);
    UpdateWindow();
}

// 0x1000d3f0 ?DisableSyntaxHighlight@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::DisableSyntaxHighlight()
{
    mData->mSyntaxHighlight = 0;
    Invalidate(FALSE);
    UpdateWindow();
}

// 0x1000d420 ?GotoLine@MfPeCodeEditor@@QAEXHH@Z
void MfPeCodeEditor::GotoLine(int line, int column)
{
    PostMessage(WM_PE_GOTO_LINE, line, column);
}

// 0x1000d440 ?SetGlyphOnLine@MfPeCodeEditor@@QAEXHH@Z
void MfPeCodeEditor::SetGlyphOnLine(int line, int glyph)
{
    mData->SetGlyph(line, 1 << glyph, mDrawing);
}

// 0x1000d470 ?UnsetGlyphOnLine@MfPeCodeEditor@@QAEXHH@Z
void MfPeCodeEditor::UnsetGlyphOnLine(int line, int glyph)
{
    mData->UnsetGlyph(line, 1 << glyph, mDrawing);
}

// 0x1000d4a0 ?ClearGlyphsOnLine@MfPeCodeEditor@@QAEXH@Z
void MfPeCodeEditor::ClearGlyphsOnLine(int line)
{
    mData->ClearGlyphsOnLine(line, mDrawing);
}

// 0x1000d4c0 ?ClearGlyphs@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::ClearGlyphs()
{
    mData->ClearGlyphs(mDrawing);
}

// 0x1000d4d0 ?ClearGlyphs@MfPeCodeEditor@@QAEXH@Z
void MfPeCodeEditor::ClearGlyphs(int glyph)
{
    mData->ClearGlyph(1 << glyph, mDrawing);
}

// 0x1000d4f0 ?AddGlyphAtLineForModule@MfPeCodeEditor@@SAXABVMfString@@HH@Z
void MfPeCodeEditor::AddGlyphAtLineForModule(const MfString &module, int line, int glyph)
{
    MfPeEditorData::AddGlobalGlyph(module, line, 1 << glyph);
}

// 0x1000d510 ?RemoveGlyphAtLineForModule@MfPeCodeEditor@@SAXABVMfString@@HH@Z
void MfPeCodeEditor::RemoveGlyphAtLineForModule(const MfString &module, int line, int glyph)
{
    MfPeEditorData::RemoveGlobalGlyph(module, line, 1 << glyph);
}

// 0x1000d530 ?ClearGlobalGlyphs@MfPeCodeEditor@@SAXXZ
void MfPeCodeEditor::ClearGlobalGlyphs()
{
    MfPeEditorData::ClearGlobalGlyphs();
}

// 0x1000d540 ?ClearGlobalGlyphs@MfPeCodeEditor@@SAXH@Z
void MfPeCodeEditor::ClearGlobalGlyphs(int glyph)
{
    MfPeEditorData::ClearGlobalGlyph(1 << glyph);
}

// 0x1000d560 ?FindFirst@MfPeCodeEditor@@QAEHABVMfString@@PAVMfPeSearchContext@@H@Z
int MfPeCodeEditor::FindFirst(const MfString &text, MfPeSearchContext *context, int backwards)
{
    if (!backwards) {
        mData->ClearSelection(mDrawing);
        mData->GotoTextStart(mDrawing);
    } else {
        mData->ClearSelection(mDrawing);
        mData->GotoTextEnd(mDrawing);
    }
    int found = mData->Find(text, mDrawing, backwards, 0);
    if (found == 1) {
        MfPeRegion region;
        region = mData->mSelection;
        if (backwards) {
            context->x = region.mStart.x;
            context->y = region.mStart.y;
        } else {
            context->x = region.mEnd.x;
            context->y = region.mEnd.y;
        }
    } else {
        mData->GetCursor(*context);
    }
    return found;
}

// 0x1000d660 ?FindNext@MfPeCodeEditor@@QAEHABVMfString@@PAVMfPeSearchContext@@H@Z
int MfPeCodeEditor::FindNext(const MfString &text, MfPeSearchContext *context, int backwards)
{
    int found = mData->Find(text, mDrawing, backwards, 0);
    if (found == 1) {
        MfPeRegion region;
        region = mData->mSelection;
        if (backwards) {
            context->x = region.mStart.x;
            context->y = region.mStart.y;
        } else {
            context->x = region.mEnd.x;
            context->y = region.mEnd.y;
        }
    } else {
        mData->GetCursor(*context);
    }
    return found;
}

// 0x1000d720 ?_GetBaseMessageMap@MfPeCodeEditor@@KGPBUAFX_MSGMAP@@XZ
// 0x1000d730 ?GetMessageMap@MfPeCodeEditor@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPeCodeEditor, CWnd)
    ON_WM_CREATE()
    ON_WM_NCDESTROY()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_LBUTTONDOWN()
    ON_WM_KEYDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_TIMER()
    ON_WM_CLOSE()
    ON_WM_CHAR()
    ON_WM_GETDLGCODE()
    ON_WM_MOUSEWHEEL()
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(ID_PE_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_PE_COPY, OnUpdateEditCopy)
    ON_COMMAND(ID_PE_TOGGLE_OVERWRITE, OnToggleOverwrite)
    ON_UPDATE_COMMAND_UI(ID_PE_TOGGLE_OVERWRITE, OnUpdateToggleOverwrite)
    ON_COMMAND(ID_PE_CUT, OnEditCut)
    ON_UPDATE_COMMAND_UI(ID_PE_CUT, OnUpdateEditCut)
    ON_COMMAND(ID_PE_PASTE, OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_PE_PASTE, OnUpdateEditPaste)
    ON_COMMAND(ID_PE_PRINT, OnEditPrint)
    ON_MESSAGE(WM_PE_UPDATE_CARET, OnUpdateCaretPos)
    ON_MESSAGE(WM_PE_GOTO_LINE, OnGotoLine)
END_MESSAGE_MAP()

// 0x1000d740 ?PreCreateWindow@MfPeCodeEditor@@MAEHAAUtagCREATESTRUCTA@@@Z
BOOL MfPeCodeEditor::PreCreateWindow(CREATESTRUCT &cs)
{
    cs.style |= WS_VSCROLL | WS_HSCROLL;
    cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, NULL, (HBRUSH)COLOR_WINDOW);
    return CWnd::PreCreateWindow(cs);
}

// 0x1000d780 ?OnCreate@MfPeCodeEditor@@IAEHPAUtagCREATESTRUCTA@@@Z
int MfPeCodeEditor::OnCreate(LPCREATESTRUCT cs)
{
    if (CWnd::OnCreate(cs) == -1)
        return -1;
    mDrawing->SetupFont(GetDC());
    mDrawing->SetTextCursor();
    mDrawing->ResetScrollRanges(1);
    return 0;
}

// 0x1000d7d0 ?OnPaint@MfPeCodeEditor@@IAEXXZ
void MfPeCodeEditor::OnPaint()
{
    CPaintDC dc(this);

    mDrawing->SetDC(&dc, &dc.m_ps.rcPaint);
    mData->Draw(mDrawing);
}

// 0x1000d840 ?OnSize@MfPeCodeEditor@@IAEXIHH@Z
void MfPeCodeEditor::OnSize(UINT type, int cx, int cy)
{
    CWnd::OnSize(type, cx, cy);
    mDrawing->SetClientSize(cx, cy);
    if (cx > 0 && !mSized) {
        mSized = 1;
        mData->ShowCursor(mDrawing);
        mData->UpdateScrollRanges(mDrawing);
    }
}

// 0x1000d890 ?OnVScroll@MfPeCodeEditor@@IAEXIIPAVCScrollBar@@@Z
void MfPeCodeEditor::OnVScroll(UINT code, UINT position, CScrollBar *scrollBar)
{
    CWnd *focus = GetFocus();
    if (code == SB_ENDSCROLL && focus && focus->m_hWnd != m_hWnd)
        SetFocus();
    mData->OnVScroll(code, position, mDrawing);
}

// 0x1000d8e0 ?OnHScroll@MfPeCodeEditor@@IAEXIIPAVCScrollBar@@@Z
void MfPeCodeEditor::OnHScroll(UINT code, UINT position, CScrollBar *scrollBar)
{
    CWnd *focus = GetFocus();
    if (code == SB_ENDSCROLL && focus && focus->m_hWnd != m_hWnd)
        SetFocus();
    mData->OnHScroll(code, position, mDrawing);
}

// 0x1000d930 ?OnClose@MfPeCodeEditor@@IAEXXZ
void MfPeCodeEditor::OnClose()
{
    KillSlowTimer();
    KillFastTimer();
    CWnd::OnClose();
}

// 0x1000d980 ?OnSetFocus@MfPeCodeEditor@@IAEXPAVCWnd@@@Z
void MfPeCodeEditor::OnSetFocus(CWnd *oldWnd)
{
    mWheelDelta = 0;
    mData->ShowCursor(mDrawing);
    mDrawing->CreateEditCaret();
    StartSetFocusTimer();
}

// 0x1000d9d0 ?OnKillFocus@MfPeCodeEditor@@IAEXPAVCWnd@@@Z
void MfPeCodeEditor::OnKillFocus(CWnd *newWnd)
{
    MfDblListItem *pli;

    mDrawing->DestroyEditCaret();
    for (MfPeSetFocusHandler *handler = (MfPeSetFocusHandler *)mSetFocusHandlers->GetFirst(pli); handler;
         handler = (MfPeSetFocusHandler *)mSetFocusHandlers->GetNext(pli))
        handler->LostFocus(this);
}

// 0x1000da20 ?OnLButtonDown@MfPeCodeEditor@@IAEXIVCPoint@@@Z
void MfPeCodeEditor::OnLButtonDown(UINT flags, CPoint point)
{
    mMouseDown = 1;
    CWnd *focus = GetFocus();
    if (focus && focus->m_hWnd != m_hWnd)
        SetFocus();
    SetCapture();
    if (point.x >= mDrawing->mMarginWidth) {
        if (mDrawing->IsOverSelection(point)) {
            mDragPending = 1;
        } else if (flags & MK_SHIFT) {
            mData->StartSelection(mDrawing);
            mData->MoveCursorTo(point, mDrawing);
            mData->ExtendSelection(mDrawing);
        } else {
            mData->ClearSelection(mDrawing);
            mData->MoveCursorTo(point, mDrawing);
            mData->StartSelection(mDrawing);
        }
    } else {
        mMarginSelect = 1;
        mDrawing->ScrollTo(WM_HSCROLL, 1);
        if (flags & MK_SHIFT) {
            mData->SelectLine(point, mDrawing);
            mData->ExtendLineSelection(point, mDrawing);
        } else {
            mData->ClearSelection(mDrawing);
            mData->MoveCursorTo(point, mDrawing);
            mData->SelectLine(point, mDrawing);
        }
    }
}

// 0x1000db60 ?OnLButtonDblClk@MfPeCodeEditor@@IAEXIVCPoint@@@Z
void MfPeCodeEditor::OnLButtonDblClk(UINT flags, CPoint point)
{
    SetCapture();
    if (point.x >= mDrawing->mMarginWidth) {
        mMouseDown = 0;
        if (mDblClickSelectsLine) {
            mData->ClearSelection(mDrawing);
            mData->MoveCursorTo(point, mDrawing);
            mData->SelectLine(point, mDrawing);
        } else {
            mWordSelect = 1;
            mData->MoveCursorTo(point, mDrawing);
            mData->SelectWord(point, mDrawing);
        }
        if (mDblClickHandler)
            mDblClickHandler->HandleDblClick(mData->GetSelectionLine());
    }
}

// 0x1000dc00 ?OnLButtonUp@MfPeCodeEditor@@IAEXIVCPoint@@@Z
void MfPeCodeEditor::OnLButtonUp(UINT flags, CPoint point)
{
    KillFastTimer();
    KillSlowTimer();
    if (mDragPending) {
        mDragPending = 0;
        mData->EndDrag(point, mDrawing);
        if (mDrawing->IsInSelection(point)) {
            mData->ClearSelection(mDrawing);
            mData->MoveCursorTo(point, mDrawing);
        } else {
            mData->ClearSelection(mDrawing);
            mData->MoveCursorTo(point, mDrawing);
            AddCommand(new MfPeDragRegionCommand(mData));
        }
    }
    mMouseDown = 0;
    mWordSelect = 0;
    mMarginSelect = 0;
    mData->EndSelection();
    ReleaseCapture();
}

// 0x1000dd40 ??1MfPeCommand@@UAE@XZ
// 0x1000dd50 ?GetPosition@MfPeCommand@@UAE?AVCPoint@@XZ
// 0x1000dd70 ??_GMfPeCommand@@UAEPAXI@Z
// 0x1000dd90 ??_GMfPeEditorState@@UAEPAXI@Z
// 0x1000ddb0 ??_GMfPeDragRegionCommand@@UAEPAXI@Z
// 0x1000ddd0 ??1MfPeDragRegionCommand@@UAE@XZ

// 0x1000de30 ?OnRButtonDown@MfPeCodeEditor@@IAEXIVCPoint@@@Z
void MfPeCodeEditor::OnRButtonDown(UINT flags, CPoint point)
{
    CMenu menu;

    menu.CreatePopupMenu();
    int selected = mData->HasSelection();
    menu.AppendMenu(selected ? MF_STRING : MF_GRAYED | MF_DISABLED, ID_PE_CUT, "Cu&t");
    menu.AppendMenu(MF_STRING, ID_PE_COPY, "&Copy");
    menu.AppendMenu(MF_STRING, ID_PE_PASTE, "P&aste");
    menu.AppendMenu(MF_SEPARATOR, 0, (LPCTSTR)NULL);
    menu.AppendMenu(MF_STRING, ID_PE_PRINT, "&Print");
    ClientToScreen(&point);
    menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

// 0x1000df60 ?StandardAutoScrollTest@MfPeCodeEditor@@AAEXVCPoint@@AAH1@Z
void MfPeCodeEditor::StandardAutoScrollTest(CPoint point, int &outside, int &border)
{
    CRect client;

    GetClientRect(&client);
    client.left = mDrawing->mMarginWidth;
    CRect inner(client);
    inner.InflateRect(-mDrawing->mCharWidth / 2, -mDrawing->LineHeight() / 2);
    outside = !client.PtInRect(point);
    border = !inner.PtInRect(point) && client.PtInRect(point);
}

// TODO 0x1000e030 ?OnMouseMove@MfPeCodeEditor@@IAEXIVCPoint@@@Z: 96% - the slots of the outside/border flags and the margin scroll test are laid out differently
void MfPeCodeEditor::OnMouseMove(UINT flags, CPoint point)
{
    if (mDragPending) {
        mData->DragTo(point, mReadOnly, mDrawing);
        int outside = 0;
        int border = 0;
        StandardAutoScrollTest(point, outside, border);
        if (border)
            StartSlowTimer();
        else
            KillSlowTimer();
    } else if (!mMouseDown && !mWordSelect) {
        mDrawing->UpdateCursor(point);
    } else {
        int border;
        int outside;
        outside = 0;
        border = 0;
        if (mMarginSelect && point.x >= mDrawing->mMarginWidth) {
            mMarginSelect = 0;
            mData->ResetSelection(point, mDrawing);
            mDrawing->SetCursorType(0);
        }
        if (mWordSelect) {
            mData->MoveCursorTo(point, mDrawing);
            mData->ExtendWordSelection(mDrawing);
            StandardAutoScrollTest(point, outside, border);
        } else if (mMarginSelect) {
            mData->ExtendLineSelection(point, mDrawing);
            CRect client;
            GetClientRect(&client);
            outside = point.y < client.top || point.y > client.bottom;
            if (point.y >= client.top && point.y < client.top + mDrawing->LineHeight() / 2)
                border = 1;
            else if (point.y > client.bottom - mDrawing->LineHeight() / 2 && point.y <= client.bottom)
                border = 1;
            else
                border = 0;
        } else {
            mData->StartSelection(mDrawing);
            mData->MoveCursorTo(point, mDrawing);
            mData->ExtendSelection(mDrawing);
            StandardAutoScrollTest(point, outside, border);
        }
        if (!mFastTimer && outside) {
            KillSlowTimer();
            StartFastTimer();
        } else if (!mSlowTimer && border) {
            KillFastTimer();
            StartSlowTimer();
        } else if (!outside && !border) {
            KillSlowTimer();
            KillFastTimer();
        }
    }
    mMousePoint = point;
}

// 0x1000e340 ?OnMouseWheel@MfPeCodeEditor@@IAEHIFVCPoint@@@Z
BOOL MfPeCodeEditor::OnMouseWheel(UINT flags, short delta, CPoint point)
{
    mWheelDelta += delta;
    int lines = -3 * mWheelDelta / WHEEL_DELTA;
    if (lines) {
        mWheelDelta += lines * (WHEEL_DELTA / 3);
        PostMessage(WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, GetScrollPos(SB_VERT) + lines));
    }
    return TRUE;
}

// 0x1000e3c0 ?IsTextModified@MfPeCodeEditor@@QAEHXZ
int MfPeCodeEditor::IsTextModified()
{
    return mUndoItem != mSavedItem;
}

// 0x1000e3e0 ?SetModificationFlag@MfPeCodeEditor@@QAEXH@Z
void MfPeCodeEditor::SetModificationFlag(int modified)
{
    if (!modified)
        mSavedItem = mUndoItem;
}

// TODO 0x1000e400 ?AddCommand@MfPeCodeEditor@@AAEXPAVMfPeCommand@@@Z: 80% - register allocation of the merge of typed characters differs
void MfPeCodeEditor::AddCommand(MfPeCommand *command)
{
    if (mReadOnly) {
        delete command;
        return;
    }
    while (mRedoItem) {
        MfDblListItem *next = mRedoItem ? mRedoItem->pliNext : 0;
        mCommands->Delete(mRedoItem);
        mRedoItem = next;
    }
    if (dynamic_cast<MfPeTypingCommand *>(command)) {
        MfPeCommand *last = CommandOf(mCommands->LastItem());
        MfPeTypingCommand *lastTyping = dynamic_cast<MfPeTypingCommand *>(last);
        MfPeInsertStringCommand *lastInsert = dynamic_cast<MfPeInsertStringCommand *>(last);
        int lastIsWord = 0;
        int follows = 0;
        if (lastInsert) {
            char c = lastInsert->mText.GetCharacter(lastInsert->mText.Length() - 1);
            if (isalnum(c) || c == '_')
                lastIsWord = 1;
            CPoint end = lastInsert->GetPosition();
            end.x += lastInsert->mText.Length();
            CPoint position = command->GetPosition();
            if (position.y == end.y && position.x == end.x)
                follows = 1;
        } else if (lastTyping) {
            lastIsWord = isalnum(lastTyping->mChar) || lastTyping->mChar == '_';
            CPoint end = lastTyping->GetPosition();
            CPoint position = command->GetPosition();
            if (position.y == end.y && position.x == end.x + 1)
                follows = 1;
        }
        if (lastInsert || lastTyping) {
            MfPeTypingCommand *typing = dynamic_cast<MfPeTypingCommand *>(command);
            int isWord = isalnum(typing->mChar) || typing->mChar == '_';
            if (follows && ((lastIsWord && isWord) || (!lastIsWord && !isWord))) {
                if (!lastInsert) {
                    lastInsert = new MfPeInsertStringCommand(mData, lastTyping);
                    lastInsert->mText << lastTyping->mChar;
                    mCommands->DeleteLast();
                    mCommands->Add(lastInsert);
                }
                lastInsert->mText << typing->mChar;
                typing->Do(mDrawing);
                delete typing;
                command = 0;
            }
        }
    }
    if (command) {
        mCommands->Add(command);
        command->Do(mDrawing);
    }
    mUndoItem = mCommands->LastItem();
    NotifyBuffMod();
}

// 0x1000e790 ??1MfPeInsertStringState@@UAE@XZ
// 0x1000e7a0 ?GetPosition@MfPeInsertStringCommand@@UAE?AVCPoint@@XZ
// 0x1000e7c0 ??_GMfPeInsertStringCommand@@UAEPAXI@Z
// 0x1000e7e0 ??1MfPeInsertStringCommand@@UAE@XZ

// 0x1000e840 ?DeleteSelectedRegion@MfPeCodeEditor@@AAEHXZ
int MfPeCodeEditor::DeleteSelectedRegion()
{
    int deleted = 0;
    if (mData->HasSelection()) {
        AddCommand(new MfPeDeleteRegionCommand(mData));
        deleted = 1;
    }
    return deleted;
}

// 0x1000e930 ??1MfPeDeleteRegionState@@UAE@XZ
// 0x1000e9c0 ??_GMfPeDeleteRegionState@@UAEPAXI@Z
// 0x1000e9e0 ??_GMfPeDeleteRegionCommand@@UAEPAXI@Z
// 0x1000ea00 ??1MfPeDeleteRegionCommand@@UAE@XZ

// 0x1000eac0 ?PreTranslateMessage@MfPeCodeEditor@@MAEHPAUtagMSG@@@Z
BOOL MfPeCodeEditor::PreTranslateMessage(MSG *msg)
{
    if (msg->message == WM_KEYDOWN && msg->wParam != VK_CONTROL && msg->wParam != VK_MENU
        && (GetKeyState(VK_CONTROL) & 0xfff0) && !(GetKeyState(VK_MENU) & 0xfff0)) {
        DispatchMessage(msg);
        return TRUE;
    }
    return CWnd::PreTranslateMessage(msg);
}

// TODO 0x1000eb20 ?OnKeyDown@MfPeCodeEditor@@IAEXIII@Z: 90% - same calls per key (checked against logicdiff: the original shares one copy of the IsSelectionPastLineEnd/CollapseSelection block between two keys by a jump, VC5 here keeps a copy in each, which is the extra call pair; the "@*" reference is a vtable store both sides have); the switch and the shared tails are laid out differently (3299 bytes instead of 3275)
void MfPeCodeEditor::OnKeyDown(UINT key, UINT repeat, UINT flags)
{
    int control = GetKeyState(VK_CONTROL) < 0;
    int shift = GetKeyState(VK_SHIFT) < 0;
    int alt = GetKeyState(VK_MENU) < 0;

    mData->EndSelection();
    switch (key) {
    case VK_LEFT:
        if (shift)
            mData->StartSelection(mDrawing);
        else
            mData->CollapseSelection(1, mDrawing);
        if (control)
            mData->WordLeft(mDrawing);
        else
            mData->CursorLeft(mDrawing);
        if (shift)
            mData->ExtendSelection(mDrawing);
        break;
    case VK_RIGHT:
        if (shift)
            mData->StartSelection(mDrawing);
        else
            mData->CollapseSelection(0, mDrawing);
        if (control)
            mData->WordRight(mDrawing);
        else
            mData->CursorRight(mDrawing);
        if (shift)
            mData->ExtendSelection(mDrawing);
        break;
    case VK_UP:
        if (control && shift)
            break;
        if (shift)
            mData->StartSelection(mDrawing);
        else
            mData->CollapseSelection(1, mDrawing);
        if (control)
            mData->ScrollUp(mDrawing);
        else
            mData->CursorUp(mDrawing);
        if (shift)
            mData->ExtendSelection(mDrawing);
        break;
    case VK_DOWN:
        if (control && shift)
            break;
        if (shift)
            mData->StartSelection(mDrawing);
        else
            mData->CollapseSelection(0, mDrawing);
        if (control)
            mData->ScrollDown(mDrawing);
        else
            mData->CursorDown(mDrawing);
        if (shift)
            mData->ExtendSelection(mDrawing);
        break;
    case VK_HOME:
        if (shift)
            mData->StartSelection(mDrawing);
        else
            mData->CollapseSelection(1, mDrawing);
        if (control)
            mData->GotoTextStart(mDrawing);
        else
            mData->CursorHome(mDrawing);
        if (shift)
            mData->ExtendSelection(mDrawing);
        break;
    case VK_END:
        if (shift)
            mData->StartSelection(mDrawing);
        else
            mData->CollapseSelection(0, mDrawing);
        if (control)
            mData->GotoTextEnd(mDrawing);
        else
            mData->CursorEnd(mDrawing);
        if (shift)
            mData->ExtendSelection(mDrawing);
        break;
    case VK_PRIOR:
        if (shift)
            mData->StartSelection(mDrawing);
        else
            mData->CollapseSelection(1, mDrawing);
        mData->PageUp(mDrawing);
        if (shift)
            mData->ExtendSelection(mDrawing);
        break;
    case VK_NEXT:
        if (shift)
            mData->StartSelection(mDrawing);
        else
            mData->CollapseSelection(0, mDrawing);
        mData->PageDown(mDrawing);
        if (shift)
            mData->ExtendSelection(mDrawing);
        break;
    case VK_RETURN:
        if (!mOverwrite) {
            DeleteSelectedRegion();
            AddCommand(new MfPeNewLineCommand(mData));
        } else if (mData->IsAtTextEnd()) {
            AddCommand(new MfPeNewLineCommand(mData));
        } else {
            mData->CursorHome(mDrawing);
            mData->CursorDown(mDrawing);
        }
        break;
    case 'C':
        if (control)
            OnEditCopy();
        break;
    case 'X':
        if (control)
            OnEditCut();
        break;
    case 'V':
        if (control)
            OnEditPaste();
        break;
    case 'Z':
        if (control && mUndoItem)
            OnEditUndo();
        break;
    case 'A':
        if (control && mRedoItem)
            OnEditRedo();
        break;
    case 'F':
        if (control && mPiccoloWnd)
            mPiccoloWnd->SetFocusToFindSelect();
        break;
    case VK_F3:
        if (mPiccoloWnd)
            mPiccoloWnd->SetSearchString();
        if (shift)
            OnFindPrevious();
        else
            OnFindNext();
        break;
    case 'G':
        if (control) {
            PiccoloConsoleContext context;
            MfPeGotoDlg dialog(mData->mCursor.y, mData->mLines->GetCount(), this);
            AfxGetMainWnd()->PostMessage(0x118);    // WM_SYSTIMER
            if (dialog.DoModal() == IDOK) {
                mData->GotoLine(dialog.mLine, 0, mDrawing);
                SetFocus();
            }
        }
        break;
    case 'P':
        if (control) {
            PiccoloConsoleContext context;
            MfPePwPreferencesDlg dialog(mData->mTabSize, nPwTabSize, mData->mSyntaxHighlight, bPwSyntaxHighlight,
                                        mData->mAutoIndent, bPwAutoIndent, 0);
            AfxGetMainWnd()->PostMessage(0x118);    // WM_SYSTIMER
            if (dialog.DoModal() == IDOK) {
                mData->SetTabSize(dialog.mTabSize, mDrawing);
                nPwTabSize = dialog.mDefaultTabSize;
                if (dialog.mSyntaxHighlight)
                    mData->mSyntaxHighlight = 1;
                else
                    mData->mSyntaxHighlight = 0;
                bPwSyntaxHighlight = dialog.mDefaultSyntaxHighlight;
                mData->SetAutoIndent(dialog.mAutoIndent, mDrawing);
                bPwAutoIndent = dialog.mDefaultAutoIndent;
                Invalidate(FALSE);
                UpdateWindow();
                SetFocus();
            }
        }
        break;
    case VK_TAB:
        if (mData->HasSelection() && mData->SpansLines()) {
            mData->SelectWholeLines(mDrawing, 1);
            if (shift) {
                if (!mData->CanUndent())
                    break;
                AddCommand(new MfPeUndentRegionCommand(mData));
            } else {
                AddCommand(new MfPeIndentRegionCommand(mData));
            }
        } else if (!mOverwrite) {
            DeleteSelectedRegion();
            AddCommand(new MfPeTypingCommand(mData, (char)key));
        } else if (shift) {
            mData->TabStopLeft(mDrawing);
        } else {
            mData->TabStopRight(mDrawing);
        }
        break;
    case VK_BACK:
        if (mData->HasSelection()) {
            if (mData->IsSelectionPastLineEnd())
                mData->CollapseSelection(1, mDrawing);
            else
                DeleteSelectedRegion();
        } else if (control) {
            AddCommand(new MfPeDeleteBackWordCommand(mData));
        } else if (mData->mStringCursor.CursorPos() > mData->mStringCursor.ExpandedSize()) {
            mData->TabLeft(mDrawing);
        } else if (mData->mStringCursor.CursorPos() == 0 && mData->mCursor.y == 1) {
            break;
        } else if (mData->mStringCursor.CursorPos() == 0) {
            AddCommand(new MfPeJoinLineCommand(mData, 0));
        } else {
            AddCommand(new MfPeDeleteCommand(mData, 1));
        }
        break;
    case VK_DELETE:
        if (mData->HasSelection()) {
            if (mData->IsSelectionPastLineEnd())
                mData->CollapseSelection(1, mDrawing);
            else
                DeleteSelectedRegion();
        } else if (control) {
            AddCommand(new MfPeDeleteForwardWordCommand(mData));
        } else if (mData->mStringCursor.CursorPos() >= mData->mStringCursor.ExpandedSize()) {
            AddCommand(new MfPeJoinLineCommand(mData, 1));
        } else {
            AddCommand(new MfPeDeleteCommand(mData, 0));
        }
        break;
    case VK_INSERT:
        mOverwrite = !mOverwrite;
        break;
    default:
        SendToKeyboardHandlers(key, alt, control, shift);
        break;
    }
}

// 0x1000f7f0 ??_GMfPeTypingCommand@@UAEPAXI@Z
// 0x1000f810 ??1MfPeTypingCommand@@UAE@XZ
// 0x1000f840 ??_GMfPeDeleteCommand@@UAEPAXI@Z
// 0x1000f860 ??1MfPeDeleteCommand@@UAE@XZ
// 0x1000f890 ??_GMfPeJoinLineCommand@@UAEPAXI@Z
// 0x1000f8b0 ??1MfPeJoinLineCommand@@UAE@XZ
// 0x1000f8e0 ??_GMfPeNewLineCommand@@UAEPAXI@Z
// 0x1000f900 ??1MfPeNewLineCommand@@UAE@XZ
// 0x1000f930 ??_GMfPeIndentRegionCommand@@UAEPAXI@Z
// 0x1000f950 ??1MfPeIndentRegionCommand@@UAE@XZ
// 0x1000f9b0 ??_GMfPeUndentRegionState@@UAEPAXI@Z
// 0x1000f9d0 ??1MfPeUndentRegionState@@UAE@XZ
// 0x1000fa50 ??_GMfPeUndentRegionCommand@@UAEPAXI@Z
// 0x1000fa70 ??1MfPeUndentRegionCommand@@UAE@XZ
// 0x1000fb20 ??_GMfPeDeleteBackWordState@@UAEPAXI@Z
// 0x1000fb40 ??1MfPeDeleteBackWordState@@UAE@XZ
// 0x1000fb80 ??_GMfPeDeleteBackWordCommand@@UAEPAXI@Z
// 0x1000fba0 ??1MfPeDeleteBackWordCommand@@UAE@XZ
// 0x1000fc00 ??_GMfPeDeleteForwardWordState@@UAEPAXI@Z
// 0x1000fc20 ??1MfPeDeleteForwardWordState@@UAE@XZ
// 0x1000fc60 ??_GMfPeDeleteForwardWordCommand@@UAEPAXI@Z
// 0x1000fc80 ??1MfPeDeleteForwardWordCommand@@UAE@XZ
// 0x1000fce0 ??1MfPeGotoDlg@@UAE@XZ
// 0x1000fd50 ??1MfPePwPreferencesDlg@@UAE@XZ

// EQUIV 0x1000fdb0 ?OnChar@MfPeCodeEditor@@IAEXIII@Z: reviewed: esi is saved only on the path that uses it
void MfPeCodeEditor::OnChar(UINT c, UINT repeat, UINT flags)
{
    MfPeCommand *command;

    mData->EndSelection();
    if (c >= ' ' && c <= '~') {
        int deleted = DeleteSelectedRegion();
        if (!mOverwrite || deleted)
            command = new MfPeTypingCommand(mData, (char)c);
        else
            command = new MfPeOverwriteCommand(mData, (char)c);
        AddCommand(command);
    }
}

// 0x1000feb0 ??_GMfPeOverwriteCommand@@UAEPAXI@Z
// 0x1000fed0 ??1MfPeOverwriteCommand@@UAE@XZ

// 0x1000ff00 ?OnGetDlgCode@MfPeCodeEditor@@IAEIXZ
UINT MfPeCodeEditor::OnGetDlgCode()
{
    return DLGC_WANTALLKEYS;
}

// 0x1000ff10 ?OnUpdateCaretPos@MfPeCodeEditor@@IAEJIJ@Z
LRESULT MfPeCodeEditor::OnUpdateCaretPos(WPARAM wParam, LPARAM lParam)
{
    mData->ShowCursor(mDrawing);
    return 0;
}

// 0x1000ff30 ?OnGotoLine@MfPeCodeEditor@@IAEJIJ@Z
LRESULT MfPeCodeEditor::OnGotoLine(WPARAM wParam, LPARAM lParam)
{
    mData->GotoLine(wParam, lParam, mDrawing);
    SetFocus();
    return 0;
}

// TODO 0x1000ff60 ?OnTimer@MfPeCodeEditor@@IAEXI@Z: 80% - the original shares the stop-timers block of the horizontal scroll tests at the end
void MfPeCodeEditor::OnTimer(UINT id)
{
    if (id == 52) {
        MfDblListItem *pli;
        for (MfPeSetFocusHandler *handler = (MfPeSetFocusHandler *)mSetFocusHandlers->GetFirst(pli); handler;
             handler = (MfPeSetFocusHandler *)mSetFocusHandlers->GetNext(pli))
            handler->ReceivedFocus(this);
        KillSetFocusTimer();
        return;
    }

    CRect client;
    GetClientRect(&client);
    client.left = mDrawing->mMarginWidth;
    CRect inner(client);
    inner.InflateRect(-mDrawing->mCharWidth / 2, -mDrawing->LineHeight() / 2);
    if (!mMarginSelect && mMousePoint.x <= inner.left) {
        if (!mDrawing->CanScroll(WM_HSCROLL, 0)) {
            KillFastTimer();
            KillSlowTimer();
            return;
        }
        mDrawing->mEditor->SendMessage(WM_HSCROLL, SB_LINELEFT);
    } else if (!mMarginSelect && mMousePoint.x >= inner.right) {
        if (!mDrawing->CanScroll(WM_HSCROLL, 1)) {
            KillFastTimer();
            KillSlowTimer();
            return;
        }
        mDrawing->mEditor->SendMessage(WM_HSCROLL, SB_LINERIGHT);
    }
    if (mMousePoint.y <= inner.top) {
        if (!mDrawing->CanScroll(WM_VSCROLL, 0)) {
            KillFastTimer();
            KillSlowTimer();
            return;
        }
        mDrawing->mEditor->SendMessage(WM_VSCROLL, SB_LINEUP);
    } else if (mMousePoint.y >= inner.bottom) {
        if (!mDrawing->CanScroll(WM_VSCROLL, 1)) {
            KillFastTimer();
            KillSlowTimer();
            return;
        }
        mDrawing->mEditor->SendMessage(WM_VSCROLL, SB_LINEDOWN);
    }
    if (mDragPending) {
        mData->DragTo(mMousePoint, mReadOnly, mDrawing);
    } else if (mMarginSelect) {
        mData->ExtendLineSelection(mMousePoint, mDrawing);
    } else if (mWordSelect) {
        mData->MoveCursorTo(mMousePoint, mDrawing);
        mData->ExtendWordSelection(mDrawing);
    } else {
        mData->StartSelection(mDrawing);
        mData->MoveCursorTo(mMousePoint, mDrawing);
        mData->ExtendSelection(mDrawing);
    }
}

// 0x10010260 ?OnEditCopy@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::OnEditCopy()
{
    MfPeCopyRegionCommand command(mData);
    command.Do(mDrawing);
}

// 0x100102e0 ??1MfPeCopyRegionCommand@@UAE@XZ
// 0x10010310 ??_GMfPeCopyRegionCommand@@UAEPAXI@Z

// 0x10010330 ?OnUpdateEditCopy@MfPeCodeEditor@@QAEXPAVCCmdUI@@@Z
void MfPeCodeEditor::OnUpdateEditCopy(CCmdUI *cmdUI)
{
    cmdUI->Enable(TRUE);
}

// 0x10010340 ?OnEditCut@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::OnEditCut()
{
    AddCommand(new MfPeCutRegionCommand(mData));
}

// 0x10010400 ??_GMfPeCutRegionCommand@@UAEPAXI@Z
// 0x10010420 ??1MfPeCutRegionCommand@@UAE@XZ

// 0x100104e0 ?OnUpdateEditCut@MfPeCodeEditor@@QAEXPAVCCmdUI@@@Z
void MfPeCodeEditor::OnUpdateEditCut(CCmdUI *cmdUI)
{
    cmdUI->Enable(mData->HasSelection() != 0);
}

// 0x10010530 ?OnEditPaste@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::OnEditPaste()
{
    DeleteSelectedRegion();
    AddCommand(new MfPePasteRegionCommand(mData));
}

// The same code as OnUpdateEditCopy, which is kept.
void MfPeCodeEditor::OnUpdateEditPaste(CCmdUI *cmdUI)
{
    cmdUI->Enable(TRUE);
}

// 0x100105c0 ??_GMfPePasteRegionState@@UAEPAXI@Z
// 0x100105e0 ??_GMfPePasteRegionCommand@@UAEPAXI@Z
// 0x10010600 ??1MfPePasteRegionCommand@@UAE@XZ

// 0x10010630 ?OnEditPrint@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::OnEditPrint()
{
    CDC dc;
    CPrintDialog dialog(FALSE, PD_NOSELECTION | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_USEDEVMODECOPIES);

    if (dialog.GetDefaults()) {
        dc.Attach(dialog.m_pd.hDC);
        mPrinting->Begin(&dc, mModuleName);
        mData->Print(mPrinting);
        mPrinting->End();
        GlobalFree(dialog.m_pd.hDevNames);
        GlobalFree(dialog.m_pd.hDevMode);
        DeleteObject(dialog.m_pd.hDC);
    }
}

// EQUIV 0x10010730 ?OnEditRedo@MfPeCodeEditor@@QAEXXZ: reviewed: register choice only
void MfPeCodeEditor::OnEditRedo()
{
    mData->ClearSelection(mDrawing);
    CommandOf(mRedoItem)->Redo(mDrawing);
    mUndoItem = mRedoItem;
    MfDblList::NextItem(mRedoItem);
    NotifyBuffMod();
}

// 0x10010780 ?OnUpdateEditRedo@MfPeCodeEditor@@QAEXPAVCCmdUI@@@Z
void MfPeCodeEditor::OnUpdateEditRedo(CCmdUI *cmdUI)
{
    cmdUI->Enable(mRedoItem != 0);
}

// EQUIV 0x100107a0 ?OnEditUndo@MfPeCodeEditor@@QAEXXZ: reviewed: register choice only
void MfPeCodeEditor::OnEditUndo()
{
    mData->ClearSelection(mDrawing);
    CommandOf(mUndoItem)->Undo(mDrawing);
    mRedoItem = mUndoItem;
    MfDblList::PrevItem(mUndoItem);
    NotifyBuffMod();
}

// 0x100107f0 ?OnUpdateEditUndo@MfPeCodeEditor@@QAEXPAVCCmdUI@@@Z
void MfPeCodeEditor::OnUpdateEditUndo(CCmdUI *cmdUI)
{
    cmdUI->Enable(mUndoItem != 0);
}

// 0x10010810 ?OnFindNext@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::OnFindNext()
{
    if (!mData->Find(mSearchString, mDrawing, 0, 1)) {
        MfString message;
        message.Format("Cannot find the string '%s'", (const char *)mSearchString);
        mDrawing->ShowMessage(message);
    }
}

// 0x100108c0 ?OnFindPrevious@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::OnFindPrevious()
{
    if (!mData->Find(mSearchString, mDrawing, 1, 1)) {
        MfString message;
        message.Format("Cannot find the string '%s'", (const char *)mSearchString);
        mDrawing->ShowMessage(message);
    }
}

// 0x10010970 ?OnToggleOverwrite@MfPeCodeEditor@@QAEXXZ
void MfPeCodeEditor::OnToggleOverwrite()
{
    mOverwrite = !mOverwrite;
}

// 0x10010980 ?OnUpdateToggleOverwrite@MfPeCodeEditor@@QAEXPAVCCmdUI@@@Z
void MfPeCodeEditor::OnUpdateToggleOverwrite(CCmdUI *cmdUI)
{
    cmdUI->SetCheck(mOverwrite != 0);
}

// 0x100109a0 ?GetIEditor@MfPeCodeEditor@@QAEPAVMfAiPiccoloEditor@@XZ
MfAiPiccoloEditor *MfPeCodeEditor::GetIEditor()
{
    return mData;
}

// 0x100109b0 ?InstallBufferModifiedCallBack@MfPeCodeEditor@@QAEXPAVMfPeBuffModCallBack@@@Z
void MfPeCodeEditor::InstallBufferModifiedCallBack(MfPeBuffModCallBack *callBack)
{
    if (!mBuffModCallBacks->Find(callBack))
        mBuffModCallBacks->Append(callBack);
}

// 0x100109e0 ?UninstallBufferModifiedCallBack@MfPeCodeEditor@@QAEXPAVMfPeBuffModCallBack@@@Z
void MfPeCodeEditor::UninstallBufferModifiedCallBack(MfPeBuffModCallBack *callBack)
{
    MfDblListItem *pli = mBuffModCallBacks->Find(callBack);
    mBuffModCallBacks->Delete(pli);
}

// 0x10010a10 ?NotifyBuffMod@MfPeCodeEditor@@AAEXXZ
void MfPeCodeEditor::NotifyBuffMod()
{
    MfDblListItem *pli;

    int empty = mData->IsTextEmpty();
    for (MfPeBuffModCallBack *callBack = (MfPeBuffModCallBack *)mBuffModCallBacks->GetFirst(pli); callBack;
         callBack = (MfPeBuffModCallBack *)mBuffModCallBacks->GetNext(pli))
        callBack->BmcbNotify(empty);
}

// 0x10010a60 ?InstallKeyboardHandler@MfPeCodeEditor@@QAEXPAVMfPeKbdHandler@@@Z
void MfPeCodeEditor::InstallKeyboardHandler(MfPeKbdHandler *handler)
{
    if (!mKeyboardHandlers->Find(handler))
        mKeyboardHandlers->Append(handler);
}

// 0x10010a90 ?UninstallKeyboardHandler@MfPeCodeEditor@@QAEXPAVMfPeKbdHandler@@@Z
void MfPeCodeEditor::UninstallKeyboardHandler(MfPeKbdHandler *handler)
{
    MfDblListItem *pli = mKeyboardHandlers->Find(handler);
    mKeyboardHandlers->Delete(pli);
}

// 0x10010ac0 ?InstallGlobalKeyboardHandler@MfPeCodeEditor@@SAXPAVMfPeKbdHandler@@@Z
void MfPeCodeEditor::InstallGlobalKeyboardHandler(MfPeKbdHandler *handler)
{
    if (!listGlobalKeyboardHandlers.Find(handler))
        listGlobalKeyboardHandlers.AddLast(handler);
}

// 0x10010af0 ?UninstallGlobalKeyboardHandler@MfPeCodeEditor@@SAXPAVMfPeKbdHandler@@@Z
void MfPeCodeEditor::UninstallGlobalKeyboardHandler(MfPeKbdHandler *handler)
{
    MfDblListItem *pli = listGlobalKeyboardHandlers.Find(handler);
    listGlobalKeyboardHandlers.Delete(pli);
}

// TODO 0x10010b20 ?SendToKeyboardHandlers@MfPeCodeEditor@@AAEXHHHH@Z: 84% - register choice flips with unrelated changes of the unit (hash order of the optimizer)
void MfPeCodeEditor::SendToKeyboardHandlers(int key, int shift, int control, int alt)
{
    int flags = (shift != 0) | (control ? 2 : 0) | (alt ? 4 : 0);
    MfDblListItem *pli;
    MfPeKbdHandler *handler;

    for (handler = (MfPeKbdHandler *)mKeyboardHandlers->GetFirst(pli); handler;
         handler = (MfPeKbdHandler *)mKeyboardHandlers->GetNext(pli)) {
        if (handler->KbhKeyDown(key, flags, GetIEditor()))
            return;
    }
    for (handler = (MfPeKbdHandler *)listGlobalKeyboardHandlers.First(pli); handler;
         handler = (MfPeKbdHandler *)MfDblList::Next(pli)) {
        if (handler->KbhKeyDown(key, flags, GetIEditor()))
            return;
    }
}

// 0x10010bf0 ?RegisterCursorChangedCallback@MfPeCodeEditor@@QAEXPAVMfPeCursorChanged@@@Z
void MfPeCodeEditor::RegisterCursorChangedCallback(MfPeCursorChanged *callBack)
{
    mData->RegisterCursorChangedCallback(callBack);
}

// 0x10010c00 ?UnregisterCursorChangedCallback@MfPeCodeEditor@@QAEXPAVMfPeCursorChanged@@@Z
void MfPeCodeEditor::UnregisterCursorChangedCallback(MfPeCursorChanged *callBack)
{
    mData->UnregisterCursorChangedCallback(callBack);
}

// 0x10010c10 ?InstallSetFocusHandler@MfPeCodeEditor@@QAEXPAVMfPeSetFocusHandler@@@Z
void MfPeCodeEditor::InstallSetFocusHandler(MfPeSetFocusHandler *handler)
{
    if (!mSetFocusHandlers->Find(handler))
        mSetFocusHandlers->Append(handler);
}

// 0x10010c40 ?UninstallSetFocusHandler@MfPeCodeEditor@@QAEXPAVMfPeSetFocusHandler@@@Z
void MfPeCodeEditor::UninstallSetFocusHandler(MfPeSetFocusHandler *handler)
{
    MfDblListItem *pli = mSetFocusHandlers->Find(handler);
    mSetFocusHandlers->Delete(pli);
}

// The same code as CConsoleOutput::OnNcDestroy, which is kept.
void MfPeCodeEditor::OnNcDestroy()
{
    CWnd::OnNcDestroy();
    delete this;
}

// 0x10010c70 ??_EMfPeDeleteBackWordCommand@@WBA@AEPAXI@Z
// 0x10010c80 ??_EMfPePasteRegionCommand@@WBA@AEPAXI@Z
// 0x10010c90 ??_EMfPeOverwriteCommand@@WBA@AEPAXI@Z
// 0x10010ca0 ??_EMfPeDeleteForwardWordCommand@@WBA@AEPAXI@Z
// 0x10010cb0 ??_EMfPeDragRegionCommand@@WBA@AEPAXI@Z
// 0x10010cc0 ??_EMfPeJoinLineCommand@@WBA@AEPAXI@Z
// 0x10010cd0 ??_EMfPeDeleteCommand@@WBA@AEPAXI@Z
// 0x10010ce0 ??_EMfPeIndentRegionCommand@@WBA@AEPAXI@Z
// 0x10010cf0 ??_EMfPeNewLineCommand@@WBA@AEPAXI@Z
// 0x10010d00 ??_EMfPeCopyRegionCommand@@WBA@AEPAXI@Z
// 0x10010d10 ??_EMfPeUndentRegionCommand@@WBA@AEPAXI@Z
// 0x10010d20 ??_EMfPeInsertStringCommand@@WBA@AEPAXI@Z
// 0x10010d30 ??_EMfPeDeleteRegionCommand@@WBA@AEPAXI@Z
// 0x10010d40 ??_EMfPeCutRegionCommand@@WBA@AEPAXI@Z
// 0x10010d50 ??_EMfPeTypingCommand@@WBA@AEPAXI@Z
