// MfScObject (PiccoloRT.dll): a Piccolo object (flag 0x200), the base of the
// script-visible wrappers. The copy constructor, assignment and destructor
// are implicit (inline): MfPoLibrary copies the members itself.
#pragma once
#include "MfScDataObject.h"

class __declspec(dllimport) MfScObject : public MfScDataObject {
public:
    MfScObject(unsigned int flags) : MfScDataObject(flags | 0x200) {}
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
};
