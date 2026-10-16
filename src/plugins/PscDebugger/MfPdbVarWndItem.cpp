// PscDebugger.dll 0x10009890-0x1000ad7f: the lines of the variable display:
// a variable, the fields of an object or the elements of an array.
#include "PscDebugger.h"

// The longest arrays shown completely.
enum { kMaxElements = 200 };

// 0x10009890 ??0MfPdbVarWndItem@@QAE@ABVMfString@@PAVMfScDataHandle@@KPAVMfPdbVarDispWnd@@HPAV0@@Z
MfPdbVarWndItem::MfPdbVarWndItem(const MfString &name, MfScDataHandle *value, unsigned long flags,
                                 MfPdbVarDispWnd *wnd, int depth, MfPdbVarWndItem *parent)
    : mName(name), mLines(1), mIsString(0), mFlags(flags), mDepth(depth), mParent(parent)
{
    mHandle = value;
    if (mHandle)
        mHandle->AddRef();
    mChildren = new MfDblList;
    mFlags |= kCollapsed;
    UpdateFlags();
    mValue = FormatValue(&mIsString);
    if (mIsString) {
        int chars = ValueChars(wnd);
        if (mValue.Length() > chars) {
            mLines = mValue.Length() / chars;
            if (mValue.Length() % chars)
                mLines++;
            if (mLines > 3)
                mLines = 3;
        }
    }
}

// 0x100099d0 ??_GMfPdbVarWndItem@@UAEPAXI@Z

// 0x100099f0 ??1MfPdbVarWndItem@@UAE@XZ
MfPdbVarWndItem::~MfPdbVarWndItem()
{
    if (mHandle)
        mHandle->Release();
    if (mChildren) {
        VwiListItemInterface lii;
        mChildren->Clear(lii);
        delete mChildren;
    }
}

// 0x10009a90 ?Destroy@VwiListItemInterface@MfPdbVarWndItem@@UAEXPAX@Z
void MfPdbVarWndItem::VwiListItemInterface::Destroy(void *item)
{
    delete (MfPdbVarWndItem *)item;
}

// 0x10009ab0 ?Matches@VwiListItemInterface@MfPdbVarWndItem@@UAEHPAX0@Z
int MfPdbVarWndItem::VwiListItemInterface::Matches(void *name, void *item)
{
    return *(MfString *)name == ((MfPdbVarWndItem *)item)->mName ? 1 : 0;
}

// EQUIV 0x10009ad0 ?Expand@MfPdbVarWndItem@@QAEXPAVMfPdbVarDispWnd@@@Z: reviewed: register choice only (names.First() lands in eax and is copied to esi in the original); the unwind funclets differ only in the scratch slot of the base-pointer temporary
void MfPdbVarWndItem::Expand(MfPdbVarDispWnd *wnd)
{
    mFlags = mFlags & ~kCollapsed | kExpanded;
    if (!mHandle || !(mFlags & kHasChildren))
        return;
    if (mHandle->IsType(0x400)) {
        MfScDataArray *array = (MfScDataArray *)mHandle->GetData();
        int count = array->mCount;
        if (count > kMaxElements)
            count = kMaxElements;
        for (int i = 0; i < count; i++) {
            MfScDataHandle *element = 0;
            array->GetAt(i, &element, 1);
            MfString name = MfString("[") + MfString(i) + MfString("]");
            MfPdbVarWndItem *item = new MfPdbVarWndItem(name, element, 0, wnd, mDepth + 1, this);
            mChildren->Add(item);
            if (element)
                element->Release();
        }
    } else {
        MfScDataObject *object = dynamic_cast<MfScDataObject *>(mHandle->GetData());
        if (object) {
            MfStringList names;
            object->GetPropertyNames(&names, 0);
            object->GetInstanceProperties(&names, 0);
            names.Sort(0);
            MfDblListItem *pos;
            MfString *name = names.First(pos);
            while (name) {
                MfScDataHandle *value = 0;
                int error = object->GetProperty(*name, &value);
                int found = 0;
                if (error == 0)
                    found = 1;
                MfPdbVarWndItem *item = new MfPdbVarWndItem(*name, value, !found, wnd, mDepth + 1, this);
                mChildren->Add(item);
                if (value)
                    value->Release();
                name = names.Next(pos);
            }
        }
    }
}

// 0x10009e30 ?Collapse@MfPdbVarWndItem@@QAEXXZ
void MfPdbVarWndItem::Collapse()
{
    VwiListItemInterface lii;
    mFlags = mFlags & ~kExpanded | kCollapsed;
    mChildren->Clear(lii);
}

