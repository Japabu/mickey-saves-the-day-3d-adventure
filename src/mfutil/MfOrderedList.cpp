// MfOrderedList (mfutil.dll 0x10009c20-0x1000acf0): sorted list on a 2-3 tree.
#include <stdio.h>
#include "MfDblList.h"
#include "Mf23Tree.h"
#include "MfOrderedList.h"

// Inline members of the headers:
// 0x10009c20 ??0MfOrderedList@@QAE@ABV0@@Z
// 0x10009c30 ?SetDeleteCB@MfOrderedList@@QAEXP6AXPAX@Z@Z
// 0x10009c50 ?Count@MfOrderedList@@QAEHXZ

// 0x10009c40 ?Max@MfOrderedList@@QAEPAXXZ
void *MfOrderedList::Max()
{
    return _Max(m_root);
}

// 0x10009c60 ?OrderedTraversal@MfOrderedList@@QAEPAVMfDblList@@XZ
// A new list with all data in order; the caller deletes it.
MfDblList *MfOrderedList::OrderedTraversal()
{
    m_ordered = new MfDblList;
    _Order(m_root);
    return m_ordered;
}

// 0x10009cb0 ?TestTreeRoot@MfOrderedList@@QAEHXZ
int MfOrderedList::TestTreeRoot()
{
    return _TestTree(m_root);
}

// 0x10009cc0 ??4MfOrderedList@@QAEAAV0@ABV0@@Z (compiler-generated)
// 0x10009ce0 ??1Mf23TreeNode@@UAE@XZ
// 0x10009d50 ??1Mf23TreeBase@@UAE@XZ
// 0x10009d70 ??_GMf23TreeBase@@UAEPAXI@Z
// 0x10009d90 ?Type@Mf23TreeNode@@UAEHXZ
// 0x10009da0 ??_GMf23TreeNode@@UAEPAXI@Z

// 0x10009dc0 ??0MfOrderedList@@QAE@P6AHPAX0@Z1@Z
MfOrderedList::MfOrderedList(CompareFn less, CompareFn equal)
{
    m_less = less;
    m_equal = equal;
    m_printCB = 0;
    m_ordered = 0;
    m_root = new Mf23TreeNode;
    m_count = 0;
}

// 0x10009e30 ??1MfOrderedList@@QAE@XZ
MfOrderedList::~MfOrderedList()
{
    delete m_root;
}

// Prints the subtree that broke the order and fails the test.
#define TREE_ERROR(t) { _PrintChildren(t, 0); printf("-------\n\n"); return 0; }

// 0x10009e40 ?_TestTree@MfOrderedList@@AAEHPAVMf23TreeBase@@@Z
// Debugging aid: checks the key order along the leftmost path.
int MfOrderedList::_TestTree(Mf23TreeBase *t)
{
    while (t && t->Type() != Mf23TreeBase::LEAF) {
        Mf23TreeNode *n = (Mf23TreeNode *)t;
        if (n->child[0] && n->child[0]->Type() == Mf23TreeBase::NODE) {
            void *a = ((Mf23TreeNode *)n->child[0])->key[1];
            void *b = ((Mf23TreeNode *)n->child[1])->key[0];
            if (a && b && !m_less(a, b))
                TREE_ERROR(t);
        }
        if (n->child[2] && n->child[2]->Type() == Mf23TreeBase::NODE) {
            void *a = ((Mf23TreeNode *)n->child[1])->key[1];
            void *b = ((Mf23TreeNode *)n->child[2])->key[0];
            if (a && b && !m_less(a, b))
                TREE_ERROR(t);
        }
        if (n->child[0] && n->child[0]->Type() == Mf23TreeBase::LEAF) {
            void *a = MF23_KEY(n->child[0]);
            void *b = n->child[1] ? MF23_KEY(n->child[1]) : 0;
            void *c = n->child[2] ? MF23_KEY(n->child[2]) : 0;
            if (a && b && !m_less(a, b))
                TREE_ERROR(t);
            if (c) {
                if (!m_less(b, c))
                    TREE_ERROR(t);
                if (!m_less(a, c))
                    TREE_ERROR(t);
            }
        }
        t = n->child[0];
    }
    return 1;
}

