# RuntimeLib edits that make its copies of other modules' classes agree with the
# owner's vtable (see port/tools/vtablecensus.py).
EDITS = {
    # MfListItemInterface is header-only in the original, with default bodies
    # (PiccoloRT/MfListItemInterface.h): MfScClassInfo then declares only Matches, and
    # its primary vtable is ~, ~, GetKey, Matches as PiccoloRT emits it (RuntimeLib's
    # copy declared Copy/Iterate/Destroy too, which added three slots before Matches)
    'RuntimeLib/MfDblList.h': [
        ('    virtual void *Copy(void *pData) = 0;\n'
         '    virtual int Iterate(void *pData) = 0;\n'
         '    virtual void Destroy(void *pData) = 0;\n',
         '    virtual void *Copy(void *pData) { return 0; }\n'
         '    virtual int Iterate(void *pData) { return 1; }\n'
         '    virtual void Destroy(void *pData) {}\n', 1),
    ],
    'RuntimeLib/MfScClassInfo.h': [
        ('    // MfListItemInterface\'s inline defaults (MfDblList.h declares them pure)\n'
         '    virtual void *Copy(void *pData) { return 0; }\n'
         '    virtual int Iterate(void *pData) { return 1; }\n'
         '    virtual void Destroy(void *pData) {}\n',
         '    // (MfListItemInterface\'s Copy, Iterate and Destroy: its defaults, see\n'
         '    // MfDblList.h; declaring them here would add primary-vtable slots)\n\n\n', 1),
    ],
    # MfActorLib's MfProtoNode declares ReadFromStream, WriteToStream, StreamType in that
    # order (they get primary-vtable slots 4..6 in that order)
    'RuntimeLib/MfProtoNode.h': [
        ('    virtual long StreamType() const;\n'
         '    virtual MfStreamError ReadFromStream(MfStream &);\n'
         '    virtual MfStreamError WriteToStream(MfStream &) const;\n',
         '    virtual MfStreamError ReadFromStream(MfStream &);\n'
         '    virtual MfStreamError WriteToStream(MfStream &) const;\n'
         '    virtual long StreamType() const;\n', 1),
    ],
    # MfSkeletonNode_unit.h's view of MfStringList counts MSVC's slots: slot 0 (v00) is
    # the destructor, which takes two slots in the port's vtables
    'RuntimeLib/MfSkeletonNode_unit.h': [
        ('    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();\n',
         '    virtual ~MfStringList(); virtual void v04(); virtual void v08(); virtual void v0c();\n', 1),
    ],
}
