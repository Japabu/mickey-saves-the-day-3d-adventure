// PscDebugger.dll 0x10002e40-0x10005968: the debugger. A script thread that
// stops (breakpoint, exception, message box) hands the request to the
// debugger's worker thread (DoWork) and waits; the worker posts a command to
// the debugger frame, whose thread shows the module and waits for the user.
#include "PscDebugger.h"

// 0x10016f80
CWinThread *pdbUIThread;
// 0x10016f84
MfPdbDebugger *pdbDebugger;

// 0x10002e40 ??0MfPdbDebugger@@QAE@XZ
MfPdbDebugger::MfPdbDebugger()
    : mRefs(1), mFrame(0), mThread(0), mActive(0), mRequest(0), mResume(0), mState(kPdbIdle),
      mStepping(0), mDialog(0)
{
    mRequest = new MfEvent;
    mDone = new MfEvent;
    mResume = new MfEvent;
    mRegistry = new MfPdbFunctionRegistry;
    mBreakpoints = new MfPdbBreakpointManager;
    mListeners = new MfDblList;
    pdbDebugger = this;
}

// 0x10003040 ?AssertValid@CObject@@UBEXXZ
// 0x10003050 ?Serialize@CObject@@UAEXAAVCArchive@@@Z
// 0x10003060 ??1MfThreadWork@@UAE@XZ
// 0x10003070 ??_GMfThreadWork@@UAEPAXI@Z
// 0x10003090 ??_GMfPdbDebugger@@UAEPAXI@Z
// 0x100030b0 ??1MfPdbMessageBox@@QAE@XZ
// 0x10003100 ??_GMfEvent@@UAEPAXI@Z

// 0x10003120 ??1MfPdbDebugger@@UAE@XZ
MfPdbDebugger::~MfPdbDebugger()
{
    pdbDebugger = 0;
    if (mRequest)
        delete mRequest;
    mRequest = 0;
    if (mDone)
        delete mDone;
    mDone = 0;
    if (mResume)
        delete mResume;
    mResume = 0;
    if (mListeners)
        delete mListeners;
    mListeners = 0;
}

// 0x10003220 ?Initialize@MfPdbDebugger@@UAEXXZ
void MfPdbDebugger::Initialize()
{
    if (pdbUIThread)
        return;

    MfPdbToolbarBitmapLoader *loader = new MfPdbToolbarBitmapLoader(0x6f);
    MfPeMetaCustomToolbar *toolbar = new MfPeMetaCustomToolbar("Breakpoint", kToolbarBreakpoint, loader);
    *toolbar + new MfPeMetaToolbarEntryItem(kButtonToggle, "Toggle Breakpoint (Ctrl+T)", this)
        + new MfPeMetaToolbarEntryItem(kButtonEnable, "Enable/Disable Breakpoint (Ctrl+Shift+T)", this)
        + new MfPeMetaToolbarEntrySeparator
        + new MfPeMetaToolbarEntryItem(kButtonDisableAll, "Disable All Breakpoints", this)
        + new MfPeMetaToolbarEntryItem(kButtonDisableModule, "Disable Module Breakpoints", this)
        + new MfPeMetaToolbarEntrySeparator
        + new MfPeMetaToolbarEntryItem(kButtonClearAll, "Clear All Breakpoints", this)
        + new MfPeMetaToolbarEntryItem(kButtonClearModule, "Clear Module Breakpoints", this)
        + new MfPeMetaToolbarEntrySeparator
        + new MfPeMetaToolbarEntryItem(kButtonEdit, "Edit Breakpoints (Ctrl+K)", this);
    CPiccoloWnd::AddMetaToolbar(toolbar);
    loader->Release();

    MfPeCodeEditor::InstallGlobalKeyboardHandler(this);
    pdbUIThread = AfxBeginThread(RUNTIME_CLASS(MfPdbDebuggerUI), THREAD_PRIORITY_ABOVE_NORMAL);
    pdbUIThread->m_bAutoDelete = TRUE;
    pdbUIReady->Wait(INFINITE);
    mFrame = (MfPdbDebuggerFrame *)pdbUIThread->m_pMainWnd;
    CConsoleEdit *editor = mFrame->EditorWnd();
    editor->InstallKeyboardHandler(this);
    mThread = new MfThread(MfThread::MF_THREAD_AUTODELETE);
    mThread->AssignWork(this);
    mBreakpoints->LoadBreakpoints();
    MfPdbVarDispWnd::Initialize();
}

