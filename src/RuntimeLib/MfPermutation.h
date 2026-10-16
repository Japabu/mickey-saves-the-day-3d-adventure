#pragma once
class MfProtoTraversal;

class __declspec(dllimport) MfPermutation {
public:
    MfPermutation & operator=(MfPermutation const &);
    MfPermutation(MfProtoTraversal *, MfProtoTraversal *);
    ~MfPermutation();
    int field_0;
    int field_4;
    int *mMap;                                              // 0x08 new index of each old node (-1: removed)
    int field_c;
};
