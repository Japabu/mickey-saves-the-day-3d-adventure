#pragma once
class MfDblList;
class MfDictionary;
class MfPcIdentifier;
class MfString;

// The identifiers of the compilation: a dictionary by name and a list in
// declaration order (0x18 bytes).
class MfPcSymbolTable {
public:
    MfPcSymbolTable();
    virtual ~MfPcSymbolTable();
    MfPcIdentifier *Add(MfString const &name);
    MfPcIdentifier *Find(MfString const &name);
    void Reset();
    void FlushLocals();
    void FlushGlobalTemps();
    int EndBlock();

    int mMaxLocals;             // +0x04 most locals alive at one time
    int mNumLocals;             // +0x08
    int mLevel;                 // +0x0c block nesting level
    MfDictionary *mSymbols;     // +0x10 name -> MfPcIdentifier
    MfDblList *mIdentifiers;    // +0x14 MfPcIdentifier, in order
};