// 0x10003880 ??_GMfPeMetaToolbarEntrySeparator@@UAEPAXI@Z
// 0x100038a0 ??_GMfPeMetaCustomToolbar@@UAEPAXI@Z
// 0x100038c0 ??_GMfPeMetaToolbarEntryItem@@UAEPAXI@Z
// 0x100038e0 ??_GMfThread@@UAEPAXI@Z

// 0x10003900 ?Shutdown@MfPdbDebugger@@UAEXXZ
void MfPdbDebugger::Shutdown()
{
    if (!pdbUIThread)
        return;
    mBreakpoints->SaveBreakpoints();
    CPiccoloWnd::RemoveMetaToolbar("Breakpoint", 0);
    CPiccoloWnd::ClearDisplayedGlyphs(kGlyphBreakpoint);
    MfPeCodeEditor::ClearGlobalGlyphs(kGlyphBreakpoint);
    CPiccoloWnd::ClearDisplayedGlyphs(kGlyphDisabledBreakpoint);
    MfPeCodeEditor::ClearGlobalGlyphs(kGlyphDisabledBreakpoint);
    MfPeCodeEditor::UninstallGlobalKeyboardHandler(this);
    CConsoleEdit *editor = mFrame->EditorWnd();
    editor->UninstallKeyboardHandler(this);
    if (mRegistry) {
        mRegistry->Clear();
        mRegistry->Release();
        mRegistry = 0;
    }
    if (mBreakpoints) {
        mBreakpoints->Release();
        mBreakpoints = 0;
    }
    pdbUIThread->PostThreadMessage(WM_QUIT, 0, 0);
    pdbUIThread = 0;
    mThread->SignalShutdown(0);
    if (mRequest->IsWaiting())
        mRequest->Signal();
    mResume->Wait(INFINITE);
    Sleep(0);
    Sleep(0);
    Sleep(0);
}

// 0x10003a60 ?IsActive@MfPdbDebugger@@UAEHXZ
int MfPdbDebugger::IsActive()
{
    return mActive;
}

// 0x10003a70 ?ProgramState@MfPdbDebugger@@UAEXH@Z
void MfPdbDebugger::ProgramState(int state)
{
    if (state == 0) {
        Notify(kPdbStarted);
        mFrame->PostMessage(WM_USER + 1);
        mBreakpoints->PlantAll();
        mBreakpoints->SaveBreakpoints();
        MfScPiccolo::bIsRunning = 1;
        mActive = 1;
    } else if (state == 1) {
        Notify(kPdbStopped);
        mActive = 0;
        MfScPiccolo::bIsRunning = 0;
        mBreakpoints->UnplantAll();
        if (mFrame->IsWindowVisible()) {
            mFrame->MessageBox("Piccolo program has exited", "Piccolo Debugger", 0);
            if (mDialog)
                mDialog->EndDialog(0);
            mFrame->SendMessage(WM_CLOSE);
        }
    }
}

// 0x10003b40 ?GetInterface@MfPdbDebugger@@UAEHHPAPAVMfAi@@@Z
int MfPdbDebugger::GetInterface(int id, MfAi **ai)
{
    int result = 3;
    MfAi *p;
    switch (id) {
    case 0:
        p = MfScPiccolo::GetSourceResolver();
        *ai = p;
        p->AddRef();
        return 0;
    case 1:
        p = mBreakpoints;
        break;
    case 2:
        p = mRegistry;
        break;
    default:
        goto done;
    }
    if (p) {
        *ai = p;
        p->AddRef();
        result = 0;
    }
done:
    return result;
}

// 0x10003b90 ?AddListener@MfPdbDebugger@@UAEXPAVMfPdbListener@@@Z
void MfPdbDebugger::AddListener(MfPdbListener *listener)
{
    mListeners->Add(listener);
}

// 0x10003ba0 ?RemoveListener@MfPdbDebugger@@UAEXPAVMfPdbListener@@@Z
void MfPdbDebugger::RemoveListener(MfPdbListener *listener)
{
    MfDblListItem *pos = mListeners->Find(listener);
    mListeners->Delete(pos);
}

