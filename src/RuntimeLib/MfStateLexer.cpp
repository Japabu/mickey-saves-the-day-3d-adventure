// The scanner of behaviour files (RuntimeLib.dll 1009bb00-1009cc70), generated
// by flex 2.4 (the skeleton of src/PiccoloComp/MfPcLexer.cpp) with a gperf
// keyword table; MfStateCompiler::Compile restarts it and the parser
// (1009cc70-) calls yylex.
//
// Start conditions: INITIAL and CODE (after the keywords that are followed
// by a code block). The rules, by their actions (the patterns are only in the
// tables):
//   1 white space   2 \n   3 "/*" comment   4 "//" comment   5 hexadecimal
//   6 octal   7 decimal   8 float   9 identifier or keyword
//   10 "\\" followed by name characters   11-14 '{' '}' ',' '#'
//   15 a code block (in CODE: "{" ... "}" counted, or
//   "{*!" ... "!*}")   16 CODE: other characters   17 CODE: \n
//   18 default rule (fatal)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MfStateCompiler.h"
#include "MfStringHeap.h"
#include "MfStateGrammar.h"

// The scanner reaches stdin, stdout and stderr through the imported _iob
// array.
#undef _iob
extern "C" __declspec(dllimport) FILE _iob[];
#undef stdin
#undef stdout
#undef stderr
#define stdin (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

#define YY_USE_PROTOS
#define YY_USE_CONST
#define yyconst const
#define YY_PROTO(proto) proto
#define YY_NULL 0
#define YY_SC_TO_UI(c) ((unsigned int) (unsigned char) c)
#define BEGIN yy_start = 1 + 2 *
#define YY_START ((yy_start - 1) / 2)
#define YY_STATE_EOF(state) (YY_END_OF_BUFFER + state + 1)
#define YY_NEW_FILE yyrestart( yyin )
#define YY_END_OF_BUFFER_CHAR 0
#define YY_BUF_SIZE 16384

typedef struct yy_buffer_state *YY_BUFFER_STATE;

#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2

#define unput(c) yyunput( c, yytext_ptr )

struct yy_buffer_state {
    FILE *yy_input_file;
    char *yy_ch_buf;            // input buffer
    char *yy_buf_pos;           // current position in input buffer
    int yy_buf_size;
    int yy_n_chars;
    int yy_is_interactive;
    int yy_fill_buffer;
    int yy_buffer_status;
#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
#define YY_BUFFER_EOF_PENDING 2
};

// the strings the parse allocates (released by MfStateCompiler::Compile)
// 0x1009bb00 _$E7 (original $E: the static initializer of gParseStrings, with 1009bb10-1009bb30)
// 0x1009bb10 _$E4
// 0x1009bb20 _$E6
// 0x1009bb30 _$E5
static MfStringHeap gParseStrings;

static YY_BUFFER_STATE yy_current_buffer = 0;
static char yy_hold_char;
static int yy_n_chars;
int yyleng;
static char *yy_c_buf_p = (char *)0;
static int yy_init = 1;
static int yy_start = 0;
static int yy_did_buffer_switch_on_eof;

static void yyunput(int c, char *buf_ptr);
void yyrestart(FILE *input_file);
void yy_load_buffer_state();
YY_BUFFER_STATE yy_create_buffer(FILE *file, int size);
void yy_init_buffer(YY_BUFFER_STATE b, FILE *file);
static void yy_fatal_error(const char msg[]);
static void *yy_flex_alloc(unsigned int size);
static void *yy_flex_realloc(void *ptr, unsigned int size);

typedef unsigned char YY_CHAR;
FILE *yyin = (FILE *)0, *yyout = (FILE *)0;
typedef int yy_state_type;
char *yytext;
#define yytext_ptr yytext

static yy_state_type yy_get_previous_state();
static int yyinput();
int yywrap();
static yy_state_type yy_try_NUL_trans(yy_state_type current_state);
static int yy_get_next_buffer();

#define YY_DO_BEFORE_ACTION \
    yytext_ptr = yy_bp; \
    yyleng = yy_cp - yy_bp; \
    yy_hold_char = *yy_cp; \
    *yy_cp = '\0'; \
    yy_c_buf_p = yy_cp;

