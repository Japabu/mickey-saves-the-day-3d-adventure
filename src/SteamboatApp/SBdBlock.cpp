#include "SBdBlock.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "MBRuntime.h"
#include "SBdLineGroup.h"
#include "SBMouseEvent.h"
#include "SBDialoguePropertyManager.h"
#include "MBRect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

bool ParseConditionalString(MBString &str);

// EQUIV 0x100457d0 ??0SBdBlock@@QAE@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z: reviewed: the original calls the broadcaster MBComPtr(const MBGUID &) out of line, VC6 inlines CreateObject (Release, MBComCreateInstance, _QueryInterface, Release); the rest is the same. Was: VC6 inlines the broadcaster CreateObject, the original calls it
SBdBlock::SBdBlock(MBString &blockName)
    : mSectionID(0), mNull(MBHashString("null")), mClicked(false), mActive(false), mSoundPlaying(false),
      mCheckConditional(false)
{
    _mRefCnt = 1;
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(700, this);
    mBlockName = blockName;
}

// 0x10045a20 ?QueryInterface@SBdBlock@@UAEJABUMBGUID@@PAPAX@Z
long SBdBlock::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBEventListener)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10045ac0 ?AddRef@SBdBlock@@UAEKXZ
unsigned long SBdBlock::AddRef()
{
    return ++_mRefCnt;
}

// 0x10045ae0 ?Release@SBdBlock@@UAEKXZ
unsigned long SBdBlock::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10045b10 ?GetLineGroup@SBdBlock@@UAEPAVSBdLineGroup@@PAVMfAiActor@@@Z
SBdLineGroup *SBdBlock::GetLineGroup(MfAiActor *mfPrimaryActor)
{
    return 0;
}

// 0x10045b30 ??_GSBdBlock@@UAEPAXI@Z

// 0x10045bc0 ??1SBdBlock@@UAE@XZ
SBdBlock::~SBdBlock()
{
    for (int i = 0; i < mLineGroups.size(); i++)
        delete mLineGroups[i];
    mLineGroups.clear();
    mConditionalVector.clear();
    mTriggerVector.clear();
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(700, this);
}

// TODO 0x10045e60 ?LoadFromPref@SBdBlock@@UAEXV?$MBComPtr@VIMBStream@@@@V?$MBComPtr@VIMBPref@@@@@Z: (91% similar) not matching yet (inline choices of the string temporaries)
void SBdBlock::LoadFromPref(MBComPtr<IMBStream> stream, MBComPtr<IMBPref> pref)
{
    int success = pref->GetString(stream, mBlockName, "conditional", mConditionalString, "");
    if (success) {
        if (mConditionalString.compare(MBString("null")) != 0) {
            mCheckConditional = true;
            ParseConditional();
        }
    } else {
        char s[80];
        sprintf(s, "%s conditional not specified", mBlockName.c_str());
    }
    success = pref->GetString(stream, mBlockName, "section id", mSectionIDstring, "");
    if (success)
        mSectionID = MBHashString(mSectionIDstring.c_str());
    mSectionIDstartValue = pref->GetLong(stream, mBlockName, mSectionIDstring, 0);
    success = pref->GetString(stream, mBlockName, "trigger", mTriggerString, "");
    if (success)
        ParseTrigger();
    MBString lineGroupName;
    int lineGroupNum = 0;
    char lineGroup[80];
    sprintf(lineGroup, "lineGroup%d", lineGroupNum);
    while (pref->GetString(stream, mBlockName, lineGroup, lineGroupName, "")) {
        SBdLineGroup *tmpLineGroup = new SBdLineGroup(lineGroupName);
        tmpLineGroup->LoadFromPref(stream, pref);
        mLineGroups.push_back(tmpLineGroup);
        lineGroupNum++;
        sprintf(lineGroup, "lineGroup%d", lineGroupNum);
    }
}

// TODO 0x100465c0 ?ParseTrigger@SBdBlock@@IAEXXZ: (78% similar) not matching yet (loop shape)
void SBdBlock::ParseTrigger()
{
    int len = mTriggerString.size();
    MBString tmpString("");
    MBString space(" ");
    int start = 0;
    while (start != -1 && start < len) {
        int pos = mTriggerString.find_first_of(space, start);
        if (pos != -1)
            tmpString = mTriggerString.substr(start, pos - start);
        else
            tmpString = mTriggerString.substr(start, len - start);
        mTriggerVector.push_back(MBHashString(tmpString.c_str()));
        if (pos == -1)
            break;
        start = pos + 1;
    }
}

// 0x10046880 ?ParseConditional@SBdBlock@@IAEXXZ
void SBdBlock::ParseConditional()
{
}

// 0x100468a0 ?SBderror@@YAHPAD@Z
int SBderror(char *s)
{
    MBDebugStr("ERROR: error parsing conditional: %s\n", s);
    return 0;
}

// 0x100468d0 _yywrap
extern "C" int __cdecl yywrap(void)
{
    return 1;
}

static const MBString *s__theString;
static unsigned long s__stringPos;
static bool s__theResult;

int SBdparse();

// 0x100468f0 ?ParseConditionalString@@YA_NAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
bool ParseConditionalString(MBString &str)
{
    s__stringPos = 0;
    s__theString = &str;
    SBdparse();
    return s__theResult;
}


// ---- the conditional parser: bison 1.25 (bison.simple, -p SBd) output; tables copied
// from the binary ----
#define yyparse SBdparse
#define yylex SBdlex
#define yyerror SBderror
#define yylval SBdlval
#define yychar SBdchar
#define yynerrs SBdnerrs

union YYSTYPE {
    int num;
    MBString *str;
};

#define NUMBER 258
#define IDENTIFIER 259
#define QSTRING 260
#define END 261
#define AND 262
#define OR 263
#define NOT 264
#define EQ 265
#define NE 266
#define LE 267
#define LT 268
#define GE 269
#define GT 270
#define RANDOM 271
#define TRUE_ 272
#define STRINGPROP 273

