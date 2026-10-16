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

Not ported: the developers' tools (Piccolo script console, debugger, class browser),
which use MFC and are not part of the game.
