#pragma once
// MfAi interfaces looked up through MfInterfaceRegistry (mfutil.dll) and the
// hook manager ("MfAiHookManager"). Declarations from the SteamboatApp PDB.
#include "MfString.h"

class __declspec(dllimport) MfAi {
public:
    virtual void GetInterfaceName(char *, int, int &, int &) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
};

class __declspec(dllimport) MfInterfaceRegistry {
public:
    static int GetInterface(const MfString &, MfAi **);
};

enum MfHookPosition {
    MF_HOOK_FIRST = 0,
    MF_HOOK_LAST = 1
};

// Hook ids (MfAiHookManager::CallHooks)
enum {
    MF_HOOK_NODE_CREATE = 0,
    MF_HOOK_NODE_DESTROY = 1,
    MF_HOOK_NODE_SET_PROTOTYPE = 2,
    MF_HOOK_NODE_CHANGE_COL_GEOM = 3,
    MF_HOOK_NODE_CHANGE_TRANSFORM = 4,
    MF_HOOK_NODE_CHANGE_MATERIAL = 5,
    MF_HOOK_NODE_CHANGE_RENDER_SETTINGS = 6,
    MF_HOOK_NODE_CHANGE_LIGHT_SETTINGS = 7,
    MF_HOOK_NODE_CHANGE_CAMERA_SETTINGS = 8,
    MF_HOOK_NODE_CHANGE_PARENT = 9,
    MF_HOOK_NODE_CHANGE_OWNER = 10,
    MF_HOOK_NODE_ADD_TO_SPACE = 11,
    MF_HOOK_NODE_REMOVE_FROM_SPACE = 12,
    MF_HOOK_ACTOR_CHANGE_SPACE = 13,
    MF_HOOK_UPDATE_WORLD = 14,
    MF_HOOK_PROXIMITY_NEAR_CHECK = 15,
    MF_HOOK_PROXIMITY_BLOCK_CHECK = 16,
    MF_HOOK_CAMERA_HAS_CHANGED = 17,
    MF_HOOK_USER_EVENT = 18,
    MF_HOOK_LBUTTON_UP = 19,
    MF_HOOK_LBUTTON_DOWN = 20,
    MF_HOOK_RBUTTON_UP = 21,
    MF_HOOK_RBUTTON_DOWN = 22,
    MF_HOOK_MOUSE_PRESS_MOVE = 23,
    MF_HOOK_MOUSE_MOVE = 24,
    MF_HOOK_MOUSE_WHEEL = 25,
    MF_HOOK_COL_QUERY = 26,
    MF_HOOK_MISC_DATA = 27,
    MF_HOOK_ACTIVITY_DATA = 28,
    MF_HOOK_FILTER_EDIT = 29,
    MF_HOOK_GUARD_COP = 30,
    MF_HOOK_FILE_HANDLE = 31,
    MF_HOOK_FILE_DESTINATION = 32,
    MF_HOOK_RAY_CAST = 33,
    MF_HOOK_SPHERE_CHECK = 34,
    MF_HOOK_CREATE_SPACE_COLNODE = 35,
    MF_HOOK_OVERRIDE_UPDATE_SPHERE_TREE = 36,
    MF_HOOK_OVERRIDE_DELETE_SPHERE_TREE = 37,
    MF_NUM_FAST_HOOKS = 38
};