extern "C" __declspec(dllimport) void *__cdecl MBmemcpy(void *dst, const void *src, unsigned int n);
extern "C" {
static void __yy_memcpy(char *to, char *from, int count);
}
#define YYFINAL 69
#define YYFLAG -32768
#define YYNTBASE 30
#define YYTRANSLATE(x) ((unsigned)(x) <= 274 ? yytranslate[x] : 34)
#define YYLAST 203
#define YYEMPTY -2
#define YYEOF 0
#define YYACCEPT return(0)
#define YYABORT return(1)
#define YYTERROR 1
#define YYINITDEPTH 200
#define YYMAXDEPTH 10000

static const char yytranslate[] = {
         0,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,     24,      2,      2,
        27,     28,     22,     21,      2,     20,     29,     23,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,     19,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,     26,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      2,      2,      2,      1,      2,      3,      4,
         5,      6,      7,      8,      9,     10,     11,     12,     13,     14,
        15,     16,     17,     18,     25
};

static const short yyr1[] = {
         0,     30,     31,     31,     32,     32,     32,     32,     32,     32,
        32,     32,     32,     32,     32,     32,     32,     32,     32,     32,
        32,     32,     32,     32,     32,     32,     32,     32,     32,     32,
        33,     33,     33,     33
};

static const short yyr2[] = {
         0,      2,      0,      2,      1,      1,      1,      4,      3,      3,
         3,      3,      3,      3,      2,      3,      2,      3,      3,      3,
         3,      3,      3,      3,      3,      3,      3,      3,      3,      3,
         1,      3,      3,      3
};

static const short yydefact[] = {
         2,      0,      4,      5,     30,      1,      0,      0,      6,      0,
         0,      3,      0,      0,     16,      0,     14,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0,      0,      0,     32,
         0,     29,     33,     15,     13,     18,     17,     22,     21,     20,
        19,      9,      8,     10,     11,     12,      0,      0,     24,     23,
        28,     27,     26,     25,     31,      7,      0,      0,      0,      0
};

static const short yydefgoto[] = {
        67,      1,     11,     12
};

static const short yypact[] = {
    -32768,     58, -32768,    -24, -32768, -32768,     66,     -8, -32768,     66,
        66,    134,    182,     -3, -32768,     66, -32768,     84,    122,     66,
        66,     66,     66,     66,     66,     66,     66,     66,     66,     66,
        66,     66,      2,      2,      2,      2,      2,      2,      2, -32768,
       103, -32768, -32768,    152,    134,    167,    167,    -11,    -11,    -11,
       -11,     -6,     -6, -32768, -32768, -32768,    -24,      2,     -1,     -1,
        -1,     -1,     -1,     -1, -32768, -32768,    -13,     21,     30, -32768
};

static const short yypgoto[] = {
    -32768, -32768,     29,    -10
};

static const short yytable[] = {
        18,     23,     24,     25,     26,     13,     56,      4,     38,     27,
        28,     29,     30,     31,     39,     42,     29,     30,     31,     15,
        38,     68,     58,     59,     60,     61,     62,     63,     64,     57,
        69,      0,      0,      0,      0,     14,      0,      0,     16,     17,
         0,      0,      0,      0,     40,      0,      0,     66,     43,     44,
        45,     46,     47,     48,     49,     50,     51,     52,     53,     54,
        55,      2,      3,      4,      5,      0,      0,      6,      0,      2,
         3,      4,      0,      0,      7,      6,      8,      0,      9,      0,
         0,      0,      7,      0,      8,     10,      9,      0,      0,      0,
         0,     19,     20,     10,     21,     22,     23,     24,     25,     26,
         0,      0,      0,      0,     27,     28,     29,     30,     31,      0,
        19,     20,     41,     21,     22,     23,     24,     25,     26,      0,
         0,      0,      0,     27,     28,     29,     30,     31,      0,      0,
         0,     65,     32,     33,     34,     35,     36,     37,      0,      0,
         0,     19,     20,     38,     21,     22,     23,     24,     25,     26,
        42,      0,      0,      0,     27,     28,     29,     30,     31,     19,
         0,      0,     21,     22,     23,     24,     25,     26,      0,      0,
         0,      0,     27,     28,     29,     30,     31,     21,     22,     23,
        24,     25,     26,      0,      0,      0,      0,     27,     28,     29,
        30,     31,     32,     33,     34,     35,     36,     37,      0,      0,
         0,      0,      0,     38
};

static const short yycheck[] = {
        10,     12,     13,     14,     15,     29,      4,      5,     21,     20,
        21,     22,     23,     24,     17,     28,     22,     23,     24,     27,
        21,      0,     32,     33,     34,     35,     36,     37,     38,     27,
         0,     -1,     -1,     -1,     -1,      6,     -1,     -1,      9,     10,
        -1,     -1,     -1,     -1,     15,     -1,     -1,     57,     19,     20,
        21,     22,     23,     24,     25,     26,     27,     28,     29,     30,
        31,      3,      4,      5,      6,     -1,     -1,      9,     -1,      3,
         4,      5,     -1,     -1,     16,      9,     18,     -1,     20,     -1,
        -1,     -1,     16,     -1,     18,     27,     20,     -1,     -1,     -1,
        -1,      7,      8,     27,     10,     11,     12,     13,     14,     15,
        -1,     -1,     -1,     -1,     20,     21,     22,     23,     24,     -1,
         7,      8,     28,     10,     11,     12,     13,     14,     15,     -1,
        -1,     -1,     -1,     20,     21,     22,     23,     24,     -1,     -1,
        -1,     28,     10,     11,     12,     13,     14,     15,     -1,     -1,
        -1,      7,      8,     21,     10,     11,     12,     13,     14,     15,
        28,     -1,     -1,     -1,     20,     21,     22,     23,     24,      7,
        -1,     -1,     10,     11,     12,     13,     14,     15,     -1,     -1,
        -1,     -1,     20,     21,     22,     23,     24,     10,     11,     12,
        13,     14,     15,     -1,     -1,     -1,     -1,     20,     21,     22,
        23,     24,     10,     11,     12,     13,     14,     15,     -1,     -1,
        -1,     -1,     -1,     21
};