// 0x10003bd0 ?Notify@MfPdbDebugger@@QAEXH@Z
void MfPdbDebugger::Notify(int event)
{
    MfDblListItem *pos;
    for (MfPdbListener *listener = (MfPdbListener *)mListeners->GetFirst(pos); listener;
         listener = (MfPdbListener *)mListeners->GetNext(pos))
        listener->Notify(event);
}

// 0x10003c10 ?GetInterfaceName@MfPdbDebugger@@UAEXPADHAAH1@Z
void MfPdbDebugger::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "PiccoloDebugger", size);
    major = 1;
    minor = 0;
}

// 0x10003c40 ?AddRef@MfPdbDebugger@@UAEHXZ
int MfPdbDebugger::AddRef()
{
    return ++mRefs;
}

// 0x10003c50 ?Release@MfPdbDebugger@@UAEHXZ
int MfPdbDebugger::Release()
{
    if (--mRefs == 0) {
        delete this;
        return 0;
    }
    return mRefs;
}

// 0x10003c70 ?SignalBreakpoint@MfPdbDebugger@@UAEHPAUMfPdbStepContext@@PAE@Z
int MfPdbDebugger::SignalBreakpoint(MfPdbStepContext *ctx, unsigned char *bytecode)
{
    if (mState)
        return 1;
    mStepping = 0;
    Notify(kPdbBreak);
    if (!ctx->routine) {
        MfString msg;
        msg << "There is an internal Piccolo VM error!\r\n" << "At Line " << 550 << " in File "
            << "MfPdbDebugger.cpp" << "\r\n" << "in method MfPdbDebugger::SignalBreakpoint.\r\n\r\n"
            << "The module to be debugged is NULL.\r\n\r\n" << "Press OK to continue program execution.";
        AskUser(msg, "Internal VM Error", 0x1030);
        Notify(kPdbTerminate);
        return 2;
    }
    mFrame->mDebugEdit->SetEntireContents(NULL_STRING, 0);
    if (ctx->routine->DebugInfo()) {
        mState = kPdbAtBreakpoint;
        mContext = ctx;
        CConsoleEdit *editor = mFrame->EditorWnd();
        editor->mEditable = 0;
        mRequest->Signal();
        mResume->Wait(INFINITE);
        editor->mEditable = 1;
    } else {
        mBytecode = mBreakpoints->FindBreakpoint((unsigned long)ctx->env->m_ip - 1)->mBytecode;
    }
    if (mStepping)
        Notify(kPdbTerminate);
    else
        Notify(kPdbContinue);
    *bytecode = mBytecode;
    return 0;
}

// EQUIV 0x10003e60 ?SignalException@MfPdbDebugger@@UAEHHABVMfString@@PAUMfPdbStepContext@@@Z: reviewed: register choice only (the original keeps 0 in edi through the line loop and compares against it, and calls operator<<(const char *) through the import instead of a cached register); same calls, arguments and branches
int MfPdbDebugger::SignalException(int code, const MfString &message, MfPdbStepContext *ctx)
{
    if (mState)
        return 1;
    mStepping = 0;
    Notify(kPdbException);
    if (!ctx->env->m_function) {
        Notify(kPdbExceptionDone);
        return 0;
    }
    MfStringList lines;
    ctx->vm->FormatException(code, message, &lines);
    MfString text = MfStringSize(0x200);
    MfDblListItem *pos;
    for (MfString *line = lines.First(pos); line; line = lines.Next(pos))
        text << *line << "\r\n";
    mFrame->mPiccoloWnd->mOutput->SwitchToWindow(mFrame->mDebugWindow);
    mFrame->mDebugEdit->SetEntireContents(text, 0);
    UINT type;
    if (ctx->routine && !ctx->routine->DebugInfo()) {
        type = 0x1030;
    } else {
        text << "Do you wish to debug the code?";
        type = 0x1024;
    }
    if (AskUser(text, "VM Exception", type) == IDYES) {
        mState = kPdbAtException;
        mContext = ctx;
        CConsoleEdit *editor = mFrame->EditorWnd();
        editor->mEditable = 0;
        mRequest->Signal();
        mResume->Wait(INFINITE);
        editor->mEditable = 1;
    }
    Notify(kPdbExceptionDone);
    return 0;
}

