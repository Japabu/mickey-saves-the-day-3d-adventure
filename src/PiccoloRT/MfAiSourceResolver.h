#pragma once
#include "MfAiFunctionRegistry.h"
class MfString;
class MfStringList;

// A kind of script source (by module type): decides whether two module names
// denote the same module. Only the slot the registry uses is named.
class MfAiSourceType : public MfAi {
public:
    // the module type the source type serves
    virtual int GetType(MfString &type) = 0;
    // reads the source of module into text; module becomes the file it was
    // read from
    virtual int ReadSource(MfString &module, MfString &text, int flags) = 0;
    // writes text as the source in file
    virtual int WriteSource(const MfString &file, MfString &text, int flags) = 0;
    // splits a search path into its directories
    virtual int SplitPath(const MfString &path, MfStringList *dirs) = 0;
    virtual int IsSameModule(const MfString &module, const MfString &name) = 0;
};

// "SourceDebugger": the source types registered by module type.
class MfAiSourceResolver : public MfAi {
public:
    virtual int Register(const MfString &type, MfAiSourceType *sourceType) = 0;
    virtual int Unregister(const MfString &type) = 0;
    virtual int Find(const MfString &type, MfAiSourceType **sourceType) = 0;
};
