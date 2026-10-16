# mflinalg edits for the 64-bit port (see port/rewrite.py, EDITS).
#
# MfMath reads and writes float bit patterns through `unsigned long *`, an 8-byte
# access on LP64 to a 4-byte float: 32-bit words here.
EDITS = {
    'mflinalg/MfMath.cpp': [
        ('        unsigned long *fi = (unsigned long *)&f;\n',
         '        port_u32 *fi = (port_u32 *)&f;    // PORT: float bits are 32-bit\n', 1),
        ('    unsigned long bits = *(unsigned long *)&x;\n',
         '    port_u32 bits = *(port_u32 *)&x;    // PORT: float bits are 32-bit\n', 1),
        ('    unsigned long mant = bits & 0x7fffff;\n',
         '    port_u32 mant = bits & 0x7fffff;    // PORT: 32-bit\n', 1),
        ('    *(unsigned long *)&x = ((long)_sqrttab[mant >> MANT_SHIFT] << MANT_SHIFT) | ((exp + 127) << 23);\n',
         '    *(port_u32 *)&x = ((port_i32)_sqrttab[mant >> MANT_SHIFT] << MANT_SHIFT) | ((exp + 127) << 23);    // PORT: 32-bit\n', 1),
    ],
}
