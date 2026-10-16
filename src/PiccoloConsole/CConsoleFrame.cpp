// CConsoleFrame: the Piccolo console. A file editor whose commands compile
// the file, store the functions, run them on the script thread and look at
// the runtime (the handle pool, object files, the diagnostics).
#include "PiccoloConsole.h"

// The registry entries; the code loads the pointers from memory.
static MfScDataHandle *nullHandle = (MfScDataHandle *)-1;
static const char *szLoggerOptions = "Logger Options";
static const char *szLoggerX = "x";
static const char *szLoggerY = "y";
static const char *szLoggerCx = "cx";
static const char *szLoggerCy = "cy";
static const char *szLoggerFlags = "Flags";
static const char *szPiccoloConsole = "Piccolo Console";
static const char *szPiccoloFileEditor = "Piccolo File Editor";
static const char *szMRUSource = "MRU Source";
static const char *szMRUObjects = "MRU Objects";
static const char *szMRUSourceEntry = "name%d";
static const char *szMRUObjectsEntry = "pobj%d";
static const char *szOptions = "Piccolo Console Options";
static const char *szStoreAfterCompile = "StoreAfterCompile";
static const char *szClearAfterStore = "ClearAfterStore";
static const char *szWriteSeparateDebug = "WriteSeparateDebug";
static const char *szAutoSwitchToOutput = "AutoSwitchToOutput";
static const char *szPwTitleBar = "PwTitleBar";
static const char *szPwToolBar = "PwToolBar";
static const char *szX = "x";
static const char *szY = "y";
static const char *szCx = "cx";
static const char *szCy = "cy";
static const char *szPaneSize = "Pane Size";
static const char *szPwTabSize = "PwTabSize";
static const char *szPwSyntaxHighlight = "PwSyntaxHighlight";
static const char *szPwAutoIndent = "PwAutoIndent";

static UINT indicators[] = {
    ID_SEPARATOR,
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL
};

CConsoleFrame *CConsoleFrame::pcfMainFrame;

static inline MfScDataMethod *MethodOf(MfScDataHandle *handle)
{
    return handle ? dynamic_cast<MfScDataMethod *>(handle->GetData()) : 0;
}

// 0x100014b0 ?GetMessageMap@CConsoleFrame@@MBEPBUAFX_MSGMAP@@XZ
BEGIN_MESSAGE_MAP(CConsoleFrame, CPiccoloFileEditor)
    ON_WM_CREATE()
    ON_COMMAND(ID_PICCOLO_BUILD, OnPiccoloCompile)
    ON_WM_PARENTNOTIFY()
    ON_COMMAND(ID_DIAGNOSTICS_LEVEL1, OnDiagnosticsLevel1)
    ON_UPDATE_COMMAND_UI(ID_DIAGNOSTICS_LEVEL1, OnUpdateDiagnosticsLevel1)
    ON_COMMAND(ID_DIAGNOSTICS_LEVEL2, OnDiagnosticsLevel2)
    ON_UPDATE_COMMAND_UI(ID_DIAGNOSTICS_LEVEL2, OnUpdateDiagnosticsLevel2)
    ON_COMMAND(ID_DIAGNOSTICS_LEVEL3, OnDiagnosticsLevel3)
    ON_UPDATE_COMMAND_UI(ID_DIAGNOSTICS_LEVEL3, OnUpdateDiagnosticsLevel3)
    ON_COMMAND(ID_DIAGNOSTICS_LEVEL4, OnDiagnosticsLevel4)
    ON_UPDATE_COMMAND_UI(ID_DIAGNOSTICS_LEVEL4, OnUpdateDiagnosticsLevel4)
    ON_COMMAND(ID_DIAGNOSTICS_NO_LEVEL, OnDiagnosticsNoLevel)
    ON_UPDATE_COMMAND_UI(ID_DIAGNOSTICS_NO_LEVEL, OnUpdateDiagnosticsNoLevel)
    ON_COMMAND(ID_PICCOLO_RUN, OnPiccoloRun)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_RUN, OnUpdatePiccoloRun)
    ON_COMMAND(ID_PICCOLO_STORE, OnPiccoloStore)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_STORE, OnUpdatePiccoloStore)
    ON_COMMAND(ID_PICCOLO_RELEASE, OnPiccoloRelease)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_RELEASE, OnUpdatePiccoloRelease)
    ON_COMMAND(ID_PICCOLO_DUMPPOOL, OnPiccoloDumppool)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_DUMPPOOL, OnUpdatePiccoloDumppool)
    ON_COMMAND(ID_PICCOLO_CLEARVARS, OnPiccoloClearvars)
    ON_COMMAND(ID_PICCOLO_CLEAR_DIAG, OnPiccoloClearDiag)
    ON_COMMAND(ID_PICCOLO_STOP, OnPiccoloStop)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_STOP, OnUpdatePiccoloStop)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_BUILD, OnUpdatePiccoloCompile)
    ON_WM_CLOSE()
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_COMMAND(ID_FILE_SAVE, OnFileSave)
    ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
    ON_COMMAND(ID_FILE_MRU_FILE1, OnFileMruFile1)
    ON_COMMAND(ID_FILE_MRU_FILE2, OnFileMruFile2)
    ON_COMMAND(ID_FILE_MRU_FILE3, OnFileMruFile3)
    ON_COMMAND(ID_FILE_MRU_FILE4, OnFileMruFile4)
    ON_COMMAND(ID_FILE_MRU_FILE5, OnFileMruFile5)
    ON_COMMAND(ID_FILE_MRU_FILE6, OnFileMruFile6)
    ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateFileMruFile1)
    ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
    ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
    ON_COMMAND(ID_FILE_SAVE_OBJECT_FILE, OnFileSaveObjectFile)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_OBJECT_FILE, OnUpdateFileSaveObjectFile)
    ON_COMMAND(ID_FILE_LOAD_OBJECT_FILE, OnFileLoadObjectFile)
    ON_COMMAND(ID_FILE_MRU_POBJ1, OnFileMruPobj1)
    ON_COMMAND(ID_FILE_MRU_POBJ2, OnFileMruPobj2)
    ON_COMMAND(ID_FILE_MRU_POBJ3, OnFileMruPobj3)
    ON_COMMAND(ID_FILE_MRU_POBJ4, OnFileMruPobj4)
    ON_COMMAND(ID_FILE_MRU_POBJ5, OnFileMruPobj5)
    ON_COMMAND(ID_FILE_MRU_POBJ6, OnFileMruPobj6)
    ON_UPDATE_COMMAND_UI(ID_FILE_MRU_POBJ1, OnUpdateFileMruPobj1)
    ON_WM_SIZE()
    ON_COMMAND(ID_VIEW_PW_TITLE_BAR, OnViewPwTitleBar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PW_TITLE_BAR, OnUpdateViewPwTitleBar)
    ON_COMMAND(ID_VIEW_PW_TOOL_BAR, OnViewPwToolBar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PW_TOOL_BAR, OnUpdateViewPwToolBar)
    ON_COMMAND(ID_VIEW_PICCOLO_CLASSES, OnViewPiccoloClasses)
    ON_COMMAND(ID_NEW_PICCOLO_WND, OnNewPiccoloWnd)
    ON_WM_TIMER()
    ON_COMMAND(ID_PICCOLO_STRESS_TEST, OnPiccoloStressTest)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_STRESS_TEST, OnUpdatePiccoloStressTest)
    ON_COMMAND(ID_VIEW_PICCOLO_WINDOW, OnViewPiccoloWindow)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PICCOLO_WINDOW, OnUpdateViewPiccoloWindow)
    ON_COMMAND(ID_ENTER_MESSAGE_LOOP, OnEnterMessageLoop)
    ON_UPDATE_COMMAND_UI(ID_ENTER_MESSAGE_LOOP, OnUpdateEnterMessageLoop)
    ON_COMMAND(ID_EXIT_MESSAGE_LOOP, OnExitMessageLoop)
    ON_UPDATE_COMMAND_UI(ID_EXIT_MESSAGE_LOOP, OnUpdateExitMessageLoop)
    ON_COMMAND(ID_BUFFER_EMPTY, OnBufferEmpty)
    ON_UPDATE_COMMAND_UI(ID_BUFFER_EMPTY, OnUpdateBufferEmpty)
    ON_COMMAND(ID_NEW_CONSOLE_WND, OnNewConsoleWnd)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_COMMAND(ID_EDIT_CUT, OnEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
    ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
    ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
    ON_COMMAND(ID_BUILD_PARAMETERS, OnBuildParameters)
    ON_COMMAND(ID_FILE_SHUTDOWN, OnFileShutdown)
    ON_UPDATE_COMMAND_UI(ID_FILE_SHUTDOWN, OnUpdateFileShutdown)
    ON_COMMAND(ID_FILE_STARTUP, OnFileStartup)
    ON_UPDATE_COMMAND_UI(ID_FILE_STARTUP, OnUpdateFileStartup)
    ON_COMMAND(ID_PICCOLO_COMPILE_STRESS_TEST, OnPiccoloCompileStressTest)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_COMPILE_STRESS_TEST, OnUpdatePiccoloCompileStressTest)
    ON_COMMAND(ID_PICCOLO_COMPILE_END_STRESS_TEST, OnPiccoloCompileEndStressTest)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_COMPILE_END_STRESS_TEST, OnUpdatePiccoloCompileEndStressTest)
    ON_COMMAND(ID_PICCOLO_CLEAR_OUTPUT, OnPiccoloClearOutput)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_CLEAR_DIAG, OnUpdatePiccoloClearDiag)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_CLEAR_OUTPUT, OnUpdatePiccoloClearOutput)
    ON_WM_SETFOCUS()
    ON_COMMAND(ID_PICCOLO_STORE_GLOBAL, OnPiccoloStoreGlobal)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_STORE_GLOBAL, OnUpdatePiccoloStoreGlobal)
    ON_COMMAND(ID_FLUSH_PICCOLO_FUNCTIONS, OnFlushPiccoloFunctions)
    ON_COMMAND(ID_LOAD_PICCOLO_FUNCTIONS, OnLoadPiccoloFunctions)
    ON_COMMAND(ID_PICCOLO_DUMP_MEMUSAGE, OnPiccoloDumpMemusage)
    ON_COMMAND(ID_FILE_STRIP_OBJECT_FILE, OnFileStripObjectFile)
    ON_COMMAND(ID_FILE_DUMP_OBJECT_FILE, OnFileDumpObjectFile)
    ON_COMMAND(ID_LOCK_GC, OnLockGc)
    ON_COMMAND(ID_UNLOCK_GC, OnUnlockGc)
    ON_COMMAND(ID_OPEN_FROM_GLOBAL, OnOpenFromGlobal)
    ON_COMMAND(ID_OPEN_FROM_PROJECT, OnOpenFromProject)
    ON_COMMAND(ID_SAVE_TO_GLOBAL, OnSaveToGlobal)
    ON_UPDATE_COMMAND_UI(ID_SAVE_TO_GLOBAL, OnUpdateSaveToGlobal)
    ON_COMMAND(ID_SAVE_TO_PROJECT, OnSaveToProject)
    ON_UPDATE_COMMAND_UI(ID_SAVE_TO_PROJECT, OnUpdateSaveToProject)
    ON_COMMAND(ID_EDIT_FIND_FIRST, OnEditFindFirst)
    ON_COMMAND(ID_EDIT_FIND_NEXT, OnEditFindNext)
    ON_COMMAND(ID_RECOMPILE_PROJECT, OnRecompileProject)
    ON_COMMAND(ID_RECOMPILE_GLOBAL, OnRecompileGlobal)
    ON_COMMAND(ID_PICCOLO_DEFAULT, OnPiccoloDefault)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_DEFAULT, OnUpdatePiccoloDefault)
    ON_COMMAND(ID_PICCOLO_NEED_FUNCTIONS, OnPiccoloNeedFunctions)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_NEED_FUNCTIONS, OnUpdatePiccoloNeedFunctions)
    ON_COMMAND(ID_PICCOLO_NO_FUNCTIONS, OnPiccoloNoFunctions)
    ON_UPDATE_COMMAND_UI(ID_PICCOLO_NO_FUNCTIONS, OnUpdatePiccoloNoFunctions)
    ON_UPDATE_COMMAND_UI(ID_RECOMPILE_GLOBAL, OnUpdatePiccoloRecompileGlobal)
    ON_UPDATE_COMMAND_UI(ID_RECOMPILE_PROJECT, OnUpdatePiccoloRecompileProject)
    ON_COMMAND(ID_VIEW_ADVANCED_MENU, OnViewAdvancedMenu)
    ON_COMMAND(ID_VIEW_BASIC_MENU, OnViewBasicMenu)
    ON_MESSAGE(WM_WORK_FINISHED, OnHandleWorkFinished)
    ON_MESSAGE(WM_EDITOR_HAS_FOCUS, OnHandleEditorHasFocus)
