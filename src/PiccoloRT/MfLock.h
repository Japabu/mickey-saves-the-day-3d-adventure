#pragma once

// RuntimeLib: mutex wrapper.
class __declspec(dllimport) MfLock {
public:
    MfLock();
    ~MfLock();
    int Lock(long timeout = -1);
    void Unlock();
private:
    void *m_handle;
};
