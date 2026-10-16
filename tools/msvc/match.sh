#!/bin/bash
# Compile one function with the original compiler and print the byte diff
# against the original binary, relocated fields masked.
#
# usage: match.sh <dll> <va> <file.c|file.cpp> [--sym NAME] [--cc vc5|vc6|vc6-rtm] [--show] [cl options...]
#   <dll>  name in orig/binaries (or a path); the compiler defaults to vc5 for the
#          Motion Factory DLLs and vc6 otherwise.
#   --show prints the side-by-side listing even when the bytes match.
# Exit status 0 = identical outside relocated fields. See tools/recomp/match.py.
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
PYTHON=${PYTHON:-$ROOT/.venv/bin/python}
[ -x "$PYTHON" ] || PYTHON=python3
exec "$PYTHON" "$HERE/../recomp/match.py" "$@"
