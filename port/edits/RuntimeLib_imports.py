# RuntimeLib edits: inline members RuntimeLib imports from other modules (see
# port/rewrite.py, EDITS).
#
# The original DLLs exported every inline member of their __declspec(dllexport)
# classes, and RuntimeLib's copies of those classes declare such members without a
# body, so the matching build calls the import. clang on macOS emits an inline only
# where it is used, so no module exports these. RuntimeLib's copies get the exporter's
# body here (as written in the exporting module's header), so RuntimeLib emits its own.

EDITS = {
    # mflinalg (MfRotation3.h, MfTransform3.h)
    'RuntimeLib/MfRotation3.h': [
        ('    MfRotation3(float, float, float, float);\n',
         '    MfRotation3(float w, float x_, float y_, float z_) { s = w; x = x_; y = y_; z = z_; }\n', 1),
        ('    static MfRotation3 __cdecl identity();\n',
         '    static MfRotation3 __cdecl identity() { return MfRotation3(); }\n', 1),
    ],
    'RuntimeLib/MfTransform3.h': [
        ('    static MfTransform3 __cdecl identity();\n',
         '    static MfTransform3 __cdecl identity() { return MfTransform3(MfRotation3::identity(), MfVector3()); }\n', 1),
        ('    void setValue(MfRotation3 const &, MfVector3 const &);\n',
         '    void setValue(MfRotation3 const &r, MfVector3 const &t) { mRotation = r; mTranslation = t; }\n', 1),
    ],
    # MfActorLib (MfProtoNode.h, MfProtoActor.h, MfColModel.h, MfFSMSpaceToBhvr.h; MfRect's
    # accessors are the plain member references)
    'RuntimeLib/MfProtoNode.h': [
        ('    void SetName(MfString);\n', '    void SetName(MfString name) { mName = name; }\n', 1),
    ],
    'RuntimeLib/MfProtoActor.h': [
        ('    MfProtoProxDef &MaxProximityDef();\n',
         '    MfProtoProxDef &MaxProximityDef() { return *(MfProtoProxDef *)maxProxDef; }\n', 1),
    ],
    'RuntimeLib/MfSphereNode.h': [
        ('    MfSphereNode();\n    ~MfSphereNode();\n',
         '    MfSphereNode() : MfNode(1) { mLeft = 0; mRight = 0; }\n    ~MfSphereNode() {}\n', 1),
    ],
    'RuntimeLib/MfFSMSpaceToBhvr.h': [
        ('    void SetSpaceName(MfString);\n', '    void SetSpaceName(MfString name) { mSpaceName = name; }\n', 1),
    ],
    'RuntimeLib/MfRect.h': [
        ('    float &minx();\n    float &maxx();\n    float &miny();\n    float &maxy();\n',
         '    float &minx() { return mMinx; }\n    float &maxx() { return mMaxx; }\n'
         '    float &miny() { return mMiny; }\n    float &maxy() { return mMaxy; }\n', 1),
    ],
    # MfPoLibrary (MfPoTransform.h)
    'RuntimeLib/MfPoTransform.h': [
        ('    MfTransform3 Transform();\n', '    MfTransform3 Transform() { return *_t3; }\n', 1),
    ],
    # mfutil (MfDirectory.h, MfDictionary.h, MfDblList.h)
    'RuntimeLib/MfDirectory.h': [
        ('    MfString GetFileName() const;\n',
         '    MfString GetFileName() const { return MfString(m_fd.cFileName); }\n', 1),
    ],
    'RuntimeLib/MfDictionary.h': [
        ('    void Clear(MfCallBack *cb = 0);\n', '    void Clear(MfCallBack *cb = 0) { ClearItems(cb); }\n', 1),
    ],
    'RuntimeLib/MfDblList.h': [
        ('    virtual int CallBack(void *a);\n'
         '    virtual int CallBack(void *a, void *b);\n'
         '    virtual int CallBack(void *a, void *b, void *c);\n',
         '    virtual int CallBack(void *a) { return 0; }\n'
         '    virtual int CallBack(void *a, void *b) { return 0; }\n'
         '    virtual int CallBack(void *a, void *b, void *c) { return 0; }\n', 1),
    ],
    # PiccoloRT (MfScClassInfo.h: the text's destructor only destroys the text)
    'RuntimeLib/MfScClassInfo.h': [
        ('    MfScClassInfoText() {}\n    virtual ~MfScClassInfoText();\n',
         '    MfScClassInfoText() {}\n    virtual ~MfScClassInfoText() {}\n', 1),
    ],
}
