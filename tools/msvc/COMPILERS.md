# Compilers and measurements

Goal: C/C++ source for every game function that the original Microsoft compilers
turn into byte-identical code, verified one function at a time by
`tools/msvc/match.sh` against the original binaries in `orig/binaries/`.

Motion Factory DLLs (RuntimeLib, MfActorLib, MfPoLibrary, MfImageLib,
PiccoloRT/Comp/Console, mflinalg, mfutil) were linked with VC5 (linker 5.10), the
game (Steamboat.exe, SteamboatApp, MBEngine, MBCom, mbruntime) with VC6; PDBs exist
for the VC6 ones. Third-party libraries (Mss32 = Miles, binkw32, SMACKW32, dbghelp,
MSVCRT, msvcirt, INETWH32) are not decompiled.

Function boundaries come from `tools/recomp/discover.py` (recursive descent from
entry, exports, PDB symbols, relocation targets, jump tables, SEH/C++ EH tables).
Steamboat.exe has no relocations; its address fields are inferred by `infer.py`.

## Original compilers (tools/msvc)

The toolchains go in `tools/msvc/vc5` and `tools/msvc/vc6` (VC++ 5.0 + VS97 SP3 and
VC++ 6.0 + VS6 SP3, matching the Rich headers). They are not in the repository: import
your own with `tools/import.py`, which checks every file against
`tools/manifest/vc5.sha256` and `vc6.sha256`. `tools/msvc/cl.sh <vc5|vc6|vc6-rtm> [cl|link|lib|dumpbin] args`
runs them under Wine (32-bit) with INCLUDE/LIB set.
`vc6-rtm` runs from a copy of `vc6/bin` with the RTM front ends swapped in
(`out/msvc/vc6-rtm/bin`), because cl.exe loads c1/c1xx from its own directory
before PATH; `cl /Bv` confirms which passes are loaded.

| setup | cl | c1 | c1xx | c2 | link |
|---|---|---|---|---|---|
| vc5 | 11.00.7022 | 11.00.7113 | 11.00.7303 | 11.00.7303 | 5.10.7303 |
| vc6 | 12.00.8168 | 12.00.8472 | 12.00.8472 | 12.00.8168 | 6.00.8447 |
| vc6-rtm | 12.00.8168 | 12.00.8168 | 12.00.8168 | 12.00.8168 | 6.00.8447 |

Game Rich headers: Steamboat modules C/C++ objects 8168, link 6.00.8447 (SteamboatApp,
MBEngine) / 8168; Motion Factory modules link 5.10.7303 (VC5 objects carry no stamp).

Smoke tests (hello world, Wine 9.0): vc5, vc6 and vc6-rtm compile, link and run
(`hello from 1100` / `1200`). vc5 `link.exe` needs MSVCP50.DLL (via MSDIS100.DLL);
`tools/msvc/vc5/bin/msvcp50.dll` 5.00.7022 from the VC5 disc provides it.

The VC98 include directory on the disc stores six C++ headers under 8.3 names
(`algrithm`, `fctional`, `stdxcept`, `streambf`, `strstrem`, `xception`), which VC6
setup renames on install; `assemble.py` now does the same (without it `<map>` does
not compile).

## Matching decompilation

Goal: C/C++ source that the original compiler turns into the original bytes, one
function at a time. Sources live in `src/<dll>/`, each matching function claimed by a
`// 0x<va> <symbol>` line above it; `src/options.txt` gives each project's options.

Tools (`tools/msvc/`):
- `match.sh <dll> <va> <file.c|cpp> [--sym NAME] [--cc vc5|vc6|vc6-rtm] [--show] [cl options]`
  compiles the file (`cl /nologo /c`), takes the function from the object (the
  `--sym` name, else the PDB/export name of `<va>`, else the object's only
  function) and diffs it against the original from `<va>` to the next function
  start found by `discover.py`, without trailing padding. It prints a side-by-side
  listing when the bytes differ. Relocated fields are masked, but checked: an
  object DIR32 must sit where the original has a base relocation and vice versa,
  and when both sides name the target (PDB/export name, import, or call target)
  the names must agree (`__imp_`, the C underscore and `@N` are ignored; PDB names
  shortened to prefix + hash are matched by prefix). Exit status 0 = match. The
  compiler defaults to vc5 for the Motion Factory DLLs, vc6 otherwise. Function
  extents are cached in `out/cache/match-<dll>-<hash>.json`.
