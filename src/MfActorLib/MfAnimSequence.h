#pragma once
#ifndef MFACTOR_API
#define MFACTOR_API __declspec(dllimport)
#endif
// MfAnimSequence: a recorded animation ('Sequ'): a sequence folder with the
// actors (Actors\), their motion (Motion\*.anm, one MfAnimFile per actor: one
// frame record per node, translation + rotation, 28 bytes; MfAnimFile is not
// exported, its functions have no names in the binary) and the add/remove-actor,
// sound and material events.
#include "MfFile.h"
#include "MfActorRecordList.h"
#include "MfAnimEvent.h"
class MfProtoActor;
class MfTransform3;
class MfVector3;

// size 0x4c
class MfAnimFile : public MfFile {
    friend class MfAnimSequence;
public:
    MfAnimFile();
    virtual ~MfAnimFile();
    virtual int Open(const MfString &, int);
    virtual int Close();
    virtual unsigned long ReadBytes(unsigned long, void *);

    int WriteFrame(const void *, int);
    int ReadFrame(int, void *, int);
    int ReadNodeFrames(int, MfTransform3 *, int, int &, int &);
private:
    static MfFileType ClassTypeInFile();
    void _Seek(unsigned long);
    int _InitWrite();
    int _InitRead();

    int m_numFrames;            // 0x34
    MfString m_name;            // 0x38
    int m_numNodes;             // 0x3c
    unsigned long m_headerPos;  // 0x40
    unsigned long m_dataStart;  // 0x44
    unsigned long m_dataEnd;    // 0x48
};

// size 0xb0
class MFACTOR_API MfAnimSequence : public MfFile {
public:
    MfAnimSequence();
    virtual ~MfAnimSequence();
    virtual int Open(const MfString &, int);
    virtual int Close();

    int IsOpen() { return m_isOpen; }
    const MfString &GetPath() { return m_path; }
    void SetTimeStep(unsigned long t) { m_timeStep = t; }
    int NextFrame() { return ++m_numFrames; }
    int GetNumFrames() const { return m_numFrames; }
    unsigned long GetTimeStep() const { return m_timeStep; }
    const MfActorRecordList &GetActorList() const { return m_actorList; }
    const MfAnimEventList &GetAddActorEvents() const { return m_addActorEvents; }
    const MfAnimEventList &GetRemoveActorEvents() const { return m_removeActorEvents; }
    const MfAnimEventList &GetSoundEvents() const { return m_soundEvents; }
    const MfAnimEventList &GetMaterialEvents() const { return m_materialEvents; }

    int WriteFrameData(const MfString &, MfTransform3 *, int);
    int ReadOneFrame(const MfString &, int, MfTransform3 *, int);
    int ReadNodeFrames(const MfString &, int, MfTransform3 *, int, int &, int &);
    int PostSoundEvent(const MfString &, const MfString &, const MfVector3 &);
    int PostAddActorEvent(MfProtoActor *, const MfString &);
    int PostRemoveActorEvent(const MfString &);
private:
    static MfFileType ClassTypeInFile();
    int _SetupSequenceFolder(MfString &);
    int _InitWriteSequence();
    int _FinishWriteSequence();
    int _WriteActorList();
    int _WriteEvents();
    int _SaveProtoActors();
    int _SaveProtoActor(MfProtoActor *);
    MfAnimFile *_GetAnimFile(const MfString &);
    int _InitReadSequence();
    int _ReadActorList();
    int _ReadEvents();
    MfProtoActor *_LoadProtoActor(const MfString &);

    int m_numFrames;                      // +0x34
    unsigned long m_timeStep;             // +0x38
    MfString m_path;                      // +0x3c
    MfActorRecordList m_actorList;        // +0x40
    MfAnimEventList m_addActorEvents;     // +0x50
    MfAnimEventList m_removeActorEvents;  // +0x64
    MfAnimEventList m_soundEvents;        // +0x78
    MfAnimEventList m_materialEvents;     // +0x8c
    MfDblList m_animFiles;                // +0xa0 (MfAnimFile *)
};
