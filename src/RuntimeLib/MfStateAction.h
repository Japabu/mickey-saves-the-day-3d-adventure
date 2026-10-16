// The script parts of a state: entry and exit actions, init_var, function
// definitions (Piccolo source and the compiled code) and the state variables.
// RTTI names; vtables 100b6154 (MfStateEntryAction, unit 10085600-10085c20),
// 100b62cc (MfStateExitAction, 10086310-10086950), 100b64c4
// (MfStateFunctionDef, 10086fd0-10087690), 100b658c (MfStateInitVar,
// 10087690-10087c00), 100b684c (MfStateVariables, 10089110-10089620). Only
// what MfState uses is declared so far.
#ifndef RUNTIMELIB_MFSTATEACTION_H
#define RUNTIMELIB_MFSTATEACTION_H
#include "MfStateProperty.h"
class MfScDataHandle;
class MfScHandleDict;
class MfScHandleList;
class MfStringList;

class MfStateEntryAction : public MfStateProperty {
public:
    enum { TYPE = 0x66 };

    MfStateEntryAction(MfString source);
    virtual ~MfStateEntryAction();
    virtual MfFSMMutable *Clone();
    virtual void Load(MfScPiccoloObjectFile *objectFile);
    virtual void Link();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures);
    virtual void Unload();
    virtual MfString *GetScript() { return &mSource; }
    virtual void SetScript(const MfString &source);
    virtual void Search(MfString text, int, int caseSensitive, int, int what, int, int, int,
                        MfFSMMutable *&found, MfFSMMutable *&foundIn) {}
    virtual MfStateProperty *GetOwner() { return mOwner; }
    virtual void Slot41(int) {}                                                 // 0xa4
    virtual void Slot42(int) {}                                                 // 0xa8

    // is there more than the braces?
    int HasScript();

    MfString mSource;                                                           // 0x28
    MfScDataHandle *mFunction;                                                  // 0x2c the compiled source
    MfStateProperty *mOwner;                                                    // 0x30 the choice point it belongs to
};

class MfStateExitAction : public MfStateProperty {
public:
    enum { TYPE = 0x67 };

    MfStateExitAction(MfString source);
    virtual ~MfStateExitAction();
    virtual MfFSMMutable *Clone();
    virtual void Load(MfScPiccoloObjectFile *objectFile);
    virtual void Link();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures);
    virtual void Unload();
    virtual MfString *GetScript() { return &mSource; }
    virtual void SetScript(const MfString &source) { mSource = source; }
    virtual void Search(MfString text, int, int caseSensitive, int, int what, int, int, int,
                        MfFSMMutable *&found, MfFSMMutable *&foundIn) {}
    virtual int Slot41() { return 0; }                                          // 0xa4
    virtual void Slot42(int) {}                                                 // 0xa8
    virtual void Slot43(int) {}                                                 // 0xac

    // is there more than the braces?
    int HasScript();

    MfString mSource;                                                           // 0x28
    MfScDataHandle *mFunction;                                                  // 0x2c
};

class MfStateFunctionDef : public MfStateProperty {
public:
    enum { TYPE = 0x6b };

    MfStateFunctionDef(MfString source);
    virtual ~MfStateFunctionDef();
    virtual MfFSMMutable *Clone();
    virtual void Load(MfScPiccoloObjectFile *objectFile);
    virtual void Link();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures);
    virtual void Unload();
    virtual MfString *GetScript() { return &mSource; }
    virtual void SetScript(const MfString &source) { mSource = source; }
    virtual void Search(MfString text, int, int caseSensitive, int, int what, int, int, int,
                        MfFSMMutable *&found, MfFSMMutable *&foundIn) {}
    virtual int Slot41() { return 0; }                                          // 0xa4
    virtual void Slot42(int) {}                                                 // 0xa8
    virtual void Slot43(int) {}                                                 // 0xac
    virtual int Slot44() { return 0; }                                          // 0xb0

    // enters the compiled functions into the variables of a state
    void DefineFunctions(MfScHandleDict *variables);
    // is there more than the braces?
    int HasScript();

    MfString mSource;                                                           // 0x28
    MfScHandleList *mFunctions;                                                 // 0x2c
};

class MfStateInitVar : public MfStateProperty {
public:
    enum { TYPE = 0x70 };

    MfStateInitVar(MfString source);
    virtual ~MfStateInitVar();
    virtual MfFSMMutable *Clone();
    virtual void Load(MfScPiccoloObjectFile *objectFile);
    virtual void Link();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures);
    virtual void Unload();
    virtual MfString *GetScript() { return &mSource; }
    virtual void SetScript(const MfString &source) { mSource = source; }
    virtual void Search(MfString text, int, int caseSensitive, int, int what, int, int, int,
                        MfFSMMutable *&found, MfFSMMutable *&foundIn) {}
    virtual int Slot41() { return 0; }                                          // 0xa4
    virtual void Slot42(int) {}                                                 // 0xa8
    virtual void Slot43(int) {}                                                 // 0xac
    virtual int Slot44() { return 0; }                                          // 0xb0

    // is there more than the braces?
    int HasScript();

    MfString mSource;                                                           // 0x28
    MfScDataHandle *mFunction;                                                  // 0x2c
};

class MfStateVariables : public MfStateProperty {
public:
    enum { TYPE = 0x68 };

    MfStateVariables(MfStringList *names);
    virtual ~MfStateVariables();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void Search(MfString text, int, int caseSensitive, int, int, int, int, int inVariables,
                        MfFSMMutable *&found, MfFSMMutable *&foundIn);

    // enters the variables, without values, into the variables of a state
    void DefineVariables(MfScHandleDict *variables);

    MfStringList *mNames;                                                       // 0x28
    MfString mMatch;                                                            // 0x2c the name Search found
};

#endif
