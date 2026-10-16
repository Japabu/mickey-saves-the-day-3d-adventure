// PiccoloConsole.dll: the Piccolo console and code editor (an MFC extension
// DLL, MFC42.DLL by ordinal). Declarations of the classes of this DLL and of
// what it uses from mfutil.dll and PiccoloRT.dll; names from the exports and
// the RTTI, layouts from the code.
#ifndef PICCOLOCONSOLE_H
#define PICCOLOCONSOLE_H

#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include <afxpriv.h>
#include <afxadv.h>

// ---- mfutil.dll -------------------------------------------------------------

class MfString;
class MfDblListItem;
class MfStringList;
class MfStringCursor;
class MfStringScanner;
class MfStringTokenizerCallBack;

// The initial size of an empty string.
class MfStringSize {
public:
    MfStringSize(int n) : size(n) {}
    int size;
};

// Reference-counted string body (0x14 bytes); the constructors and the
// destructor are inline, the plugin keeps its own copy of the vtable.
class __declspec(dllimport) MfStringRef {
public:
    MfStringRef(int size) : nRefs(0)
    {
        nAlloc = size + nNumPadChars;
        pData = new char[nAlloc + 1];
        pData[0] = 0;
        nLength = 0;
    }
    MfStringRef(const MfStringRef &other) : pData(0), nLength(0), nAlloc(0), nRefs(0)
    {
        MakeCopy(other);
    }
    virtual ~MfStringRef() { delete[] pData; }
    void MakeCopy(const MfStringRef &other);
    MfStringRef *CopyOnWrite() { return new MfStringRef(*this); }
    void Truncate(int length);
    void ConvertToLower();
    void NoLeadingWhiteSpace();
    void NoTrailingWhiteSpace();
    int ReadFromFile(const MfString &fileName);
    int WriteToFile(const MfString &fileName) const;

    char *pData;        // +0x04
    int nLength;        // +0x08
    int nAlloc;         // +0x0c
    int nRefs;          // +0x10
protected:
    static int nNumPadChars;
};

class __declspec(dllimport) MfNullString {
};
extern __declspec(dllimport) MfNullString NULL_STRING;

class __declspec(dllimport) MfString {
public:
    MfString();
    MfString(const char *s);
    MfString(const MfString &s);
    MfString(const MfNullString &);
    MfString(int value);
    MfString(const MfStringSize &size);
    ~MfString();
    MfString &operator=(const MfString &s);
    MfString &operator=(const MfNullString &);
    MfString &operator=(const char *s);
    MfString &operator<<(const char *s);
    MfString &operator<<(const MfString &s);
    MfString &operator<<(char c);
    MfString &operator<<(int value);
    char GetCharacter(int index) const;
    int operator==(const MfString &s) const;
    int operator!=(const MfString &s) const;
    int operator==(const MfNullString &) const;
    int operator!=(const MfNullString &) const;
    operator const char *() const;
    operator char *() const;
    int __cdecl Format(char *const format, ...);
    MfString RightString(int count) const;
    MfString LeftString(int count) const;
    int CaseCompare(const MfString &s) const;
    MfString MidString(int start, int count) const;
    MfString operator+(const MfString &s) const;
    void ConvertToLines(MfStringList *lines) const;
    int CountCharacters(int length, int tabSize) const;
    void ExpandTabs(int tabSize);
    int ContainsSubString(const MfString &s, int start) const;
    int CaseContainsSubString(const MfString &s, int start) const;
    int ReverseCaseContainsSubString(const MfString &s, int start) const;
    unsigned int CuBeginningOfString(MfStringCursor *cursor) const;
    unsigned int CuBindToCursor(unsigned int position, MfStringCursor *cursor) const;
    unsigned int CuEndOfString(MfStringCursor *cursor) const;
    unsigned int CuMoveLeft(MfStringCursor *cursor) const;
    unsigned int CuMoveRight(MfStringCursor *cursor) const;
    unsigned int CuScanBackward(MfStringCursor *cursor, MfStringScanner &scanner) const;
    unsigned int CuScanForward(MfStringCursor *cursor, MfStringScanner &scanner) const;
    unsigned int CuSetTabSize(unsigned int tabSize, MfStringCursor *cursor) const;
    unsigned int CuSetCursorPos(unsigned int position, MfStringCursor *cursor) const;
    unsigned int CuSetCharPos(unsigned int position, MfStringCursor *cursor) const;
    int GetVariable(const MfString &name);
    void SplitPath(MfString *drive, MfString *directory, MfString *name, MfString *extension) const;
    void Tokenize(const MfString &separators, MfStringTokenizerCallBack &callBack) const;
    int Length() const { return pRef->nLength; }
    const char *GetData() const { return pRef->pData; }
    int ReadFromFile(const MfString &fileName)
    {
        CopyOnWrite(0);
        return pRef->ReadFromFile(fileName);
    }
    int WriteToFile(const MfString &fileName) const { return pRef->WriteToFile(fileName); }
    void Truncate(int length)
    {
        CopyOnWrite(0);
        pRef->Truncate(length);
    }
    void ConvertToLower()
    {
        CopyOnWrite(0);
        pRef->ConvertToLower();
    }
    void NoLeadingWhiteSpace()
    {
        CopyOnWrite(0);
        pRef->NoLeadingWhiteSpace();
    }
    void NoTrailingWhiteSpace()
    {
        CopyOnWrite(0);
        pRef->NoTrailingWhiteSpace();
    }
private:
    // Gives the string a body of its own before it is changed.
    void CopyOnWrite(int extra)
    {
        if (pRef == &mfsrNullInitializer) {
            pRef = new MfStringRef(extra);
            pRef->nRefs++;
        } else if (pRef->nRefs > 1) {
            pRef->nRefs--;
            pRef = pRef->CopyOnWrite();
            pRef->nRefs++;
        }
    }
    MfStringRef *pRef;
    static MfStringRef mfsrNullInitializer;
};

class MfCallBack;
class MfListItemInterface;

class MfDblListItem {
public:
    void *pData;                // +0x00
    MfDblListItem *pliPrev;     // +0x04
    MfDblListItem *pliNext;     // +0x08
};

// A new MfDblList gets a local copy of the vtable, a list that is a member or a
// base class the imported one (VC5 lays overloaded virtuals out in reverse order
// of declaration).
class __declspec(dllimport) MfDblList {
public:
    enum EnumListPosition { LP_BEFORE = 0, LP_AFTER = 1 };
    MfDblList() : pliFirst(0), pliLast(0), nCount(0) {}
    virtual ~MfDblList();
    virtual void Prepend(void *const pData);
    virtual void Append(void *const pData);
    virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli);
    virtual void Merge(MfDblList &list, EnumListPosition pos, MfDblListItem *pli);
    virtual void Add(void *const pData);
    virtual void Copy(const MfDblList &list);
    virtual void Copy(const MfDblList &list, MfCallBack &cb);
    virtual void Copy(const MfDblList &list, MfListItemInterface &lii);
    virtual void *GetFirst(MfDblListItem *&pli) const;
    virtual void *GetNext(MfDblListItem *&pli) const;
    virtual void *GetCurrent(MfDblListItem *&pli) const;
    virtual void *GetPrev(MfDblListItem *&pli) const;
    virtual void *GetLast(MfDblListItem *&pli) const;
    virtual void Iterate(MfCallBack &cb) const;
    virtual void Iterate(MfListItemInterface &lii) const;
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&pli);
    virtual void Clear();
    virtual void Clear(MfCallBack &cb);
    virtual void Clear(MfListItemInterface &lii);
    virtual MfDblListItem *Find(void *pData) const;
    virtual MfDblListItem *Find(void *pData, MfCallBack &cb) const;
    virtual MfDblListItem *Find(void *pData, MfListItemInterface &lii) const;
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n) const;
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n, MfCallBack &cb) const;
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n, MfListItemInterface &lii) const;
    int GetCount() const { return nCount; }
    MfDblListItem *FirstItem() const { return pliFirst; }
    MfDblListItem *LastItem() const { return pliLast; }
    // Non-virtual iteration that keeps the position in a register.
    void *First(MfDblListItem *&pli) const
    {
        pli = pliFirst;
        return pli ? pli->pData : 0;
    }
    void *Last(MfDblListItem *&pli) const
    {
        pli = pliLast;
        return pli ? pli->pData : 0;
    }
    static void *Next(MfDblListItem *&pli)
    {
        if (pli)
            pli = pli->pliNext;
        return pli ? pli->pData : 0;
    }
    static MfDblListItem *NextItem(MfDblListItem *&pli)
    {
        if (pli)
            pli = pli->pliNext;
        return pli;
    }
    static MfDblListItem *PrevItem(MfDblListItem *&pli)
    {
        if (pli)
            pli = pli->pliPrev;
        return pli;
    }
    void AddLast(void *data) { Insert(data, LP_AFTER, pliLast); }
    void AddTail(void *data) { Append(data); }
protected:
    MfDblListItem *pliFirst;    // +0x04
    MfDblListItem *pliLast;     // +0x08
    int nCount;                 // +0x0c
};

class __declspec(dllimport) MfStringTokenizerCallBack {
public:
    virtual ~MfStringTokenizerCallBack() {}
};

// A list of strings (0x14 bytes).
class __declspec(dllimport) MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    MfStringList() {}
    virtual ~MfStringList();
    virtual MfString *Add(const MfString &s);
    virtual void Clear();
};

inline MfStringList::~MfStringList()
{
    Clear();
}

// A call back of the containers; the constructors are inline in this DLL.
class __declspec(dllimport) MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *a);
    virtual int CallBack(void *a, void *b);
    virtual int CallBack(void *a, void *b, void *c);
};

class __declspec(dllimport) MfHashTable {
public:
    virtual ~MfHashTable();
    void Iterate(MfCallBack &cb);
protected:
    void ClearItems(MfCallBack *cb);
    friend class MfPeEditorData;
    // the data members of mfutil's MfHashTable: sizeof must be 0x14 for `new MfScHandleDict`
    int nNumberOfBuckets;        // +4
    int nBucketSize;             // +8
    void **apBuckets;            // +0xc
    void **apmfcnChain;          // +0x10
};

// Hash table of (MfString key, void *data).
class __declspec(dllimport) MfDictionary : public MfHashTable {
public:
    MfDictionary(int buckets);
    virtual ~MfDictionary();
    void *Add(const MfString &key, void *data);
    void *Find(const MfString &key);
};

// The registry settings of the application.
class __declspec(dllimport) UtilityApp {
public:
    UtilityApp() {}
    ~UtilityApp() {}
    int GetProfileInt(const MfString &section, const MfString &entry, int value);
    int WriteProfileInt(const MfString &section, const MfString &entry, int value);
};

// The time a file was last written.
class __declspec(dllimport) MfFileTime {
public:
    MfFileTime();
    int SetFromFile(const MfString &fileName);
    FILETIME mTime;                 // +0x00
};

class __declspec(dllimport) MfCriticalSection {
public:
    void Enter() { EnterCriticalSection(&mSection); }
    void Leave() { LeaveCriticalSection(&mSection); }
private:
    CRITICAL_SECTION mSection;
};

// Asks for a name (0x14 bytes).
class __declspec(dllimport) MfNameDialog {
public:
    MfNameDialog(MfString caption, MfString prompt, MfString name, HWND parent);
    int DoModal();
    MfString GetName();
private:
    HWND mParent;                   // +0x00
    int mResult;                    // +0x04
    MfString mCaption;              // +0x08
    MfString mPrompt;               // +0x0c
    MfString mName;                 // +0x10
};

class __declspec(dllimport) MfLogger {
public:
    static void Shutdown();
};

// A list item that is handed to the list functions to compare and destroy items.
class LiiExemplar {
public:
    ~LiiExemplar() {}
};

// ---- this DLL: resources ------------------------------------------------------

// 0x10035b20: the resources of this DLL.
extern HINSTANCE hPiccoloConsoleResources;

// Makes the DLL's resources current while it lives.
class PiccoloConsoleContext {
public:
    PiccoloConsoleContext()
    {
        mOldResources = AfxGetResourceHandle();
        AfxSetResourceHandle(hPiccoloConsoleResources);
    }
    virtual ~PiccoloConsoleContext() { AfxSetResourceHandle(mOldResources); }
private:
    HINSTANCE mOldResources;
};

// ---- this DLL: the code editor ------------------------------------------------

