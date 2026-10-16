#!/bin/bash
# Where might a function that does not match do something else than the original?
# Compares calls, globals/strings, constants and member offsets of original and
# compiled code for every TODO with a body, most suspicious first.
#
# usage: logicdiff.sh <dll> <va> <file.cpp> [--sym NAME]
#        logicdiff.sh [-j N] [--all] [filter...]
# See tools/recomp/logicdiff.py for what it compares.
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
PYTHON=${PYTHON:-$ROOT/.venv/bin/python}
[ -x "$PYTHON" ] || PYTHON=python3
exec "$PYTHON" "$HERE/../recomp/logicdiff.py" "$@"
