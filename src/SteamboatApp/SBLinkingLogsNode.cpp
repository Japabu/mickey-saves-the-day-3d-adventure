#include "SBLinkingLogsNode.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBLinkingLogsGameBoard.h"

// Event channels of the board's tiles and of the level buttons (m_objLeveler).
const long kLinkingLogsTileChannel = 204;
const long kLinkingLogsLevelChannel = 1122;

// Where the board sits on the screen and the size of one tile.
MBPoint BOARD_ORIGIN(60, 0);
MBSize BOARD_TILE_SIZE(60, 60);

// 0x1007a000 ??0SBLinkingLogsNode@@QAE@PAVIMBGameEngine@@PAVMfAiSpace@@@Z
SBLinkingLogsNode::SBLinkingLogsNode(IMBGameEngine *engine, MfAiSpace *space)
    : SBActivityNode(engine, space), m_pGameBoard(0), m_pBGAnim(0),
      m_objLeveler(kLinkingLogsLevelChannel, 1), m_bPlayAsMickey(true)
{
    SetBackgroundMusicState(true);
}

// 0x1007a100 ??0SBLinkingLogsNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBLinkingLogsNode::SBLinkingLogsNode(IMBGameEngine *engine, MBString spaceCode)
    : SBActivityNode(engine, spaceCode), m_pGameBoard(0), m_pBGAnim(0),
      m_objLeveler(kLinkingLogsLevelChannel, 1), m_bPlayAsMickey(true)
{
    SetBackgroundMusicState(true);
}

// 0x1007a310 ??1SBLinkingLogsNode@@UAE@XZ
SBLinkingLogsNode::~SBLinkingLogsNode()
{
}

// EQUIV 0x1007a3a0 ?OnStart@SBLinkingLogsNode@@UAEXXZ: reviewed: the event manager's CreateObject is called in the original and inlined here; the hint sound is picked with a branch there and a select here
void SBLinkingLogsNode::OnStart()
{
    SetSceneBackgroundMusic(MBString("logs"));
    SteamboatNode::OnStart();
    bool bLoaded = AddAnimToWorld(MBString("LLSPLevelBackground"), m_pBGAnim) == 0;
    if (bLoaded) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
        long nMaxLevel = gs->GetProperty(MAX_LEVEL_PROP_NAME, 1);
        m_bPlayAsMickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
        broadcaster->AddListener(kLinkingLogsTileChannel, this);
        broadcaster->AddListener(kLinkingLogsLevelChannel, this);
        m_objLeveler.Create(MBPoint3(0, -20, 5150), 1);
        m_objLeveler.SetLevelRange(1, nMaxLevel);
        m_objLeveler.SetLevel(nMaxLevel);
        InitGame();
        // the first level starts with Mickey's (or, while he is captive, Minnie's) hint
        if (nMaxLevel == 1)
            StartSound(m_bPlayAsMickey ? "ll03xmc0010" : "ll03xmi0010");
    }
}

// 0x1007a6a0 ?OnEnd@SBLinkingLogsNode@@UAEXXZ
void SBLinkingLogsNode::OnEnd()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->RemoveListener(kLinkingLogsTileChannel, this);
    broadcaster->RemoveListener(kLinkingLogsLevelChannel, this);
    m_objLeveler.Destroy();
    delete m_pGameBoard;
    SteamboatNode::OnEnd();
    if (m_pBGAnim) {
        m_pBGAnim->Release();
        m_pBGAnim = 0;
    }
}

// TODO 0x1007a7e0 ?InitGame@SBLinkingLogsNode@@IAEXXZ: same code with ebx and edi swapped and the
// board rectangle built in another order (local order tried)
void SBLinkingLogsNode::InitGame()
{
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    IMBStream *tempStream;
    m_pBGAnim->SetCurrentSequence(m_objLeveler.GetLevel() - 1);
    m_pBGAnim->Play();
    m_pBGAnim->Place(MBPoint3(0, 0, 0));
    if (fileSpec) {
        MBString prefFile = mSpaceCode + "scene.ini";
        MBComPtr<IMBStream> stream;
        fileSpec->SetPath(prefFile.c_str());
        MBOpenFile(fileSpec, 5, &tempStream);
        stream.Attach(tempStream);
        if (stream) {
            m_pGameBoard = new CSBLinkingLogsGameBoard(this);
            m_pGameBoard->InitializeGameBoard(MBRect(BOARD_ORIGIN, BOARD_TILE_SIZE), m_objLeveler.GetLevel(), m_bPlayAsMickey, stream);
        }
    }
}

// 0x1007aa30 ?ListenToEvent@SBLinkingLogsNode@@UAEXJJPAX@Z
void SBLinkingLogsNode::ListenToEvent(long channel, long event, void *data)
{
    SBActivityNode::ListenToEvent(channel, event, data);
    if (channel == kLinkingLogsLevelChannel) {
        // a new level: a new board
        delete m_pGameBoard;
        m_pGameBoard = 0;
        InitGame();
    } else if (m_pGameBoard) {
        m_pGameBoard->NotifyEvent(channel, event, data);
    }
}

// 0x1007aab0 ?Idle@SBLinkingLogsNode@@UAEXXZ
void SBLinkingLogsNode::Idle()
{
    SteamboatNode::Idle();
    if (m_pGameBoard)
        m_pGameBoard->Idle();
}

// 0x1007aae0 ?UpdateLevel@SBLinkingLogsNode@@QAEXJ@Z
void SBLinkingLogsNode::UpdateLevel(long nLevelCompleted)
{
    // finishing the highest open level opens the next one
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    long nMaxLevel = gs->GetProperty(MAX_LEVEL_PROP_NAME, 1);
    if (nMaxLevel < 10) {
        if (nMaxLevel <= nLevelCompleted)
            nMaxLevel++;
        gs->SetProperty(MAX_LEVEL_PROP_NAME, nMaxLevel);
        m_objLeveler.SetLevelRange(1, nMaxLevel);
    }
}

// Template and inline instances this unit also emits:
// 0x10026200 ?CreateObject@?$MBComPtr@VIMBFileSpec@@@@QAEJABUMBGUID@@@Z
// 0x1007a0c0 ??_GSBLinkingLogsNode@@UAEPAXI@Z

// 0x10079930 _$E49
// 0x10079950 _$E46
// 0x10079f20 _$E48
// 0x10079f40 _$E47
// 0x10079f70 _$E67
// 0x10079f90 _$E66
// 0x10079fc0 _$E70
// 0x10079fe0 _$E69
// 0x1007abf0 _$E81
// 0x1007ac30 _$E77
// 0x1007ac50 _$E80