END_MESSAGE_MAP()

// 0x100014c0 ??0CConsoleFrame@@QAE@HH@Z
CConsoleFrame::CConsoleFrame(int, int resourceId)
    : CPiccoloFileEditor(NULL_STRING, szPiccoloConsole),
      mSourceMRU(0), mObjectMRU(0), mGlobalPool(0), mSecondPool(0), mThirdPool(0), mVariables(0),
      mStressTester(0), mClearAfterStore(0), mStoreAfterCompile(1), mAutoSwitchToOutput(1),
      mWriteSeparateDebug(1), mPwTitleBar(0), mPwToolBar(1), mShowPiccoloWnd(1), mCompileFlags(0),
      mInMessageLoop(0), mCheckingFileTime(0), mPiccoloRunning(1), mResourceId(resourceId)
{
    if (resourceId == 0x82)
        SetBaseTitle(szPiccoloFileEditor);
    if (pcfMainFrame == NULL)
        pcfMainFrame = this;

    UtilityApp app;
    mStoreAfterCompile = app.GetProfileInt(szOptions, szStoreAfterCompile, 1);
    mClearAfterStore = app.GetProfileInt(szOptions, szClearAfterStore, 0);
    mAutoSwitchToOutput = app.GetProfileInt(szOptions, szAutoSwitchToOutput, 1);
    mWriteSeparateDebug = app.GetProfileInt(szOptions, szWriteSeparateDebug, 1);
    mPwTitleBar = app.GetProfileInt(szOptions, szPwTitleBar, 0);
    mPwToolBar = app.GetProfileInt(szOptions, szPwToolBar, 1);
    mPaneSize = app.GetProfileInt(szOptions, szPaneSize, -1);
    mFlags &= ~7;
    mFlags |= PiccoloWindowFlags();

    mExecWork = new MfExecWork(this);
    mThread = new MfScThread(2);
    mSourceMRU = new CRecentFileList(0, szMRUSource, szMRUSourceEntry, 5, 30);
    mSourceMRU->ReadList();
    mObjectMRU = new CRecentFileList(5, szMRUObjects, szMRUObjectsEntry, 5, 30);
    mObjectMRU->ReadList();
    mGlobalPool = new MfScHandleDict;
    mSecondPool = new MfScHandleDict;
    mThirdPool = new MfScHandleDict;
    mVariables = new MfScVariableStack(mGlobalPool, 0);
    mVariables->PushVarPool(mSecondPool);
    mVariables->PushVarPool(mThirdPool);
    SetupVariables();
}

// 0x10001b70 ??_GMfScHandleDict@@UAEPAXI@Z
// 0x10001b90 ??1MfThreadWork@@UAE@XZ
// 0x10001ba0 ??_GMfThreadWork@@UAEPAXI@Z

// TODO 0x10001bc0 ?DoWork@MfExecWork@@UAEXXZ: 54% - same code, different register allocation (the original keeps this in ebx with a spill at [esp+0x18] and 0 in ebp)
void MfExecWork::DoWork()
{
    int count = 0;
    MfScDataHandle *params[2];
    MfScBytecodeRoutine *routine = 0;
    if (mFunction)
        routine = dynamic_cast<MfScBytecodeRoutine *>(mFunction->GetData());

    mParameters.NoLeadingWhiteSpace();
    mParameters.NoTrailingWhiteSpace();
    if (routine->m_numArgs == -1 || routine->m_numArgs == 2) {
        // main(argc, argv): the module of the function and the parameters
        count = 2;
        MfStringList words;
        if (mParameters != NULL_STRING)
            mParameters.Tokenize(" \t\n", words);
        params[0] = MfScDataHandle::CreateData(words.GetCount() + 1);
        params[1] = MfScDataHandle::CreateDataArray(words.GetCount() + 1);
        MfScDataArray *arguments = (MfScDataArray *)params[1]->GetData();
        arguments->SetAt(0, MfScDataHandle::CreateData(routine->ModuleName(), 0), 1);
        if (words.GetCount() > 0) {
            MfDblListItem *item;
            MfString *word = (MfString *)words.First(item);
            for (int i = 1; i <= words.GetCount(); i++) {
                arguments->SetAt(i, MfScDataHandle::CreateData(*word, 0), 1);
                word = (MfString *)MfDblList::Next(item);
            }
        }
    }

    mBusy = 1;
    if (mFunction) {
        MfAiPdbDebugger *debugger = MfScDebugger::pdbgTheDebugger;
        if (debugger)
            debugger->ProgramState(0);
        mContext = ((MfScThread *)MfThread::GetThread())->mContext;
        mContext->mVariables = mVariables;
        MfHPTimer timer;
        timer.Start();
        if (mLoop) {
            while (!mStop) {
                MfScDataHandle *result = mMachine.ExecuteFunctionArrayParams(mFunction, count, params);
                Sleep(50);
                if (result)
                    result->Release();
            }
        } else {
            MfScDataHandle *result = mMachine.ExecuteFunctionArrayParams(mFunction, count, params);
            if (result)
                result->Release();
        }
        timer.Stop();
        mContext->mVariables = 0;
        mVariables = 0;
        double ms = timer.GetElapsedTimeInMs();
        int seconds = (int)(ms * 0.001);
        int microseconds = (int)((ms - seconds * 1000.0) * 1000.0);
        ::PostMessage(mOwner->m_hWnd, WM_WORK_FINISHED, microseconds, seconds);
        if (debugger)
            debugger->ProgramState(1);
    }
    mBusy = 0;
}

// 0x10001f20 ??_GMfExecWork@@UAEPAXI@Z
// 0x10001f40 ??1MfExecWork@@UAE@XZ
// 0x10001fb0 ??_ECConsoleFrame@@UAEPAXI@Z
// 0x10002010 ??_GMfScThread@@UAEPAXI@Z
// 0x10002030 ??_GMfScVariableStack@@UAEPAXI@Z

