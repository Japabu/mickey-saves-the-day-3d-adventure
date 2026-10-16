// MfThread (mfutil.dll): a worker thread.
#pragma once

class __declspec(dllimport) MfThread {
public:
    MfThread(int);
    virtual ~MfThread();
    static MfThread *__cdecl GetThread();
protected:
    virtual void ThreadEntry();
private:
    int field_4;
    int field_8;
    int field_c;
    int field_10;
    unsigned int mState;                                    // 0x14
    int field_18;
    int field_1c;
};