// 0x10009fa0 ?_Order@MfOrderedList@@AAEXPAVMf23TreeBase@@@Z
void MfOrderedList::_Order(Mf23TreeBase *t)
{
    if (t->Type() == Mf23TreeBase::LEAF) {
        m_ordered->Add(MF23_KEY(t));
        return;
    }
    Mf23TreeNode *n = (Mf23TreeNode *)t;
    if (n->child[0])
        _Order(n->child[0]);
    if (n->child[1])
        _Order(n->child[1]);
    if (n->child[2])
        _Order(n->child[2]);
}

// 0x1000a000 ?Clear@MfOrderedList@@QAEXXZ
// Hands every datum to the delete callback.
void MfOrderedList::Clear()
{
    MfDblList *list = OrderedTraversal();
    MfDblListItem *pli;
    for (void *p = list->GetFirst(pli); p; p = list->GetNext(pli))
        m_deleteCB(p);
    delete list;
}

// 0x1000a050 ?_PrintChildren@MfOrderedList@@AAEXPAVMf23TreeBase@@H@Z
void MfOrderedList::_PrintChildren(Mf23TreeBase *t, int depth)
{
    if (!t || t->Type() == Mf23TreeBase::LEAF)
        return;
    Mf23TreeNode *n = (Mf23TreeNode *)t;
    int i;
    for (i = 0; i < depth; i++)
        printf("  ");
    printf("%d-L<< ", depth);
    _PrintChild(n->child[0]);
    printf(" >>\n");
    _PrintChildren(n->child[0], depth + 1);
    for (i = 0; i < depth; i++)
        printf("  ");
    printf("%d-M<< ", depth);
    _PrintChild(n->child[1]);
    printf(" >>\n");
    _PrintChildren(n->child[1], depth + 1);
    for (i = 0; i < depth; i++)
        printf("  ");
    printf("%d-R<< ", depth);
    _PrintChild(n->child[2]);
    printf(" >>\n");
    _PrintChildren(n->child[2], depth + 1);
}

// 0x1000a180 ?_PrintChild@MfOrderedList@@AAEXPAVMf23TreeBase@@@Z
void MfOrderedList::_PrintChild(Mf23TreeBase *t)
{
    if (!t) {
        printf("( )");
        return;
    }
    if (t->Type() == Mf23TreeBase::LEAF) {
        printf(" #%d  (", t->index);
        m_printCB(MF23_KEY(t));
        printf(")");
        return;
    }
    printf(" #%d  [", t->index);
    m_printCB(((Mf23TreeNode *)t)->key[0]);
    printf(":");
    m_printCB(((Mf23TreeNode *)t)->key[1]);
    printf("]");
}

// 0x1000a220 ?_Search@MfOrderedList@@AAEPAVMf23TreeBase@@PAXH@Z
// The leaf where key belongs (bLeaf) or its parent node.
Mf23TreeBase *MfOrderedList::_Search(void *key, int bLeaf)
{
    Mf23TreeBase *t = m_root;
    Mf23TreeBase *parent = t;
    while (t) {
        parent = t;
        Mf23TreeNode *n = (Mf23TreeNode *)t;
        if (m_less(key, n->key[0]))
            t = n->child[0];
        else if (n->child[2] && !m_less(key, n->key[1]))
            t = n->child[2];
        else
            t = n->child[1];
        if (t && t->Type() == Mf23TreeBase::LEAF) {
            if (!bLeaf)
                return parent;
            return t;
        }
    }
    if (!bLeaf)
        return parent;
    return t;
}

// 0x1000a2b0 ?_Search@MfOrderedList@@AAEPAVMf23TreeLeaf@@PAX@Z
Mf23TreeLeaf *MfOrderedList::_Search(void *key)
{
    Mf23TreeLeaf *leaf = (Mf23TreeLeaf *)_Search(key, 1);
    if (leaf && m_equal(key, leaf->data))
        return leaf;
    return 0;
}

