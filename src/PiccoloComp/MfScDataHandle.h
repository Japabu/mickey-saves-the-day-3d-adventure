#pragma once
class MfScData;
class MfString;
class MfVariant;

// PiccoloRT.dll: a reference-counted, typed value of the script VM (8 bytes).
class __declspec(dllimport) MfScDataHandle {
public:
    MfScData *GetData() const { return (m_flags & 0xfc0) ? m_data : 0; }
    unsigned short Release() { return m_refCount > 0 ? --m_refCount : m_refCount; }
    static MfScDataHandle *CreateBytecodeFunction(MfString const &name, int numArgs);
    static MfScDataHandle *CreateData(MfVariant const &value, int flags);
private:
    unsigned short m_refCount;  // +0
    unsigned short m_flags;     // +2
    MfScData *m_data;           // +4
};
