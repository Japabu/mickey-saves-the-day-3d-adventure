// MfPoView: the Piccolo object of a render view ("View"). MfPoView.cpp (the
// methods, 10074b30-10075920) still declares the class in MfPoView_unit.h;
// this header is the class as MfRenderer, MfPoClasses.cpp and MfPoView_1.cpp
// (the head of the unit: class name, class object constructor) see it.
#ifndef RUNTIMELIB_MFPOVIEW_H
#define RUNTIMELIB_MFPOVIEW_H
#include "MfPoBase.h"
class MfRenderView;
class MfScPropertyExtList;

class MfPoView : public MfPoBase {
public:
    MfPoView(MfRenderView *view);                           // 10074b70
    virtual ~MfPoView();
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
    virtual MfScPropertyExtList *GetPropertyExtensions();
    static void CreateClassObject();                        // 10074cd0
    static void DestroyClassObject();                       // 10075290

    MfScDataHandle *Constructor(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *Delete(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *SetBounds(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *SetSpace(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *StartOptions(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *AddOption(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *EndOptions(MfScDataHandle *, int, MfScDataHandle **);
    MfScDataHandle *DisplayText(MfScDataHandle *, int, MfScDataHandle **);

    MfRenderView *mView;                                    // 0x14

private:
    // the class object (MfPoView.cpp's unit header calls the tag MfPoClassTag)
    MfPoView(MfScData::Exemplar);

    static MfPoView *s_classObject;                         // 100f5a3c
    static MfScPropertyExtList *s_extensions;               // 100f5a40
    static MfScPropTokenTable *s_tokenTable;                // 100f5a44
};

#endif
