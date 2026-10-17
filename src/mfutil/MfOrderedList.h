// MfOrderedList: sorted collection of void* on a 2-3 tree. Exported names
// only; members named from their use.
#ifndef MFUTIL_MFORDEREDLIST_H
#define MFUTIL_MFORDEREDLIST_H

#include "mfutil_api.h"

class Mf23TreeBase;
class Mf23TreeLeaf;
class Mf23TreeNode;
class MfDblList;

class MFUTIL_API MfOrderedList {
public:
    typedef int (*CompareFn)(void *, void *);
    typedef void (*DataFn)(void *);

    MfOrderedList(CompareFn less, CompareFn equal);
    MfOrderedList(const MfOrderedList &) {}
    ~MfOrderedList();
    MfDblList *OrderedTraversal();
    int Count() { return m_count; }
    int TestTreeRoot();
    void *Find(void *);
    void *Max();
    void *Min();
    void *Pop();
    void Add(void *);
    void Clear();
    void Delete(void *);
    void PrintList(DataFn);
    void Remove(void *);
    void SetDeleteCB(DataFn cb) { m_deleteCB = cb; }
private:
    Mf23TreeBase *_Search(void *, int);
    Mf23TreeLeaf *MinLeaf();
    Mf23TreeLeaf *_Search(void *);
    int _TestTree(Mf23TreeBase *);
    void *_Max(Mf23TreeBase *);
    void _InsertLeaf(Mf23TreeLeaf *, Mf23TreeNode *, Mf23TreeBase *&);
    void _InsertNode(Mf23TreeNode *, Mf23TreeNode *, Mf23TreeBase *&);
    void _MoveNodeLeft(Mf23TreeBase *);
    void _MoveNodeRight(Mf23TreeBase *);
    void _Order(Mf23TreeBase *);
    void _PrintChild(Mf23TreeBase *);
    void _PrintChildren(Mf23TreeBase *, int);
    void _Remove(Mf23TreeLeaf *);
    void _UpdateDel(void *, void *, Mf23TreeBase *);
    void _UpdateIns(void *, Mf23TreeBase *);
    void _UpdateParentIndices(Mf23TreeNode *, int);
    void _UpdateSimple(Mf23TreeNode *);

    MfDblList *m_ordered;
    CompareFn m_less;
    CompareFn m_equal;
    DataFn m_deleteCB;
    DataFn m_printCB;
    Mf23TreeNode *m_root;
    int m_count;
    int m_newMax;       // set when an insert passed the largest key of a node
};

#endif
