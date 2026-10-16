# MfPoLibrary edits that make its views of other modules' classes agree with the
# owners' vtables (port/tools/vtablecensus.py).
EDITS = {
    # PiccoloRT's MfScClassInfo declares Matches: a primary-vtable slot after GetKey
    'MfPoLibrary/MfScClassInfo.h': [
        ('    MfScClassInfo(const MfString &name);\n    virtual ~MfScClassInfo();\n    virtual MfString GetKey() const;\n',
         '    MfScClassInfo(const MfString &name);\n    virtual ~MfScClassInfo();\n'
         '    virtual MfString GetKey() const; virtual int Matches(void *item, void *key);\n', 1),
    ],
}
