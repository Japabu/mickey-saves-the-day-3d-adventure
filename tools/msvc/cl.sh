#!/bin/bash
# Run the original Microsoft compilers under Wine.
#
# usage: cl.sh <vc5|vc6|vc6-rtm> [cl|link|lib|dumpbin|editbin|shell] <args...>
#   vc5      Visual C++ 5.0 + VS97 SP3 (c1 11.00.7113, c1xx/c2 11.00.7303, link 5.10.7303)
#   vc6      Visual C++ 6.0 + VS6 SP3 (c1/c1xx 12.00.8472, c2 12.00.8168, link 6.00.8447)
#   vc6-rtm  vc6 with the RTM front ends (c1/c1xx 12.00.8168) from vc6/bin-rtm
# The tool defaults to cl. INCLUDE/LIB point at the setup's include/lib (+MFC/ATL).
#
# cl.exe loads c1/c1xx from its own directory before PATH, so vc6-rtm runs from a
# copy of vc6/bin with the RTM front ends swapped in (out/msvc/vc6-rtm/bin).
# Wine uses its own prefix in out/wine (WINEPREFIX overrides).
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
if ! command -v wine >/dev/null 2>&1 && [ "$(uname)" = Darwin ]; then
  for dir in /opt/local/bin /opt/homebrew/bin; do
    [ -x "$dir/wine" ] && PATH="$dir:$PATH"
  done
  WINE_BIN="/Applications/Wine Devel.app/Contents/Resources/wine/bin"
  [ -x "$WINE_BIN/wine" ] && PATH="$WINE_BIN:$PATH"
fi
command -v wine >/dev/null 2>&1 || {
  echo "Wine was not found. Install Wine and put its bin directory on PATH." >&2
  exit 1
}
setup=$1
shift || true
tool=cl
case "$1" in cl|link|lib|dumpbin|editbin|nmake|shell) tool=$1; shift ;; esac

case "$setup" in
  vc5) D=$HERE/vc5; BIN=$D/bin ;;
  vc6) D=$HERE/vc6; BIN=$D/bin ;;
  vc6-rtm)
    D=$HERE/vc6
    BIN=$ROOT/out/msvc/vc6-rtm/bin
    if [ ! -f "$BIN/.ok" ] || [ "$D/bin-rtm/c1xx.dll" -nt "$BIN/.ok" ]; then
      rm -rf "$BIN"; mkdir -p "$BIN"
      cp "$D"/bin/* "$BIN"/
      cp "$D"/bin-rtm/c1.dll "$D"/bin-rtm/c1xx.dll "$BIN"/
      touch "$BIN/.ok"
    fi ;;
  *) echo "usage: $0 <vc5|vc6|vc6-rtm> [cl|link|lib|dumpbin|editbin] args..." >&2; exit 2 ;;
esac

winpath() { local p=$1; echo "Z:${p//\//\\}"; }
export WINEPREFIX=${WINEPREFIX:-$ROOT/out/wine}
export WINEDEBUG=${WINEDEBUG:--all}
export WINEDLLOVERRIDES=${WINEDLLOVERRIDES:-mscoree,mshtml=}
mkdir -p "$WINEPREFIX"
export WINEPATH="$(winpath "$BIN")"
export INCLUDE="$(winpath "$D/include");$(winpath "$D/mfc/include");$(winpath "$D/atl/include")"
export LIB="$(winpath "$D/lib");$(winpath "$D/mfc/lib")"
# `shell` keeps one Windows command prompt open with this environment: commands
# written to it start in milliseconds, while every fresh `wine` start costs a second or more
[ "$tool" = shell ] && exec wine cmd.exe /q /v:on
exec wine "$BIN/$tool.exe" "$@"