int yychar;
YYSTYPE yylval;
int yynerrs;

int yylex();

// 0x10046920 ?SBdparse@@YAHXZ
int yyparse(void)
{
    register int yystate;
    register int yyn;
    register short *yyssp;
    register YYSTYPE *yyvsp;
    int yyerrstatus;
    int yychar1 = 0;

    short yyssa[YYINITDEPTH];
    YYSTYPE yyvsa[YYINITDEPTH];

    short *yyss = yyssa;
    YYSTYPE *yyvs = yyvsa;

    int yystacksize = YYINITDEPTH;

    YYSTYPE yyval;
    int yylen;

    yystate = 0;
    yyerrstatus = 0;
    yynerrs = 0;
    yychar = YYEMPTY;

    yyssp = yyss - 1;
    yyvsp = yyvs;

yynewstate:
    *++yyssp = yystate;

    if (yyssp >= yyss + yystacksize - 1) {
        YYSTYPE *yyvs1 = yyvs;
        short *yyss1 = yyss;
        int size = yyssp - yyss + 1;

        if (yystacksize >= YYMAXDEPTH) {
            yyerror("parser stack overflow");
            return 2;
        }
        yystacksize *= 2;
        if (yystacksize > YYMAXDEPTH)
            yystacksize = YYMAXDEPTH;
        yyss = (short *)_alloca(yystacksize * sizeof(*yyssp));
        __yy_memcpy((char *)yyss, (char *)yyss1, size * sizeof(*yyssp));
        yyvs = (YYSTYPE *)_alloca(yystacksize * sizeof(*yyvsp));
        __yy_memcpy((char *)yyvs, (char *)yyvs1, size * sizeof(*yyvsp));

        yyssp = yyss + size - 1;
        yyvsp = yyvs + size - 1;

        if (yyssp >= yyss + yystacksize - 1)
            YYABORT;
    }

    goto yybackup;
yybackup:
    yyn = yypact[yystate];
    if (yyn == YYFLAG)
        goto yydefault;

    if (yychar == YYEMPTY)
        yychar = yylex();

    if (yychar <= 0) {
        yychar1 = 0;
        yychar = YYEOF;
    } else {
        yychar1 = YYTRANSLATE(yychar);
    }

    yyn += yychar1;
    if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
        goto yydefault;

    yyn = yytable[yyn];
    if (yyn < 0) {
        if (yyn == YYFLAG)
            goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
    } else if (yyn == 0)
        goto yyerrlab;

    if (yyn == YYFINAL)
        YYACCEPT;

    if (yychar != YYEOF)
        yychar = YYEMPTY;

    *++yyvsp = yylval;

    if (yyerrstatus)
        yyerrstatus--;

    yystate = yyn;
    goto yynewstate;

yydefault:
    yyn = yydefact[yystate];
    if (yyn == 0)
        goto yyerrlab;

yyreduce:
    yylen = yyr2[yyn];
    if (yylen > 0)
        yyval = yyvsp[1 - yylen];

    switch (yyn) {
    case 1:
        s__theResult = yyvsp[-1].num != 0;
        YYACCEPT;
        break;
    case 2:
        yyval.num = 1;
        break;
    case 3:
        yyval.num = yyvsp[-1].num && yyvsp[0].num;
        break;
    case 4:
        yyval.num = yyvsp[0].num;
        break;
    case 5:
        yyval.num = SBDialoguePropertyManager::GetInstance()->GetProperty(MBHashString(yyvsp[0].str->c_str()));
        break;
    case 6:
        yyval.num = 1;
        break;
    case 7:
        if (yyvsp[-1].num == 0)
            yyval.num = 0;
        else
            yyval.num = rand() % yyvsp[-1].num + 1;
        break;
    case 8:
        yyval.num = yyvsp[-2].num + yyvsp[0].num;
        break;
    case 9:
        yyval.num = yyvsp[-2].num - yyvsp[0].num;
        break;
    case 10:
        yyval.num = yyvsp[-2].num * yyvsp[0].num;
        break;
    case 11:
        yyval.num = yyvsp[-2].num / yyvsp[0].num;
        break;
    case 12:
        yyval.num = yyvsp[-2].num % yyvsp[0].num;
        break;
    case 13:
        yyval.num = yyvsp[-2].num || yyvsp[0].num;
        break;
    case 14:
        yyval.num = -yyvsp[0].num;
        break;
    case 15:
        yyval.num = yyvsp[-2].num && yyvsp[0].num;
        break;
    case 16:
        yyval.num = !yyvsp[0].num;
        break;
    case 17:
        yyval.num = yyvsp[-2].num == yyvsp[0].num;
        break;
    case 18:
        yyval.num = yyvsp[-2].num != yyvsp[0].num;
        break;
    case 19:
        yyval.num = yyvsp[-2].num <= yyvsp[0].num;
        break;
    case 20:
        yyval.num = yyvsp[-2].num < yyvsp[0].num;
        break;
    case 21:
        yyval.num = yyvsp[-2].num >= yyvsp[0].num;
        break;
    case 22:
        yyval.num = yyvsp[-2].num > yyvsp[0].num;
        break;
    case 23:
        yyval.num = *yyvsp[-2].str == *yyvsp[0].str;
        delete yyvsp[-2].str;
        delete yyvsp[0].str;
        break;
    case 24:
        yyval.num = (bool)yyvsp[-2].str->compare(*yyvsp[0].str); // not operator!=: compare inlined, bool result
        delete yyvsp[-2].str;
        delete yyvsp[0].str;
        break;
    case 25:
        yyval.num = *yyvsp[-2].str <= *yyvsp[0].str;
        delete yyvsp[-2].str;
        delete yyvsp[0].str;
        break;
    case 26:
        yyval.num = *yyvsp[-2].str < *yyvsp[0].str;
        delete yyvsp[-2].str;
        delete yyvsp[0].str;
        break;
    case 27:
        yyval.num = *yyvsp[-2].str >= *yyvsp[0].str;
        delete yyvsp[-2].str;
        delete yyvsp[0].str;
        break;
    case 28:
        yyval.num = *yyvsp[-2].str > *yyvsp[0].str;
        delete yyvsp[-2].str;
        delete yyvsp[0].str;
        break;
    case 29:
        yyval.num = yyvsp[-1].num;
        break;
    case 30:
        yyval.str = yyvsp[0].str;
        break;
    case 31:
        yyval.str = new MBString(*yyvsp[-2].str + *yyvsp[0].str);
        delete yyvsp[-2].str;
        delete yyvsp[0].str;
        break;
    case 32:
        yyval.str = new MBString(SBDialoguePropertyManager::GetInstance()->GetPropertyString(MBHashString(yyvsp[-2].str->c_str())));
        delete yyvsp[-2].str;
        break;
    case 33:
        yyval.str = yyvsp[-1].str;
        break;
    }

    yyvsp -= yylen;
    yyssp -= yylen;

    *++yyvsp = yyval;

    yyn = yyr1[yyn];

    yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
    if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
        yystate = yytable[yystate];
    else
        yystate = yydefgoto[yyn - YYNTBASE];

    goto yynewstate;

yyerrlab:
    if (!yyerrstatus) {
        ++yynerrs;
        yyerror("parse error");
    }

    goto yyerrlab1;
yyerrlab1:
    if (yyerrstatus == 3) {
        if (yychar == YYEOF)
            YYABORT;
        yychar = YYEMPTY;
    }

    yyerrstatus = 3;
    goto yyerrhandle;

yyerrdefault:
yyerrpop:
    if (yyssp == yyss)
        YYABORT;
    yyvsp--;
    yystate = *--yyssp;

yyerrhandle:
    yyn = yypact[yystate];
    if (yyn == YYFLAG)
        goto yyerrdefault;

    yyn += YYTERROR;
    if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
        goto yyerrdefault;

    yyn = yytable[yyn];
    if (yyn < 0) {
        if (yyn == YYFLAG)
            goto yyerrpop;
        yyn = -yyn;
        goto yyreduce;
    } else if (yyn == 0)
        goto yyerrpop;

    if (yyn == YYFINAL)
        YYACCEPT;

    *++yyvsp = yylval;

    yystate = yyn;
    goto yynewstate;
}