// 0x10002050 ??0CConsoleFrame@@QAE@XZ
CConsoleFrame::CConsoleFrame()
    : CPiccoloFileEditor(NULL_STRING, szPiccoloConsole),
      mSourceMRU(0), mObjectMRU(0), mGlobalPool(0), mSecondPool(0), mThirdPool(0), mVariables(0),
      mThread(0), mExecWork(0), mShowPiccoloWnd(1), mCompileFlags(0), mResourceId(0x80)
{
}

// 0x100021c0 ??1CConsoleFrame@@UAE@XZ
CConsoleFrame::~CConsoleFrame()
{
    if (pcfMainFrame == this)
        pcfMainFrame = NULL;
    if (mSourceMRU)
        mSourceMRU->WriteList();
    if (mObjectMRU)
        mObjectMRU->WriteList();

    UtilityApp app;
    app.WriteProfileInt(szOptions, szStoreAfterCompile, mStoreAfterCompile);
    app.WriteProfileInt(szOptions, szClearAfterStore, mClearAfterStore);
    app.WriteProfileInt(szOptions, szAutoSwitchToOutput, mAutoSwitchToOutput);
    app.WriteProfileInt(szOptions, szWriteSeparateDebug, mWriteSeparateDebug);
    app.WriteProfileInt(szOptions, szPwTitleBar, mPwTitleBar);
    app.WriteProfileInt(szOptions, szPwToolBar, mPwToolBar);

    delete mSourceMRU;
    delete mObjectMRU;
    if (mExecWork) {
        mThread->SignalShutdown(1);
        delete mExecWork;
    }
    delete mVariables;
    delete mGlobalPool;
    delete mSecondPool;
    delete mThirdPool;
}

// 0x10002520 ?Initialize@CConsoleFrame@@SAXXZ
void CConsoleFrame::Initialize()
{
    UtilityApp app;
    nPwTabSize = app.GetProfileInt(szOptions, szPwTabSize, nPwTabSize);
    bPwSyntaxHighlight = app.GetProfileInt(szOptions, szPwSyntaxHighlight, bPwSyntaxHighlight);
    bPwAutoIndent = app.GetProfileInt(szOptions, szPwAutoIndent, bPwAutoIndent);
}

// 0x10002670 ?Shutdown@CConsoleFrame@@SAXXZ
void CConsoleFrame::Shutdown()
{
    UtilityApp app;
    app.WriteProfileInt(szOptions, szPwTabSize, nPwTabSize);
    app.WriteProfileInt(szOptions, szPwSyntaxHighlight, bPwSyntaxHighlight);
    app.WriteProfileInt(szOptions, szPwAutoIndent, bPwAutoIndent);
}

// 0x100027b0 ?SetupVariables@CConsoleFrame@@QAEXXZ
void CConsoleFrame::SetupVariables()
{
    mSecondPool->Add("aNullVar", nullHandle, MfScHandleDict::kAddRef);
    mThirdPool->Add("aSetVar", MfScDataHandle::CreateData(0x45), MfScHandleDict::kPassRef);
    MfScDataHandle *readOnly = MfScDataHandle::CreateData(1.2345f);
    readOnly->SetReadOnly();
    mThirdPool->Add("aROVar", readOnly, MfScHandleDict::kPassRef);
    mSecondPool->Add("aDuplicateVar", MfScDataHandle::CreateData("Duplicate var at second level.", 0),
                     MfScHandleDict::kPassRef);
    mThirdPool->Add("aDuplicateVar", MfScDataHandle::CreateData("Duplicate var at third level.", 0),
                    MfScHandleDict::kPassRef);
}

// 0x10002970 ?DoCreateWindow@CConsoleFrame@@QAEXH@Z
void CConsoleFrame::DoCreateWindow(int)
{
    CreateFrameWindow(NULL);
}

// 0x10002980 ?CreateFrameWindow@CConsoleFrame@@MAEXPAVCWnd@@@Z
// 0x10002a20 ??1PiccoloConsoleContext@@UAE@XZ
// 0x10002a40 ??_GPiccoloConsoleContext@@UAEPAXI@Z
// plugins/pscclassinfo.dll 0x10001d70 ??1PiccoloConsoleContext@@UAE@XZ
// plugins/pscdebugger.dll 0x100042e0 ??1PiccoloConsoleContext@@UAE@XZ
// plugins/pscdebugger.dll 0x10004300 ??_GPiccoloConsoleContext@@UAEPAXI@Z
void CConsoleFrame::CreateFrameWindow(CWnd *)
{
    PiccoloConsoleContext context;
    LoadFrame(mResourceId, WS_OVERLAPPEDWINDOW, NULL, NULL);
    ShowWindow(SW_SHOW);
    UpdateWindow();
}

// 0x10002a70 ?PreCreateWindow@CConsoleFrame@@MAEHAAUtagCREATESTRUCTA@@@Z
BOOL CConsoleFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    UtilityApp app;
    cs.x = app.GetProfileInt(szOptions, szX, CW_USEDEFAULT);
    cs.y = app.GetProfileInt(szOptions, szY, CW_USEDEFAULT);
    cs.cx = app.GetProfileInt(szOptions, szCx, CW_USEDEFAULT);
    cs.cy = app.GetProfileInt(szOptions, szCy, CW_USEDEFAULT);
    return CFrameWnd::PreCreateWindow(cs);
}

// 0x10002c20 ?OpenFile@CConsoleFrame@@QAEHABVMfString@@H@Z
int CConsoleFrame::OpenFile(const MfString &fileName, int reload)
{
    MfString text;
    if (!text.ReadFromFile(fileName)) {
        MfString message;
        message.Format("Cannot open file:\n%s", (const char *)fileName);
        MessageBox(message, 0, 0);
        return 0;
    }
    CConsoleEdit *editor = mPiccoloWnd->EditorWnd();
    editor->SetEntireContents(text, reload);
    editor->SetUnModified();
    mTextModified = 0;
    mSourceMRU->Add(fileName);
    SetFileName(fileName);
    mFileTime.SetFromFile(fileName);
    return 1;
}

// 0x10002e00 ??_GMfStringRef@@UAEPAXI@Z

// 0x10002e30 ?SaveFile@CConsoleFrame@@QAEHABVMfString@@@Z
int CConsoleFrame::SaveFile(const MfString &fileName)
{
    CConsoleEdit *editor = mPiccoloWnd->EditorWnd();
    MfString text = editor->GetEntireContents();
    if (!text.WriteToFile(fileName))
        return 0;
    editor->SetUnModified();
    mTextModified = 0;
    SetFileName(fileName);
    mFileTime.SetFromFile(fileName);
    return 1;
}

// 0x10002f00 ?OpenMRUFile@CConsoleFrame@@QAEXH@Z
void CConsoleFrame::OpenMRUFile(int index)
{
    if (mPiccoloWnd->EditorWnd()->IsTextModified()) {
        int answer = MessageBox("Text has been modified. Save changes?", "Query", MB_YESNOCANCEL | MB_ICONQUESTION);
        if (answer == IDYES)
            OnFileSave();
        else if (answer == IDCANCEL)
            return;
    }
    if (!OpenFile(MfString((*mSourceMRU)[index]), 0))
        mSourceMRU->Remove(index);
}

// 0x10002fc0 ?OpenPobjFile@CConsoleFrame@@QAEHABVMfString@@@Z
int CConsoleFrame::OpenPobjFile(const MfString &fileName)
{
    MfScPiccoloObjectFile objectFile;
    MfScPiccoloObjectFile::MfScPOFErrorCode error = objectFile.Open(fileName, 0, 1, 0);
    if (error != MfScPiccoloObjectFile::kPOFOk) {
        MfString message;
        message.Format("Cannot open file:\n%s\n\nThe reason is\n%s", (char *)fileName,
                       (char *)MfScPiccoloObjectFile::ECToString(error));
        MessageBox(message, 0, 0);
        return 0;
    }
    MfScHandleList functions;
    objectFile.GrabFunctions(&functions);
    objectFile.ReadClose();
    MfString text;
    OutputPrint(1, text, 1);
    for (MfScDataHandle *function = functions.First(); function; function = functions.Next()) {
        MfScDataMethod *method = MethodOf(function);
        mGlobalPool->Replace(method->Name(), function, MfScHandleDict::kAddRef);
        text.Format("Storing function '%s'", (char *)method->Name());
        OutputPrint(1, text, 1);
    }
    mObjectMRU->Add(fileName);
    return 1;
}

// 0x10003230 ?OpenMRUPobj@CConsoleFrame@@QAEXH@Z
void CConsoleFrame::OpenMRUPobj(int index)
{
    if (!OpenPobjFile(MfString((*mObjectMRU)[index])))
        mObjectMRU->Remove(index);
}

// 0x100032c0 ?OnCreateClient@CConsoleFrame@@UAEHPAUtagCREATESTRUCTA@@PAUCCreateContext@@@Z
BOOL CConsoleFrame::OnCreateClient(LPCREATESTRUCT cs, CCreateContext *context)
{
    return CFrameWnd::OnCreateClient(cs, context);
}