#define YY_END_OF_BUFFER 19
static yyconst short int yy_accept[44] =
    {
        0,     1,     1,    16,    16,    19,    18,     1,     2,    14,
       13,    18,    18,    18,     7,     7,     9,    10,    11,    12,
       16,    17,    15,     1,     0,     8,     8,     3,     4,     8,
        6,     7,     0,     0,     9,    10,    16,     0,     0,     8,
        5,     0,     8,     0
    } ;

static yyconst int yy_ec[256] =
    {
        0,     1,     1,     1,     1,     1,     1,     1,     1,     2,
        3,     1,     1,     2,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     2,     1,     1,     4,     1,     1,     1,     1,
        1,     1,     5,     6,     7,     8,     9,    10,    11,    12,
       12,    12,    12,    12,    12,    12,    13,    13,     1,     1,
        1,     1,     1,     1,     1,    14,    14,    14,    14,    15,
       14,    16,    16,    16,    16,    16,    16,    16,    16,    16,
       16,    16,    16,    16,    16,    16,    16,    16,    17,    16,
       16,     1,    18,     1,     1,    19,     1,    14,    14,    14,
       14,    15,    14,    16,    16,    16,    16,    16,    16,    16,
       16,    16,    16,    16,    16,    16,    16,    16,    16,    16,
       20,    16,    16,    21,     1,    22,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1
    } ;

static yyconst int yy_meta[24] =
    {
        0,     1,     1,     1,     1,     1,     2,     1,     2,     3,
        1,     4,     4,     4,     5,     6,     7,     7,     8,     7,
        7,     1,     1,     0
    } ;

static yyconst short int yy_base[56] =
    {
        0,     0,     0,    21,    23,   106,   107,   102,   107,   107,
      107,    89,    16,    25,    36,    22,     0,     0,   107,   107,
       93,   107,   107,    81,    21,    31,    39,   107,   107,    35,
       19,    43,    35,     0,     0,     0,    36,    49,    48,    51,
        0,    54,    57,   107,    70,    76,    78,    81,    85,    90,
       95,    96,   101,     0,     0,     0
    } ;

static yyconst short int yy_def[56] =
    {
        0,    43,     1,    44,    44,    43,    43,    43,    43,    43,
       43,    45,    43,    43,    43,    46,    47,    48,    43,    43,
       43,    43,    43,    43,    43,    45,    49,    43,    43,    49,
       14,    46,    50,    51,    47,    48,    43,    52,    43,    43,
       51,    43,    43,     0,    43,    43,    43,    43,    43,    43,
       43,    43,    43,     0,     0,     0
    } ;

static yyconst short int yy_nxt[132] =
    {
        0,     6,     7,     8,     9,     6,     6,    10,    11,    12,
       13,    14,    15,    15,    16,    16,    16,    16,    17,    16,
       16,    18,    19,    20,    21,    20,    21,    26,    26,    26,
       27,    29,    26,    26,    26,    28,    43,    32,    36,    43,
       29,    38,    22,    38,    22,    29,    32,    30,    30,    31,
       32,    32,    29,    33,    37,    41,    33,    41,    32,    39,
       39,    39,    39,    39,    39,    42,    42,    42,    42,    42,
       42,     6,     6,     6,     6,     6,     6,     6,     6,    25,
       25,    31,    31,    23,    31,    34,    34,    34,    34,    35,
       35,    35,    35,    35,    26,    36,    26,    39,    24,    39,
       40,    40,    40,    42,    23,    42,    43,     5,    43,    43,
       43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
       43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
        0,     0
    } ;

static yyconst short int yy_chk[132] =
    {
        0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
        1,     1,     1,     3,     3,     4,     4,    12,    12,    12,
       13,    15,    24,    24,    24,    13,    30,    15,    36,    30,
       25,    32,     3,    32,     4,    14,    25,    14,    14,    14,
       29,    14,    31,    14,    26,    37,    14,    37,    31,    38,
       38,    38,    39,    39,    39,    41,    41,    41,    42,    42,
       42,    44,    44,    44,    44,    44,    44,    44,    44,    45,
       45,    46,    46,    23,    46,    47,    47,    47,    47,    48,
       48,    48,    48,    48,    49,    20,    49,    50,    11,    50,
       51,    51,    51,    52,     7,    52,     5,    43,    43,    43,
       43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
       43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
        0,     0
    } ;