extern "C" {
// 0x10047770 __yy_memcpy
// name check strips the C underscore from the PDB's plain static name too (___yy_memcpy vs
// __yy_memcpy), so no claim is accepted
static void __yy_memcpy(char *to, char *from, int count)
{
    register char *f = from;
    register char *t = to;
    register int i = count;

    while (i-- > 0)
        *t++ = *f++;
}
}

// ---- the conditional scanner: flex 2.5 skeleton (%option never-interactive), the tokens
// the parser above reads; tables copied from the binary ----
#include "../MBEngine/yy_buffer_state.h" // flex's buffer, as in the script compiler

#define YY_BUF_SIZE 16384
#define YY_READ_BUF_SIZE 8192
#define YY_END_OF_BUFFER_CHAR 0
#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
#define YY_BUFFER_EOF_PENDING 2
#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2
#define YY_MORE_ADJ 0
#define YY_FATAL_ERROR(msg) yy_fatal_error(msg)
#define YY_NEW_FILE yyrestart(yyin)
#define yytext_ptr yytext
#define YY_SC_TO_UI(c) ((unsigned int)(unsigned char)c)
/* the conditional comes from s__theString */
#define YY_INPUT(buf, result, max_size) \
    { \
        char *where = buf; \
        unsigned long wanted = max_size; \
        unsigned long left = s__theString->length() - s__stringPos; \
        int n = MBmin(wanted, left); \
        MBmemcpy(where, s__theString->c_str() + s__stringPos, n); \
        s__stringPos += n; \
        result = n; \
    }
typedef yy_buffer_state *YY_BUFFER_STATE;
typedef unsigned int yy_size_t;
typedef unsigned char YY_CHAR;
typedef int yy_state_type;

static yy_buffer_state *yy_current_buffer;
static char yy_hold_char;
static int yy_n_chars;
static char *yy_c_buf_p;

static const short int yy_accept[58] = {
         0,      0,      0,     30,     29,      1,     24,     29,     14,     29,
        16,     17,     13,     11,     12,     18,     15,      8,     27,     29,
        25,      9,      9,      9,      9,      9,      9,     29,      1,     19,
         0,     10,      0,     22,      8,     28,     20,     21,     26,      9,
         9,      9,      9,      5,      9,      9,     23,      4,      3,      9,
         9,      9,      7,      2,      9,      9,      6,      0
};

static const int yy_ec[256] = {
         0,      1,      1,      1,      1,      1,      1,      1,      1,      2,
         3,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      2,      4,      5,      1,      1,      6,      7,      1,
         8,      9,     10,     11,      1,     12,     13,     14,     15,     15,
        15,     15,     15,     15,     15,     15,     15,     15,      1,      1,
        16,     17,     18,      1,      1,     19,     19,     19,     19,     19,
        19,     19,     19,     19,     19,     19,     19,     19,     19,     19,
        19,     19,     19,     19,     19,     19,     19,     19,     19,     19,
        19,      1,     20,      1,      1,     21,      1,     22,     19,     19,
        23,     19,     19,     24,     19,     25,     19,     19,     26,     19,
        27,     28,     19,     19,     29,     30,     31,     32,     19,     19,
        19,     19,     19,      1,     33,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1
};

static const int yy_meta[34] = {
         0,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      2,      1,      1,      1,      2,
         1,      2,      2,      2,      2,      2,      2,      2,      2,      2,
         2,      2,      2,      1
};

static const short int yy_base[60] = {
         0,      0,      0,     69,     70,     66,     50,     29,     70,     59,
        70,     70,     70,     70,     70,     70,     70,     50,     18,     47,
        46,      0,     35,      9,     32,     38,     28,     25,     55,     70,
        33,     70,     53,     70,     37,     70,     70,     70,     70,      0,
        28,     19,     22,      0,     20,     17,     70,      0,      0,     19,
        21,     18,      0,      0,     15,     16,      0,     70,     53,     37
};

