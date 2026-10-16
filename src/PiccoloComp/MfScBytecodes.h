#pragma once
class MfPcCodeChunk;

// PiccoloRT.dll: the bytecode set. The compiler reads the instruction sizes.
class __declspec(dllimport) MfScBytecodes {
    friend class MfPcCodeChunk;
public:
    static int BytecodeSize(unsigned char);
    static int IsJump(unsigned char);
    static int JumpOffset(unsigned char *);
private:
    static int aiBytecodeSize[];    // bytes of each instruction, by opcode
};