// 0x100040c0 ?HandleCommand@MfPdbDebugger@@UAEHHHPAVMfAiPiccoloEditor@@@Z
int MfPdbDebugger::HandleCommand(int toolbar, int id, MfAiPiccoloEditor *editor)
{
    MfString module;
    int column, line;
    editor->GetPosition(&column, &line);
    editor->GetModuleName(&module);
    if (toolbar == kToolbarBreakpoint) {
        switch (id) {
        case kButtonToggle:
            mBreakpoints->ToggleBreakpoint(module, line);
            break;
        case kButtonEnable:
            mBreakpoints->ToggleEnabled(module, line);
            break;
        case kButtonDisableAll:
            mBreakpoints->DisableAll();
            break;
        case kButtonDisableModule:
            mBreakpoints->DisableModule(module);
            break;
        case kButtonClearAll:
            mBreakpoints->RemoveAll();
            break;
        case kButtonClearModule:
            mBreakpoints->RemoveModule(module);
            break;
        case kButtonEdit: {
            MfAi *windows;
            CWnd *wnd;
            if (editor->GetInterface(1, &windows) && ((MfAiPeWindows *)windows)->GetMainWindow(&wnd)) {
                PscDebuggerContext context;
                MfPdbEditBreakpointsDlg dlg(wnd);
                dlg.DoModal();
                windows->Release();
            }
            break;
        }
        }
    }
    return 1;
}

// 0x100042e0 ??1PscDebuggerContext@@UAE@XZ
// 0x10004300 ??_GPscDebuggerContext@@UAEPAXI@Z
// 0x10004330 ??1MfPdbEditBreakpointsDlg@@UAE@XZ

// 0x100043a0 ?KbhKeyDown@MfPdbDebugger@@UAEHHHPAVMfAiPiccoloEditor@@@Z
int MfPdbDebugger::KbhKeyDown(int key, int flags, MfAiPiccoloEditor *editor)
{
    int handled = 0;
    switch (key) {
    case 'S':
        if (flags == 2) {
            mFrame->PostMessage(WM_COMMAND, ID_PDB_SAVE);
            handled = 1;
        }
        break;
    case 'O':
        if (flags == 2) {
            mFrame->PostMessage(WM_COMMAND, ID_PDB_SELECT_MODULE);
            handled = 1;
        }
        break;
    case VK_F5:
        if (mState == kPdbAtBreakpoint) {
            if (flags & 2) {
                mFrame->PostMessage(WM_COMMAND, ID_PDB_STOP);
                handled = 1;
            } else if (flags == 0) {
                mFrame->PostMessage(WM_COMMAND, ID_PDB_GO);
                handled = 1;
            }
        }
        break;
    case VK_F6:
        if (mState == kPdbAtBreakpoint && flags == 0) {
            mFrame->PostMessage(WM_COMMAND, ID_PDB_STEP_OVER);
            handled = 1;
        }
        break;
    case VK_F7:
        if (mState == kPdbAtBreakpoint && flags == 0) {
            mFrame->PostMessage(WM_COMMAND, ID_PDB_STEP_INTO);
            handled = 1;
        }
        break;
    case VK_F8:
        if (mState == kPdbAtBreakpoint) {
            if (flags & 2) {
                mFrame->PostMessage(WM_COMMAND, ID_PDB_STEP_OUT);
                handled = 1;
            } else if (flags == 0) {
                mFrame->PostMessage(WM_COMMAND, ID_PDB_RUN_TO_CURSOR);
                handled = 1;
            }
        }
        break;
    case 'K':
        if (flags & 2) {
            HandleCommand(kToolbarBreakpoint, kButtonEdit, editor);
            handled = 1;
        }
        break;
    case 'T':
        if (flags == 2) {
            HandleCommand(kToolbarBreakpoint, kButtonToggle, editor);
            handled = 1;
        } else if (flags == 6) {
            HandleCommand(kToolbarBreakpoint, kButtonEnable, editor);
            handled = 1;
        }
        break;
    }
    return handled;
}

// 0x10004590 ?HandleUpdate@MfPdbDebugger@@UAEHHHPAVCCmdUI@@@Z
int MfPdbDebugger::HandleUpdate(int toolbar, int id, CCmdUI *cmdUI)
{
    cmdUI->Enable(TRUE);
    return 1;
}

