# mfutil: source patterns that match (VC5 /O2 /GX /MD /GR)

Every item was verified by match.sh in the unit named in brackets.

## Shared headers
- One declaration per class: `<Class>.h` (MfString.h, MfStringRef.h, MfDblList.h,
  MfCallBack.h, MfHashTable.h, ...), each self-contained, exports through
  `MFUTIL_API` (mfutil_api.h). `MfString.h` defines STRICT before windows.h, includes
  the small string classes (MfNullString, MfStringSize, MfStringCursor,
  MfStringScanner, MfStringRef, MfStringTokenizerCallBack) and, after MfString,
  MfVariant.h, MfTime.h and MfSystemTime.h.
- Inline bodies are the ones the original objects emitted (MfCopyFolder.cpp keeps
  the copies of the string classes' inline members, MfStringRef.cpp those of
  MfVariantData/MfVariant, MfDirectory.cpp those of MfTime/MfSystemTime/MfFileTime).
- Own copies: VC5 compiles some functions differently once a file sees the shared
  headers, even where the text is the same. MfCopyFolder.cpp, MfHashTable.cpp,
  MfStringRef.cpp (with MfVariant_unit.h) and MfTime.cpp therefore keep the
  declarations they were matched with (`*_unit.h`, each marked `// own copy`).
- `MfString::LoadString` takes `void *`: MfStringRef.cpp was compiled without STRICT
  (`?LoadStringA@MfString@@QAEHPAXI@Z`).

## Code shapes
- Local char arrays get stack slots by size: the smaller array sits lower
  (`char digits[14]` below `char key[16]`) [MfKeyManager::GenerateKey].
- Unsigned date compares (`cmp; sbb; inc`) come from `unsigned long` locals
  [MfKeyManager::DateIsValid].
- `MfString value = var.Get();` constructs in place; `MfString value = var;` through
  a conversion operator copies a temporary [UtilityApp::GetProfileInt].
- A named `MfString value(n); var.Set(value);` gives the original slot order where
  `var.Set(MfString(n))` does not [UtilityApp::WriteProfileInt].
- MfVariant: the MfVd* data classes are ordinary (not inline) member functions in
  source order; the linker folded identical ones (??_G, Assign, conversions) into
  a later class's copy [MfVariant].
- Commutative x87 operand order (mflinalg) depends on what the translation unit
  compiled before the function: the same function alone in a file can match while
  it does not in the full file. Bisect with the preceding functions to find which
  ones flip it (MfVector3 0x10005cc0 is flipped by 0x10005c80 alone).

## Wave 6 (MfLogger, MfLoggerUI)
- MfLogger.h must include MfString.h before anything that pulls in windows.h
  (MfString.h defines STRICT; without it HWND__ does not exist) [MfLogger].
- The logger is two objects: MfLogger.cpp (MfLogger, MfLogMessage, the UI thread
  MfLoggerFrameUI) and MfLoggerUI.cpp (MfLoggerFrame, MfLoggerWindow,
  MfLogMessageDialog, MfLoggerOptions); MfLoggerUI.h has the profile keys as
  `static char *` (each object has its own table in .data).
- Nested `if (a) { ... if (b) { ... } }` instead of early returns lets the
  temporaries of the first part share stack slots with later locals [Shutdown].
- `if (p) p->close(); delete p; p = 0;` reloads p for the delete [Shutdown].
- `if (!pending && (a != b || (... && c != d))) pending = 1;` gives the single
  store the original has; nested ifs store twice through a register [Filter].
- Win32 handles are fetched in their own statement before the call that uses
  them: `HWND h = GetDlgItem(...); SendMessage(h, ...)`; `if (x) Send(h, ..., 1, 0);
  else Send(h, ..., 0, 0);` gives the branchy push of 1/0 (a ?: gives setne)
  [MfLoggerOptions].
- Window and dialog procedures: one `result` variable returned at the end, the
  WM_CREATE/WM_INITDIALOG case in an if/else, the other messages in a switch;
  WM_DESTROY falls through to `default:` (DefWindowProc) [MfLoggerFrame::WndProc].
- `InsertColumn(0, "Date", ...)` (implicit MfString temporary) recomputes the
  temporary's address; `MfString("Date")` reuses the constructor's return value.
