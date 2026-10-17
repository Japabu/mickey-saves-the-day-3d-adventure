// MfOptionHookData: the data of the 'VOpt' view hook: an option of a render
// view's popup menu was chosen (MfRenderView builds the menu, the
// MfViewEventHandler handles the choice).
#ifndef RUNTIMELIB_MFOPTIONHOOKDATA_H
#define RUNTIMELIB_MFOPTIONHOOKDATA_H
#include "MfHookManager.h"
class MfRenderView;

struct MfOptionHookData : public MfHookData {
    MfRenderView *view;                                     // 0x10
    int option;                                             // 0x14
};

#endif