// 0x100045b0 ?DoWork@MfPdbDebugger@@UAEXXZ
void MfPdbDebugger::DoWork()
{
    while (pdbUIThread) {
        mRequest->Wait(INFINITE);
        if (mState == kPdbAtBreakpoint) {
            mFrame->PostMessage(WM_COMMAND, ID_PDB_SHOW_BREAKPOINT);
            mDone->Wait(INFINITE);
        } else if (mState == kPdbAtException) {
            mFrame->PostMessage(WM_COMMAND, ID_PDB_SHOW_EXCEPTION);
            mDone->Wait(INFINITE);
        } else if (mState == kPdbMessage) {
            mFrame->PostMessage(WM_COMMAND, ID_PDB_SHOW_MESSAGE);
            mDone->Wait(INFINITE);
        }
        if (pdbUIThread)
            mFrame->ClearCurrentLine();
        mState = kPdbIdle;
        mContext = 0;
        Sleep(0);
        Sleep(0);
        Sleep(0);
        if (mResume->IsWaiting())
            mResume->Signal();
    }
}

// 0x10004690 ?OnShowBreakpoint@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnShowBreakpoint()
{
    MfString module(mContext->routine->ModuleName());
    MfString moduleType(*mContext->routine->mModuleType);
    MfScBytecodeRoutine *routine = mContext->routine;
    int offset = mContext->env->m_ip - routine->mBytecodes;
    int line;
    if (routine->mDebugInfo)
        line = routine->mDebugInfo->FindLine(offset);
    else
        line = -1;
    mStopModule = module;
    mStopLine = line;
    MfAiSourceType *source;
    if (MfScPiccolo::GetSourceResolver()->Find(moduleType, &source)) {
        unsigned long address = (unsigned long)mContext->env->m_ip - 1;
        MfPdbBreakpoint *bp = mBreakpoints->FindBreakpoint(address);
        int show = 1;
        mBytecode = bp->mBytecode;
        unsigned long data = bp->mData;
        if (data == (kStepOver | kStepInvoke)) {
            mBreakpoints->DeleteBreakpoint(address);
            mBreakpoints->PlantStepInto(mContext);
            show = 0;
        } else if (data & kStepJump) {
            mBreakpoints->DeleteBreakpoint(address);
            mBreakpoints->PlantJump(mContext, data);
            show = 0;
        } else if (data & kStepTarget) {
            mBreakpoints->DeleteBreakpoint(address);
            mBreakpoints->PlantStep(mContext, data & kStepModes, 1);
            show = 0;
        } else if (data == kStepTemporary) {
            mBreakpoints->DeleteBreakpoint(address);
        }
        if (show) {
            CConsoleEdit *editor = mFrame->EditorWnd();
            if (module != mFrame->mFileName && editor->IsModified()) {
                MfString msg = MfString("The text in the debugger buffer for the module\r\n\r\n\t")
                    + mFrame->mFileName
                    + MfString("\r\n\r\nhas changed. Do you wish to save these changes before switching to the module\r\n\r\n\t")
                    + module + MfString("\r\n\r\nIf you choose no, your changes will be lost.");
                if (mFrame->MessageBox(msg, "Query", MB_YESNO | MB_ICONQUESTION) == IDYES)
                    mFrame->SendMessage(WM_COMMAND, ID_PDB_SAVE);
            }
            if (mFrame->LoadModule(line, module, source, mContext->env, 1)) {
                mModule = module;
                mModuleType = moduleType;
                mFrame->Activate();
                return;
            } else {
                MfString msg;
                msg << "The debugger could not load the source from the provider.\r\n"
                    << "The module name is: " << module << "\r\n" << "and the module type is: "
                    << moduleType << "\r\n\r\n" << "Press OK to continue program execution";
                ::MessageBox(0, msg, "Debugger Error", MB_ICONERROR | MB_SYSTEMMODAL);
                Sleep(0);
                Sleep(0);
                Sleep(0);
                if (mDone->IsWaiting())
                    mDone->Signal();
            }
        }
        source->Release();
    } else {
        MfString msg;
        msg << "The debugger cannot find the source provider.\r\n" << "The module name is: " << module
            << "\r\n" << "and the module type is: " << moduleType << "\r\n\r\n"
            << "Press OK to continue program execution";
        ::MessageBox(0, msg, "Debugger Error", MB_ICONERROR | MB_SYSTEMMODAL);
        Sleep(0);
        Sleep(0);
        Sleep(0);
        if (mDone->IsWaiting())
            mDone->Signal();
    }
    Sleep(0);
    Sleep(0);
    Sleep(0);
    if (mDone->IsWaiting())
        mDone->Signal();
}