- A member function that only calls another (`void OnClear() { MfLogger::ClearWindow(); }`)
  compiles to a 5-byte jmp; the verifier resolves calls to it to the jump target's
  name, so callers stay TODO (tool).
- Struct-with-string varargs: Log(int, src, msg, int, ...) ignores the int and
  takes MfString pointers from the varargs.

## Wave 7 (regexpr.cpp, MfLogger::DoWork)
- 0x10012820-0x1001492c is Tatu Ylonen's regexpr.c (Python's January 1992 copy),
  compiled as C++ (regexpr.cpp): the ctype calls go through the CRT imports, which
  the C macros of VC5's ctype.h would inline. No asserts, re_match/re_comp/re_exec
  removed, an RE_IGNORE_CASE syntax bit added (0x100) [regexpr].
- VC5 emits a static function after the first function that calls it, not at its
  definition (re_compile_initialize after re_set_syntax) [regexpr].
- Register/addressing tie-breaks (base vs index in `p[i]`, commutative x87
  operands, which value stays in memory) depend on how many symbols the
  translation unit declared before the function: deleting the assert lines,
  dropping a prototype from regexpr.h or adding one std header flips them.
  Comments and identifier names do not. When code is right but registers are
  not, try include variants (a std header the unit plausibly used) with a script
  that rewrites the include block and runs check.sh, TODO lines turned into claims
  [regexpr re_search_2, MfActorLib SetHomeTransform, MfJointKeyframe].
- A local initialized to 0 shares the stack slot the original gives it only if
  declared where the original initializes it (`pliNew = 0` before the lock,
  `bShown = 0` after it) [MfLogger::DoWork].

## Wave 8 (completion pass: every function has a body)
- `MfEnvVariable.h` is the shared declaration (ctor, dtor, Set and SetValueDirect
  inline in the class; MfEnvVariable.cpp claims their exported copies as
  "(inline, header)"); MfRegVariable.h includes it. MfEnvVariable_unit.h is gone.
- A value that the original keeps in a register across `operator new` is a local:
  `MfStringRef *body = mfsr; ... new MfStringRef(*body)` (with `new MfStringRef(*mfsr)`
  VC5 reloads this->mfsr after the call) [MfString::Tokenize, ConvertToLines].
- Commutative operand order (`imul eax, [mem]` operands) and some temp registers
  flip when the unit declares one more std header before its own: `#include <stdlib.h>`
  before the `_unit.h` matched MfIndex::Search and MfDictionary::Search (the latter
  also needs `ppBucket++, (*piSlot)++` in the for-increment); `<stdio.h>` (only that
  one) took MfStringRef::ReplaceString from 52% to 97%. A loop script that tries
  `stdlib/string/stdio/windows/iostream/ctype/time.h` per TODO is cheap; most TODOs
  did not move.
- `name + "_ORDER"` is ambiguous (MfString has conversions to char * and the
  global operators); write `name + MfString("_ORDER")` [GetVariable].
- MfString::GetVariable inlines MfEnvVariable's ctor/dtor (needs the inline header)
  and builds the MfStringList on the stack (vtables stored inline, ~MfStringList
  calls Clear then ~MfDblList).
- VC5 inline decisions for MfString::CopyOnWrite / MfStringRef::CopyOnWrite /
  MfStringRef(int) are still not understood: the original inlines them in
  ReverseCaseContainsSubString (we match), inlines only the outer one in
  CaseContainsSubString and none in GetVariable, while our compile inlines all three.
  Padding the function with no-op statements does not change our choice (so it is
  not a plain size budget).
- match.sh now matches functions whose only difference was the C-underscore import
  name (`_stricmp`, `_beginthread`, `_endthread`, `_mtlock`): CaseCompare,
  operator>>, MfThread ctor and ThreadInit are claims now.

## Review r12b
- `if (msg == WM_COMMAND && (LOWORD(wParam) == IDOK || ...)) {...} else if (msg == EN_CHANGE)`
  (one if/else chain) gives MfNameDialog::DialogProc's layout: the IDOK path returns
  directly, the other WM_COMMAND path reloads msg for the EN_CHANGE test.
