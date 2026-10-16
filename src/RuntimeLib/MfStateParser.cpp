// The parser of behaviour files (RuntimeLib.dll 1009cc70-1009dee0), generated
// by Berkeley yacc 1.9 (the skeleton of src/PiccoloComp/MfPcParser.cpp) from
// the grammar below; the tables are data copied from the binary, the grammar
// was recovered from them. The actions build the machines, states and
// transitions and hand the machines to the compiler (MfStateCompiler::Finish).
//
// The grammar (rule numbers as in the tables):
//   1 file: machines_opt
//   2 machine: hfsm
//   3 machines_opt: /* empty */                   4 | machines
//   5 machines: machine                           6 | machines machine
//   7 hfsm: MF_HFSM name '{' hfsm_items_opt '}'
//   8 hfsm_item: state   9 | func_def   10 | note   11 | choice_pt   12 | camera_setting
//  13 | playback
//  14 hfsm_items_opt: /* empty */                15 | hfsm_items
//  16 hfsm_items: hfsm_item                      17 | hfsm_items hfsm_item
//  18 func_def: FUNC_DEF CODE
//  19 state: STATE name '{' state_items_opt '}'
//  20 state_item: display_pos   21 | camera_setting   22 | vars   23 | entry_action
//  24 | exit_action   25 | init_var   26 | transition   27 | state   28 | func_def
//  29 | start_state   30 | begin_hist   31 | exit_port   32 | entry_port   33 | choice_pt
//  34 | note   35 | playback
//  36 state_items: state_item                    37 | state_items state_item
//  38 state_items_opt: /* empty */               39 | state_items
//  40 display_pos: DISPLAY_POS '{' FLOAT FLOAT FLOAT FLOAT '}'
//  41 camera_setting: CAMERA_SETTING '{' FLOAT FLOAT FLOAT FLOAT '}'
//  42 playback: PLAYBACK '{' INTEGER '}'
//  43 name_list: name                            44 | name_list ',' name
//  45 vars: VARS '{' name_list_opt '}'
//  46 name_list_opt: /* empty */                 47 | name_list
//  48 start_state: START_STATE
//  49 begin_hist: BEGIN_HIST
//  50 entry_action: ENTRY_ACTION CODE
//  51 exit_action: EXIT_ACTION CODE
//  52 init_var: INIT_VAR CODE
//  53 transition: TRANSITION name '{' trans_items_opt '}'
//  54 trans_item: visual_con   55 | trig_by   56 | trig_action   57 | trans_type
//  58 | dest_state   59 | init   60 | exitby   61 | enterby   62 | dest_choice_pt
//  63 trans_items: trans_item                    64 | trans_items trans_item
//  65 trans_items_opt: /* empty */               66 | trans_items
//  67 visual_con: VISUAL_CON FLOAT FLOAT '{' points_opt '}'
//  68 point: FLOAT FLOAT
//  69 points: point                              70 | points ',' point
//  71 points_opt: /* empty */                    72 | points
//  73 trig_by: TRIG_BY name '{' trig_guard_opt '}'
//  74 trig_guard_opt: /* empty */                75 | TRIG_GUARD CODE
//  76 trig_action: TRIG_ACTION CODE
//  77 dest_state: DEST_STATE IDENTIFIER
//  78 init: INIT
//  79 exitby: EXITBY IDENTIFIER
//  80 enterby: ENTERBY IDENTIFIER IDENTIFIER
//  81 dest_choice_pt: DEST_CHOICE_PT IDENTIFIER IDENTIFIER
//  82 trans_type: TRANS_TYPE IDENTIFIER
//  83 exit_port: EXIT_PORT name INTEGER '{' port_items_opt '}'
//  84 port_items_opt: /* empty */                85 | port_items
//  86 port_items: port_item                      87 | port_items port_item
//  88 port_item: entry_action   89 | display_pos   90 | transition
//  91 entry_port: ENTRY_PORT name INTEGER '{' port_items_opt '}'   92-98 as 84-90
//  99 choice_pt: CHOICE_PT name '{' choice_items_opt '}'
// 100 choice_items_opt: /* empty */             101 | choice_items
// 102 choice_items: choice_item                 103 | choice_items choice_item
// 104 choice_item: display_pos   105 | entry_action   106 | transition
// 107 note_contents: NOTE_CONTENTS CODE
// 108 note: NOTE name '{' note_items_opt '}'
// 109 note_items_opt: /* empty */               110 | note_items
// 111 note_items: note_item                     112 | note_items note_item
// 113 note_item: display_pos   114 | note_contents
// 115 name: IDENTIFIER
#include "MfStateCompiler.h"
#include "MfStateGrammar.h"
#include "MfFiniteStateMachine.h"
#include "MfFSMList.h"
#include "MfFSMNote.h"
#include "MfState.h"
#include "MfStateAction.h"
#include "MfStateChoicePoint.h"
#include "MfStateDisplayPos.h"
#include "MfStateEntryPort.h"
#include "MfStateExitPort.h"
#include "MfStateProperty.h"
#include "MfStateTransition.h"
#include "MfStringList.h"
#include "MfTransitionProperty.h"
#include "Mf2DVectorList.h"

