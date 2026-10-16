# RuntimeLib edits that give its copies of other modules' classes the owners' data
# layout (port/tools/layoutcensus.py). Applied after RuntimeLib_imports.py.
EDITS = {
    # MfActorLib's MfProtoActor: maxProxDef is an MfProtoProxDef and listCompiledBhvrs an
    # MfStringList (40 bytes each here, 28 and 20 on Windows): with char arrays of the
    # Windows sizes, instances (0xe8) and everything after came out early
    'RuntimeLib/MfProtoActor.h': [
        ('class MfProtoProxDef;\n', '#include "MfProtoProxDef.h"\n', 1),
        ('class MfStringList;\n', '#include "MfStringList.h"\n', 1),
        ('    char maxProxDef[0x1c];                                  // 0x6c MfProtoProxDef\n',
         '    MfProtoProxDef maxProxDef;                              // 0x6c\n', 1),
        ('    char listCompiledBhvrs[0x14];                           // 0xb4 MfStringList\n',
         '    MfStringList listCompiledBhvrs;                         // 0xb4\n', 1),
        ('    MfProtoProxDef &MaxProximityDef() { return *(MfProtoProxDef *)maxProxDef; }',
         '    MfProtoProxDef &MaxProximityDef() { return maxProxDef; }', 1),
    ],
    # MfActorLib's MfTriangleSet: the source, vertex, normal, colour, texture vertex,
    # triangle, material index and material members are pointers
    'RuntimeLib/MfTriangleSet.h': [
        ('    int m10, m14, m18, m1c, m20, m24;                       // 0x10\n',
         '    void *m10; int m14; void *m18, *m1c, *m20, *m24;        // 0x10\n', 1),
        ('    int m30, m34;                                           // 0x30\n',
         '    void *m30, *m34;                                        // 0x30\n', 1),
        ('    int m3c;                                                // 0x3c\n',
         '    void *m3c;                                              // 0x3c\n', 1),
    ],
    # PiccoloRT's MfScPiccoloObjectFile (its override MfScPiccoloObjectFile.h): RuntimeLib
    # makes these on the stack; it reads the error code at 0x04 and the open flag (mStatus) at 0x1c
    'RuntimeLib/MfScPiccoloObjectFile.h': [
        ('    int mError;                                             // 0x04 0 when the file opened; RuntimeLib loads one only then\n'
         '    char m08[0x14];                                         // 0x08\n'
         '    int mStatus;                                            // 0x1c mOpen: RuntimeLib writes the file only when it is set\n'
         '    char m20[0x18];                                         // 0x20\n',
         '    int mError;                                             // 0x04 0 when the file opened; RuntimeLib loads one only then\n'
         '    unsigned int mSystemError; int mWrite; int mType; unsigned int mOptions; MfString mName; // 0x08\n'
         '    int mStatus;                                            // 0x1c mOpen: RuntimeLib writes the file only when it is set\n'
         '    void *mFile, *mMapping; char *mBase; void *mLists; void *mModules; MfScPiccoloObjectFile *mDebugFile; // 0x20\n', 1),
    ],
    # MfActorLib's MfCamera: +0x20 is an MfPropertyList (40 bytes here, 20 on Windows);
    # MfCameraNode embeds an MfCamera that MfActorLib's constructor fills
    'RuntimeLib/MfCamera.h': [
        ('    int field_20;\n    char unknown_24[16];                                    // size 0x34 (new MfSpaceView)\n',
         '    alignas(8) char field_20[40];                           // 0x20 MfPropertyList (MfActorLib)\n'
         '    // (size 0x34 on Windows, new MfSpaceView)\n', 1),
    ],
}