// 0x10009e60 ?UpdateFlags@MfPdbVarWndItem@@QAEXXZ
void MfPdbVarWndItem::UpdateFlags()
{
    if ((mFlags & kNoChildren) || (mFlags & (kNotFound | kError)) || !mHandle)
        return;
    if (mHandle->IsType(0x400)) {
        mFlags |= kHasChildren;
        return;
    }
    MfScDataObject *object = dynamic_cast<MfScDataObject *>(mHandle->GetData());
    if (object) {
        MfStringList names;
        object->GetPropertyNames(&names, 0);
        object->GetInstanceProperties(&names, 1);
        if (names.GetCount())
            mFlags |= kHasChildren;
        else
            mFlags &= ~kHasChildren;
    }
}

// EQUIV 0x10009fb0 ?FormatValue@MfPdbVarWndItem@@QAE?AVMfString@@PAH@Z: reviewed: stack slots of the string temporaries (the original keeps one in the dead isString argument slot) and mFlags read as a dword instead of a byte before the same test al, 3
MfString MfPdbVarWndItem::FormatValue(int *isString)
{
    *isString = 0;
    if ((mFlags & (kNotFound | kError)) == 0) {
        if (mHandle) {
            if (mHandle->IsType(0x200)) {
                MfScDataObject *object = dynamic_cast<MfScDataObject *>(mHandle->GetData());
                if (object)
                    return MfString("{") + object->GetClassName() + MfString("}");
                return "{classname}";
            }
            MfString info;
            mHandle->InfoShort(&info);
            if (mHandle->IsType(0x100)) {
                *isString = 1;
                return MfString("\"") + info + MfString("\"");
            }
            if (mHandle->IsType(0xc0)) {
                MfScDataObject *object = (MfScDataObject *)mHandle->GetData();
                return MfString("{") + object->GetClassName() + MfString(" value=") + info + MfString("}");
            }
            if (mHandle->IsType(0x400)) {
                MfScDataArray *array = (MfScDataArray *)mHandle->GetData();
                return MfString("{Array size=") + MfString(array->mCount) + MfString("}");
            }
            return info;
        }
        return "{nil}";
    }
    return "VEXERR: cannot evaluate";
}

// 0x1000a380 ?NameChars@MfPdbVarWndItem@@QAEHPAVMfPdbVarDispWnd@@@Z
int MfPdbVarWndItem::NameChars(MfPdbVarDispWnd *wnd)
{
    int width = wnd->mNameWidth;
    return (width - Indent()) / wnd->mCharWidth;
}

// 0x1000a3b0 ?ValueChars@MfPdbVarWndItem@@QAEHPAVMfPdbVarDispWnd@@@Z
int MfPdbVarWndItem::ValueChars(MfPdbVarDispWnd *wnd)
{
    CRect rect;
    wnd->GetClientRect(&rect);
    return (rect.right - wnd->mNameWidth - rect.left - 4) / wnd->mCharWidth;
}

// 0x1000a3f0 ?DrawGlyph@MfPdbVarWndItem@@QAEXPAVCDC@@VCRect@@PAVMfPdbVarDispWnd@@@Z
void MfPdbVarWndItem::DrawGlyph(CDC *dc, CRect rect, MfPdbVarDispWnd *wnd)
{
    if (mFlags & kHasChildren) {
        CDC memDC;
        memDC.CreateCompatibleDC(dc);
        CBitmap *old = 0;
        if (mFlags & kCollapsed)
            old = memDC.SelectObject(&wnd->mPlus);
        else if (mFlags & kExpanded)
            old = memDC.SelectObject(&wnd->mMinus);
        int x = GlyphLeft();
        int y = rect.top + (wnd->mLineHeight - kGlyphSize) / 2;
        dc->BitBlt(x, y, kGlyphSize, kGlyphSize, &memDC, 0, 0, SRCCOPY);
        memDC.SelectObject(old);
    } else {
        COLORREF background = dc->GetBkColor();
        rect.left = GlyphLeft();
        rect.right = rect.left + kGlyphSize;
        rect.top += (wnd->mLineHeight - kGlyphSize) / 2;
        rect.bottom = rect.top + kGlyphSize;
        dc->FillSolidRect(&rect, background);
    }
}