// 0x100032e0 ?KbhKeyDown@CConsoleFrame@@MAEHHHPAVMfAiPiccoloEditor@@@Z
int CConsoleFrame::KbhKeyDown(int key, int flags, MfAiPiccoloEditor *)
{
    int handled = 0;
    switch (key) {
    case 'S':
        if (flags == 2) {
            ::PostMessage(m_hWnd, WM_COMMAND, ID_FILE_SAVE, 0);
            handled = 1;
        }
        break;
    case 'N':
        if (flags == 2) {
            ::PostMessage(m_hWnd, WM_COMMAND, ID_FILE_NEW, 0);
            handled = 1;
        }
        break;
    case 'O':
        if (flags == 2) {
            ::PostMessage(m_hWnd, WM_COMMAND, ID_FILE_OPEN, 0);
            handled = 1;
        }
        break;
    case VK_F4:
        if (!mExecWork->mBusy && flags == 0) {
            ::PostMessage(m_hWnd, WM_COMMAND, ID_PICCOLO_BUILD, 0);
            handled = 1;
        }
        break;
    case VK_F5:
        if (flags == 0 && CanRunPiccolo()) {
            ::PostMessage(m_hWnd, WM_COMMAND, ID_PICCOLO_RUN, 0);
            handled = 1;
        } else if (flags == 2 && mExecWork->mBusy) {
            ::PostMessage(m_hWnd, WM_COMMAND, ID_PICCOLO_STOP, 0);
            handled = 1;
        }
        break;
    }
    return handled;
}

// TODO 0x10003460 ?OnCreate@CConsoleFrame@@IAEHPAUtagCREATESTRUCTA@@@Z: 79% - register allocation; the original keeps the saved resource handle of the PiccoloConsoleContext in ebx for the inline destructor, this build loads it again from the object
int CConsoleFrame::OnCreate(LPCREATESTRUCT cs)
{
    if (CPiccoloFileEditor::OnCreate(cs) == -1)
        return -1;
    PiccoloConsoleContext context;
    if (!mToolBar.Create(this, 0x50002800, AFX_IDW_TOOLBAR) || !mToolBar.LoadToolBar(mResourceId) ||
        !mStatusBar.Create(this, 0x50008200, AFX_IDW_STATUS_BAR) ||
        !mStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
        return -1;
    mToolBar.SetBarStyle(mToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
    CConsoleEdit *editor = EditorWnd();
    editor->InstallKeyboardHandler(this);
    editor->InstallSetFocusHandler(this);
    CConsoleOutput *output = mPiccoloWnd->OutputWnd();
    CWnd *classTree = MfScDataObject::CreateClassTree(output, 0x800000);
    if (classTree)
        output->InstallUserWnd(MfString("Classes"), classTree);
    return 0;
}

// EQUIV 0x10003600 ?OnSize@CConsoleFrame@@IAEXIHH@Z: reviewed: the four terms of the window extent are summed in another order
void CConsoleFrame::OnSize(UINT type, int cx, int cy)
{
    CFrameWnd::OnSize(type, cx, cy);
    CRect toolBar;
    CRect statusBar;
    ::GetWindowRect(mToolBar.m_hWnd, &toolBar);
    ::GetClientRect(mStatusBar.m_hWnd, &statusBar);
    mPiccoloWnd->MoveWindow(0, toolBar.Height(), cx, cy - toolBar.Height() - statusBar.Height(), TRUE);
}

// 0x10003680 ?OnPiccoloCompile@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloCompile()
{
    if (FileName() == NULL_STRING) {
        MfString message;
        message << "The text must be saved as a file before compiling.\r\n" << "Do you wish to continue?";
        int answer = MessageBox(message, "Query", MB_YESNO | MB_ICONQUESTION);
        if (answer == IDYES)
            OnFileSaveAs();
        else if (answer == IDNO)
            return;
        if (FileName() == NULL_STRING)
            return;
    }
    if (mPiccoloWnd->EditorWnd()->IsTextModified())
        OnFileSave();
    int compiled = mPiccoloWnd->Compile(mWriteSeparateDebug, mCompileFlags);
    MfAiCompiler *compiler;
    bool store;
    {
        MfString name("PiccoloCompilerI");
        store = MfInterfaceRegistry::GetInterface(name, (MfAi **)&compiler) && mStoreAfterCompile && compiled &&
                compiler->CompiledOK();
    }
    if (store) {
        if (mResourceId == 0x80)
            OnPiccoloStore();
        else if (mResourceId == 0x82)
            OnPiccoloStoreGlobal();
        compiler->Release();
    }
}

// 0x10003860 ?OnUpdatePiccoloCompile@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloCompile(CCmdUI *cmdUI)
{
    int haveCompiler = 0;
    MfAi *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", &compiler)) {
        haveCompiler = 1;
        compiler->Release();
    }
    cmdUI->Enable(haveCompiler && !mStressTester && mPiccoloRunning && !mExecWork->mBusy);
}

// 0x10003920 ?OnRecompileProject@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnRecompileProject()
{
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        compiler->UpdatePiccoloDataPathFiles(1);
        compiler->Release();
    }
    MfScPiccolo::LoadPiccoloDataPathFunctions(1);
}

// 0x100039b0 ?OnUpdatePiccoloRecompileProject@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloRecompileProject(CCmdUI *cmdUI)
{
    MfAi *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", &compiler)) {
        cmdUI->Enable(TRUE);
        compiler->Release();
    } else {
        cmdUI->Enable(FALSE);
    }
}

// 0x10003a50 ?OnRecompileGlobal@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnRecompileGlobal()
{
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        compiler->UpdatePiccoloLibFiles(1);
        compiler->Release();
    }
    MfScPiccolo::LoadPiccoloLibFunctions(1);
}

// 0x10003ae0 ?OnUpdatePiccoloRecompileGlobal@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloRecompileGlobal(CCmdUI *cmdUI)
{
    MfAi *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", &compiler)) {
        cmdUI->Enable(TRUE);
        compiler->Release();
    } else {
        cmdUI->Enable(FALSE);
    }
}

// 0x10003b80 ?OnParentNotify@CConsoleFrame@@IAEXIJ@Z
void CConsoleFrame::OnParentNotify(UINT message, LPARAM lParam)
{
    CPiccoloFileEditor::OnParentNotify(message, lParam);
}

// 0x10003ba0 ?OnFileMruFile1@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruFile1()
{
    OpenMRUFile(0);
}

// 0x10003bb0 ?OnFileMruFile2@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruFile2()
{
    OpenMRUFile(1);
}

// 0x10003bc0 ?OnFileMruFile3@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruFile3()
{
    OpenMRUFile(2);
}

// 0x10003bd0 ?OnFileMruFile4@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruFile4()
{
    OpenMRUFile(3);
}

// 0x10003be0 ?OnFileMruFile5@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruFile5()
{
    OpenMRUFile(4);
}

// 0x10003bf0 ?OnFileMruFile6@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruFile6()
{
    OpenMRUFile(5);
}

// 0x10003c00 ?OnUpdateFileMruFile1@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateFileMruFile1(CCmdUI *cmdUI)
{
    if (mSourceMRU == NULL)
        cmdUI->Enable(FALSE);
    else
        mSourceMRU->UpdateMenu(cmdUI);
}

// 0x10003c30 ?OnFileMruPobj1@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruPobj1()
{
    OpenMRUPobj(0);
}

// 0x10003c40 ?OnFileMruPobj2@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruPobj2()
{
    OpenMRUPobj(1);
}

// 0x10003c50 ?OnFileMruPobj3@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruPobj3()
{
    OpenMRUPobj(2);
}

// 0x10003c60 ?OnFileMruPobj4@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruPobj4()
{
    OpenMRUPobj(3);
}

// 0x10003c70 ?OnFileMruPobj5@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruPobj5()
{
    OpenMRUPobj(4);
}

// 0x10003c80 ?OnFileMruPobj6@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileMruPobj6()
{
    OpenMRUPobj(5);
}

// 0x10003c90 ?OnUpdateFileMruPobj1@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateFileMruPobj1(CCmdUI *cmdUI)
{
    if (mObjectMRU == NULL)
        cmdUI->Enable(FALSE);
    else
        mObjectMRU->UpdateMenu(cmdUI);
}

// 0x10003cc0 ?OnFileOpen@CConsoleFrame@@IAEXXZ
// 0x10003df0 ??1CFileDialog@@UAE@XZ
void CConsoleFrame::OnFileOpen()
{
    if (mPiccoloWnd->EditorWnd()->IsTextModified()) {
        int answer = MessageBox("Text has been modified. Save changes?", "Query", MB_YESNOCANCEL | MB_ICONQUESTION);
        if (answer == IDYES)
            OnFileSave();
        else if (answer == IDCANCEL)
            return;
    }
    CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                       "Piccolo Files(*.pc)|*.pc|Piccolo Constant Sets(*.pcs)|*.pcs||", this);
    if (dialog.DoModal() == IDOK) {
        MfString fileName(dialog.GetPathName());
        OpenFile(fileName, 0);
    }
}

// 0x10003e40 ?OnOpenFromGlobal@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnOpenFromGlobal()
{
    char directory[270];
    GetCurrentDirectory(sizeof(directory), directory);
    MfString global;
    if (global.GetVariable("PICCOLOLIB")) {
        SetCurrentDirectory(global);
        OnFileOpen();
        SetCurrentDirectory(directory);
    } else {
        MessageBox("There was no global folder set in the PICCOLOLIB variable", "Error", MB_ICONERROR);
    }
}

// 0x10003f30 ?OnOpenFromProject@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnOpenFromProject()
{
    char directory[270];
    GetCurrentDirectory(sizeof(directory), directory);
    SetCurrentDirectory(MfScPiccolo::fsDataPath);
    OnFileOpen();
    SetCurrentDirectory(directory);
}

// 0x10003f80 ?OnFileNew@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileNew()
{
    CConsoleEdit *editor = mPiccoloWnd->EditorWnd();
    if (editor->IsTextModified()) {
        int answer = MessageBox("Text has been modified. Save changes?", "Query", MB_YESNOCANCEL | MB_ICONQUESTION);
        if (answer == IDYES)
            OnFileSave();
        else if (answer == IDCANCEL)
            return;
    }
    editor->SetEntireContents("", 0);
    SetFileName(NULL_STRING);
}

