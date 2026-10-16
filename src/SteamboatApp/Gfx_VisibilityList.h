#pragma once
#include "StdString.h"
namespace Gfx { class Camera; }

namespace Gfx {
// Per-chunk lists of visible LOD nodes, loaded from a text file: chunk i's
// string holds entries "<lod letter><two-digit node index>" (no PDB
// declaration; member names chosen from the use and the status string
// "CurChunk: %i , Selected %i is %s with LOD %i").
class VisibilityList {
public:
    VisibilityList();
    virtual ~VisibilityList();
    void ToggleVisibilityList();
    void ToggleChunkDetail();
    void ToggleChunkVis();
    void LoadVisList(_SS<char>);
    void SaveVisList();
    void AddVisChunk(int, int, int);
    void DelVisChunk(int, int);
    bool GetVisChunkVis(int, int);
    int GetVisChunkLod(int, int);
    void UpdateVisibilityList(Gfx::Camera *);
    void DecChunk();
    void IncChunk();
    bool m_Enabled;
    int m_Selected;
    int m_SelectedLod;
    int m_CurChunk;
    int m_NextChunk;
    char unknown_18[24];
    _SS<char> *m_Chunks;
    _SS<char> m_FileName;
    int m_NumChunks;
};
}
