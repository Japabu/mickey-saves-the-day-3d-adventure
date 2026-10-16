// MfAiCompiler: the script compiler interface ("PiccoloCompilerI"); see
// src/PiccoloComp.
#pragma once
#include "MfAi.h"
class MfPcErrorCallBack;
class MfScHandleList;
class MfString;
class MfStringList;

// compiles a script's constant definitions: (module name, source, errors)
typedef int (*MfConstInitFunction)(const MfString &, const MfString &, MfString *);

class MfAiCompiler : public MfAi {
public:
    virtual int Compile(const MfString &module, const MfString &type, const MfString &source,
                        MfPcErrorCallBack *errors, int flags) = 0;
    virtual int Flags() = 0;
    virtual int CodeType() const = 0;
    virtual void ClearCodeType() = 0;
    virtual MfScHandleList &Functions() = 0;
    virtual int IsSymbolGlobalPermanent(const MfString &name) const = 0;
    virtual void GetGlobalSymbolNames(MfStringList *names, int flags) const = 0;
    virtual void FlushGlobalTemps() = 0;
    virtual MfConstInitFunction GetConstInitFunction() = 0;
    virtual int NumberOfErrors() const = 0;
    virtual int NumberOfWarnings() const = 0;
    virtual const MfString &ModuleName() const = 0;
    virtual const MfString &ModuleType() const = 0;
    virtual void UpdatePiccoloDataPathFiles(int force) = 0;
    virtual void UpdatePiccoloLibFiles(int force) = 0;
    virtual int PiccoloLookupKeyword(const MfString &word) = 0;
    virtual MfString ParseModuleNameFromErrorInfo(const MfString &info) = 0;
    virtual int ParseLineNumberFromErrorInfo(const MfString &info) = 0;
};
