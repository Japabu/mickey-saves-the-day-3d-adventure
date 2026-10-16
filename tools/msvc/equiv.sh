#!/bin/bash
# Is a function that does not compile to identical bytes still the original's logic?
#
# usage: equiv.sh <dll> <va> <file.cpp> [--sym NAME]
#        equiv.sh --todos [-j N] [filter...]    every TODO with a body
#        equiv.sh --claims [-j N] [filter...]   self-test on matched functions
# See tools/recomp/equiv.py for what the verdicts prove.
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
PYTHON=${PYTHON:-$ROOT/.venv/bin/python}
[ -x "$PYTHON" ] || PYTHON=python3
exec "$PYTHON" "$HERE/../recomp/equiv.py" "$@"
