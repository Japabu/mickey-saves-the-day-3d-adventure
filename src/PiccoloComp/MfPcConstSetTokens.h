#pragma once

// The tokens of the constant set grammar (its y.tab.h), shared by the parser
// (MfPcConstSetParser.cpp) and the scanner (MfPcConstSetLexer.cpp): the
// tokens of the Piccolo grammar without '#'.
#define CSTOK_IDENTIFIER       257
#define CSTOK_INTEGER          258
#define CSTOK_FLOAT            259
#define CSTOK_STRING           260
#define CSTOK_CHARACTER        261
#define CSTOK_BREAK            262
#define CSTOK_CASE             263
#define CSTOK_CATCH            264
#define CSTOK_CONST            265
#define CSTOK_CONTINUE         266
#define CSTOK_DEFAULT          267
#define CSTOK_DO               268
#define CSTOK_ELSE             269
#define CSTOK_FOR              270
#define CSTOK_FUNCTION         271
#define CSTOK_GOTO             272
#define CSTOK_GLOBAL           273
#define CSTOK_IF               274
#define CSTOK_NEW              275
#define CSTOK_NIL              276
#define CSTOK_RETURN           277
#define CSTOK_SWITCH           278
#define CSTOK_THIS             279
#define CSTOK_THROW            280
#define CSTOK_TRY              281
#define CSTOK_USE              282
#define CSTOK_VAR              283
#define CSTOK_VOID             284
#define CSTOK_WHILE            285
#define CSTOK_NOT              286  // !
#define CSTOK_MOD              287  // %
#define CSTOK_XOR              288  // ^
#define CSTOK_AND              289  // &
#define CSTOK_MULT             290  // *
#define CSTOK_LPAREN           291  // (
#define CSTOK_RPAREN           292  // )
#define CSTOK_MINUS            293  // -
#define CSTOK_PLUS             294  // +
#define CSTOK_ASSIGN           295  // =
#define CSTOK_LBRACE           296  // {
#define CSTOK_RBRACE           297  // }
#define CSTOK_OR               298  // |
#define CSTOK_BITNOT           299  // ~
#define CSTOK_LBRACKET         300  // [
#define CSTOK_RBRACKET         301  // ]
#define CSTOK_SEMICOLON        302  // ;
#define CSTOK_COLON            303  // :
#define CSTOK_GT               304  // >
#define CSTOK_LT               305  // <
#define CSTOK_QUESTION         306  // ?
#define CSTOK_COMMA            307  // ,
#define CSTOK_DOT              308  // .
#define CSTOK_DIV              309  // /
#define CSTOK_INCR             310  // ++
#define CSTOK_DECR             311  // --
#define CSTOK_LSHIFT           312  // <<
#define CSTOK_RSHIFT           313  // >>
#define CSTOK_SCOPE            314  // ::
#define CSTOK_DOLLAR           315  // $
#define CSTOK_GTE              316  // >=
#define CSTOK_LTE              317  // <=
#define CSTOK_EQ               318  // ==
#define CSTOK_NE               319  // !=
#define CSTOK_LOGAND           320  // &&
#define CSTOK_LOGOR            321  // ||
#define CSTOK_MULT_ASSIGN      322  // *=
#define CSTOK_DIV_ASSIGN       323  // /=
#define CSTOK_MOD_ASSIGN       324  // %=
#define CSTOK_ADD_ASSIGN       325  // +=
#define CSTOK_SUB_ASSIGN       326  // -=
#define CSTOK_LSHIFT_ASSIGN    327  // <<=
#define CSTOK_RSHIFT_ASSIGN    328  // >>=
#define CSTOK_AND_ASSIGN       329  // &=
#define CSTOK_XOR_ASSIGN       330  // ^=
#define CSTOK_OR_ASSIGN        331  // |=
