#include "MfRenderView.h"
#include "MfScDataHandle.h"
#include "MfString.h"

// the implicit copy members of the exported class
static MfRenderView &(MfRenderView::*s_assign)(const MfRenderView &) = &MfRenderView::operator=;

// 0x1001eba0 ??0MfRenderView@@QAE@ABV0@@Z (implicit copy constructor)
// 0x1001ec20 ??4MfRenderView@@QAEAAV0@ABV0@@Z (implicit assignment)

// 0x1001eb10 ?IsVisible@MfRenderView@@QAE_NXZ
bool MfRenderView::IsVisible()
{
    return mVisible;
}

// 0x1001eb20 ?GetBounds@MfRenderView@@QAEXAAH000@Z
void MfRenderView::GetBounds(int &left, int &top, int &width, int &height)
{
    left = mLeft;
    top = mTop;
    width = mWidth;
    height = mHeight;
}

// 0x1001eb50 ?GetSize@MfRenderView@@QAEXAAH0@Z
void MfRenderView::GetSize(int &width, int &height)
{
    width = mWidth;
    height = mHeight;
}

// 0x1001eb80 ?PiccoloViewName@MfRenderView@@QAEAAVMfString@@XZ
MfString & MfRenderView::PiccoloViewName()
{
    return mPiccoloViewName;
}

// 0x1001eb90 ?GetView@MfRenderView@@QAEPAVMfIRenderView@@XZ
MfIRenderView * MfRenderView::GetView()
{
    return mView;
}
