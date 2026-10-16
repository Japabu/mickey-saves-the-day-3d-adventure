"""The original game code in an x86 emulator (unicorn), for differential tests.

Machine(dll) maps the original DLL from orig/binaries at its own image base, so the
addresses in the source claims stay valid; the game DLLs it imports from are mapped at
other bases (relocated) and linked by their exports. Imports from Windows and the C
runtime go to small stubs: the C runtime's memory, string and math functions are
implemented here, everything else stops the run with the function's name.

Math functions pass their values on the x87 stack, which unicorn does not expose as
numbers: their stubs store the stack top to memory (fstp), run Python on the value, and
load the result back (fld), in x86 code.
"""
import math
import os
import struct
import sys

from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_PROT_ALL
from unicorn.x86_const import (UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_ESP,
                               UC_X86_REG_EIP, UC_X86_REG_GDTR, UC_X86_REG_FS, UC_X86_REG_CS,
                               UC_X86_REG_DS, UC_X86_REG_SS, UC_X86_REG_ES,
                               UC_X86_REG_FPCW)

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, 'tools', 'recomp'))
import pe  # noqa: E402

BINARIES = os.path.join(ROOT, 'orig', 'binaries')
STUBS, STUB_SIZE = 0x7f000000, 0x40     # one stub per imported Windows/CRT function
SCRATCH = 0x7e000000                    # x87 values passing through memory
HEAP, HEAP_SIZE = 0x40000000, 0x4000000
STACK, STACK_SIZE = 0x7d000000, 0x100000
RETURN = 0x7e00f000                     # the return address of a call from the harness
DEP_BASE = 0x20000000                   # where imported game DLLs go
GDT, TEB = 0x7e100000, 0x7e110000       # the segments; fs points to a thread block


class EmuError(Exception):
    pass


def _find_binary(name):
    for f in os.listdir(BINARIES):
        if f.lower() == name.lower():
            return os.path.join(BINARIES, f)
    return None