int yylex();

// the compiler being run
static MfStateCompiler *gParseCompiler;

#define YYERRCODE 256
short yylhs[] =
    {
        -1,     0,     4,     1,     1,     2,     2,     3,    26,    26,
        26,    26,    26,    26,     8,     8,     7,     7,    25,    23,
        19,    19,    19,    19,    19,    19,    19,    19,    19,    19,
        19,    19,    19,    19,    19,    19,     5,     5,     6,     6,
        17,    38,    39,    55,    55,    20,    56,    56,    27,    28,
        21,    22,    24,    18,    40,    40,    40,    40,    40,    40,
        40,    40,    40,    50,    50,    51,    51,    44,    52,    53,
        53,    54,    54,    41,    57,    57,    42,    43,    46,    47,
        48,    49,    45,    29,    10,    10,     9,     9,    31,    31,
        31,    30,    12,    12,    11,    11,    32,    32,    32,    34,
        14,    14,    13,    13,    33,    33,    33,    37,    36,    16,
        16,    15,    15,    35,    35,    58
    } ;
short yylen[] =
    {
         2,     1,     1,     0,     1,     1,     2,     5,     1,     1,
         1,     1,     1,     1,     0,     1,     1,     2,     2,     5,
         1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
         1,     1,     1,     1,     1,     1,     1,     2,     0,     1,
         7,     7,     4,     1,     3,     4,     0,     1,     1,     1,
         2,     2,     2,     5,     1,     1,     1,     1,     1,     1,
         1,     1,     1,     1,     2,     0,     1,     6,     2,     1,
         3,     0,     1,     5,     0,     2,     2,     2,     1,     2,
         3,     3,     2,     6,     0,     1,     1,     2,     1,     1,
         1,     6,     0,     1,     1,     2,     1,     1,     1,     5,
         0,     1,     1,     2,     1,     1,     1,     2,     5,     0,
         1,     1,     2,     1,     1,     1
    } ;
short yydefred[] =
    {
         0,     0,     0,     1,     0,     2,     5,   115,     0,     6,
         0,     0,     0,     0,     0,     0,     0,     0,     0,     8,
         9,    16,    11,    10,    12,    13,     0,    18,     0,     0,
         0,     0,    17,     7,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,    48,     0,     0,     0,    49,     0,
         0,    20,    26,    36,    22,    23,    24,    27,    25,    28,
        29,    30,    31,    32,    33,    34,    21,    35,     0,    42,
         0,     0,     0,   113,   111,   114,     0,     0,   104,   106,
       105,   102,     0,    50,    51,     0,     0,     0,     0,    52,
        37,    19,     0,   107,   112,   108,   103,    99,     0,     0,
        43,     0,     0,     0,     0,     0,     0,    45,     0,     0,
         0,     0,    78,     0,     0,     0,     0,    63,    55,    56,
        58,    54,    57,    59,    60,    61,    62,     0,     0,     0,
         0,     0,    41,    44,    77,     0,    76,     0,    79,     0,
         0,    82,    64,    53,     0,     0,     0,    89,    90,    88,
        86,     0,     0,    97,    98,    96,    94,     0,     0,    80,
        81,     0,    87,    83,    95,    91,     0,     0,     0,    40,
        75,    73,     0,    69,     0,     0,    68,     0,    67,    70
    } ;