static yy_state_type yy_last_accepting_state;
static char *yy_last_accepting_cpos;

#define YY_MORE_ADJ 0

#define INITIAL 0
#define CODE 1

// the compiler the scanner reports its position to
static MfStateCompiler *gScanCompiler;
// the code block being scanned
static MfString *gCode;

int KeywordToken(char *text, int length);

#define YY_INPUT(buf,result,max_size) result = gScanCompiler->ReadInput(buf, max_size);
#define YY_FATAL_ERROR(msg) yy_fatal_error(msg)
#define YY_READ_BUF_SIZE 8192
#define yyterminate() return YY_NULL
#define YY_USER_ACTION gScanCompiler->Advance(yytext, yyleng);
#define YY_BREAK break;

// 0x1009bb40 ?yylex@@YAHXZ
int yylex()
{
    register yy_state_type yy_current_state;
    register char *yy_cp, *yy_bp;
    register int yy_act;

    if (yy_init) {
        if (!yy_start)
            yy_start = 1;
        if (!yyin)
            yyin = stdin;
        if (!yyout)
            yyout = stdout;
        if (yy_current_buffer)
            yy_init_buffer(yy_current_buffer, yyin);
        else
            yy_current_buffer = yy_create_buffer(yyin, YY_BUF_SIZE);
        yy_load_buffer_state();
        yy_init = 0;
    }

    while (1) {
        yy_cp = yy_c_buf_p;
        *yy_cp = yy_hold_char;
        yy_bp = yy_cp;
        yy_current_state = yy_start;
yy_match:
        do {
            register YY_CHAR yy_c = yy_ec[YY_SC_TO_UI(*yy_cp)];
            if (yy_accept[yy_current_state]) {
                yy_last_accepting_state = yy_current_state;
                yy_last_accepting_cpos = yy_cp;
            }
            while (yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state) {
                yy_current_state = (int)yy_def[yy_current_state];
                if (yy_current_state >= 44)
                    yy_c = yy_meta[(unsigned int)yy_c];
            }
            yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int)yy_c];
            ++yy_cp;
        } while (yy_base[yy_current_state] != 107);

yy_find_action:
        yy_act = yy_accept[yy_current_state];

        YY_DO_BEFORE_ACTION;

