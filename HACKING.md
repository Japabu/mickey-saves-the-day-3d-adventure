# Working on the decompilation by hand

Goal: C/C++ that the original 1999/2000 Microsoft compilers turn into the exact bytes
of the original game binaries, one function at a time, verified by a tool and never by
eye. This page is the reference for the workflow, the tools and the rules.

## Setup (once)

First import your own copies of the game binaries and the compilers (README.md,
Requirements, says exactly which):

```
python3 tools/import.py all /Volumes/MICKEY3D /path/to/vc5 /path/to/vc6
tools/setup.sh
```

`import.py` hashes every file under the directories you give and copies each one the
manifests in `tools/manifest/` list to its place (`orig/binaries/`, `tools/msvc/vc5/`,
`tools/msvc/vc6/`); file names and layout of your copies do not matter. Those
directories are git-ignored: never commit them.

`setup.sh` checks that the imports are complete, checks for Wine (runs the original
compilers) and Python 3, creates a project-local Python environment with `capstone`,
creates the Wine prefix in `out/wine`, extracts PDB types to `out/types/` and checks
that one known function still matches. It installs no host software. On
Debian/Ubuntu, Wine with 32-bit support is
`dpkg --add-architecture i386 && apt-get update && apt-get install wine wine32:i386`.

## Workflow

All commands run from `src/`.

1. Pick a unit and list its functions (status `todo` = not matched yet):
   `../tools/msvc/funcs.sh MBEngine.dll --module CMBPref`
2. Look at a function: disassembly with names, PDB prototype and locals:
   `../tools/msvc/show.sh MBEngine.dll 1000d090`
3. Write the C++ in `MBEngine/CMBPref.cpp`, functions in address order, and keep this
   running beside the editor:
   `../tools/msvc/watch.sh MBEngine.dll 1000d090 MBEngine/CMBPref.cpp`
   Every save recompiles with the binary's options (`src/options.txt`) and shows
   original | yours side by side, ending in `MATCH` or `DIFF: ... N differing, NN% similar`.
