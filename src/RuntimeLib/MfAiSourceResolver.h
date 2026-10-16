// MfAiSourceType, MfAiSourceResolver (PiccoloRT.dll): the kinds of script
// source by module type and their registry ("SourceDebugger"). As in
// src/PiccoloRT/MfAiSourceResolver.h.
#ifndef RUNTIMELIB_MFAISOURCERESOLVER_H
#define RUNTIMELIB_MFAISOURCERESOLVER_H
#include "MfAi.h"
class MfString;
class MfStringList;

class MfAiSourceType : public MfAi {
public:
    // the module type the source type serves
    virtual int GetType(MfString &type) = 0;
    // reads the source of module into text
    virtual int ReadSource(MfString &module, MfString &text, int flags) = 0;
    // writes text as the source in file
    virtual int WriteSource(const MfString &file, MfString &text, int flags) = 0;
    // splits a search path into its directories
    virtual int SplitPath(const MfString &path, MfStringList *dirs) = 0;
    virtual int IsSameModule(const MfString &module, const MfString &name) = 0;
};

class MfAiSourceResolver : public MfAi {
public:
    virtual int Register(const MfString &type, MfAiSourceType *sourceType) = 0;
    virtual int Unregister(const MfString &type) = 0;
    virtual int Find(const MfString &type, MfAiSourceType **sourceType) = 0;
};

#endif
