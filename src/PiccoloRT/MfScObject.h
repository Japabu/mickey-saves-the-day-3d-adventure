// MfScObject: the base of the script objects proper (with the boxed MfScObjInteger and MfScObjFloat, and MfScObjString).
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfScDataObject.h"
#include "MfString.h"

// Base of the script objects proper (MfScDataObject with the 0x200 flag).
class PICCOLORT_API MfScObject : public MfScDataObject {
public:
    MfScObject(unsigned int flags = 0) : MfScDataObject(flags | kObject) {}
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *);
    virtual void ReleaseCircLinks(MfScLinkData *);
    virtual int GetThroughCount(MfScDataHandle *);
};

// Boxed integer (0x18 bytes).
class PICCOLORT_API MfScObjInteger : public MfScObject {
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
class PICCOLORT_API MfScObjFloat : public MfScObject {
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

#include "MfScObjString.h"
