# macOS port

A native macOS (Apple Silicon) build of the game from the decompiled source in `src/`.
Nothing here changes `src/`: the decompilation stays byte-verifiable by
`tools/msvc/check.sh`, and the port adapts it from outside.

## How the port relates to `src/`

- **One library per original binary.** Every original DLL becomes a macOS dylib built
  with hidden symbols; what the DLL exported (`__declspec(dllexport)`) is exported. The
  DLLs carry private copies of shared classes, so they cannot share one link. The
  executable is `Steamboat`; plugins are loaded with `dlopen` where the game calls
  `LoadLibrary`. Module table and flags: `port/flags.py`.
- **Microsoft C++ is compiled by clang** in Microsoft-compatibility mode
  (`-fms-extensions -fms-compatibility -fdelayed-template-parsing`);
  `compat/prelude.h` (force-included) maps the remaining language extensions.
- **APIs the game called come from `compat/include/`**: the Windows, DirectX, C
  runtime and old iostream headers the source includes, written for the port (no
  Microsoft headers are used). They declare exactly what the game uses.
- **Implementations come from `platform/`**: Win32 on SDL3 and POSIX, DirectDraw and
  Direct3D 7 drawing in software, Miles Sound System on SDL audio, Bink and Smacker on
  FFmpeg.
- **When a source file cannot be compiled as it is**, a copy goes to
  `port/override/<same path under src/>` and is compiled instead. The first line of the
  copy says why. Prefer a compat header or a platform function: an override is a fork
  of decompiled code and should stay small and rare.
- **Real decompilation bugs found while porting are fixed in `src/`**, and
  `tools/msvc/check.sh` must stay clean for the directory.

## 64-bit

macOS runs only 64-bit code; the game was written for 32-bit Windows. The Windows type
names (`DWORD`, `LONG`, `UINT`, `HRESULT`, ...) are 32 bits wide in `compat/`, but the C
`long` of the source is 64 bits here, and pointers no longer fit in 32-bit integers.
Structures read from files, the script VM's data handles, and pointer/integer casts are
where this bites; fixes go into overrides with a comment.

## Building

Requirements: Xcode command-line tools, Homebrew `cmake`, `ninja`, `sdl3`, `ffmpeg`.

```
python3 port/tools/census.py                        # how many source files compile, per module
cmake -S port -B out/port/build -G Ninja            # configure (module table: port/flags.py)
cmake --build out/port/build                        # or --target <module>
```

For crash reports with exact locations, build a second tree with sanitizers:
`cmake -S port -B out/port/build-asan -G Ninja -DPORT_SANITIZE=address,undefined`.

How to point the game at your CD comes with the first running build.

Not ported: the developers' tools (Piccolo script console, debugger, class browser),
which use MFC and are not part of the game.
