// The keyframes of a skill's extra tracks (MfSkill::SoundTrack, EventTrack).
// RuntimeLib's own classes (unit 10053220-100537d0); only what other units
// use is declared so far.
#ifndef RUNTIMELIB_MFSKILLKEYFRAMES_H
#define RUNTIMELIB_MFSKILLKEYFRAMES_H
#include "MfKeyframe.h"
#include "MfKeyList.h"
#include "MfMaterial.h"
#include "MfString.h"
class MfAiActorNode;
class MfISoundResource;
class MfSkeletonTraversal;

// a sound the skill starts at its time
class MfSoundKeyframe : public MfKeyframe {
public:
    enum { STREAM_TYPE = 'yeKS' };

    MfSoundKeyframe();                                      // 10053220
    MfSoundKeyframe(const MfSoundKeyframe &key);            // 100532b0
    virtual ~MfSoundKeyframe();                             // 100533b0
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &stream);
    virtual MfStreamError WriteToStream(MfStream &stream) const;
    virtual MfKeyframe *CreateCopy();
    void SetName(MfString name);                            // 100534a0
    void SetVolume(float volume);                           // 10053420
    MfString Name() { return mName; }
    float Volume() { return mVolume; }
    // loads the sound file; 0 if it cannot be found (100535e0)
    int Load();
    // starts the sound; returns whether there is one
    int Play(MfAiActorNode *node);                          // 100535b0

    float mVolume;                                          // 0x0c in dB
    MfString mName;                                         // 0x10
    int m14, m18;
    MfISoundResource *mSound;                               // 0x1c
    int mMute;                                              // 0x20
    int mAmbient;                                           // 0x24
    int mStream;                                            // 0x28
};

// a message the skill sends to its actor at its time
class MfEventKeyframe : public MfKeyframe {
public:
    enum { STREAM_TYPE = 'yeKv' };

    MfEventKeyframe();                                      // 10020d30
    MfEventKeyframe(const MfEventKeyframe &key);
    virtual ~MfEventKeyframe();
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &stream);
    virtual MfStreamError WriteToStream(MfStream &stream) const;
    virtual MfKeyframe *CreateCopy();

    MfString mMessage;                                      // 0x0c
    int mBroadcast;                                         // 0x10 sent without a sender
};

// a texture the skill puts on one material of one node at its time
class MfTextureKeyframe : public MfKeyframe {
public:
    enum { STREAM_TYPE = 'yeKT' };

    MfTextureKeyframe();                                    // 100539b0
    MfTextureKeyframe(const MfTextureKeyframe &key);
    virtual ~MfTextureKeyframe();
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &stream);
    virtual MfStreamError WriteToStream(MfStream &stream) const;
    virtual MfKeyframe *CreateCopy();
    void SetMaterial(const MfString &texture, const MfMaterial &material);   // 10053b70
    void Apply(MfSkeletonTraversal *nodes);                 // 10053b90
    // loads the material into the renderer once (10053bc0, 10053bf0)
    void Preload();
    void Unload();

    int mMaterialIndex;                                     // 0x0c
    int mNodeIndex;                                         // 0x10
    MfMaterial mMaterial;                                   // 0x14
    void *mPreloaded;                                       // 0xe0 the renderer's handle
};

// the texture keyframes of one material of one node
class MfTextureKeyList : public MfKeyList {
public:
    enum { STREAM_TYPE = 'TxKL' };

    MfTextureKeyList();                                     // 10053d40
    MfTextureKeyList(const MfTextureKeyList &list);         // 10053dc0
    virtual ~MfTextureKeyList();
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &stream);
    virtual MfStreamError WriteToStream(MfStream &stream) const;
    void InitMaterials();                                   // 10053e90

    MfString mName;                                         // 0x14
    int mMaterialIndex;                                     // 0x18
    int mNodeIndex;                                         // 0x1c -1: none
};

#endif
