#pragma once

// Root of the scene-graph classes: reference counting and a chain of
// TypeInfo records for run-time type tests.
class StdBase {
public:
    struct TypeInfo {
        char const * typeName;
        TypeInfo const * baseTypeInfo;
    };
    int GetRefCount() { return m_RefCount; }
    virtual TypeInfo const * GetTypeInfo() const;
    virtual char const * GetTypeName() const;
    static TypeInfo const * GetStaticTypeInfo() { return &s_TypeInfo; }
    void Ref();
    void Unref();
    bool IsOfExactType(TypeInfo const *) const;
    bool IsOfType(TypeInfo const *) const;
protected:
    StdBase(const StdBase &) : m_RefCount(0) {}
    StdBase() : m_RefCount(0) {}
    virtual ~StdBase() {}
    static TypeInfo const s_TypeInfo;
private:
    int m_RefCount;
};

// object as a T when its run-time type is T or derived from T, else 0.
// The T * argument only selects the type (VC6 cannot take it explicitly).
template <class T>
inline T *StdBaseCast(StdBase *object, T *)
{
    if (object) {
        StdBase::TypeInfo const *typeInfo = object->GetTypeInfo();
        while (typeInfo != T::GetStaticTypeInfo()) {
            typeInfo = typeInfo->baseTypeInfo;
            if (typeInfo == 0)
                return 0;
        }
        return (T *)object;
    }
    return 0;
}