short yydgoto[] =
    {
         2,     3,     4,     5,     6,    49,    50,    17,    18,   145,
       146,   151,   152,    76,    77,    71,    72,    51,    52,    53,
        54,    55,    56,    19,    58,    20,    21,    60,    61,    62,
        63,   150,   156,    81,    22,    74,    23,    75,    24,    25,
       117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
       127,   128,   173,   174,   175,    98,    99,   167,     8
    } ;
short yysindex[] =
    {
      -278,  -251,     0,     0,  -278,     0,     0,     0,  -252,     0,
      -245,  -251,  -247,  -250,  -248,  -251,  -251,  -245,  -231,     0,
         0,     0,     0,     0,     0,     0,  -225,     0,  -215,  -210,
      -219,  -214,     0,     0,  -217,  -203,  -197,  -276,  -213,  -195,
      -190,  -184,  -251,  -186,     0,  -251,  -251,  -181,     0,  -217,
      -185,     0,     0,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0,  -178,     0,
      -177,  -276,  -183,     0,     0,     0,  -213,  -180,     0,     0,
         0,     0,  -251,     0,     0,  -176,  -175,  -172,  -171,     0,
         0,     0,  -170,     0,     0,     0,     0,     0,  -167,  -174,
         0,  -267,  -169,  -164,  -163,  -162,  -251,     0,  -154,  -251,
      -156,  -153,     0,  -151,  -148,  -146,  -144,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,  -267,  -149,  -145,
      -213,  -213,     0,     0,     0,  -143,     0,  -141,     0,  -137,
      -136,     0,     0,     0,  -135,  -213,  -142,     0,     0,     0,
         0,  -213,  -139,     0,     0,     0,     0,  -147,  -134,     0,
         0,  -138,     0,     0,     0,     0,  -132,  -133,  -128,     0,
         0,     0,  -126,     0,  -131,  -129,     0,  -128,     0,     0
    } ;
short yyrindex[] =
    {
       135,     0,     0,     0,   138,     0,     0,     0,     0,     0,
      -127,     0,     0,     0,     0,     0,     0,  -125,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,  -124,     0,     0,  -123,  -122,     0,
         0,     0,     0,     0,     0,     0,     0,     0,     0,  -121,
         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
         0,  -120,     0,     0,     0,     0,  -119,     0,     0,     0,
         0,     0,  -118,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0,  -117,     0,
         0,  -116,     0,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,  -115,     0,     0,
      -114,  -113,     0,     0,     0,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,  -112,     0,     0,     0,     0,
         0,  -111,     0,     0,     0,     0,     0,  -110,     0,     0,
         0,     0,     0,     0,     0,     0,     0,     0,  -109,     0,
         0,     0,     0,     0,  -108,     0,     0,     0,     0,     0
    } ;
short yygindex[] =
    {
         0,     0,     0,     0,   136,     0,     0,     0,     0,     0,
         0,     0,     0,     0,     0,     0,     0,   -37,   -35,   110,
         0,   -33,     0,   -24,     0,   -23,   143,     0,     0,     0,
         0,    16,    11,    87,   -18,    93,   -12,     0,   -11,    -7,
        38,     0,     0,     0,     0,     0,     0,     0,     0,     0,
         0,     0,   -10,     0,     0,     0,     0,     0,    17
    } ;
