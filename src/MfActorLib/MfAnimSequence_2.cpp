// MfActorLib.dll 0x10003740-0x10004c90: MfAnimSequence (vc5 /O2 /GX)
// Out-of-line members of MfAnimSequence (the class's inline accessors are
// emitted in MfAnimSequence.cpp, MfAnimFile in MfFile.cpp).
#include "MfFile.h"
#include "MfString.h"
#include "MfDblList.h"
#include "MfCopyFolder.h"
#include "MfStreamObject.h"
#include "MfStream.h"
#include "MfFileStream.h"
#include "MfStreamSizedObject.h"
#include "MfTypedValue.h"
#include "MfListValue.h"
#include "MfFileType.h"
#include "MfProtoActor.h"
#include "MfActorRecord.h"
#include "MfActorRecordList.h"
#include "MfAnimEventList.h"
#include "MfAnimFile.h"
#include "MfAnimSequence.h"

// 0x10003740 ??0MfAnimSequence@@QAE@XZ
MfAnimSequence::MfAnimSequence()
    : m_numFrames(0), m_timeStep(50)
{
    m_type = ClassTypeInFile();
    m_version = 1;
}

// 0x10003860 ??_EMfAnimSequence@@UAEPAXI@Z (compiler generated)
// 0x100038c0 ??1MfAnimSequence@@UAE@XZ
MfAnimSequence::~MfAnimSequence()
{
    if (m_isOpen)
        Close();
}

// 0x10003a00 ?Open@MfAnimSequence@@UAEHABVMfString@@H@Z
int MfAnimSequence::Open(const MfString &name, int mode)
{
    if (m_isOpen)
        Close();
    int ok;
    if (mode == 1) {
        MfString path(name);
        if (!_SetupSequenceFolder(path))
            return 0;
        if (!MfFile::Open(path, 1))
            return 0;
        ok = _InitWriteSequence();
    } else {
        if (!MfFile::Open(name, mode))
            return 0;
        MfString drive;
        MfString dir;
        name.SplitPath(&drive, &dir, 0, 0);
        m_path = drive + dir;
        ok = _InitReadSequence();
    }
    if (!ok) {
        Close();
        return 0;
    }
    return 1;
}

// 0x10003ba0 ?WriteFrameData@MfAnimSequence@@QAEHABVMfString@@PAVMfTransform3@@H@Z
int MfAnimSequence::WriteFrameData(const MfString &name, MfTransform3 *xf, int numNodes)
{
    if (!m_isOpen)
        return 0;
    MfAnimFile *file = _GetAnimFile(name);
    if (!file)
        return 0;
    return file->WriteFrame(xf, numNodes);
}

// 0x10003be0 ?ReadOneFrame@MfAnimSequence@@QAEHABVMfString@@HPAVMfTransform3@@H@Z
int MfAnimSequence::ReadOneFrame(const MfString &name, int frame, MfTransform3 *xf, int numNodes)
{
    if (!m_isOpen)
        return 0;
    MfAnimFile *file = _GetAnimFile(name);
    if (!file)
        return 0;
    return file->ReadFrame(frame, xf, numNodes);
}

// 0x10003c20 ?ReadNodeFrames@MfAnimSequence@@QAEHABVMfString@@HPAVMfTransform3@@HAAH2@Z
int MfAnimSequence::ReadNodeFrames(const MfString &name, int node, MfTransform3 *xf,
                                   int numFrames, int &rotChanged, int &posChanged)
{
    if (!m_isOpen)
        return 0;
    MfAnimFile *file = _GetAnimFile(name);
    if (!file)
        return 0;
    return file->ReadNodeFrames(node, xf, numFrames, rotChanged, posChanged);
}

// 0x10003c60 ?Close@MfAnimSequence@@UAEHXZ
int MfAnimSequence::Close()
{
    int ok = 1;
    if (m_numFrames > 0 && m_mode == 1) {
        if (!_FinishWriteSequence())
            ok = 0;
    }
    MfDblListItem *it;
    MfAnimFile *file;
    for (file = (MfAnimFile *)m_animFiles.GetFirst(it); file;
         file = (MfAnimFile *)m_animFiles.GetNext(it))
        delete file;
    m_animFiles.Clear();
    m_numFrames = 0;
    m_actorList.Clear();
    m_addActorEvents.Clear();
    m_removeActorEvents.Clear();
    m_soundEvents.Clear();
    m_materialEvents.Clear();
    if (MfFile::Close() && ok)
        return 1;
    return 0;
}

