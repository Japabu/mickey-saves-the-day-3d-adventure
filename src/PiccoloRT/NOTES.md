# PiccoloRT.dll notes (vc5 /O2 /GX /MD, no PDB)

Patterns verified by matches in this binary.

- **Overloaded virtuals are laid out in reverse declaration order** (VC5).
  `MfCallBack` declares `CallBack(1)`, `CallBack(2)`, `CallBack(3)`; its vtable is
  `??_E, CallBack(3), CallBack(2), CallBack(1)`. Same for the `MfDblList` overloads.
- **mfutil classes**: `MfDblList`, `MfDictionary`, `MfHashTable`, `MfStringList` are
  `__declspec(dllimport)` with inline constructors (`MfDblList() : pliFirst(0), ...`).
  `MfSortedList` is *not* imported as a class (only its members are), so PiccoloRT emits
  its vtable (0x1002063c). `MfCallBack` has an inline ctor/dtor: PiccoloRT loads
  `__imp_??_7MfCallBack@@6B@` itself.
- `MfDblList::GetFirst`/`GetNext` are inline-bodied virtuals; called on a static
  `MfDblList` object they are devirtualised and inlined. To keep a call virtual
  (as `MfScDiagnostics::AddOutput` has it) call through a reference.
- `MfString::Length()` is inline (`mfsr->m_length`, +8 of the body).
- Every source that includes `MfScDataHandle.h` has its own copy of the "undefined
  variable" marker `pdhUndefined` (a file static initialised to -1, in .data).
- Flags: `if (~flags & 1)` (not `!(flags & 1)`) produced the original
  `not eax; test al,1` in `MfScDataObject::MfScDataObject`.
- A by-value tag parameter (`LiiExemplar`) passed from a named local gives the
  original EH state numbering (`LiiExemplar lii; MfScClassInfo exemplar(lii);`).
- A struct built with its default ctor and then assigned field by field
  (`MfScDiagnosticInfo info; info.mLevel = level; info.mMessage = msg;`) gives one EH
  state; a converting ctor adds a second one.
- Register choice in very small functions can depend on unrelated content of the
  translation unit (e.g. how many inline dllexport functions it emits):
  `MfScDebugInfo::GetConstUsageInfo` flipped eax/edx when `MfScLineInfo.h` gained inline
  members. Check the neighbours of a shared header change with match-all.

- `MfScDataObject::mFlags` is a plain `unsigned int`: as a union with a bitfield struct
  the implicit copy constructors copy the bitfields one by one.

Tool issues (not source problems):
- Identical-COMDAT folding: when the original target of a vtable slot or an EH funclet
  call is an address with several names (empty `ret`/`ret 4` functions), match.py
  compares only the first name (`img.name()`), e.g. `??1LiiExemplar` vs
  `??1MfScBytecodes`, `MfListItemInterface::Delete` vs `MfScDiagnosticCallBack::CallBack`.

- `MfString result; if (x) { T *r = dynamic_cast<T *>(...); result = r->Name(); } return result;`
  (cast into a named local) gave the original register choice in
  MfScFunctionRegistryEntry::Name/ModuleName/ModuleType; the one-expression form
  swaps eax/ecx for the copy-constructor argument.
- A pointer released on two paths (`if (type && type->IsSameModule(...)) {...;
  type->Release(); return m; } if (type) type->Release();`) keeps the second
  null test the original has (MfScFunctionRegistry::FindModule).
- `return frame->mFrame != 0;` (re-read through the pointer just written) vs
  `return m_frame != 0;` decides the register in MfScVmEnv::FirstFrame.
- `if (stack->Get(...) != kVarOk) return Global(...); return kVarOk;` returns a
  constant 0 (xor eax), the `== kVarOk` form returns the call's eax.
- MfListItemInterface slot 2 is `Destroy(void *)` (exported overrides
  ?Destroy@MfScFunctionRegistryEntry / Module), now in MfListItemInterface.h.
- A class whose virtual destructor comes from its second base (MfScSourceResolver:
  MfAiSourceResolver, MfCallBack) compiles the destructor and ??_G with `this`
  pointing to that base (lea esi, [ecx-4]).
