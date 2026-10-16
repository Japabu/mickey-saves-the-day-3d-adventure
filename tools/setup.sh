#!/bin/bash
# One-time project-local setup: create a Python environment and Wine prefix,
# extract PDB types, and run a compiler smoke test. Host tools are never installed.
# usage: tools/setup.sh
set -e
ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

if [ "$(uname)" = Darwin ]; then
  for dir in /opt/local/bin /opt/homebrew/bin; do
    [ -d "$dir" ] && PATH="$dir:$PATH"
  done
  if ! command -v wine >/dev/null 2>&1; then
    WINE_BIN="/Applications/Wine Devel.app/Contents/Resources/wine/bin"
    [ -x "$WINE_BIN/wine" ] && PATH="$WINE_BIN:$PATH"
  fi
fi

command -v wine >/dev/null 2>&1 || {
  echo "Wine is required. Install it for your OS, then put wine and wineboot on PATH." >&2
  exit 1
}
command -v wineboot >/dev/null 2>&1 || { echo "wineboot was not found on PATH." >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "Python 3 is required." >&2; exit 1; }

# The game's binaries and the Microsoft compilers are not in the repository: each
# contributor imports their own copies (tools/import.py checks them by SHA-256)
for set in game vc5 vc6; do
  missing=0
  while read -r digest path; do
    [ -f "$ROOT/$path" ] || missing=$((missing + 1))
  done < "$ROOT/tools/manifest/$set.sha256"
  if [ "$missing" -ne 0 ]; then
    echo "$missing files of '$set' are missing. Import your own copies first:" >&2
    echo "  python3 tools/import.py $set <directory> ...   (see README.md, Requirements)" >&2
    exit 1
  fi
done

if [ ! -x "$ROOT/.venv/bin/python" ]; then
  python3 -m venv "$ROOT/.venv"
fi
PYTHON="$ROOT/.venv/bin/python"
export PYTHON
"$PYTHON" -c 'import capstone' 2>/dev/null || "$PYTHON" -m pip install capstone
# unicorn: the x86 emulator that runs original functions for port/difftest
"$PYTHON" -c 'import unicorn' 2>/dev/null || "$PYTHON" -m pip install 'unicorn==2.1.4'

echo "== Wine prefix (first run takes a minute)"
mkdir -p "$ROOT/out"
if [ "$(uname)" = Darwin ] && grep -q '^#arch=win32$' "$ROOT/out/wine/system.reg" 2>/dev/null; then
  WINEPREFIX="$ROOT/out/wine" wineserver -k >/dev/null 2>&1 || true
  rm -rf "$ROOT/out/wine"
  echo "Removed the incompatible generated 32-bit Wine prefix"
fi
bootlog=$(mktemp "$ROOT/out/wineboot.XXXXXX")
if ! WINEPREFIX="$ROOT/out/wine" WINEDEBUG=-all wineboot -i >"$bootlog" 2>&1; then
  cat "$bootlog" >&2
  echo "Wine prefix initialization failed; see the message above." >&2
  exit 1
fi
rm -f "$bootlog"

echo "== PDB types -> out/types"
"$PYTHON" tools/re/pdbtypes.py all >/dev/null

echo "== smoke test"
tools/msvc/check.sh MBEngine/CMBWinFileSpec.cpp >/dev/null && echo "OK: MBEngine/CMBWinFileSpec.cpp" \
  || { echo "smoke test failed: tools/msvc/check.sh MBEngine/CMBWinFileSpec.cpp"; exit 1; }
echo "ready. Read HACKING.md"
