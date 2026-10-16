#pragma once
#include <stdio.h>
class MfPcCompiler;
class MfPcTokenInfo;

// The Piccolo grammar (Berkeley yacc, prefix "pc") in MfPcParser.cpp and its
// scanner (flex 2.4, prefix "pc") in MfPcLexer.cpp.

class MfPcChunkList;
class MfPcDeclarator;
class MfPcExpression;
class MfPcExpressionList;
class MfString;

// The value of a grammar symbol (0x14 bytes): the semantic value followed by
// the source position of the symbol, laid out like MfPcTokenInfo so that the
// scanner records the position of each token in place.
struct YYSTYPE {
    YYSTYPE() {}
    union {
        int ival;                   // INTEGER, CHARACTER, assignment operators
        float fval;                 // FLOAT
        char *sval;                 // IDENTIFIER (text in the compiler's string pool)
        MfString *str;              // STRING
        MfPcExpression *expr;       // expressions, identifiers
        MfPcExpressionList *exprs;  // argument, parameter and initializer lists
        MfPcChunkList *chunks;      // statements, declarations, function bodies
        MfPcDeclarator *decl;       // the name in a declaration
    };
    int line;
    int startColumn;
    int endColumn;
    char *text;
};
extern YYSTYPE pclval;

extern int pcdebug;
int pcparse();
int pclex();
void pcrestart(FILE *input);
void PcLexReset();
void PcSetLexCompiler(MfPcCompiler *compiler);
void PcSetParseCompiler(MfPcCompiler *compiler);
int PcLookupKeyword(char *text, int length);
