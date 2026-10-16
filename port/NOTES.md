# Port notes

## SteamboatApp

Fixes in `src/` (non-standard C++ that VC6 accepted; `tools/msvc/check.sh SteamboatApp/` stays
clean):
- FixedVec.h: the `FixedVec<float, 3>::multScal` specialization now says `template <>`.
- Gfx_View.h: dropped the stray `class Vec3;` that redeclared the `Vec3` template.

Port options (`port/flags.py`): libpng is built without `PNG_USE_PNGVCRD`, so pngvcrd.c (MMX
inline assembly) compiles to nothing and libpng uses its C filters.

Still open in other areas (reported, not mine):
- Gfx_BoundingVolume.h (G2): `inline VisTData<float>::VisTData()` needs `template <>`;
  Gfx_BoundingVolume.cpp uses `_copysign`.
- With `_WIN32`/`_MSC_VER` defined, libc++ headers take their MSVC paths
  (`vcruntime_exception.h`, a `va_list` clash): seen in EG2_Asset.cpp and ViewEventHandler.cpp,
  which include `<locale>`.

Port overrides and rules (SteamboatApp):
- MBContainers.h: MBVector hands out `T *` iterators and takes them in insert/erase (VC6's vector
  iterator is `T *`); it carries rewrite.py's MBAllocator/MBMap changes, keep both in step.
- MBComPtr.h: `operator<` on the interface pointer for maps keyed by MBComPtr; a copy of src,
  keep in step.
- Graph_NodeContext.h: values in a one-field struct (libc++'s vector<bool> is a bit vector).
- Gfx_TextureAttribute.cpp, Graph_FileStream.cpp: 64-bit reads (see below).
- libpng/pngconf.h, zlib/zconf.h: TARGET_OS_MAC is not Classic Mac OS.
- Temporaries bound to non-const references and the other VC6 conversions go through
  rewrite.py's FILE_RULES (port_lvalue and friends), requested from D.

64-bit hazards found so far:
- Graph::Stream (scene graph files: Gfx_*, Graph_*): `Read(unsigned long *)` reads 4 bytes into
  an 8-byte `unsigned long` and `ByteSwap(unsigned long *)` swaps 4 bytes of it, so the upper half
  keeps whatever the variable held, and `Write(unsigned long)` must write only 4 bytes. Every
  `unsigned long` read from a file (Gfx_Attribute, Gfx_AnimatedNode, Gfx_Camera,
  Gfx_DisplayQuality, Gfx_FileNode, Gfx_IONode, Gfx_IndexedTriangleSet, Gfx_IndexedVertexShape,
  Gfx_LODNode, Gfx_Node) is read through this. Override port/override/SteamboatApp/Graph_FileStream.cpp
  clears the value before the 4-byte read (little-endian: the low half is the file value), and
  `Read(unsigned char *, int)` no longer passes its int as `unsigned long &`. Writing is correct
  as it is (4 low bytes).
- For G2 (shapes): Gfx_IndexedVertexShape.cpp and Gfx_PackedVertexShape.cpp store and load
  packed colours as `*(unsigned long *)&m_CList[k]`; with an 8-byte `unsigned long` that writes
  over the next element (the store of 0xff000000 into k + 1 hides it, the loads read 8 bytes).
  These need a 32-bit type.
- The `throw (long)__LINE__` error codes (CSBInventory, SavedGameDB, SteamboatNode) are caught as
  `long`: harmless, the value fits.
- The saved games and the saved-game database are written through the engine's preference files
  (text), not as binary structures, so `long` fields (sgdbinfo::nIcon etc.) do not change the
  format.
- Event data travels as `void *` (ListenToEvent, click data); no pointer is narrowed to 32 bits in
  the game code seen so far. The script side (MfProperty conversions in CSBGameState,
  SBDialoguePropertyManager) depends on RuntimeLib's data handles.
- Class members typed `long` (animation sequence ids, MBPoint coordinates) are in-memory only and
  are not affected unless they are read from a file with a fixed size.