// 0x10003d20 ?PostSoundEvent@MfAnimSequence@@QAEHABVMfString@@0ABVMfVector3@@@Z
int MfAnimSequence::PostSoundEvent(const MfString &, const MfString &, const MfVector3 &)
{
    return 1;
}

// 0x10003d30 ?PostAddActorEvent@MfAnimSequence@@QAEHPAVMfProtoActor@@ABVMfString@@@Z
int MfAnimSequence::PostAddActorEvent(MfProtoActor *actor, const MfString &name)
{
    if (m_actorList.Contains(name))
        return 0;
    MfString animPath = m_path + MfString("Motion\\");
    animPath << name << MfString(".anm");
    MfAnimFile *file = new MfAnimFile;
    if (!file->Open(animPath, 1)) {
        delete file;
        return 0;
    }
    MfString actorName(name);
    m_actorList.AddRecord(new MfActorRecord(actor, actorName));
    m_animFiles.Append(file);
    return 1;
}

// 0x10003ee0 ?PostRemoveActorEvent@MfAnimSequence@@QAEHABVMfString@@@Z
int MfAnimSequence::PostRemoveActorEvent(const MfString &)
{
    return 1;
}

// 0x10003ef0 ?_SetupSequenceFolder@MfAnimSequence@@AAEHAAVMfString@@@Z
int MfAnimSequence::_SetupSequenceFolder(MfString &path)
{
    MfString drive;
    MfString dir;
    MfString fname;
    MfString ext;
    path.SplitPath(&drive, &dir, &fname, &ext);
    m_path = drive + dir + MfString("\\");
    OFSTRUCT ofs;
    OpenFile(m_path, &ofs, OF_EXIST);
    if (ofs.nErrCode == 5) {
        MessageBoxA(0, "The selected sequence folder already exists.\n"
                       "Please choose another sequence name.", 0, 0);
        return 0;
    }
    if (!CreateDirectoryA(m_path, 0)) {
        MessageBoxA(0, "Could not create folder for sequence files.", 0, 0);
        return 0;
    }
    MfString actorsDir = m_path + MfString("Actors\\");
    if (OpenFile(actorsDir, &ofs, OF_EXIST) == -1 && !CreateDirectoryA(actorsDir, 0)) {
        MessageBoxA(0, "Could not create Actors folder within sequence folder.", 0, 0);
        return 0;
    }
    MfString soundsDir = m_path + MfString("Sounds\\");
    if (OpenFile(soundsDir, &ofs, OF_EXIST) == -1 && !CreateDirectoryA(soundsDir, 0)) {
        MessageBoxA(0, "Could not create Sounds folder within sequence folder.", 0, 0);
        return 0;
    }
    MfString motionDir = m_path + MfString("Motion\\");
    if (OpenFile(motionDir, &ofs, OF_EXIST) == -1 && !CreateDirectoryA(motionDir, 0)) {
        MessageBoxA(0, "Could not create Motion folder within sequence folder.", 0, 0);
        return 0;
    }
    return 1;
}

// 0x100042f0 ?SizeOf@MfStream@@SAHD@Z (folded with
// ?_ReadEvents@MfAnimSequence@@AAEHXZ and the MfStream::SizeOf inlines)
int MfAnimSequence::_InitWriteSequence()
{
    return 1;
}

// 0x10004300 ?_FinishWriteSequence@MfAnimSequence@@AAEHXZ
int MfAnimSequence::_FinishWriteSequence()
{
    int ok = Write(m_numFrames);
    ok &= _WriteActorList();
    ok &= _WriteEvents();
    ok &= _SaveProtoActors();
    return ok;
}

// 0x10004330 ?_WriteActorList@MfAnimSequence@@AAEHXZ
int MfAnimSequence::_WriteActorList()
{
    if (!Write(m_actorList.GetCount()))
        return 0;
    MfDblListItem *it;
    MfActorRecord *rec;
    for (rec = (MfActorRecord *)m_actorList.GetFirst(it); rec;
         rec = (MfActorRecord *)m_actorList.GetNext(it)) {
        if (!Write(rec->m_name))
            return 0;
        if (!Write(rec->m_actor->Name()))
            return 0;
    }
    return 1;
}

// 0x100043e0 ?_WriteEvents@MfAnimSequence@@AAEHXZ
int MfAnimSequence::_WriteEvents()
{
    MfFileStream stream(m_handle, MF_FILE_WRITE, false);
    stream << m_addActorEvents;
    stream << m_removeActorEvents;
    stream << m_soundEvents;
    stream << m_materialEvents;
    return stream.Status() == MF_STREAM_OK;
}

