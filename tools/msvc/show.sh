#!/bin/bash
# Front end for tools/recomp/show.py (see its docstring).
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
PYTHON=${PYTHON:-$ROOT/.venv/bin/python}
[ -x "$PYTHON" ] || PYTHON=python3
exec "$PYTHON" "$HERE/../recomp/show.py" "$@"