// Commands and messages of the editor windows.
enum {
    ID_PICCOLO_COMPILE = 0x9c49,
    ID_PICCOLO_CLASSES = 0x9c4d,
    ID_PE_CUT = 0x9c50,
    ID_PE_COPY = 0x9c51,
    ID_PE_PASTE = 0x9c52,
    ID_PE_UNDO = 0x9c53,
    ID_PE_REDO = 0x9c54,
    ID_PE_FIND_SELECT = 0x9c55,
    ID_PE_FIND_NEXT = 0x9c56,
    ID_PE_FIND_PREVIOUS = 0x9c57,
    ID_PE_TOGGLE_OVERWRITE = 0x9c62,
    ID_PE_PRINT = 0x9c7d,
    ID_PE_CUSTOM_FIRST = 0xd000,
    ID_PE_CUSTOM_LAST = 0xd400
};
#define WM_PE_UPDATE_CARET (WM_USER + 1)
#define WM_PE_GOTO_LINE (WM_USER + 2)

class MfPeCodeEditor;

// The interfaces of the editor (names from PscDebugger, which uses them).
class MfAi {
public:
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
};

class MfAiPeWindows : public MfAi {
public:
    virtual int QueryMainWindow(CWnd **wnd) = 0;
    virtual int QueryEditorWindow(CWnd **wnd) = 0;
};

// The editor interface a keyboard handler or a toolbar command gets.
class MfAiPiccoloEditor : public MfAi {
public:
    virtual int GetInterface(int id, MfAi **ai) = 0;
    virtual void GetPosition(int *column, int *line) = 0;
    virtual void GetModuleName(MfString *module) = 0;
    virtual void GetLineText(int line, MfString *text) = 0;
    virtual void GetWordAtCursor(MfString *word, int) = 0;
};

class MfPeCeDblClickHandler {
public:
    virtual ~MfPeCeDblClickHandler() {}
    virtual void HandleDblClick(const MfString &line) = 0;
};

class MfPeCursorChanged {
public:
    virtual void CursorChanged(int column, int line) = 0;
};

// Told when the buffer of an editor changes.
class MfPeBuffModCallBack {
public:
    virtual void BmcbNotify(int modified) = 0;
};

class MfPeKbdHandler {
public:
    virtual int KbhKeyDown(int key, int flags, MfAiPiccoloEditor *editor) = 0;
};

class MfPeSetFocusHandler {
public:
    virtual void ReceivedFocus(MfPeCodeEditor *editor) = 0;
    virtual void LostFocus(MfPeCodeEditor *editor) = 0;
};

class MfPeDrawingContext;
class CPiccoloWnd;

// A text region of the editor (positions are column/line, both from 1):
// the selection, the area a command works on. The pivot is the part a
// selection by words or lines keeps while it is extended.
class MfPeRegion {
public:
    enum Location { BEFORE = 3, AFTER = 4, INSIDE = 5 };

    MfPeRegion();
    virtual ~MfPeRegion();
    MfPeRegion &operator=(const MfPeRegion &other);
    int IsEmpty() const { return mStart.x == mEnd.x && mStart.y == mEnd.y; }
    int IsSelected() const { return mActive && !IsEmpty(); }
    void Reset();
    void Set(CPoint start, CPoint end);
    void Extend(CPoint point, int invalidate, MfPeDrawingContext *context);
    void Invalidate(MfPeDrawingContext *context);
    void ResetPivotEnd();
    void AdvancePivotEnd(int lastLine, int column);
    int GetRun(int line, int column, int *length);
    int Locate(int column, int line, int inclusive);
    int LocatePivot(CPoint point);

    CPoint mStart;                  // +0x04
    CPoint mEnd;                    // +0x0c
    int mActive;                    // +0x14
    CPoint mPivotStart;             // +0x18
    CPoint mPivotEnd;               // +0x20
};

// Draws the text of an editor and keeps its geometry: font, scroll position,
// caret and cursors (0x8c bytes).
class MfPeDrawingContext {
public:
    MfPeDrawingContext(MfPeCodeEditor *editor);
    virtual ~MfPeDrawingContext();
    void ResetScrollRanges(int lines);
    void SetLineCount(int lines);
    int CanScroll(UINT message, int direction);
    void SetupFont(CDC *dc);
    void SetClientSize(int width, int height);
    CPoint PointToPosition(const CPoint &point);
    CPoint PositionToPoint(const CPoint &position);
    void ClampPoint(CPoint &point);
    void SetDC(CDC *dc, const RECT *clip);
    void InvalidateLine(int line);
    void InvalidateMargin(int line);
    void InvalidateMarginFrom(int line);
    void InvalidateFrom(int line);
    void Update();
    void GetTextRect(int line, int firstColumn, int lastColumn, RECT *rect);
    void GetMarginRect(int line, RECT *rect);
    int IsTextVisible(int line, int column);
    int IsMarginVisible(int line);
    void DrawString(const MfString &text, COLORREF color, int column, int line);
    void DrawLineEnd(int line, int column);
    void EraseMargin(int line);
    void DrawGlyphs(int line, int glyphs);
    void EraseFrom(int line);
    void CreateEditCaret();
    void DestroyEditCaret();
    void SetCaret(const CPoint &position);
    void UpdateCaretForSelection();
    void UpdateCaretForMargin();
    void ScrollLines(int lines);
    void ScrollColumns(int columns);
    void ShowLine(const CPoint &position, int offset);
    void ShowColumn(const CPoint &position);
    void SetTextCursor();
    void UpdateCursor(CPoint point);
    void SetCursorType(int type);
    int UpdateDragCursor(CPoint point, int noDrop);
    void EndDrag(CPoint point);
    int IsOverSelection(CPoint point);
    int IsInSelection(CPoint point);
    int OpenClipboard(int empty);
    int CloseClipboard();
    int LineHeight() const { return mLineSpacing + mCharHeight; }
    int LineToY(int line) const { return (line - mTopLine) * LineHeight(); }
    void ScrollTo(UINT message, int position);
    void ShowMessage(const MfString &text);

    MfPeCodeEditor *mEditor;        // +0x04
    CDC *mDC;                       // +0x08
    CFont mFont;                    // +0x0c
    CRgn mUpdateRgn;                // +0x14
    CRect mClipRect;                // +0x1c
    CPoint mCaretPos;               // +0x2c
    int mHasCaret;                  // +0x34
    int mCaretInMargin;             // +0x38 caret hidden: in the margin
    int mCaretInSelection;          // +0x3c caret hidden: over the selection
    MfPeRegion *mSelection;         // +0x40
    int mDragging;                  // +0x44
    int mCharWidth;                 // +0x48
    int mCharHeight;                // +0x4c
    int mLineSpacing;               // +0x50
    int mTopLine;                   // +0x54
    int mVisibleLines;              // +0x58 including a partial line
    int mFullLines;                 // +0x5c
    int mLeftColumn;                // +0x60
    int mMarginWidth;               // +0x64
    int mVisibleColumns;            // +0x68 including a partial column
    int mFullColumns;               // +0x6c
    HCURSOR mMarginCursor;          // +0x70
    HCURSOR mTextCursor;            // +0x74
    HCURSOR mArrowCursor;           // +0x78
    HCURSOR mDropCursor;            // +0x7c
    HCURSOR mNoDropCursor;          // +0x80
    int mWidth;                     // +0x84
    int mHeight;                    // +0x88
};

// ---- the text and the commands on it ----

class MfPeTextLineList;
class MfPePrintingContext;
class MfPeEditorState;
class MfPeCharInsertState;
class MfPeCharOverwriteState;
class MfPeInsertStringState;
class MfPeDeleteCharState;
class MfPeJoinLineState;
class MfPeNewLineState;
class MfPeDeleteRegionState;
class MfPePasteRegionState;
class MfPeDragRegionState;
class MfPeIndentRegionState;
class MfPeUndentRegionState;
class MfPeDeleteBackWordState;
class MfPeDeleteForwardWordState;

// A position in the text of a search (column, line).
class MfPeSearchContext : public CPoint {
public:
    MfPeSearchContext() : CPoint(0, 0) {}
};

// A cursor over the tab-expanded display of a string (mfutil.dll; this DLL
// was built with the inline constructor).
class __declspec(dllimport) MfStringCursor {
public:
    MfStringCursor(int tabSize) : nCursPos(0), nCharPos(0), nTabSize(tabSize), nExpandedLength(0) {}
    unsigned int CursorPos() const { return nCursPos; }
    unsigned int CharPos() const { return nCharPos; }
    unsigned int TabSize() const { return nTabSize; }
    unsigned int ExpandedSize() const { return nExpandedLength; }

    int nCursPos;                   // +0x00 column, the tabs expanded
    int nCharPos;                   // +0x04 character
    int nTabSize;                   // +0x08
    int nExpandedLength;            // +0x0c
};

// A character class for the scans of a cursor.
class MfStringScanner {
public:
    virtual int TestChar(char c) = 0;
};

// The settings of new editors (the preferences of the editor windows).
extern int nPwTabSize;
extern int bPwSyntaxHighlight;
extern int bPwAutoIndent;

// High-resolution timer (mfutil.dll).
class __declspec(dllimport) MfHPTimer {
public:
    MfHPTimer() { Initialize(); }
    ~MfHPTimer() {}
    void Start() { QueryPerformanceCounter(&m_start); }
    void Stop() { QueryPerformanceCounter(&m_stop); }
    static int Initialize();
    double GetElapsedTimeInMs();
    LARGE_INTEGER m_start;          // +0x00
    LARGE_INTEGER m_stop;           // +0x08
};

// A token of a line the syntax colouring found: where it starts, how long
// it is and its kind (the index of its colour).
class MfPeMarker {
public:
    MfPeMarker() { *(int *)this = 0; }
    MfPeMarker(int column, int length, int kind)
        : mColumn(column), mLength(length), mKind(kind), mPad(0) {}
    ~MfPeMarker() {}
    unsigned char mColumn;
    unsigned char mLength;
    unsigned char mKind;
    unsigned char mPad;
};

// An array that grows by its first size.
template <class T> class MfArray {
public:
    MfArray() : mSize(5), mData(0) { mData = new T[5]; }
    virtual ~MfArray() { delete[] mData; }
    // Makes room for the element at index.
    void Grow(unsigned int index)
    {
        if (index >= mSize) {
            unsigned int size = index + 6;
            T *data = new T[size];
            for (unsigned int i = 0; i < mSize; i++)
                data[i] = mData[i];
            mSize = size;
            delete[] mData;
            mData = data;
        }
    }
    void Set(unsigned int index, const T &value)
    {
        if (index >= mSize)
            Grow(index);
        mData[index] = value;
    }
    unsigned int mSize;             // +0x04
    T *mData;                       // +0x08
};

// The syntax colouring of a line.
class MfPeLineSyntax {
public:
    enum {
        SCANNED = 1,
        COMMENT_START = 2,          // a comment starts on the line and goes on
        IN_COMMENT = 4,             // the line starts inside a comment
        COMMENT_END = 8,
        CHANGED = 0x10
    };
    MfPeLineSyntax() : mFlags(0), mCount(0) {}
    void Add(const MfPeMarker &marker);
    MfPeMarker Get(unsigned int index)
    {
        if (index >= mMarkers.mSize)
            mMarkers.Grow(index);
        return mMarkers.mData[index];
    }
    int mFlags;                     // +0x00
    int mCount;                     // +0x04
    MfArray<MfPeMarker> mMarkers;   // +0x08
};

// What the scanner of the syntax colouring reads: the text of one line;
// it adds the tokens it finds to the syntax of the line.
class MfPeLexInput {
public:
    void Init(const MfString &text, MfPeLineSyntax *syntax);
    void Skip(char *text, int length);
    void NextColumn();
    int Read(char *buffer, int size);
    void Token(char kind, int length);

    MfPeLineSyntax *mSyntax;        // +0x00
    MfString mText;                 // +0x04
    int mColumn;                    // +0x08
    int mRead;                      // +0x0c
};

// A spin lock (mfutil.dll; the constructor is inline in this DLL).
class __declspec(dllimport) MfLock {
public:
    MfLock() : lwLockWord(0) { InterlockedExchange(&lwLockWord, 0); }
    ~MfLock() {}
    int Lock(long timeout);
    void Unlock();
private:
    long lwLockWord;
};

// One line of the text of an editor (0x20 bytes).
class MfPeTextLine {
public:
    MfPeTextLine() : mGlyphs(0) {}
    MfPeTextLine(const MfString &text) : mText(text), mGlyphs(0) {}
    virtual ~MfPeTextLine() {}
    int Draw(int line, MfPeDrawingContext *context, int tabSize, int highlight);
    void DrawMargin(int line, MfPeDrawingContext *context);
    void Print(MfPePrintingContext *context, int tabSize, int highlight);
    void Scan(int tabSize);
    void ScanText(MfString &text, int tabSize);
    void FillTo(MfStringCursor *cursor);
    void InsertChar(char c, MfStringCursor *cursor);
    char DeleteChar(int forward, MfStringCursor *cursor);

    MfString mText;                 // +0x04
    MfPeLineSyntax mSyntax;         // +0x08
    int mGlyphs;                    // +0x1c
};

