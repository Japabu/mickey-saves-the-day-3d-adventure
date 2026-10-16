"""The differential test cases: each runs one original function in the emulator and the
port's version (a PROBE in port/tests/<Module>/probes.cpp) on the same inputs.

A case is Case(name, dll, va, inputs, original): inputs() yields the input bytes, the
original(machine, input) function lays them out the way the original expects, calls it
and returns the result bytes in the probe's format.
"""
import random
import struct

F = struct.Struct('<f')


def floats(*v):
    return struct.pack(f'<{len(v)}f', *v)


class Case:
    def __init__(self, name, dll, va, inputs, original, tolerance=1e-4):
        self.name, self.dll, self.va = name, dll, va
        self.inputs, self.original, self.tolerance = inputs, original, tolerance


def unit(rng):
    while True:
        v = [rng.uniform(-1, 1) for _ in range(3)]
        n = sum(x * x for x in v) ** 0.5
        if n > 0.1:
            return [x / n for x in v]


# ---- SteamboatApp -------------------------------------------------------------------------

def refframe_inputs():
    rng = random.Random(1)
    # the view camera's set-up (the NaN case), an up along the view, then random frames
    yield floats(0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 1, 0)
    yield floats(0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 1)
    yield floats(0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, -1)
    for _ in range(200):
        at = [rng.uniform(-10, 10) for _ in range(3)]
        frm = [rng.uniform(-10, 10) for _ in range(3)]
        yield floats(*at, *frm, *unit(rng), *unit(rng))


def refframe_original(m, data):
    v = struct.unpack('<12f', data)
    # EAIRefFrame: type 0x00, At 0x04, From 0x10, Up 0x1c, upLock 0x28
    obj = m.alloc(0x2c, struct.pack('<i9fi', 0, *v[:9], 0))
    up = m.alloc(12, floats(*v[9:]))
    m.call('SteamboatApp.dll', 0x100dc870, [up], this=obj)
    return floats(*m.floats(obj + 4, 9))


# ---- RuntimeLib ---------------------------------------------------------------------------

def path_inputs():
    rng = random.Random(2)
    # the main menu's walk in place: one spot, the last copy a float crumb off
    yield struct.pack('<i', 3) + floats(12.8283081, -1.64587998, 12.8283081, -1.64587998,
                                        12.8283081, -1.64588022)
    yield struct.pack('<i', 3) + floats(0, 0, 0, 1, 0, 2)
    for _ in range(200):
        n = rng.randint(2, 12)
        pts = []
        x, y = rng.uniform(-20, 20), rng.uniform(-20, 20)
        for _ in range(n):
            pts += [x, y]
            if rng.random() < 0.2:
                continue              # the same point again
            x += rng.uniform(-1, 1)
            y += rng.uniform(-1, 1)
        yield struct.pack('<i', n) + floats(*pts)


def path_original(m, data):
    n = struct.unpack_from('<i', data)[0]
    xy = struct.unpack_from(f'<{2 * n}f', data, 4)
    points = m.alloc(12 * n, b''.join(floats(xy[2 * i], xy[2 * i + 1], 0) for i in range(n)))
    # MfPath2: mCount 0x1c, mPoints 0x20 (object 0x38 bytes)
    obj = m.alloc(0x38)
    m.write(obj + 0x1c, struct.pack('<iI', n, points))
    m.call('RuntimeLib.dll', 0x1004a070, [], this=obj)
    return floats(*[m.floats(points + 12 * i + 8, 1)[0] for i in range(n)])


# ---- mflinalg (exact: tolerance 0) ----------------------------------------------------------

def rotation(rng):
    q = [rng.uniform(-1, 1) for _ in range(4)]
    n = sum(x * x for x in q) ** 0.5
    return [x / n for x in q]


def transform_inputs(with_point):
    def gen():
        rng = random.Random(3)
        for _ in range(300):
            t = [rng.uniform(-20, 20) for _ in range(3)] + rotation(rng)
            p = [rng.uniform(-20, 20) for _ in range(3)] if with_point else []
            yield floats(*t, *p)
    return gen


def transform_times_vector_original(m, data):
    t = m.alloc(28, data[:28])
    p = m.alloc(12, data[28:40])
    out = m.alloc(12)
    m.call('mflinalg.dll', 0x10005790, [out, p], this=t)    # returned through a hidden pointer
    return m.read(out, 12)


def transform_inverse_original(m, data):
    t = m.alloc(28, data[:28])
    out = m.alloc(28)
    m.call('mflinalg.dll', 0x10005350, [out], this=t)
    return m.read(out, 28)


def rotation_inputs():
    rng = random.Random(4)
    for _ in range(300):
        yield floats(*rotation(rng), *[rng.uniform(-20, 20) for _ in range(3)])


def rotation_times_vector_original(m, data):
    r = m.alloc(16, data[:16])
    p = m.alloc(12, data[16:28])
    out = m.alloc(12)
    m.call('mflinalg.dll', 0x10003d90, [out, p], this=r)
    return m.read(out, 12)


CASES = [
    Case('EAIRefFrame_validateNewUpVec', 'SteamboatApp.dll', 0x100dc870, refframe_inputs,
         refframe_original),
    Case('MfPath2_Update', 'RuntimeLib.dll', 0x1004a070, path_inputs, path_original),
    Case('MfTransform3_times_vector', 'mflinalg.dll', 0x10005790, transform_inputs(True),
         transform_times_vector_original, tolerance=0),
    Case('MfTransform3_inverse', 'mflinalg.dll', 0x10005350, transform_inputs(False),
         transform_inverse_original, tolerance=0),
    Case('MfRotation3_times_vector', 'mflinalg.dll', 0x10003d90, rotation_inputs,
         rotation_times_vector_original, tolerance=0),
]