// EQUIV 0x10004b50 ?OnShowException@MfPdbDebugger@@IAEXXZ: reviewed: register choice only (env->m_ip loaded through eax in the original, edx here; one lea/push through ecx vs eax)
void MfPdbDebugger::OnShowException()
{
    if (!mContext->routine) {
        MfScVmFrame frame;
        mContext->env->FirstFrame(&frame);
        mContext->env->NextFrame(&frame);
        mContext->routine =
            frame.mFunction ? dynamic_cast<MfScBytecodeRoutine *>(frame.mFunction->GetData()) : 0;
        mContext->env->RestoreFrame(&frame);
    }
    MfString module(mContext->routine->ModuleName());
    MfString moduleType(*mContext->routine->mModuleType);
    MfScBytecodeRoutine *routine = mContext->routine;
    int offset = mContext->env->m_ip - routine->mBytecodes;
    int line;
    if (routine->mDebugInfo)
        line = routine->mDebugInfo->FindLine(offset);
    else
        line = -1;
    mStopModule = module;
    mStopLine = line;
    MfAiSourceType *source;
    if (MfScPiccolo::GetSourceResolver()->Find(moduleType, &source)) {
        if (mFrame->LoadModule(line, module, source, mContext->env, 1)) {
            mModule = module;
            mModuleType = moduleType;
            mFrame->Activate();
            return;
        } else {
            MfString msg;
            msg << "The debugger could not load the source from the provider.\r\n" << "The module name is: "
                << module << "\r\n" << "and the module type is: " << moduleType << "\r\n\r\n"
                << "Press OK to continue program execution";
            ::MessageBox(0, msg, "Debugger Error", MB_ICONERROR | MB_SYSTEMMODAL);
            Sleep(0);
            Sleep(0);
            Sleep(0);
            if (mDone->IsWaiting())
                mDone->Signal();
        }
        source->Release();
    } else {
        MfString msg;
        msg << "The debugger cannot find the source provider.\r\n" << "The module name is: " << module
            << "\r\n" << "and the module type is: " << moduleType << "\r\n\r\n"
            << "Press OK to continue program execution";
        ::MessageBox(0, msg, "Debugger Error", MB_ICONERROR | MB_SYSTEMMODAL);
        Sleep(0);
        Sleep(0);
        Sleep(0);
        if (mDone->IsWaiting())
            mDone->Signal();
    }
    Sleep(0);
    Sleep(0);
    Sleep(0);
    if (mDone->IsWaiting())
        mDone->Signal();
}

// 0x10004ed0 ?OnShowMessage@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnShowMessage()
{
    mMessageBox.mResult = ::MessageBox(mMessageBox.mOwner, mMessageBox.mText, mMessageBox.mCaption,
                                       mMessageBox.mType);
    Sleep(0);
    Sleep(0);
    Sleep(0);
    if (mDone->IsWaiting())
        mDone->Signal();
}

// 0x10004f30 ?Continue@MfPdbDebugger@@QAEXXZ
void MfPdbDebugger::Continue()
{
    if (mDone->IsWaiting())
        mDone->Signal();
    mFrame->ClearCurrentLine();
}

// 0x10004f60 ?GetVariable@MfPdbDebugger@@QAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPdbDebugger::GetVariable(const MfString &name, MfScDataHandle **value)
{
    int found = 1;
    if (name == "this") {
        if (mContext->env->m_object)
            mContext->env->m_object->AddRef();
        *value = mContext->env->m_object;
    } else {
        MfScBytecodeRoutine *routine = mContext->routine;
        int offset = mContext->env->m_ip - routine->mBytecodes;
        MfScDebugInfo *info = routine->mDebugInfo;
        if (info) {
            int level = info->FindLevel(offset);
            if (level <= 0)
                level = 1;
            if (!info->GetVariable(name, mContext->env->m_frame, level, value)
                && mContext->env->GetVariable(name, value) != MfScVariableStack::kVarOk)
                found = 0;
        }
    }
    return found;
}