// The lines of an editor; it owns them.
class MfPeTextLineList : public MfDblList {
public:
    MfPeTextLineList();
    virtual ~MfPeTextLineList();
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&item);
    virtual void Clear();
    MfPeTextLine *Current(MfDblListItem *&item) const { return (MfPeTextLine *)MfDblList::GetCurrent(item); }
    MfPeTextLine *Next(MfDblListItem *&item) const { return (MfPeTextLine *)MfDblList::GetNext(item); }
    MfPeTextLine *Prev(MfDblListItem *&item) const { return (MfPeTextLine *)MfDblList::GetPrev(item); }
};

// How much of the first and the last line a region took.
struct MfPeRegionText {
    int mFirstLength;               // the length of the first line
    int mFirstTaken;                // what was taken from its end
    int mLastLength;                // the length of the last line
    int mLastTaken;                 // what was taken from its start
};

// The text of an editor and everything done to it (0x68 bytes); the
// MfAiPiccoloEditor interface of the editor.
class MfPeEditorData : public MfAiPiccoloEditor {
public:
    // The character classes of the moves by words.
    class IdentScanner : public MfStringScanner {
    public:
        virtual int TestChar(char c);
    };
    class PuncScanner : public MfStringScanner {
    public:
        virtual int TestChar(char c);
    };
    class WhiteSpaceScanner : public MfStringScanner {
    public:
        virtual int TestChar(char c);
    };

    // The glyphs set on the lines of one module, kept for the editors that
    // open it later.
    class GlyphModule : public MfCallBack {
    public:
        // The glyphs of one line.
        class GlyphInfo : public MfCallBack {
        public:
            GlyphInfo() : mLine(0), mGlyphs(0) {}
            GlyphInfo(int line, int glyphs) : mLine(line), mGlyphs(glyphs) {}
            virtual ~GlyphInfo() {}
            virtual int CallBack(void *info);
            int mLine;              // +0x04
            int mGlyphs;            // +0x08
        };
        GlyphModule(LiiExemplar);
        GlyphModule(const MfString &module);
        virtual ~GlyphModule();
        virtual int CallBack(void *module);
        GlyphInfo *Find(int line, MfDblListItem **position);
        void Add(int line, int glyphs);
        void Remove(int line, int glyphs);
        int Get(int line);
        void Clear(int glyphs);
        MfString mModule;           // +0x04
        MfDblList mGlyphs;          // +0x08
    };

    // Clears one kind of glyph in every module.
    class GlyphModuleIterator : public MfCallBack {
    public:
        GlyphModuleIterator(int glyphs) : mGlyphs(glyphs) {}
        virtual int CallBack(void *key, void *module, void *);
        int mGlyphs;                // +0x04
    };

    MfPeEditorData(MfPeDrawingContext *context);
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual int GetInterface(int id, MfAi **ai);
    virtual void GetPosition(int *column, int *line);
    virtual void GetModuleName(MfString *module);
    virtual void GetLineText(int line, MfString *text);
    virtual void GetWordAtCursor(MfString *word, int allLines);
    virtual ~MfPeEditorData();

    // The cursor as a command keeps it: the column is the character + 1.
    void GetCursor(CPoint &cursor) const
    {
        cursor = mCursor;
        cursor.x = mStringCursor.nCursPos + 1;
    }
    static MfPeTextLine *LineOf(MfDblListItem *item)
    {
        MfPeTextLine *line = item ? (MfPeTextLine *)item->pData : 0;
        return line;
    }

    MfString GetText();
    void SetText(const MfString &text, MfPeDrawingContext *context, int keepCursor);
    void AppendText(const MfString &text, MfPeDrawingContext *context);
    void Clear(MfPeDrawingContext *context);
    const MfString &GetSelectionLine();
    int IsAtTextEnd();
    void OnVScroll(UINT code, UINT position, MfPeDrawingContext *context);
    void OnHScroll(UINT code, UINT position, MfPeDrawingContext *context);
    void Draw(MfPeDrawingContext *context);
    void UpdateSyntax();
    void Print(MfPePrintingContext *context);
    void CursorUp(MfPeDrawingContext *context);
    void CursorDown(MfPeDrawingContext *context);
    void CursorLeft(MfPeDrawingContext *context);
    void CursorRight(MfPeDrawingContext *context);
    void TabLeft(MfPeDrawingContext *context);
    void TabStopLeft(MfPeDrawingContext *context);
    void TabStopRight(MfPeDrawingContext *context);
    void CursorHome(MfPeDrawingContext *context);
    void CursorEnd(MfPeDrawingContext *context);
    void PageUp(MfPeDrawingContext *context);
    void PageDown(MfPeDrawingContext *context);
    void GotoTextStart(MfPeDrawingContext *context);
    void GotoTextEnd(MfPeDrawingContext *context);
    void WordLeft(MfPeDrawingContext *context);
    void WordRight(MfPeDrawingContext *context);
    void ScrollUp(MfPeDrawingContext *context);
    void ScrollDown(MfPeDrawingContext *context);
    MfDblListItem *GetLineItem(int line);
    void SetCursor(int column, int line);
    void MoveCursorTo(CPoint point, MfPeDrawingContext *context);
    void ShowCursor(MfPeDrawingContext *context);
    void UpdateScrollRanges(MfPeDrawingContext *context);
    void RegisterCursorChangedCallback(MfPeCursorChanged *callBack);
    void UnregisterCursorChangedCallback(MfPeCursorChanged *callBack);
    void NotifyCursorChanged();
    void DragTo(CPoint point, int noDrop, MfPeDrawingContext *context);
    void EndDrag(CPoint point, MfPeDrawingContext *context);
    void StartSelection(MfPeDrawingContext *context);
    void SelectLine(CPoint point, MfPeDrawingContext *context);
    void SelectWord(CPoint point, MfPeDrawingContext *context);
    void CollapseSelection(int toStart, MfPeDrawingContext *context);
    void ClearSelection(MfPeDrawingContext *context);
    void ResetSelection(CPoint point, MfPeDrawingContext *context);
    void ExtendSelection(MfPeDrawingContext *context);
    void ExtendLineSelection(CPoint point, MfPeDrawingContext *context);
    void ExtendWordSelection(MfPeDrawingContext *context);
    void EndSelection();
    int IsSelectionPastLineEnd();
    int HasSelection() const
    {
        return mSelection.mActive
            && !(mSelection.mStart.x == mSelection.mEnd.x && mSelection.mStart.y == mSelection.mEnd.y);
    }
    int IsTextEmpty();