#define YYTABLESIZE 167
short yytable[] =
    {
        73,    78,    43,    79,     1,    80,     7,   108,   109,   110,
        57,    59,   111,    10,    27,    28,    64,    29,    70,   112,
       113,   114,    65,    66,    11,    57,    59,    67,    26,   115,
       116,    64,    30,    31,    73,    33,    12,    65,    66,    78,
        34,    79,    67,    80,    13,    35,    37,    14,    15,    36,
        16,    38,    11,    39,    40,    41,    42,    68,    40,    85,
        42,    43,    87,    88,    12,    43,    44,    45,    46,    69,
        82,    83,    13,    47,    48,    14,    15,    84,    16,    86,
        89,    91,    92,    95,    93,   102,    97,   103,   104,   101,
       105,   129,   107,   147,   153,   148,   154,   149,   155,   100,
       106,   130,   131,   134,   132,   136,   138,   137,   147,   139,
       148,   140,   149,   141,   153,   144,   154,   143,   155,   158,
       159,   160,   157,   133,   163,   161,   135,   165,   169,   170,
       166,   168,   172,   171,   176,     3,   177,   178,     4,    14,
         9,    15,    38,   109,   100,    39,   110,   101,    46,    47,
        65,    66,    84,    92,    85,    93,    74,    71,    72,    90,
        32,   162,   164,    96,    94,   142,     0,   179
    } ;
short yycheck[] =
    {
        37,    38,   278,    38,   282,    38,   257,   274,   275,   276,
        34,    34,   279,   265,   261,   265,    34,   265,   294,   286,
       287,   288,    34,    34,   269,    49,    49,    34,    11,   296,
       297,    49,    15,    16,    71,   266,   281,    49,    49,    76,
       265,    76,    49,    76,   289,   260,   265,   292,   293,   259,
       295,   265,   269,   270,   271,   272,   273,   260,   271,    42,
       273,   278,    45,    46,   281,   278,   283,   284,   285,   266,
       265,   261,   289,   290,   291,   292,   293,   261,   295,   265,
       261,   266,   260,   266,   261,   260,   266,   259,   259,   265,
       260,   260,   266,   130,   131,   130,   131,   130,   131,    82,
       267,   265,   265,   257,   266,   261,   257,   260,   145,   257,
       145,   257,   145,   257,   151,   260,   151,   266,   151,   260,
       257,   257,   265,   106,   266,   260,   109,   266,   266,   261,
       277,   265,   260,   266,   260,     0,   267,   266,     0,   266,
         4,   266,   266,   266,   266,   266,   266,   266,   266,   266,
       266,   266,   266,   266,   266,   266,   266,   266,   266,    49,
        17,   145,   151,    76,    71,   127,    -1,   177
    } ;
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 297
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE

// 0x1009cc70 ?yyerror@@YAXPAD@Z
void yyerror(char *message)
{
    gParseCompiler->SyntaxError();
}