// 0x1000a2f0 ?_UpdateIns@MfOrderedList@@AAEXPAXPAVMf23TreeBase@@@Z
// Raises the keys above t that are smaller than key.
void MfOrderedList::_UpdateIns(void *key, Mf23TreeBase *t)
{
    for (; t && t->parent; t = t->parent) {
        if (t->index != 2 && m_less(t->parent->key[t->index], key))
            t->parent->key[t->index] = key;
    }
}

// 0x1000a340 ?_UpdateDel@MfOrderedList@@AAEXPAX0PAVMf23TreeBase@@@Z
// Replaces oldKey by newKey in the keys above t.
void MfOrderedList::_UpdateDel(void *oldKey, void *newKey, Mf23TreeBase *t)
{
    for (; t && t->parent; t = t->parent) {
        if (t->index != 2 && m_equal(t->parent->key[t->index], oldKey))
            t->parent->key[t->index] = newKey;
    }
}

// 0x1000a390 ?_UpdateSimple@MfOrderedList@@AAEXPAVMf23TreeNode@@@Z
void MfOrderedList::_UpdateSimple(Mf23TreeNode *n)
{
    if (n->child[0])
        n->key[0] = MF23_KEY(n->child[0]);
    else
        n->key[0] = 0;
    if (n->child[1])
        n->key[1] = MF23_KEY(n->child[1]);
    else
        n->key[1] = 0;
}

// 0x1000a3c0 ?_UpdateParentIndices@MfOrderedList@@AAEXPAVMf23TreeNode@@H@Z
void MfOrderedList::_UpdateParentIndices(Mf23TreeNode *n, int which)
{
    if (n->child[0]->Type() == Mf23TreeBase::LEAF) {
        if (n->child[0]) {
            n->key[0] = MF23_KEY(n->child[0]);
            n->key[1] = MF23_KEY(n->child[1]);
        }
        return;
    }
    Mf23TreeNode *c;
    if (which == 1) {
        c = (Mf23TreeNode *)n->child[1];
        if (c->child[2] == 0)
            n->key[1] = c->key[1];
        else
            n->key[1] = _Max(c->child[2]);
    } else if (which == 0) {
        c = (Mf23TreeNode *)n->child[0];
        if (c->child[2] == 0)
            n->key[0] = c->key[1];
        else
            n->key[0] = _Max(c->child[2]);
        c = (Mf23TreeNode *)n->child[1];
        if (c->child[2] == 0)
            n->key[1] = c->key[1];
        else
            n->key[1] = _Max(c->child[2]);
    }
}

// 0x1000a460 ?_MoveNodeRight@MfOrderedList@@AAEXPAVMf23TreeBase@@@Z
// Shifts t and the siblings after it one place right.
void MfOrderedList::_MoveNodeRight(Mf23TreeBase *t)
{
    Mf23TreeNode *p = t->parent;
    int i = t->index;
    if (i == 0)
        p->key[1] = p->key[0];
    for (int j = 2; j > i; j--) {
        p->child[j] = p->child[j - 1];
        if (p->child[j])
            p->child[j]->index = j;
    }
}

// 0x1000a4a0 ?_InsertLeaf@MfOrderedList@@AAEXPAVMf23TreeLeaf@@PAVMf23TreeNode@@AAPAVMf23TreeBase@@@Z
// Puts leaf under node; a fourth child comes back in split.
void MfOrderedList::_InsertLeaf(Mf23TreeLeaf *leaf, Mf23TreeNode *node, Mf23TreeBase *&split)
{
    void *key = leaf->data;
    split = node->child[2];
    leaf->parent = node;
    if (!node->child[0] || m_less(key, MF23_KEY(node->child[0]))) {
        node->child[2] = node->child[1];
        if (node->child[2])
            node->child[2]->index = 2;
        node->child[1] = node->child[0];
        if (node->child[1])
            node->child[1]->index = 1;
        node->child[0] = leaf;
        leaf->index = 0;
        node->key[1] = node->key[0];
        node->key[0] = key;
    } else if (!node->child[1] || m_less(key, MF23_KEY(node->child[1]))) {
        node->child[2] = node->child[1];
        if (node->child[2])
            node->child[2]->index = 2;
        node->child[1] = leaf;
        leaf->index = 1;
        node->key[1] = key;
    } else if (!node->child[2] || m_less(key, MF23_KEY(node->child[2]))) {
        node->child[2] = leaf;
        leaf->index = 2;
        _UpdateIns(leaf->data, leaf);
    } else {
        split = leaf;
        m_newMax = 1;
        leaf->parent = 0;
    }
}

