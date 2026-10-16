#pragma once

class MfScBytecodes {
public:
    MfScBytecodes & operator=(MfScBytecodes const &);
    MfScBytecodes();
    static int __cdecl BytecodeSize(unsigned char);
    static int __cdecl IsJump(unsigned char);
    static int __cdecl IsJumpNotZero(unsigned char);
    static int __cdecl IsJumpZero(unsigned char);
    static int __cdecl IsUnconditionalJump(unsigned char);
    static int __cdecl JumpOffset(unsigned char *);
    ~MfScBytecodes();
private:
    static int * aiBytecodeSize;
public:
    char field_0;
};
