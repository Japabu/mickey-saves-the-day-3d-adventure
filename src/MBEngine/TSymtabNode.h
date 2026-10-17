// TSymtabNode.h - a symbol of the script compiler.
#ifndef MBENGINE_TSYMTABNODE_H
#define MBENGINE_TSYMTABNODE_H

class IMBStream;
class TSymtab;

class TSymtabNode {
    TSymtabNode *left;
    TSymtabNode *right;
    unsigned long nodeIndex;
    char name[32];
public:
    union {
        float num;
        char str[32];
    } value;
    unsigned long type;
    void *param;

    TSymtabNode(void);
    TSymtabNode(const char *theName, long index);
    ~TSymtabNode(void);
    const char *Name(void) { return name; }
    const char *GetString(void) { return value.str; }
    TSymtabNode *LeftSubtree(void) { return left; }
    TSymtabNode *RightSubtree(void) { return right; }
    void SetValue(float num);
    void SetValue(long num);
    void SetValue(const char *str);
    unsigned long Index(void) const { return nodeIndex; }
    void Print(void) const;
    unsigned long StreamOut(IMBStream *outFile);
    unsigned long StreamIn(const char *buffer);

    friend class TSymtab;
};

#endif
