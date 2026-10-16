#include "SBGridlockNode.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include "CSBGridLockGameBoard.h"

// game state property holding the highest level the player reached
static const char *MAX_LEVEL_PROP_NAME = "GLMaxLevel";
// the board's first tile: top left corner and size
static MBPoint BOARD_ORIGIN(80, 22);
static MBSize BOARD_TILE_SIZE(68, 70);

// Event channel of the level buttons (m_objLeveler).
const long kGridlockLevelChannel = 1122;

// 0x10068810 ??0SBGridlockNode@@QAE@PAVIMBGameEngine@@PAVMfAiSpace@@@Z
SBGridlockNode::SBGridlockNode(IMBGameEngine *engine, MfAiSpace *space)
    : SBActivityNode(engine, space), m_pGameBoard(0), m_objLeveler(kGridlockLevelChannel, 1),
      m_bPlayAsMickey(true)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(kGridlockLevelChannel, this);
    SetBackgroundMusicState(true);
}

// 0x10068950 ??_GSBGridlockNode@@UAEPAXI@Z

// 0x10068990 ??0SBGridlockNode@@QAE@PAVIMBGameEngine@@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
SBGridlockNode::SBGridlockNode(IMBGameEngine *engine, MBString spaceCode)
    : SBActivityNode(engine, spaceCode), m_pGameBoard(0), m_objLeveler(kGridlockLevelChannel, 1),
      m_bPlayAsMickey(true)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(kGridlockLevelChannel, this);
    SetBackgroundMusicState(true);
}

// 0x10068b40 ??1SBGridlockNode@@UAE@XZ
SBGridlockNode::~SBGridlockNode()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(kGridlockLevelChannel, this);
}

// 0x10068c70 ?OnStart@SBGridlockNode@@UAEXXZ
void SBGridlockNode::OnStart()
{
    SetSceneBackgroundMusic("trafficjam");
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    long nMaxLevel = gs->GetProperty(MAX_LEVEL_PROP_NAME, 1);
    m_bPlayAsMickey = gs->GetProperty("HCMickeyCaptive", 0) == 0;
    SteamboatNode::OnStart();
    m_objLeveler.Create(MBPoint3(0, 0, 155), 1);
    m_objLeveler.SetLevelRange(1, nMaxLevel);
    m_objLeveler.SetLevel(nMaxLevel);
    InitGame();
    if (nMaxLevel == 1)
        if (m_bPlayAsMickey)
            StartSound("gl02amc0010");
        else
            StartSound("gl02ami0010");
}

// 0x10068e30 ?OnEnd@SBGridlockNode@@UAEXXZ
void SBGridlockNode::OnEnd()
{
    delete m_pGameBoard;
    m_objLeveler.Destroy();
    SteamboatNode::OnEnd();
}

// 0x10068e70 ?UpdateLevel@SBGridlockNode@@QAEXJ@Z
void SBGridlockNode::UpdateLevel(long nLevelCompleted)
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

// 0x10068f80 ?InitGame@SBGridlockNode@@IAEXXZ
void SBGridlockNode::InitGame()
{
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    m_pGameBoard = new CSBGridLockGameBoard(this);
    if (fileSpec != 0 && m_pGameBoard) {
        MBString prefFile = mSpaceCode + "scene.ini";
        IMBStream *stream = 0;
        fileSpec->SetPath(prefFile.c_str());
        MBOpenFile(fileSpec, 5, &stream);
        if (stream) {
            m_pGameBoard->InitializeGameBoard(MBRect(BOARD_ORIGIN, BOARD_TILE_SIZE), m_objLeveler.GetLevel(), m_bPlayAsMickey, stream);
            if (stream) {
                stream->Release();
                stream = 0;
            }
        }
    }
}

// 0x100691b0 ?ReinitGame@SBGridlockNode@@IAEXXZ
void SBGridlockNode::ReinitGame()
{
    delete m_pGameBoard;
    m_pGameBoard = 0;
    InitGame();
}

// 0x100691f0 ?ListenToEvent@SBGridlockNode@@MAEXJJPAX@Z
void SBGridlockNode::ListenToEvent(long channel, long event, void *data)
{
    SBActivityNode::ListenToEvent(channel, event, data);
    if (channel == kGridlockLevelChannel)
        ReinitGame();
    else if (m_pGameBoard)
        m_pGameBoard->NotifyEvent(channel, event, data);
}

// 0x10069260 ?Idle@SBGridlockNode@@MAEXXZ
void SBGridlockNode::Idle()
{
    if (m_pGameBoard)
        m_pGameBoard->Idle();
    SteamboatNode::Idle();
}

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x10068130 _$E50
// 0x10068150 _$E47
// 0x10068720 _$E49
// 0x10068740 _$E48
// 0x10068770 _$E61
// 0x10068790 _$E60
// 0x100687c0 _$E64
// 0x100687e0 _$E63
// 0x10069360 _$E75
// 0x100693a0 _$E71
// 0x100693c0 _$E74
