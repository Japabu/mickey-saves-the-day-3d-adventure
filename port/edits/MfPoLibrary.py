# MfPoLibrary edits for the port (see port/rewrite.py, EDITS).
#
# typeid(MfPlList) (MfPoActor's argument errors): PscStdlib, which defines the class,
# is a plugin MfPoLibrary does not link; with no key function here the type_info is
# emitted locally (it only names the type in a message).
EDITS = {
    'MfPoLibrary/MfScHandleList.h': [
        ('    virtual ~MfPlList();\n', '    virtual ~MfPlList() {}    // PORT: no key function here, so the type_info is local\n', 1),
    ],
}

# MfPoLibrary/MfPoBase.h: InvalidType's variable arguments are all pointers; an overload passes literal 0 arguments on as null pointers (an int in `...` is not a pointer on arm64).
EDITS['MfPoLibrary/MfPoBase.h'] = [
    ('class MfProperty;\n'
     'class MfTypedValue;\n'
     'class type_info;\n'
     '\n'
     'class __declspec(dllexport) MfPoBase : public MfScObject {\n',
     'class MfProperty;\n'
     'class MfTypedValue;\n'
     '#include <typeinfo.h> // port: was a declaration of class type_info\n'
     '\n'
     'class __declspec(dllexport) MfPoBase : public MfScObject {\n', 1),
    ('    const type_info &getType(MfScDataHandle *, const type_info *);\n'
     '    void __cdecl InvalidType(MfString, MfString, int, int, MfScDataHandle **, ...);\n'
     '    void __cdecl WrongNumber(MfString, MfString, int, int, ...);\n'
     '    static MfScDataHandle *__cdecl PropertyToPiccolo(MfProperty *);\n',
     '    const type_info &getType(MfScDataHandle *, const type_info *);\n'
     '    void __cdecl InvalidType(MfString, MfString, int, int, MfScDataHandle **, ...);\n'
     '    // PORT: callers pass a literal 0 for "no expected type" among the const type_info *\n'
     "    // arguments; through `...` that is a 32-bit int, and on arm64 InvalidType's 64-bit\n"
     '    // va_arg would read garbage in the upper half. This overload takes the calls first\n'
     '    // and hands every argument on as a pointer (an int becomes a null pointer).\n'
     '    template <class... A>\n'
     '    void InvalidType(MfString cls, MfString func, int nPrefixes, int nParams, MfScDataHandle **params, A... a)\n'
     '    {\n'
     '        void (__cdecl MfPoBase::*variadic)(MfString, MfString, int, int, MfScDataHandle **, ...) = &MfPoBase::InvalidType;\n'
     '        (this->*variadic)(cls, func, nPrefixes, nParams, params, PortVarargPointer(a)...);\n'
     '    }\n'
     '    static const void *PortVarargPointer(int v) { return (const void *)(intptr_t)v; }\n'
     '    template <class T> static T *PortVarargPointer(T *p) { return p; }\n'
     '    void __cdecl WrongNumber(MfString, MfString, int, int, ...);\n'
     '    static MfScDataHandle *__cdecl PropertyToPiccolo(MfProperty *);\n'
     '#line 23\n', 1),
]

# MfPoLibrary/MfThread.h: the members have the types of the real class in mfutil (pointers where this view had int), so the inline MfScThread::GetVmEnv() reads mVmEnv at the real offset on 64-bit.
EDITS['MfPoLibrary/MfThread.h'] = [
    ('    virtual void ThreadEntry();\n'
     'private:\n'
     '    int field_4;\n'
     '    int field_8;\n'
     '    int field_c;\n'
     '    int field_10;\n'
     '    unsigned int mState;                                    // 0x14\n'
     '    int field_18;\n'
     '    int field_1c;\n'
     '};\n',
     '    virtual void ThreadEntry();\n'
     'private:\n'
     "    // PORT: the member types of mfutil's MfThread (src/mfutil/MfThread.h), so\n"
     '    // MfScThread::mVmEnv is at the real offset (+64 on 64-bit)\n'
     '    unsigned int field_4;                                  // dwThreadId\n'
     '    int field_8;                                            // iFlags\n'
     '    MfThread *field_c;                                      // pThread\n'
     '    void *field_10;                                         // hThread\n'
     '    unsigned int mState;                                    // 0x14\n'
     '    void *field_18;                                         // pevtWork\n'
     '    void *field_1c;                                         // pWork\n'
     '};\n'
     '#line 20\n', 1),
]