    // What the commands do and undo; the state is the one of the command.
    void InsertChar(char c, MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoInsertChar(MfPeEditorState *state, MfPeDrawingContext *context);
    void OverwriteChar(char c, MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoOverwriteChar(MfPeEditorState *state, MfPeDrawingContext *context);
    void InsertString(MfString *text, MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoInsertString(int length, MfPeEditorState *state, MfPeDrawingContext *context);
    void DeleteChar(int forward, MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoDeleteChar(MfPeEditorState *state, MfPeDrawingContext *context);
    void JoinLine(int forward, MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoJoinLine(MfPeEditorState *state, MfPeDrawingContext *context);
    void NewLine(MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoNewLine(MfPeEditorState *state, MfPeDrawingContext *context);
    void DeleteRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void CopyRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void CutRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoDeleteRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void PasteRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoPasteRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void DragRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoDragRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    int SpansLines();
    int CanUndent();
    void SelectWholeLines(MfPeDrawingContext *context, int update);
    void IndentRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoIndentRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void UndentRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoUndentRegion(MfPeEditorState *state, MfPeDrawingContext *context);
    void DeleteBackWord(MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoDeleteBackWord(MfPeEditorState *state, MfPeDrawingContext *context);
    void DeleteForwardWord(MfPeEditorState *state, MfPeDrawingContext *context);
    void UndoDeleteForwardWord(MfPeEditorState *state, MfPeDrawingContext *context);

    int Find(const MfString &text, MfPeDrawingContext *context, int backwards, int wrap);
    void GotoLine(int line, int mark, MfPeDrawingContext *context);
    void SetGlyph(int line, int glyphs, MfPeDrawingContext *context);
    void UnsetGlyph(int line, int glyphs, MfPeDrawingContext *context);
    void ClearGlyphsOnLine(int line, MfPeDrawingContext *context);
    void ClearGlyphs(MfPeDrawingContext *context);
    void ClearGlyph(int glyphs, MfPeDrawingContext *context);
    void SetModuleName(const MfString &module, MfPeDrawingContext *context);
    static void AddGlobalGlyph(const MfString &module, int line, int glyphs);
    static void RemoveGlobalGlyph(const MfString &module, int line, int glyphs);
    static void ClearGlobalGlyphs();
    static void ClearGlobalGlyph(int glyphs);
    void SetTabSize(int tabSize, MfPeDrawingContext *context);
    void SetAutoIndent(int autoIndent, MfPeDrawingContext *context);

private:
    // Puts the cursor on the column nearest to mCursor.x in its line.
    void BindCursor() { mCursor.x = LineOf(mCursorLine)->mText.CuBindToCursor(mCursor.x - 1, &mStringCursor) + 1; }
    void CreateLines();
    void JoinLines(int forward);
    void SplitLine();
    void GotoSelectionStart(MfPeDrawingContext *context);
    void GetRegionText(int remove, MfStringList *text, MfPeRegionText *lengths);
    void InsertLines(MfStringList *text);
    void CopyToClipboard(MfStringList *text, MfPeDrawingContext *context);
    void PasteFromClipboard(MfStringList *text, MfPeDrawingContext *context);

    static MfDictionary dictGlyphModules;
    static MfDblList listEditors;

public:
    MfPeTextLineList *mLines;       // +0x04
    MfString mModuleName;           // +0x08
    int mRefCount;                  // +0x0c
    int mTabSize;                   // +0x10
    CPoint mCursor;                 // +0x14 column (tabs expanded), line
    MfStringCursor mStringCursor;   // +0x1c
    MfDblList *mCursorCallBacks;    // +0x2c
    MfPeRegion mSelection;          // +0x30
    MfDblListItem *mCursorLine;     // +0x58
    int mUnixFormat;                // +0x5c
    int mSyntaxHighlight;           // +0x60
    int mAutoIndent;                // +0x64
};

// What a command keeps to undo itself; the commands derive from their state
// (the layouts behind mDone are known from the sizes only).
class MfPeEditorState {
public:
    MfPeEditorState() : mDone(0) {}
    virtual ~MfPeEditorState() {}
    int mDone;                      // +0x04
};

class MfPeCharInsertState : public MfPeEditorState {
public:
    CPoint mCursor;                 // +0x08
    int mLength;                    // +0x10 of the line before
};

class MfPeInsertStringState : public MfPeEditorState {
public:
    CPoint mCursor;                 // +0x08
    int mLength;                    // +0x10 of the line before
};

class MfPeCharOverwriteState : public MfPeEditorState {
public:
    CPoint mCursor;                 // +0x08
    int mLength;                    // +0x10 of the line before
    char mOldChar;                  // +0x14
};

class MfPeDeleteCharState : public MfPeEditorState {
public:
    CPoint mCursor;                 // +0x08
    int mForward;                   // +0x10
    char mDeletedChar;              // +0x14
};

class MfPeJoinLineState : public MfPeEditorState {
public:
    CPoint mCursor;                 // +0x08
    int mLength;                    // +0x10 of the first line before
    int mForward;                   // +0x14
};

class MfPeNewLineState : public MfPeEditorState {
public:
    CPoint mCursor;                 // +0x08
};

class MfPeDeleteRegionState : public MfPeEditorState {
public:
    virtual ~MfPeDeleteRegionState() {}
    CPoint mCursor;                 // +0x08
    MfStringList mText;             // +0x10
};

class MfPePasteRegionState : public MfPeEditorState {
public:
    int mLength;                    // +0x08 of the line before
    CPoint mStart;                  // +0x0c
    CPoint mEnd;                    // +0x14
};

class MfPeDragRegionState : public MfPeEditorState {
public:
    CPoint mTo;                     // +0x08
    CPoint mEnd;                    // +0x10
    CPoint mFrom;                   // +0x18
    int mLength;                    // +0x20 of the line dropped on
};

class MfPeIndentRegionState : public MfPeEditorState {
public:
    CPoint mStart;                  // +0x08
    CPoint mEnd;                    // +0x10
};

class MfPeUndentRegionState : public MfPeEditorState {
public:
    CPoint mStart;                  // +0x08
    CPoint mEnd;                    // +0x10
    MfStringList mRemoved;          // +0x18
};

class MfPeDeleteBackWordState : public MfPeEditorState {
public:
    MfString mWord;                 // +0x08
    CPoint mFrom;                   // +0x0c
    CPoint mTo;                     // +0x14
    int mJoined;                    // +0x1c
};

class MfPeDeleteForwardWordState : public MfPeEditorState {
public:
    MfString mWord;                 // +0x08
    CPoint mCursor;                 // +0x0c
    int mJoined;                    // +0x14
};

// A change of the text that can be undone and redone (0x10 bytes); the
// position is where the cursor was.
class MfPeCommand {
public:
    MfPeCommand(MfPeEditorData *data);
    virtual ~MfPeCommand() {}
    virtual void Do(MfPeDrawingContext *context) = 0;
    virtual void Undo(MfPeDrawingContext *context) = 0;
    virtual void Redo(MfPeDrawingContext *context);
    virtual CPoint GetPosition() { return mPosition; }

    MfPeEditorData *mData;          // +0x04
    CPoint mPosition;               // +0x08
};

class MfPeTypingCommand : public MfPeCommand, public MfPeCharInsertState {
public:
    MfPeTypingCommand(MfPeEditorData *data, char c) : MfPeCommand(data), mChar(c) {}
    virtual ~MfPeTypingCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
    char mChar;                     // +0x24
};

class MfPeInsertStringCommand : public MfPeCommand, public MfPeInsertStringState {
public:
    MfPeInsertStringCommand(MfPeEditorData *data, MfPeCharInsertState *typed) : MfPeCommand(data)
    {
        mCursor = typed->mCursor;
        mLength = typed->mLength;
    }
    virtual ~MfPeInsertStringCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
    virtual void Redo(MfPeDrawingContext *context);
    virtual CPoint GetPosition() { return mCursor; }
    MfString mText;                 // +0x24
};

class MfPeOverwriteCommand : public MfPeCommand, public MfPeCharOverwriteState {
public:
    MfPeOverwriteCommand(MfPeEditorData *data, char c) : MfPeCommand(data), mChar(c) {}
    virtual ~MfPeOverwriteCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
    char mChar;                     // +0x28
};

class MfPeDeleteCommand : public MfPeCommand, public MfPeDeleteCharState {
public:
    MfPeDeleteCommand(MfPeEditorData *data, int backspace) : MfPeCommand(data), mBackspace(backspace) {}
    virtual ~MfPeDeleteCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
    int mBackspace;                 // +0x28
};

class MfPeJoinLineCommand : public MfPeCommand, public MfPeJoinLineState {
public:
    MfPeJoinLineCommand(MfPeEditorData *data, int forward) : MfPeCommand(data), mForward(forward) {}
    virtual ~MfPeJoinLineCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
    int mForward;                   // +0x28
};

class MfPeNewLineCommand : public MfPeCommand, public MfPeNewLineState {
public:
    MfPeNewLineCommand(MfPeEditorData *data) : MfPeCommand(data) {}
    virtual ~MfPeNewLineCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
};

class MfPeDeleteRegionCommand : public MfPeCommand, public MfPeDeleteRegionState {
public:
    MfPeDeleteRegionCommand(MfPeEditorData *data) : MfPeCommand(data) {}
    virtual ~MfPeDeleteRegionCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
    virtual void Redo(MfPeDrawingContext *context);
    MfPeRegion mRegion;             // +0x34
};

class MfPeCutRegionCommand : public MfPeCommand, public MfPeDeleteRegionState {
public:
    MfPeCutRegionCommand(MfPeEditorData *data) : MfPeCommand(data) {}
    virtual ~MfPeCutRegionCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
    virtual void Redo(MfPeDrawingContext *context);
    MfPeRegion mRegion;             // +0x34
};

class MfPeCopyRegionCommand : public MfPeCommand, public MfPeEditorState {
public:
    MfPeCopyRegionCommand(MfPeEditorData *data) : MfPeCommand(data) {}
    virtual ~MfPeCopyRegionCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context) {}
};

class MfPePasteRegionCommand : public MfPeCommand, public MfPePasteRegionState {
public:
    MfPePasteRegionCommand(MfPeEditorData *data) : MfPeCommand(data) {}
    virtual ~MfPePasteRegionCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
};

class MfPeDragRegionCommand : public MfPeCommand, public MfPeDragRegionState {
public:
    MfPeDragRegionCommand(MfPeEditorData *data) : MfPeCommand(data) {}
    virtual ~MfPeDragRegionCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
    virtual void Redo(MfPeDrawingContext *context);
    MfPeRegion mRegion;             // +0x34
};

class MfPeIndentRegionCommand : public MfPeCommand, public MfPeIndentRegionState {
public:
    MfPeIndentRegionCommand(MfPeEditorData *data) : MfPeCommand(data) {}
    virtual ~MfPeIndentRegionCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
    virtual void Redo(MfPeDrawingContext *context);
    MfPeRegion mRegion;             // +0x28
};

class MfPeUndentRegionCommand : public MfPeCommand, public MfPeUndentRegionState {
public:
    MfPeUndentRegionCommand(MfPeEditorData *data) : MfPeCommand(data) {}
    virtual ~MfPeUndentRegionCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
    virtual void Redo(MfPeDrawingContext *context);
    MfPeRegion mRegion;             // +0x3c
};

class MfPeDeleteBackWordCommand : public MfPeCommand, public MfPeDeleteBackWordState {
public:
    MfPeDeleteBackWordCommand(MfPeEditorData *data) : MfPeCommand(data) {}
    virtual ~MfPeDeleteBackWordCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
};

class MfPeDeleteForwardWordCommand : public MfPeCommand, public MfPeDeleteForwardWordState {
public:
    MfPeDeleteForwardWordCommand(MfPeEditorData *data) : MfPeCommand(data) {}
    virtual ~MfPeDeleteForwardWordCommand() {}
    virtual void Do(MfPeDrawingContext *context);
    virtual void Undo(MfPeDrawingContext *context);
};

// The commands of an editor, the oldest first; it owns them.
class MfPeCommandList : public MfDblList {
public:
    MfPeCommandList();
    virtual ~MfPeCommandList();
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&pli);
    virtual void Clear();
};

// ---- the window ----

// The code editor window (0xb0 bytes).
class __declspec(dllexport) MfPeCodeEditor : public CWnd {
    DECLARE_DYNCREATE(MfPeCodeEditor)
public:
    MfPeCodeEditor();
    virtual ~MfPeCodeEditor();
    MfString GetTextData();
    void SetTextData(const MfString &text, int);
    void SetModuleName(const MfString &module);
    MfString GetModuleName() const;
    void AppendText(const MfString &text);
    void Clear();
    void SetUnixFormat();
    void SetDosFormat();
    void EnableSyntaxHighlight();
    void DisableSyntaxHighlight();
    void GotoLine(int line, int column);
    void SetGlyphOnLine(int line, int glyph);
    void UnsetGlyphOnLine(int line, int glyph);
    void ClearGlyphsOnLine(int line);
    void ClearGlyphs();
    void ClearGlyphs(int glyph);
    static void AddGlyphAtLineForModule(const MfString &module, int line, int glyph);
    static void RemoveGlyphAtLineForModule(const MfString &module, int line, int glyph);
    static void ClearGlobalGlyphs();
    static void ClearGlobalGlyphs(int glyph);
    int FindFirst(const MfString &text, MfPeSearchContext *context, int backwards);
    int FindNext(const MfString &text, MfPeSearchContext *context, int backwards);
    int IsTextModified();
    void SetModificationFlag(int modified);
    void OnEditCopy();
    void OnUpdateEditCopy(CCmdUI *cmdUI);
    void OnEditCut();
    void OnUpdateEditCut(CCmdUI *cmdUI);
    void OnEditPaste();
    void OnUpdateEditPaste(CCmdUI *cmdUI);
    void OnEditPrint();
    void OnEditRedo();
    void OnUpdateEditRedo(CCmdUI *cmdUI);
    void OnEditUndo();
    void OnUpdateEditUndo(CCmdUI *cmdUI);
    void OnFindNext();
    void OnFindPrevious();
    void OnToggleOverwrite();
    void OnUpdateToggleOverwrite(CCmdUI *cmdUI);
    MfAiPiccoloEditor *GetIEditor();
    void SetPiccoloWindow(CPiccoloWnd *wnd) { mPiccoloWnd = wnd; }
    void DoSetReadOnly() { mReadOnly = 1; }
    void DoSetReadWrite() { mReadOnly = 0; }
    void SetDblClickSelectsLine() { mDblClickSelectsLine = 1; }
    void SetDblClickSelectsWord() { mDblClickSelectsLine = 0; }
    void InstallDblClickHandler(MfPeCeDblClickHandler *handler) { mDblClickHandler = handler; }
    int IsAttached(MfPeEditorData *data) { return data == mData; }
    void SetSearchString(const MfString &text) { mSearchString = text; }
    void InstallBufferModifiedCallBack(MfPeBuffModCallBack *callBack);
    void UninstallBufferModifiedCallBack(MfPeBuffModCallBack *callBack);
    void InstallKeyboardHandler(MfPeKbdHandler *handler);
    void UninstallKeyboardHandler(MfPeKbdHandler *handler);
    static void InstallGlobalKeyboardHandler(MfPeKbdHandler *handler);
    static void UninstallGlobalKeyboardHandler(MfPeKbdHandler *handler);
    void RegisterCursorChangedCallback(MfPeCursorChanged *callBack);
    void UnregisterCursorChangedCallback(MfPeCursorChanged *callBack);
    void InstallSetFocusHandler(MfPeSetFocusHandler *handler);
    void UninstallSetFocusHandler(MfPeSetFocusHandler *handler);
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual BOOL PreTranslateMessage(MSG *msg);
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT type, int cx, int cy);
    afx_msg void OnVScroll(UINT code, UINT position, CScrollBar *scrollBar);
    afx_msg void OnHScroll(UINT code, UINT position, CScrollBar *scrollBar);
    afx_msg void OnClose();
    afx_msg void OnSetFocus(CWnd *oldWnd);
    afx_msg void OnKillFocus(CWnd *newWnd);
    afx_msg void OnLButtonDown(UINT flags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT flags, CPoint point);
    afx_msg void OnLButtonUp(UINT flags, CPoint point);
    afx_msg void OnRButtonDown(UINT flags, CPoint point);
    afx_msg void OnMouseMove(UINT flags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT flags, short delta, CPoint point);
    afx_msg void OnKeyDown(UINT key, UINT repeat, UINT flags);
    afx_msg void OnChar(UINT c, UINT repeat, UINT flags);
    afx_msg UINT OnGetDlgCode();
    afx_msg LRESULT OnUpdateCaretPos(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGotoLine(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer(UINT id);
    afx_msg void OnNcDestroy();
    DECLARE_MESSAGE_MAP()
private:
    void StandardAutoScrollTest(CPoint point, int &scrollX, int &scrollY);
    void AddCommand(MfPeCommand *command);
    int DeleteSelectedRegion();
    void NotifyBuffMod();
    void SendToKeyboardHandlers(int key, int shift, int control, int alt);
    void StartSlowTimer()
    {
        if (!mSlowTimer)
            mSlowTimer = ::SetTimer(m_hWnd, 50, 100, NULL);
    }
    void StartFastTimer()
    {
        if (!mFastTimer)
            mFastTimer = ::SetTimer(m_hWnd, 51, 25, NULL);
    }
    void StartSetFocusTimer()
    {
        if (!mSetFocusTimer)
            mSetFocusTimer = ::SetTimer(m_hWnd, 52, 100, NULL);
    }
    void KillSlowTimer()
    {
        if (mSlowTimer) {
            ::KillTimer(m_hWnd, mSlowTimer);
            mSlowTimer = 0;
        }
    }
    void KillFastTimer()
    {
        if (mFastTimer) {
            ::KillTimer(m_hWnd, mFastTimer);
            mFastTimer = 0;
        }
    }
    void KillSetFocusTimer()
    {
        if (mSetFocusTimer) {
            ::KillTimer(m_hWnd, mSetFocusTimer);
            mSetFocusTimer = 0;
        }
    }
    static MfPeCommand *CommandOf(MfDblListItem *item) { return item ? (MfPeCommand *)item->pData : 0; }

    static MfDblList listGlobalKeyboardHandlers;

    MfPeDrawingContext *mDrawing;   // +0x40
    MfPePrintingContext *mPrinting; // +0x44
    MfPeEditorData *mData;          // +0x48
    int mReadOnly;                  // +0x4c
    MfString mModuleName;           // +0x50
    int mOverwrite;                 // +0x54
    MfString mSearchString;         // +0x58
    CPiccoloWnd *mPiccoloWnd;       // +0x5c
    int mMouseDown;                 // +0x60
    int mMarginSelect;              // +0x64
    int mWordSelect;                // +0x68
    int mDblClickSelectsLine;       // +0x6c
    MfPeCeDblClickHandler *mDblClickHandler; // +0x70
    int mDragPending;               // +0x74
    CPoint mMousePoint;             // +0x78
    int mWheelDelta;                // +0x80
    int mSized;                     // +0x84
    MfPeCommandList *mCommands;     // +0x88
    MfDblListItem *mUndoItem;       // +0x8c
    MfDblListItem *mRedoItem;       // +0x90
    MfDblListItem *mSavedItem;      // +0x94
    MfDblList *mBuffModCallBacks;   // +0x98
    MfDblList *mKeyboardHandlers;   // +0x9c
    MfDblList *mSetFocusHandlers;   // +0xa0
    UINT mFastTimer;                // +0xa4
    UINT mSlowTimer;                // +0xa8
    UINT mSetFocusTimer;            // +0xac
};

// Asks for the line to go to (0x78 bytes).
class MfPeGotoDlg : public CDialog {
public:
    MfPeGotoDlg(int line, int lines, CWnd *parent);
protected:
    virtual void DoDataExchange(CDataExchange *dx);
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
public:
    int mLine;                      // +0x60 the line chosen
    int mCurrentLine;               // +0x64
    int mLineCount;                 // +0x68
    CString mCurrentText;           // +0x6c
    CString mLineText;              // +0x70
    CString mCountText;             // +0x74
};

// The preferences of the editor windows (0x80 bytes).
class MfPePwPreferencesDlg : public CDialog {
public:
    MfPePwPreferencesDlg(int tabSize, int defaultTabSize, int syntaxHighlight, int defaultSyntaxHighlight,
                         int autoIndent, int defaultAutoIndent, CWnd *parent);
protected:
    virtual void DoDataExchange(CDataExchange *dx);
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
public:
    int mTabSize;                   // +0x60
    int mDefaultTabSize;            // +0x64
    int mAutoIndent;                // +0x68
    int mSyntaxHighlight;           // +0x6c
    int mDefaultAutoIndent;         // +0x70
    int mDefaultSyntaxHighlight;    // +0x74
    CString mTabSizeText;           // +0x78
    CString mDefaultTabSizeText;    // +0x7c
};

// Prints the text of an editor: a title on top and the page number below
// every page (0x58 bytes).
class MfPePrintingContext {
public:
    MfPePrintingContext(MfPeCodeEditor *editor);
    virtual ~MfPePrintingContext() {}
    int Begin(CDC *dc, const MfString &title);
    void End();
    void Print(const MfString &text, int style);
    void NewLine();
    void StartNewPage();

    MfPeCodeEditor *mEditor;        // +0x04
    CDC *mDC;                       // +0x08
    CFont mTitleFont;               // +0x0c
    CFont mFont;                    // +0x14
    CFont mItalicFont;              // +0x1c
    CFont mBoldFont;                // +0x24
    MfString mTitle;                // +0x2c
    int mCharWidth;                 // +0x30
    int mCharHeight;                // +0x34
    int mMarginX;                   // +0x38
    int mMarginY;                   // +0x3c
    int mLinesPerPage;              // +0x40
    int mColumnsPerLine;            // +0x44
    int mLine;                      // +0x48
    int mColumn;                    // +0x4c
    int mPage;                      // +0x50
    int mPrinting;                  // +0x54
};

inline void MfPeDrawingContext::ScrollTo(UINT message, int position)
{
    mEditor->SendMessage(message, MAKELONG(SB_THUMBPOSITION, position));
}

inline void MfPeDrawingContext::ShowMessage(const MfString &text)
{
    mEditor->MessageBox(text, "Piccolo Editor", MB_ICONEXCLAMATION);
    mEditor->SetFocus();
}

class __declspec(dllexport) CConsoleEdit : public MfPeCodeEditor {
    DECLARE_DYNCREATE(CConsoleEdit)
public:
    CConsoleEdit();
    virtual ~CConsoleEdit();
    void SetReadOnly(int readOnly);
    MfString GetEntireContents();
    void SetEntireContents(const MfString &text, int);
    int IsModified();
    void SetModified();
    void SetUnModified();
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    DECLARE_MESSAGE_MAP()
};

// ---- this DLL: the editor window ------------------------------------------------

// ---- what the output windows show ----

class MfIPcErrorInfo {
public:
    virtual const MfString &GetModule() const = 0;
    virtual const MfString &GetMessageA() const = 0;
    virtual void FormatLine(MfStringList &lines) const = 0;
};

// Receives the errors of a compilation (PiccoloComp).
class __declspec(dllimport) MfPcErrorCallBack {
public:
    virtual void CallBack(MfIPcErrorInfo *info) = 0;
};

// One diagnostic message and its level (PiccoloRT).
class __declspec(dllimport) MfScDiagnosticInfo {
public:
    MfString mMessage;              // +0x00
    int mLevel;                     // +0x04
};

class __declspec(dllimport) MfScDiagnosticCallBack {
public:
    virtual ~MfScDiagnosticCallBack() {}
    virtual void CallBack(const MfScDiagnosticInfo &info);
};

class __declspec(dllimport) MfScDiagnostics {
    friend class CConsoleFrame;
public:
    static void AddOutput(MfScDiagnosticCallBack *callBack);
    static int RemoveOutput(MfScDiagnosticCallBack *callBack);
private:
    static int iLevel;
};

class CConsoleOutput;

// Shows the errors of a compilation in the build window.
class MfPcErrorWindowCallBack : public MfPcErrorCallBack {
public:
    MfPcErrorWindowCallBack(CConsoleOutput *output) : mOutput(output) {}
    virtual void CallBack(MfIPcErrorInfo *info);
    virtual ~MfPcErrorWindowCallBack();
    CConsoleOutput *mOutput;        // +0x04
};

// Shows the diagnostic messages in the diagnostic window.
class MfDiagnosticOutputCallBack : public MfScDiagnosticCallBack {
public:
    MfDiagnosticOutputCallBack(CConsoleOutput *output) : mOutput(output) {}
    virtual ~MfDiagnosticOutputCallBack();
    virtual void CallBack(const MfScDiagnosticInfo &info);
    CConsoleOutput *mOutput;        // +0x04
};

// Shows the run time errors (level 5) in the console window.
class MfRuntimeErrorCallBack : public MfScDiagnosticCallBack {
public:
    MfRuntimeErrorCallBack(CConsoleOutput *output) : mOutput(output) {}
    virtual ~MfRuntimeErrorCallBack();
    virtual void CallBack(const MfScDiagnosticInfo &info);
    CConsoleOutput *mOutput;        // +0x04
};

class MfAiConsoleOutput;

// The script console manager of PiccoloRT.
class __declspec(dllimport) MfScConsoleManager {
public:
    void AddOutput(MfAiConsoleOutput *output);
    void RemoveOutput(MfAiConsoleOutput *output);
private:
    static MfScConsoleManager *pcmManager;
    friend class MfPiccoloConsoleWnd;
};

// The console of the scripts: an editor window that prints.
class MfAiConsoleOutput : public MfAi {
public:
    virtual void PrintString(const MfString &text) = 0;
    virtual void PrintStringNl(const MfString &text) = 0;
    virtual void ClearConsole() = 0;
};

class __declspec(dllexport) MfPiccoloConsoleWnd : public MfPeCodeEditor, public MfAiConsoleOutput {
public:
    MfPiccoloConsoleWnd();
    virtual ~MfPiccoloConsoleWnd();
    void Enable();
    void Disable();
    virtual BOOL Create(const RECT &rect, CWnd *parent, UINT id, DWORD style, DWORD exStyle);
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual void PrintString(const MfString &text);
    virtual void PrintStringNl(const MfString &text);
    virtual void ClearConsole();
protected:
    void Print(const MfString &text, int newLine);
};

// The output windows of an editor window on tabs (0xe4 bytes): the console,
// the build and the diagnostic windows and up to 10 windows of plugins.
class __declspec(dllexport) CConsoleOutput : public CTabCtrl {
    DECLARE_DYNCREATE(CConsoleOutput)
public:
    CConsoleOutput();
    virtual ~CConsoleOutput();
    static void Initialize();
    void DiagOutputCBDestroyed(MfDiagnosticOutputCallBack *callBack);
    void RtErrorCBDestroyed(MfRuntimeErrorCallBack *callBack);
    void ErrWndCBDestroyed(MfPcErrorWindowCallBack *callBack);
    void SwitchToWindow(int window);
    void Print(int window, const MfString &text, int newLine);
    void Clear(int window);
    void InstallBuildDblClickHandler(MfPeCeDblClickHandler *handler);
    void SetupOutput();
    void InstallWnd(int window);
    int InstallUserWnd(const MfString &name, CWnd *wnd);
    MfPcErrorCallBack &ErrorHandler() { return *mErrorHandler; }
protected:
    CWnd *GetWindow(int window);
    int FromTab(int tab) { return mTabWindow[tab]; }
    int ToTab(int window) { return mWindowTab[window]; }
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *result);
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnSize(UINT type, int cx, int cy);
    afx_msg void OnSelchange(NMHDR *nmhdr, LRESULT *result);
    afx_msg void OnNcDestroy();
    DECLARE_MESSAGE_MAP()

    MfPiccoloConsoleWnd *mConsole;  // +0x40
    MfPeCodeEditor *mBuildWnd;      // +0x44
    MfPeCodeEditor *mDiagWnd;       // +0x48
    CWnd *mUserWnds[10];            // +0x4c
    int mUserCount;                 // +0x74
    int mCurrentTab;                // +0x78
    int mTabCount;                  // +0x7c
    int mTabWindow[10];             // +0x80
    int mWindowTab[10];             // +0xa8
    CFont mFont;                    // +0xd0
    MfPcErrorCallBack *mErrorHandler;   // +0xd8
    MfDiagnosticOutputCallBack *mDiagCallBack;  // +0xdc
    MfRuntimeErrorCallBack *mRuntimeCallBack;   // +0xe0
};

// The splitter between the editor and the output windows.
class CPiccoloSplitter : public CSplitterWnd {
public:
    virtual CWnd *GetActivePane(int *row = NULL, int *col = NULL);
protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *result);
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *result);
};

// The tool bar of an editor window: its update tells the window.
class CPiccoloToolBar : public CToolBar {
protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};

class CPiccoloWnd;

// The combo box of the find tool (0x44 bytes).
class CFindSelectComb : public CComboBox {
public:
    CFindSelectComb(CPiccoloWnd *owner);
    virtual ~CFindSelectComb();
protected:
    DECLARE_MESSAGE_MAP()
    CPiccoloWnd *mOwner;            // +0x40
};

// The edit field of the find tool, a child of the combo box (0x4c bytes).
class MfPcsFindSelectEdit : public CEdit {
public:
    MfPcsFindSelectEdit() : mSuperWndProc(0) {}
    virtual BOOL PreTranslateMessage(MSG *msg);
protected:
    virtual WNDPROC *GetSuperWndProcAddr();
    afx_msg void OnKeyDown(UINT key, UINT repeat, UINT flags);
    afx_msg void OnSetFocus(CWnd *oldWnd);
    DECLARE_MESSAGE_MAP()
public:
    CPiccoloWnd *mOwner;            // +0x40
    WNDPROC mSuperWndProc;          // +0x44
    CString mLastText;              // +0x48
};

// The window that shows the full title when it does not fit (0x48 bytes).
class MfToolHelp : public CWnd {
public:
    MfToolHelp();
    virtual ~MfToolHelp();
    void FitText();
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
public:
    CFont *mFont;                   // +0x40
    MfString mText;                 // +0x44
};

// ---- the custom tool bars ----

class MfAiToolbarBitmapLoader : public MfAi {
public:
    virtual int LoadBitmap(CToolBar *toolBar) = 0;
};

class MfAiToolbarHandlers : public MfAi {
public:
    virtual int HandleCommand(int toolbar, int id, MfAiPiccoloEditor *editor) = 0;
    virtual int HandleUpdate(int toolbar, int id, CCmdUI *cmdUI) = 0;
};


class MfListItemInterface {
public:
    virtual void *Clone(void *data) { return 0; }
    virtual int Accept(void *data) { return 1; }
    virtual void Destroy(void *data) {}
    virtual int Compare(void *data, void *other) { return 0; }
};

class MfPeMetaCustomToolbarEntry;
class MfPeMetaToolbarEntryItem;
class MfPeMetaToolbarEntrySeparator;
class MfPeCustomToolbar;

// The description of a custom tool bar a plugin installs (0x14 bytes).
class __declspec(dllexport) MfPeMetaCustomToolbar : public MfListItemInterface {
public:
    MfPeMetaCustomToolbar(const MfString &name, int id, MfAiToolbarBitmapLoader *loader);
    MfPeMetaCustomToolbar(LiiExemplar);
    virtual ~MfPeMetaCustomToolbar();
    virtual void Destroy(void *data);
    MfPeMetaCustomToolbar &operator+(MfPeMetaCustomToolbarEntry *entry)
    {
        Add(entry);
        return *this;
    }
    int ID() const { return mID; }
    const MfString &Name() const { return mName; }
    MfPeCustomToolbar *CreateInstance(CPiccoloWnd *wnd, int &nextCommand);
    void Add(MfPeMetaCustomToolbarEntry *entry);

    MfString mName;                 // +0x04
    int mID;                        // +0x08
    MfDblList *mEntries;            // +0x0c
    MfAiToolbarBitmapLoader *mLoader;   // +0x10
};

// An entry of a custom tool bar: a button or a separator (8 bytes).
class __declspec(dllexport) MfPeMetaCustomToolbarEntry : public MfListItemInterface {
public:
    enum { ITEM = 0, SEPARATOR = 1 };
    MfPeMetaCustomToolbarEntry(LiiExemplar) {}
    MfPeMetaCustomToolbarEntry() : mType(ITEM) {}
    MfPeMetaCustomToolbarEntry(int type) : mType(type) {}
    virtual ~MfPeMetaCustomToolbarEntry() {}
    virtual void Destroy(void *data);
    int Type() const { return mType; }
    MfPeMetaToolbarEntryItem *ToItem() { return (MfPeMetaToolbarEntryItem *)this; }
    MfPeMetaToolbarEntrySeparator *ToSeparator() { return (MfPeMetaToolbarEntrySeparator *)this; }

    int mType;                      // +0x04
};

// A button of a custom tool bar (0x14 bytes).
class __declspec(dllexport) MfPeMetaToolbarEntryItem : public MfPeMetaCustomToolbarEntry {
public:
    MfPeMetaToolbarEntryItem(int id, const MfString &toolTip, MfAiToolbarHandlers *handlers);
    virtual ~MfPeMetaToolbarEntryItem();
    const MfString &ToolTip() const { return mToolTip; }
    int ID() const { return mID; }
    int HandleCommand(int toolbar, MfAiPiccoloEditor *editor) { return mHandlers->HandleCommand(toolbar, mID, editor); }
    int HandleUpdate(int toolbar, CCmdUI *cmdUI) { return mHandlers->HandleUpdate(toolbar, mID, cmdUI); }

    MfString mToolTip;              // +0x08
    int mID;                        // +0x0c
    MfAiToolbarHandlers *mHandlers; // +0x10
};

// A separator of a custom tool bar (8 bytes).
class __declspec(dllexport) MfPeMetaToolbarEntrySeparator : public MfPeMetaCustomToolbarEntry {
public:
    MfPeMetaToolbarEntrySeparator() : MfPeMetaCustomToolbarEntry(SEPARATOR) {}
};

// A custom tool bar in an editor window (0x18 bytes).
class MfPeCustomToolbar : public MfListItemInterface {
public:
    MfPeCustomToolbar(MfPeMetaCustomToolbar *meta, MfDblList *entries, MfAiToolbarBitmapLoader *loader,
                      CPiccoloWnd *wnd, int &nextCommand);
    MfPeCustomToolbar(LiiExemplar);
    virtual ~MfPeCustomToolbar();
    virtual void Destroy(void *data);
    int HandleCommand(int command, MfAiPiccoloEditor *editor);
    int HandleUpdate(int command, CCmdUI *cmdUI);
    int GetToolTip(int command, MfString &toolTip);

    MfPeMetaCustomToolbar *mMeta;   // +0x04
    MfPeMetaToolbarEntryItem **mItems;  // +0x08
    int mCount;                     // +0x0c
    int mFirstCommand;              // +0x10
    CPiccoloToolBar *mToolBar;      // +0x14
};

// ---- PiccoloRT.dll: the script runtime the console frame drives ----------------

class MfScData;
class MfScHandleEnum;
class CConsoleFrame;

// A reference-counted, typed value of the script VM (8 bytes).
class __declspec(dllimport) MfScDataHandle {
public:
    MfScData *GetData() const { return (m_flags & 0xfc0) ? m_data : 0; }
    int IsType(unsigned long type) const { return m_flags & type; }
    void SetReadOnly() { m_flags |= 8; }
    unsigned short AddRef() { return ++m_refCount; }
    unsigned short Release() { return m_refCount > 0 ? --m_refCount : m_refCount; }
    static MfScDataHandle *CreateData(const MfString &text, int);
    static MfScDataHandle *CreateData(float value);
    static MfScDataHandle *CreateData(int value);
    static MfScDataHandle *CreateDataArray(int size);
    static void DumpPool();
    static void EnumHandles(int heap, MfScHandleEnum &enumerator);
    static void LockGC() { csGCGuard.Enter(); }
    static void UnlockGC() { csGCGuard.Leave(); }
private:
    static MfCriticalSection csGCGuard;
    unsigned short m_refCount;      // +0x00
    unsigned short m_flags;         // +0x02 0x800 function, 0x8 read only
    MfScData *m_data;               // +0x04
};

// Told of every handle by MfScDataHandle::EnumHandles.
class MfScHandleEnum {
public:
    virtual int EnumHandle(MfScDataHandle *handle) = 0;
};

class __declspec(dllimport) MfScData {
public:
    virtual ~MfScData();
    MfString &Name() { return *m_name; }
protected:
    int m_lock;                     // +0x04
    MfString *m_name;               // +0x08
};

// A script array.
class __declspec(dllimport) MfScDataArray : public MfScData {
public:
    virtual ~MfScDataArray();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual int SetAt(int index, MfScDataHandle *value, int noAddRef);
};

class __declspec(dllimport) MfScDataMethod : public MfScData {
public:
    virtual ~MfScDataMethod();
    int m_type;                     // +0x0c 0 bytecode
    MfScDataHandle **m_constants;   // +0x10
    int m_numConstants;             // +0x14
    int m_numArgs;                  // +0x18 -1 any number
};

// A routine compiled to Piccolo bytecode (0x3c bytes).
class __declspec(dllimport) MfScBytecodeRoutine : public MfScDataMethod {
public:
    virtual ~MfScBytecodeRoutine();
    unsigned int MemoryUsage(int part);
    MfString &ModuleName() { return *mModuleName; }
    unsigned char *mBytecodes;      // +0x1c
    int mCodeSize;                  // +0x20
    int mFlags;                     // +0x24
    MfString *mModuleName;          // +0x28
};

// A list of script function handles with an internal position.
class __declspec(dllimport) MfScHandleList : public MfDblList {
public:
    MfScHandleList() : mPos(0) {}
    virtual ~MfScHandleList();
    MfScDataHandle *First()
    {
        mPos = pliFirst;
        MfScDataHandle *handle = mPos ? (MfScDataHandle *)mPos->pData : 0;
        return handle;
    }
    MfScDataHandle *Next()
    {
        if (mPos)
            mPos = mPos->pliNext;
        MfScDataHandle *handle = mPos ? (MfScDataHandle *)mPos->pData : 0;
        return handle;
    }
private:
    MfDblListItem *mPos;            // +0x10
};

// Script variables by name.
class __declspec(dllimport) MfScHandleDict : public MfDictionary {
public:
    enum HandleDictPassRef { kAddRef = 0, kPassRef = 1 };
    MfScHandleDict() : MfDictionary(0x35) {}
    virtual ~MfScHandleDict();
    MfScDataHandle *Find(const MfString &name);
    int Add(const MfString &name, MfScDataHandle *handle, HandleDictPassRef passRef);
    void Replace(const MfString &name, MfScDataHandle *handle, HandleDictPassRef passRef);
    void Clear();
};

// The variable pools visible to running code, innermost last.
class __declspec(dllimport) MfScVariableStack {
public:
    enum MfScVariableErrorCode { kVarOk = 0, kVarNotFound = 1, kVarReadOnly = 2 };
    MfScVariableStack(MfScHandleDict *pool, int size);
    virtual ~MfScVariableStack();
    void PushVarPool(MfScHandleDict *pool)
    {
        if (mCount < mSize - 1) {
            mPools[mCount] = pool;
            mCount++;
        }
    }
private:
    int mSize;                      // +0x04
    int mCount;                     // +0x08
    MfScHandleDict **mPools;        // +0x0c
};

class __declspec(dllimport) MfScVmEnv {
public:
    static MfScVariableStack::MfScVariableErrorCode SetGlobalVariable(const MfString &name, MfScDataHandle *handle,
                                                                     int, int);
};

// What a thread running scripts knows about its run.
struct MfScThreadContext {
    int m_00[12];
    MfScVariableStack *mVariables;  // +0x30
    int m_34[6];
    int mMessageLoop;               // +0x4c the script may run a message loop
    int mAbort;                     // +0x50 the run is to stop
};

// Work an MfThread does: DoWork runs on that thread.
class MfThreadWork {
public:
    virtual ~MfThreadWork() {}
    virtual void DoWork() = 0;
};

class __declspec(dllimport) MfThread {
public:
    MfThread(int flags);
    virtual ~MfThread();
    int AssignWork(MfThreadWork *work);
    void SignalShutdown(int);
    static MfThread *GetThread();
protected:
    virtual void ThreadEntry();
public:
    unsigned long mThreadId;        // +0x04
    int mFlags;                     // +0x08
    MfThread *mThread;              // +0x0c
    HANDLE mHandle;                 // +0x10
    unsigned int mState;            // +0x14
    void *mWork;                    // +0x18
    MfThreadWork *mWorkItem;        // +0x1c
};

// A thread that runs scripts (the vftable is emitted here).
class __declspec(dllimport) MfScThread : public MfThread {
public:
    MfScThread(int flags);
    virtual ~MfScThread();
    MfScThreadContext *mContext;    // +0x20
};

class __declspec(dllimport) MfScVirtualMachine {
public:
    MfScVirtualMachine();
    virtual ~MfScVirtualMachine();
    MfScDataHandle *ExecuteFunctionArrayParams(MfScDataHandle *function, int count, MfScDataHandle **params);
private:
    int m_04;
};

class MfAiPdbDebugger : public MfAi {
public:
    virtual int SignalBreakpoint(void *context, unsigned char *bytecode) = 0;
    virtual int SignalException(int code, const MfString &message, void *context) = 0;
    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual int IsActive() = 0;
    virtual void ProgramState(int state) = 0;
};

class MfExecWork;

class __declspec(dllimport) MfScDebugger {
    friend class MfExecWork;
private:
    static MfAiPdbDebugger *pdbgTheDebugger;
};

// A file of compiled functions (.pobj, 0x38 bytes).
class __declspec(dllimport) MfScPiccoloObjectFile {
public:
    enum MfScPOFErrorCode { kPOFOk = 0 };
    MfScPiccoloObjectFile() : m_1c(0), m_20(0), m_24(0), m_28(0), m_2c(0), m_30(0), m_34(0) {}
    MfScPiccoloObjectFile(const MfString &fileName, int write, int, unsigned int debug);
    virtual ~MfScPiccoloObjectFile();
    MfScPOFErrorCode Open(const MfString &fileName, int write, int, unsigned int debug);
    MfScPOFErrorCode GrabFunctions(MfScHandleList *functions);
    MfScPOFErrorCode AddFunction(MfScDataHandle *function);
    MfScPOFErrorCode ReadClose();
    MfScPOFErrorCode WriteClose();
    void DumpFile(MfStringList *lines, int what);
    static MfString ECToString(MfScPOFErrorCode error);
private:
    int m_04[5];
    MfString mFileName;             // +0x18
    int m_1c, m_20, m_24, m_28, m_2c, m_30, m_34;
};

class __declspec(dllimport) MfScPiccolo {
    friend class CConsoleFrame;
public:
    static void Initialize();
    static void SetAsMessageThread();
    static int InitializePlugins(MfString *errors);
    static void LoadPiccoloDataPathFunctions(int);
    static void LoadPiccoloLibFunctions(int);
    static void FlushPiccoloDataPathFunctions();
    static void Shutdown(MfString *errors);
private:
    static MfString fsDataPath;
};

// The compiler interface ("PiccoloCompilerI").
class MfAiCompiler : public MfAi {
public:
    virtual int Compile(const MfString &module, const MfString &type, const MfString &source,
                        MfPcErrorCallBack *errors, int flags) = 0;
    virtual int Flags() = 0;
    virtual int CodeType() const = 0;
    virtual void ClearCodeType() = 0;
    virtual MfScHandleList *Functions() = 0;
    virtual int IsSymbolGlobalPermanent(const MfString &name) const = 0;
    virtual void GetGlobalSymbolNames(MfStringList *names, int flags) const = 0;
    virtual void FlushGlobalTemps() = 0;
    virtual void *GetConstInitFunction() = 0;
    virtual int NumberOfErrors() const = 0;
    virtual int NumberOfWarnings() const = 0;
    virtual const MfString &ModuleName() const = 0;
    virtual const MfString &ModuleType() const = 0;
    virtual void UpdatePiccoloDataPathFiles(int force) = 0;
    virtual void UpdatePiccoloLibFiles(int force) = 0;
    virtual int PiccoloLookupKeyword(const MfString &word) = 0;
    virtual MfString ParseModuleNameFromErrorInfo(const MfString &info) = 0;
    virtual int ParseLineNumberFromErrorInfo(const MfString &info) = 0;
    bool CompiledOK() { return CodeType() != 1; }
};

class __declspec(dllimport) MfInterfaceRegistry {
public:
    static int GetInterface(const MfString &name, MfAi **ai);
};

class __declspec(dllimport) MfScDataObject {
public:
    static void DisplayClassInfo(CWnd *parent);
    static CWnd *CreateClassTree(CWnd *parent, int style);
};

// An editor window: tool bar, code editor and output windows (0x130 bytes).
class __declspec(dllexport) CPiccoloWnd : public CWnd, public MfPeCeDblClickHandler, public MfAiPeWindows,
                                          public MfPeCursorChanged {
public:
    CPiccoloWnd(int flags = 0x70, int paneSize = -1);
    virtual ~CPiccoloWnd();
    MfString GetModuleName() const { return mModuleName; }
    CConsoleEdit *EditorWnd() const { return mEditor; }
    CConsoleOutput *OutputWnd() const { return mOutput; }
    int GetPaneSize();
    void SetModuleName(const MfString &module);
    void SetFlags(int flags);
    void EraseCursorPosition(CDC *dc);
    int DrawCursorPosition(CDC *dc, int measure);
    int Compile(int debug, int flags);
    void SetFocusToEdit();
    void SetFocusToFindSelect();
    void SetSearchString();
    void AddCustomToolbar(MfPeMetaCustomToolbar *meta);
    void RemoveCustomToolbar(MfPeMetaCustomToolbar *meta);
    static MfAiPeWindows *LookupIWindows(MfPeEditorData *data);
    static void AddMetaToolbar(MfPeMetaCustomToolbar *meta);
    static int RemoveMetaToolbar(const MfString &name, MfDblListItem **position);
    static void ClearAllMetaToolbars();
    static void SetGlyphOnLineForModule(const MfString &module, int line, int glyph);
    static void UnsetGlyphOnLineForModule(const MfString &module, int line, int glyph);
    static void ClearDisplayedGlyphs(int glyph);
    // MfPeCeDblClickHandler
    virtual void HandleDblClick(const MfString &line);
    // MfAiPeWindows
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();
    virtual int QueryMainWindow(CWnd **wnd);
    virtual int QueryEditorWindow(CWnd **wnd);
    // MfPeCursorChanged
    virtual void CursorChanged(int column, int line);
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual BOOL IsFrameWnd() const;
    void InitTitleBarData();
    void AdjustWindows(int cx, int cy);
    int ArrangeCustomToolbars(CToolBar *after, int x, int y, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT type, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT flags, CPoint point);
    afx_msg LRESULT OnSettingChange(WPARAM wParam, LPARAM lParam);
    afx_msg void OnPiccoloCompile();
    afx_msg void OnPiccoloClasses();
    afx_msg void OnEditCopy();
    afx_msg void OnUpdateEditCopy(CCmdUI *cmdUI);
    afx_msg void OnEditCut();
    afx_msg void OnUpdateEditCut(CCmdUI *cmdUI);
    afx_msg void OnEditPaste();
    afx_msg void OnUpdateEditPaste(CCmdUI *cmdUI);
    afx_msg void OnEditRedo();
    afx_msg void OnUpdateEditRedo(CCmdUI *cmdUI);
    afx_msg void OnEditUndo();
    afx_msg void OnUpdateEditUndo(CCmdUI *cmdUI);
    afx_msg void OnFindNext();
    afx_msg void OnFindPrevious();
    afx_msg void OnToggleOverwrite();
    afx_msg void OnUpdateToggleOverwrite(CCmdUI *cmdUI);
    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR *nmhdr, LRESULT *result);
    afx_msg void OnCustomToolbarCommand(UINT id);
    afx_msg void OnCustomToolbarUpdate(CCmdUI *cmdUI);
    DECLARE_MESSAGE_MAP()

    static MfDblList listPiccoloWindows;
    static MfDblList listMetaToolbars;
    static MfDblList listWindowList;

public:
    CPiccoloSplitter *mSplitter;    // +0x4c
    CPiccoloToolBar *mToolBar;      // +0x50
    CFindSelectComb mFindSelect;    // +0x54
    CFont mFont;                    // +0x98
    MfPcsFindSelectEdit mFindEdit;  // +0xa0
    int mCursorColumn;              // +0xec
    int mCursorLine;                // +0xf0
    CConsoleEdit *mEditor;          // +0xf4
    CConsoleOutput *mOutput;        // +0xf8
    MfString mModuleName;           // +0xfc
    int mCompileResult;             // +0x100
    int mUpdatingCmdUI;             // +0x104 IsFrameWnd while the tool bar updates
    int mFlags;                     // +0x108
    int mTitleHeight;               // +0x10c
    int mToolBarHeight;             // +0x110
    int mPaneSize;                  // +0x114
    CFont *mTitleFont;              // +0x118
    int mTitleTruncated;            // +0x11c
    int mShowingTitle;              // +0x120
    MfToolHelp *mToolHelp;          // +0x124
    int mNextCommand;               // +0x128
    MfDblList *mCustomToolbars;     // +0x12c
};

class MfPiccoloConsoleFrame;

// Told when the console frame is destroyed.
class MfConsoleFrameEventCallback {
public:
    virtual void FrameDestroyed(MfPiccoloConsoleFrame *frame) = 0;
};

enum {
    ID_PCW_CLEAR = 0x9c5f,
    ID_PCW_TOGGLE_LOGGING = 0x9c60
};

// The frame window of the script console (0x174 bytes).
class __declspec(dllexport) MfPiccoloConsoleFrame : public CFrameWnd, public MfPeBuffModCallBack {
public:
    MfPiccoloConsoleFrame(CWnd *parent = 0, MfConsoleFrameEventCallback *callback = 0);
    virtual ~MfPiccoloConsoleFrame();
protected:
    virtual void BmcbNotify(int empty);
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnSize(UINT type, int cx, int cy);
    afx_msg void OnNcDestroy();
    afx_msg void OnPcwClear();
    afx_msg void OnUpdatePcwClear(CCmdUI *cmdUI);
    afx_msg void OnPcwToggleLogging();
    DECLARE_MESSAGE_MAP()

    int mEmpty;                     // +0xc4
    int mLogging;                   // +0xc8
    MfPiccoloConsoleWnd *mConsole;  // +0xcc
    CToolBar mToolBar;              // +0xd0
    MfConsoleFrameEventCallback *mCallback; // +0x170
};

// Asks for the parameters of a script run (0x64 bytes).
class MfPiccoloParametersDlg : public CDialog {
public:
    MfPiccoloParametersDlg(CWnd *parent);
protected:
    virtual void DoDataExchange(CDataExchange *dx);
    DECLARE_MESSAGE_MAP()
public:
    CString mParameters;            // +0x60
};

// A frame window editing one file (0xe4 bytes).
class __declspec(dllexport) CPiccoloFileEditor : public CFrameWnd, public MfPeBuffModCallBack {
public:
    CPiccoloFileEditor(const MfString &fileName, const MfString &baseTitle);
    virtual ~CPiccoloFileEditor();
    virtual void ReadCurrentFile(MfString *text);
    virtual void WriteCurrentFile(MfString *text);
    virtual void CreateFrameWindow(CWnd *parent);
    CConsoleEdit *EditorWnd();
    const MfString &FileName() const { return mFileName; }
    void SetBaseTitle(const MfString &title) { mBaseTitle = title; }
    void SetFileName(const MfString &fileName);
    void DisableConsole();
    void SetOutputWndHints(int hints);
    void SetEditorFlagHints(int hints);
    void UpdateWindowTitle();
    void OutputSwitchToWindow(int window);
    void OutputPrint(int window, const MfString &text, int);
    void OutputClear(int window);
protected:
    virtual void BmcbNotify(int modified);
    void StartUpdateTimer();
    void StopUpdateTimer();
    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnSize(UINT type, int cx, int cy);
    afx_msg void OnPfeClose();
    afx_msg void OnTimer(UINT id);
    afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()

    int mBufferModified;            // +0xc4
    int mTextModified;              // +0xc8
    CPiccoloWnd *mPiccoloWnd;       // +0xcc
    int mFlags;                     // +0xd0 editor hints 0x7, console 0x8, output hints 0x70
    int mPaneSize;                  // +0xd4
    MfString mFileName;             // +0xd8
    MfString mBaseTitle;            // +0xdc
    int mTimerId;                   // +0xe0
};

// ---- this DLL: the console frame ------------------------------------------------

// Commands of the console frame (resource.h).
#define ID_PICCOLO_BUILD                         0x8003
#define ID_DIAGNOSTICS_NO_LEVEL                  0x8008
#define ID_DIAGNOSTICS_LEVEL1                    0x8009
#define ID_DIAGNOSTICS_LEVEL2                    0x800a
#define ID_DIAGNOSTICS_LEVEL3                    0x800b
#define ID_DIAGNOSTICS_LEVEL4                    0x800c
#define ID_PICCOLO_RUN                           0x800d
#define ID_PICCOLO_STORE                         0x800e
#define ID_PICCOLO_RELEASE                       0x800f
#define ID_PICCOLO_DUMPPOOL                      0x8010
#define ID_PICCOLO_CLEARVARS                     0x8011
#define ID_PICCOLO_CLEAR_DIAG                    0x8012
#define ID_PICCOLO_STOP                          0x8014
#define ID_VIEW_OPTIONS                          0x8016
#define ID_FILE_SAVE_OBJECT_FILE                 0x9c41
#define ID_FILE_LOAD_OBJECT_FILE                 0x9c42
#define ID_FILE_MRU_POBJ1                        0x9c43
#define ID_FILE_MRU_POBJ2                        0x9c44
#define ID_FILE_MRU_POBJ3                        0x9c45
#define ID_FILE_MRU_POBJ4                        0x9c46
#define ID_FILE_MRU_POBJ5                        0x9c47
#define ID_FILE_MRU_POBJ6                        0x9c48
#define ID_VIEW_PW_TITLE_BAR                     0x9c4a
#define ID_VIEW_PW_TOOL_BAR                      0x9c4b
#define ID_VIEW_PICCOLO_CLASSES                  0x9c4c
#define ID_NEW_PICCOLO_WND                       0x9c4f
#define ID_PICCOLO_STRESS_TEST                   0x9c58
#define ID_VIEW_PICCOLO_WINDOW                   0x9c59
#define ID_ENTER_MESSAGE_LOOP                    0x9c5a
#define ID_EXIT_MESSAGE_LOOP                     0x9c5b
#define ID_BUFFER_EMPTY                          0x9c5d
#define ID_NEW_CONSOLE_WND                       0x9c5e
#define ID_BUILD_PARAMETERS                      0x9c61
#define ID_FILE_STARTUP                          0x9c63
#define ID_FILE_SHUTDOWN                         0x9c64
#define ID_PICCOLO_COMPILE_STRESS_TEST           0x9c65
#define ID_PICCOLO_COMPILE_END_STRESS_TEST       0x9c66
#define ID_PICCOLO_CLEAR_OUTPUT                  0x9c67
#define ID_PICCOLO_STORE_GLOBAL                  0x9c68
#define ID_LOAD_PICCOLO_FUNCTIONS                0x9c69
#define ID_FLUSH_PICCOLO_FUNCTIONS               0x9c6a
#define ID_PICCOLO_DUMP_MEMUSAGE                 0x9c6b
#define ID_FILE_STRIP_OBJECT_FILE                0x9c6c
#define ID_FILE_DUMP_OBJECT_FILE                 0x9c6d
#define ID_LOCK_GC                               0x9c6e
#define ID_UNLOCK_GC                             0x9c6f
#define ID_SAVE_TO_GLOBAL                        0x9c71
#define ID_SAVE_TO_PROJECT                       0x9c72
#define ID_OPEN_FROM_PROJECT                     0x9c75
#define ID_OPEN_FROM_GLOBAL                      0x9c76
#define ID_EDIT_FIND_FIRST                       0x9c78
#define ID_EDIT_FIND_NEXT                        0x9c7a
#define ID_RECOMPILE_PROJECT                     0x9c7b
#define ID_RECOMPILE_GLOBAL                      0x9c7c
#define ID_PICCOLO_DEFAULT                       0x9c7e
#define ID_PICCOLO_NO_FUNCTIONS                  0x9c7f
#define ID_PICCOLO_NEED_FUNCTIONS                0x9c80
#define ID_VIEW_ADVANCED_MENU                    0x9c81
#define ID_VIEW_BASIC_MENU                       0x9c82
#define WM_WORK_FINISHED (WM_USER + 1)
#define WM_EDITOR_HAS_FOCUS (WM_USER + 2)

// Runs a script function on the script thread of the console (0x2c bytes).
class MfExecWork : public MfThreadWork {
public:
    MfExecWork(CConsoleFrame *owner) : mFunction(0), mVariables(0), mContext(0), mBusy(0), mOwner(owner) {}
    virtual ~MfExecWork()
    {
        if (mFunction)
            mFunction->Release();
    }
    virtual void DoWork();
    void SetWork(MfScDataHandle *function, MfScVariableStack *variables, const MfString &parameters)
    {
        if (!mBusy) {
            if (mFunction)
                mFunction->Release();
            mFunction = function;
            if (function)
                function->AddRef();
            mVariables = variables;
            mParameters = parameters;
        }
    }
    void SetLoop(int loop)
    {
        mLoop = loop;
        mStop = 0;
    }

    MfString mParameters;           // +0x04
    MfScDataHandle *mFunction;      // +0x08
    MfScVariableStack *mVariables;  // +0x0c
    MfScVirtualMachine mMachine;    // +0x10
    MfScThreadContext *mContext;    // +0x18
    int mBusy;                      // +0x1c
    CConsoleFrame *mOwner;          // +0x20
    int mLoop;                      // +0x24 run again until stopped
    int mStop;                      // +0x28
};

// Compiles the text of the console over and over on a thread of its own (0x2c bytes).
class CCompilerStressTester : public MfThread, public MfThreadWork {
public:
    CCompilerStressTester() : MfThread(2), mStop(0) {}
    virtual void DoWork();

    MfString mSource;               // +0x24
    int mStop;                      // +0x28
};

// The options of the console (0x78 bytes).
class CConsoleOptions : public CDialog {
public:
    CConsoleOptions(int clearAfterStore, int storeAfterCompile, int autoSwitchToOutput, int writeSeparateDebug,
                    int pwTitleBar, int pwToolBar, CWnd *parent = NULL);

    //{{AFX_DATA(CConsoleOptions)
    int mClearAfterStore;           // +0x60
    int mStoreAfterCompile;         // +0x64
    int mAutoSwitchToOutput;        // +0x68
    int mWriteSeparateDebug;        // +0x6c
    int mPwTitleBar;                // +0x70
    int mPwToolBar;                 // +0x74
    //}}AFX_DATA

protected:
    virtual void DoDataExchange(CDataExchange *dx);
    DECLARE_MESSAGE_MAP()
};

// The Piccolo console: a file editor that compiles, stores and runs the
// scripts (0x270 bytes).
class __declspec(dllexport) CConsoleFrame : public CPiccoloFileEditor, public MfPeKbdHandler,
                                            public MfPeSetFocusHandler, public MfScHandleEnum {
public:
    CConsoleFrame(int, int resourceId);
    CConsoleFrame();
    virtual ~CConsoleFrame();
    static void Initialize();
    static void Shutdown();
    void SetupVariables();
    void DoCreateWindow(int);
    int OpenFile(const MfString &fileName, int reload);
    int SaveFile(const MfString &fileName);
    void OpenMRUFile(int index);
    int OpenPobjFile(const MfString &fileName);
    void OpenMRUPobj(int index);
    void DoClose();
    virtual BOOL OnCreateClient(LPCREATESTRUCT cs, CCreateContext *context);

    static CConsoleFrame *pcfMainFrame;

protected:
    int PiccoloWindowFlags()
    {
        int flags = 0;
        if (mPwTitleBar)
            flags = 1;
        if (mPwToolBar)
            flags |= 2;
        return flags;
    }
    int CanRunPiccolo();
    virtual void ReadCurrentFile(MfString *text);
    virtual void WriteCurrentFile(MfString *text);
    virtual void CreateFrameWindow(CWnd *parent);
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *result);
    // MfPeKbdHandler
    virtual int KbhKeyDown(int key, int flags, MfAiPiccoloEditor *editor);
    // MfPeSetFocusHandler
    virtual void ReceivedFocus(MfPeCodeEditor *editor);
    virtual void LostFocus(MfPeCodeEditor *editor);
    // MfScHandleEnum
    virtual int EnumHandle(MfScDataHandle *handle);

    afx_msg int OnCreate(LPCREATESTRUCT cs);
    afx_msg void OnPiccoloCompile();
    afx_msg void OnParentNotify(UINT message, LPARAM lParam);
    afx_msg void OnDiagnosticsLevel1();
    afx_msg void OnUpdateDiagnosticsLevel1(CCmdUI *cmdUI);
    afx_msg void OnDiagnosticsLevel2();
    afx_msg void OnUpdateDiagnosticsLevel2(CCmdUI *cmdUI);
    afx_msg void OnDiagnosticsLevel3();
    afx_msg void OnUpdateDiagnosticsLevel3(CCmdUI *cmdUI);
    afx_msg void OnDiagnosticsLevel4();
    afx_msg void OnUpdateDiagnosticsLevel4(CCmdUI *cmdUI);
    afx_msg void OnDiagnosticsNoLevel();
    afx_msg void OnUpdateDiagnosticsNoLevel(CCmdUI *cmdUI);
    afx_msg void OnPiccoloRun();
    afx_msg void OnUpdatePiccoloRun(CCmdUI *cmdUI);
    afx_msg void OnPiccoloStore();
    afx_msg void OnUpdatePiccoloStore(CCmdUI *cmdUI);
    afx_msg void OnPiccoloRelease();
    afx_msg void OnUpdatePiccoloRelease(CCmdUI *cmdUI);
    afx_msg void OnPiccoloDumppool();
    afx_msg void OnUpdatePiccoloDumppool(CCmdUI *cmdUI);
    afx_msg void OnPiccoloClearvars();
    afx_msg void OnPiccoloClearDiag();
    afx_msg void OnPiccoloStop();
    afx_msg void OnUpdatePiccoloStop(CCmdUI *cmdUI);
    afx_msg void OnUpdatePiccoloCompile(CCmdUI *cmdUI);
    afx_msg void OnClose();
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnFileMruFile1();
    afx_msg void OnFileMruFile2();
    afx_msg void OnFileMruFile3();
    afx_msg void OnFileMruFile4();
    afx_msg void OnFileMruFile5();
    afx_msg void OnFileMruFile6();
    afx_msg void OnUpdateFileMruFile1(CCmdUI *cmdUI);
    afx_msg void OnViewOptions();
    afx_msg void OnUpdateFileSave(CCmdUI *cmdUI);
    afx_msg void OnFileClose();
    afx_msg void OnFileSaveObjectFile();
    afx_msg void OnUpdateFileSaveObjectFile(CCmdUI *cmdUI);
    afx_msg void OnFileLoadObjectFile();
    afx_msg void OnFileMruPobj1();
    afx_msg void OnFileMruPobj2();
    afx_msg void OnFileMruPobj3();
    afx_msg void OnFileMruPobj4();
    afx_msg void OnFileMruPobj5();
    afx_msg void OnFileMruPobj6();
    afx_msg void OnUpdateFileMruPobj1(CCmdUI *cmdUI);
    afx_msg void OnSize(UINT type, int cx, int cy);
    afx_msg void OnViewPwTitleBar();
    afx_msg void OnUpdateViewPwTitleBar(CCmdUI *cmdUI);
    afx_msg void OnViewPwToolBar();
    afx_msg void OnUpdateViewPwToolBar(CCmdUI *cmdUI);
    afx_msg void OnViewPiccoloClasses();
    afx_msg void OnNewPiccoloWnd();
    afx_msg void OnTimer(UINT id);
    afx_msg void OnPiccoloStressTest();
    afx_msg void OnUpdatePiccoloStressTest(CCmdUI *cmdUI);
    afx_msg void OnViewPiccoloWindow();
    afx_msg void OnUpdateViewPiccoloWindow(CCmdUI *cmdUI);
    afx_msg void OnEnterMessageLoop();
    afx_msg void OnUpdateEnterMessageLoop(CCmdUI *cmdUI);
    afx_msg void OnExitMessageLoop();
    afx_msg void OnUpdateExitMessageLoop(CCmdUI *cmdUI);
    afx_msg void OnBufferEmpty();
    afx_msg void OnUpdateBufferEmpty(CCmdUI *cmdUI);
    afx_msg void OnNewConsoleWnd();
    afx_msg void OnEditCopy();
    afx_msg void OnUpdateEditCopy(CCmdUI *cmdUI);
    afx_msg void OnEditCut();
    afx_msg void OnUpdateEditCut(CCmdUI *cmdUI);
    afx_msg void OnEditPaste();
    afx_msg void OnUpdateEditPaste(CCmdUI *cmdUI);
    afx_msg void OnEditUndo();
    afx_msg void OnUpdateEditUndo(CCmdUI *cmdUI);
    afx_msg void OnEditRedo();
    afx_msg void OnUpdateEditRedo(CCmdUI *cmdUI);
    afx_msg void OnBuildParameters();
    afx_msg void OnFileShutdown();
    afx_msg void OnUpdateFileShutdown(CCmdUI *cmdUI);
    afx_msg void OnFileStartup();
    afx_msg void OnUpdateFileStartup(CCmdUI *cmdUI);
    afx_msg void OnPiccoloCompileStressTest();
    afx_msg void OnUpdatePiccoloCompileStressTest(CCmdUI *cmdUI);
    afx_msg void OnPiccoloCompileEndStressTest();
    afx_msg void OnUpdatePiccoloCompileEndStressTest(CCmdUI *cmdUI);
    afx_msg void OnPiccoloClearOutput();
    afx_msg void OnUpdatePiccoloClearDiag(CCmdUI *cmdUI);
    afx_msg void OnUpdatePiccoloClearOutput(CCmdUI *cmdUI);
    afx_msg void OnSetFocus(CWnd *oldWnd);
    afx_msg void OnPiccoloStoreGlobal();
    afx_msg void OnUpdatePiccoloStoreGlobal(CCmdUI *cmdUI);
    afx_msg void OnFlushPiccoloFunctions();
    afx_msg void OnLoadPiccoloFunctions();
    afx_msg void OnPiccoloDumpMemusage();
    afx_msg void OnFileStripObjectFile();
    afx_msg void OnFileDumpObjectFile();
    afx_msg void OnLockGc();
    afx_msg void OnUnlockGc();
    afx_msg void OnOpenFromGlobal();
    afx_msg void OnOpenFromProject();
    afx_msg void OnSaveToGlobal();
    afx_msg void OnUpdateSaveToGlobal(CCmdUI *cmdUI);
    afx_msg void OnSaveToProject();
    afx_msg void OnUpdateSaveToProject(CCmdUI *cmdUI);
    afx_msg void OnEditFindFirst();
    afx_msg void OnEditFindNext();
    afx_msg void OnRecompileProject();
    afx_msg void OnRecompileGlobal();
    afx_msg void OnPiccoloDefault();
    afx_msg void OnUpdatePiccoloDefault(CCmdUI *cmdUI);
    afx_msg void OnPiccoloNeedFunctions();
    afx_msg void OnUpdatePiccoloNeedFunctions(CCmdUI *cmdUI);
    afx_msg void OnPiccoloNoFunctions();
    afx_msg void OnUpdatePiccoloNoFunctions(CCmdUI *cmdUI);
    afx_msg void OnUpdatePiccoloRecompileGlobal(CCmdUI *cmdUI);
    afx_msg void OnUpdatePiccoloRecompileProject(CCmdUI *cmdUI);
    afx_msg void OnViewAdvancedMenu();
    afx_msg void OnViewBasicMenu();
    afx_msg LRESULT OnHandleWorkFinished(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnHandleEditorHasFocus(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

    CToolBar mToolBar;              // +0xf0
    CStatusBar mStatusBar;          // +0x190
    CRecentFileList *mSourceMRU;    // +0x210
    CRecentFileList *mObjectMRU;    // +0x214
    MfScHandleDict *mGlobalPool;    // +0x218 the stored functions and variables
    MfScHandleDict *mSecondPool;    // +0x21c
    MfScHandleDict *mThirdPool;     // +0x220
    MfScVariableStack *mVariables;  // +0x224
    MfScThread *mThread;            // +0x228
    MfExecWork *mExecWork;          // +0x22c
    MfString mParameters;           // +0x230
    MfFileTime mFileTime;           // +0x234
    CCompilerStressTester *mStressTester; // +0x23c
    int mClearAfterStore;           // +0x240
    int mStoreAfterCompile;         // +0x244
    int mAutoSwitchToOutput;        // +0x248
    int mWriteSeparateDebug;        // +0x24c
    int mPwTitleBar;                // +0x250
    int mPwToolBar;                 // +0x254
    int mShowPiccoloWnd;            // +0x258
    int mCompileFlags;              // +0x25c 0x40 no functions, 0x80 need functions
    int mInMessageLoop;             // +0x260
    int mCheckingFileTime;          // +0x264
    int mPiccoloRunning;            // +0x268
    UINT mResourceId;               // +0x26c
};

#endif
