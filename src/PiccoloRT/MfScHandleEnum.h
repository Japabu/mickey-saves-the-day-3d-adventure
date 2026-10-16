#pragma once
class MfScDataHandle;

// Visits the handles in use (MfScDataHandle::EnumHandles); returning 0
// stops the enumeration.
class MfScHandleEnum {
public:
    virtual int CallBack(MfScDataHandle *handle) = 0;
};