class Machine:
    def __init__(self, dll):
        self.uc = Uc(UC_ARCH_X86, UC_MODE_32)
        self.images = {}            # lower-case dll name -> (PE, base)
        self.stub_names = {}        # stub address -> import name
        self.handlers = {}          # hook address -> python function
        self.next_dep = DEP_BASE
        self.heap_top = HEAP
        self.uc.mem_map(STUBS, 0x100000, UC_PROT_ALL)
        self.uc.mem_map(SCRATCH, 0x10000, UC_PROT_ALL)
        self.uc.mem_map(HEAP, HEAP_SIZE, UC_PROT_ALL)
        self.uc.mem_map(STACK, STACK_SIZE, UC_PROT_ALL)
        self.next_stub = STUBS
        self._segments()
        self.uc.hook_add(UC_HOOK_CODE, self._on_code, begin=STUBS, end=STUBS + 0x100000 - 1)
        self.main = self.load(dll, at_base=True)

    def _segments(self):
        # flat code/data segments and fs at the thread block (fs:[0] is the SEH chain,
        # ending in -1); the harness never raises exceptions
        self.uc.mem_map(GDT, 0x10000, UC_PROT_ALL)
        self.uc.mem_map(TEB, 0x10000, UC_PROT_ALL)

        def desc(base, limit, access):
            flags = 0xc    # 4K granularity, 32-bit
            return struct.pack('<HHBBBB', limit & 0xffff, base & 0xffff, (base >> 16) & 0xff, access,
                               ((limit >> 16) & 0xf) | (flags << 4), (base >> 24) & 0xff)
        gdt = bytes(8) + desc(0, 0xfffff, 0x9b) + desc(0, 0xfffff, 0x93) + desc(TEB, 0xf, 0xf3) \
            + desc(0, 0xfffff, 0x93)
        self.uc.mem_write(GDT, gdt)
        self.uc.reg_write(UC_X86_REG_GDTR, (0, GDT, len(gdt) - 1, 0))
        self.uc.mem_write(TEB, struct.pack('<IIII', 0xffffffff, STACK + STACK_SIZE, STACK, 0))
        self.uc.mem_write(TEB + 0x18, struct.pack('<I', TEB))
        self.uc.reg_write(UC_X86_REG_CS, 1 << 3)
        self.uc.reg_write(UC_X86_REG_DS, 2 << 3)
        self.uc.reg_write(UC_X86_REG_ES, 2 << 3)
        self.uc.reg_write(UC_X86_REG_SS, 4 << 3)
        self.uc.reg_write(UC_X86_REG_FS, (3 << 3) | 3)

    # ---- loading ----------------------------------------------------------------
    def load(self, name, at_base=False):
        key = name.lower()
        if key in self.images:
            return self.images[key][1]
        path = _find_binary(name)
        if not path:
            return None
        image = pe.PE(path)
        if at_base:
            base = image.image_base
        else:
            base = self.next_dep
            self.next_dep += (image.size_of_image + 0xfffff) & ~0xfffff
        size = (image.size_of_image + 0xfff) & ~0xfff
        self.uc.mem_map(base, size, UC_PROT_ALL)
        self.uc.mem_write(base, image.data[:image.size_of_headers])
        for s in image.sections:
            raw = image.data[s.rawptr:s.rawptr + min(s.rawsize, s.vsize or s.rawsize)]
            self.uc.mem_write(base + s.rva, raw)
        delta = base - image.image_base
        if delta:
            for site, target in image.relocs.items():
                self.uc.mem_write(site + delta, struct.pack('<I', (target + delta) & 0xffffffff))
        self.images[key] = (image, base)
        # imports: other game DLLs are loaded and linked, the rest gets stubs
        for imp in image.imports:
            target = None
            dep = self.load(imp.dll) if _find_binary(imp.dll) else None
            if dep is not None:
                target = self._export(imp.dll, imp.name, imp.ordinal)
            if target is None:
                target = self._stub(imp.name or f'{imp.dll}#{imp.ordinal}')
            self.uc.mem_write(imp.iat_va + delta, struct.pack('<I', target))
        return base

    def _export(self, dll, name, ordinal):
        image, base = self.images[dll.lower()]
        e = image.exports
        if not e:
            return None
        index = None
        if name is not None:
            for n, o in zip(e['strnames'], e['ords']):
                if n == name:
                    index = o
                    break
        else:
            index = ordinal - e['base']
        if index is None or index >= len(e['funcs']):
            return None
        return base + e['funcs'][index]

    def address(self, dll, va):
        """A VA of the original dll (as in the claims) where this machine mapped it."""
        image, base = self.images[dll.lower()]
        return va - image.image_base + base

    # ---- stubs for Windows and the C runtime ------------------------------------------
    def _stub(self, name):
        addr = self.next_stub
        self.next_stub += STUB_SIZE
        self.stub_names[addr] = name
        code, hook_offset, handler = self._stub_code(name)
        self.uc.mem_write(addr, code)
        self.handlers[addr + hook_offset] = handler
        return addr

    def _stub_code(self, name):
        """(code, offset of the hooked instruction, handler) for an imported function."""
        nop_ret = bytes([0x90, 0xc3])
        unary = {'_CIsqrt': math.sqrt, '_CIacos': math.acos, '_CIasin': math.asin,
                 '_CIatan': math.atan, '_CIsin': math.sin, '_CIcos': math.cos, '_CItan': math.tan,
                 '_CIexp': math.exp, '_CIlog': math.log}
        binary = {'_CIatan2': math.atan2, '_CIpow': math.pow, '_CIfmod': math.fmod}
        cdecl1 = {'sqrt': math.sqrt, 'acos': math.acos, 'asin': math.asin, 'atan': math.atan,
                  'sin': math.sin, 'cos': math.cos, 'tan': math.tan, 'exp': math.exp,
                  'log': math.log, 'fabs': abs, 'floor': math.floor, 'ceil': math.ceil}
        cdecl2 = {'atan2': math.atan2, 'pow': math.pow, 'fmod': math.fmod}
        a, b = SCRATCH, SCRATCH + 8
        fstp_a = bytes([0xdd, 0x1d]) + struct.pack('<I', a)       # fstp qword [a]
        fstp_b = bytes([0xdd, 0x1d]) + struct.pack('<I', b)
        fld_a = bytes([0xdd, 0x05]) + struct.pack('<I', a)        # fld qword [a]
        if name in unary:
            f = unary[name]
            return fstp_a + bytes([0x90]) + fld_a + bytes([0xc3]), len(fstp_a), \
                lambda m: m._set_double(a, f(m._double(a)))
        if name in binary:
            f = binary[name]   # ST1 op ST0 (x in ST1, y in ST0 for atan2/pow)
            return fstp_b + fstp_a + bytes([0x90]) + fld_a + bytes([0xc3]), len(fstp_b + fstp_a), \
                lambda m: m._set_double(a, f(m._double(a), m._double(b)))
        if name in cdecl1:
            f = cdecl1[name]

            def h(m, f=f):
                m._set_double(a, f(m._arg_double(0)))
            return bytes([0x90]) + fld_a + bytes([0xc3]), 0, h
        if name in cdecl2:
            f = cdecl2[name]

            def h(m, f=f):
                m._set_double(a, f(m._arg_double(0), m._arg_double(2)))
            return bytes([0x90]) + fld_a + bytes([0xc3]), 0, h
        if name == '_ftol':   # ST0 -> edx:eax, truncated
            def h(m):
                v = int(m._double(a))
                m.uc.reg_write(UC_X86_REG_EAX, v & 0xffffffff)
                m.uc.reg_write(UC_X86_REG_EDX, (v >> 32) & 0xffffffff)
            return fstp_a + bytes([0x90, 0xc3]), len(fstp_a), h
        simple = {
            'memcpy': self._memcpy, 'memmove': self._memcpy, 'memset': self._memset,
            'strlen': self._strlen, 'malloc': self._malloc, '??2@YAPAXI@Z': self._malloc,
            'free': self._noop, '??3@YAXPAX@Z': self._noop, 'operator delete': self._noop,
        }
        if name in simple:
            return nop_ret, 0, simple[name]
        if name in ('_adjust_fdiv', '_fltused'):   # data imports: a zero dword
            return bytes(8), 999, None
        return nop_ret, 0, lambda m, name=name: m._unknown(name)

    def _on_code(self, uc, address, size, user):
        h = self.handlers.get(address)
        if h:
            h(self)

    def _unknown(self, name):
        self.uc.emu_stop()
        self.error = f'called {name}, which the harness does not implement'

    # C runtime functions (cdecl: arguments above the return address)
    def _arg(self, i):
        esp = self.uc.reg_read(UC_X86_REG_ESP)
        return struct.unpack('<I', self.uc.mem_read(esp + 4 + 4 * i, 4))[0]

    def _arg_double(self, i):
        esp = self.uc.reg_read(UC_X86_REG_ESP)
        return struct.unpack('<d', self.uc.mem_read(esp + 4 + 4 * i, 8))[0]

    def _double(self, at):
        return struct.unpack('<d', self.uc.mem_read(at, 8))[0]

    def _set_double(self, at, v):
        self.uc.mem_write(at, struct.pack('<d', v))

    def _memcpy(self, m):
        dst, src, n = self._arg(0), self._arg(1), self._arg(2)
        self.uc.mem_write(dst, bytes(self.uc.mem_read(src, n)))
        self.uc.reg_write(UC_X86_REG_EAX, dst)

    def _memset(self, m):
        dst, c, n = self._arg(0), self._arg(1), self._arg(2)
        self.uc.mem_write(dst, bytes([c & 0xff]) * n)
        self.uc.reg_write(UC_X86_REG_EAX, dst)

    def _strlen(self, m):
        p, n = self._arg(0), 0
        while self.uc.mem_read(p + n, 1)[0]:
            n += 1
        self.uc.reg_write(UC_X86_REG_EAX, n)

    def _malloc(self, m):
        self.uc.reg_write(UC_X86_REG_EAX, self.alloc(self._arg(0)))

    def _noop(self, m):
        pass

    # ---- memory and calls ------------------------------------------------------
    def alloc(self, size, data=None):
        addr = self.heap_top
        self.heap_top = (self.heap_top + max(size, 4) + 15) & ~15
        if self.heap_top > HEAP + HEAP_SIZE:
            raise EmuError('emulator heap exhausted')
        self.uc.mem_write(addr, data if data is not None else bytes(size))
        return addr

    def write(self, addr, data):
        self.uc.mem_write(addr, data)

    def read(self, addr, n):
        return bytes(self.uc.mem_read(addr, n))

    def floats(self, addr, n):
        return list(struct.unpack(f'<{n}f', self.read(addr, 4 * n)))

    # The x87 precision the code ran with: Direct3D set the game's main thread to 24 bits
    # (no DDSCL_FPUPRESERVE), the C runtime's default for other threads (the walk
    # planner's) is 53 bits.
    PRECISION = {24: 0x007f, 53: 0x027f, 64: 0x037f}

    def call(self, dll, va, args=(), this=None, callee_pops=False, limit=10_000_000, precision=24):
        """Runs the function at the original VA of dll (thiscall when this is given, else
        cdecl/stdcall) with 32-bit arguments and the x87 at the given precision; returns eax."""
        self.error = None
        self.uc.reg_write(UC_X86_REG_FPCW, self.PRECISION[precision])
        sp = STACK + STACK_SIZE - 0x100
        for a in reversed(args):
            sp -= 4
            self.uc.mem_write(sp, struct.pack('<I', a & 0xffffffff))
        sp -= 4
        self.uc.mem_write(sp, struct.pack('<I', RETURN))
        self.uc.reg_write(UC_X86_REG_ESP, sp)
        if this is not None:
            self.uc.reg_write(UC_X86_REG_ECX, this)
        start = self.address(dll, va)
        try:
            self.uc.emu_start(start, RETURN, count=limit)
        except UcError as e:
            eip = self.uc.reg_read(UC_X86_REG_EIP)
            raise EmuError(f'{e} at {eip:#x}') from None
        if self.error:
            raise EmuError(self.error)
        if self.uc.reg_read(UC_X86_REG_EIP) != RETURN:
            raise EmuError(f'did not return (instruction limit) at {self.uc.reg_read(UC_X86_REG_EIP):#x}')
        return self.uc.reg_read(UC_X86_REG_EAX)
