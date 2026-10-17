// MfThreadWork: a work item an MfThread runs (MfThread::AssignWork); slot 1 runs it
// (mfutil.dll; not exported, all inline).
#ifndef MFUTIL_MFTHREADWORK_H
#define MFUTIL_MFTHREADWORK_H

class MfThreadWork {
public:
    virtual ~MfThreadWork() {}
    virtual void DoWork() = 0;
};

#endif
