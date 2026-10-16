// MBEngine.dll .\MBEngineRelease\SquishPalette.obj
#include "SquishPalette.h"

// 0x10067220 $E21
// 0x10067230 $E18
// 0x10067360 $E20
// 0x10067370 $E19
static SquishPaletteManager gPaletteManager;

// 0x100674f0 ??0SquishPalette@@QAE@XZ
SquishPalette::SquishPalette()
{
    mSize = 0;
    mRefCount = 0;
    mSeed = 0;
    mEntries = 0;
}

// 0x10067500 ??1SquishPalette@@QAE@XZ
SquishPalette::~SquishPalette()
{
    Reset();
}

// 0x10067510 ?Find@SquishPalette@@SAPAV1@J@Z
SquishPalette *SquishPalette::Find(long seed)
{
    return gPaletteManager.Find(seed);
}

// 0x10067580 ?CreateSeed@SquishPalette@@SAJKPAUSquishPixel@@@Z
long SquishPalette::CreateSeed(unsigned long size, SquishPixel *pixels)
{
    long seed = 0;
    if (pixels) {
        while (size--) {
            seed += pixels->Pixel.bgra32;
            pixels++;
        }
    }
    return seed;
}

// 0x100675b0 ?Create@SquishPalette@@SAPAV1@KPAUSquishPixel@@@Z
SquishPalette *SquishPalette::Create(unsigned long size, SquishPixel *pixels)
{
    long seed = CreateSeed(size, pixels);
    SquishPalette *palette = gPaletteManager.Find(seed);
    if (!palette) {
        palette = new SquishPalette;
        if (!palette)
            return 0;
        if (!palette->Resize(size)) {
            delete palette;
            return 0;
        }
        MBmemcpy(palette->mEntries, pixels, size * sizeof(SquishPixel));
        palette->SetSeed(seed);
        gPaletteManager.Add(palette);
    }
    return palette;
}

// 0x10067730 ?Reset@SquishPalette@@IAEXXZ
void SquishPalette::Reset(void)
{
    if (mEntries != 0)
    {
        MBfree_release(mEntries);
    }
    mEntries = 0;
    mSize = 0;
}

// 0x10067760 ?Resize@SquishPalette@@IAE_NK@Z
bool SquishPalette::Resize(unsigned long newSize)
{
    Reset();
    if (newSize) {
        mEntries = new SquishPixel[newSize];
        if (mEntries)
            mSize = newSize;
    }
    return mSize == newSize;
}

// 0x100677f0 ?SetPixel@SquishPalette@@QAEXKKK@Z
void SquishPalette::SetPixel(unsigned long index, unsigned long pixel, unsigned long format)
{
    if (mEntries != 0)
    {
        if (index < mSize)
        {
            mEntries[index].SetPixel(pixel, format);
        }
    }
}

// 0x10067820 ?GetPixel@SquishPalette@@QBEKKK@Z
unsigned long SquishPalette::GetPixel(unsigned long index, unsigned long format) const
{
    if (mEntries && index < mSize)
        return mEntries[index].GetPixel(format);
    return 0;
}

// 0x10067850 ?GetPixel@SquishPalette@@QBE?AUSquishPixel@@K@Z
SquishPixel SquishPalette::GetPixel(unsigned long index) const
{
    if (mEntries && index < mSize)
        return mEntries[index];
    return SquishPixel(0, 0, 0, 0);
}

// 0x100678a0 ?GetIndex@SquishPalette@@QBEJUSquishPixel@@@Z
long SquishPalette::GetIndex(SquishPixel pixel) const
{
    if (!mEntries)
        return -1;
    for (unsigned long i = 0; i < mSize; i++) {
        if (mEntries[i].Pixel.bgra32 == pixel.Pixel.bgra32)
            return i;
    }
    return 0;
}

// 0x100678d0 ?GetNearestIndex@SquishPalette@@QBEJUSquishPixel@@@Z
long SquishPalette::GetNearestIndex(SquishPixel pixel) const
{
    if (!mEntries)
        return -1;
    long bestDistance = 0x7fffffff;
    long bestIndex = -1;
    for (unsigned long i = 0; i < mSize; i++) {
        if (mEntries[i].Pixel.bgra32 == pixel.Pixel.bgra32)
            return i;
        long delta = mEntries[i].Red() - pixel.Red();
        long distance = delta * delta;
        delta = mEntries[i].Green() - pixel.Green();
        distance += delta * delta;
        delta = mEntries[i].Blue() - pixel.Blue();
        distance += delta * delta;
        if (distance < bestDistance) {
            bestDistance = distance;
            bestIndex = i;
        }
    }
    return bestIndex;
}