// 0x10004060 ?OnFileSave@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileSave()
{
    if (mFileName == NULL_STRING) {
        OnFileSaveAs();
        return;
    }
    if (!SaveFile(mFileName)) {
        MfString message = MfString("Cannot save the file ") + mFileName +
                           MfString("\r\nCheck to see if the file is read-only and that you have\r\n") +
                           MfString("permission to write the file.");
        MessageBox(message, "Error", MB_ICONERROR);
    }
}

// 0x100041b0 ?OnUpdateFileSave@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateFileSave(CCmdUI *cmdUI)
{
    cmdUI->Enable(mPiccoloWnd->EditorWnd()->IsTextModified() != 0);
}

// 0x100041e0 ?OnSaveToGlobal@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnSaveToGlobal()
{
    char directory[270];
    GetCurrentDirectory(sizeof(directory), directory);
    MfString global;
    if (global.GetVariable("PICCOLOLIB")) {
        SetCurrentDirectory(global);
        OnFileSaveAs();
        SetCurrentDirectory(directory);
    } else {
        MessageBox("There was no global folder set in the PICCOLOLIB variable", "Error", MB_ICONERROR);
    }
}

// 0x100042d0 ?OnSaveToProject@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnSaveToProject()
{
    char directory[270];
    GetCurrentDirectory(sizeof(directory), directory);
    SetCurrentDirectory(MfScPiccolo::fsDataPath);
    OnFileSaveAs();
    SetCurrentDirectory(directory);
}

// 0x10004320 ?OnUpdateSaveToGlobal@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateSaveToGlobal(CCmdUI *cmdUI)
{
    OnUpdateFileSave(cmdUI);
}

// 0x10004320 ?OnUpdateSaveToProject@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateSaveToProject(CCmdUI *cmdUI)
{
    OnUpdateFileSave(cmdUI);
}

// 0x10004330 ?OnFileSaveAs@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileSaveAs()
{
    CFileDialog dialog(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                       "Piccolo Files(*.pc)|*.pc|Piccolo Constant Sets(*.pcs)|*.pcs||", this);
    if (dialog.DoModal() == IDOK) {
        MfString fileName(dialog.GetPathName());
        MfString extension;
        fileName.SplitPath(0, 0, 0, &extension);
        if (extension == NULL_STRING) {
            if (dialog.m_ofn.nFilterIndex == 1)
                fileName << ".pc";
            else
                fileName << ".pcs";
        }
        if (!SaveFile(fileName)) {
            MfString message = MfString("Cannot save the file ") + mFileName +
                               MfString("\r\nCheck to see if the file is read-only and that you have\r\n") +
                               MfString("permission to write the file.");
            MessageBox(message, "Error", MB_ICONERROR);
        } else {
            mSourceMRU->Add(fileName);
        }
    }
}

// 0x100045a0 ?OnDiagnosticsLevel1@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnDiagnosticsLevel1()
{
    MfScDiagnostics::iLevel = 1;
}

// 0x100045b0 ?OnUpdateDiagnosticsLevel1@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateDiagnosticsLevel1(CCmdUI *cmdUI)
{
    cmdUI->SetCheck(MfScDiagnostics::iLevel == 1);
}

// 0x100045d0 ?OnDiagnosticsLevel2@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnDiagnosticsLevel2()
{
    MfScDiagnostics::iLevel = 2;
}

// 0x100045e0 ?OnUpdateDiagnosticsLevel2@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateDiagnosticsLevel2(CCmdUI *cmdUI)
{
    cmdUI->SetCheck(MfScDiagnostics::iLevel == 2);
}

// 0x10004600 ?OnDiagnosticsLevel3@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnDiagnosticsLevel3()
{
    MfScDiagnostics::iLevel = 3;
}

// 0x10004610 ?OnUpdateDiagnosticsLevel3@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateDiagnosticsLevel3(CCmdUI *cmdUI)
{
    cmdUI->SetCheck(MfScDiagnostics::iLevel == 3);
}

// 0x10004630 ?OnDiagnosticsLevel4@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnDiagnosticsLevel4()
{
    MfScDiagnostics::iLevel = 4;
}

// 0x10004640 ?OnUpdateDiagnosticsLevel4@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateDiagnosticsLevel4(CCmdUI *cmdUI)
{
    cmdUI->SetCheck(MfScDiagnostics::iLevel == 4);
}

// 0x10004660 ?OnDiagnosticsNoLevel@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnDiagnosticsNoLevel()
{
    MfScDiagnostics::iLevel = 0;
}

// 0x10004670 ?OnUpdateDiagnosticsNoLevel@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateDiagnosticsNoLevel(CCmdUI *cmdUI)
{
    cmdUI->SetCheck(MfScDiagnostics::iLevel == 0);
}

// 0x10004690 ?OnPiccoloRun@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloRun()
{
    // the program just compiled, else the stored main
    MfScDataHandle *function = 0;
    MfAiCompiler *compiler = 0;
    MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler);
    if (compiler && compiler->CodeType() == 1 && compiler->Functions()->GetCount())
        function = compiler->Functions()->First();
    else if (mGlobalPool)
        function = mGlobalPool->Find("main");
    if (compiler)
        compiler->Release();

    if (!mExecWork->mBusy && function) {
        if (mAutoSwitchToOutput)
            OutputSwitchToWindow(0);
        OutputClear(0);
        mExecWork->SetWork(function, mVariables, mParameters);
        mExecWork->SetLoop(0);
        mThread->AssignWork(mExecWork);
    }
}

// 0x10004820 ?CanRunPiccolo@CConsoleFrame@@IAEHXZ
int CConsoleFrame::CanRunPiccolo()
{
    MfScDataHandle *mainFunction = 0;
    if (mGlobalPool)
        mainFunction = mGlobalPool->Find("main");
    int codeType = 0;
    int functions = 0;
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        codeType = compiler->CodeType();
        functions = compiler->Functions()->GetCount();
        compiler->Release();
    }
    if (!mExecWork->mBusy && (mainFunction || (codeType == 1 && functions > 0)))
        return 1;
    return 0;
}

// 0x10004940 ?OnUpdatePiccoloRun@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloRun(CCmdUI *cmdUI)
{
    cmdUI->Enable(!mStressTester && mPiccoloRunning && CanRunPiccolo());
}

// 0x10004980 ?OnPiccoloStressTest@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloStressTest()
{
    MfScDataHandle *function = 0;
    MfAiCompiler *compiler = 0;
    MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler);
    if (compiler && compiler->CodeType() == 1 && compiler->Functions()->GetCount())
        function = compiler->Functions()->First();
    else if (mGlobalPool)
        function = mGlobalPool->Find("main");
    if (compiler)
        compiler->Release();

    if (!mExecWork->mBusy && function) {
        if (mAutoSwitchToOutput)
            OutputSwitchToWindow(0);
        OutputClear(0);
        mExecWork->SetWork(function, mVariables, mParameters);
        mExecWork->SetLoop(1);
        mThread->AssignWork(mExecWork);
    }
}

// 0x10004b20 ?OnUpdatePiccoloStressTest@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloStressTest(CCmdUI *cmdUI)
{
    MfScDataHandle *mainFunction = 0;
    if (mGlobalPool)
        mainFunction = mGlobalPool->Find("main");
    int functions = 0;
    int codeType = 0;
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        codeType = compiler->CodeType();
        functions = compiler->Functions()->GetCount();
        compiler->Release();
    }
    cmdUI->Enable(!mStressTester && mPiccoloRunning && !mExecWork->mBusy &&
                  (mainFunction || (codeType == 1 && functions > 0)));
}

// EQUIV 0x10004c50 ?OnPiccoloStore@CConsoleFrame@@IAEXXZ: reviewed: register choice only
void CConsoleFrame::OnPiccoloStore()
{
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        MfString text;
        OutputPrint(1, text, 1);
        for (MfScDataHandle *function = compiler->Functions()->First(); function;
             function = compiler->Functions()->Next()) {
            MfScDataMethod *method = MethodOf(function);
            mGlobalPool->Replace(method->Name(), function, MfScHandleDict::kAddRef);
            text.Format("Storing function '%s'", (const char *)method->Name());
            OutputPrint(1, text, 1);
        }
        if (mClearAfterStore)
            compiler->Functions()->Clear();
        compiler->Release();
    }
}

// 0x10004de0 ?OnUpdatePiccoloStore@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloStore(CCmdUI *cmdUI)
{
    int functions = 0;
    int codeType = 0;
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        codeType = compiler->CodeType();
        functions = compiler->Functions()->GetCount();
        compiler->Release();
    }
    cmdUI->Enable(!mStressTester && mPiccoloRunning && !mExecWork->mBusy && codeType == 2 && functions > 0 &&
                  mGlobalPool);
}

// EQUIV 0x10004ec0 ?OnPiccoloStoreGlobal@CConsoleFrame@@IAEXXZ: reviewed: register choice only
void CConsoleFrame::OnPiccoloStoreGlobal()
{
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        MfString text;
        OutputPrint(1, text, 1);
        for (MfScDataHandle *function = compiler->Functions()->First(); function;
             function = compiler->Functions()->Next()) {
            MfScDataMethod *method = MethodOf(function);
            MfScVmEnv::SetGlobalVariable(method->Name(), function, 0, 0);
            text.Format("Storing function '%s'", (const char *)method->Name());
            OutputPrint(1, text, 1);
        }
        if (mClearAfterStore)
            compiler->Functions()->Clear();
        compiler->Release();
    }
}

