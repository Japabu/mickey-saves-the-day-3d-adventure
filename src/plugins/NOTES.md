# Piccolo plugins (vc5 /O2 /GX /MD /GR, no PDB)

Patterns verified by matches in the plugin DLLs.

- **MfString temporaries**: `a + MfString("x")` and `a + "x"` (implicit
  conversion) compile differently: the explicit temporary is passed straight
  from the constructor's return register (`push eax`), the implicit one by
  `lea` of its slot, and the slots are allocated in another order. Pick the
  form by the push. `GetVariable("MFDATAPATH")` was implicit,
  `m_dataPath + MfString("\\Actors")` explicit.
- **Log messages**: `MfLogger::Log(level, module, function, msg)` is always
  preceded by two named locals constructed function first, module second
  (`MfString function("Content"); MfString module("Content Info");`) in their
  own block; the statement after the log (e.g. `m_dataPath = ".";`) is outside
  that block.
- `MfString` needs `operator<<(int)` declared, else `<< 0` binds to
  `operator<<(const char *)` (null pointer) and `<< 1` to `MfString(int)`.
- A redundant `else return 0; return 0;` after a branch that returns adds an
  unreachable inline-destructor expansion: no code, but extra EH states (the
  FuncInfo maxState differs while the code is identical).
- dllimport classes with inline empty constructor/destructor (`UtilityApp`,
  `MfTime`, `PcContentArg`): the normal path has no code, the unwind funclet
  calls the imported destructor.
- `new ImportedClass(...)` (MfScClassInfo, MfScCiMethod, MfScDataObject):
  after the imported constructor the plugin stores its local copy of the
  vtable; the matching `??_G` is emitted locally.
- `*ci + &(*new MfScCiMethod("M") + new MfScMethodParameterInfo("P", 0));`
  as one expression: the parameter object is created first and the result of
  the inner `operator+` is pushed directly.
- MfString's inline `MakeUnique()` (null body -> `new MfStringRef(0)`,
  shared -> `CopyOnWrite()`), with `NoLeadingWhiteSpace`,
  `NoTrailingWhiteSpace`, `Truncate` inline on top of it, matches the
  PscContentInfo reader.
- Iterating an MfStringList member: the non-virtual inline `First/Next`
  (MfStringList) keep `pos` in a register; `GetFirst/GetNext` (inline
  virtuals of MfDblList) give the `mov reg, eax` after the null test that
  PcContent::GetContentNames has.
- Pointers to the string of a script argument, `(MfScObjString *)h->GetData()`
  converted to `MfString *` produce the `p ? p + 0x14 : 0` sequence; convert at
  the call (`*(MfString *)p`) when the original evaluates the conversions in
  argument order.

- A folded function sits where its first definition was compiled: the
  `return 1` at PscClassInfo 0x10001470 (also `_PscUnload@0`) is
  `CClassTree::AddRef`, defined out of line between GetInterfaceName and
  InsertClass (inline in the class it would follow the constructor).

Tool notes: `_$E<n>` names of static initializers change when the header
changes (they are numbered per object); re-read them from match.sh's list of
defined symbols after editing a header.

PscStdlib (own header PscStdlib.h, one file per script class):

- **Class objects**: each class publishes one instance built by a constructor
  taking an empty by-value `MfPlClassArg` (its out-of-line `??1` is emitted once
  and used by the unwind funclets). The name, the class object and the shutdown
  flag are file statics (`static MfString fsName("List")`): a static *class
  member* of type MfString gets a guarded `$E` destructor instead.
- **Registration constructors**: `SetProperty(0, name, CreateData(method, full,
  n), 1)` in a block with the named locals `full` then `name`; class info as
  `MfScCiMethod *method = new MfScCiMethod("X"); *ci + method; *method + new
  MfScMethodParameterInfo("p", 0);` (method allocated before its parameters).
  `MfScCiProperty`, `MfScPropTokenTable` (constructor and `Add`) and
  `MfScClassInfoItem`'s constructor are inline.
