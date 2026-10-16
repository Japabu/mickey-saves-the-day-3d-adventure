// MfMsgTypeTable: the names of the message types (RuntimeLib.dll
// 100648d0-10067bd0).
#include "MfMsgTypeTable.h"
#include "MfDictionary.h"
#include "MfStringList.h"

// a name of a message type (the list of names of the tables)
struct MfMsgTypeName {
    MfMsgTypeName(const MfMsgTypeInfo &info, const MfString &name) : mInfo(-1, -1)
    {
        mInfo = info;
        mName = name;
    }

    MfMsgTypeInfo mInfo;                                    // 0x00
    MfString mName;                                         // 0x08
};

static int gTableRefs;                                      // the instances
static MfDictionary *gTypes;                                // name -> MfMsgTypeInfo
static MfStringList *gTypeNames;
static MfDblList *gNames;                                   // of MfMsgTypeName

// 0x100648d0 ??0MfMsgTypeTable@@QAE@XZ
MfMsgTypeTable::MfMsgTypeTable()
{
    mFirstSub = 1000;
    if (!gTypes)
        Init();
    gTableRefs++;
}

// 0x10064900 ??1MfMsgTypeTable@@QAE@XZ
MfMsgTypeTable::~MfMsgTypeTable()
{
    if (--gTableRefs == 0) {
        if (gTypes)
            delete gTypes;
        gTypes = 0;
        MfDblListItem *pli;
        void *info;
        for (info = mInfos->GetFirst(pli); info; info = mInfos->GetNext(pli))
            delete (MfMsgTypeInfo *)info;
        if (mInfos)
            delete mInfos;
        if (gTypeNames)
            delete gTypeNames;
        gTypeNames = 0;
        MfMsgTypeName *name;
        for (name = (MfMsgTypeName *)gNames->GetFirst(pli); name; name = (MfMsgTypeName *)gNames->GetNext(pli))
            delete name;
        if (gNames)
            delete gNames;
    }
}

// 0x100649d0 ??1MfMsgTypeName@@QAE@XZ

