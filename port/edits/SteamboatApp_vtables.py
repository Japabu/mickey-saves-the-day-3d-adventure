# SteamboatApp edits that make its views of other modules' classes agree with the
# owners' vtables (port/tools/vtablecensus.py).
EDITS = {
    # PiccoloRT's MfScDataObject: slot 0x2c is GetPropTokenTable (MfScDataObject.h in
    # PiccoloRT); the view guessed a second FilloutPropertyNameList there
    'SteamboatApp/MfScDataObject.h': [
        ('    virtual void FilloutPropertyNameList(MfStringList *, void *); // vftable+0x2c\n',
         '    virtual class MfScPropTokenTable *GetPropTokenTable(); // vftable+0x2c\n', 1),
    ],
    # DatabaseMgr::Database (MBEngine's SimpleNode.cpp and SteamboatApp's copy): the
    # resource file pointer, then the reference count that AddDBRef counts
    'SteamboatApp/DatabaseMgr.h': [
        ('        int field_0;\n        int field_4;\n', '        void *field_0;\n        int field_4;\n', 1),
    ],
}