4. On MATCH, claim the function: the line above it reads `// 0x1000d090 <symbol>`, the
   symbol the object file defines for it (usually the original's mangled name). Then
   check the whole file: `../tools/msvc/check.sh MBEngine/CMBPref.cpp`.
5. Stuck? Write `// TODO 0x<va> <symbol>: <why>` and move on.

VS Code: `Terminal > Run Task` has tasks for watching a function, showing the original
and checking the current file.

## Tools (`tools/msvc/`)

| command | what it does |
|---|---|
| `funcs.sh <bin> [--todo] [--module M] [--from VA] [--to VA] [--summary]` | functions with size, status (`match`, library kinds, `todo`), PDB module, names |
| `show.sh <bin> <va>` | annotated disassembly of the original: call targets, globals, imports, strings, float constants, jump tables, `*` = fixup field; PDB prototype and locals |
| `match.sh <bin> <va> <file.cpp> [--sym NAME] [--show]` | compiles the file and diffs the function against the original; exit 0 = MATCH |
| `check.sh [-j N] [-v] [file, directory or binary...]` | checks every claim in the source (default: all); exit 0 = all match |
| `watch.sh <bin> <va> <file.cpp>` | `match.sh --show` on every save |
| `cl.sh <vc5\|vc6\|vc6-rtm> [cl\|link\|lib\|dumpbin] args` | the original compilers under Wine |
| `equiv.sh <bin> <va> <file.cpp>`, `equiv.sh --todos [filter...]` | whether a `TODO` function is still the original's logic (below) |

`<bin>` is a path under `orig/binaries` (`RuntimeLib.dll`, `plugins/PscStdlib.dll`).
Also: `python3 tools/re/pdbtypes.py header|funcs|func|stats ...` (types, prototypes and
locals from the PDBs).

**What MATCH means:** identical bytes outside relocated fields, and the relocated
fields are checked too: same fixup positions, the same target symbol where the original
target is named, consistent offsets, jump tables pointing into the function itself, and
read-only data the function refers to (float constants, string literals) with the
original contents.

**Logic of near misses.** The source is ported, so a `TODO` body must be the original's
logic even where its bytes differ. `equiv.sh` checks that. `renaming-equivalent` is a
proof: the same instructions in the same order up to a consistent renaming of registers
(only those without an implicit role) and of local stack slots, with jump tables, EH
tables, vtables and constants agreeing. `not-renaming` says only that this proof does
not apply. `equiv.sh --claims` is its self-test: every matched function must pass.

**C++ exception handling funclets.** A function compiled with `/GX` that needs
unwinding has its unwind funclets and handler stub at the end of the code section.
They show up as tiny separate functions with status `eh-funclet`. Never decompile them
on their own: match the parent; `match.sh` and `check.sh` compare its funclet block too.

## Compilers and options

Measured; the evidence is in [`tools/msvc/COMPILERS.md`](tools/msvc/COMPILERS.md).

| binaries | compiler | options |
|---|---|---|
| RuntimeLib, MfActorLib, MfPoLibrary, MfImageLib, PiccoloRT/Comp/Console, mflinalg, mfutil, plugins/* | vc5 | `/O2 /GX /MD`; bundled libjpeg in MfImageLib: `/O2 /Gy` |
| SteamboatApp | vc6 | `/O2 /GX /MT` |
| Steamboat.exe, MBEngine, MBCom, mbruntime, zlib | vc6 | `/O2 /MT` |

Options are fixed per binary: the CRT imports and the presence of EH funclets prove
them. `match.sh` picks the compiler from the binary's linker version. **Never change
options to make one function match**; a difference means the source is wrong.

## Layout

- `src/<Bin>/` per binary (`<Bin>` = file name without extension).
- One unit = one source file with its header `src/<Bin>/<unit>.h`: an original object
  file where the PDB names it, else a class.
- Class headers: where the binary has a PDB (SteamboatApp, MBEngine, MBCom, mbruntime,
  Steamboat.exe), start from the original declaration that
  `python3 tools/re/pdbtypes.py header orig/binaries/<Bin>.pdb` prints: member names,
  typedefs, access and layout. Without a PDB a class header is reconstructed from
  vtables, mangled names and the machine code of the methods; unknown members are
  `field_<offset>` until a method shows what they hold. One header per class,
  `src/<Bin>/<Class>.h`, shared by every unit that uses it, so a change to it is kept
  only if every unit including it still matches (`check.sh`).
  `src/<Bin>/enums.h` holds the enumerations the binary names.
- `<unit>_unit.h` is the private header of a unit written by hand before class headers
  existed. It declares everything that unit needs and is included by nothing else.
- Source is meant to be read: members by name (no casts of `this`,
  no pointer plus offset), no address as a number, named constants and four-character
  codes (`'PrtN'`) where the binary shows them.
- Each function is preceded by `// 0x10001670 ?sqrt@MfMath@@SAMM@Z` (VA and original name).
- `src/<Bin>/NOTES.md`, where it exists, lists source patterns that are known to match
  for that binary. Read it first; add a line for a new pattern.
- `src/lib/<bin>.txt` lists statically linked library code, which is not decompiled.

## Claims

- `// 0x<va> <symbol>` at the start of a line in a `.c`/`.cpp` file claims that the
  object compiled from the file defines `<symbol>` with the bytes of the original
  function at `<va>` in the file's binary. Text after the symbol is a comment.
  `// <binary> 0x<va> <symbol>` claims the same compiled code for another binary (an
  inline function of a shared header that both binaries contain).
- `check.sh` compiles every file once with its binary's options and checks every claim;
  `funcs.sh` and the progress block count claims. Only code that matches is claimed.
- Options are per original project, never per file: `src/options.txt` has one line per
  binary and one per library that was built on its own and linked in (`libpng/`, `jpeg/`).
- Several claims may name one address: where the linker folded identical functions into one
  copy, each file that defines one of them claims its own symbol there. A call to such a
  function from another file is accepted because its symbol is claimed (and checked) there.
- `// TODO 0x<va> <symbol>: <reason>` notes a function that does not match yet.
- `// EQUIV 0x<va> <symbol>: <kind> ...` marks a function whose bytes differ but whose
  logic is the original's. Two kinds, counted apart in the progress block:
  - `proven`: written only by `equiv.sh --todos --mark` (renaming-equivalent); `check.sh`
    re-proves it on every run.
  - `reviewed: <reason>`: written by whoever compared both instruction streams and is
    certain the logic is the same. Allowed differences: register choice, stack slots,
    order of independent instructions, a helper inlined in one and called in the other
    with the same effect. Never when a constant, a call or its arguments, a condition,
    the order of side effects or the control flow differs: in doubt it stays a `TODO`
    (those get differential testing at the very end). `check.sh` checks that the file
    compiles and defines the function.
  An equivalent function is done for the port: spend the time on unwritten functions.
- `logicdiff.sh [filter...]` screens the `TODO`s with a body for logic that may differ:
  per function it compares the callees and their order, the named globals and strings,
  the immediate constants and the member offsets of the original and the compiled code,
  and ranks the functions by what differs (missing or extra calls first). It proves
  nothing and ignores register, stack and layout differences; read the flagged
  functions with `match.sh --show` before fixing or marking them.
- One source file compiled into two binaries (the original projects shared it): the
  second binary's directory gets a file that `#include`s it and carries that binary's
  claims, so it is compiled with that binary's options.
- Something only part of a file needs (template instances that want `/Oa`, say) is a
  `#pragma` in the source with a comment giving the evidence, never an options line.

## Rules

1. Genuine C/C++ only. Forbidden: `__asm`, `_emit`, `__declspec(naked)`, code as byte
   arrays, any trick that places bytes instead of compiling source. `#pragma`
   (`optimize`, `intrinsic`, `inline_depth`, `pack`) only with a comment giving the evidence.
2. Original names: PDB names and export names. Unnamed functions in binaries without a
   PDB get a descriptive name or `fn_<va>`.
3. Not decompiled: EH funclets (they come with their parent), statically linked
   CRT/STL/MFC code, incremental-link and import thunks. Compiler-generated functions
   (`??_G`/`??_E` deleting destructors, `$E` static initializers) are matched, from the
   class declarations.
4. Calls into other DLLs: declare the callee `__declspec(dllimport)` when the original
   calls through the import table (`[import X]` in `show.sh`), otherwise not.
5. Only code that matches is claimed.

## Tips

- Start with small functions (getters, constructors, `Make...` factories).
- Existing units in `src/MBEngine/` are the best examples; copy their declarations.
- Write it like a 1999 programmer: plain loops, early returns, no modern idioms.
  Register or order differences usually mean a different statement order, a
  temporary variable, or a different type (`int` vs `long`, signed vs unsigned).
- `_N` in a mangled name is `bool`.
- Do not paste the game's code into public sites (decomp.me scratches are public).
