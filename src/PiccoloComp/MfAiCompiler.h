#pragma once
#include "MfAi.h"
class MfPcErrorCallBack;
class MfScHandleList;
class MfString;
class MfStringList;

// The script compiler interface ("PiccoloCompilerI").
class MfAiCompiler : public MfAi {
public:
    virtual int Compile(MfString const &module, MfString const &type, MfString const &source,
                        MfPcErrorCallBack *errors, int flags) = 0;
    virtual int Flags() = 0;
    virtual int CodeType() const = 0;
    virtual void ClearCodeType() = 0;
    virtual MfScHandleList &Functions() = 0;
    virtual int IsSymbolGlobalPermanent(MfString const &name) const = 0;
    virtual void GetGlobalSymbolNames(MfStringList *names, int flags) const = 0;
    virtual void FlushGlobalTemps() = 0;
    virtual int (*GetConstInitFunction())(MfString const &, MfString const &, MfString *) = 0;
    virtual int NumberOfErrors() const = 0;
    virtual int NumberOfWarnings() const = 0;
    virtual MfString const &ModuleName() const = 0;
    virtual MfString const &ModuleType() const = 0;
    virtual void UpdatePiccoloDataPathFiles(int force) = 0;
    virtual void UpdatePiccoloLibFiles(int force) = 0;
    virtual int PiccoloLookupKeyword(MfString const &word) = 0;
    virtual MfString ParseModuleNameFromErrorInfo(MfString const &info) = 0;
    virtual int ParseLineNumberFromErrorInfo(MfString const &info) = 0;
};