do_action:
        switch (yy_act) {
        case 0:
            *yy_cp = yy_hold_char;
            yy_cp = yy_last_accepting_cpos;
            yy_current_state = yy_last_accepting_state;
            goto yy_find_action;

        case 1:
            YY_USER_ACTION
            YY_BREAK
        case 2:
            YY_USER_ACTION
            gScanCompiler->NextLine();
            YY_BREAK
        case 3:
            YY_USER_ACTION
            {
                int c;
                while ((c = yyinput()) != EOF) {
                    gScanCompiler->NextChar();
                    if (c == '*') {
                        c = yyinput();
                        if (c == '/') {
                            gScanCompiler->NextChar();
                            break;
                        }
                        unput(c);
                    } else if (c == '\n')
                        gScanCompiler->NextLine();
                }
            }
            YY_BREAK
        case 4:
            YY_USER_ACTION
            {
                int c;
                while ((c = yyinput()) != EOF) {
                    gScanCompiler->NextChar();
                    if (c == '\n') {
                        gScanCompiler->NextLine();
                        break;
                    }
                }
            }
            YY_BREAK
        case 5:
            YY_USER_ACTION
            yylval.ival = strtoul(yytext, 0, 16);
            return TOK_INTEGER;
            YY_BREAK
        case 6:
            YY_USER_ACTION
            yylval.ival = strtoul(yytext, 0, 8);
            return TOK_INTEGER;
            YY_BREAK
        case 7:
            YY_USER_ACTION
            yylval.ival = atoi(yytext);
            return TOK_INTEGER;
            YY_BREAK
        case 8:
            YY_USER_ACTION
            yylval.dval = atof(yytext);
            return TOK_FLOAT;
            YY_BREAK
        case 9:
            YY_USER_ACTION
            {
                int token = KeywordToken(yytext, yyleng);
                if (token) {
                    if (token == TOK_ENTRY_ACTION || token == TOK_EXIT_ACTION || token == TOK_INIT_VAR ||
                        token == TOK_FUNC_DEF || token == TOK_TRIG_GUARD || token == TOK_NOTE_CONTENTS ||
                        token == TOK_TRIG_ACTION)
                        BEGIN(CODE);
                    return token;
                }
                yylval.sval = gParseStrings.AllocString();
                *yylval.sval = yytext;
                return TOK_IDENTIFIER;
            }
            YY_BREAK
        case 10:
            YY_USER_ACTION
            yylval.sval = gParseStrings.AllocString();
            *yylval.sval = yytext;
            return TOK_STRING;
            YY_BREAK
        case 11:
            YY_USER_ACTION
            return TOK_LBRACE;
            YY_BREAK
        case 12:
            YY_USER_ACTION
            return TOK_RBRACE;
            YY_BREAK
        case 13:
            YY_USER_ACTION
            return TOK_COMMA;
            YY_BREAK
        case 14:
            YY_USER_ACTION
            return TOK_HASH;
            YY_BREAK
        case 15:
            YY_USER_ACTION
            {
                gCode = gParseStrings.AllocString();
                int depth = 1;
                int c = yyinput();
                MfString text;
                int c1 = yyinput();
                int c2 = yyinput();
                if (c1 == '*' && c2 == '!') {
                    gScanCompiler->NextChar();
                    gScanCompiler->NextChar();
                    gScanCompiler->NextChar();
                    c = yyinput();
                    while (c != EOF) {
                        gScanCompiler->NextChar();
                        if (c == '!') {
                            int e1 = yyinput();
                            int e2 = yyinput();
                            int e3 = yyinput();
                            if (e1 == '*' && e2 == c && e3 == '}') {
                                gScanCompiler->NextChar();
                                gScanCompiler->NextChar();
                                gScanCompiler->NextChar();
                                break;
                            }
                            unput(e3);
                            unput(e2);
                            unput(e1);
                        }
                        if (c == '\n')
                            gScanCompiler->NextLine();
                        *gCode << (char)c;
                        c = yyinput();
                    }
                } else {
                    unput(c2);
                    unput(c1);
                    do {
                        gScanCompiler->NextChar();
                        if (c == '{')
                            depth++;
                        if (c == '}')
                            depth--;
                        if (!depth)
                            break;
                        if (c == '\n')
                            gScanCompiler->NextLine();
                        *gCode << (char)c;
                        c = yyinput();
                    } while (c != EOF);
                }
                BEGIN(INITIAL);
                yylval.sval = gCode;
                return TOK_CODE;
            }
            YY_BREAK
        case 16:
            YY_USER_ACTION
            gScanCompiler->NextChar();
            YY_BREAK
        case 17:
            YY_USER_ACTION
            gScanCompiler->NextLine();
            YY_BREAK
        case 18:
            YY_USER_ACTION
            YY_FATAL_ERROR("flex scanner jammed");
            YY_BREAK
        case YY_STATE_EOF(INITIAL):
        case YY_STATE_EOF(CODE):
            yyterminate();

        case YY_END_OF_BUFFER: {
            int yy_amount_of_matched_text = yy_cp - yytext_ptr - 1;
            *yy_cp = yy_hold_char;
            if (yy_current_buffer->yy_buffer_status == YY_BUFFER_NEW) {
                yy_n_chars = yy_current_buffer->yy_n_chars;
                yy_current_buffer->yy_input_file = yyin;
                yy_current_buffer->yy_buffer_status = YY_BUFFER_NORMAL;
            }
            if (yy_c_buf_p <= &yy_current_buffer->yy_ch_buf[yy_n_chars]) {
                yy_state_type yy_next_state;
                yy_c_buf_p = yytext_ptr + yy_amount_of_matched_text;
                yy_current_state = yy_get_previous_state();
                yy_next_state = yy_try_NUL_trans(yy_current_state);
                yy_bp = yytext_ptr + YY_MORE_ADJ;
                if (yy_next_state) {
                    yy_cp = ++yy_c_buf_p;
                    yy_current_state = yy_next_state;
                    goto yy_match;
                } else {
                    yy_cp = yy_c_buf_p;
                    goto yy_find_action;
                }
            } else
                switch (yy_get_next_buffer()) {
                case EOB_ACT_END_OF_FILE:
                    yy_did_buffer_switch_on_eof = 0;
                    if (yywrap()) {
                        yy_c_buf_p = yytext_ptr + YY_MORE_ADJ;
                        yy_act = YY_STATE_EOF(YY_START);
                        goto do_action;
                    } else {
                        if (!yy_did_buffer_switch_on_eof)
                            YY_NEW_FILE;
                    }
                    break;
                case EOB_ACT_CONTINUE_SCAN:
                    yy_c_buf_p = yytext_ptr + yy_amount_of_matched_text;
                    yy_current_state = yy_get_previous_state();
                    yy_cp = yy_c_buf_p;
                    yy_bp = yytext_ptr + YY_MORE_ADJ;
                    goto yy_match;
                case EOB_ACT_LAST_MATCH:
                    yy_c_buf_p = &yy_current_buffer->yy_ch_buf[yy_n_chars];
                    yy_current_state = yy_get_previous_state();
                    yy_cp = yy_c_buf_p;
                    yy_bp = yytext_ptr + YY_MORE_ADJ;
                    goto yy_find_action;
                }
            break;
        }

        default:
            YY_FATAL_ERROR("fatal flex scanner internal error--no action found");
        }
    }
}