- **Script exceptions** come in two shapes: `MfScVmEnv *env = thread->mVmEnv;
  env->ThrowException(n, "...")` (environment loaded before the message,
  Math) and an inline helper `ThrowScriptException(n, "...")` whose body is
  `MfScThread *thread = (MfScThread *)MfThread::GetThread();
  thread->mVmEnv->ThrowException(code, message);` (message built first,
  List/Dictionary).
- `if (!a || !b) throw-nil; else if (type) ...; else throw-type;` is laid out
  with the nil throw last but numbered first (EH state 0).
- `MfScObject` has no declared destructor in the plugin header: the derived
  destructors jump to `~MfScDataObject` without storing MfScObject's vtable.
- A loop over `argv` that tests the type and takes the data of one handle
  caches the flags only with a local `MfScDataHandle *h = argv[i]`; a further
  local `MfString *s` shifts the unwind funclet's temporary slot as in the
  original (MfPlLogger::Log).

PscDebugger (MFC extension DLL, own header PscDebugger.h, one file per class):

- Class names come from the RTTI of the vtables (`.?AV...` behind the
  complete object locator at vtable-4); message maps (AFX_MSGMAP at the
  address GetMessageMap returns) give the handler of each command id.
- Identical functions of different units were folded by the linker
  (_GetBaseMessageMap of every CDialog map at 0x1000b5a0, OnGetDlgCode,
  MfPdbFlatModuleList::FillTree/OnCreate into the tree's): write the
  source in every class, claim only the surviving copy.
- A single exit beats early returns: `int found = 1; if (...) {...} else {...}
  return found;` gave the original stack reload of the result, `return 1`
  in a branch did not (MfPdbDebugger::GetVariable); likewise
  `int handled = 0; switch ... return handled;` (KbhKeyDown, switch cases in
  source order = code order).
- `x->Method(this)` evaluates `this`'s base conversion before the object
  expression; the original calls `mFrame->EditorWnd()` first, i.e. a local
  `CConsoleEdit *editor = mFrame->EditorWnd(); editor->Install...(this);`.
- `*toolbar + new A + new B ...` (an inline `operator+` calling Add) gives
  distinct temporary slots and creation in reverse order of the Adds.
- Error branches that build a message: put them in `else { MfString msg; ... }`
  so the message dies before the following `source->Release()`.
- `if (!p) p = GetFirst(); if (!p) return;` (two tests) vs nested ifs decides
  the jump target after the first test (OnKeyDown, OnSetFocus).
- Inline MFC helpers decide the code: CListBox::GetCurSel/GetItemDataPtr,
  CTreeCtrl::InsertItem(text, parent, after) and SetItemData,
  CBitmap::LoadBitmap, CWnd::GetSafeHwnd, CEdit::SetSel, CRect(&rect);
  CDC::SelectStockObject is NOT inline in VC5 MFC (write
  GetStockObject/::SelectObject), CRect::SetRect calls ::SetRect (the
  original stores the fields).
- MfHPTimer's Start/Stop/AccumTimer must be inline to get the original
  register use (MfPdbVmListDlg::OnGetDispInfo).
- MfScVmEnv needs `#pragma pack(4)` in the plugin header: its 64-bit timer
  fields would otherwise move m_4 to +8.
- Reusing one CRect for a second purpose (HitTest's glyph rectangle) and
  block-scoped CRects (DrawItem) reproduce the original frame size.

- A list position inline whose result goes through a local
  (`T *x = pos ? (T *)pos->pData : 0; return x;`) makes VC5 test a copy of
  the result in a second register; the original MfDirectory::First
  (PscContentInfo) and MfScHandleList::First/Next (PscDebugger) are written
  that way (ScanFolder, LoadModule).

- A call argument `x == C ? 0 : x` compiles to neg/sbb; the original's
  `cmp; push 0; je; push x; jmp; push 0` comes from an if/else of two calls
  (`if (i == LB_ERR) f(..., 0, 0); else f(..., i, 0);`) whose shared tail the
  compiler merges (PscContentInfo ChooseContentProc).
