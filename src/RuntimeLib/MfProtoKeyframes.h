// The keyframes of a prototype skill's extra tracks (MfActorLib.dll).
// Declarations from the SteamboatApp PDB.
#ifndef RUNTIMELIB_MFPROTOKEYFRAMES_H
#define RUNTIMELIB_MFPROTOKEYFRAMES_H
#include "MfMaterial.h"
#include "MfStreamObject.h"
#include "MfString.h"
class MfStream;

struct __declspec(dllimport) MfProtoTextureKeyframe : public MfStreamObject {
    MfProtoTextureKeyframe();
    MfProtoTextureKeyframe(const MfProtoTextureKeyframe &);
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    unsigned long time;                                     // 0x04
    int nodeIndex;                                          // 0x08
    int matIndex;                                           // 0x0c
    MfMaterial material;                                    // 0x10
};

struct __declspec(dllimport) MfProtoSoundKeyframe : public MfStreamObject {
    MfProtoSoundKeyframe();
    MfProtoSoundKeyframe(const MfProtoSoundKeyframe &);
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    unsigned long time;                                     // 0x04
    MfString filename;                                      // 0x08
    float volume;                                           // 0x0c
    int mute;                                               // 0x10
    int ambient;                                            // 0x14
    int stream;                                             // 0x18
};

struct __declspec(dllimport) MfProtoEventKeyframe : public MfStreamObject {
    MfProtoEventKeyframe();
    MfProtoEventKeyframe(const MfProtoEventKeyframe &);
    static const long STREAM_TYPE;
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;

    unsigned long time;                                     // 0x04
    MfString eventName;                                     // 0x08
    int useBroadcast;                                       // 0x0c
};

#endif
