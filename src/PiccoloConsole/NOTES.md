# PiccoloConsole.dll (vc5 /O2 /GX /MD /GR /D_AFXDLL, MFC extension DLL, no PDB)

All units include `PiccoloConsole.h`, the one header of the DLL: the classes of
this DLL and what it uses from mfutil.dll, PiccoloRT.dll and MFC. Names come
from the exports and the RTTI (`.?AV...` behind the complete object locator at
vtable-4: also nested classes such as `GlyphModule@MfPeEditorData`); message
maps give the handlers of the command ids (the console frame's ids are
`#define`s, as in a resource.h; an enum of them shifted the optimizer's hash
order in other units). The console frame is three objects: `CConsoleEdit.cpp`
(it also gets the exported inlines of the header), `CConsoleFrame.cpp` (with
MfExecWork and CCompilerStressTester) and `CConsoleOptions.cpp`. The syntax
colouring's flex scanner is `pelex.cpp`, the last object of the editor group
(a lower-case name sorts after `MfPeUndentRegionCommand.cpp`): flex 2.4
skeleton as in PiccoloComp's MfPcLexer.cpp, tables copied from the binary,
rules found by simulating the DFA.

## Layout of the binary

- One source file per class, linked in alphabetical order (upper-case `C*`
  before `M*`; `MfPeLineSyntax.cpp` sits between `MfPeJoinLineCommand.cpp` and
  `MfPeMetaCustomToolbar.cpp`). `PiccoloConsole.cpp` holds the AppWizard
  `DllMain` (`#include <afxdllx.h>` gives `ExtRawDllMain`).
- The `$E` initializers of a unit's statics come first: for a static data
  member four functions (call pair, constructor, `atexit` registration,
  guarded destructor), for a file static the destructor has no guard. Their
  numbers change with every header edit: re-read them from the object (the
  list of defined symbols of match.sh) after the last header change.
- COMDATs (inline virtual destructors, `??_G`, adjustor thunks
  `??_E...@@WBA@AEPAXI@Z`, implicit dialog destructors, template members)
  follow the function that first needs them; claim them as comment lines at
  that place. Identical functions are folded (`AddRef`/`Release` returning
  1, the trivial state `??_G`, `MfPeCommandList::Clear`/`MfPeTextLineList`'s).

## Compiler behaviour that decided matches

- **VC5's inline budget**: whether a nested inline is expanded depends on how
  many inline expansions the function already has. Using the accessors
  (`MfStringCursor::CharPos()` instead of the field) in `SplitLine` stopped
  the expansion of `MfArray`'s constructor exactly as in the original; the
  `MfPeLineSyntax` level between `MfPeTextLine` and `MfArray` is needed for
  the same reason (flattening it made more callers inline the array). The same
  budget explains the three shapes of `MfString::CopyOnWrite(0)` (call,
  inline with `MfStringRef(int)` call, fully inline) in one header.
- Statement order is source order: `columns += 1 - left; mLeftColumn = 1;`
  vs the other order decided OnHScroll; switch cases are laid out in source
  order (SB_TOP/SB_BOTTOM came first in OnVScroll).
- `~flags & BIT` (not/test) and `!(flags & BIT)` (test) compile differently.
- A `next` variable copied into `line` after the null test (`mov esi, ecx`)
  is a separate source variable whose value is still used (Print).
- `MfDblList::First(item)` with `item` a member writes through the reference,
  so a following read of another member is not folded (GotoTextStart).
- `context->ScrollTo(WM_VSCROLL, y)` (inline with an `int` parameter) masks
  the position with `and 0xffff`; `MAKEWPARAM` on the expression does 16-bit
  arithmetic instead.
- Command and state classes need explicit empty virtual destructors where the
  original stores their own vtables in the destructor (all commands,
  `MfPeDeleteRegionState`, `GlyphInfo`); the implicit ones skip the store.
- `MfPeMarker` constructors take `int` (the column is computed in 32 bits and
  stored as a byte).
- The scripts' editor functions take `MfPeEditorState *` and
  `dynamic_cast` to their state (`__RTDynamicCast` with the source type
  MfPeEditorState).
- Register choice of commutative operands (`a + b` vs `b + a`) flips with
  unrelated changes of the unit (hash order of the optimizer): such claims are
  TODOs with that reason; re-run the flip script after the last header change.

- An implicit conversion to a `const MfString &` parameter
  (`OutputPrint(2, "", 1)`, `SetEntireContents("", 0)`) passes the temporary
  by `lea` of its slot; an explicit `MfString("")` pushes the constructor's
  `eax`. A named local in a nested block can take the slot of a dead
  argument (`MfString word(text)` in pelex's IsKeyword).
- Copies of an inline's result into a second register after the null test
  (`test ecx, ecx / mov edi, ecx`) come from a local in the inline:
  `MfScHandleList::First/Next` and `MfPeEditorData::LineOf` are written
  `T *x = p ? ... : 0; return x;`.
- `mCursor = CPoint(column, line)` (not two member stores) makes the following
  `mCursor.x - 1` read memory again (GotoSelectionStart); locals copied up
  front (`int startColumn = mSelection.mStart.x`) load all four coordinates
  first (IsSelectionPastLineEnd).
- Two stores through one inline (`mExecWork->SetLoop(0)`) share one load of
  the pointer; separate statements reload it.
- In a `&&` chain the order of the zero-initialised locals decides which one
  reuses the zero register (`int functions = 0; int codeType = 0;`).
- Implicit destructors do not store the class's own vtables first (the
  stress tester's): leave them out instead of writing `{}`.

## Tool notes

- A compile error makes check.sh very slow: test one function with match.sh
  first.
- `inlining LiiExemplar's constructor` in `~MfPeMetaCustomToolbar` crashes the
  compiler (C1001); the exemplar is a named local there.
