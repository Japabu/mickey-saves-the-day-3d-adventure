#pragma once

// Singly linked list of values (Utilities/ValList.h) with a cursor
// (curr/cnum) for First/Next iteration.
template <class T>
class ValList {
public:
    ValList(const ValList<T> &l)
    {
        head = tail = curr = 0;
        tnum = cnum = 0;
        AddList(l);
    }
    ValList() { head = tail = curr = 0; tnum = cnum = 0; }
    ~ValList() { RemoveAll(); }
    bool InList() const;
    int Count() const { return tnum; }
    int Index() const { return cnum; }
    T First();
    T Next();
    T Last();
    T Curr() { return curr->elem; }
    T GetAt(int i)
    {
        if (i < 0 || i >= tnum)
            return 0;
        if (cnum != i) {
            if (cnum > i) {
                cnum = 0;
                curr = head;
            }
            while (cnum < i) {
                cnum++;
                curr = curr->next;
            }
        }
        return curr->elem;
    }
    int FindFirst(T);
    int FindNext(T);
    int Find(T);
    int Add(T t)
    {
        curr = new L;
        if (!curr)
            return -1;
        curr->next = 0;
        curr->elem = t;
        cnum = tnum++;
        if (!head)
            head = tail = curr;
        else {
            tail->next = curr;
            tail = curr;
        }
        return cnum;
    }
    int Insert(T, int);
    T Remove()
    {
        L *l = head;
        T t = l->elem;
        head = l->next;
        curr = head;
        cnum = 0;
        if (!head)
            head = tail = 0;
        delete l;
        tnum--;
        return t;
    }
    T Remove(T);
    T RemoveAt(int);
    void RemoveAll()
    {
        while (head) {
            curr = head->next;
            delete head;
            head = curr;
        }
        head = tail = curr = 0;
        tnum = cnum = 0;
    }
    int AddList(const ValList<T> &l)
    {
        for (L *p = l.head; p; p = p->next) {
            if (Add(p->elem) < 0)
                return -1;
        }
        return cnum;
    }
    int InsertList(const ValList<T> &, int);
    void RemoveList(const ValList<T> &);
    void Swap(int, int);
    void Sort(int (__cdecl *)(const T &, const T &));
    void Sort();
    ValList<T> &operator=(const ValList<T> &);
    T &operator[](int i)
    {
        if (cnum == i)
            return curr->elem;
        if (cnum >= i) {
            curr = head;
            cnum = 0;
        }
        while (cnum < i) {
            cnum++;
            curr = curr->next;
        }
        return curr->elem;
    }
    struct L {
        T elem;
        L *next;
    };
protected:
    L *head;
    L *tail;
    L *curr;
    int tnum;
    int cnum;
};
