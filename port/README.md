# Port

The game built from the decompiled source in `src/`, natively for macOS (Apple Silicon)
and as WebAssembly for the browser. How it works: [NOTES.md](NOTES.md).

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
  A held item sparkles over what a click with it is used on (an object, a character),
  not over the floor, where a click only makes Mickey walk.
- **Options panel** (Escape; `platform/win32/options.cpp`, `platform/media`): an overall
  volume and one each for music, voices and sound effects, logarithmic and fine at the
  quiet end; moving the voices or effects slider plays a sample, and the music plays on
  under the panel. Full screen and the 3D sharpness are set there too.
- **Smooth movement** (`port/edits/improvements.py`, `port/improve/RuntimeLib/walk.cpp`):
  animations are sampled between their keyframes (the original stepped at 30 Hz, which
  showed as jerks at today's frame rates); a walking character's height follows the
  floor over a few frames instead of jumping when a walk starts; two walking characters
  do not stop for each other (the original cancelled one walk at random, which made
  Mickey pause, snap round and slide in the main menu).
- **Skipping lines**: Space or Enter ends the line being spoken, and the dialogue goes on
  with the next one (`port/improve/MBEngine/dialogue.cpp`).
- **The recipe again** (`platform/win32/defaults.cpp`): Goofy reads Von Drake's tissue only
  once; using the tissue under the magnifying glass on him makes him read it again.
- **No vanishing characters** (`port/edits/improvements.py`): a script that turns a
  character toward the point it stands on (Ludwig Von Drake after the tissue) no longer
  makes its position invalid, which made it disappear.
- **Leaving the game** (`port/improve/SteamboatApp/credits.cpp`): Yes on the exit board
  always works (Mickey's question is stopped so his goodbye can play; the original
  waited for ever when Yes came while he still asked), and a click, Space, Enter or
  Escape skips the credits.

## Building

Requirements: Xcode command-line tools, `cmake`, `ninja`. The first configure runs
`port/deps.py`, which downloads SDL3 and FFmpeg (pinned releases, checked by SHA-256)
and builds them as static libraries into `out/deps/<target>/` (FFmpeg with only the
decoders the game needs); the executable links everything statically but the system's
own libraries.

```
python3 port/tools/census.py                        # how many source files compile, per module
cmake -S port -B out/port/build -G Ninja            # configure (module table: port/flags.py)
cmake --build out/port/build                        # or --target Steamboat, port_tests, <module>
```

For crash reports with exact locations, build a second tree with sanitizers:
`cmake -S port -B out/port/build-asan -G Ninja -DPORT_SANITIZE=address,undefined`.

Not ported: the developers' tools (Piccolo script console, debugger, class browser),
which use MFC and are not part of the game.

## Browser

The same source builds for WebAssembly with [Emscripten](https://emscripten.org)
6.0.12, installed into `out/emsdk`:

```
git clone https://github.com/emscripten-core/emsdk out/emsdk
out/emsdk/emsdk install 6.0.12 && out/emsdk/emsdk activate 6.0.12
source out/emsdk/emsdk_env.sh
cmake -S port -B out/port/build-web -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=$PWD/out/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
cmake --build out/port/build-web --target Steamboat
python3 port/web/serve.py               # then open http://localhost:8000/
```

The page (`port/web/`) asks for the ISO and copies it into the browser's private
storage for the site (OPFS) once; saved games stay there too, and the start page exports
and imports them as small JSON files to keep or send to someone. Nothing is uploaded. The
game needs shared memory, so the server must send COOP/COEP headers: `serve.py` does,
and on hosts that cannot (GitHub Pages) `coi-serviceworker.js` adds them. Add
`?log=1` (or `?log=<channels>`) to the address for the `PORT_LOG` output in the
browser console. For a smaller download (about 5 MB instead of 43 MB), configure
without debug info: `-DCMAKE_C_FLAGS_RELWITHDEBINFO="-O2 -DNDEBUG"
-DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -DNDEBUG"`.

## Run

You need the game's CD (the German release, "Micky auf Ganovenjagd") as an `.iso` image,
or as a mounted directory.

1. Install the tools once:
   `xcode-select --install`, then `brew install cmake ninja`.
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

**Where things go.** The CD is never written. The data directory is
`~/Library/Application Support/Mickey Saves the Day`, or `MICKEY_DATA` if set; it holds
the port's settings (`port.ini`). Everything the game writes goes to its folder for the
CD's language (`ger`, `eng_gbr`, ..., from the CD's `steamboat.ini`), so CDs of
different languages keep their own saved games. Inside it, `C:\` of the game is
mirrored as folders:
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
| `MICKEY_LANG` | the language of the port's texts, the CD's unless set (`ger`, `eng`, ...) |
| `PORT_LOG` | diagnostics on stderr: `1` for all, or channel names, for example `PORT_LOG=vfs,module,resource`. Channels: `vfs` `module` `resource` `registry` `window` `process` `dx` `media` `stub` `exception` `debug` `debugstring` `defaults` |
| `PORT_DEBUGSTRINGS=1` | print the game's own debug output (OutputDebugString) |

**Reporting a crash.** Run from a terminal with `PORT_LOG=1` and save the output:

```
PORT_LOG=1 MICKEY_CD=... out/port/build/Steamboat 2>&1 | tee mickey.log
```

Send `mickey.log` together with the newest `Steamboat-*.ips` file from
`~/Library/Logs/DiagnosticReports`. Also say what you did just before the crash.
