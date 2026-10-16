#pragma once
// MfScDataHandle.h: a reference-counted, typed value of Motivate's Piccolo script VM
// (PiccoloRT.dll), as the game declares it (SteamboatApp.pdb). Only the members the game calls
// are declared; the accessors are inline. Unlike the runtime's own header, SetType here stores
// the whole flag word (SteamboatNode::CleanupExitSceneEvent writes the constant 0x12).
class MfScData;
class MfScDataObject;
class MfString;

class MfScDataHandle {
public:
    MfScDataHandle();
    ~MfScDataHandle();
    unsigned short AddRef() { return ++udwRefCount; }
    unsigned short Release() { return udwRefCount > 0 ? --udwRefCount : udwRefCount; }
    unsigned short GetRefCount() const { return udwRefCount; }
    unsigned int GetType() const { return udwFlags & 0x1ff0; }
    void SetType(unsigned long type) { udwFlags = (unsigned short)type; }
    int GetInt() const { return lValue; }
    void SetInt(int value) { lValue = value; }
    float GetFloat() const { return fValue; }
    void SetFloat(float value) { fValue = value; }
    MfScData *GetData() const { return pdData; }
    void SetData(MfScData *data) { pdData = data; }
    MfScDataHandle *GetPointer() const { return phPointer; }
    void SetPointer(MfScDataHandle *pointer) { phPointer = pointer; }
    static MfScDataHandle *__cdecl CreateData(MfScDataObject *);
    static MfScDataHandle *__cdecl CreateData(const MfString &, int);
    static MfScDataHandle *__cdecl CreateData(float);
    static MfScDataHandle *__cdecl CreateData(int);
private:
    unsigned short udwRefCount;                         // +0x0000
    unsigned short udwFlags;                            // +0x0002
    union {                                             // +0x0004
        MfScData *pdData;
        MfScDataHandle *phPointer;
        long lValue;
        float fValue;
    };
};