// 0x1009c480 ?yy_get_next_buffer@@YAHXZ
static int yy_get_next_buffer()
{
    register char *dest = yy_current_buffer->yy_ch_buf;
    register char *source = yytext_ptr - 1;
    register int number_to_move, i;
    int ret_val;

    if (yy_c_buf_p > &yy_current_buffer->yy_ch_buf[yy_n_chars + 1])
        YY_FATAL_ERROR("fatal flex scanner internal error--end of buffer missed");

    if (yy_current_buffer->yy_fill_buffer == 0) {
        if (yy_c_buf_p - yytext_ptr - YY_MORE_ADJ == 1)
            return EOB_ACT_END_OF_FILE;
        else
            return EOB_ACT_LAST_MATCH;
    }

    number_to_move = yy_c_buf_p - yytext_ptr;

    for (i = 0; i < number_to_move; ++i)
        *(dest++) = *(source++);

    if (yy_current_buffer->yy_buffer_status == YY_BUFFER_EOF_PENDING)
        yy_n_chars = 0;
    else {
        int num_to_read = yy_current_buffer->yy_buf_size - number_to_move - 1;

        while (num_to_read <= 0) {
            YY_BUFFER_STATE b = yy_current_buffer;
            int yy_c_buf_p_offset = yy_c_buf_p - b->yy_ch_buf;

            b->yy_buf_size *= 2;
            b->yy_ch_buf = (char *)yy_flex_realloc((void *)b->yy_ch_buf, b->yy_buf_size);

            if (!b->yy_ch_buf)
                YY_FATAL_ERROR("fatal error - scanner input buffer overflow");

            yy_c_buf_p = &b->yy_ch_buf[yy_c_buf_p_offset];

            num_to_read = yy_current_buffer->yy_buf_size - number_to_move - 1;
        }

        if (num_to_read > YY_READ_BUF_SIZE)
            num_to_read = YY_READ_BUF_SIZE;

        YY_INPUT((&yy_current_buffer->yy_ch_buf[number_to_move]), yy_n_chars, num_to_read);
    }

    if (yy_n_chars == 0) {
        if (number_to_move - YY_MORE_ADJ == 1) {
            ret_val = EOB_ACT_END_OF_FILE;
            yyrestart(yyin);
        } else {
            ret_val = EOB_ACT_LAST_MATCH;
            yy_current_buffer->yy_buffer_status = YY_BUFFER_EOF_PENDING;
        }
    } else
        ret_val = EOB_ACT_CONTINUE_SCAN;

    yy_n_chars += number_to_move;
    yy_current_buffer->yy_ch_buf[yy_n_chars] = YY_END_OF_BUFFER_CHAR;
    yy_current_buffer->yy_ch_buf[yy_n_chars + 1] = YY_END_OF_BUFFER_CHAR;

    yytext_ptr = &yy_current_buffer->yy_ch_buf[1];

    return ret_val;
}