// EQUIV 0x1000a5b0 ?Draw@MfPdbVarWndItem@@QAEXPAVMfPdbVarDispWnd@@PAVCDC@@HPAUtagRECT@@@Z: reviewed: rect and cell swap stack slots, register choice (the original spills the guide line's x and reloads mParent->mDepth), operand order of two integer sums; same calls, arguments and order
void MfPdbVarWndItem::Draw(MfPdbVarDispWnd *wnd, CDC *dc, int, LPRECT out)
{
    UpdateFlags();
    CRect rect;
    if (!wnd->GetItemRect(this, &rect))
        return;
    *out = rect;
    CRect nameRect = rect;
    CRect valueRect = rect;
    int nameWidth = wnd->mNameWidth;
    COLORREF background = dc->GetBkColor();
    COLORREF foreground = dc->GetTextColor();
    if (mFlags & kSelected) {
        dc->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
        dc->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
    }
    int header = wnd->mHeaderHeight;
    if (rect.top >= header) {
        CRect cell;
        cell.left = nameRect.left;
        cell.top = nameRect.top;
        cell.right = nameWidth;
        cell.bottom = nameRect.bottom;
        dc->FillSolidRect(&cell, dc->GetBkColor());
        dc->DrawFocusRect(&cell);
        cell.right--;
        cell.bottom--;
        dc->FillSolidRect(&cell, dc->GetBkColor());
        DrawGlyph(dc, rect, wnd);
        MfString name(mName);
        int chars = NameChars(wnd);
        if (name.Length() > chars)
            name.Truncate(chars);
        if (name.Length() > 0) {
            int indent = Indent();
            int offset = mDepth ? 16 : 22;
            dc->TextOut(rect.left + offset + indent, rect.top, name, name.Length());
        }
        cell.left = nameWidth;
        cell.top = valueRect.top;
        cell.right = valueRect.right;
        cell.bottom = valueRect.bottom;
        dc->FillSolidRect(&cell, dc->GetBkColor());
        dc->DrawFocusRect(&cell);
        cell.right--;
        cell.bottom--;
        dc->FillSolidRect(&cell, dc->GetBkColor());
    } else if (rect.bottom >= header) {
        CRect cell;
        cell.left = nameRect.left;
        cell.top = header;
        cell.right = nameWidth;
        cell.bottom = nameRect.bottom;
        dc->FillSolidRect(&cell, dc->GetBkColor());
        dc->DrawFocusRect(&cell);
        cell.right--;
        cell.bottom--;
        dc->FillSolidRect(&cell, dc->GetBkColor());
        cell.left = nameWidth;
        cell.top = header;
        cell.right = valueRect.right;
        cell.bottom = valueRect.bottom;
        dc->FillSolidRect(&cell, dc->GetBkColor());
        dc->DrawFocusRect(&cell);
        cell.right--;
        cell.bottom--;
        dc->FillSolidRect(&cell, dc->GetBkColor());
    }
    if (mLines > 1) {
        int chars = ValueChars(wnd);
        MfString text(chars);
        for (int i = 0; i < mLines; i++) {
            int y = i * wnd->mLineHeight + rect.top;
            if (y >= wnd->mHeaderHeight) {
                text = mValue.MidString(i * chars, chars);
                if (i == 2 && text.Length() == chars) {
                    text.Truncate(text.Length() - 3);
                    text << "...";
                }
                int x = wnd->mNameWidth;
                dc->TextOut(x + 4, y, text, text.Length());
            }
        }
    } else if (rect.top >= wnd->mHeaderHeight) {
        int x = wnd->mNameWidth;
        dc->TextOut(x + 4, rect.top, mValue, mValue.Length());
    }
    dc->SetBkColor(background);
    dc->SetTextColor(foreground);
    if (mParent && rect.top >= wnd->mHeaderHeight) {
        int top = wnd->mHeaderHeight;
        CRect parentRect;
        if (wnd->GetItemRect(mParent, &parentRect) == 1 && parentRect.top >= wnd->mHeaderHeight)
            top = parentRect.top + (wnd->mLineHeight + kGlyphSize) / 2;
        CPen pen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
        CPen *oldPen = dc->SelectObject(&pen);
        int x = mParent->GlyphLeft() + 5;
        int y = wnd->mLineHeight / 2 + rect.top;
        int right = GlyphLeft() + ((mFlags & kHasChildren) ? 0 : 5);
        dc->MoveTo(x, top);
        dc->LineTo(x, y);
        dc->LineTo(right, y);
        dc->SelectObject(oldPen);
    }
}

// 0x1000ac40 ?HitTest@MfPdbVarWndItem@@QAEHVCPoint@@PAVMfPdbVarDispWnd@@@Z
int MfPdbVarWndItem::HitTest(CPoint point, MfPdbVarDispWnd *wnd)
{
    int hit = kHitNothing;
    CRect rect;
    if (!wnd->GetItemRect(this, &rect))
        return hit;
    if (PtInRect(&rect, point) && point.y >= wnd->mHeaderHeight)
        hit = kHitLine;
    if (mFlags & kHasChildren) {
        rect.left = GlyphLeft();
        rect.right = rect.left + kGlyphSize;
        rect.top += (wnd->mLineHeight - kGlyphSize) / 2;
        rect.bottom = rect.top + kGlyphSize;
        if (PtInRect(&rect, point)) {
            if (mFlags & kCollapsed)
                hit = kHitExpand;
            else if (mFlags & kExpanded)
                hit = kHitCollapse;
        }
    }
    return hit;
}

// 0x1000ad40 ?Invalidate@MfPdbVarWndItem@@QAEXPAVMfPdbVarDispWnd@@@Z
void MfPdbVarWndItem::Invalidate(MfPdbVarDispWnd *wnd)
{
    CRect rect;
    if (wnd->GetItemRect(this, &rect)) {
        if (rect.top < wnd->mHeaderHeight)
            rect.top = wnd->mHeaderHeight;
        wnd->InvalidateRect(&rect, FALSE);
    }
}
