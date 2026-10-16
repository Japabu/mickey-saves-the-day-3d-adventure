#include "CMBTextControl_2.h"

// 0x100288c0 ?SetSize@CMBTextControl@@UAEXJJ@Z
void CMBTextControl::SetSize(long width, long height)
{
    mSize.cx = width;
    mSize.cy = height;
    SetDirty();
}

// 0x100288e0 ?GetSize@CMBTextControl@@UBE?AVMBSize@@XZ
MBSize CMBTextControl::GetSize() const
{
    return mSize;
}

// 0x10028950 ?Hide@CMBTextControl@@UAEJXZ
long CMBTextControl::Hide()
{
    if (!mHideCount)
        SetDirty();
    return ++mHideCount;
}

// 0x100289a0 ?GetParent@CMBTextControl@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBTextControl::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x100289c0 ?SetParent@CMBTextControl@@MAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBTextControl::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *old = mParent;
    if (old)
    {
        mParent = 0;
        old->RemoveObject(this);
    }
    mParent = newParent;
    return old;
}

// 0x10028d30 ?ListenToEvent@CMBTextControl@@UAEXJJPAX@Z
void CMBTextControl::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -1 && event == 4)
        AddCharacter(*(unsigned short *)data);
}

// 0x10028fd0 ?SetColorTransparent@CMBTextControl@@UAEXW4MBTextControlColorFlag@@@Z
void CMBTextControl::SetColorTransparent(MBTextControlColorFlag colorflag)
{
    mColorFlags ^= colorflag;
}

// 0x100291c0 ?SetType@CMBTextControl@@UAEXW4MBTextControlTypeFlag@@@Z
void CMBTextControl::SetType(MBTextControlTypeFlag type)
{
    mTextControlType = type;
}

// 0x100291d0 ?SetValidator@CMBTextControl@@UAEXPAVIMBTextValidator@@@Z
void CMBTextControl::SetValidator(IMBTextValidator *validator)
{
    mValidatorPtr = validator;
}

// 0x10029200 ?SetJustification@CMBTextControl@@UAEXW4MBTextControlJustifyFlag@@@Z
void CMBTextControl::SetJustification(MBTextControlJustifyFlag justify)
{
    mJustification = justify;
}
