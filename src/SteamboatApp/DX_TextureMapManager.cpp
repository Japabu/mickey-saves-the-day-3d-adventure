#include "DX_TextureMapManager.h"
#include "DX_TextureMap.h"
#include "DX_Direct3D.h"
#include "DX_Device.h"
#include "DX_Exception.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

#define THIS_FILE "D:\\tugboat\\code\\SDK\\Scene\\DirectX\\DXTextureMapManager.cpp"

// 0x100cc940 ??0TextureMapManager@DX@@QAE@PAVDirectDraw@1@@Z
DX::TextureMapManager::TextureMapManager(DX::DirectDraw *directDraw)
    : m_pDirectDraw(directDraw), m_FirstTime(true), m_bAGP(false)
{
}

// 0x100cca40 ??_GTextureMapManager@DX@@UAEPAXI@Z

// 0x100ccad0 ??1TextureMapManager@DX@@UAE@XZ
DX::TextureMapManager::~TextureMapManager()
{
    Release(false);
}

// TODO 0x100ccbc0 ?GetTextureMap@TextureMapManager@DX@@QAEPAVTextureMap@2@AAUDirectDrawSurfaceAttrib@2@PAVDevice@2@_N@Z: 72% - the original calls the small std::string helpers out of line, VC6 here inlines them
DX::TextureMap *DX::TextureMapManager::GetTextureMap(DX::DirectDrawSurfaceAttrib &attrib, DX::Device *device, bool ownDC)
{
    StringToTextureMap::iterator it = m_TextureMaps.find(std::string(attrib.fileName.c_str()));
    if (it != m_TextureMaps.end())
        return (*it).second;
    DX::TextureMap *map = new DX::TextureMap(m_pDirectDraw);
    if (ownDC)
        map->m_bOwnDC = true;
    if (m_FirstTime) {
        m_FreeTextureMemory = m_pDirectDraw->GetFreeTextureMemory();
        m_TotalTextureMemory = m_pDirectDraw->GetTotalTextureMemory(device->GetDirect3D()->DriverInfo());
        DDCAPS_DX7 caps;
        memset(&caps, 0, sizeof(caps));
        caps.dwSize = sizeof(caps);
        m_FirstTime = false;
        if (m_pDirectDraw->GetCOMInterface())
            m_pDirectDraw->GetCOMInterface()->GetCaps(&caps, 0);
        if ((caps.dwCaps2 & DDCAPS2_NONLOCALVIDMEM) && device->GetDirect3D()->DriverInfo().bTextureNonLocalVidMem)
            m_bAGP = true;
    }
    LoadTextureSubr(map, attrib, device);
    unsigned long freeMemory = m_pDirectDraw->GetFreeTextureMemory();
    map->m_TextureMemory = m_FreeTextureMemory - freeMemory;
    m_FreeTextureMemory = freeMemory;
    m_TextureMaps.insert(StringToTextureMap::value_type(std::string(attrib.fileName.c_str()), map));
    return map;
}

// 0x100cceb0 ??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@std@@QAE@XZ

// TODO 0x100ccef0 ?Release@TextureMapManager@DX@@QAEX_N@Z: 39% - the original calls _Tree::erase, the list node string destructor and its _Refcnt/deallocate helpers out of line; VC6 here inlines them
void DX::TextureMapManager::Release(bool clearPaths)
{
    m_FirstTime = true;
    for (StringToTextureMap::iterator it = m_TextureMaps.begin(); it != m_TextureMaps.end(); ++it) {
        delete (*it).second;
        (*it).second = 0;
    }
    m_TextureMaps.erase(m_TextureMaps.begin(), m_TextureMaps.end());
    if (clearPaths)
        m_Paths.erase(m_Paths.begin(), m_Paths.end());
}

// TODO 0x100cd000 ?LoadTextureSubr@TextureMapManager@DX@@IAEXPAVTextureMap@2@AAUDirectDrawSurfaceAttrib@2@PAVDevice@2@@Z: 56% - as GetTextureMap: basic_string::operator[] and the string copy are out of line in the original
void DX::TextureMapManager::LoadTextureSubr(DX::TextureMap *map, DX::DirectDrawSurfaceAttrib &attrib, DX::Device *device)
{
    try {
        attrib.isEmulation = !device->GetDirect3D()->DriverInfo().bIsHardware;
        attrib.isAGP = m_bAGP;
        if (attrib.fileName[0U] == '\\' || attrib.fileName[1U] == ':') {
            map->LoadTexture(std::string(attrib.fileName.c_str()), device, attrib);
        } else {
            std::list<std::string>::iterator path;
            for (path = m_Paths.begin(); path != m_Paths.end(); ++path) {
                try {
                    map->LoadTexture(*path + attrib.fileName.c_str(), device, attrib);
                    break;
                } catch (DX::Exception &e) {
                    if (e.GetResult() != 110)
                        throw;
                }
            }
            if (path == m_Paths.end())
                throw DX::Exception(110, "Could not locate texture map");
        }
    } catch (...) {
        delete map;
        throw;
    }
}

// 0x100ce570 _$E27
// 0x100ce5a0 _$E23
// 0x100ce5b0 _$E26

// Template and inline instances this unit also emits:
// 0x100cca60 ??1?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@V?$allocator@PAVTextureMap@DX@@@2@@std@@QAE@XZ
// 0x100cd220 ??1?$list@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAE@XZ
// 0x100cd280 ?insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@2@U_Kfn@?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@f966f143
// 0x100cd3f0 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@2@U_Kfn@?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@V?$allocator@PAVTextureMap@DX@@@2@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@V?$allocator@PAVTextureMap@DX@@@2@@std@@QAE?AViterator@12@V312@0@Z
// 0x100cd530 ?find@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@2@U_Kfn@?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@sta4bde49f
// 0x100cd5c0 ?_Inc@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@2@U_Kfn@?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$al071c9fdd
// TODO 0x100cd670 ?_Refcnt@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEAAEPBD@Z: inline STL instance; this unit inlines every call here, so no out-of-line copy is emitted
// TODO 0x100cd680 ?deallocate@?$allocator@D@std@@QAEXPAXI@Z: inline STL instance; this unit inlines every call here, so no out-of-line copy is emitted
// 0x100cd690 ?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@2@U_Kfn@?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@V?$allocator@PAVTextureMap@DX@@@2@@2@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@V?$allocator@PAVTextureMap@DX@@@2@@std@@QAE?AViterator@12@V312@@Z
// 0x100cdbc0 ?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@2@U_Kfn@?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@491e9c0e
// 0x100cdc70 ?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@2@U_Kfn@?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@38aa7e64
// 0x100cdf80 ?erase@?$list@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAE?AViterator@12@V312@@Z
// 0x100cdff0 ??0?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@2@U_Kfn@?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$allb5af8c25
// 0x100ce010 ?_Lbound@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@2@U_Kfn@?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@4a282c67
// 0x100ce0d0 ?_Dec@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@2@U_Kfn@?$multimap@V?$basic_string@DU?$char_traits@D@std@@V?$alde93cd73
// TODO 0x100ce190 ??Hstd@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@0@ABV10@PBD@Z: 96% - the original loads basic_string::npos from its static for the length computation, VC6 here folds it to or eax, -1
// 0x100ce2c0 ?_Construct@std@@YAXPAU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PAVTextureMap@DX@@@1@ABU21@@Z
// TODO 0x100ce410 ??A?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAADI@Z: inline STL instance; this unit inlines every call here, so no out-of-line copy is emitted
// 0x100ce4b0 ?compare@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBEHABV12@@Z
// 0x100ce500 ?compare@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBEHIIPBDI@Z
