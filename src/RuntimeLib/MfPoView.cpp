// MfPoView (RuntimeLib.dll 0x100738c0-0x10075920): the Piccolo object "View",
// the script side of a render view (MfRenderView): bounds, scene, z order,
// camera node and the option menus of the view event handler.
#include "MfPoView.h"
#include "MfAiActorNode.h"
#include "MfCameraNode.h"
#include "MfLiveActor.h"
#include "MfPoActor.h"
#include "MfPoActorNode.h"
#include "MfPoSpace.h"
#include "MfRenderView.h"
#include "MfRenderer.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScObjString.h"
#include "MfScPropTokenTable.h"
#include "MfScPropertyExtList.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"
#include "MfViewEventHandler.h"
#include <typeinfo.h>

// 0x100738c0 _$E59
// 0x100738d0 _$E56
// 0x100738f0 _$E58
// 0x10073900 _$E57
// (the unguarded atexit destructor shows a file static, not a static member)
static MfString strClassName("View");

// 0x10073910 ?GetClassNameA@MfPoView@@UAE?AVMfString@@XZ (folded)
MfString MfPoView::GetClassName()
{
    return strClassName;
}

// 0x10073940 ??0MfPoView@@AAE@VExemplar@MfScData@@@Z
MfPoView::MfPoView(MfScData::Exemplar) : MfPoBase(0), mView(0)
{
    mFlags = (mFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    s_classObject = this;

    MfScClassInfo *ci = new MfScClassInfo("View");
    MfScCiMethod *m;
    SetProperty(0, "SetBounds", MfScDataHandle::CreateData((MfScMethod)&MfPoView::SetBounds, "View::SetBounds", -1), 1);
    m = new MfScCiMethod("SetBounds");
    *ci + m;
    *m + new MfScMethodParameterInfo("int left", 0);
    *m + new MfScMethodParameterInfo("int top", 0);
    *m + new MfScMethodParameterInfo("int right", 0);
    *m + new MfScMethodParameterInfo("int bottom", 0);
    SetProperty(0, "View", MfScDataHandle::CreateData((MfScMethod)&MfPoView::Constructor, "View::View", 1), 1);
    m = new MfScCiMethod("View");
    *ci + m;
    *m + new MfScMethodParameterInfo("String viewName", 0);
    SetProperty(0, "Delete", MfScDataHandle::CreateData((MfScMethod)&MfPoView::Delete, "View::Delete", 0), 1);
    m = new MfScCiMethod("Delete");
    *ci + m;
    SetProperty(0, "SetSpace", MfScDataHandle::CreateData((MfScMethod)&MfPoView::SetSpace, "View::SetSpace", -1), 1);
    m = new MfScCiMethod("SetSpace");
    *ci + m;
    *m + new MfScMethodParameterInfo("Space space", 0);
    SetProperty(0, "StartOptions", MfScDataHandle::CreateData((MfScMethod)&MfPoView::StartOptions, "View::StartOptions", 1), 1);
    m = new MfScCiMethod("StartOptions");
    *ci + m;
    *m + new MfScMethodParameterInfo("Actor targetActor", 0);
    SetProperty(0, "EndOptions", MfScDataHandle::CreateData((MfScMethod)&MfPoView::EndOptions, "View::EndOptions", 0), 1);
    m = new MfScCiMethod("EndOptions");
    *ci + m;
    SetProperty(0, "AddOption", MfScDataHandle::CreateData((MfScMethod)&MfPoView::AddOption, "View::AddOption", 2), 1);
    m = new MfScCiMethod("AddOption");
    *ci + m;
    *m + new MfScMethodParameterInfo("int optionID", 0);
    *m + new MfScMethodParameterInfo("String optionName", 0);
    SetProperty(0, "DisplayText", MfScDataHandle::CreateData((MfScMethod)&MfPoView::DisplayText, "View::DisplayText", 1), 1);
    *ci + new MfScCiProperty("isVisible - int");
    *ci + new MfScCiProperty("iZOrder - int");
    *ci + new MfScCiProperty("screenWidth - int (read only)");
    *ci + new MfScCiProperty("screenHeight - int (read only)");
    *ci + new MfScCiProperty("cameraNode - CameraNode");

    s_tokenTable = new MfScPropTokenTable;
    s_tokenTable->Add("isVisible", 0);
    s_tokenTable->Add("iZOrder", 1);
    s_tokenTable->Add("screenWidth", 2);
    s_tokenTable->Add("screenHeight", 3);
    s_tokenTable->Add("cameraNode", 4);
    MfScDataObject::AddClassInfo(ci);
    s_extensions = new MfScPropertyExtList;
}

MfPoView *MfPoView::s_classObject;
MfScPropertyExtList *MfPoView::s_extensions;
MfScPropTokenTable *MfPoView::s_tokenTable;

// 0x10074b30 ?GetPropTokenTable@MfPoView@@UAEPAVMfScPropTokenTable@@XZ
MfScPropTokenTable *MfPoView::GetPropTokenTable()
{
    return s_tokenTable;
}

// 0x10074b40 ?GetPropertyExtensions@MfPoView@@UAEPAVMfScPropertyExtList@@XZ
MfScPropertyExtList *MfPoView::GetPropertyExtensions()
{
    return s_extensions;
}

// 0x10074b50 ??_GMfPoView@@UAEPAXI@Z (compiler-generated)

// EQUIV 0x10074b70 ??0MfPoView@@QAE@PAVMfRenderView@@@Z: reviewed: register allocation: this lives in ebp vs esi and the imported function pointers in esi vs edi; the same calls in the same order
MfPoView::MfPoView(MfRenderView *view) : MfPoBase(0), mView(view)
{
    if (!view) {
        MfString message("View.Constructor()   The view is invalid.");
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->ThrowException(15, (char *)message);
    }
}

// 0x10074c30 ??1MfPoView@@UAE@XZ
MfPoView::~MfPoView()
{
    if (this == s_classObject) {
        delete s_tokenTable;
        s_tokenTable = 0;
        if (this == s_classObject) {
            delete s_extensions;
            s_extensions = 0;
            if (this == s_classObject)
                s_classObject = 0;
        }
    }
}

// 0x10074cd0 ?CreateClassObject@MfPoView@@SAXXZ
void MfPoView::CreateClassObject()
{
    if (!s_classObject) {
        MfScData::Exemplar exemplar;
        s_classObject = new MfPoView(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(s_classObject);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

// 0x10074d60 ?Constructor@MfPoView@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// View(name): the view of that name; a new 64x64 view on the first view's
// window if there is none.
MfScDataHandle *MfPoView::Constructor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    int width, height;
    MfDblListItem *pos;
    MfRenderView *view;
    if (args[0] && args[0]->IsType(0x100)) {
        MfString name(*dynamic_cast<MfScObjString *>(args[0]->GetData()));
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
        for (view = (MfRenderView *)(&MfRenderer::Instance()->mViews)->GetFirst(pos); view;
             view = (MfRenderView *)(&MfRenderer::Instance()->mViews)->GetNext(pos)) {
            if (view->mPiccoloViewName == name) {
                result = view->GetPiccoloHandle();
                return result;
            }
        }
        HWND__ *window = ((MfRenderView *)(&MfRenderer::Instance()->mViews)->GetFirst(pos))->mWindow;
        tagRECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = 64;
        rect.bottom = 64;
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
        view = MfRenderer::Instance()->CreateView(window, rect, width, height);
        view->ShowCameraName(false);
        view->ShowFrameRate(false);
        view->mPiccoloViewName = name;
        result = view->GetPiccoloHandle();
    }
    return result;
}

// 0x10074ee0 ?Delete@MfPoView@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// Delete(): destroys the view (not the default view, which has no name).
MfScDataHandle *MfPoView::Delete(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mView, "View", "Delete"))
        return 0;
    if (mView->mPiccoloViewName == NULL_STRING) {
        BadParameters("View.Delete()  The default view can not be deleted.");
        return MfScDataHandle::CreateData(0);
    }
    MfRenderer::Instance()->DestroyView(mView);
    return MfScDataHandle::CreateData(1);
}

// 0x10074fc0 ?SetBounds@MfPoView@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// SetBounds(left, top, right, bottom)
MfScDataHandle *MfPoView::SetBounds(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!InvalidObject(mView, "View", "SetBounds")) {
        if (mView->mPiccoloViewName == NULL_STRING) {
            BadParameters("View.SetBounds()  The default view size can not be edited.");
        } else if (!CheckNull("View.SetBounds()", nArgs, args, 0)) {
            if (args[0]->IsType(0x10) && args[1]->IsType(0x10) && args[2]->IsType(0x10) && args[3]->IsType(0x10)) {
                mView->SetBounds(args[0]->mLong, args[1]->mLong, args[2]->mLong, args[3]->mLong);
                return MfScDataHandle::CreateData(1);
            }
            InvalidType("View", "SetBounds", 0, nArgs, args, &typeid(int), &typeid(int), &typeid(int), &typeid(int));
        }
    }
    return MfScDataHandle::CreateData(0);
}