static const short int yy_def[60] = {
         0,     57,      1,     57,     57,     57,     57,     58,     57,     57,
        57,     57,     57,     57,     57,     57,     57,     57,     57,     57,
        57,     59,     59,     59,     59,     59,     59,     57,     57,     57,
        58,     57,     58,     57,     57,     57,     57,     57,     57,     59,
        59,     59,     59,     59,     59,     59,     57,     59,     59,     59,
        59,     59,     59,     59,     59,     59,     59,      0,     57,     57
};

static const short int yy_nxt[104] = {
         0,      4,      5,      4,      6,      7,      8,      9,     10,     11,
        12,     13,     14,     15,     16,     17,     18,     19,     20,     21,
         4,      4,     22,     21,     21,     21,     21,     23,     24,     25,
        26,     21,     21,     27,     31,     35,     36,     41,     31,     39,
        56,     42,     55,     54,     53,     52,     51,     50,     49,     32,
        48,     47,     34,     32,     30,     30,     57,     28,     46,     45,
        44,     43,     40,     38,     37,     34,     33,     29,     28,     57,
         3,     57,     57,     57,     57,     57,     57,     57,     57,     57,
        57,     57,     57,     57,     57,     57,     57,     57,     57,     57,
        57,     57,     57,     57,     57,     57,     57,     57,     57,     57,
        57,     57,     57,     57
};

static const short int yy_chk[104] = {
         0,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      1,      1,      1,      1,      1,      1,
         1,      1,      1,      1,      7,     18,     18,     23,     30,     59,
        55,     23,     54,     51,     50,     49,     45,     44,     42,      7,
        41,     40,     34,     30,     58,     58,     32,     28,     27,     26,
        25,     24,     22,     20,     19,     17,      9,      6,      5,      3,
        57,     57,     57,     57,     57,     57,     57,     57,     57,     57,
        57,     57,     57,     57,     57,     57,     57,     57,     57,     57,
        57,     57,     57,     57,     57,     57,     57,     57,     57,     57,
        57,     57,     57,     57
};


#define YY_END_OF_BUFFER 30
#define INITIAL 0
#define YY_START ((yy_start - 1) / 2)
#define YY_STATE_EOF(state) (YY_END_OF_BUFFER + state + 1)
#define YY_BREAK break;
#define ECHO (void)fwrite(yytext, yyleng, 1, yyout)
#define YY_DO_BEFORE_ACTION \
    yytext_ptr = yy_bp; \
    yyleng = (int)(yy_cp - yy_bp); \
    yy_hold_char = *yy_cp; \
    *yy_cp = '\0'; \
    yy_c_buf_p = yy_cp;

FILE *yyin = (FILE *)0, *yyout = (FILE *)0;
char *yytext;
int yyleng;
static int yy_init = 1; /* whether we need to initialize */
// the scanner's file-scope helpers have undecorated names (C linkage, as yy_fatal_error)
extern "C" {
static int yy_get_next_buffer();
static yy_state_type yy_get_previous_state();
static yy_state_type yy_try_NUL_trans(yy_state_type current_state);
}

static yy_state_type yy_last_accepting_state;
static char *yy_last_accepting_cpos;
static int yy_start;

static int yy_did_buffer_switch_on_eof;

extern "C" void __cdecl yy_fatal_error(const char *msg);
extern "C" void *__cdecl yy_flex_alloc(unsigned int size);
extern "C" void *__cdecl yy_flex_realloc(void *ptr, unsigned int size);
extern "C" void __cdecl yy_flex_free(void *ptr);
YY_BUFFER_STATE yy_create_buffer(FILE *file, int size);
void yy_init_buffer(YY_BUFFER_STATE b, FILE *file);
void yy_load_buffer_state(void);
void yy_flush_buffer(YY_BUFFER_STATE b);
void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer);
YY_BUFFER_STATE yy_scan_bytes(const char *bytes, int len);
void yyrestart(FILE *input_file);