// 0x1000a5a0 ?_InsertNode@MfOrderedList@@AAEXPAVMf23TreeNode@@0AAPAVMf23TreeBase@@@Z
// Puts newNode after node in node's parent; a fourth child comes back in split.
void MfOrderedList::_InsertNode(Mf23TreeNode *newNode, Mf23TreeNode *node, Mf23TreeBase *&split)
{
    Mf23TreeNode *p = node->parent;
    split = 0;
    if (p == 0) {
        Mf23TreeNode *root = new Mf23TreeNode;
        root->child[0] = node;
        node->parent = root;
        node->index = 0;
        root->key[0] = node->key[1];
        root->child[1] = newNode;
        newNode->parent = root;
        newNode->index = 1;
        root->key[1] = newNode->key[1];
        root->parent = 0;
        m_root = root;
        return;
    }
    if (node->index == 2) {
        split = newNode;
        newNode->parent = 0;
        return;
    }
    split = p->child[2];
    if (split)
        split->parent = 0;
    if (node->index == 0) {
        p->child[2] = p->child[1];
        p->child[2]->index = 2;
        p->child[1] = newNode;
        newNode->index = 1;
        p->key[1] = newNode->key[1];
        newNode->parent = p;
        return;
    }
    p->child[2] = newNode;
    newNode->index = 2;
    newNode->parent = p;
}

// 0x1000a680 ?_MoveNodeLeft@MfOrderedList@@AAEXPAVMf23TreeBase@@@Z
// Takes t out of its parent, shifting the later siblings left.
void MfOrderedList::_MoveNodeLeft(Mf23TreeBase *t)
{
    if (!t)
        return;
    int i = t->index;
    Mf23TreeNode *p = t->parent;
    if (i == 2) {
        p->child[2] = 0;
        return;
    }
    for (int j = i; j < 2; j++) {
        p->child[j] = p->child[j + 1];
        if (p->child[j])
            p->child[j]->index = j;
    }
    p->child[2] = 0;
    if (i == 0)
        p->key[0] = p->key[1];
    p->key[1] = _Max(p->child[1]);
}

// 0x1000a6f0 ?MinLeaf@MfOrderedList@@AAEPAVMf23TreeLeaf@@XZ
Mf23TreeLeaf *MfOrderedList::MinLeaf()
{
    Mf23TreeBase *t = m_root;
    while (t && t->Type() != Mf23TreeBase::LEAF)
        t = ((Mf23TreeNode *)t)->child[0];
    return (Mf23TreeLeaf *)t;
}

// 0x1000a710 ?Min@MfOrderedList@@QAEPAXXZ
void *MfOrderedList::Min()
{
    Mf23TreeLeaf *leaf = MinLeaf();
    if (leaf)
        return leaf->data;
    return 0;
}

// 0x1000a720 ?Pop@MfOrderedList@@QAEPAXXZ
// Removes and returns the smallest datum.
void *MfOrderedList::Pop()
{
    Mf23TreeLeaf *leaf = MinLeaf();
    if (leaf) {
        void *data = leaf->data;
        _Remove(leaf);
        return data;
    }
    return 0;
}

