#pragma once
class MfPcCodeChunk;

// PiccoloRT.dll: the bytecode set. The compiler reads the instruction sizes.
class __declspec(dllimport) MfScBytecodes {   // own copy: src/PiccoloRT/MfScBytecodes.h is not dllimport and has no friend for MfPcCodeChunk (aiBytecodeSize)
    friend class MfPcCodeChunk;
public:
    static int BytecodeSize(unsigned char);
    static int IsJump(unsigned char);
    static int JumpOffset(unsigned char *);
private:
    static int aiBytecodeSize[];    // bytes of each instruction, by opcode
};
