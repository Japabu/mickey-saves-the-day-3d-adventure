#pragma once

// The tokens of the Piccolo grammar (yacc's y.tab.h), shared by the parser
// (MfPcParser.cpp) and the scanner (MfPcLexer.cpp). The keywords come from
// the scanner's keyword table.
#define TOK_IDENTIFIER         257
#define TOK_INTEGER            258
#define TOK_FLOAT              259
#define TOK_STRING             260
#define TOK_CHARACTER          261
#define TOK_BREAK              262
#define TOK_CASE               263
#define TOK_CATCH              264
#define TOK_CONST              265
#define TOK_CONTINUE           266
#define TOK_DEFAULT            267
#define TOK_DO                 268
#define TOK_ELSE               269
#define TOK_FOR                270
#define TOK_FUNCTION           271
#define TOK_GOTO               272
#define TOK_GLOBAL             273
#define TOK_IF                 274
#define TOK_NEW                275
#define TOK_NIL                276
#define TOK_RETURN             277
#define TOK_SWITCH             278
#define TOK_THIS               279
#define TOK_THROW              280
#define TOK_TRY                281
#define TOK_USE                282
#define TOK_VAR                283
#define TOK_VOID               284
#define TOK_WHILE              285
#define TOK_NOT                286  // !
#define TOK_MOD                287  // %
#define TOK_XOR                288  // ^
#define TOK_AND                289  // &
#define TOK_MULT               290  // *
#define TOK_LPAREN             291  // (
#define TOK_RPAREN             292  // )
#define TOK_MINUS              293  // -
#define TOK_PLUS               294  // +
#define TOK_ASSIGN             295  // =
#define TOK_LBRACE             296  // {
#define TOK_RBRACE             297  // }
#define TOK_OR                 298  // |
#define TOK_BITNOT             299  // ~
#define TOK_LBRACKET           300  // [
#define TOK_RBRACKET           301  // ]
#define TOK_SEMICOLON          302  // ;
#define TOK_COLON              303  // :
#define TOK_GT                 304  // >
#define TOK_LT                 305  // <
#define TOK_QUESTION           306  // ?
#define TOK_COMMA              307  // ,
#define TOK_DOT                308  // .
#define TOK_DIV                309  // /
#define TOK_INCR               310  // ++
#define TOK_DECR               311  // --
#define TOK_LSHIFT             312  // <<
#define TOK_RSHIFT             313  // >>
#define TOK_SCOPE              314  // ::
#define TOK_DOLLAR             315  // $
#define TOK_HASH               316  // #
#define TOK_GTE                317  // >=
#define TOK_LTE                318  // <=
#define TOK_EQ                 319  // ==
#define TOK_NE                 320  // !=
#define TOK_LOGAND             321  // &&
#define TOK_LOGOR              322  // ||
#define TOK_MULT_ASSIGN        323  // *=
#define TOK_DIV_ASSIGN         324  // /=
#define TOK_MOD_ASSIGN         325  // %=
#define TOK_ADD_ASSIGN         326  // +=
#define TOK_SUB_ASSIGN         327  // -=
#define TOK_LSHIFT_ASSIGN      328  // <<=
#define TOK_RSHIFT_ASSIGN      329  // >>=
#define TOK_AND_ASSIGN         330  // &=
#define TOK_XOR_ASSIGN         331  // ^=
#define TOK_OR_ASSIGN          332  // |=

