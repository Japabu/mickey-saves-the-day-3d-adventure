// own copy: VC5 compiles MfScDataHandle::GarbageCollectContainer differently with the shared
// headers; MfScObject.h as MfScDataHandle_2.cpp saw it at cab9731.
#pragma once
#include "MfScDataObject.h"
#include "MfString.h"

// Base of the script objects proper (MfScDataObject with the 0x200 flag).
class __declspec(dllexport) MfScObject : public MfScDataObject {
public:
    MfScObject(unsigned int flags = 0) : MfScDataObject(flags | kObject) {}
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
};

// Boxed integer (0x18 bytes).
class __declspec(dllexport) MfScObjInteger : public MfScObject {
public:
    MfScObjInteger(int value);                  // 0x1000c400
    virtual ~MfScObjInteger() {}
    virtual MfPropertyErrorCode GetProperty(const MfString &name, MfScDataHandle **value);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value,
                                            int passRef);
    virtual MfString GetClassNameA();
    int Value() const { return mValue; }
    // describes the class to the class-info browser (once)
    static void AddClassInfo();
    static void ClearClassInfo();
    int mValue;     // +0x14
private:
    static int bClassInfoAdded;
};

// Boxed float (0x18 bytes).
class __declspec(dllexport) MfScObjFloat : public MfScObject {
public:
    MfScObjFloat(float value);                  // 0x1000c080
    virtual ~MfScObjFloat() {}
    virtual MfPropertyErrorCode GetProperty(const MfString &name, MfScDataHandle **value);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value,
                                            int passRef);
    virtual MfString GetClassNameA();
    float Value() const { return mValue; }
    // describes the class to the class-info browser (once)
    static void AddClassInfo();
    static void ClearClassInfo();
    float mValue;   // +0x14
private:
    static int bClassInfoAdded;
};

// String object (0x1c bytes).
class __declspec(dllexport) MfScObjString : public MfScObject, public MfString {
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
