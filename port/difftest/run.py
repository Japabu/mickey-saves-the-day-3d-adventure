"""Differential tests: original functions (in the emulator) against the port's.

usage: .venv/bin/python port/difftest/run.py [name substring] [-v]
Needs out/port/build/port_tests (built with the port) and orig/binaries. Each case's
inputs go through the original function and through the port's probe; the results are
compared as 32-bit floats (relative tolerance per case), NaN only equal to NaN.
"""
import math
import os
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import cases  # noqa: E402
import emu  # noqa: E402

PORT_TESTS = os.path.join(ROOT, 'out', 'port', 'build', 'port_tests')


def same(a, b, tol):
    if math.isnan(a) or math.isnan(b):
        return math.isnan(a) and math.isnan(b)
    return abs(a - b) <= tol * max(1.0, abs(a), abs(b))


def run_case(case, verbose):
    inputs = list(case.inputs())
    proc = subprocess.run([PORT_TESTS, '--probe', case.name], input=''.join(i.hex() + '\n' for i in inputs),
                          capture_output=True, text=True, env=dict(os.environ))
    if proc.returncode:
        print(f'{case.name}: port probe failed: {proc.stderr.strip()[-300:]}')
        return False
    ported = [bytes.fromhex(l) for l in proc.stdout.split('\n') if l]
    bad = 0
    for k, (data, port) in enumerate(zip(inputs, ported)):
        m = emu.Machine(case.dll)
        try:
            orig = case.original(m, data)
        except emu.EmuError as e:
            print(f'{case.name} #{k}: original failed: {e}')
            return False
        n = min(len(orig), len(port)) // 4
        o = struct.unpack(f'<{n}f', orig[:4 * n])
        p = struct.unpack(f'<{n}f', port[:4 * n])
        diff = [i for i in range(n) if not same(o[i], p[i], case.tolerance)]
        if diff or len(orig) != len(port):
            bad += 1
            if bad <= 5 or verbose:
                ins = struct.unpack(f'<{len(data) // 4}f', data[:len(data) // 4 * 4])
                print(f'{case.name} #{k}: differs at {diff[:6]}\n    input    {[round(x, 6) for x in ins]}'
                      f'\n    original {[round(x, 6) for x in o]}\n    port     {[round(x, 6) for x in p]}')
    print(f'{case.name}: {len(inputs) - bad}/{len(inputs)} agree')
    return bad == 0


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    verbose = '-v' in sys.argv
    ok = True
    for case in cases.CASES:
        if args and not any(a in case.name for a in args):
            continue
        ok &= run_case(case, verbose)
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