- MfScFunctionRegistryModule::FindFunction(int, ...) really stores
  `distance = first - distance` (a bug in the original), not `first - line`.

## Patterns (worker C, first PiccoloRT round)

- `_$E` names of static initialisers renumber whenever a file's includes
  change (a header change renumbers them in every file including it). Read the
  object's symbol list (`match.sh ... --sym <nonexistent>` prints the defined
  names in address order: entry, constructor, atexit registration,
  destructor per object) and rewrite the claims before the final check.
- A static object that is a class member (MfScPiccolo::fsDataPath,
  MfScVirtualMachine::csThreadBreakLock, MfScFileProvider::sFileName) gets a
  destructor with a guard bit; a file-static one (sPiccoloName, sWindowClass)
  does not.
- Script methods that check their arguments: the first check returns early,
  the success path sits inside `if (ok) { ...; return x; }`, the last error
  after it; `MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;`
  (or `MfScThread *thread = ...; thread->mVmEnv->...`) where the original calls
  GetThread before pushing the arguments, the one-expression form after.
- `MfString("nil")` passed to `CreateData(const MfString &, int)` uses the
  constructor's return value (eax); the implicit conversion reloads the
  address with lea.
- MfString's copy-on-write is inline (MfString.h): `CopyOnWrite(0)` with
  `IncrementCount`/`DecrementCount`, the inline MfStringRef constructors
  (MakeCopy and nNumPadChars imported, the vtable stored twice), and the inline
  ConvertToLower/ConvertToUpper/ReadFromFile/WriteToFile wrappers.
- MfStringList's constructor and destructor are inline (`{ Clear(); }`). The
  code of functions with a local MfStringList matches, but the unwind funclet
  that destroys its MfStringTokenizerCallBack base keeps its base-pointer
  temporary 4-12 bytes lower in the original frame (FlushPiccoloDataPathFunctions,
  Initialize, LoadPiccoloLibFunctions, InitializeExtensions, PMqueryPropertyNames):
  not found yet, left as TODO.
- MfThreadWork (vtable 0x10020884 emitted locally) is a header-only abstract
  class; MfThread's copy constructor and assignment are implicit (inline).
- MfScThread::ThreadEntry and MfScGcThread::ThreadEntry are identical and
  folded at 0x1000b880 (two claims).
- `dynamic_cast` to the same class (`dynamic_cast<MfScData *>(handle->GetData())`)
  still calls __RTDynamicCast in VC5 (MfScVmEnv::GetCurrentCallStack).
- Class-info items are added with `*info + new MfScCiProperty("value")`
  (operator+, ??H), not +=.
- A float copied through an inline accessor (`((MfScObjFloat *)d)->Value()`)
  goes through the FPU (fld/fstp); the member directly is a mov.
- The string object's case and whitespace methods (PMToLower 0x1000eae0 and
  its siblings) expand the same copy-on-write but call the imported
  MfStringRef copy constructor and keep a separate IncrementCount per branch,
  where MfScFileProvider's unit inlines the copy (as MfString.h does now). The
  same split exists in RuntimeLib (MfFiniteStateMachine). Not solved; those
  functions are still open.
- Overrides a class's vtable needs must all be declared even when defined in
  another file: a constructor's vtable reference is checked slot by slot
  (MfScObjString: GetProperty, SetProperty, GetPropTokenTable, the protected
  GetClassNameA).
