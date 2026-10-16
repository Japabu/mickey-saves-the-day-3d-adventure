#pragma once
#include <stdio.h>
class MfPcConstSet;
class MfPcExpression;
class MfString;

// The constant set grammar (Berkeley yacc, prefix "cs") in
// MfPcConstSetParser.cpp and its scanner (flex 2.4, prefix "cs") in
// MfPcConstSetLexer.cpp.

// The value of a grammar symbol.
typedef union {
    int ival;                   // INTEGER, CHARACTER
    float fval;                 // FLOAT
    char *sval;                 // IDENTIFIER
    MfString *str;              // STRING
    MfPcExpression *expr;       // expressions
} YYSTYPE;
extern YYSTYPE cslval;

int csparse();
int cslex();
void csrestart(FILE *input);
void CsLexReset();
void CsSetLexConstSet(MfPcConstSet *constSet);
void CsSetParseConstSet(MfPcConstSet *constSet);
int CsLookupKeyword(char *text, int length);