// 0x100477b0 ?SBdlex@@YAHXZ
int yylex()
{
    register yy_state_type yy_current_state;
    register char *yy_cp, *yy_bp;
    register int yy_act;

    if (yy_init) {
        yy_init = 0;

        if (!yy_start)
            yy_start = 1; /* first start state */

        if (!yyin)
            yyin = stdin;

        if (!yyout)
            yyout = stdout;

        if (!yy_current_buffer)
            yy_current_buffer = yy_create_buffer(yyin, YY_BUF_SIZE);

        yy_load_buffer_state();
    }

    while (1) { /* loops until end-of-file is reached */
        yy_cp = yy_c_buf_p;

        /* Support of yytext. */
        *yy_cp = yy_hold_char;

        /* yy_bp points to the position in yy_ch_buf of the start of
         * the current run.
         */
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
                if (yy_current_state >= 58)
                    yy_c = yy_meta[(unsigned int)yy_c];
            }
            yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int)yy_c];
            ++yy_cp;
        } while (yy_base[yy_current_state] != 70);

    yy_find_action:
        yy_act = yy_accept[yy_current_state];
        if (yy_act == 0) { /* have to back up */
            yy_cp = yy_last_accepting_cpos;
            yy_current_state = yy_last_accepting_state;
            yy_act = yy_accept[yy_current_state];
        }

        YY_DO_BEFORE_ACTION;

    do_action: /* This label is used only to access EOF actions. */

        switch (yy_act) { /* beginning of action switch */
        case 0: /* must back up */
            /* undo the effects of YY_DO_BEFORE_ACTION */
            *yy_cp = yy_hold_char;
            yy_cp = yy_last_accepting_cpos;
            yy_current_state = yy_last_accepting_state;
            goto yy_find_action;

        case YY_STATE_EOF(INITIAL): /* <<EOF>> is the first rule of the scanner */
            return END;
            YY_BREAK
        case 1: /* white space */
            YY_BREAK
        case 2:
            return RANDOM;
            YY_BREAK
        case 3:
            return NOT;
            YY_BREAK
        case 4:
            return AND;
            YY_BREAK
        case 5:
            return OR;
            YY_BREAK
        case 6:
            return TRUE_;
            YY_BREAK
        case 7:
            return STRINGPROP;
            YY_BREAK
        case 8:
            yylval.num = atoi(yytext);
            return NUMBER;
            YY_BREAK
        case 9:
            yylval.str = new MBString(yytext);
            return IDENTIFIER;
            YY_BREAK
        case 10: {
            char *end = strchr(yytext + 1, '"');
            *end = '\0';
            yylval.str = new MBString(yytext + 1);
            *end = '"';
            return QSTRING;
        }
            YY_BREAK
        case 11:
            return '+';
            YY_BREAK
        case 12:
            return '-';
            YY_BREAK
        case 13:
            return '*';
            YY_BREAK
        case 14:
            return '%';
            YY_BREAK
        case 15:
            return '/';
            YY_BREAK
        case 16:
            return '(';
            YY_BREAK
        case 17:
            return ')';
            YY_BREAK
        case 18:
            return '.';
            YY_BREAK
        case 19:
            return EQ;
            YY_BREAK
        case 20:
            return EQ;
            YY_BREAK
        case 21:
            return NE;
            YY_BREAK
        case 22:
            return AND;
            YY_BREAK
        case 23:
            return OR;
            YY_BREAK
        case 24:
            return NOT;
            YY_BREAK
        case 25:
            return LE;
            YY_BREAK
        case 26:
            return LT;
            YY_BREAK
        case 27:
            return GE;
            YY_BREAK
        case 28:
            return GT;
            YY_BREAK
        case 29:
            ECHO;
            YY_BREAK

        case YY_END_OF_BUFFER: {
            /* Amount of text matched not including the EOB char. */
            int yy_amount_of_matched_text = (int)(yy_cp - yytext_ptr) - 1;

            /* Undo the effects of YY_DO_BEFORE_ACTION. */
            *yy_cp = yy_hold_char;

            if (yy_current_buffer->yy_buffer_status == YY_BUFFER_NEW) {
                yy_n_chars = yy_current_buffer->yy_n_chars;
                yy_current_buffer->yy_input_file = yyin;
                yy_current_buffer->yy_buffer_status = YY_BUFFER_NORMAL;
            }

            if (yy_c_buf_p <= &yy_current_buffer->yy_ch_buf[yy_n_chars]) { /* This was really a NUL. */
                yy_state_type yy_next_state;

                yy_c_buf_p = yytext_ptr + yy_amount_of_matched_text;

                yy_current_state = yy_get_previous_state();

                yy_next_state = yy_try_NUL_trans(yy_current_state);

                yy_bp = yytext_ptr + YY_MORE_ADJ;

                if (yy_next_state) {
                    /* Consume the NUL. */
                    yy_cp = ++yy_c_buf_p;
                    yy_current_state = yy_next_state;
                    goto yy_match;
                }

                else {
                    yy_cp = yy_c_buf_p;
                    goto yy_find_action;
                }
            }

            else
                switch (yy_get_next_buffer()) {
                case EOB_ACT_END_OF_FILE: {
                    yy_did_buffer_switch_on_eof = 0;

                    if (yywrap()) {
                        yy_c_buf_p = yytext_ptr + YY_MORE_ADJ;

                        yy_act = YY_STATE_EOF(YY_START);
                        goto do_action;
                    }

                    else {
                        if (!yy_did_buffer_switch_on_eof)
                            YY_NEW_FILE;
                    }
                    break;
                }

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
        } /* end of action switch */
    } /* end of scanning one token */
} /* end of yylex */

// 0x100481e0 yy_get_next_buffer
extern "C" static int yy_get_next_buffer()
{
    register char *dest = yy_current_buffer->yy_ch_buf;
    register char *source = yytext_ptr;
    register int number_to_move, i;
    int ret_val;

    if (yy_c_buf_p > &yy_current_buffer->yy_ch_buf[yy_n_chars + 1])
        YY_FATAL_ERROR("fatal flex scanner internal error--end of buffer missed");

    if (yy_current_buffer->yy_fill_buffer == 0) { /* Don't try to fill the buffer, so this is an EOF. */
        if (yy_c_buf_p - yytext_ptr - YY_MORE_ADJ == 1) {
            /* We matched a single character, the EOB, so
             * treat this as a final EOF.
             */
            return EOB_ACT_END_OF_FILE;
        } else {
            /* We matched some text prior to the EOB, first
             * process it.
             */
            return EOB_ACT_LAST_MATCH;
        }
    }

    /* Try to read more data. */

    /* First move last chars to start of buffer. */
    number_to_move = (int)(yy_c_buf_p - yytext_ptr) - 1;

    for (i = 0; i < number_to_move; ++i)
        *(dest++) = *(source++);

    if (yy_current_buffer->yy_buffer_status == YY_BUFFER_EOF_PENDING)
        /* don't do the read, it's not guaranteed to return an EOF,
         * just force an EOF
         */
        yy_n_chars = 0;

    else {
        int num_to_read = yy_current_buffer->yy_buf_size - number_to_move - 1;

        while (num_to_read <= 0) { /* Not enough room in the buffer - grow it. */
            /* just a shorter name for the current buffer */
            YY_BUFFER_STATE b = yy_current_buffer;

            int yy_c_buf_p_offset = (int)(yy_c_buf_p - b->yy_ch_buf);

            if (b->yy_is_our_buffer) {
                int new_size = b->yy_buf_size * 2;

                if (new_size <= 0)
                    b->yy_buf_size += b->yy_buf_size / 8;
                else
                    b->yy_buf_size *= 2;

                b->yy_ch_buf = (char *)
                    /* Include room in for 2 EOB chars. */
                    yy_flex_realloc((void *)b->yy_ch_buf, b->yy_buf_size + 2);
            } else
                /* Can't grow it, we don't own it. */
                b->yy_ch_buf = 0;

            if (!b->yy_ch_buf)
                YY_FATAL_ERROR("fatal error - scanner input buffer overflow");

            yy_c_buf_p = &b->yy_ch_buf[yy_c_buf_p_offset];

            num_to_read = yy_current_buffer->yy_buf_size - number_to_move - 1;
        }

        if (num_to_read > YY_READ_BUF_SIZE)
            num_to_read = YY_READ_BUF_SIZE;

        /* Read in more data. */
        YY_INPUT((&yy_current_buffer->yy_ch_buf[number_to_move]), yy_n_chars, num_to_read);
    }

    if (yy_n_chars == 0) {
        if (number_to_move == YY_MORE_ADJ) {
            ret_val = EOB_ACT_END_OF_FILE;
            yyrestart(yyin);
        }

        else {
            ret_val = EOB_ACT_LAST_MATCH;
            yy_current_buffer->yy_buffer_status = YY_BUFFER_EOF_PENDING;
        }
    }

    else
        ret_val = EOB_ACT_CONTINUE_SCAN;

    yy_n_chars += number_to_move;
    yy_current_buffer->yy_ch_buf[yy_n_chars] = YY_END_OF_BUFFER_CHAR;
    yy_current_buffer->yy_ch_buf[yy_n_chars + 1] = YY_END_OF_BUFFER_CHAR;

    yytext_ptr = &yy_current_buffer->yy_ch_buf[0];

    return ret_val;
}