- Round 2 patterns:
  - `new` of a dllimport class with virtual functions (MfThread in
    MfScDataHandle::operator new, MfStringRef in the copy-on-write) calls the
    imported constructor and then stores the local copy of the vtable.
  - An inline wrapper adds an inlining level that changes register choice:
    MfScDataHandle_2 calls `aphhp[i]->m_cs.Enter()` directly, while
    MfScHandleHeap::GrowHeap matches with its own inline Lock().
  - A compiler-generated virtual destructor of a class with a destructible
    member (MfScGCTimerWork, 0x10006190) does not store its own vptr; an
    explicit empty one does. An explicit empty destructor without members
    (MfScGCWork, 0x10006100) only stores the base vptr.
  - `int naps = 60; if (pause) naps = ...;` (not a ?:), and a loop
    `int start = naps; while (naps--) ... (start - naps) * 500` gives the
    strength-reduced induction variable of MfScGCWork::Work.
  - `int tries = 10; while (tries--) { if (...) break; Sleep(100); }` rotates
    to an initial counter of 9 (Shutdown).
  - A local returned uninitialized on one path stays in its stack slot
    (GarbageCollectNotContainer's `pause` is set only inside `if (collected > 0)`).
  - Statement order of struct field stores follows the source (OPENFILENAME in
    BrowseForFile); the dialog's file buffer is a 1-byte `char fileName[1]`
    placed unaligned next to the MfString temporaries.
  - HIWORD before LOWORD (`int notify = HIWORD(wParam); int id = LOWORD(wParam);`)
    for the hook's register order.
  - Message blocks for MfLogger::Log use named MfString locals (declared in the
    reverse of the order they are added) and `errors + a + b + ...` through the
    inline `MfStringList::operator+`, which calls Add as a virtual call.
  - Inline helpers of a dllexport class can be expanded in a caller while the
    exported out-of-line copy still exists (MfScPiccolo(), MfScPropTokenTable(),
    MfScPropTokenTable::Add(const MfString &, int)): their claims are now
    "(inline, header)".
  - Script-method tables: PICCOLO_METHOD / STRING_METHOD macros with named
    `fullName`/`propertyName` locals and the method's argument count.
  - `SetGlobalVariable("String", ...)` (implicit conversion) and
    `SetGlobalVariable(MfString("String"), ...)` get different temporary slots.
  - `MfString prefix = type.LeftString(5); if (prefix == "class") ...` (named)
    for TypeInfo's frame; typeid(int)/typeid(float)/typeid(*m_data) feed one
    type_info::name call.
  - `lines->MfDblList::GetLast(pos)` / `GetPrev(pos)` are inline (MfDblList.h)
    and expand to the pliLast / pliPrev walks of FormatException.
  - String-literal arguments to `const MfString &` parameters are implicit
    conversions (lea of the temporary, not the constructor's eax):
    `MfRegularExpression objectFiles(".*\\.pobj$")`, `GetInterface("PiccoloCompilerI", ...)`,
    `RetrieveDllNames(errors, "^psc.*\\.dll$", &names)`, `CaseCompare("PscClassInfo.dll")`,
    `entry->SetProperty(handle, "name", ...)`. Fixed the main code of
    LoadPiccoloModulesFromDirectory and PMgetFunctionsInDataPath (only the
    MfStringList unwind-funclet slot is left there).
  - UtilityApp (mfutil, dllimport, empty inline ctor/dtor) is declared in
    UtilityApp.h; `UtilityApp app;` before the list it fills (RetrieveDllNames).
  - Experiment (not applied, MfDblList.h belongs to another worker): an inline
    `MfDblList::GetNext` that reads the fields directly (`pli = pli->pliNext;
    return pli ? pli->pData : 0;`) loads the next item straight into the loop
    register as the original does (PMcallStack: 85 -> 4 differing bytes); all 783
    PiccoloRT claims still matched with it. GetFirst must keep `pli->GetData()`
    (the direct form changes the first load); making GetCurrent direct merges the
    two Delete branches of InitializePlugins.
  - The unwind-funclet slot of an inline ~MfStringList is not affected by
    declaring MfStringList's/MfStringTokenizerCallBack's constructors implicit
    or qualifying `MfStringList::Clear()`; making ~MfStringTokenizerCallBack
    out of line changes the frame. Still open.

## The interpreter (MfScVirtualMachine::ExecuteCode, 0x10016f90, still TODO)

- MfScVmEnv's state as ExecuteCode uses it (names now as in PscDebugger's
  header): +0x08 `unsigned char *m_ip`, +0x10 frame, +0x14 `m_constants`
  (the running routine's constant pool, was `m_args`), +0x18 `m_object`
  ("this"), +0x1c `m_function` (the two were swapped before; MfScVmFrame's
  mObject/mFunction likewise). FormatException takes both the name and the
  routine from frame.mFunction (+8).
- VC5 stops inlining in a function this big: the later handlers call
  out-of-line copies of the inline members (GetType, GetInt = the folded
  GetCodeSize, SetType, PushDataPassRef, Release, ...) and the file's own
  inline helpers, emitted right after the function (0x1001b230-0x1001b2c3,
  claimed: Fetch16, Local, StackItem, Constant, HasType, ThreadManager), and
  dllimport inline members through the import (MfStringList ctor/dtor,
  MfString::Length). Not reproduced: where the original gives up (CMPGTE) and
  why it calls Release out of line in POP_FP, ADDSP and POP_REG/CLEAR_REG
  (inside the inlined PopAndReleaseData / ClearScratchREGn) but inlines it in
  the jumps and the arithmetic.
- `unsigned char low = *env->m_ip++; int high = *(char *)env->m_ip++;
  return high << 8 | low;` matches the out-of-line Fetch16; the one-expression
  form stores m_ip after the shift.
- MfScVmEnv has more exported inline members than the header had:
  GetSPHandle/SetSPHandle, ClearScratchREG0-3, ClearScratchRegisters,
  SetVariableStack, Start/RefreshProcessTimer, PushDataPassRef,
  PopDataPassRef, PushDataNoCheck, PopAndReleaseDataNoCheck (now declared).

## MfScPiccoloObjectFile writer (WriteClose, ComputeFunctionSectionSize, DumpFile)

- The original MfScPiccoloObjectFile.cpp calls `MfDblList::GetFirst/GetNext` qualified
  on a handle list (`list->MfDblList::GetFirst(list->CurrentPos())`) through the
  imports, i.e. without the inline bodies our MfDblList.h has (FormatException
  does inline them). Not solvable without a header split; WriteClose and
  ComputeFunctionSectionSize stay TODO for this.
- `MfScConstUsageInfo usage(info.Name(), info.Value())` locals: the original
  inlines the destructor (vptr store, ~MfVariant, ~MfString) in WriteClose,
  ComputeFunctionSectionSize and GetDebugInfo(unsigned); ours calls ??1. An inline
  `virtual ~MfScConstUsageInfo() {}` in MfScConstUsageInfo.h (and the 0x10008ab0
  claim in MfScDebugInfo.cpp becoming "(inline, header)") is the likely fix; untried.
- Small fixed-size fills are memset intrinsics: `memset(&header[5], 0xff, 16)`
  gives `or edx,-1` + four movs, `memset(&info[7], 0xb0, 40)` the `rep stosd`.
- `MfTime now; now.Update();` (two inline ftime expansions, the MfTime gets
  an EH state) for the object file time stamp; `MfTime stamp(t, 0)` as a named
  local (not a temporary) for DumpFile's FormatTime. PiccoloRT/MfTime.h holds
  the inline part of mfutil's MfTime.
- `lines->Add(MfString(""))` (explicit temporary) passes the constructor's
  eax, as noted for CreateData.
- MfScCodeLevelInfo's `operator=` returns by value and is called out of line;
  the writer copies the two fields by hand.
- Round 3:
  - `~MfScConstUsageInfo` is inline in the header (claim "(inline, header)");
    with it GetDebugInfo(unsigned) matches.
  - The inline `MfDblList::GetNext` reads `pli->pliNext` / `pli->pData`
    directly (GetFirst keeps the accessor); all claims still match.
  - GarbageCollectContainer: `int suspended; int pause; pause = 0; suspended = 0;`
    sets the slot order; the scan loop decrements before the time-out test, and
    `handle = last; heap->m_free = 0;` precede the rebuild loop.
  - Function-local `static MfString line(...)` objects get their own atexit
    destructors (_$E, 0x10006780/0x10006d40/0x100070b0).
  - Several older TODOs (the CreateData family, debug-info and class-info
    ICF/extent cases) match with the current tools and headers and are claimed.
- InfoLong/InfoShort read the boxed values as members (`(int)m_long`,
  `((MfScObjInteger *)m_data)->mValue`): the accessors change the registers. The
  boxed float still comes out as a dword copy, where the original pushes it with
  fld/fstp (EQUIV reviewed).