- `check.sh` checks every function the source claims.
- `sweep.sh [setups]` runs every listed function against a set of option variants.

### Compiler and options (measured)

| DLL | compiler | options | evidence |
|---|---|---|---|
| mflinalg.dll | vc5 (c1xx 11.00.7303) | `/O2` (= `/Ox`) | 7/7 match; vc6 misses `MfMath::det2` (VC5 interleaves x87 loads with `fxch`, VC6 does not) |
| mbruntime.dll | vc6 (c2 12.00.8168) | `/O2`, and `/MT` where it matters | 8/8 leaves; std::string/map members need `/MT` (`_Lockit` in `<xtree>`); `basic_string::assign` and `_Copy` match with vc6 and not vc5 |
| SteamboatApp.dll `libpng/` | vc6 | `/Ox` (not `/O2`) | string literals are not pooled (`png_get_libpng_ver` returns one of two "1.0.6" copies, `png_crc_finish` uses two "CRC error" copies): `/O2` implies `/GF`, `/Ox` does not; 183/183 claims match, 9 more functions with it. No `/Gy` either, so one `.text` holds the unit: match.py checks calls to its other functions by name, claim or bytes; `/DZLIB_DLL /D_WINDOWS`: zlib's crc32/inflate are called as `WINAPI` (the callee pops), zconf.h's `ZEXPORT` under those defines; 8 more functions match with it |

Option sweep over the leaf functions (`sweep.sh`, 15 functions × 14 variants × 3
setups): identical bytes for `/O2`, `/Ox`, `/Ox /Op`, `/O2 /G5`, `/O2 /GB` (the
default), `/O2 /Gy`, `/O2 /GX`, `/O2 /Zi`. These break matches: `/O1` (2–3 of 7–8),
`/Og` (0), `/Od` (0), `/O2 /Oy-` (0: frame pointer omitted), `/O2 /G6` (4/8, 5/7),
and for mbruntime `/O2 /Oi-` (3/8: memcpy/memset/strcpy/strlen are inline
intrinsics). So: `/O2` with the default `/GB` scheduling. The leaf functions
alone do not separate VC5 from VC6 for mbruntime; `det2` and the STL members do.

mbruntime imports only KERNEL32 and USER32: the CRT is linked statically, which
fits `/MT`. Its STL members match `/O2 /MT` without `/GX`, with the STL's
`_TRY_BEGIN`/`_CATCH` macros defined as plain blocks before any STL header
(`src/MBEngine/MBStl.h`): the stock definitions give `basic_string::_Copy` an EH
frame and keep it from being inlined into `_Grow`. The STL code also inlines
mbruntime's own global `operator new`/`delete` (`MBmalloc_release`, and
`MBfree_release` behind a null check).

### VC6 RTM (8168) vs SP3 (8472) front ends: indistinguishable — use `vc6`

- Code: c1/c1xx 8168 and 8472 produced byte-identical code sections (and
  relocations) for every test: all matched functions, and ~100 KB of extra code
  (map/set/list/deque/vector/wstring instantiations, sort/stable_sort, iostreams,
  x87 loops, switch tables) under `/O2 /MT`, `/O2 /MT /GX`, `/O1 /GX`, `/Od /GX`.
- Stamps: both the object `@comp.id` (id 11, build 8168) and the CodeView
  `S_COMPILE` string ("Version 12.00.8168.0") come from c2, which is 8168 in both
  setups. The Rich header (C++ objects 8168 ×19, C 8168 ×111) and the eight
  modules in mbruntime.pdb therefore fit either front end.
- The linker is 6.00.8447 (SP3), so SP3 was installed, and SP3 installs the 8472
  front ends with it. `vc6` (SP3) is the default; `vc6-rtm` stays as a cross-check
  in case a construct turns up where the two differ.

### First matched functions (verified with match.sh)

mflinalg.dll (vc5 `/O2`):