// 0x10005050 ?OnUpdatePiccoloStoreGlobal@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloStoreGlobal(CCmdUI *cmdUI)
{
    int functions = 0;
    int codeType = 0;
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        codeType = compiler->CodeType();
        functions = compiler->Functions()->GetCount();
        compiler->Release();
    }
    cmdUI->Enable(!mStressTester && mPiccoloRunning && !mExecWork->mBusy && codeType == 2 && functions > 0);
}

// 0x10005130 ?OnPiccoloRelease@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloRelease()
{
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        compiler->Functions()->Clear();
        compiler->ClearCodeType();
        if (!mExecWork->mBusy)
            mExecWork->SetWork(0, 0, NULL_STRING);
        compiler->Release();
    }
}

// 0x10005240 ?OnUpdatePiccoloRelease@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloRelease(CCmdUI *cmdUI)
{
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        cmdUI->Enable(!mStressTester && mPiccoloRunning && !mExecWork->mBusy &&
                      (compiler->Functions()->GetCount() > 0 || mExecWork->mFunction));
        compiler->Release();
    } else {
        cmdUI->Enable(FALSE);
    }
}

// 0x10005330 ?OnPiccoloDumppool@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloDumppool()
{
    MfScDataHandle::DumpPool();
}

// 0x10005340 ?OnPiccoloClearvars@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloClearvars()
{
    if (mGlobalPool) {
        mGlobalPool->Clear();
        mSecondPool->Clear();
        mThirdPool->Clear();
        SetupVariables();
    }
}

// 0x10005370 ?OnPiccoloClearDiag@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloClearDiag()
{
    OutputClear(2);
}

// 0x10005380 ?OnUpdatePiccoloClearDiag@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloClearDiag(CCmdUI *cmdUI)
{
    cmdUI->Enable(!mStressTester && mPiccoloRunning && !mExecWork->mBusy);
}

// 0x100053c0 ?OnPiccoloClearOutput@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloClearOutput()
{
    OutputClear(0);
}

// 0x10005380 ?OnUpdatePiccoloClearOutput@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloClearOutput(CCmdUI *cmdUI)
{
    cmdUI->Enable(!mStressTester && mPiccoloRunning && !mExecWork->mBusy);
}

// 0x100053d0 ?OnPiccoloStop@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloStop()
{
    MfExecWork *work = mExecWork;
    if (work->mBusy) {
        work->mStop = 1;
        work->mContext->mAbort = 1;
    }
}

// 0x100053f0 ?OnUpdatePiccoloStop@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloStop(CCmdUI *cmdUI)
{
    cmdUI->Enable(!mStressTester && mPiccoloRunning && mExecWork->mBusy);
}

// 0x10005430 ?OnClose@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnClose()
{
    CConsoleEdit *editor = mPiccoloWnd->EditorWnd();
    if ((!mExecWork || !mExecWork->mBusy) && editor->IsTextModified()) {
        int answer = MessageBox("Text has been modified. Save changes?", "Query", MB_YESNOCANCEL | MB_ICONQUESTION);
        if (answer == IDYES) {
            OnFileSave();
        } else if (answer == IDNO) {
            mTextModified = 0;
            editor->SetUnModified();
        } else {
            return;
        }
    }
    DoClose();
}

// 0x100054b0 ?DoClose@CConsoleFrame@@QAEXXZ
void CConsoleFrame::DoClose()
{
    MfScDiagnostics::iLevel = 0;
    MfExecWork *work = mExecWork;
    if (work && work->mBusy) {
        work->mStop = 1;
        work->mContext->mAbort = 1;
        TerminateThread(mThread->mHandle, 0);
    }
    if (mStressTester) {
        mStressTester->mStop = 1;
        mStressTester->SignalShutdown(0);
        mStressTester = 0;
        Sleep(500);
    }
    mGlobalPool->Clear();
    if (mPiccoloRunning) {
        MfAiCompiler *compiler;
        if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
            compiler->Functions()->Clear();
            compiler->Release();
        }
    }

    WINDOWPLACEMENT placement;
    GetWindowPlacement(&placement);
    CRect rect(placement.rcNormalPosition);
    UtilityApp app;
    app.WriteProfileInt(szOptions, szX, rect.left);
    app.WriteProfileInt(szOptions, szY, rect.top);
    app.WriteProfileInt(szOptions, szCx, rect.right - rect.left);
    app.WriteProfileInt(szOptions, szCy, rect.bottom - rect.top);
    app.WriteProfileInt(szOptions, szPaneSize, mPiccoloWnd->GetPaneSize());
    CPiccoloFileEditor::OnClose();
}

// 0x10005790 ?OnViewOptions@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnViewOptions()
{
    PiccoloConsoleContext context;
    CConsoleOptions dialog(mClearAfterStore, mStoreAfterCompile, mAutoSwitchToOutput, mWriteSeparateDebug,
                           mPwTitleBar, mPwToolBar, this);
    AfxGetMainWnd()->PostMessage(0x118);    // WM_SYSTIMER
    if (dialog.DoModal() == IDOK) {
        mClearAfterStore = dialog.mClearAfterStore;
        mStoreAfterCompile = dialog.mStoreAfterCompile;
        mAutoSwitchToOutput = dialog.mAutoSwitchToOutput;
        mWriteSeparateDebug = dialog.mWriteSeparateDebug;
        mPwTitleBar = dialog.mPwTitleBar;
        mPwToolBar = dialog.mPwToolBar;
        mPiccoloWnd->SetFlags(PiccoloWindowFlags());
    }
}

// 0x10005900 ?OnFileClose@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileClose()
{
    ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
}

// EQUIV 0x10005920 ?OnFileSaveObjectFile@CConsoleFrame@@IAEXXZ: reviewed: register choice and the order of an independent load and address computation
void CConsoleFrame::OnFileSaveObjectFile()
{
    CFileDialog dialog(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                       "Piccolo Files(*.pc)|*.pc|Piccolo Constant Sets(*.pcs)|*.pcs||", this);
    if (dialog.DoModal() == IDOK) {
        MfAiCompiler *compiler;
        if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
            MfString fileName(dialog.GetPathName());
            MfScPiccoloObjectFile objectFile(fileName, 1, 1, 0);
            if (compiler->CodeType() == 1) {
                // a program: its function is main
                MfScDataHandle *function = compiler->Functions()->First();
                MethodOf(function)->Name() = "main";
                objectFile.AddFunction(function);
            } else {
                for (MfScDataHandle *function = compiler->Functions()->First(); function;
                     function = compiler->Functions()->Next())
                    objectFile.AddFunction(function);
            }
            objectFile.WriteClose();
            mObjectMRU->Add(fileName);
            compiler->Release();
        }
    }
}

// 0x10005b60 ?OnUpdateFileSaveObjectFile@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateFileSaveObjectFile(CCmdUI *cmdUI)
{
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        cmdUI->Enable(!mStressTester && mPiccoloRunning && compiler->Functions()->GetCount() > 0);
        compiler->Release();
    }
}

// 0x10005c10 ?OnFileLoadObjectFile@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileLoadObjectFile()
{
    CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                       "Piccolo Object Files(*.pobj)|*.pobj|Piccolo Executable Files(*.pexe)|*.pexe||", this);
    if (dialog.DoModal() == IDOK) {
        OnPiccoloRelease();
        MfString fileName(dialog.GetPathName());
        OpenPobjFile(fileName);
    }
}

// 0x10005d00 ?OnViewPwTitleBar@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnViewPwTitleBar()
{
    mPwTitleBar = !mPwTitleBar;
    mPiccoloWnd->SetFlags(PiccoloWindowFlags());
}

// 0x10005d40 ?OnUpdateViewPwTitleBar@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateViewPwTitleBar(CCmdUI *cmdUI)
{
    cmdUI->SetCheck(mPwTitleBar);
}

// 0x10005d60 ?OnViewPwToolBar@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnViewPwToolBar()
{
    mPwToolBar = !mPwToolBar;
    mPiccoloWnd->SetFlags(PiccoloWindowFlags());
}

// 0x10005da0 ?OnUpdateViewPwToolBar@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateViewPwToolBar(CCmdUI *cmdUI)
{
    cmdUI->SetCheck(mPwToolBar);
}

// 0x10005dc0 ?OnHandleWorkFinished@CConsoleFrame@@IAEJIJ@Z
LRESULT CConsoleFrame::OnHandleWorkFinished(WPARAM, LPARAM)
{
    return 0;
}

// 0x10005dd0 ?OnNewPiccoloWnd@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnNewPiccoloWnd()
{
    CPiccoloFileEditor *editor = new CPiccoloFileEditor(mFileName, "Piccolo Editor");
    editor->DisableConsole();
    editor->SetEditorFlagHints(3);
    editor->CreateFrameWindow(this);
}

// 0x10005e90 ?OnNewConsoleWnd@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnNewConsoleWnd()
{
    new MfPiccoloConsoleFrame(this, 0);
}

// 0x10005ee0 ?OnTimer@CConsoleFrame@@IAEXI@Z
void CConsoleFrame::OnTimer(UINT id)
{
    CPiccoloFileEditor::OnTimer(id);
}