// 0x10075140 ?SetSpace@MfPoView@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// SetSpace(space): the view shows the space's render scene.
MfScDataHandle *MfPoView::SetSpace(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mView, "View", "SetSpace"))
        return MfScDataHandle::CreateData(0);
    if (CheckNull("View.SetSpace()", nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(0x200)) {
        MfPoSpace *space = args[0] ? dynamic_cast<MfPoSpace *>(args[0]->GetData()) : 0;
        mView->SetScene(space->mSpace->GetRenderScene());
        MfRenderer::Instance()->RenderAllViews();
        return MfScDataHandle::CreateData(1);
    }
    InvalidType("View", "SetSpace", 0, nArgs, args, &typeid(MfPoSpace));
    return MfScDataHandle::CreateData(0);
}

static int bDestroyed;

// 0x10075290 ?DestroyClassObject@MfPoView@@SAXXZ
void MfPoView::DestroyClassObject()
{
    if (!bDestroyed) {
        bDestroyed = 1;
        MfScDataHandle *handle;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->mFlags &= ~8;
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            handle->Release();
        }
    }
}

// 0x100752f0 ?GetProperty@MfPoView@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoView::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    if (mView == (MfRenderView *)-1)
        return (MfPropertyErrorCode)5;
    *value = 0;
    if (GetPropertyExt(name, value))
        return (MfPropertyErrorCode)0;
    int token = LookupPropertyToken(name, s_tokenTable);
    if (this == s_classObject && token != 2 && token != 3) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (token) {
        case 0: {   // isVisible
            bool visible = mView->mVisible;
            *value = MfScDataHandle::CreateData(visible);
            break;
        }
        case 1:     // iZOrder
            *value = MfScDataHandle::CreateData(mView->mZOrder);
            break;
        case 2:     // screenWidth
            *value = MfScDataHandle::CreateData(GetSystemMetrics(SM_CXSCREEN));
            break;
        case 3:     // screenHeight
            *value = MfScDataHandle::CreateData(GetSystemMetrics(SM_CYSCREEN));
            break;
        case 4: {   // cameraNode
            MfCameraNode *camera = static_cast<MfCameraNode *>(mView->mCameraNode);
            if (camera)
                *value = camera->GetPiccoloHandle();
            break;
        }
        default:
            err = s_classObject->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x10075430 ?SetProperty@MfPoView@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoView::SetProperty(MfScDataHandle *self, const MfString &name,
                                                          MfScDataHandle *value, int release)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    if (mView == (MfRenderView *)-1)
        return (MfPropertyErrorCode)5;
    if (this == s_classObject)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        switch (LookupPropertyToken(name, s_tokenTable)) {
        case 0:     // isVisible
            if (value->IsType(0x10))
                mView->Show(value->mLong != 0);
            else
                err = (MfPropertyErrorCode)4;
            break;
        case 1:     // iZOrder (not of the default view)
            if (mView->mPiccoloViewName != NULL_STRING && value->IsType(0x10))
                MfRenderer::Instance()->SetZOrder(mView, value->mLong);
            else
                err = (MfPropertyErrorCode)4;
            break;
        case 4: {   // cameraNode
            MfPoActorNode *camera;
            if (value->IsType(0x200) &&
                (camera = dynamic_cast<MfPoActorNode *>(value->GetData())) != 0 &&
                camera->mActorNode->GetNodeType() == MF_NODE_CAMERA) {
                static_cast<MfCameraNode *>(camera->mActorNode)->AttachView(mView);
                MfRenderer::Instance()->RenderAllViews();
            } else
                err = (MfPropertyErrorCode)4;
            break;
        }
        default:
            err = (MfPropertyErrorCode)2;
            break;
        }
    } else
        err = (MfPropertyErrorCode)4;
    if (release && value)
        value->Release();
    return err;
}