// 0x1009c600 ?yy_get_previous_state@@YAHXZ
static yy_state_type yy_get_previous_state()
{
    register yy_state_type yy_current_state;
    register char *yy_cp;

    yy_current_state = yy_start;

    for (yy_cp = yytext_ptr + YY_MORE_ADJ; yy_cp < yy_c_buf_p; ++yy_cp) {
        register YY_CHAR yy_c = (*yy_cp ? yy_ec[YY_SC_TO_UI(*yy_cp)] : 1);
        if (yy_accept[yy_current_state]) {
            yy_last_accepting_state = yy_current_state;
            yy_last_accepting_cpos = yy_cp;
        }
        while (yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state) {
            yy_current_state = (int)yy_def[yy_current_state];
            if (yy_current_state >= 44)
                yy_c = yy_meta[(unsigned int)yy_c];
        }
        yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int)yy_c];
    }

    return yy_current_state;
}

// 0x1009c6e0 ?yy_try_NUL_trans@@YAHH@Z
static yy_state_type yy_try_NUL_trans(yy_state_type yy_current_state)
{
    register int yy_is_jam;
    register char *yy_cp = yy_c_buf_p;

    register YY_CHAR yy_c = 1;
    if (yy_accept[yy_current_state]) {
        yy_last_accepting_state = yy_current_state;
        yy_last_accepting_cpos = yy_cp;
    }
    while (yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state) {
        yy_current_state = (int)yy_def[yy_current_state];
        if (yy_current_state >= 44)
            yy_c = yy_meta[(unsigned int)yy_c];
    }
    yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int)yy_c];
    yy_is_jam = (yy_current_state == 43);

    return yy_is_jam ? 0 : yy_current_state;
}

// 0x1009c790 ?yyunput@@YAXHPAD@Z
static void yyunput(int c, register char *yy_bp)
{
    register char *yy_cp = yy_c_buf_p;

    *yy_cp = yy_hold_char;

    if (yy_cp < yy_current_buffer->yy_ch_buf + 2) {
        register int number_to_move = yy_n_chars + 2;
        register char *dest = &yy_current_buffer->yy_ch_buf[yy_current_buffer->yy_buf_size + 2];
        register char *source = &yy_current_buffer->yy_ch_buf[number_to_move];

        while (source > yy_current_buffer->yy_ch_buf)
            *--dest = *--source;

        yy_cp += dest - source;
        yy_bp += dest - source;
        yy_n_chars = yy_current_buffer->yy_buf_size;

        if (yy_cp < yy_current_buffer->yy_ch_buf + 2)
            YY_FATAL_ERROR("flex scanner push-back overflow");
    }

    if (yy_cp > yy_bp && yy_cp[-1] == '\n')
        yy_cp[-2] = '\n';

    *--yy_cp = (char)c;

    YY_DO_BEFORE_ACTION;
}

