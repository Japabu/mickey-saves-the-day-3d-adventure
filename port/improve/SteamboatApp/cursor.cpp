// cursor.cpp - improvement over the original (port/edits/improvements.py hooks it in):
// the cursor shows the highlight only where a click does something, and a held item
// sparkles over what a click with it would be used on.
#include "SteamboatNode.h"
#include "CSBInventory.h"
#include "ViewEventHandler.h"
#include "MBRuntime.h"
#include "MBColor.h"
#include "SBItem.h"
#include "MfAiActorNode.h"
#include "MfHookData.h"
#include "MfProperty.h"
#include "MfAiColNode.h"
#include "MfColResults.h"
#include "SBMotivateDriver.h"
#include <ctype.h>
#include <string.h>

enum { kPlainCursor = 0, kHighlightCursor = 10, kTalkCursor = 11 };

// ---- The cursor's tip is one pixel; the glove under it reaches right and down. A click
// into the 3D scene (and the cursor over it) also finds what it can be used on a little
// right of and below the tip, nearest first (game pixels of the 640x480 screen), when
// the tip itself hits nothing such: thin things like the fishing pole. Without one there
// the tip's hit stands, so a walk goes exactly where it points. The scripts' own picking
// does the same (port/improve/RuntimeLib/pick.cpp).

// One of the game's items (steamboat.ini) in this scene: its script takes a click (the
// bait bucket gives a worm), though it has none of the marks below.
static bool port_scene_item(MfAiActor *actor)
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    SBItem *item = gs ? gs->GetItem(MBString((const char *)actor->GetInstanceName())) : 0;
    MBString space = SteamboatNode::GetSpaceCode();
    return item && item->IsItemActiveInSpace(space);
}

// What a click can be used on: an actor marked so for the scripts (picked up, talked to,
// explaining itself) or an item of the scene, not the player.
static bool port_interactive(MfAiActor *actor)
{
    if (!actor)
        return false;
    const char *n = (const char *)actor->GetClassNameA();
    if (n && (!strncasecmp(n, "GDmickey", 8) || !strncasecmp(n, "GDminnie", 8)))
        return false;
    MfProperty prop;
    return actor->GetProperty(MfString("HC_graspable"), prop) || actor->GetProperty(MfString("HCTalking"), prop) ||
           actor->GetProperty(MfString("HC_cursorType"), prop) || port_scene_item(actor);
}

static MfAiActor *port_cast(ViewEventHandler *handler, float x, float y, MfPickData &pick)
{
    MfAiSpace *space = SBMotivateDriver::GetMotivateSpace();
    MfColResultRay hit;
    MfVector3 position, direction;
    if (!space || !handler->ConstructRay(x, y, position, direction) || !space->CastRay(position, direction, hit, 0, 1) ||
        !hit.node)
        return 0;
    pick.pickedNode = hit.node->Owner();
    pick.pickPoint.x() = hit.pos.x();
    pick.pickPoint.y() = hit.pos.y();
    pick.pickPoint.z() = hit.pos.z();
    pick.triIndex = hit.triIndex;
    return hit.node->Owner()->GetActor();
}

// The points around the tip, nearest first: every pixel of the area for a click, every
// second one for the cursor (which looks every frame and holds what it found a moment).
struct PortAround {
    signed char d[144][2];
    int n;
    explicit PortAround(int step) : n(0)
    {
        for (int r2 = 1; r2 <= 9 * 9 * 2; r2++)
            for (int y = -2; y <= 9; y += step)
                for (int x = -2; x <= 9; x += step)
                    if (x * x + y * y == r2 && n < 144) {
                        d[n][0] = (signed char)x;
                        d[n][1] = (signed char)y;
                        n++;
                    }
    }
};
static bool sHoverPick; // PortHover3D asks, not a click

MfAiActor *PortWidePick(ViewEventHandler *handler, MfMouseHookData *data, MfPickData &pick)
{
    MfAiActor *actor = port_cast(handler, data->x, data->y, pick);
    if (port_interactive(actor))
        return actor;
    static const PortAround kClick(1), kHover(2);
    const PortAround &around = sHoverPick ? kHover : kClick;
    for (int i = 0; i < around.n; i++) {
        MfPickData near;
        MfAiActor *other = port_cast(handler, data->x + around.d[i][0] / 640.0f, data->y + around.d[i][1] / 480.0f, near);
        if (port_interactive(other)) {
            pick = near;
            return other;
        }
    }
    return actor;
}

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
    if (actor->GetProperty(MfString("HCTalking"), prop))
        return kTalkCursor;
    return port_scene_item(actor) ? kHighlightCursor : kPlainCursor;
}

// ---- A held item sparkles over what a click with it is used on: an item hotspot (the
// fishing pole) or a 3D actor (a character), not the floor or the scenery, where a click
// only makes the player walk and puts the item back.

static bool sOverItemHotspot;   // the mouse is over an item hotspot (its enter/leave events)
static unsigned int sHotspotCursor; // the cursor a hotspot set (SetCursorSequence), 0 when left

void PortHotspotCursor(unsigned int seq) { sHotspotCursor = seq; } // (the decompiled code's unsigned long is 32-bit)
static IMBAnim *sSparkleCursor; // the item cursor with a sparkle sequence, and that sequence
static long sSparkleSeq = -1;

void PortOverItemHotspot(bool over) { sOverItemHotspot = over; }

