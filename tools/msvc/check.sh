#!/bin/bash
# Check every function the source claims (see tools/recomp/check.py).
# usage: check.sh [-j N] [-v] [file, directory or binary ...]
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
PYTHON=${PYTHON:-$ROOT/.venv/bin/python}
[ -x "$PYTHON" ] || PYTHON=python3
exec "$PYTHON" "$HERE/../recomp/check.py" "$@"