// 0x1009c850 ?yyinput@@YAHXZ
static int yyinput()
{
    int c;

    *yy_c_buf_p = yy_hold_char;

    if (*yy_c_buf_p == YY_END_OF_BUFFER_CHAR) {
        if (yy_c_buf_p < &yy_current_buffer->yy_ch_buf[yy_n_chars])
            *yy_c_buf_p = '\0';
        else {
            yytext_ptr = yy_c_buf_p;
            ++yy_c_buf_p;

            switch (yy_get_next_buffer()) {
            case EOB_ACT_END_OF_FILE: {
                if (yywrap()) {
                    yy_c_buf_p = yytext_ptr + YY_MORE_ADJ;
                    return EOF;
                }

                YY_NEW_FILE;
                return yyinput();
            }

            case EOB_ACT_CONTINUE_SCAN:
                yy_c_buf_p = yytext_ptr + YY_MORE_ADJ;
                break;

            case EOB_ACT_LAST_MATCH:
                YY_FATAL_ERROR("unexpected last match in yyinput()");
            }
        }
    }

    c = *yy_c_buf_p == (char)EOF ? EOF : (unsigned char)*yy_c_buf_p;
    *yy_c_buf_p = '\0';
    yy_hold_char = *++yy_c_buf_p;

    return c;
}

// 0x1009c940 ?yyrestart@@YAXPAU_iobuf@@@Z
void yyrestart(FILE *input_file)
{
    if (!yy_current_buffer)
        yy_current_buffer = yy_create_buffer(yyin, YY_BUF_SIZE);

    yy_init_buffer(yy_current_buffer, input_file);
    yy_load_buffer_state();
}

// 0x1009c980 ?yy_load_buffer_state@@YAXXZ
void yy_load_buffer_state()
{
    yy_n_chars = yy_current_buffer->yy_n_chars;
    yytext = yy_c_buf_p = yy_current_buffer->yy_buf_pos;
    yyin = yy_current_buffer->yy_input_file;
    yy_hold_char = *yy_c_buf_p;
}

// 0x1009c9b0 ?yy_create_buffer@@YAPAUyy_buffer_state@@PAU_iobuf@@H@Z
YY_BUFFER_STATE yy_create_buffer(FILE *file, int size)
{
    YY_BUFFER_STATE b;

    b = (YY_BUFFER_STATE)yy_flex_alloc(sizeof(struct yy_buffer_state));

    if (!b)
        yy_fatal_error("out of dynamic memory in yy_create_buffer()");

    b->yy_buf_size = size;

    // two characters longer for the two end-of-buffer characters
    b->yy_ch_buf = (char *)yy_flex_alloc(b->yy_buf_size + 2);

    if (!b->yy_ch_buf)
        yy_fatal_error("out of dynamic memory in yy_create_buffer()");

    yy_init_buffer(b, file);

    return b;
}

// 0x1009ca10 ?yy_init_buffer@@YAXPAUyy_buffer_state@@PAU_iobuf@@@Z
void yy_init_buffer(YY_BUFFER_STATE b, FILE *file)
{
    b->yy_input_file = file;

    b->yy_ch_buf[0] = '\n';
    b->yy_n_chars = 1;

    b->yy_ch_buf[1] = YY_END_OF_BUFFER_CHAR;
    b->yy_ch_buf[2] = YY_END_OF_BUFFER_CHAR;

    b->yy_buf_pos = &b->yy_ch_buf[1];

    b->yy_is_interactive = 0;

    b->yy_fill_buffer = 1;

    b->yy_buffer_status = YY_BUFFER_NEW;
}

// 0x1009ca50 ?yy_fatal_error@@YAXQBD@Z
static void yy_fatal_error(const char msg[])
{
    (void)fprintf(stderr, "%s\n", msg);
    exit(1);
}

// 0x1009ca80 ?yy_flex_alloc@@YAPAXI@Z
static void *yy_flex_alloc(unsigned int size)
{
    return (void *)malloc(size);
}

// 0x1009ca90 ?yy_flex_realloc@@YAPAXPAXI@Z
static void *yy_flex_realloc(void *ptr, unsigned int size)
{
    return (void *)realloc(ptr, size);
}

/* The keyword table (gperf, with duplicate hash values). */

#define TOTAL_KEYWORDS 29
#define MIN_WORD_LENGTH 4
#define MAX_WORD_LENGTH 14
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 49

struct resword { char *name; int token; };