// 0x10067980 ?StreamIn@SquishPalette@@SAPAV1@PAVSquishFile@@@Z
SquishPalette *SquishPalette::StreamIn(SquishFile *file)
{
    unsigned long seed;
    if (!file->ReadBytes(&seed, 4))
        return 0;
    SquishPalette *palette = Find(seed);
    unsigned long size;
    if (!file->ReadBytes(&size, 4))
        return 0;
    if (palette) {
        // already loaded: skip the entries
        if (!file->Seek(size * sizeof(SquishPixel), kSquishFileSeekCurrent))
            return 0;
    } else if (size) {
        SquishPixel *pixels = new SquishPixel[size];
        if (!pixels)
            return 0;
        if (!file->ReadBytes(pixels, size * sizeof(SquishPixel)))
            return 0;
        palette = Create(size, pixels);
        delete[] pixels;
    }
    return palette;
}

// 0x10067a60 ?BeginStreamOut@SquishPalette@@SAXXZ
void SquishPalette::BeginStreamOut(void)
{
    gPaletteManager.BeginStreamOut();
}

// 0x10067b20 ?StreamOut@SquishPalette@@QBE_NPAVSquishFile@@@Z
bool SquishPalette::StreamOut(SquishFile *file) const
{
    {
        unsigned long seed = mSeed;
        if (!file->WriteBytes(&seed, 4))
            return false;
    }
    // a palette written before in this stream is referenced by its seed only
    unsigned long tSize;
    if (gPaletteManager.HasBeenStreamed(mSeed))
        tSize = 0;
    else
        tSize = mSize;
    if (!file->WriteBytes(&tSize, 4))
        return false;
    if (tSize && !file->WriteBytes(mEntries, tSize * sizeof(SquishPixel)))
        return false;
    gPaletteManager.MarkStreamed(mSeed);
    return true;
}

// 0x10067c10 ?EndStreamOut@SquishPalette@@SAXXZ
void SquishPalette::EndStreamOut(void)
{
    gPaletteManager.EndStreamOut();
}

// 0x100677c0 ??_H@YGXPAXIHP6EX0@Z@Z
// 0x10067d00 ?begin@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@QAE?AViterator@12@XZ
// 0x10067d10 ?erase@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10068200 ?erase@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x100682f0 ?find@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x10068340 ?erase@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$MBAllocator@_N@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10068430 ?insert@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@KU?$pair@$$CBKPAVSquishPaded65064
// 0x100686d0 ?lower_bound@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@QAE?AViterator@12@ABK@Z
// 0x10068720 ?_Erase@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10068780 ?_Lrotate@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@IAEXPAU_Node@12@@Z
// 0x100687e0 ?_Min@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@KAPAU_Node@12@PAU312@@Z
// 0x10068820 ?_Rrotate@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10068880 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@QAEXXZ
// 0x10068910 ?insert@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$MBAllocator@_N@@@std@@QAE?AU?$pair@Viterator@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$cf69e3e7
// 0x10068bb0 ?erase@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$MBAllocator@_N@@@std@@QAE?AViterator@12@V312@@Z
// 0x100690a0 ?_Erase@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$MBAllocator@_N@@@std@@IAEXPAU_Node@12@@Z
// 0x10069100 ?_Insert@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBKPAVSquish8c344aef
// 0x100693e0 ?_Lbound@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x10069430 ?_Buynode@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x10069450 ?_Insert@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$MBAllocator@_N@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBK_N@2@@Z
// 0x10069730 ?_Lrotate@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$MBAllocator@_N@@@std@@IAEXPAU_Node@12@@Z
// 0x10069790 ?_Rrotate@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$MBAllocator@_N@@@std@@IAEXPAU_Node@12@@Z
// 0x100697f0 ?_Buynode@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$MBAllocator@_N@@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x10069810 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKPAVSquishPalette@@@std@@U_Kfn@?$map@KPAVSquishPalette@@U?$less@K@std@@V?$MBAllocator@PAVSquishPalette@@@@@2@U?$less@K@2@V?$MBAllocator@PAVSquishPalette@@@@@std@@QAEXXZ
// 0x100698b0 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$MBAllocator@_N@@@std@@QAEXXZ
// 0x10069950 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBK_N@std@@U_Kfn@?$map@K_NU?$less@K@std@@V?$MBAllocator@_N@@@2@U?$less@K@2@V?$MBAllocator@_N@@@std@@QAEXXZ
// 0x100699e0 ?_Construct@std@@YAXPAU?$pair@$$CBKPAVSquishPalette@@@1@ABU21@@Z
// 0x10069a00 ?_Construct@std@@YAXPAU?$pair@$$CBK_N@1@ABU21@@Z
// 0x10069a20 $E30
// 0x10069a40 $E29
