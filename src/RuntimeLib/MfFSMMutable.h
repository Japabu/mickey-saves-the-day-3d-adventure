// MfFSMMutable: the base of the parts of a behaviour (a hierarchical finite
// state machine as the behaviour file describes it: states, transitions,
// notes and their properties). RTTI name; vtable 100b5c1c, 22 slots; unit
// 10082260-10082700. RuntimeLib internal: the names of the methods are
// descriptive, slots whose role is not known yet are named after their slot.
// A part has a name and a path (its parents' names, separated by '\').
#ifndef RUNTIMELIB_MFFSMMUTABLE_H
#define RUNTIMELIB_MFFSMMUTABLE_H
#include "MfDblList.h"
#include "MfString.h"
class MfFiniteStateMachine;
class MfFSMList;
class MfPcErrorCallBack;
class MfScPiccoloObjectFile;
class MfStateProperty;
class MfStringList;
class ostream;

// mfutil.dll: writes a string to a stream
__declspec(dllimport) ostream &operator<<(ostream &os, const MfString &s);

class MfFSMMutable {
public:
    MfFSMMutable();
    virtual ~MfFSMMutable();                                                    // 0x00
    // a copy of the part (and its properties)
    virtual MfFSMMutable *Clone() { return 0; }                                 // 0x04
    virtual void Slot2() {}                                                     // 0x08
    virtual void Slot3() {}                                                     // 0x0c
    // takes the compiled script functions from a .pobj file
    virtual void Load(MfScPiccoloObjectFile *objectFile) {}                     // 0x10
    // looks up the compiled script functions of the part
    virtual void Link() {}                                                      // 0x14
    // writes the part in the behaviour file format, indented by indent tabs
    virtual void Write(ostream &os, int &indent, int mode) {}                   // 0x18
    // compiles the scripts (into objectFile if given); counts the failures
    virtual void Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors,
                         int &failures) {}                                      // 0x1c
    // lets go of the compiled script functions
    virtual void Unload() {}                                                    // 0x20
    // the number of children named name, apart from except
    virtual int CountNamed(MfString name, MfFSMMutable *except) { return 0; }   // 0x24
    // can the user delete the part? (not the machine's root state)
    virtual int CanDelete() { return 0; }                                       // 0x28
    // may a transition lead from the part to target?
    virtual int CanConnectTo(MfStateProperty *target) { return 0; }             // 0x2c
    virtual MfString &GetPath() { return mPath; }                               // 0x30
    // the path is the parent's path and the name
    virtual void SetPath(MfString parent);                                      // 0x34
    virtual void SetName(const char *name);                                     // 0x38
    virtual MfString &GetName() { return mName; }                               // 0x3c
    // the part at path: names are the path's parts, pos the one this part
    // stands at, depth its index
    virtual MfFSMMutable *Find(MfString path, MfStringList *names, MfDblListItem *pos,
                               int depth);                                      // 0x40
    virtual void Slot17(int) {}                                                 // 0x44
    // adds the part and its properties to list
    virtual void CollectAll(MfDblList *list) {}                                 // 0x48
    // the Piccolo source of a script part
    virtual MfString *GetScript() { return 0; }                                 // 0x4c
    virtual void SetScript(const MfString &source) {}                           // 0x50
    // looks for text in the part
    virtual void Search(MfString text, int, int caseSensitive, int, int what, int, int, int,
                        MfFSMMutable *&found, MfFSMMutable *&foundIn);          // 0x54

    void SetFullPath(MfString path);
    // the names of a path's parts, without the first
    static void SplitPath(MfStringList *names, MfString path);

    int m04;                                                                    // 0x04
    MfString mPath;                                                             // 0x08
    MfString mName;                                                             // 0x0c
    MfFiniteStateMachine *mMachine;                                             // 0x10
};

#endif