// 0x100649e0 ?Init@MfMsgTypeTable@@IAEXXZ
void MfMsgTypeTable::Init()
{
    MfString name;
    MfMsgTypeInfo *info;
    int i;
    mNextSub = mFirstSub + 62;
    mInfos = new MfDblList;
    gTypes = new MfDictionary(53);
    gTypeNames = new MfStringList;
    gNames = new MfDblList;
    name = "mf_path_blocked";
    info = new MfMsgTypeInfo(15, 0);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_path_cleared";
    info = new MfMsgTypeInfo(15, 1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_actor_approaching";
    info = new MfMsgTypeInfo(15, 2);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_actor_retreating";
    info = new MfMsgTypeInfo(15, 3);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_activity_finished";
    info = new MfMsgTypeInfo(11, 0);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_loco_finished";
    info = new MfMsgTypeInfo(11, 1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_manip_finished";
    info = new MfMsgTypeInfo(11, 2);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_actor_lock";
    info = new MfMsgTypeInfo(17, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_actor_unlock";
    info = new MfMsgTypeInfo(18, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_collision";
    info = new MfMsgTypeInfo(35, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_collision_report_complete";
    info = new MfMsgTypeInfo(37, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_received_collision";
    info = new MfMsgTypeInfo(36, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_manip_result";
    info = new MfMsgTypeInfo(26, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_pose_result";
    info = new MfMsgTypeInfo(28, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_loco_result";
    info = new MfMsgTypeInfo(20, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_stop_result";
    info = new MfMsgTypeInfo(22, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_loco_expanded_search";
    info = new MfMsgTypeInfo(21, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_manip_failure";
    info = new MfMsgTypeInfo(16, 2);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_loco_failure";
    info = new MfMsgTypeInfo(16, 5);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_contour_failed_down";
    info = new MfMsgTypeInfo(57, 1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_all_contour_failed_down";
    info = new MfMsgTypeInfo(57, 2);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_mouse_lbtn_down";
    info = new MfMsgTypeInfo(39, 0);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_mouse_lbtn_up";
    info = new MfMsgTypeInfo(39, 1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_mouse_rbtn_up";
    info = new MfMsgTypeInfo(39, 3);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_mouse_rbtn_down";
    info = new MfMsgTypeInfo(39, 2);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_mouse_move";
    info = new MfMsgTypeInfo(39, 4);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_mouse_ldown_event";
    info = new MfMsgTypeInfo(39, 5);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_mouse_lup_event";
    info = new MfMsgTypeInfo(39, 6);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_mouse_rup_event";
    info = new MfMsgTypeInfo(39, 8);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_mouse_rdown_event";
    info = new MfMsgTypeInfo(39, 7);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_mouse_move_event";
    info = new MfMsgTypeInfo(39, 9);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk1_xaxis_move";
    info = new MfMsgTypeInfo(54, 20);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk1_yaxis_move";
    info = new MfMsgTypeInfo(54, 21);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk1_zaxis_move";
    info = new MfMsgTypeInfo(54, 22);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk1_rx_axis_move";
    info = new MfMsgTypeInfo(54, 23);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk1_ry_axis_move";
    info = new MfMsgTypeInfo(54, 24);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk1_rz_axis_move";
    info = new MfMsgTypeInfo(54, 25);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk1_POV0_move";
    info = new MfMsgTypeInfo(54, 32);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk1_POV1_move";
    info = new MfMsgTypeInfo(54, 33);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk1_POV2_move";
    info = new MfMsgTypeInfo(54, 34);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk1_POV3_move";
    info = new MfMsgTypeInfo(54, 35);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    for (i = 0; i < 10; i++) {
        name = "mf_joystk1_btn";
        name << i + 1;
        name << "_down";
        info = new MfMsgTypeInfo(0x36, i);
        gTypes->Add(name, info);
        gNames->Add(new MfMsgTypeName(*info, name));
        gTypeNames->Add(name);
    }
    name = "mf_joystk2_xaxis_move";
    info = new MfMsgTypeInfo(54, 26);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk2_yaxis_move";
    info = new MfMsgTypeInfo(54, 27);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk2_zaxis_move";
    info = new MfMsgTypeInfo(54, 28);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk2_rx_axis_move";
    info = new MfMsgTypeInfo(54, 29);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk2_ry_axis_move";
    info = new MfMsgTypeInfo(54, 30);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk2_rz_axis_move";
    info = new MfMsgTypeInfo(54, 31);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk2_POV0_move";
    info = new MfMsgTypeInfo(54, 36);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk2_POV1_move";
    info = new MfMsgTypeInfo(54, 37);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk2_POV2_move";
    info = new MfMsgTypeInfo(54, 38);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_joystk2_POV3_move";
    info = new MfMsgTypeInfo(54, 39);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    for (i = 0; i < 10; i++) {
        name = "mf_joystk2_btn";
        name << i + 1;
        name << "_down";
        info = new MfMsgTypeInfo(0x36, i + 10);
        gTypes->Add(name, info);
        gNames->Add(new MfMsgTypeName(*info, name));
        gTypeNames->Add(name);
    }
    name = "mf_actor_deleted";
    info = new MfMsgTypeInfo(10, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_actor_added_to_space";
    info = new MfMsgTypeInfo(50, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_actor_removed_from_space";
    info = new MfMsgTypeInfo(51, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_keyboard_event";
    info = new MfMsgTypeInfo(40, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_keyboard_key_down";
    info = new MfMsgTypeInfo(42, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_keyboard_key_up";
    info = new MfMsgTypeInfo(43, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_keyboard_key_repeat";
    info = new MfMsgTypeInfo(44, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_option_event";
    info = new MfMsgTypeInfo(41, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_enter_space";
    info = new MfMsgTypeInfo(46, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_exit_space";
    info = new MfMsgTypeInfo(45, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_change_parent";
    info = new MfMsgTypeInfo(55, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_sound_finished";
    info = new MfMsgTypeInfo(56, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_string_message";
    info = new MfMsgTypeInfo(58, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_static_actor_change";
    info = new MfMsgTypeInfo(12, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
    name = "mf_new_space_loaded";
    info = new MfMsgTypeInfo(52, -1);
    gTypes->Add(name, info);
    gNames->Add(new MfMsgTypeName(*info, name));
    gTypeNames->Add(name);
}

// 0x10067a30 ??_GMfDictionary@@UAEPAXI@Z

// 0x10067a50 ?Name@MfMsgTypeTable@@QAEPAVMfString@@PAUMfMsgTypeInfo@@@Z
MfString *MfMsgTypeTable::Name(MfMsgTypeInfo *info)
{
    MfDblListItem *pli;
    for (MfMsgTypeName *name = (MfMsgTypeName *)gNames->GetFirst(pli); name;
         name = (MfMsgTypeName *)gNames->GetNext(pli))
        if (name->mInfo.type == info->type && name->mInfo.sub == info->sub)
            return &name->mName;
    return 0;
}

// 0x10067ab0 ?Lookup@MfMsgTypeTable@@QAE?AUMfMsgTypeInfo@@ABVMfString@@@Z
MfMsgTypeInfo MfMsgTypeTable::Lookup(const MfString &name)
{
    MfMsgTypeInfo *info = Find(name);
    return *info;
}

// 0x10067ad0 ?Find@MfMsgTypeTable@@IAEPAUMfMsgTypeInfo@@ABVMfString@@@Z
MfMsgTypeInfo *MfMsgTypeTable::Find(const MfString &name)
{
    MfMsgTypeInfo *info = (MfMsgTypeInfo *)gTypes->Find(name);
    if (!info) {
        info = new MfMsgTypeInfo(14, mNextSub);
        mInfos->Add(info);
        gTypes->Add(name, info);
        gNames->Add(new MfMsgTypeName(*info, name));
        gTypeNames->Add(name);
        mNextSub++;
    }
    return info;
}