// 0x10005ef0 ?OnSetFocus@CConsoleFrame@@IAEXPAVCWnd@@@Z
void CConsoleFrame::OnSetFocus(CWnd *oldWnd)
{
    CPiccoloFileEditor::OnSetFocus(oldWnd);
    EditorWnd()->SetFocus();
}

// 0x10005f10 ?ReceivedFocus@CConsoleFrame@@MAEXPAVMfPeCodeEditor@@@Z
void CConsoleFrame::ReceivedFocus(MfPeCodeEditor *)
{
    ::PostMessage(m_hWnd, WM_EDITOR_HAS_FOCUS, 0, 0);
}

// 0x10005f30 ?OnHandleEditorHasFocus@CConsoleFrame@@IAEJIJ@Z
LRESULT CConsoleFrame::OnHandleEditorHasFocus(WPARAM, LPARAM)
{
    if (!mCheckingFileTime) {
        mCheckingFileTime = 1;
        MfFileTime fileTime;
        fileTime.SetFromFile(mFileName);
        if (CompareFileTime(&fileTime.mTime, &mFileTime.mTime) > 0) {
            MfString message;
            message = MfString("The file ") + mFileName + MfString("\r\nhas changed on disk. Do you wish to reload it?");
            if (MessageBox(message, "File Changed", MB_YESNO | MB_ICONQUESTION) == IDYES)
                OpenFile(mFileName, 1);
            else
                mFileTime.SetFromFile(mFileName);
        }
        mCheckingFileTime = 0;
    }
    return 0;
}

// 0x100060a0 ?LostFocus@CConsoleFrame@@MAEXPAVMfPeCodeEditor@@@Z
void CConsoleFrame::LostFocus(MfPeCodeEditor *)
{
}

// 0x100060a0 ?OnUpdatePiccoloDumppool@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloDumppool(CCmdUI *)
{
}

// 0x100060b0 ?OnViewPiccoloWindow@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnViewPiccoloWindow()
{
    mShowPiccoloWnd = !mShowPiccoloWnd;
    if (mShowPiccoloWnd)
        mPiccoloWnd->ShowWindow(SW_SHOW);
    else
        mPiccoloWnd->ShowWindow(SW_HIDE);
}

// 0x100060f0 ?OnUpdateViewPiccoloWindow@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateViewPiccoloWindow(CCmdUI *cmdUI)
{
    cmdUI->SetCheck(mShowPiccoloWnd != 0);
}

// 0x10006110 ?OnEnterMessageLoop@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnEnterMessageLoop()
{
    MfScThreadContext *context = ((MfScThread *)MfThread::GetThread())->mContext;
    if (context->mMessageLoop) {
        mInMessageLoop = 1;
        while (mInMessageLoop) {
            MSG msg;
            while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
                if (!AfxGetApp()->PumpMessage()) {
                    ::PostQuitMessage(0);
                    return;
                }
            }
            LONG idleCount = 0;
            while (AfxGetApp()->OnIdle(idleCount++))
                ;
        }
    }
}

// 0x100061b0 ?OnBuildParameters@CConsoleFrame@@IAEXXZ
// 0x10006290 ??1MfPiccoloParametersDlg@@UAE@XZ
void CConsoleFrame::OnBuildParameters()
{
    PiccoloConsoleContext context;
    MfPiccoloParametersDlg dialog(this);
    dialog.mParameters = (const char *)mParameters;
    if (dialog.DoModal() == IDOK)
        mParameters = dialog.mParameters;
}

// 0x100062e0 ?OnUpdateEnterMessageLoop@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateEnterMessageLoop(CCmdUI *cmdUI)
{
    cmdUI->Enable(!mStressTester && !mInMessageLoop);
}

// 0x10006320 ?OnExitMessageLoop@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnExitMessageLoop()
{
    mInMessageLoop = 0;
}

// 0x10006330 ?OnUpdateExitMessageLoop@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateExitMessageLoop(CCmdUI *cmdUI)
{
    cmdUI->Enable(!mStressTester && mInMessageLoop);
}

// 0x10006370 ?Initialize@CConsoleOutput@@SAXXZ (folded: ?OnBufferEmpty@CConsoleFrame@@IAEXXZ)
void CConsoleFrame::OnBufferEmpty()
{
}

// 0x10006380 ?OnUpdateBufferEmpty@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateBufferEmpty(CCmdUI *cmdUI)
{
    cmdUI->SetCheck(mBufferModified != 0);
}

// 0x100063a0 ?OnNotify@CConsoleFrame@@MAEHIJPAJ@Z
BOOL CConsoleFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *result)
{
    return CPiccoloFileEditor::OnNotify(wParam, lParam, result);
}

// 0x100063c0 ?OnEditCopy@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnEditCopy()
{
    mPiccoloWnd->EditorWnd()->OnEditCopy();
}

// 0x100063e0 ?OnUpdateEditCopy@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateEditCopy(CCmdUI *cmdUI)
{
    mPiccoloWnd->EditorWnd()->OnUpdateEditCopy(cmdUI);
}

// 0x10006400 ?OnEditCut@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnEditCut()
{
    mPiccoloWnd->EditorWnd()->OnEditCut();
}

// 0x10006420 ?OnUpdateEditCut@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateEditCut(CCmdUI *cmdUI)
{
    mPiccoloWnd->EditorWnd()->OnUpdateEditCut(cmdUI);
}

// 0x10006440 ?OnEditPaste@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnEditPaste()
{
    mPiccoloWnd->EditorWnd()->OnEditPaste();
}

// 0x100063e0 ?OnUpdateEditPaste@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateEditPaste(CCmdUI *cmdUI)
{
    mPiccoloWnd->EditorWnd()->OnUpdateEditCopy(cmdUI);
}

// 0x10006460 ?OnEditUndo@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnEditUndo()
{
    mPiccoloWnd->EditorWnd()->OnEditUndo();
}

// 0x10006480 ?OnUpdateEditUndo@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateEditUndo(CCmdUI *cmdUI)
{
    mPiccoloWnd->EditorWnd()->OnUpdateEditUndo(cmdUI);
}

// 0x100064a0 ?OnEditRedo@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnEditRedo()
{
    mPiccoloWnd->EditorWnd()->OnEditRedo();
}

// 0x100064c0 ?OnUpdateEditRedo@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateEditRedo(CCmdUI *cmdUI)
{
    mPiccoloWnd->EditorWnd()->OnUpdateEditRedo(cmdUI);
}

// 0x100064e0 ?OnFileShutdown@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileShutdown()
{
    mPiccoloRunning = 0;
    mGlobalPool->Clear();
    mExecWork->SetWork(0, 0, NULL_STRING);
    if (mExecWork) {
        mThread->SignalShutdown(1);
        delete mExecWork;
        mExecWork = 0;
    }
    CConsoleOutput::Initialize();
    CConsoleFrame::Shutdown();
    MfString errors;
    MfScPiccolo::Shutdown(&errors);
    MfLogger::Shutdown();
}

// 0x100065f0 ?OnUpdateFileShutdown@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateFileShutdown(CCmdUI *cmdUI)
{
    cmdUI->Enable(!mStressTester && mPiccoloRunning);
}

// 0x10006630 ?OnFileStartup@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileStartup()
{
    mPiccoloRunning = 1;
    MfScPiccolo::Initialize();
    MfScPiccolo::SetAsMessageThread();
    CConsoleOutput::Initialize();
    CConsoleFrame::Initialize();
    MfString errors;
    MfScPiccolo::InitializePlugins(&errors);
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        compiler->UpdatePiccoloDataPathFiles(0);
        compiler->UpdatePiccoloLibFiles(0);
        compiler->Release();
    }
    MfScPiccolo::LoadPiccoloDataPathFunctions(1);
    MfScPiccolo::LoadPiccoloLibFunctions(1);
    mExecWork = new MfExecWork(this);
    mThread = new MfScThread(2);
    mPiccoloWnd->OutputWnd()->SetupOutput();
}

// 0x100067c0 ?OnUpdateFileStartup@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdateFileStartup(CCmdUI *cmdUI)
{
    cmdUI->Enable(!mStressTester && !mPiccoloRunning);
}

// 0x10006800 ?OnPiccoloCompileStressTest@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloCompileStressTest()
{
    OnPiccoloRelease();
    mStressTester = new CCompilerStressTester;
    mStressTester->mSource = mPiccoloWnd->EditorWnd()->GetEntireContents();
    mStressTester->AssignWork(mStressTester);
}

// 0x100068f0 ?DoWork@CCompilerStressTester@@UAEXXZ
void CCompilerStressTester::DoWork()
{
    while (!mStop) {
        MfAiCompiler *compiler;
        if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
            compiler->Compile(NULL_STRING, "FILE", mSource, 0, 0);
            Sleep(0);
            Sleep(0);
            Sleep(0);
            compiler->Functions()->Clear();
            compiler->ClearCodeType();
            compiler->Release();
        }
    }
}

// 0x10006a20 ??_GCCompilerStressTester@@UAEPAXI@Z
// 0x10006a40 ??1CCompilerStressTester@@UAE@XZ

// 0x10006aa0 ?OnUpdatePiccoloCompileStressTest@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloCompileStressTest(CCmdUI *cmdUI)
{
    int haveCompiler = 0;
    MfAi *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", &compiler)) {
        haveCompiler = 1;
        compiler->Release();
    }
    cmdUI->Enable(haveCompiler && !mStressTester);
}

// 0x10006b40 ?OnPiccoloCompileEndStressTest@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloCompileEndStressTest()
{
    mStressTester->mStop = 1;
    mStressTester->SignalShutdown(0);
    mStressTester = 0;
}

