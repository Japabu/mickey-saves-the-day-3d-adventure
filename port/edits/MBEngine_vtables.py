# MBEngine edits that make unit views agree with the complete declarations
# (port/tools/vtablecensus.py).
EDITS = {
    # the fonts' views of Squish's frame and movie: their first member is an MBVector (a
    # std::vector: three pointers in libc++, four in VC6's library), so the members below
    # it sit where the real classes have them
    'MBEngine/CMBFont.h': [
        ('    void *mBlocks[4];\n', '    alignas(void *) char mBlocks[3 * sizeof(void *)];    /* port: MBVector<SquishImageBlock *> (libc++: 3 pointers) */\n', 1),
        ('    void *mFrames[4];\n', '    alignas(void *) char mFrames[3 * sizeof(void *)];    /* port: MBVector<SquishImageFrame *> (libc++: 3 pointers) */\n', 1),
    ],
    'MBEngine/CMBUniFont.h': [
        ('    void *mBlocks[4];\n', '    alignas(void *) char mBlocks[3 * sizeof(void *)];    /* port: MBVector<SquishImageBlock *> (libc++: 3 pointers) */\n', 1),
        ('    void *mFrames[4];\n', '    alignas(void *) char mFrames[3 * sizeof(void *)];    /* port: MBVector<SquishImageFrame *> (libc++: 3 pointers) */\n', 1),
    ],
}
