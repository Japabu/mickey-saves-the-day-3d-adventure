// MfOrderedList (mfutil.dll): a sorted collection of void * on a 2-3 tree.
// Only what RuntimeLib uses; as in src/mfutil/MfOrderedList.h.
#ifndef RUNTIMELIB_MFORDEREDLIST_H
#define RUNTIMELIB_MFORDEREDLIST_H
class MfDblList;

class __declspec(dllimport) MfOrderedList {
public:
    typedef int (*CompareFn)(void *, void *);
    typedef void (*DataFn)(void *);

    MfOrderedList(CompareFn less, CompareFn equal);
    ~MfOrderedList();
    void *Pop();
    void Add(void *);
    void Remove(void *);

private:
    MfDblList *m_ordered;
    CompareFn m_less;
    CompareFn m_equal;
    DataFn m_deleteCB;
    DataFn m_printCB;
    void *m_root;
    int m_count;
    int m_newMax;
};

#endif
