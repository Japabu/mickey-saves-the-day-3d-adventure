#pragma once
#include "MfDblList.h"
#include "MfPcParserItem.h"
class MfPcCodeChunk;
class MfScBytecodeRoutine;

// The code of a function as a list of MfPcCodeChunk: instructions, jumps,
// labels and the debug records (line numbers, code levels, variables,
// constants) that become the routine's MfScDebugInfo.
class MfPcChunkList : public MfDblList, public MfPcParserItem {
public:
    MfPcChunkList() {}
    virtual ~MfPcChunkList() {}
    void Dump();
    int CodeSize();
    void CopyCode(unsigned char *p);
    void AssignOffsets();
    void SetJumpSizes(int size, int all);
    void ComputeDisplacements();
    void AssembleJumps();
    int ShrinkJumps(int);
    void Walk();
    void BuildLineInfo(MfScBytecodeRoutine *routine);
    void BuildVariableInfo(MfScBytecodeRoutine *routine);
    void BuildCodeLevelInfo(MfScBytecodeRoutine *routine);
    void BuildConstUsageInfo(MfScBytecodeRoutine *routine);
    void ListParameters(MfScBytecodeRoutine *routine);
    void AddLineInfo();
};
