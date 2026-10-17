# macOS port

A native macOS (Apple Silicon) build of the game from the decompiled source in `src/`.
Nothing here changes `src/`: the decompilation stays byte-verifiable by
`tools/msvc/check.sh`, and the port adapts it from outside.

## How the port relates to `src/`

- **One executable, no DLLs.** Every original binary is a module (an object library),
  and all of them link into the one executable `Steamboat`. Each module has a generated
  table of what Windows knew of its DLL (name, DllMain, the DLLs it imports from, the
  functions `GetProcAddress` finds), so `LoadLibrary` and `GetProcAddress` work as
  before, and DllMains run when Windows ran them: at start for the DLLs the executable
  imports from, at `LoadLibrary` for the plugins (`platform/win32/module.cpp`). Names
  that several DLLs each defined for themselves (DllMain, the plugin entry points, the
  parsers' `yy` names, SteamboatApp's copies of MBEngine classes) get the module's name
  as prefix in that module (`RENAMES`). Module table and flags: `port/flags.py`.
- **Code every DLL had its own copy of** (inline functions, templates, vtables) is
  linked once. `python3 port/tools/mergecheck.py` checks that the modules' copies agree
  and `python3 port/tools/vtablecensus.py --cross` that their vtables do.
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

## Improvements

The port plays like the original except for these deliberate improvements. The
decompiled code in `src/` stays as the original was; each improvement is new code in
`port/improve/<module>/` or `platform/`, hooked in by `port/edits/improvements.py`.

- **Sharper 3D.** The 3D characters and objects are drawn at `MICKEY_SCALE` times the
  game's 640x480 (default 3); the painted backgrounds keep their resolution
  (`platform/dx`, the sharp copies of the surfaces).
- **An honest cursor** (`port/improve/SteamboatApp/cursor.cpp`): the plain glove means
  "walk there", the pointing hand "do something" (pick up, use, play a sound), and a
  new cursor, the glove with a speech bubble, "Mickey says something" (explains a
  painted spot, talks to a character). The original never looked at the 3D scene
  before a click and gave several explain spots the plain glove; on the inventory bar
  the pointing hand now shows only over the buttons and the items usable in the scene.
- **Smooth movement** (`port/edits/improvements.py`, `port/improve/RuntimeLib/walk.cpp`):
  animations are sampled between their keyframes (the original stepped at 30 Hz, which
  showed as jerks at today's frame rates); a walking character's height follows the
  floor over a few frames instead of jumping when a walk starts; two walking characters
  do not stop for each other (the original cancelled one walk at random, which made
  Mickey pause, snap round and slide in the main menu).
- **Leaving the game** (`port/improve/SteamboatApp/credits.cpp`): Yes on the exit board
  always works (Mickey's question is stopped so his goodbye can play; the original
  waited for ever when Yes came while he still asked), and a click, Space, Enter or
  Escape skips the credits.

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
cmake --build out/port/build                        # or --target Steamboat, port_tests, <module>
```

For crash reports with exact locations, build a second tree with sanitizers:
`cmake -S port -B out/port/build-asan -G Ninja -DPORT_SANITIZE=address,undefined`.

Not ported: the developers' tools (Piccolo script console, debugger, class browser),
which use MFC and are not part of the game.

## Run

You need the game's CD (the German release, "Micky auf Ganovenjagd") as an `.iso` image,
or as a mounted directory.

1. Install the tools once:
   `xcode-select --install`, then `brew install cmake ninja sdl3 ffmpeg`.
2. Build from the repository root:
   ```
   cmake -S port -B out/port/build -G Ninja
   cmake --build out/port/build
   ```
3. Unpack the image if it came zipped (for example
   `unzip <name>.zip`). This should give you `Micky_auf_Ganovenjagd.iso`.
4. Start the game:
   ```
   MICKEY_CD=/path/to/Micky_auf_Ganovenjagd.iso out/port/build/Steamboat
   ```
   `MICKEY_CD` can also be the CD's root directory, such as the mounted image under
   `/Volumes/...`. To set it once instead, put `cd = <path>` in `port.ini` in the data
   directory (see below).

**Where things go.** The CD is never written. Everything the game writes goes to the data
directory: `~/Library/Application Support/Mickey Saves the Day`, or `MICKEY_DATA` if set.
Inside it, `C:\` of the game is mirrored as folders:
- saved games and the ini files go to `setup/hd/binaries/`;
- the registry is `registry.ini`.

On first start the installer's ini files are copied there. Delete the directory to start
over.

**Settings** (environment variables):

| Variable | Effect |
|---|---|
| `MICKEY_CD` | the CD: `.iso` file or directory |
| `MICKEY_DATA` | the data directory (default above) |
| `MICKEY_WINDOWED=1` | play in a window instead of full screen |
| `MICKEY_SCREEN_W`, `MICKEY_SCREEN_H` | the desktop size the game sees (default 640 x 480) |
| `MICKEY_SMOOTH=1` | smooth (linear) scaling of the picture instead of sharp pixels |
| `MICKEY_SCALE=n` | the 3D characters are drawn at n times the game's 640x480 (default 3; 1 is the original resolution); the painted backgrounds keep theirs |
| `MICKEY_TEXT_AA=1` | anti-aliased text |
| `MICKEY_LANG` | the CD's language, German unless set (`eng`, ...) |
| `PORT_LOG` | diagnostics on stderr: `1` for all, or channel names, for example `PORT_LOG=vfs,module,resource`. Channels: `vfs` `module` `resource` `registry` `window` `process` `dx` `media` `stub` `exception` `debug` `debugstring` `defaults` |
| `PORT_DEBUGSTRINGS=1` | print the game's own debug output (OutputDebugString) |

**Reporting a crash.** Run from a terminal with `PORT_LOG=1` and save the output:

```
PORT_LOG=1 MICKEY_CD=... out/port/build/Steamboat 2>&1 | tee mickey.log
```

Send `mickey.log` together with the newest `Steamboat-*.ips` file from
`~/Library/Logs/DiagnosticReports`. Also say what you did just before the crash.