// 0x1009cab0 ?hash@@YAHPADH@Z
static int hash(register char *str, register int len)
{
    static unsigned char asso_values[] =
    {
         50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
         50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
         50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
         50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
         50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
         50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
         50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
         50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
         50,  50,  50,  50,  50,  50,  50,  50,  50,  50,
         50,  50,  50,  50,  50,  50,  50,  50,   5,   0,
         15,  10,   0,  20,  50,   0,  50,  10,  50,  10,
          0,  50,   0,  50,   5,  15,   0,  50,  30,  50,
         50,   0,  50,  50,  50,  50,  50,  50
    };
    return len + asso_values[str[len - 1]] + asso_values[str[0]];
}

// 0x1009cae0 ?in_word_set@@YAPAUresword@@PADH@Z
static struct resword *in_word_set(register char *str, register int len)
{
    static struct resword wordlist[] =
    {
        {"",}, {"",}, {"",}, {"",},
        {"init", TOK_INIT},
        {"trig_by", TOK_TRIG_BY},
        {"func_def", TOK_FUNC_DEF},
        {"choice_pt", TOK_CHOICE_PT},
        {"transition", TOK_TRANSITION},
        {"trig_action", TOK_TRIG_ACTION},
        {"init_var", TOK_INIT_VAR},
        {"note", TOK_NOTE},
        {"begin_hist", TOK_BEGIN_HIST},
        {"exitby", TOK_EXITBY},
        {"enterby", TOK_ENTERBY},
        {"playback", TOK_PLAYBACK},
        {"exit_port", TOK_EXIT_PORT},
        {"trans_type", TOK_TRANS_TYPE},
        {"entry_port", TOK_ENTRY_PORT},
        {"exit_action", TOK_EXIT_ACTION},
        {"entry_action", TOK_ENTRY_ACTION},
        {"functions", TOK_FUNCTIONS},
        {"trig_guard", TOK_TRIG_GUARD},
        {"mf_hfsm", TOK_MF_HFSM},
        {"note_contents", TOK_NOTE_CONTENTS},
        {"dest_choice_pt", TOK_DEST_CHOICE_PT},
        {"state", TOK_STATE},
        {"camera_setting", TOK_CAMERA_SETTING},
        {"dest_state", TOK_DEST_STATE},
        {"start_state", TOK_START_STATE},
        {"visual_con", TOK_VISUAL_CON},
        {"display_pos", TOK_DISPLAY_POS},
        {"vars", TOK_VARS},
    };

    static signed char lookup[] =
    {
         -1,  -1,  -1,  -1,   4,  -1,  -1,   5,   6,   7,
          8,   9,  -1,  10,  11,  12,  13,  14,  15,  16,
         60,  19,  20,  -1,  21,  22,  -1,  23,  24,  25,
         26, -17,  -2,  -1,  27,  28,  29,  -1,  -1,  -1,
         30,  31,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  32
    };

    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
        register int key = hash(str, len);

        if (key <= MAX_HASH_VALUE && key >= 0) {
            register int index = lookup[key];

            if (index >= 0 && index < MAX_HASH_VALUE) {
                register char *s = wordlist[index].name;

                if (*s == *str && !strcmp(str + 1, s + 1))
                    return &wordlist[index];
            } else if (index < 0 && index >= -MAX_HASH_VALUE)
                return 0;
            else {
                register int offset = key + index + (index > 0 ? -MAX_HASH_VALUE : MAX_HASH_VALUE);
                register struct resword *base = &wordlist[-lookup[offset]];
                register struct resword *ptr = base + -lookup[offset + 1];

                while (--ptr >= base)
                    if (*str == *ptr->name && !strcmp(str + 1, ptr->name + 1))
                        return ptr;
            }
        }
    }
    return 0;
}

// 0x1009cc30 ?KeywordToken@@YAHPADH@Z
int KeywordToken(char *text, int length)
{
    struct resword *keyword = in_word_set(text, length);
    if (keyword)
        return keyword->token;
    return 0;
}

int yywrap()
{
    return 1;
}

// 0x1009cc50 ?ReleaseParseStrings@@YAXXZ
void ReleaseParseStrings()
{
    gParseStrings.ReleaseAll();
}

// 0x1009cc60 ?SetScanCompiler@@YAXPAVMfStateCompiler@@@Z
void SetScanCompiler(MfStateCompiler *compiler)
{
    gScanCompiler = compiler;
}
