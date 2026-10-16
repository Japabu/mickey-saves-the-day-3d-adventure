#pragma once
// MfList: doubly linked list of MfListItem pointers, with forward and
// backward iterators. Exported names only; members named from their use.
class MfListItem;

struct MfListNode {
    MfListItem *item;
    MfListNode *prev;
    MfListNode *next;
};

class __declspec(dllexport) MfList {
    friend class MfListIter;
    friend class MfListIterBwd;
public:
    MfList();
    MfList(const MfList &);
    ~MfList();
    MfList &operator=(const MfList &);
    int length() const;
    void add(MfListItem *);
    void addFirst(MfListItem *);
    void addLast(MfListItem *);
    int remove(MfListItem *);
    int contains(MfListItem *);
    void removeContents();
    void deleteContents();
protected:
    void removeNode(MfListNode *);
    void deleteNode(MfListNode *);
private:
    int m_count;
    MfListNode *m_head;
    MfListNode *m_tail;
};

class __declspec(dllexport) MfListIter {
public:
    MfListIter();
    void start(const MfList &);
    void advance();
    MfListItem *operator*();
    MfListItem *prev();
    MfListItem *next();
    void removeItem();
    void deleteItem();
protected:
    MfList *m_list;
    MfListNode *m_cur;
    MfListNode *m_next;
};

class __declspec(dllexport) MfListIterBwd : public MfListIter {
public:
    MfListIterBwd() {}
    void start(const MfList &);
    void advance();
};