// 0x10005020 ?AskUser@MfPdbDebugger@@QAEHABVMfString@@0I@Z
int MfPdbDebugger::AskUser(const MfString &text, const MfString &caption, UINT type)
{
    mState = kPdbMessage;
    mContext = 0;
    mMessageBox.mOwner = 0;
    mMessageBox.mText = text;
    mMessageBox.mCaption = caption;
    mMessageBox.mType = type;
    mRequest->Signal();
    mResume->Wait(INFINITE);
    return mMessageBox.mResult;
}

// 0x100050a0 ?_GetBaseMessageMap@MfPdbDebugger@@KGPBUAFX_MSGMAP@@XZ
// 0x100050b0 ?GetMessageMap@MfPdbDebugger@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(MfPdbDebugger, CCmdTarget)
    ON_COMMAND(ID_PDB_GO, OnGo)
    ON_UPDATE_COMMAND_UI(ID_PDB_GO, OnUpdateStopped)
    ON_COMMAND(ID_PDB_STOP, OnStop)
    ON_UPDATE_COMMAND_UI(ID_PDB_STOP, OnUpdateStopped)
    ON_COMMAND(ID_PDB_STEP_OVER, OnStepOver)
    ON_UPDATE_COMMAND_UI(ID_PDB_STEP_OVER, OnUpdateStopped)
    ON_COMMAND(ID_PDB_STEP_INTO, OnStepInto)
    ON_UPDATE_COMMAND_UI(ID_PDB_STEP_INTO, OnUpdateStopped)
    ON_COMMAND(ID_PDB_EDIT_BREAKPOINTS, OnEditBreakpoints)
    ON_COMMAND(ID_PDB_SELECT_MODULE, OnSelectModule)
    ON_COMMAND(ID_PDB_RUN_TO_CURSOR, OnRunToCursor)
    ON_UPDATE_COMMAND_UI(ID_PDB_RUN_TO_CURSOR, OnUpdateStopped)
    ON_COMMAND(ID_PDB_STEP_OUT, OnStepOut)
    ON_UPDATE_COMMAND_UI(ID_PDB_STEP_OUT, OnUpdateStopped)
    ON_COMMAND(ID_PDB_SAVE, OnSave)
    ON_UPDATE_COMMAND_UI(ID_PDB_SAVE, OnUpdateSave)
    ON_COMMAND(ID_PDB_VIEW_VARIABLE, OnViewVariable)
    ON_UPDATE_COMMAND_UI(ID_PDB_VIEW_VARIABLE, OnUpdateActive)
    ON_UPDATE_COMMAND_UI(ID_PDB_SELECT_MODULE, OnUpdateActive)
    ON_COMMAND(ID_PDB_VM_LIST, OnVmList)
    ON_COMMAND(ID_PDB_SHOW_BREAKPOINT, OnShowBreakpoint)
    ON_COMMAND(ID_PDB_SHOW_EXCEPTION, OnShowException)
    ON_COMMAND(ID_PDB_SHOW_MESSAGE, OnShowMessage)
END_MESSAGE_MAP()

// 0x100050c0 ?OnGo@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnGo()
{
    Continue();
}

// 0x100050d0 ?OnStop@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnStop()
{
    mContext->env->m_stop = 1;
    mStepping = 1;
    mFrame->OnClose();
}

// 0x100050f0 ?OnStepOver@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnStepOver()
{
    mBreakpoints->PlantStep(mContext, kStepOver, 1);
    OnGo();
}

// 0x10005110 ?OnStepInto@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnStepInto()
{
    mBreakpoints->PlantStep(mContext, kStepInto, 1);
    OnGo();
}

// 0x10005130 ?OnUpdateStopped@MfPdbDebugger@@IAEXPAVCCmdUI@@@Z
void MfPdbDebugger::OnUpdateStopped(CCmdUI *cmdUI)
{
    cmdUI->Enable(mState == kPdbAtBreakpoint);
}

// 0x10005150 ?OnRunToCursor@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnRunToCursor()
{
    MfAiPiccoloEditor *editor = mFrame->EditorWnd()->GetIEditor();
    int column, line;
    editor->GetPosition(&column, &line);
    MfString module;
    editor->GetModuleName(&module);
    mBreakpoints->PlantLine(mContext, module, line);
    OnGo();
}

