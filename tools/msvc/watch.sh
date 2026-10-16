#!/bin/bash
# Re-run match.sh every time the source file is saved (options from src/options.txt
# unless given). Run from src/.
# usage: watch.sh <bin> <va> <file> [cl options] [--sym NAME]    (Ctrl-C to stop)
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
PYTHON=${PYTHON:-$ROOT/.venv/bin/python}
[ -x "$PYTHON" ] || PYTHON=python3
bin=$1 va=$2 file=$3; shift 3
last=
while true; do
  now=$(cksum "$file")
  if [ "$now" != "$last" ]; then
    last=$now
    clear
    echo "== $bin $va $file $*   ($(date +%T))"
    "$HERE/match.sh" "$bin" "$va" "$file" "$@" --show 2>&1 \
      | grep -v -e 'while compiling class-template' -e '^[[:space:]]*$'
  fi
  sleep 0.5
done
