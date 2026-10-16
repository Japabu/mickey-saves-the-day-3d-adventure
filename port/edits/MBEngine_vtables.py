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
}