// 0x100051f0 ?OnStepOut@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnStepOut()
{
    mBreakpoints->PlantReturn(mContext);
    OnGo();
}

// 0x10005210 ?OnEditBreakpoints@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnEditBreakpoints()
{
    PscDebuggerContext context;
    MfPdbEditBreakpointsDlg dlg(mFrame);
    dlg.DoModal();
}

// 0x100052f0 ?OnViewVariable@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnViewVariable()
{
    PscDebuggerContext context;
    MfPdbViewVariable dlg;
    dlg.mFrame = mFrame;
    dlg.DoModal();
}

// 0x100053a0 ?OnSelectModule@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnSelectModule()
{
    if (mFrame->EditorWnd()->IsModified()) {
        int answer = mFrame->MessageBox("Text has been modified. Save changes?", "Query",
                                        MB_YESNOCANCEL | MB_ICONQUESTION);
        if (answer == IDYES)
            mFrame->SendMessage(WM_COMMAND, ID_PDB_SAVE);
        else if (answer == IDCANCEL)
            return;
    }
    PscDebuggerContext context;
    MfPdbModuleSelector dlg(mFrame);
    if (dlg.DoModal() == IDOK && mModule != dlg.mModule) {
        mModule = dlg.mModule;
        mModuleType = dlg.mModuleType;
        MfAiSourceType *source;
        if (MfScPiccolo::GetSourceResolver()->Find(dlg.mModuleType, &source)) {
            MfString module(dlg.mModule);
            if (mModule == mStopModule)
                mFrame->LoadModule(mStopLine, module, source, mContext->env, 1);
            else
                mFrame->LoadModule(1, module, source, mContext->env, 0);
            source->Release();
        }
    }
}

// 0x100055e0 ??1MfPdbModuleSelector@@UAE@XZ

// 0x10005670 ?OnUpdateActive@MfPdbDebugger@@IAEXPAVCCmdUI@@@Z
void MfPdbDebugger::OnUpdateActive(CCmdUI *cmdUI)
{
    cmdUI->Enable(mState != kPdbIdle);
}

// 0x10005690 ?OnSave@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnSave()
{
    MfAiSourceType *source;
    if (MfScPiccolo::GetSourceResolver()->Find(mModuleType, &source)) {
        CConsoleEdit *editor = mFrame->EditorWnd();
        MfString text = editor->GetEntireContents();
        if (source->SaveSource(mModule, text, mContext->env) == 0) {
            editor->SetUnModified();
            mFrame->SetFileName(mModule);
        } else {
            MfString msg = MfString("Error saving module\r\n\r\n\t") + mModule;
            mFrame->MessageBox(msg, "Error", MB_ICONERROR);
        }
        source->Release();
    } else {
        MfString msg;
        msg << "The debugger cannot find the source provider.\r\n" << "The module name is: " << mModule
            << "\r\n" << "and the module type is: " << mModuleType;
        mFrame->MessageBox(msg, "Debugger Error", MB_ICONERROR | MB_SYSTEMMODAL);
    }
}

// 0x10005830 ?OnUpdateSave@MfPdbDebugger@@IAEXPAVCCmdUI@@@Z
void MfPdbDebugger::OnUpdateSave(CCmdUI *cmdUI)
{
    if (mFrame) {
        CConsoleEdit *editor = mFrame->EditorWnd();
        cmdUI->Enable(mState && editor->IsModified());
    } else {
        cmdUI->Enable(FALSE);
    }
}

// 0x10005890 ?OnVmList@MfPdbDebugger@@IAEXXZ
void MfPdbDebugger::OnVmList()
{
    mDialog = new MfPdbVmListDlg(mFrame);
    mDialog->DoModal();
    delete mDialog;
    mDialog = 0;
}

// 0x10005920 ??3CObject@@SGXPAX@Z
// 0x10005930 ?GetInterfaceName@MfPdbDebugger@@W3AEXPADHAAH1@Z
// 0x10005940 ?AddRef@MfPdbDebugger@@W3AEHXZ
// 0x10005950 ?Release@MfPdbDebugger@@W3AEHXZ
// 0x10005960 ??_EMfPdbDebugger@@WCI@AEPAXI@Z
