// TSymtab.h - a symbol table of the script compiler.
#ifndef MBENGINE_TSYMTAB_H
#define MBENGINE_TSYMTAB_H

class IMBStream;
class TSymtabNode;

class TSymtab {
    TSymtabNode *root;
    long nodeCnt;
    long symbolTableIndex;
    static long symbolTableCnt;
    static TSymtab *symbolTableList;
    // stand-in: the original's ctype id $E22/$E21 are one past <string> (18/19) + TSymtab's
    // 2 statics; the obj's PDB data has no further guard/static, so one more static data member
    // declaration takes it (name unknown; never defined or used)
    static long sUnknownSymtabStatic;
    TSymtab *next;
public:
    TSymtab(void);
    ~TSymtab(void);
    TSymtabNode *Search(const char *pString) const;
    TSymtabNode *Enter(const char *pString);
    TSymtabNode *Root(void) const { return root; }
    TSymtab *Next(void) const { return next; }
    int NodeCount(void) const { return nodeCnt; }
    void Print(void) const { root->Print(); }
    unsigned long StreamOut(IMBStream *outFile);
};

#endif