// 0x10006b70 ?OnUpdatePiccoloCompileEndStressTest@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloCompileEndStressTest(CCmdUI *cmdUI)
{
    cmdUI->Enable(mStressTester != 0);
}

// 0x10006b90 ?OnFlushPiccoloFunctions@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFlushPiccoloFunctions()
{
    MfScPiccolo::FlushPiccoloDataPathFunctions();
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        compiler->FlushGlobalTemps();
        compiler->Release();
    }
}

// 0x10006c20 ?OnLoadPiccoloFunctions@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnLoadPiccoloFunctions()
{
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        compiler->UpdatePiccoloDataPathFiles(0);
        compiler->Release();
    }
    MfScPiccolo::LoadPiccoloDataPathFunctions(1);
}

// 0x10006cb0 ?EnumHandle@CConsoleFrame@@MAEHPAVMfScDataHandle@@@Z
int CConsoleFrame::EnumHandle(MfScDataHandle *handle)
{
    // the memory of the bytecode functions
    if (handle->IsType(0x800) && ((MfScDataMethod *)handle->GetData())->m_type == 0) {
        MfScBytecodeRoutine *routine = (MfScBytecodeRoutine *)handle->GetData();
        MfString text;
        text << "Function '" << routine->Name() << "' in '" << routine->ModuleName() << "'";
        OutputPrint(2, text, 1);
        unsigned int names = routine->MemoryUsage(1);
        unsigned int code = routine->MemoryUsage(2);
        unsigned int constants = routine->MemoryUsage(4);
        unsigned int debug = routine->MemoryUsage(8);
        text.Format("            Names Size: %5d", names);
        OutputPrint(2, text, 1);
        text.Format("             Code Size: %5d", code);
        OutputPrint(2, text, 1);
        text.Format("    Constant Pool Size: %5d", constants);
        OutputPrint(2, text, 1);
        text.Format("            Debug Size: %5d", debug);
        OutputPrint(2, text, 1);
        text.Format("-----------------------------");
        OutputPrint(2, text, 1);
        text.Format("             Full Size: %5d", names + code + constants + debug);
        OutputPrint(2, text, 1);
        OutputPrint(2, "", 1);
    }
    return 1;
}

// 0x10006ed0 ?OnPiccoloDumpMemusage@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloDumpMemusage()
{
    OutputSwitchToWindow(2);
    OutputPrint(2, "Function Memory Usage", 1);
    OutputPrint(2, "============================================", 1);
    MfScDataHandle::EnumHandles(1, *this);
}

// 0x10006f90 ?OnFileStripObjectFile@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnFileStripObjectFile()
{
    CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, "Piccolo Object Files(*.pobj)|*.pobj||", this);
    if (dialog.DoModal() == IDOK) {
        MfString fileName(dialog.GetPathName());
        MfScPiccoloObjectFile objectFile;
        MfScPiccoloObjectFile::MfScPOFErrorCode error = objectFile.Open(fileName, 0, 1, 0);
        if (error != MfScPiccoloObjectFile::kPOFOk) {
            MfString message;
            message.Format("Cannot open file:\n%s\n\nThe reason is\n%s", (char *)fileName,
                           (char *)MfScPiccoloObjectFile::ECToString(error));
            MessageBox(message, 0, 0);
        }
        // write the functions again, without the debug information
        MfScHandleList functions;
        objectFile.GrabFunctions(&functions);
        objectFile.ReadClose();
        objectFile.Open(fileName, 1, 1, (mWriteSeparateDebug != 0) + 1);
        for (MfScDataHandle *function = functions.First(); function; function = functions.Next())
            objectFile.AddFunction(function);
        objectFile.WriteClose();
    }
}

// EQUIV 0x10007210 ?OnFileDumpObjectFile@CConsoleFrame@@IAEXXZ: reviewed: stack slot of the this copy only
void CConsoleFrame::OnFileDumpObjectFile()
{
    CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                       "Piccolo Object Files(*.pobj)|*.pobj|Piccolo Debug Files(*.pdbg)|*.pdbg|"
                       "Piccolo Executable Files(*.pexe)|*.pexe||",
                       this);
    if (dialog.DoModal() == IDOK) {
        MfString fileName(dialog.GetPathName());
        MfScPiccoloObjectFile objectFile;
        MfScPiccoloObjectFile::MfScPOFErrorCode error = objectFile.Open(fileName, 0, 1, 0);
        if (error != MfScPiccoloObjectFile::kPOFOk) {
            MfString message;
            message.Format("Cannot open file:\n%s\n\nThe reason is\n%s", (char *)fileName,
                           (char *)MfScPiccoloObjectFile::ECToString(error));
            MessageBox(message, 0, 0);
        }
        OutputSwitchToWindow(0);
        OutputPrint(0, fileName, 1);
        MfStringList lines;
        objectFile.DumpFile(&lines, 0xf);
        MfDblListItem *item;
        for (MfString *line = (MfString *)lines.First(item); line; line = (MfString *)MfDblList::Next(item))
            OutputPrint(0, *line, 1);
        OutputPrint(0, "", 1);
        objectFile.ReadClose();
    }
}

// 0x100074f0 ?OnLockGc@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnLockGc()
{
    MfScDataHandle::LockGC();
}

// 0x10007500 ?OnUnlockGc@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnUnlockGc()
{
    MfScDataHandle::UnlockGC();
}

// What the console searched for last.
// 0x10007510 _$E247
// 0x10007520 _$E244
// 0x10007530 _$E246
// 0x10007540 _$E245
static MfString findString;
// 0x10007550 _$E250
// 0x10007560 _$E249
static MfPeSearchContext findContext;

// 0x10007570 ?OnEditFindFirst@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnEditFindFirst()
{
    MfNameDialog dialog("Enter the string", "String", NULL_STRING, 0);
    dialog.DoModal();
    findString = dialog.GetName();
    if (!mPiccoloWnd->EditorWnd()->FindFirst(findString, &findContext, 0))
        MessageBox("Cannot find string", 0, 0);
}

// 0x10007690 ?OnEditFindNext@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnEditFindNext()
{
    if (!mPiccoloWnd->EditorWnd()->FindNext(findString, &findContext, 0))
        MessageBox("Cannot find string", 0, 0);
}

// 0x100076d0 ?OnPiccoloDefault@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloDefault()
{
    mCompileFlags = 0;
}

// 0x100076e0 ?OnUpdatePiccoloDefault@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloDefault(CCmdUI *cmdUI)
{
    cmdUI->SetCheck((mCompileFlags & 0xc0) == 0);
}

// 0x10007700 ?OnPiccoloNeedFunctions@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloNeedFunctions()
{
    mCompileFlags = 0x80;
}

// 0x10007710 ?OnUpdatePiccoloNeedFunctions@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloNeedFunctions(CCmdUI *cmdUI)
{
    cmdUI->SetCheck((mCompileFlags & 0x80) != 0);
}

// 0x10007730 ?OnPiccoloNoFunctions@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnPiccoloNoFunctions()
{
    mCompileFlags = 0x40;
}

// 0x10007740 ?OnUpdatePiccoloNoFunctions@CConsoleFrame@@IAEXPAVCCmdUI@@@Z
void CConsoleFrame::OnUpdatePiccoloNoFunctions(CCmdUI *cmdUI)
{
    cmdUI->SetCheck((mCompileFlags & 0x40) != 0);
}

// 0x10007760 ?OnViewAdvancedMenu@CConsoleFrame@@IAEXXZ
// 0x10007860 ??_GCObject@@UAEPAXI@Z
// 0x10007880 ??1CObject@@UAE@XZ
// 0x10007890 ??_GCMenu@@UAEPAXI@Z
// 0x100078b0 ??1CMenu@@UAE@XZ
void CConsoleFrame::OnViewAdvancedMenu()
{
    PiccoloConsoleContext context;
    CMenu menu;
    menu.LoadMenu(0x80);
    CMenu *oldMenu = GetMenu();
    SetMenu(&menu);
    if (oldMenu->m_hMenu != m_hMenuDefault)
        oldMenu->DestroyMenu();
}

// 0x10007900 ?OnViewBasicMenu@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnViewBasicMenu()
{
    PiccoloConsoleContext context;
    CMenu menu;
    menu.LoadMenu(0x82);
    CMenu *oldMenu = GetMenu();
    SetMenu(&menu);
    if (oldMenu->m_hMenu != m_hMenuDefault)
        oldMenu->DestroyMenu();
}

// The handler of ID_VIEW_PICCOLO_CLASSES is the same code as CPiccoloWnd's:
// 0x1000b3c0 ?OnViewPiccoloClasses@CConsoleFrame@@IAEXXZ
void CConsoleFrame::OnViewPiccoloClasses()
{
    MfScDataObject::DisplayClassInfo(this);
}

// The functions of the frame that read and write the file do nothing; the
// console reads and writes its file itself (same code as the empty inline
// functions of MFC):
// 0x100094d0 ?ReadCurrentFile@CConsoleFrame@@MAEXPAVMfString@@@Z
void CConsoleFrame::ReadCurrentFile(MfString *)
{
}

// 0x100094d0 ?WriteCurrentFile@CConsoleFrame@@MAEXPAVMfString@@@Z
void CConsoleFrame::WriteCurrentFile(MfString *)
{
}

// 0x10007a00 ??_ECCompilerStressTester@@WCA@AEPAXI@Z
// 0x10007a10 ??1CConsoleOptions@@UAE@XZ
