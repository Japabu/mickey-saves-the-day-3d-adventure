#pragma once

// Reference-counted base of the scene-graph library's value classes
// (Text, TextPrimitive, Vec<T>).
class BasicObject {
    int refCount__;
protected:
    BasicObject(const BasicObject &) : refCount__(0) {}
    BasicObject() : refCount__(0) {}
    virtual ~BasicObject() {}
public:
    void ref() const { ((BasicObject *)this)->refCount__++; }
    void unref() const
    {
        if (refCount__)
            ((BasicObject *)this)->refCount__--;
        if (refCount__ == 0)
            delete (BasicObject *)this;
    }
    void unrefNoDelete() const;
    int numRefs() const { return refCount__; }
};
