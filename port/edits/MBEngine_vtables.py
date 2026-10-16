# MBEngine edits that make unit views agree with the complete declarations
# (port/tools/vtablecensus.py).
EDITS = {
    # IMBFileSpec's slot +0x2c is Hash() (MBFileUtils.h and the other views); this
    # view named it GetID
    'MBEngine/CMBFileStreamLocator.h': [
        ('    virtual void v28(void) = 0;\n    virtual unsigned long GetID(void) = 0;\n};',
         '    virtual void v28(void) = 0;\n    virtual unsigned long Hash(void) const = 0;\n};', 1),
    ],
    'MBEngine/CMBFileStreamLocator.cpp': [
        ('        id = mFile->GetID();\n', '        id = mFile->Hash();\n', 1),
    ],
    # the fonts' views of Squish's frame and movie: their first member is an MBVector (a
    # std::vector, 24 bytes here, 16 in VC6's library): the sizes below it were 8 off
    'MBEngine/CMBFont.h': [
        ('    void *mBlocks[4];\n', '    alignas(8) char mBlocks[24];    /* port: MBVector<SquishImageBlock *> */\n', 1),
        ('    void *mFrames[4];\n', '    alignas(8) char mFrames[24];    /* port: MBVector<SquishImageFrame *> */\n', 1),
    ],
    'MBEngine/CMBUniFont.h': [
        ('    void *mBlocks[4];\n', '    alignas(8) char mBlocks[24];    /* port: MBVector<SquishImageBlock *> */\n', 1),
        ('    void *mFrames[4];\n', '    alignas(8) char mFrames[24];    /* port: MBVector<SquishImageFrame *> */\n', 1),
    ],
}
