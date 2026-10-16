// MfFSMNote, MfFSMNoteContents: a note of the behaviour editor and its text
// ('note_contents {!*!...!*!}'). RTTI names; vtables 100b5c7c and 100b5d34;
// unit 10082700-10082c00.
#ifndef RUNTIMELIB_MFFSMNOTE_H
#define RUNTIMELIB_MFFSMNOTE_H
#include "MfStateProperty.h"

class MfFSMNote : public MfStateProperty {
public:
    enum { TYPE = 0x72 };

    MfFSMNote(MfString name, MfStatePropertyList *properties);
    virtual ~MfFSMNote();
    virtual MfFSMMutable *Clone();
    virtual void FormatForExecution(MfFiniteStateMachine *machine);

    MfStateProperty *mContents;                                                 // 0x28
};

class MfFSMNoteContents : public MfStateProperty {
public:
    enum { TYPE = 0x73 };

    MfFSMNoteContents(MfString text);
    virtual ~MfFSMNoteContents();
    virtual MfFSMMutable *Clone();
    virtual void Write(ostream &os, int &indent, int mode);
    virtual void SetPath(MfString parent);

    MfString mText;                                                             // 0x28
};

#endif