| va | function | bytes | relocated |
|---|---|---|---|
| 10001580 | `MfMath::det2` | 21 | 0 |
| 10001670 | `MfMath::sqrt` (table-driven, 5 relocated fields) | 115 | 20 |
| 10001740 | `MfVector3::MfVector3(float, float, float)` | 25 | 0 |
| 10001760 | `MfVector3::MfVector3(const MfVector3 &)` | 25 | 0 |
| 100017a0 | `MfVector3::setValue` | 23 | 0 |
| 10001780 | `MfVector2::operator[]` | 10 | 0 |
| 10001790 | `MfVector2::operator[] const` | 10 | 0 |

mbruntime.dll (vc6):

| va | function | bytes | options |
|---|---|---|---|
| 10001b00 | `MBmemcpy` | 31 | `/O2` |
| 10001b40 | `MBmemset` (`c & 0xff`) | 48 | `/O2` |
| 10001b70 | `MBmemclr` | 27 | `/O2` |
| 10001cc0 | `MBstrcpy` | 44 | `/O2` |
| 10001db0 | `MBstrlen` | 19 | `/O2` |
| 10001f50 | `MBSetAssertBehavior` | 16 | `/O2` |
| 10002790 | `MBCheckHeapOnAlloc` | 16 | `/O2` |
| 100025e0 | `MBDebugHeapFindHeaderFromPtr` | 30 | `/O2` |
| 10001470 | `operator new` | 14 | `/O2` |
| 10001480 | `operator delete` | 16 | `/O2` |
| 10002b60 | `std::string::assign(const string &, size_t, size_t)` | 516 | `/O2 /MT` |
| 10002d70 | `std::string::_Tidy` | 76 | `/O2 /MT` |
| 10003070 | `std::string::_Eos` | 17 | `/O2 /MT` |
| 10003b50 | `std::string::max_size` | 6 | `/O2 /MT` |
| 10003b60 | `std::string::_Copy` | 166 | `/O2 /MT` (STL macros as plain blocks) |
| 10003c10 | `std::string::_Split` | 201 | `/O2 /MT` |
| 10003750 | `map<ulong,string>` `_Tree::_Erase` | 136 | `/O2 /MT` |
| 100037e0 | `_Tree::_Insert` | 723 | `/O2 /MT` |
| 10003ce0 | `_Tree::_Lbound` | 71 | `/O2 /MT` |
| 10003e10 | `_Tree::const_iterator::_Dec` | 159 | `/O2 /MT` |
| 10003eb0 | `_Tree::const_iterator::_Inc` | 132 | `/O2 /MT` |
| 10003f40 | `std::_Construct(pair<const ulong, string> *, ...)` | 295 | `/O2 /MT` |

Source-level findings so far: `MfMath::sqrt` is a table square root (top mantissa
bits + exponent parity index a `short` table built by `MfMath()`); the debug heap
puts a 0x30-byte header with magic `'MBDN'` at +4 in front of each block; statement
order decides register assignment (`sqrt`: computing the exponent before the
mantissa gives the original registers).

- VC5 front ends: c1 (C) is 11.00.7113 and c1xx (C++) 11.00.7303; only C++ was tested.

## The inline budget (measured 2026-10-16)

"Inline budget" is the usual reason for TODOs that stay at 50-90%: the original
calls an inline member out of line where our compile inlines it, or the other way
round. It is one rule of the front end (c1xx), identical in VC5 (11.00.7303) and
VC6 (12.00.8472); measured with generated test units (`out/inl/`) and checked on
PiccoloRT's ExecuteCode.

What decides:

- The budget is per function and counts the function's *source* size (expression
  nodes as written, before optimization) plus the inline expansions accepted so far.
  Code anywhere in the function counts, also after the call site and also dead code:
  `if (0) { ... }` or a `const int` false condition costs as much as live code and
  generates nothing.
- Candidates are taken in source order, depth first (an inline's own inline calls are
  expanded inside it).