// 0x1000a750 ?_Max@MfOrderedList@@AAEPAXPAVMf23TreeBase@@@Z
void *MfOrderedList::_Max(Mf23TreeBase *t)
{
    if (!t)
        return 0;
    if (t->Type() == Mf23TreeBase::LEAF)
        return MF23_KEY(t);
    Mf23TreeNode *n = (Mf23TreeNode *)t;
    if (!n->child[0])
        return 0;
    if (!n->parent && n->child[0]->Type() == Mf23TreeBase::LEAF) {
        if (n->child[2])
            return MF23_KEY(n->child[2]);
        if (n->child[1])
            return MF23_KEY(n->child[1]);
        return MF23_KEY(n->child[0]);
    }
    while (t && t->Type() != Mf23TreeBase::LEAF) {
        n = (Mf23TreeNode *)t;
        if (n->child[2])
            t = n->child[2];
        else
            t = n->child[1];
    }
    if (t)
        return MF23_KEY(t);
    return 0;
}

// 0x1000a7e0 ?Add@MfOrderedList@@QAEXPAX@Z
void MfOrderedList::Add(void *data)
{
    m_count++;
    Mf23TreeLeaf *leaf = new Mf23TreeLeaf;
    leaf->data = data;
    Mf23TreeNode *node = (Mf23TreeNode *)_Search(data, 0);
    Mf23TreeBase *split;
    m_newMax = 0;
    _InsertLeaf(leaf, node, split);
    if (!split)
        return;
    do {
        Mf23TreeNode *nn = new Mf23TreeNode;
        nn->child[0] = node->child[2];
        node->child[2]->parent = nn;
        node->child[2]->index = 0;
        nn->child[1] = split;
        split->parent = nn;
        split->index = 1;
        _UpdateParentIndices(nn, 0);
        node->child[2] = 0;
        _InsertNode(nn, node, split);
        if (node->index != 2)
            _UpdateParentIndices(node->parent, node->index);
        node = node->parent;
    } while (split);
    if (m_newMax)
        _UpdateIns(leaf->data, leaf);
}

// 0x1000a910 ?Type@Mf23TreeLeaf@@UAEHXZ
// 0x1000a920 ??_GMf23TreeLeaf@@UAEPAXI@Z

// 0x1000a940 ?Remove@MfOrderedList@@QAEXPAX@Z
void MfOrderedList::Remove(void *data)
{
    _Remove(_Search(data));
}

// 0x1000a960 ?Delete@MfOrderedList@@QAEXPAX@Z
// Removes data and hands it to the delete callback.
void MfOrderedList::Delete(void *data)
{
    Mf23TreeLeaf *leaf = _Search(data);
    if (leaf) {
        void *p = leaf->data;
        _Remove(leaf);
        m_deleteCB(p);
    }
}

// 0x1000a990 ?Find@MfOrderedList@@QAEPAXPAX@Z
void *MfOrderedList::Find(void *data)
{
    Mf23TreeLeaf *leaf = _Search(data);
    if (leaf)
        return leaf->data;
    return 0;
}