// 0x10048430 yy_get_previous_state
/* yy_get_previous_state - get the state just before the EOB char was reached */
extern "C" static yy_state_type yy_get_previous_state()
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
            if (yy_current_state >= 58)
                yy_c = yy_meta[(unsigned int)yy_c];
        }
        yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int)yy_c];
    }

    return yy_current_state;
}

// 0x10048550 yy_try_NUL_trans
/* yy_try_NUL_trans - try to make a transition on the NUL character
 *
 * synopsis
 *	next_state = yy_try_NUL_trans( current_state );
 */
extern "C" static yy_state_type yy_try_NUL_trans(yy_state_type yy_current_state)
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
        if (yy_current_state >= 58)
            yy_c = yy_meta[(unsigned int)yy_c];
    }
    yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int)yy_c];
    yy_is_jam = (yy_current_state == 57);

    return yy_is_jam ? 0 : yy_current_state;
}


// 0x10048630 ?yyrestart@@YAXPAU_iobuf@@@Z
void yyrestart(FILE *input_file)
{
    if (!yy_current_buffer)
        yy_current_buffer = yy_create_buffer(yyin, YY_BUF_SIZE);

    yy_init_buffer(yy_current_buffer, input_file);
    yy_load_buffer_state();
}

// 0x10048680 ?yy_switch_to_buffer@@YAXPAUyy_buffer_state@@@Z
void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer)
{
    if (yy_current_buffer == new_buffer)
        return;

    if (yy_current_buffer) {
        /* Flush out information for old buffer. */
        *yy_c_buf_p = yy_hold_char;
        yy_current_buffer->yy_buf_pos = yy_c_buf_p;
        yy_current_buffer->yy_n_chars = yy_n_chars;
    }

    yy_current_buffer = new_buffer;
    yy_load_buffer_state();

    /* We don't actually know whether we did this switch during
     * EOF (yywrap()) processing, but the only time this flag
     * is looked at is after yywrap() is called, so it's safe
     * to go ahead and always set it.
     */
    yy_did_buffer_switch_on_eof = 1;
}

// 0x100486f0 ?yy_load_buffer_state@@YAXXZ
void __cdecl yy_load_buffer_state()
{
    yy_n_chars = yy_current_buffer->yy_n_chars;
    yy_c_buf_p = yy_current_buffer->yy_buf_pos;
    yytext = yy_c_buf_p;
    yyin = yy_current_buffer->yy_input_file;
    yy_hold_char = *yy_c_buf_p;
}

// 0x10048730 ?yy_create_buffer@@YAPAUyy_buffer_state@@PAU_iobuf@@H@Z
YY_BUFFER_STATE yy_create_buffer(FILE *file, int size)
{
    YY_BUFFER_STATE b;

    b = (YY_BUFFER_STATE)yy_flex_alloc(sizeof(struct yy_buffer_state));
    if (!b)
        YY_FATAL_ERROR("out of dynamic memory in yy_create_buffer()");

    b->yy_buf_size = size;

    /* yy_ch_buf has to be 2 characters longer than the size given because
     * we need to put in 2 end-of-buffer characters.
     */
    b->yy_ch_buf = (char *)yy_flex_alloc(b->yy_buf_size + 2);
    if (!b->yy_ch_buf)
        YY_FATAL_ERROR("out of dynamic memory in yy_create_buffer()");

    b->yy_is_our_buffer = 1;

    yy_init_buffer(b, file);

    return b;
}

// 0x100487b0 ?yy_delete_buffer@@YAXPAUyy_buffer_state@@@Z
void yy_delete_buffer(YY_BUFFER_STATE b)
{
    if (!b)
        return;

    if (b == yy_current_buffer)
        yy_current_buffer = (YY_BUFFER_STATE)0;

    if (b->yy_is_our_buffer)
        yy_flex_free((void *)b->yy_ch_buf);

    yy_flex_free((void *)b);
}

// 0x10048800 ?yy_init_buffer@@YAXPAUyy_buffer_state@@PAU_iobuf@@@Z
void yy_init_buffer(YY_BUFFER_STATE b, FILE *file)
{
    yy_flush_buffer(b);

    b->yy_input_file = file;
    b->yy_fill_buffer = 1;
    b->yy_is_interactive = 0;
}

// 0x10048840 ?yy_flush_buffer@@YAXPAUyy_buffer_state@@@Z
void __cdecl yy_flush_buffer(yy_buffer_state *b)
{
    b->yy_n_chars = 0;
    b->yy_ch_buf[0] = 0;
    b->yy_ch_buf[1] = 0;
    b->yy_buf_pos = b->yy_ch_buf;
    b->yy_at_bol = 1;
    b->yy_buffer_status = 0;
    if (b == yy_current_buffer)
        yy_load_buffer_state();
}