// 0x100044b0 ?_SaveProtoActors@MfAnimSequence@@AAEHXZ
int MfAnimSequence::_SaveProtoActors()
{
    MfDblListItem *it;
    MfActorRecord *rec;
    for (rec = (MfActorRecord *)m_actorList.GetFirst(it); rec;
         rec = (MfActorRecord *)m_actorList.GetNext(it))
        _SaveProtoActor(rec->m_actor);
    return 1;
}

// 0x100044f0 ?_SaveProtoActor@MfAnimSequence@@AAEHPAVMfProtoActor@@@Z
int MfAnimSequence::_SaveProtoActor(MfProtoActor *actor)
{
    MfString dir = m_path + MfString("Actors\\") + actor->Name() + MfString("\\");
    OFSTRUCT ofs;
    if (OpenFile(dir, &ofs, OF_EXIST) != -1)
        return 1;
    if (!CreateDirectoryA(dir, 0))
        return 0;
    MfCopyFolder copier(actor->Path(), MfString("Textures"), dir);
    if (!copier.Copy(1)) {
        MessageBoxA(0, "Couldn't create actor 'Textures' folder", 0, 0);
        return 0;
    }
    MfString oldPath(actor->Path());
    actor->Path() = dir;
    MfString file = actor->Path() + actor->Name() + MfString(".act");
    int err = MfProtoActor::WriteActorFile(file, actor);
    actor->Path() = oldPath;
    return err == 0;
}

// 0x100047a0 ?_GetAnimFile@MfAnimSequence@@AAEPAVMfAnimFile@@ABVMfString@@@Z
MfAnimFile *MfAnimSequence::_GetAnimFile(const MfString &name)
{
    MfDblListItem *it;
    MfAnimFile *file;
    for (file = (MfAnimFile *)m_animFiles.GetFirst(it); file;
         file = (MfAnimFile *)m_animFiles.GetNext(it)) {
        if (file->m_name == name)
            return file;
    }
    return 0;
}

// 0x10004810 ?_InitReadSequence@MfAnimSequence@@AAEHXZ
int MfAnimSequence::_InitReadSequence()
{
    if (!Read(m_numFrames))
        return 0;
    if (!_ReadActorList())
        return 0;
    return _ReadEvents() != 0;
}

// 0x10004840 ?_ReadActorList@MfAnimSequence@@AAEHXZ
int MfAnimSequence::_ReadActorList()
{
    int count = 0;
    if (!Read(count))
        return 0;
    for (int i = 0; i < count; i++) {
        MfString actorName;
        MfString actorFile;
        if (!Read(actorName) || !Read(actorFile))
            return 0;
        MfProtoActor *actor = _LoadProtoActor(actorFile);
        if (actor) {
            MfString animPath = m_path + MfString("Motion\\");
            animPath << actorName << MfString(".anm");
            MfAnimFile *file = new MfAnimFile;
            if (file->Open(animPath, 0)) {
                m_actorList.AddRecord(new MfActorRecord(actor, actorName));
                m_animFiles.Append(file);
            } else {
                delete file;
            }
        } else {
            MfString msg = MfString("Unable to load ProtoActor: ") + actorFile;
            MessageBoxA(0, msg, 0, 0);
        }
    }
    return 1;
}

// 0x100042f0 ?_ReadEvents@MfAnimSequence@@AAEHXZ (the linker folded _InitWriteSequence into it)
int MfAnimSequence::_ReadEvents()
{
    return 1;
}

// 0x10004b00 ?_LoadProtoActor@MfAnimSequence@@AAEPAVMfProtoActor@@ABVMfString@@@Z
MfProtoActor *MfAnimSequence::_LoadProtoActor(const MfString &name)
{
    MfDblListItem *it;
    MfActorRecord *rec;
    for (rec = (MfActorRecord *)m_actorList.GetFirst(it); rec;
         rec = (MfActorRecord *)m_actorList.GetNext(it)) {
        if (rec->m_actor->Name() == name)
            return rec->m_actor;
    }
    MfString dir = m_path + MfString("Actors\\") + name + MfString("\\");
    MfString file = dir + name + MfString(".act");
    return MfProtoActor::ReadActorFile(file);
}

// 0x10004c80 ??_EMfListValue@@WM@AEPAXI@Z (compiler generated adjustor thunk for
// MfListValue's MfDblList vtable, emitted here because MfListValue.h is included)


// Also matched by this code:
// piccolocomp.dll 0x1000fd80 ?PostSoundEvent@MfAnimSequence@@QAEHABVMfString@@0ABVMfVector3@@@Z
// plugins/pscstdlib.dll 0x100082d0 ?PostSoundEvent@MfAnimSequence@@QAEHABVMfString@@0ABVMfVector3@@@Z