- Two limits:
  - growth: a function may grow to about 3.1 times its own size (allowance about
    2.1 x S0, S0 = the function's size without expansions). A candidate that does not
    fit is called, later smaller ones may still be inlined. This decides in small and
    medium functions: there more code in the caller means more inlining.
  - a total cap of about 3,500 statements of the form `s += g_v[k] * x;` (each about 7
    nodes; a call statement `s += f(g_v[k] + a);` costs 1.4 of them). Once a big
    function reaches it, every later call stays a call, even trivial ones. Here more
    code in the caller means less inlining.
- Cost of an expansion is the callee's body (as written) plus about 2.3 statement
  units of overhead: `return x;` costs about 1.5 call statements, `return x * 3 +
  g_v[1];` 2.6, a 10-statement body about 12.4 statements.
- No effect: `/Ob1` vs `/Ob2` (/O2 already is /Ob1, the inline keyword or in-class
  bodies), `#pragma inline_depth(n)`, `#pragma inline_recursion`, `/Ox`, dllexport
  on the classes. `__forceinline` (VC6 only; VC5 rejects it) bypasses the budget.
  There is no project option or pragma that moves the limit, so nothing for
  src/options.txt.

How to use it on a TODO:

- Find where the original stops inlining (the first out-of-line copy of an inline
  member, `tools/msvc/show.sh`), and where ours stops. In a big function (cap regime)
  ours stopping later means our function, or the bodies it inlines before that
  point, is smaller than the original's; stopping earlier means bigger. In a small
  function (growth regime) it is the other way round for the caller, while bigger
  callee bodies always mean less inlining.
- The size of the difference can be measured: put a block of dead statements
  (`if (0) { gFill[0] = gFill[1] * 3; ... }`) into a scratch copy and grow it until
  the out-of-line call counts equal the original's. The count is the missing (or
  surplus) source size in statement units.
- Example: MfScVirtualMachine::ExecuteCode (PiccoloRT, 17 KB). The original gives up
  in CMPGTE, ours seven cases later. With 500 dead statements (about 3,500 nodes,
  15% of the cap) in a scratch copy, our out-of-line calls are 574 like the
  original's and their distribution per callee agrees (HasType 45/45, GetType 29/27,
  GetFloat 19/17, SetType 10/9; Release 103/117 is a separate effect, see below);
  similarity rises only from 52% to 58%, the rest is block layout and registers. So
  the original's function was larger as written than ours by about that much: code
  we do not see (compiled-out debug or trace statements under a constant condition,
  bigger macros) or bigger header inline bodies.
- Not explained by the budget: the original calls MfScDataHandle::Release out of line
  inside the inlined MfScVmEnv helpers (PopAndReleaseData, ClearScratchREGn, POP_FP)
  long before its give-up point. Nested inlining as such works (tested: an inline
  calling an inline is expanded inside it, dllexport or not).
- SteamboatApp VertexClusterAttribute::AddCluster (growth regime) is not fixed by
  caller size; there the callee bodies differ (the original's AVec/IVec constructors
  are larger, they are compiled without /Oa), which is the other lever.

## Overloaded virtual functions: vftable order (checked 2026-10-16)

VC5 and VC6 place all overloads of a virtual function that a class introduces at the slot
of its first declaration, as one group, in *reverse* declaration order. Test unit
`struct A { virtual void f(int); virtual void g(); virtual void f(char *);
virtual void f(double); virtual void h(); };` gives the vftable f(double), f(char *),
f(int), g, h with both compilers. Overloads that a derived class adds later get slots of
their own after the base's (MfPropertyList: Add(const MfProperty *) at 4, Add(const
MfProperty &) at 11). Order matters only where the group is introduced (the interface);
overriders in derived classes may declare in any order.

A wrong order in a header compiles silently and calls the other overload through the
interface. Found in MBEngine: IMBEventManager::RemoveListener was declared in the wrong
order in seven unit headers (~CMBNodeManager, ~CMBDirectDrawSurface called the wrong
slot; see src/MBEngine/NOTES.md).