// 0x10075590 ?StartOptions@MfPoView@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// StartOptions(actor): opens the option menu of the view for the actor.
MfScDataHandle *MfPoView::StartOptions(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!InvalidObject(mView, "View", "StartOptions")) {
        if (args[0] && args[0]->IsType(0x200)) {
            MfPoActor *actor = args[0] ? dynamic_cast<MfPoActor *>(args[0]->GetData()) : 0;
            actor->mActor->SetOptionTarget(actor->mActor);
            g_viewEventHandler->StartOptions(mView, static_cast<MfLiveActor *>(actor->mActor));
            return MfScDataHandle::CreateData(1);
        }
    }
    return MfScDataHandle::CreateData(0);
}

// 0x10075680 ?AddOption@MfPoView@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// AddOption(id, name): an entry of the open option menu; choosing it calls
// the target actor's HandleOption(id).
MfScDataHandle *MfPoView::AddOption(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull("View.AddOption()", nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (args[0]->IsType(0x10) && args[1]->IsType(0x100)) {
        int option = args[0]->mLong;
        MfScObjString *text = args[1] ? dynamic_cast<MfScObjString *>(args[1]->GetData()) : 0;
        g_viewEventHandler->AddOption(text, &MfLiveActor::HandleOption, option, 1, 0);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType("View", "AddOption", 0, nArgs, args, &typeid(int), &typeid(MfScObjString));
    return MfScDataHandle::CreateData(0);
}

// 0x100757a0 ?EndOptions@MfPoView@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoView::EndOptions(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mView, "View", "EndOptions"))
        return 0;
    g_viewEventHandler->EndOptions();
    return MfScDataHandle::CreateData(1);
}

// 0x10075840 ?DisplayText@MfPoView@@QAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// DisplayText(text): does nothing but answer 1 (an unused local string).
MfScDataHandle *MfPoView::DisplayText(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mView, "View", "DisplayText"))
        return MfScDataHandle::CreateData(0);
    MfString text;
    return MfScDataHandle::CreateData(1);
}

// 0x10075900 ?FilloutPropertyNameList@MfPoView@@UAEXPAVMfStringList@@@Z
void MfPoView::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, s_tokenTable);
}