// TODO 0x1000a9b0 ?_Remove@MfOrderedList@@AAEXPAVMf23TreeLeaf@@@Z: (98% similar) control-flow layout: the original re-tests p->parent at the loop top after the entry test (ours threads past it) and on i == 2 jumps straight to the left-sibling case without the sib = 0 store; same paths for every input. Tried for(;;) { if (!p->parent) break; ... }, if (i == 2 || !(sib = ...)), the swapped if/else, else sib = 0 and ?: (no better)
// Unlinks leaf; a node left with one child borrows from or merges with a sibling.
void MfOrderedList::_Remove(Mf23TreeLeaf *leaf)
{
    Mf23TreeBase *old = leaf;
    if (!leaf)
        return;
    m_count--;
    Mf23TreeNode *p = leaf->parent;
    if (leaf->index == 2)
        _UpdateDel(MF23_KEY(p->child[2]), MF23_KEY(p->child[1]), p);
    else if (leaf->index == 1 && p->child[2] == 0)
        _UpdateDel(MF23_KEY(p->child[1]), MF23_KEY(p->child[0]), p);
    _MoveNodeLeft(leaf);
    _UpdateSimple(p);
    if (p->child[1] || !p->parent) {
        delete leaf;
        return;
    }
    while (p->parent) {
        Mf23TreeNode *sib = 0;
        int merged;
        int i = p->index;
        if (i != 2)
            sib = (Mf23TreeNode *)p->parent->child[i + 1];
        if (sib == 0) {
            sib = (Mf23TreeNode *)p->parent->child[i - 1];
            if (sib->child[2]) {
                // borrow the last child of the left sibling
                void *key;
                if (sib->child[2]->Type() == Mf23TreeBase::LEAF) {
                    key = MF23_KEY(sib->child[2]);
                    _UpdateDel(key, MF23_KEY(sib->child[1]), sib);
                } else {
                    key = _Max(sib->child[2]);
                    _UpdateDel(key, sib->key[1], sib);
                }
                if (m_less(key, p->key[0])) {
                    _MoveNodeRight(p->child[0]);
                    p->child[0] = sib->child[2];
                    p->child[0]->parent = p;
                    p->child[0]->index = 0;
                    p->key[0] = key;
                } else {
                    p->child[1] = sib->child[2];
                    p->child[1]->parent = p;
                    p->child[1]->index = 1;
                    p->key[1] = key;
                    _UpdateIns(key, p);
                }
                sib->child[2] = 0;
                merged = 0;
            } else {
                // merge into the left sibling
                sib->child[2] = p->child[0];
                sib->child[2]->index = 2;
                sib->child[2]->parent = sib;
                if (sib->child[2]->Type() == Mf23TreeBase::LEAF)
                    _UpdateIns(MF23_KEY(sib->child[2]), sib);
                else
                    _UpdateIns(p->key[0], sib);
                p->child[0] = 0;
                merged = 1;
            }
        } else {
            if (sib->child[2]) {
                // borrow the first child of the right sibling
                p->child[1] = sib->child[0];
                p->key[1] = sib->key[0];
                _MoveNodeLeft(sib->child[0]);
                p->child[1]->index = 1;
                p->child[1]->parent = p;
                if (p->child[1]->Type() == Mf23TreeBase::LEAF)
                    _UpdateIns(MF23_KEY(p->child[1]), p);
                else
                    _UpdateIns(p->key[1], p);
                merged = 0;
            } else {
                // merge into the right sibling
                _MoveNodeRight(sib->child[0]);
                sib->child[0] = p->child[0];
                sib->key[0] = p->key[0];
                sib->child[0]->index = 0;
                sib->child[0]->parent = sib;
                p->child[0] = 0;
                merged = 1;
            }
        }
        if (old->Type() == Mf23TreeBase::NODE) {
            ((Mf23TreeNode *)old)->child[0] = 0;
            ((Mf23TreeNode *)old)->child[1] = 0;
            ((Mf23TreeNode *)old)->child[2] = 0;
        }
        delete old;
        old = p;
        p = p->parent;
        if (merged)
            _MoveNodeLeft(old);
        if (p->child[1])
            return;
    }
    // the root has one child left: pull its children up
    ((Mf23TreeNode *)old)->child[0] = 0;
    ((Mf23TreeNode *)old)->child[1] = 0;
    ((Mf23TreeNode *)old)->child[2] = 0;
    delete old;
    Mf23TreeNode *c = (Mf23TreeNode *)p->child[0];
    m_root->child[0] = c->child[0];
    m_root->child[0]->parent = p;
    m_root->child[1] = c->child[1];
    m_root->child[1]->parent = p;
    m_root->child[2] = c->child[2];
    m_root->child[2]->parent = p;
    m_root->key[0] = c->key[0];
    m_root->key[1] = c->key[1];
    c->child[0] = 0;
    c->child[1] = 0;
    c->child[2] = 0;
    delete c;
}

// 0x1000aca0 ?PrintList@MfOrderedList@@QAEXP6AXPAX@Z@Z
void MfOrderedList::PrintList(DataFn print)
{
    printf("\n\n");
    m_printCB = print;
    printf("0-Root<< ");
    _PrintChild(m_root);
    printf(" >>\n");
    _PrintChildren(m_root, 1);
}
