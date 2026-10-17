#pragma once
#include "MfString.h"

// mfutil.dll: looks for a file along a list of directories.
class __declspec(dllimport) MfPathSearch {   // own copy: src/mfutil/MfPathSearch.h includes src/mfutil/MfString.h, which conflicts with the older MfString kept here
public:
    MfPathSearch(MfString const &file, MfString const &path);
    virtual ~MfPathSearch();
    MfString Search();
private:
    void *mPaths;               // +0x04
    int mCount;                 // +0x08
};