// 0x10048880 ?yy_scan_buffer@@YAPAUyy_buffer_state@@PADI@Z
YY_BUFFER_STATE yy_scan_buffer(char *base, yy_size_t size)
{
    YY_BUFFER_STATE b;

    if (size < 2 ||
        base[size - 2] != YY_END_OF_BUFFER_CHAR ||
        base[size - 1] != YY_END_OF_BUFFER_CHAR)
        /* They forgot to leave room for the EOB's. */
        return 0;

    b = (YY_BUFFER_STATE)yy_flex_alloc(sizeof(struct yy_buffer_state));
    if (!b)
        YY_FATAL_ERROR("out of dynamic memory in yy_scan_buffer()");

    b->yy_buf_size = size - 2; /* "- 2" to take care of EOB's */
    b->yy_buf_pos = b->yy_ch_buf = base;
    b->yy_is_our_buffer = 0;
    b->yy_input_file = 0;
    b->yy_n_chars = b->yy_buf_size;
    b->yy_is_interactive = 0;
    b->yy_at_bol = 1;
    b->yy_fill_buffer = 0;
    b->yy_buffer_status = YY_BUFFER_NEW;

    yy_switch_to_buffer(b);

    return b;
}

// 0x10048920 ?yy_scan_string@@YAPAUyy_buffer_state@@PBD@Z
YY_BUFFER_STATE yy_scan_string(const char *yy_str)
{
    int len;
    for (len = 0; yy_str[len]; ++len)
        ;

    return yy_scan_bytes(yy_str, len);
}

// 0x10048950 ?yy_scan_bytes@@YAPAUyy_buffer_state@@PBDH@Z
YY_BUFFER_STATE yy_scan_bytes(const char *bytes, int len)
{
    YY_BUFFER_STATE b;
    char *buf;
    yy_size_t n;
    int i;

    /* Get memory for full buffer, including space for trailing EOB's. */
    n = len + 2;
    buf = (char *)yy_flex_alloc(n);
    if (!buf)
        YY_FATAL_ERROR("out of dynamic memory in yy_scan_bytes()");

    for (i = 0; i < len; ++i)
        buf[i] = bytes[i];

    buf[len] = buf[len + 1] = YY_END_OF_BUFFER_CHAR;

    b = yy_scan_buffer(buf, n);
    if (!b)
        YY_FATAL_ERROR("bad buffer in yy_scan_bytes()");

    /* It's okay to grow etc. this buffer, and we should throw it
     * away when we're done.
     */
    b->yy_is_our_buffer = 1;

    return b;
}


// 0x100489f0 yy_fatal_error
extern "C" void __cdecl yy_fatal_error(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(2);
}

// 0x10048a20 yy_flex_alloc
extern "C" void *__cdecl yy_flex_alloc(unsigned int size)
{
    return malloc(size);
}

// 0x10048a40 yy_flex_realloc
extern "C" void *__cdecl yy_flex_realloc(void *ptr, unsigned int size)
{
    return realloc(ptr, size);
}

// 0x10048a70 yy_flex_free
extern "C" void __cdecl yy_flex_free(void *ptr)
{
    free(ptr);
}

// 0x10048a90 ?EvaluateConditional@SBdBlock@@IAE_NXZ
bool SBdBlock::EvaluateConditional()
{
    return ParseConditionalString(mConditionalString);
}

// 0x10048ab0 ?EvaluateBlock@SBdBlock@@UAE_NXZ
bool SBdBlock::EvaluateBlock()
{
    if (!mClicked)
        return false;
    mClicked = false;
    if (EvaluateConditional())
        return true;
    return false;
}

// EQUIV 0x10048ae0 ?ListenToEvent@SBdBlock@@UAEXJJPAX@Z: reviewed: the original inlines _Eos and the string _Tidy(true) of the case 712 message, VC6 calls both and duplicates the shared return tail; same loops, compares and calls. Was: the original inlines _Eos in the char* string construction, VC6 calls it
void SBdBlock::ListenToEvent(long channel, long event, void *data)
{
    if (channel == 700) {
        switch (event) {
        case 712: {
            MBString message((char *)data);
            for (MBVector<unsigned long>::iterator it = mTriggerVector.begin(); it != mTriggerVector.end(); it++) {
                if (MBHashString(message.c_str()) == *it) {
                    mClicked = true;
                    break;
                }
            }
            break;
        }
        case 710: {
            MBString message = ((SBMouseEvent *)data)->data;
            for (MBVector<unsigned long>::iterator it = mTriggerVector.begin(); it != mTriggerVector.end(); it++) {
                if (MBHashString(message.c_str()) == *it) {
                    mClicked = true;
                    break;
                }
            }
            break;
        }
        }
    }
}

// Matched by this code (template and inline instances):
// 0x100459d0 ??1?$MBVector@K@@QAE@XZ
// 0x10045b70 ??1?$MBVector@PAVSBdLineGroup@@@@QAE@XZ
// 0x10048f00 ??1?$vector@PAVSBdLineGroup@@V?$MBAllocator@PAVSBdLineGroup@@@@@std@@QAE@XZ
// 0x10049010 ?_Destroy@?$vector@KV?$MBAllocator@K@@@std@@IAEXPAK0@Z
// 0x10049070 ?insert@?$vector@KV?$MBAllocator@K@@@std@@QAEXPAKIABK@Z
// 0x10049310 ?insert@?$vector@PAVSBdLineGroup@@V?$MBAllocator@PAVSBdLineGroup@@@@@std@@QAEXPAPAVSBdLineGroup@@IABQAV3@@Z

// Not claimed (static initializers and header instances of this object):
// 0x10048fd0 ?erase@?$vector@KV?$MBAllocator@K@@@std@@QAEPAKPAK0@Z
// 0x10049030 ?erase@?$vector@PAVSBdLineGroup@@V?$MBAllocator@PAVSBdLineGroup@@@@@std@@QAEPAPAVSBdLineGroup@@PAPAV3@0@Z
// 0x100495b0 _$E31
// 0x100495f0 _$E27
// 0x10049610 _$E30
