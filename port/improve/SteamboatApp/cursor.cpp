// cursor.cpp - improvement over the original (port/edits/improvements.py hooks it in):
// the cursor shows the highlight only where a click does something.
#include "SteamboatNode.h"
#include "CSBInventory.h"
#include "ViewEventHandler.h"
#include "MBRuntime.h"
#include "MBColor.h"
#include "SBItem.h"
#include "MfAiActorNode.h"
#include "MfHookData.h"
#include "MfProperty.h"

enum { kPlainCursor = 0, kHighlightCursor = 10, kTalkCursor = 11 };

// The cursor for a click that makes Mickey say something (explain a thing, talk to
// somebody): the plain glove with a speech bubble at its fingertip. Added to the
// cursor animation as sequence 11 (the original has 0..10) by UpdateCursorRegpoints.
void PortAddTalkCursor(IMBAnim *cursor)
{
    if (!cursor || cursor->GetNumSequences() != kTalkCursor)
        return;
    long glove = cursor->GetCastID(kPlainCursor, 0);
    IMBSurface *src = 0;
    cursor->GetCastMemberSurface(glove, &src);
    if (!src)
        return;
    IMBScreenManager *screens = 0;
    IMBScreen *screen = 0;
    MBGetScreenManager(&screens);
    if (screens) {
        screens->GetScreenByIndex(0, &screen);
        screens->Release();
    }
    // the bubble, 15 x 12 pixels: its left edge over the glove's thumb, its tail
    // pointing down to the fingertip
    static const char *const kBubble[] = {
        "..XXXXXXXXXXX..",
        ".XWWWWWWWWWWWX.",
        "XWWWWWWWWWWWWWX",
        "XWWWWWWWWWWWWWX",
        "XWWXXWWXXWWXXWX",
        "XWWXXWWXXWWXXWX",
        "XWWWWWWWWWWWWWX",
        "XWWWWWWWWWWWWWX",
        ".XWWWWWWWWWWWX.",
        "..XWXXXXXXXXX..",
        "..XWX..........",
        "..XX...........",
    };
    const int bw = 15, bh = 12;
    long gw = (long)src->GetWidth(), gh = (long)src->GetHeight();
    long bx = gw / 2 + 2, by = 0;               // where the bubble goes
    long gy = bh - 4;                           // the glove moves down under the tail
    long w = bx + bw > gw ? bx + bw : gw, h = gy + gh;
    IMBSurface *dst = 0;
    if (screen) {
        screen->CreateSurface((unsigned short)w, (unsigned short)h, 0x10, &dst);
        screen->Release();
    }
    if (!dst) {
        src->Release();
        return;
    }
    MBColor key = src->GetTransparentColor();
    dst->SetTransparentColor(key);
    dst->ColorFill(key, 0);
    MBRect from(0, 0, gw, gh), to(0, gy, gw, gy + gh);
    src->Blit(dst, &from, &to, 1);
    MBColor black(0, 0, 0, 255), white(255, 255, 255, 255);
    for (int y = 0; y < bh; y++)
        for (int x = 0; x < bw; x++) {
            char c = kBubble[y][x];
            if (c == '.')
                continue;
            MBRect px(bx + x, by + y, bx + x + 1, by + y + 1);
            dst->ColorFill(c == 'X' ? black : white, &px);
        }
    // the hot spot stays at the fingertip: the glove's own point, moved down with it
    MBPoint reg = cursor->GetCastMemberRegistrationPoint(glove);
    reg.y -= gy;
    long cast = cursor->AddCastMember(dst, reg);
    long seq = cursor->AddSequence();
    long frame = cursor->AddFrame(seq);
    cursor->SetCastID(seq, frame, cast);
    dst->Release();
    src->Release();
}

// What a click on the actor makes the player's character do: what GDmickeyWalk.bhvr
// does on a click into the scene (an actor with HC_graspable 1 and no item required,
// HC_cursorType "none", is walked to and picked up; an actor that has HCTalking is
// walked to and talked to). The cursor that says so, or the plain one for nothing.
static long cursor_for(MfAiActor *actor, MfAiActor *player)
{
    if (!actor || actor == player)
        return kPlainCursor;
    MfProperty prop;
    if (actor->GetProperty(MfString("HC_graspable"), prop) && (int)prop == 1) {
        MfProperty type;
        MfString needs;
        if (actor->GetProperty(MfString("HC_cursorType"), type) && type.ConvertToString(needs) && needs == MfString("none"))
            return kHighlightCursor;
    }
    return actor->GetProperty(MfString("HCTalking"), prop) ? kTalkCursor : kPlainCursor;
}

void SteamboatNode::PortHover3D()
{
    static long sShown = kPlainCursor; // the cursor this function set (not a hotspot)
    long want = kPlainCursor;
    MBMouseEvent evt;
    if (!sCursorItem && sMfPrimaryActor && sMotivateEventHandler && mMotivateHotspot && mMotivateHotspot->IsActive() &&
        sInputEventChannelActive && MBGetScreenMouseLocation(evt.mMousePos, &evt.mScreen)) {
        MBComPtr<ISBInventory> gui(MBIID_ISBInventory);
        bool onMenu = gui->IsPointOnMenu(evt.mMousePos);
        if (!onMenu) {
            MfPickData pick;
            MfMouseHookData data;
            CopyMBMouseEventToMfMouseHookData(evt, data);
            want = cursor_for(sMotivateEventHandler->HandleLButtonUp(&data, pick), sMfPrimaryActor);
        }
        if (evt.mScreen)
            evt.mScreen->Release();
        if (onMenu) {
            sShown = kPlainCursor; // the inventory sets the cursor there (CSBInventory::PortCursor)
            return;
        }
    }
    if (!mCursor || sCursorItem)
        return;
    long now = mCursor->GetCurrentSequenceIndex();
    if (want == sShown)
        return;
    // only over the plain cursor or our own: a hotspot's cursor wins
    if (now == kPlainCursor || (sShown != kPlainCursor && now == sShown))
        mCursor->SetCurrentSequence(want);
    sShown = now == kPlainCursor || now == sShown ? want : kPlainCursor;
}

bool SteamboatNode::PortHoldsItem()
{
    return sCursorItem != 0;
}

void CSBInventory::PortCursor(const MBPoint &worldMousePosition)
{
    if (!IsActive() || m_eBarState != visible || SteamboatNode::PortHoldsItem())
        return;
    gui_anims anim = GetAnimFromPoint(worldMousePosition);
    bool live = anim == recyclebin || anim == map || anim == pants;
    if (anim >= item1 && anim <= item10) {
        MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
        SBItem *item = gs->GetItem(m_vecItemsInInventory[anim - item1].strInstanceName);
        MBString space = SteamboatNode::GetSpaceCode();
        live = item && item->IsItemActiveInSpace(space);
    }
    SteamboatNode::GetActiveCursor()->SetCurrentSequence(live ? kHighlightCursor : kPlainCursor);
}
