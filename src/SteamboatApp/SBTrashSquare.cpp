#include "SBTrashSquare.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBTrashTile.h"
#include "SteamboatNode.h"

// 0x1009fa60 ??0SBTrashSquare@@QAE@XZ
SBTrashSquare::SBTrashSquare()
{
}

// 0x1009fab0 ?IsOpen@SBTrashSquare@@UAE_NXZ
bool SBTrashSquare::IsOpen()
{
    return mTiles.size() < mMaxLayers;
}

// 0x1009fad0 ?IsEmpty@SBTrashSquare@@UAE_NXZ
bool SBTrashSquare::IsEmpty()
{
    return mTiles.size() == 0;
}

// 0x1009fb30 ??0SBTrashSquare@@QAE@ABVMBPoint3@@ABVMBPoint@@J@Z
SBTrashSquare::SBTrashSquare(const MBPoint3 &pos, const MBPoint &coords, const long maxLayers)
    : mPosition(pos), mCoords(coords), mMaxLayers(maxLayers)
{
}

// 0x1009fbb0 ??1SBTrashSquare@@UAE@XZ
SBTrashSquare::~SBTrashSquare()
{
    MBList<CSBTrashTile *>::iterator it = mTiles.begin();
    while (it != mTiles.end()) {
        CSBTrashTile *tile = *it;
        SteamboatNode::RemoveFromWorld((ISBAnimButton *)tile);
        tile->Release();
        it = mTiles.erase(it);
    }
}

// 0x1009fd40 ?Init@SBTrashSquare@@UAEXABVMBPoint3@@ABVMBPoint@@J@Z
void SBTrashSquare::Init(const MBPoint3 &pos, const MBPoint &coords, const long maxLayers)
{
    mMaxLayers = maxLayers;
    mPosition = pos;
    mCoords = coords;
}

// 0x1009fd90 ?PushTile@SBTrashSquare@@UAE_NPAVCSBTrashTile@@@Z
bool SBTrashSquare::PushTile(CSBTrashTile *tile)
{
    if (!IsOpen())
        return false;
    // only the top tile of the stack can be clicked
    if (mTiles.size() > 0)
        mTiles.front()->Disable();
    mTiles.push_front(tile);
    tile->Place(GetPosition(0));
    tile->Enable();
    tile->SetBroadcastChannel(400);
    tile->SetClickMessage(401, this);
    return true;
}

// 0x1009fe80 ?PopTile@SBTrashSquare@@UAEPAVCSBTrashTile@@XZ
CSBTrashTile *SBTrashSquare::PopTile()
{
    CSBTrashTile *tile = 0;
    if (mTiles.size() > 0) {
        tile = mTiles.front();
        mTiles.pop_front();
    }
    return tile;
}

// 0x1009fed0 ?PeekTopTile@SBTrashSquare@@UBEPAVCSBTrashTile@@XZ
CSBTrashTile *SBTrashSquare::PeekTopTile() const
{
    if (mTiles.size())
        return mTiles.front();
    return 0;
}

// TODO 0x1009ff00 ?GetPosition@SBTrashSquare@@UAE?AVMBPoint3@@H@Z: same arithmetic, the original walks mPosition through a pointer and keeps more registers (constructor and operator+ forms tried) (58% similar)
MBPoint3 SBTrashSquare::GetPosition(int nextTile)
{
    // each layer sits 8 pixels higher and in front of the one below
    long layer = mTiles.size() + nextTile;
    return mPosition + MBPoint3(0, 8 - layer * 8, layer + mPosition.y * 10);
}

// 0x1009ff70 ?Enable@SBTrashSquare@@UAEXXZ
void SBTrashSquare::Enable()
{
    if (mTiles.size() > 0)
        mTiles.front()->Enable();
}

// Template and inline instances this unit also emits:
// 0x1009faf0 ??_GSBTrashSquare@@UAEPAXI@Z
// 0x1009fcc0 ??1?$MBList@PAVCSBTrashTile@@@@QAE@XZ

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x1009f420 _$E18
// 0x1009f440 _$E15
// 0x1009fa10 _$E17
// 0x1009fa30 _$E16
// 0x1009ffa0 _$E61
// 0x1009ffe0 _$E57
// 0x100a0000 _$E60