Project-wide check: every class in a header under src/ that declares several overloads of
a virtual method was compared with the original vftables that carry names (the PDB of
MBEngine, SteamboatApp, Steamboat.exe, mbruntime; the exported ??_7 tables of
RuntimeLib, MfActorLib, MfPoLibrary, PiccoloRT, PiccoloComp, PiccoloConsole, mfutil):
the slot order of each overload group (by the parameter types of the slot's mangled
name) must be the reverse of our declaration order in the class that introduces the
group. 223 declarations agree; the only disagreements are overriding classes (CMBFont,
CMBUniFont, CMBWinSystemFont, CMBScreenPoly, CSBNodeManager, MfPropertyList/Table, whose
interfaces are declared correctly), so nothing else to fix. Checked groups include
IMBEventManager::RemoveListener, IMBFileSpec::SetPath, IMBFont::DrawText/GetTextSize,
IMBPref::GetString/SetString, IMBScript::Get/SetScriptVariable, IMBSoundManager::
IsSoundPlaying, IMBStringTable::Load, IMBTextControl::Get/SetText, IMBAnim's Get/Set
properties, IMBScreenPoly/ISBAnimButton/ISBClickable messages and SetAnim, ISBGameState,
ISBNodeManager::AddNode, ScriptHelpNode Start/StopScript, SteamboatNode::OpenPrefStream,
SBActorManager::LoadActor, MfAiActor/MfAiActorNode Get/SetProperty, RemoveActivity,
SendEvent, StartTimer, the Create*Activity families, MfColNode::CheckCollision and
MfSpace ShowAxis/ShowGrid. Not covered: binaries without vftable names (MfImageLib,
mflinalg, MBCom, zlib: their source directories declare no overloaded virtuals) and classes whose vftable the original does not contain.
The scan scripts are kept in the session scratch (vtovl.py, ovlcheck2.py).

## Worker setup (pilot, 2026-10-05)

Three MBEngine units (CMBWinClock, CMBResourceMgr, MBSoundUtils: 44 functions, VC6
`/O2`) were each decompiled by three setups, one fresh agent per unit, at most 6
compiles per function, verified afterwards by re-checking every match:

| setup | matches | tokens | tokens/match |
|---|--:|--:|--:|
| Sonnet, `show.sh` only | 29 | 388k | 13.4k |
| Sonnet, `show.sh --ghidra` | 35 | 407k | 11.6k |
| Opus, `show.sh` only | 41 | 344k | 8.4k |

Opus matched the most and spent the fewest tokens per match: the hard part is
reproducing VC6's choices (what it inlines, register allocation, `$E` numbering),
and the weaker model spends its compiles without converging. The Ghidra drafts
(`tools/ghidra/`) gave Sonnet some control flow but wrong argument counts and no
template semantics; both agents that used them called them marginal (the Ghidra
scripts have since been removed). Default: Opus, one agent per unit, annotated asm
only, 6-compile cap, a few agents in parallel, verification only by the checker.

Two of Opus' 41 matched `$E20`/`$E19` only via `--sym` naming the neighbouring
`$E19`/`$E18` (identical bytes, numbering off by one); they are in the `.todo`, so
39 went into `src/MBEngine/`.

Every unit re-derived the same declarations (`MBGUID`, `MBComPtr<T>`, the inline
`operator new`/`delete` over `MBmalloc_release`, the `MBIID_*` constants). Shared
per-binary headers would save that work in every later unit.

### Bundling (batches 1-3 on MBEngine)

| batch | per worker | matched | tokens/match | tokens/byte of code |
|---|---|--:|--:|--:|
| 1 | one unit, README sections | 69/71 | 6.7k | 150 |
| 2 | one unit, + NOTES.md | 72/77 | 7.2k | 142 |
| 3 | 2-3 related units, `src/WORKER.md` | 206/226 | 4.5k | 48 |

A worker pays a fixed 50-80k tokens to read the brief, the notes and the headers
before its first compile; single small units (0.3-1 KB) are dominated by it.
Bundles of related units (3-7 KB) spread it and reuse their own declarations.
Default now: one worker per 3-7 KB of related units, functions over ~600 bytes
get 3 compiles. Not worth workers: the `$E` numbering gaps (one header construct
the PDB does not show; 174k tokens spent, none closed) stay in the `.todo`.


## Since then

From 2026-10-07 to 2026-10-13 a loop driving local models (Qwen 27B class on
llama.cpp) took over from the cloud workers. It matched functions, but too slowly
and too rarely on anything non-trivial to finish the project, and it was removed
with branch `decomp`. Two things it left behind are kept: the stricter verifier
(read-only data such as vtables must equal the original's, which moved 192
functions from the manifests to `.todo` files) and the class headers it drafted.
Those headers often ignored the PDB: where a binary has one, `pdbtypes.py header`
gives the original declarations (member names, typedefs, layout) and is the
better starting point.
