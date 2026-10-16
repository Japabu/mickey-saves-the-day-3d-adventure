#pragma once
#include "MfPcParserItem.h"
class MfPcChunkList;
class MfPcExpression;

// A declared name while the parser builds a declaration (0x14 bytes): the
// identifier (or the null expression after a syntax error) and, for an array,
// the code that computes its size.
class MfPcDeclarator : public MfPcParserItem {
public:
    MfPcDeclarator(MfPcExpression *name, MfPcChunkList *code) : mName(name), mCode(code) {}
    virtual ~MfPcDeclarator() {}

    MfPcExpression *mName;      // +0x0c
    MfPcChunkList *mCode;       // +0x10
};