// 0x1009cc80 ?SetParseCompiler@@YAXPAVMfStateCompiler@@@Z
void SetParseCompiler(MfStateCompiler *compiler)
{
    gParseCompiler = compiler;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
#define yyerrok (yyerrflag = 0)
#define yyclearin (yychar = (-1))

// 0x1009cc90 ?yyparse@@YAHXZ
int
yyparse()
{
    register int yym, yyn, yystate;

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
yynewerror:
    yyerror("syntax error");
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
{
    gParseCompiler->Finish((MfStatePropertyList *)yyvsp[0].machines);
}
break;
case 2:
{
    yyval.machine = yyvsp[0].machine;
}
break;
case 3:
{
    yyval.machines = new MfFSMList(1);
}
break;
case 4:
{
    yyval.machines = yyvsp[0].machines;
}
break;
case 5:
{
    yyval.machines = new MfFSMList(1);
    yyval.machines->AddItem(yyvsp[0].machine);
}
break;
case 6:
{
    yyval.machines = yyvsp[-1].machines;
    yyval.machines->AddItem(yyvsp[0].machine);
}
break;
case 7:
{
    yyval.machine = new MfFiniteStateMachine(*yyvsp[-3].sval, yyvsp[-1].props, 0);
}
break;
case 8:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 9:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 10:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 11:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 12:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 13:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 14:
{
    yyval.props = new MfStatePropertyList(0);
}
break;
case 15:
{
    yyval.props = yyvsp[0].props;
}
break;
case 16:
{
    yyval.props = new MfStatePropertyList(0);
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 17:
{
    yyval.props = yyvsp[-1].props;
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 18:
{
    yyval.prop = new MfStateFunctionDef(*yyvsp[0].sval);
}
break;
case 19:
{
    yyval.prop = new MfState(*yyvsp[-3].sval, yyvsp[-1].props);
}
break;
case 20:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 21:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 22:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 23:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 24:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 25:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 26:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 27:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 28:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 29:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 30:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 31:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 32:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 33:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 34:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 35:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 36:
{
    yyval.props = new MfStatePropertyList(0);
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 37:
{
    yyval.props = yyvsp[-1].props;
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 38:
{
    yyval.props = new MfStatePropertyList(0);
}
break;
case 39:
{
    yyval.props = yyvsp[0].props;
}
break;
case 40:
{
    yyval.prop = new MfStateDisplayPos(yyvsp[-4].dval, yyvsp[-3].dval, yyvsp[-2].dval, yyvsp[-1].dval);
}
break;
case 41:
{
    yyval.prop = new MfFSMCameraSettings(yyvsp[-4].dval, yyvsp[-3].dval, yyvsp[-2].dval, yyvsp[-1].dval);
}
break;
case 42:
{
    yyval.prop = new MfFSMPlayBack(yyvsp[-1].ival);
}
break;
case 43:
{
    yyval.names = new MfStringList;
    yyval.names->Add(*yyvsp[0].sval);
}
break;
case 44:
{
    yyval.names = yyvsp[-2].names;
    yyval.names->Add(*yyvsp[0].sval);
}
break;
case 45:
{
    yyval.prop = new MfStateVariables(yyvsp[-1].names);
}
break;
case 46:
{
    yyval.names = new MfStringList;
}
break;
case 47:
{
    yyval.names = yyvsp[0].names;
}
break;
case 48:
{
    yyval.prop = new MfStartProperty;
}
break;
case 49:
{
    yyval.prop = new MfStateBeginHistory;
}
break;
case 50:
{
    yyval.prop = new MfStateEntryAction(*yyvsp[0].sval);
}
break;
case 51:
{
    yyval.prop = new MfStateExitAction(*yyvsp[0].sval);
}
break;
case 52:
{
    yyval.prop = new MfStateInitVar(*yyvsp[0].sval);
}
break;
case 53:
{
    yyval.prop = new MfStateTransition(*yyvsp[-3].sval, yyvsp[-1].transProps);
}
break;
case 54:
{
    yyval.trans = yyvsp[0].trans;
}
break;
case 55:
{
    yyval.trans = yyvsp[0].trans;
}
break;
case 56:
{
    yyval.trans = yyvsp[0].trans;
}
break;
case 57:
{
    yyval.trans = yyvsp[0].trans;
}
break;
case 58:
{
    yyval.trans = yyvsp[0].trans;
}
break;
case 59:
{
    yyval.trans = yyvsp[0].trans;
}
break;
case 60:
{
    yyval.trans = yyvsp[0].trans;
}
break;
case 61:
{
    yyval.trans = yyvsp[0].trans;
}
break;
case 62:
{
    yyval.trans = yyvsp[0].trans;
}
break;
case 63:
{
    yyval.transProps = new MfTransPropList(0);
    yyval.transProps->Append(yyvsp[0].trans);
}
break;
case 64:
{
    yyval.transProps = yyvsp[-1].transProps;
    yyval.transProps->Append(yyvsp[0].trans);
}
break;
case 65:
{
    yyval.transProps = new MfTransPropList(0);
}
break;
case 66:
{
    yyval.transProps = yyvsp[0].transProps;
}
break;
case 67:
{
    yyval.trans = new MfVisualConnector(yyvsp[-4].dval, yyvsp[-3].dval, yyvsp[-1].points);
}
break;
case 68:
{
    yyval.point = new Mf2DVector(yyvsp[-1].dval, yyvsp[0].dval);
}
break;
case 69:
{
    yyval.points = new Mf2DVectorList(1);
    yyval.points->Append(yyvsp[0].point);
}
break;
case 70:
{
    yyval.points = yyvsp[-2].points;
    yyval.points->Append(yyvsp[0].point);
}
break;
case 71:
{
    yyval.points = new Mf2DVectorList(1);
}
break;
case 72:
{
    yyval.points = yyvsp[0].points;
}
break;
case 73:
{
    yyval.trans = new MfTransTriggeredBy(*yyvsp[-3].sval, yyvsp[-1].sval);
}
break;
case 74:
{
    yyval.sval = 0;
}
break;
case 75:
{
    yyval.sval = yyvsp[0].sval;
}
break;
case 76:
{
    yyval.trans = new MfTransTrigAction(*yyvsp[0].sval);
}
break;
case 77:
{
    yyval.trans = new MfTransDestState(*yyvsp[0].sval);
}
break;
case 78:
{
    yyval.trans = new MfTransInit;
}
break;
case 79:
{
    yyval.trans = new MfTransExitBy(*yyvsp[0].sval);
}
break;
case 80:
{
    yyval.trans = new MfTransEnterBy(*yyvsp[-1].sval, *yyvsp[0].sval);
}
break;
case 81:
{
    yyval.trans = new MfTransChoicePt(*yyvsp[-1].sval, *yyvsp[0].sval);
}
break;
case 82:
{
    yyval.trans = new MfTransitionType(*yyvsp[0].sval);
}
break;
case 83:
{
    yyval.prop = new MfStateExitPort(*yyvsp[-4].sval, yyvsp[-3].ival, yyvsp[-1].props);
}
break;
case 84:
{
    yyval.props = new MfStatePropertyList(0);
}
break;
case 85:
{
    yyval.props = yyvsp[0].props;
}
break;
case 86:
{
    yyval.props = new MfStatePropertyList(0);
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 87:
{
    yyval.props = yyvsp[-1].props;
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 88:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 89:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 90:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 91:
{
    yyval.prop = new MfStateEntryPort(*yyvsp[-4].sval, yyvsp[-3].ival, yyvsp[-1].props);
}
break;
case 92:
{
    yyval.props = new MfStatePropertyList(0);
}
break;
case 93:
{
    yyval.props = yyvsp[0].props;
}
break;
case 94:
{
    yyval.props = new MfStatePropertyList(0);
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 95:
{
    yyval.props = yyvsp[-1].props;
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 96:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 97:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 98:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 99:
{
    yyval.prop = new MfStateChoicePoint(*yyvsp[-3].sval, yyvsp[-1].props);
}
break;
case 100:
{
    yyval.props = new MfStatePropertyList(0);
}
break;
case 101:
{
    yyval.props = yyvsp[0].props;
}
break;
case 102:
{
    yyval.props = new MfStatePropertyList(0);
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 103:
{
    yyval.props = yyvsp[-1].props;
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 104:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 105:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 106:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 107:
{
    yyval.prop = new MfFSMNoteContents(*yyvsp[0].sval);
}
break;
case 108:
{
    yyval.prop = new MfFSMNote(*yyvsp[-3].sval, yyvsp[-1].props);
}
break;
case 109:
{
    yyval.props = new MfStatePropertyList(0);
}
break;
case 110:
{
    yyval.props = yyvsp[0].props;
}
break;
case 111:
{
    yyval.props = new MfStatePropertyList(0);
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 112:
{
    yyval.props = yyvsp[-1].props;
    yyval.props->AddProperty(yyvsp[0].prop);
}
break;
case 113:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 114:
{
    yyval.prop = yyvsp[0].prop;
}
break;
case 115:
{
    yyval.sval = yyvsp[0].sval;
}
break;
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
