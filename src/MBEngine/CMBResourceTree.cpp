// MBEngine.dll .\MBEngineRelease\CMBResourceTree.obj
#include "CMBResourceTree.h"

// the original reports the build machine's __FILE__
#define kThisFile "D:\\tugboat\\code\\sdk\\muffin\\src\\IO\\CMBResourceTree.cpp"

// 0x1000f640 ?MakeCMBResourceTree@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBResourceTree(void)
{
    IMBUnknown *p = (IMBResourceFile *)new CMBResourceTree;
    if (p)
        p->AddRef();
    return p;
}

// 0x1000f670 ??0CMBResourceTree@@QAE@XZ
CMBResourceTree::CMBResourceTree(void)
{
    _mRefCnt = 0;
}

// 0x1000f730 ?QueryInterface@CMBResourceTree@@UAEJABUMBGUID@@PAPAX@Z
long CMBResourceTree::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBFile)
        p = (IMBFile *)this;
    if (iid == MBIID_IMBResourceFile)
        p = (IMBResourceFile *)this;
    if (iid == MBIID_IMBUnknown)
        p = (IMBFile *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1000f850 ?AddRef@CMBResourceTree@@UAEKXZ
unsigned long CMBResourceTree::AddRef()
{
	return ++_mRefCnt;
}

// 0x1000f860 ?Release@CMBResourceTree@@UAEKXZ
unsigned long CMBResourceTree::Release()
{
	unsigned long r = --_mRefCnt;
	if (!_mRefCnt && this)
	{
		this->~CMBResourceTree();
		MBfree_release(this);
	}
	return r;
}

// 0x1000f890 ??1CMBResourceTree@@QAE@XZ
CMBResourceTree::~CMBResourceTree(void)
{
    Close();
}

// 0x1000f990 ?Open@CMBResourceTree@@UAE_NPBVIMBFileSpec@@K@Z
bool CMBResourceTree::Open(const IMBFileSpec *fileSpec, unsigned long flags)
{
    AddDirectory(fileSpec);
    IMBFileSpec *parent;
    fileSpec->GetParent(&parent);
    if (parent) {
        mSpec.CreateObject(MBIID_IMBFileSpec);
        mSpec->SetPath(parent, fileSpec->GetName().c_str());
        parent->Release();
    }
    return true;
}

// 0x1000faa0 ?GetFileSpec@CMBResourceTree@@UAEXPAPAVIMBFileSpec@@@Z
void CMBResourceTree::GetFileSpec(IMBFileSpec **fileSpec)
{
	if (mSpec)
	{
		mSpec->AddRef();
	}
	*fileSpec = mSpec;
}

// 0x1000fad0 ?Close@CMBResourceTree@@UAE_NXZ
bool CMBResourceTree::Close(void)
{
    mResMap.erase(mResMap.begin(), mResMap.end());
    return true;
}

// 0x1000fbc0 ?LoadResource@CMBResourceTree@@UAEXKKPAPAVIMBMemoryBlock@@@Z
void CMBResourceTree::LoadResource(unsigned long resID, unsigned long type, IMBMemoryBlock **resourceData)
{
    *resourceData = 0;
    IMBStream *stream;
    GetStream(resID, type, &stream);
    if (stream) {
        unsigned long size = stream->GetStreamSize();
        IMBMemoryBlock *block;
        MBComCreateInstance(0, MBIID_CMBMemoryBlock, (IMBUnknown **)&block);
        if (block && !block->Allocate(size)) {
            MBDebugStr("ERROR: alloc failure in %s, line %d.\n", kThisFile, 135);
            block->Release();
            block = 0;
        }
        if (block) {
            unsigned long read = size;
            stream->Read(block->GetData(), read);
            if (size != read) {
                MBDebugStr("ERROR: read failure in %s, line %d.\n", kThisFile, 148);
                block->Release();
                block = 0;
            }
        }
        stream->Release();
        *resourceData = block;
    }
}

// 0x1000fca0 ?GetStream@CMBResourceTree@@UAEXKKPAPAVIMBStream@@@Z
void CMBResourceTree::GetStream(unsigned long resID, unsigned long type, IMBStream **stream)
{
    *stream = 0;
    TypeMap::iterator it = mResMap.find(type);
    if (it != mResMap.end()) {
        FileMap::iterator file = (*it).second.find(resID);
        if (file != (*it).second.end())
            MBOpenFile((*file).second, 5, stream);
    }
}

// TODO 0x1000fd30 ?AddDirectory@CMBResourceTree@@IAE_NPBVIMBFileSpec@@@Z: original computes files.size() once with the begin()==0 test threaded straight to the loop exit and keeps this in edi; count variable and plain for both differ (51% similar)
bool CMBResourceTree::AddDirectory(const IMBFileSpec *dir)
{
    MBVector<MBComPtr<IMBFileSpec> > files;
    MBGetFilesInDirectory(dir, files, 0xd, -1);
    unsigned long count = files.size();
    for (unsigned long i = 0; i < count; i++) {
        unsigned long hash = files[i]->Hash();
        unsigned long type = files[i]->GetType();
        if (mResMap[type].find(hash) != mResMap[type].end()) {
            // source control droppings collide all the time; anything else is worth a warning
            if (!files[i]->GetTitle().find(".scc")) {
                MBDebugStr("WARN: Hash collision in CMBResourceTree.\n");
                MBDebugStr("      File %s collides.\n", files[i]->GetPath().c_str());
            }
        } else {
            mResMap[type][hash] = files[i];
        }
    }
    return true;
}

// 0x10010020 ?Dump@CMBResourceTree@@QAEXXZ
void CMBResourceTree::Dump(void)
{
    for (TypeMap::iterator it = mResMap.begin(); it != mResMap.end(); ++it) {
        const char *type = (const char *)&(*it).first;
        MBDebugStr("Dumping type '%c%c%c%c':\n", type[3], type[2], type[1], type[0]);
        for (FileMap::iterator file = (*it).second.begin(); file != (*it).second.end(); ++file)
            MBDebugStr("\tFile: '%s' (hash %d):\n", (*file).second->GetName().c_str(), (*file).first);
    }
}

// 0x10010110 ?SetSize@CMBResourceTree@@UAE_NK@Z
bool CMBResourceTree::SetSize(unsigned long size)
{
	return false;
}

// 0x10010120 ??0?$vector@V?$MBComPtr@VIMBFileSpec@@@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE@ABV?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@Z
// 0x10010140 ??0?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@QAE@XZ
// 0x10010270 ?insert@?$map@KV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@KU?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U2ef3cf23
// 0x10010380 ?find@?$_Tree@KU?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@@@2@b7a73c4d
// 0x100103d0 ??0?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@std@@QAE@ABKABV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@Z
// 0x10010590 ??0?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE@ABU?$less@K@1@ABV?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@Z
// 0x10010630 ??1?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE@XZ
// 0x10010770 ?begin@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE?AViterator@12@XZ
// 0x10010780 ?find@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE?AViterator@12@ABK@Z
// TODO 0x100107d0 ?_Destroy@?$vector@V?$MBComPtr@VIMBFileSpec@@@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@IAEXPAV?$MBComPtr@VIMBFileSpec@@@@0@Z: only emitted out of line once AddDirectory matches (ours inlines it)
// 0x10010800 ??0?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@QAE@ABV0@@Z
// 0x100109b0 ?erase@?$_Tree@KU?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@@@267284565
// 0x10010f20 ?deallocate@?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@QAEXPAXK@Z
// 0x10010f40 ?_Erase@?$_Tree@KU?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@@@f4b083f4
// 0x10011030 ?insert@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE?AU?$pair@Vit63362fda
// 0x100112d0 ?erase@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE?AViterator@12e7518cd3
// 0x100113d0 ?lower_bound@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE?AViteraba60c024
// 0x10011420 ?_Lmost@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@IAEAAPAU_Node@12@XZ
// 0x10011430 ??0?$MBComPtr@VIMBFileSpec@@@@QAE@ABV0@@Z
// 0x10011450 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAEXXZ
// 0x100114e0 ??0iterator@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE@PAU_Node@12@@Z
// 0x100114f0 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less1b1fcc7a
// 0x10011580 ??0?$pair@Viterator@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@_N@sfa437461
// 0x100115a0 ?_Insert@?$_Tree@KU?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@@5f379e9b
// 0x10011880 ?_Lbound@?$_Tree@KU?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@@a6da4266
// 0x100118d0 ?erase@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10011dd0 ?_Erase@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10011e40 ?_Insert@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@IAE?AViterator@c519039b
// 0x10012120 ?_Lbound@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x10012170 ?_Copy@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@IAEPAU_Node@12@PAU312@0@Z
// 0x10012220 ?_Lrotate@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10012280 ?_Rrotate@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@IAEXPAU_Node@12@@Z
// 0x100122e0 ?_Buynode@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@IAEPAU_Node@12d266377d
// 0x10012300 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAEXXZ
// 0x100123a0 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@std@@U_Kfn@?$map@KV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@U?$less@K@std@@V?$MBAllocator@V?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$lessc3f1540a
// 0x10012440 ?_Construct@std@@YAXPAU?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@1@ABU21@@Z
// 0x100125f0 ?_Max@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@KAPAU_Node@12@PAU312@@Z
// 0x10012630 ?_Construct@std@@YAXPAU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@1@ABU21@@Z
// 0x10012650 ??_G?$pair@$$CBKV?$MBMap@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@@@@std@@QAEPAXI@Z
// 0x10012770 ??Eiterator@?$_Tree@KU?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@U_Kfn@?$map@KV?$MBComPtr@VIMBFileSpec@@@@U?$less@K@std@@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@2@U?$less@K@2@V?$MBAllocator@V?$MBComPtr@VIMBFileSpec@@@@@@@std@@QAE?AV012@H@Z
// 0x10012800 ??_G?$pair@$$CBKV?$MBComPtr@VIMBFileSpec@@@@@std@@QAEPAXI@Z
// 0x10012840 $E24
// 0x10012860 $E23
// 0x10012870 ?QueryInterface@CMBResourceTree@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10012880 ?AddRef@CMBResourceTree@@W3AEKXZ
// 0x10012890 ?Release@CMBResourceTree@@W3AEKXZ
