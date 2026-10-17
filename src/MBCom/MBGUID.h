// MBGUID: the muffin component model's 128-bit interface and class id
// (muffin/include/Com), compared field by field and ordered for the factory map.
#ifndef MBGUID_H
#define MBGUID_H

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &guid) const
    {
        return data1 == guid.data1 && data2 == guid.data2 && data3 == guid.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)guid.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)guid.data4)[1];
    }
    bool operator!=(const MBGUID &guid) const { return !(*this == guid); }
};

// 0x100022d0 ??M@YA_NABUMBGUID@@0@Z (MBComFactory.obj): the key order of
// MBComFactory's factory map.
inline bool operator<(const MBGUID &guid1, const MBGUID &guid2)
{
    if (guid1.data1 > guid2.data1)
        return false;
    if (guid1.data1 < guid2.data1)
        return true;
    if (guid1.data2 > guid2.data2)
        return false;
    if (guid1.data2 < guid2.data2)
        return true;
    if (guid1.data3 > guid2.data3)
        return false;
    if (guid1.data3 < guid2.data3)
        return true;
    if (*(unsigned long *)&guid1.data4[0] > *(unsigned long *)&guid2.data4[0])
        return false;
    if (*(unsigned long *)&guid1.data4[0] < *(unsigned long *)&guid2.data4[0])
        return true;
    return *(unsigned long *)&guid1.data4[4] < *(unsigned long *)&guid2.data4[4];
}

#endif
