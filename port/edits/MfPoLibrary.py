# MfPoLibrary edits for the port (see port/rewrite.py, EDITS).
#
# typeid(MfPlList) (MfPoActor's argument errors): PscStdlib, which defines the class,
# is a plugin MfPoLibrary does not link; with no key function here the type_info is
# emitted locally (it only names the type in a message).
EDITS = {
    'MfPoLibrary/MfScHandleList.h': [
        ('    virtual ~MfPlList();\n', '    virtual ~MfPlList() {}    // PORT: no key function here, so the type_info is local\n', 1),
    ],
}
