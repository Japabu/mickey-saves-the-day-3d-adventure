# MfActorLib edits for the 32-bit integer model (see rewrite.windows_long): with long
# rewritten to int, MfStream's long overloads repeat the int ones and are compiled out.
# They apply after MfActorLib.py's edits (edit files apply in name order).
EDITS = {
    'MfActorLib/MfStream.cpp': [
        ('MfStream &MfStream::operator>>(long &i)\n{\n'
         '    port_i32 v = 0; ReadBytes(sizeof(v), &v); i = v;    // PORT: a long is 4 bytes in the stream\n'
         '    return *this;\n}\n',
         '#if 0 /* port: the int overload serves (long is int) */\n{\n'
         '    port_i32 v = 0; ReadBytes(sizeof(v), &v); i = v;    // PORT: a long is 4 bytes in the stream\n'
         '    return *this;\n#endif\n', 1),
        ('MfStream &MfStream::operator<<(long i)\n{\n'
         '    port_i32 v = (port_i32)i; WriteBytes(sizeof(v), &v);    // PORT: a long is 4 bytes in the stream\n'
         '    return *this;\n}\n',
         '#if 0 /* port: the int overload serves (long is int) */\n{\n'
         '    port_i32 v = (port_i32)i; WriteBytes(sizeof(v), &v);    // PORT: a long is 4 bytes in the stream\n'
         '    return *this;\n#endif\n', 1),
        ('int MfStream::SizeOf(long)\n{\n    return 4;\n}\n',
         '#if 0 /* port: the int overload serves (long is int) */\n{\n    return 4;\n#endif\n', 1),
    ],
}
