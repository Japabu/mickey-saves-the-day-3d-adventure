# Improvements over the original game (not ports of its behaviour): each one hooks a
# function of port/improve/<module>/ into the decompiled code, which keeps matching the
# original binaries. See port/README.md, "Improvements".
EDITS = {
    # The cursor tells the truth:
    # - over a 3D object a click would do something with (an item to pick up, a character
    #   to talk to) it shows the highlight, as it does over the painted hotspots (the
    #   original never looked at the 3D scene before the click);
    # - over the open inventory bar it shows the highlight only on what reacts to a click:
    #   the three buttons and the items that can be used in this scene (the original
    #   highlighted the whole bar, empty slots and unusable items included).
    # Smooth movement at any frame rate: the skills (animations) are sampled between their
    # keyframes too. The game's data switches that off, so the characters' poses and their
    # walk along a path advanced in 30 Hz steps; at the frame rates of 2001 each frame
    # crossed a step and that looked even, at today's they come as visible jerks (most
    # frames move by nothing, then one by a whole keyframe).
    # A walking character's height follows the floor smoothly (port/improve/RuntimeLib/
    # walk.cpp) instead of jumping by up to a quarter of a leg when a walk starts.
    'RuntimeLib/MfLocoActivity.cpp': [
        ('    mLastSampleTime = sample;\n    mPathPos = pos;\n',
         '    mLastSampleTime = sample; { void PortEaseWalkHeight(MfSkeletonNode *, const MfTransform3 &, unsigned long); '
         'PortEaseWalkHeight(mActor->skeleton(), mLastTransform, elapsed); }\n    mPathPos = pos;\n', 1),
    ],
    # Leaving the game: Mickey's goodbye ends the main menu (the credits follow when it is
    # over), but an actor does not start a line while his last one plays, so a Yes clicked
    # while Mickey still asked "really?" dropped the goodbye and the menu waited for ever.
    # His question stops first.
    'SteamboatApp/SBMainMenuNode.cpp': [
        ('                StartDialogue("ui03xmc0110", true);\n',
         '                { MBComPtr<IMBSoundManager> sounds(MBIID_IMBSoundManager); if (sounds) sounds->StopChannel(20); } '
         'StartDialogue("ui03xmc0110", true);\n', 1),
    ],
    # The credits can be skipped: a click, Space, Enter or Escape ends them
    # (port/improve/SteamboatApp/credits.cpp).
    'SteamboatApp/SBCreditsNode.h': [
        ('    virtual void OnKeyDown(MBKeyboardEvent &key);',
         '    virtual void OnKeyDown(MBKeyboardEvent &key); virtual void OnLMouseUp(MBMouseEvent &evt); bool mPortDone;', 1),
    ],
    'SteamboatApp/SBCreditsNode.cpp': [
        ('    SetBackgroundMusicState(true);\n}\n', '    SetBackgroundMusicState(true); mPortDone = false;\n}\n', 1),
        ('void SBCreditsNode::OnKeyDown(MBKeyboardEvent &key)\n{\n    SteamboatNode::OnKeyDown(key);\n',
         'void SBCreditsNode::OnKeyDown(MBKeyboardEvent &key)\n{\n    SteamboatNode::OnKeyDown(key); '
         'if (key.mCharCode == 27 || key.mCharCode == \' \' || key.mCharCode == 13) Done();\n', 1),
        ('    MBComPtr<ISBNodeManager> mgr(MBIID_ISBNodeManager);\n    mgr->AddNode("MV");\n',
         '    if (mPortDone) return; mPortDone = true; MBComPtr<ISBNodeManager> mgr(MBIID_ISBNodeManager);\n    mgr->AddNode("MV");\n', 1),
    ],
    # Two walking characters do not stop for each other. The engine cancels a walk when
    # another character's path comes near (MfProximityManager), puts the walker into
    # its idle pose and plans again once the way is clear, and between equals the one
    # that waits is the one lower in memory: in the main menu Mickey stopped at random
    # while Minnie crossed, then walked on from there (or turned and slid to his spot
    # when the idle pose's end reached his behaviour as the walk's). A character
    # standing in the way still makes a walker plan around it.
    'RuntimeLib/MfLiveActor.cpp': [
        ('    if (priority < 0)\n        return;\n    if (!moving && !mBlockList.GetCount())\n',
         '    if (priority < 0 || (walking && other->IsCurrentActivity(MfLocoActivity::TYPE)))\n        return;\n'
         '    if (!moving && !mBlockList.GetCount())\n', 1),
    ],
    # The options panel's volumes (platform/win32/options.cpp; Escape opens it): every
    # channel's sound gets its slider's gain (20 voices, 10 music, the rest sound effects)
    # as a float factor with its volume (platform/media/miles.cpp, port_sound_next_channel),
    # again when a slider moves; the background music stream follows
    # the music slider (port/improve/MBEngine/sound.cpp, SteamboatApp/sound.cpp).
    'MBEngine/MBSoundChannel.h': [
        ('    long mChannelID;\n    float mRateMultiplier;\n    short mVolume;',
         '    long mChannelID;\n    float mRateMultiplier;\n    short mVolume; unsigned mPortGainVersion;', 1),
    ],
    'MBEngine/MBSoundChannel.cpp': [
        ('void MBSoundChannel::Idle(void)\n{\n',
         'void MBSoundChannel::Idle(void)\n{ { unsigned PortSoundGainVersion(); unsigned v = PortSoundGainVersion(); '
         'if (v != mPortGainVersion) { mPortGainVersion = v; VolumeChanged(); } }\n', 1),
        ('    return (short)(mVolume * masterVolume / 127);\n',
         '    void PortNextSoundChannel(int); PortNextSoundChannel(mChannelID); '
         'return (short)(mVolume * masterVolume / 127);\n', 1),
    ],
    'MfActorLib/MfProtoSkill.cpp': [
        ('        s >> iSampleRate >> bInterpolate;\n',
         '        s >> iSampleRate >> bInterpolate; bInterpolate = 1;\n', 1),
    ],
    'SteamboatApp/SteamboatNode.h': [
        ('    virtual void OnMouseMove(MBMouseEvent &evt);        // vftable+0x5c',
         '    virtual void OnMouseMove(MBMouseEvent &evt); void PortHover3D(); static bool PortHoldsItem(); // vftable+0x5c; '
         'PortHover3D: port/improve/SteamboatApp/cursor.cpp', 1),
    ],
    'SteamboatApp/SteamboatNode.cpp': [
        # the background music follows the options panel's music volume (see MBSoundChannel below)
        ('                sBackgroundMusic = AIL_open_stream(dig, path, 0);\n',
         '                sBackgroundMusic = AIL_open_stream(dig, path, 0); { void PortMusicStream(void *); PortMusicStream(sBackgroundMusic); }\n', 2),
        # Painted hotspots a click explains (dialog) show a cursor of their own, the talk
        # cursor (a speech bubble, port/improve/SteamboatApp/cursor.cpp); those that play a
        # sound show the highlight. The original's scene files gave most of them the
        # highlight and a few the plain cursor (cursor = 0, e.g. Pete's house in
        # PEscene.ini), which says "walk there".
        ('                        mouseEvent->data = motivateMessageHandler;\n',
         '                        mouseEvent->data = motivateMessageHandler; if (!mouseEvent->cursorID || mouseEvent->cursorID == 10) mouseEvent->cursorID = kPortTalkCursor;\n', 1),
        # the talk cursor: a sequence the cursor animation gets once its own are aligned
        ('void UpdateCursorRegpoints(IMBAnim *cursor);\n',
         'void UpdateCursorRegpoints(IMBAnim *cursor); void PortAddTalkCursor(IMBAnim *cursor); enum { kPortTalkCursor = 11 };\n', 1),
        ('            cursor->SetCastMemberRegistrationPoint(castID, regPoint);\n        }\n    }\n}\n',
         '            cursor->SetCastMemberRegistrationPoint(castID, regPoint);\n        }\n    }\n    PortAddTalkCursor(cursor); }\n', 1),
        ('                        mouseEvent->data = sfx;\n',
         '                        mouseEvent->data = sfx; if (!mouseEvent->cursorID) mouseEvent->cursorID = 10;\n', 1),
        # once a frame (mouse moves never reach OnMouseMove: the node does not listen on
        # the channel they come on)
        ('void SteamboatNode::Idle()\n{\n', 'void SteamboatNode::Idle()\n{ PortHover3D();\n', 1),
        # A held item sparkles over an item hotspot or a 3D actor a click with it is used on
        # (port/improve/SteamboatApp/cursor.cpp): the item cursor gets a sparkle sequence, the
        # item hotspots' enter and leave events are noted while an item is held too.
        ('        case 203:\n            if (!sCursorItem)\n                SetCursorSequence(0);\n            break;\n'
         '        case 202:\n            if (!sCursorItem)\n                SetCursorSequence(((SBItem *)data)->GetRolloverCursor());\n',
         '        case 203: { void PortOverItemHotspot(bool); PortOverItemHotspot(false); }\n            if (!sCursorItem)\n                SetCursorSequence(0);\n            break;\n'
         '        case 202: { void PortOverItemHotspot(bool); PortOverItemHotspot(true); }\n            if (!sCursorItem)\n                SetCursorSequence(((SBItem *)data)->GetRolloverCursor());\n', 1),
        ('        surface->Release();\n    }\n\n    int cursorInt = 0;\n    MBString cursorString = item->GetClassNameA();\n',
         '        surface->Release();\n    } { void PortAddSparkles(IMBAnim *); PortAddSparkles(cursor); }\n\n    int cursorInt = 0;\n    MBString cursorString = item->GetClassNameA();\n', 1),
    ],
    'SteamboatApp/CSBInventory.h': [
        ('    virtual void WithinRollover(const MBPoint &worldMousePosition);',
         '    virtual void WithinRollover(const MBPoint &worldMousePosition); '
         'void PortCursor(const MBPoint &worldMousePosition); // port/improve/SteamboatApp/cursor.cpp', 1),
    ],
    'SteamboatApp/CSBInventory.cpp': [
        ('    if (m_eBarState == visible)\n        SteamboatNode::GetActiveCursor()->SetCurrentSequence(10);',
         '    if (m_eBarState == visible)\n        PortCursor(worldMousePosition);', 1),
        ('                    SetGUIAnimSequence(eAnim, m_eCurrentHighlightState);\n'
         '                    m_eCurrentHighlightAnim = eAnim;\n'
         '                }\n'
         '            }\n'
         '        }\n'
         '    }\n',
         '                    SetGUIAnimSequence(eAnim, m_eCurrentHighlightState);\n'
         '                    m_eCurrentHighlightAnim = eAnim;\n'
         '                }\n'
         '            }\n'
         '        }\n'
         '    PortCursor(worldMousePosition); }\n', 1),
    ],
    # Facing the point it stands on: the script of Von Drake's lab (DKinit's face_board,
    # after the tissue dialogue) turns him toward his own saved position. The direction
    # is then (0, 0, 0) and its normalization 0/0 made his transform NaN, so he vanished
    # (the original's x87 arithmetic likely left a tiny nonzero rest). Such a turn is
    # no turn.
    'RuntimeLib/MfApiActor.cpp': [
        ('    float length = (float)sqrt(to.lengthSquared());\n',
         '    float length = (float)sqrt(to.lengthSquared()); if (!(length > 1e-6f)) return;    // PORT: no direction, no turn\n', 1),
    ],
}
