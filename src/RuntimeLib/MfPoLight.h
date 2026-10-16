// MfPoLight (MfPoLibrary.dll): the Piccolo object of a light node.
#ifndef RUNTIMELIB_MFPOLIGHT_H
#define RUNTIMELIB_MFPOLIGHT_H
#include "MfPoActorNode.h"
class MfLight;

class __declspec(dllimport) MfPoLight : public MfPoActorNode {
public:
    MfPoLight(MfAiActorNode *node, MfLight *light);
    MfPoLight(const MfPoLight &);
    virtual ~MfPoLight();
    MfPoLight &operator=(const MfPoLight &);
    virtual MfPropertyErrorCode GetProperty(const MfString &, MfScDataHandle **);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *, const MfString &, MfScDataHandle *, int);
private:
    virtual MfString GetClassName();
    virtual MfScPropTokenTable *GetPropTokenTable();
    virtual void FilloutPropertyNameList(MfStringList *);
public:
    void SetNodeDeleted()
    {
        mLightNode = (MfAiActorNode *)-1;
        mActorNode = (MfAiActorNode *)-1;
    }

    MfLight *mLight;                                        // 0x18
    MfAiActorNode *mLightNode;                              // 0x1c
};

#endif
