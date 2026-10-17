// MfScObjString: the script string object.
#pragma once
#include "PiccoloRT_api.h"
#include "MfScObject.h"
#include "MfString.h"

// String object (0x1c bytes).
class PICCOLORT_API MfScObjString : public MfScObject, public MfString {
public:
    MfScObjString(int readOnly = 0);            // 0x1000c740
    virtual ~MfScObjString();                   // 0x1000c7b0
    int IsReadOnly() const { return m_readOnly; }
    static void __cdecl Initialize();
    static void __cdecl ClearProperties();
    virtual MfPropertyErrorCode GetProperty(const MfString &name, MfScDataHandle **value);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value,
                                            int passRef);
    virtual MfScPropTokenTable *GetPropTokenTable() { return pttPropTokens; }
protected:
    virtual MfString GetClassNameA() { return MfString("String"); }
    virtual void FilloutPropertyNameList(MfStringList *names);
private:
    MfScDataHandle *PMPrint(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMPrintNl(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMLeftString(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMMidString(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMRightString(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMToInt(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMToFloat(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMAscii(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMCaseSubString(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMCharsInSet(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMSubString(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMStripLeadingWhite(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMStripTrailingWhite(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMStripWhite(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMToLower(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    MfScDataHandle *PMToUpper(MfScDataHandle *self, int argc, MfScDataHandle **argv);
    static MfScPropTokenTable *pttPropTokens;
    static MfScHandleDict *pdictStringProps;
    int m_readOnly;     // +0x18
};
