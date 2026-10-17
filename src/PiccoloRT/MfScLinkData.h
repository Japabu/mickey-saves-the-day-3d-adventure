// MfScLinkData: the result of counting the circular links of a data object.
#pragma once
#include "PiccoloRT_api.h"

// Result of counting the circular links of a data object (see
// MfScData::CountCircLinks). There is only ever one alive: operator new
// hands out a static buffer.
class PICCOLORT_API MfScLinkData {
public:
    MfScLinkData() : mCount(0) {}
    virtual ~MfScLinkData() {}
    int & Count();
    static void * __cdecl operator new(unsigned int);
    static void __cdecl operator delete(void *);
    int mCount;     // +4
};

template <class T> struct MfScListLinkItem {
    MfScListLinkItem(T value) : m_value(value) {}
    ~MfScListLinkItem() {}
    T m_value;                     // +0
    MfScListLinkItem<T> *m_next;   // +4
};

// Link data that also remembers where the links were found.
template <class T> class MfScListLinkData : public MfScLinkData {
public:
    typedef MfScListLinkItem<T> Item;
    MfScListLinkData() : m_head(0) {}
    virtual ~MfScListLinkData() {
        Item *item = m_head;
        while (item) {
            Item *next = item->m_next;
            delete item;
            item = next;
        }
    }
    void Add(T value) {
        Item *item = new Item(value);
        item->m_next = m_head;
        m_head = item;
    }
    Item *m_head; // +8
};
