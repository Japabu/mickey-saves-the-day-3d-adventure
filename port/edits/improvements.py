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
    'SteamboatApp/SteamboatNode.h': [
        ('    virtual void OnMouseMove(MBMouseEvent &evt);        // vftable+0x5c',
         '    virtual void OnMouseMove(MBMouseEvent &evt); void PortHover3D(); static bool PortHoldsItem(); // vftable+0x5c; '
         'PortHover3D: port/improve/SteamboatApp/cursor.cpp', 1),
    ],
    'SteamboatApp/SteamboatNode.cpp': [
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
}
