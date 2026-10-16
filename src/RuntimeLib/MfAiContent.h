// MfAiContent: the "ContentInfoI" interface of the content plugin (the data
// folder the application works with). As in
// src/plugins/PscContentInfo/PscContentInfo.h.
#ifndef RUNTIMELIB_MFAICONTENT_H
#define RUNTIMELIB_MFAICONTENT_H
#include <windows.h>
#include "MfAi.h"
#include "MfString.h"
class MfStringList;
struct MfContentDescription;
typedef void (*MfContentNotify)(int stage);

class MfAiContent : public MfAi {
public:
    virtual int SelectContent(const MfString &name) = 0;
    virtual int GetCurrentContent(MfContentDescription *desc) = 0;
    virtual int GetContent(const MfString &name, MfContentDescription *desc) = 0;
    virtual MfString GetDataPath() = 0;
    virtual int ChooseContent(HWND hParent, MfString *name) = 0;
    virtual int SetContentPath(const MfString &path) = 0;
    virtual int SaveContentInfoToPath(const MfString &path, const MfString &fileName, MfContentDescription *desc) = 0;
    virtual int CreateDataFolders() = 0;
    virtual MfContentNotify SetNotify(MfContentNotify notify) = 0;
    virtual int GetContentNames(MfStringList *names) = 0;
    virtual void Refresh() = 0;
};

#endif