// A sequence of the item cursor (SteamboatNode::SetCursorItem, sequence 1) with yellow
// sparkles around the item, centred on the hot spot like the item.
void PortAddSparkles(IMBAnim *cursor)
{
    sSparkleCursor = 0;
    sSparkleSeq = -1;
    if (!cursor || cursor->GetNumSequences() < 2)
        return;
    long item = cursor->GetCastID(1, 0);
    IMBSurface *src = 0;
    cursor->GetCastMemberSurface(item, &src);
    if (!src)
        return;
    IMBScreenManager *screens = 0;
    IMBScreen *screen = 0;
    MBGetScreenManager(&screens);
    if (screens) {
        screens->GetScreenByIndex(0, &screen);
        screens->Release();
    }
    static const char *const kSparkle[] = {
        "....Y....",
        "....Y....",
        "...YWY...",
        "YYYWWWYYY",
        "...YWY...",
        "....Y....",
        "....Y....",
    };
    const int sw = 9, sh = 7, m = 8; // a sparkle, the margin around the item
    long iw = (long)src->GetWidth(), ih = (long)src->GetHeight();
    long w = iw + 2 * m, h = ih + 2 * m;
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
    MBRect from(0, 0, iw, ih), to(m, m, m + iw, m + ih);
    src->Blit(dst, &from, &to, 1);
    MBColor yellow(255, 214, 64, 255), white(255, 255, 255, 255);
    // three sparkles: top left, top right, bottom right
    const long at[3][2] = {{0, 2}, {w - sw, 0}, {w - sw - 2, h - sh}};
    for (int k = 0; k < 3; k++)
        for (int y = 0; y < sh; y++)
            for (int x = 0; x < sw; x++) {
                char c = kSparkle[y][x];
                if (c == '.')
                    continue;
                MBRect px(at[k][0] + x, at[k][1] + y, at[k][0] + x + 1, at[k][1] + y + 1);
                dst->ColorFill(c == 'W' ? white : yellow, &px);
            }
    MBPoint reg = cursor->GetCastMemberRegistrationPoint(item);
    reg.x -= m;
    reg.y -= m;
    long cast = cursor->AddCastMember(dst, reg);
    long seq = cursor->AddSequence();
    long frame = cursor->AddFrame(seq);
    cursor->SetCastID(seq, frame, cast);
    dst->Release();
    src->Release();
    sSparkleCursor = cursor;
    sSparkleSeq = seq;
}

// A 3D actor a click with an item is used on: not the player, not a floor.
static bool takes_item(MfAiActor *actor, MfAiActor *player)
{
    if (!actor || actor == player)
        return false;
    MBString name = (char *)actor->GetClassNameA();
    for (size_t i = 0; i < name.size(); i++)
        name[i] = (char)tolower((unsigned char)name[i]);
    return name.find("floor") == MBString::npos;
}

void SteamboatNode::PortHover3D()
{
    static long sShown = kPlainCursor; // the cursor this function set (not a hotspot)
    long want = kPlainCursor;
    bool sparkle = false;
    MBMouseEvent evt;
    if (sMfPrimaryActor && sMotivateEventHandler && mMotivateHotspot && mMotivateHotspot->IsActive() &&
        sInputEventChannelActive && MBGetScreenMouseLocation(evt.mMousePos, &evt.mScreen)) {
        MBComPtr<ISBInventory> gui(MBIID_ISBInventory);
        bool onMenu = gui->IsPointOnMenu(evt.mMousePos);
        if (!onMenu) {
            MfPickData pick;
            MfMouseHookData data;
            CopyMBMouseEventToMfMouseHookData(evt, data);
            sHoverPick = true;
            MfAiActor *actor = sMotivateEventHandler->HandleLButtonUp(&data, pick);
            sHoverPick = false;
            if (sCursorItem)
                sparkle = sOverItemHotspot || takes_item(actor, sMfPrimaryActor);
            else
                want = cursor_for(actor, sMfPrimaryActor);
        }
        if (evt.mScreen)
            evt.mScreen->Release();
        if (onMenu && !sCursorItem) {
            sShown = kPlainCursor; // the inventory sets the cursor there (CSBInventory::PortCursor)
            return;
        }
    }
    // a thin thing slips between the cursor's points as it moves: what was found stays
    // found for a moment (this runs once a frame: about 150 ms)
    static int sFoundFrames;
    static long sFoundWant;
    static bool sFoundSparkle;
    if (want != kPlainCursor || sparkle) {
        sFoundFrames = 9;
        sFoundWant = want;
        sFoundSparkle = sparkle;
    } else if (sFoundFrames > 0) {
        sFoundFrames--;
        want = sCursorItem ? kPlainCursor : sFoundWant;
        sparkle = sCursorItem ? sFoundSparkle : false;
    }
    if (!mCursor)
        return;
    if (sCursorItem) {
        if (mCursor == sSparkleCursor && sSparkleSeq >= 0) {
            long seq = sparkle ? sSparkleSeq : 1;
            if (mCursor->GetCurrentSequenceIndex() != seq)
                mCursor->SetCurrentSequence(seq);
        }
        return;
    }
    long now = mCursor->GetCurrentSequenceIndex();
    if (sHotspotCursor) {
        // a hotspot's cursor wins, also when it is the same one this function shows
        sShown = kPlainCursor;
        return;
    }
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
