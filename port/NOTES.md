# Port notes

These notes record what porting each module needed and what is still open. The sections from
`mbruntime` to `plugins/PscContentInfo` come from a 64-bit hazard survey: a list of the places
where the 32-bit Windows code goes wrong at run time on LP64 arm64. On arm64, pointers and C
`long` are 8 bytes, `wchar_t` is 4, and the compat `DWORD`/`LONG`/`UINT` stay 32 bits with a
16-bit `WCHAR`. Each bullet gives the severity first: **[crash]** for memory corruption, a
truncated pointer or a fault; **[data]** for wrong file or stream data; **[minor]** for logs,
compile-only problems or latent cases. Then come the location, the cause, the fix for the port,
and whether a file format is involved. On-disk layouts always stay 32-bit and are converted at
the boundary. One cause turns up in most modules: a DLL declares another DLL's class again (the
`*_unit.h` views and local copies), with `int` or `char[]` filler where the real class has
pointers or `MfString`s. On 32-bit those layouts matched; on LP64 they are too small or put
fields at the wrong offsets.

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

## mbruntime

- **[crash]** `src/mbruntime/MBWinDebug.cpp:17` — `wvsprintf(buffer, fmt, (va_list)(&fmt + 1))` builds the va_list by hand from the address of the named parameter. On Apple arm64 the named `fmt` arrives in x0 (its address is a spill slot in the callee's frame), and the variadic arguments go to 8-byte slots in the caller's outgoing area. `&fmt + 1` points at unrelated stack, so every `%s` dereferences garbage. MBDebugStr is the logger for every MB DLL, and it runs from DllMain on module load. Fix: override MBDebugStr to use `va_list ap; va_start(ap, fmt); vsnprintf(buffer, sizeof buffer, fmt, ap); va_end(ap);`, and take `len` from the clamped return value. Format: no file format involved (log.txt is text).
- **[data]** `src/mbruntime/MBstrings.cpp:8-14` — `MBHashString` computes `hash = hash * 38 + toupper(c)` in `unsigned long`. The original wrapped at 32 bits; here it wraps at 64. Any name of about 7 or more characters already exceeds 2^32, so every resource, sound, dialogue and section ID comes out different from the original. Lookups that hash both sides at runtime still agree (CMBResourceZipfile::ParseZip, GetStream). Callers that store the hash at narrower width do not: `SteamboatApp/Actor.h:89` `int mCurrentDialogId = MBHashString(...)` (Actor.cpp:158) truncates it, and later compares against `unsigned long` hashes then fail. Precomputed IDs (if any data or tables hold them) cannot match either. Fix: compute and return the hash as 32-bit (`port_u32 hash; ... return hash;`, or return DWORD) in an override of MBstrings.cpp. Callers that compare an `int`-stored ID with an `unsigned long` ID still sign-extend; that belongs to the SteamboatApp survey. Format: keep the 32-bit hash values, which must match whatever the original produced or stored.
- **[minor]** `src/mbruntime/MBMemory.cpp:31-46` — `MBDebugHeapHeader` grows from 32 to 56 bytes (`long`, `unsigned long` and pointers are all 8). `kMBDebugHeapUserOffset` becomes 72, so debug-heap user pointers are only 8-byte aligned, not the 16 that malloc and the original layout (48) give. The layout is self-consistent because everything uses `sizeof`, but 16-byte-aligned data (NEON `float32x4_t`, `alignas(16)`, `max_align_t` assumptions) placed in a debug-heap block may be misaligned. Fix: pad the header to a multiple of 16 (or `alignas(16)`) in an override. Format: no file format involved.
- **[minor]** `src/mbruntime/MBMemory.cpp:119-120, 200, 218, 228, 238`, `src/mbruntime/MBMemory.cpp:102, 119, 126`, `src/mbruntime/MBDebug.cpp:29-30` — debug-heap and assert messages print pointers with `0x%08X`, which truncates to the low 32 bits. They also print `unsigned long`/`long` values (`size`, `index`, `line`, `count`) with `%d`. On Apple arm64 each variadic argument fills an 8-byte slot, so `%d` reads the low half correctly for small values, but the output is UB and the pointers come out wrong. Fix: `%p` for pointers and `%lu`/`%ld` (or casts to int) for the longs. Format: no file format involved (log text only).

## zlib

- No 64-bit hazards found. This is stock zlib 1.1.3. `zconf.h` is not customised for width: `uLong` = `unsigned long` (64-bit), STDC is on, and `ZLIB_DLL`/WINAPI only affect linkage. Checked:
  - `crc32` keeps the value within 32 bits: table entries < 2^32, `crc >> 8`, and `^ 0xffffffffL`.
  - `adler32` masks with `& 0xffff` and reduces mod BASE.
  - `gzio.c` `putLong` writes exactly 4 bytes. `getLong` builds the value from 4 bytes, and the trailer compares against the 32-bit crc and total_in.
  - The inflate check bytes in `inflate.c:199-254` are assembled from 4 bytes into a uLong.
  - The `inflate_huft` and `inflate.c` unions hold only uInt/Byte fields; no struct is mapped onto bytes.
  - `gzprintf` uses the stdarg branch; `zcalloc` is plain calloc.
  - `z_off_t` = `long` is only an in-memory offset.
  - Callers (MBEngine `CMBResourceZipfile`, SteamboatApp libpng) must use `uLong`/`uLongf` for `destLen`-style out parameters and must not treat `get_crc_table()` as 32-bit entries; that belongs to their surveys.

## mflinalg

- **[crash]** `src/mflinalg/MfMath.cpp:44-51` — the `_sqrttab` builder aliases `unsigned long *fi = (unsigned long *)&f` onto a 4-byte `float` and stores through it (`*fi = ...`). On LP64 that is an 8-byte store into a 4-byte stack slot, which clobbers the neighbouring locals. The reads at lines 48 and 51 pick up 4 bytes of garbage above the float, so the table is built from wrong bits. Fix: use `port_u32`/`uint32_t` (or `memcpy` into a `uint32_t`) in an override of MfMath.cpp. Format: no file format involved.
- **[crash]** `src/mflinalg/MfMath.cpp:172-178` — `MfMath::sqrt` reads `*(unsigned long *)&x` (8 bytes from a 4-byte float parameter slot) and writes `*(unsigned long *)&x = ...` (8 bytes back). The read brings in garbage above bit 31, so `(short)((bits >> 23) - 127)` takes stack garbage into the exponent and the results are wrong. The write overruns the parameter's spill slot. Fix: same as above, with 32-bit bit patterns (`uint32_t bits; memcpy(&bits, &x, 4); ... memcpy(&x, &bits, 4);`). Format: no file format involved.

## mfutil

- **[crash]** `src/mfutil/MfNameDialog.cpp:52, 62`, `src/mfutil/MfKeyDialog.cpp:43, 55`, `src/mfutil/MfLoggerUI.cpp:56, 61, 474, 483, 558, 564` — each dialog or window keeps its C++ `this` in `GWL_USERDATA` through `SetWindowLong(A)(hwnd, GWL_USERDATA, lParam / (LONG)ptr)` and reads it back with `(T *)GetWindowLong(A)(hwnd, GWL_USERDATA)`. `LONG` is 32 bits, so the pointer is truncated and the next message dereferences a bad pointer. Fix: `SetWindowLongPtr`/`GetWindowLongPtr` with `GWLP_USERDATA` and `LONG_PTR` in compat, applied by a rewrite.py rule (same text in 3 files), or have the platform layer keep its own per-window pointer slot. Format: no file format involved.
- **[crash]** `src/mfutil/MfStringRef.cpp:81, 87, 125, 133, 170, 172, 265-266` — MfStringRef's wide-string API is 16-bit (`const unsigned short *`, matching the original `PBG`/`PAG` exports), but it calls the C library's `wcslen`/`wcstombs`/`mbstowcs`, which take a 4-byte `wchar_t` on macOS. On VC this compiled because wchar_t was unsigned short. Here it does not compile without a cast, and a `(wchar_t *)` cast is wrong:
  - `wcslen` over UTF-16 reads code-unit pairs, so it gets wrong lengths and runs past the terminator.
  - `DuplicateUnicode` (line 263-266) has `mbstowcs` write 4-byte units into `new unsigned short[m_len + 1]`, a 2x heap overflow.

  Fix: an override with 16-bit helpers (a UTF-16 strlen, and char<->UTF-16 conversion loops or iconv). WCHAR in compat is already 16-bit. Format: no file format involved (in-memory API contract: 16-bit wide strings).
- **[crash]** `src/mfutil/MfVersionResource.cpp:49-51` — `unsigned long *pFixed` scans the version resource for `VS_FFI_SIGNATURE` in 8-byte steps with 8-byte loads. The signature is a 32-bit DWORD followed by `dwStrucVersion`, so the 64-bit value never equals 0xFEEF04BD. The loop runs off the end of the resource (fault) or matches garbage. Fix: scan with `port_u32 *` or `DWORD *`. Format: keep the on-disk 32-bit layout (VS_VERSIONINFO is a fixed binary format) and read it with 32-bit/16-bit types.
- **[data]** `src/mfutil/MfVersionResource.cpp:57, 59, 63-65, 83, 87, 92` — the block keys are UTF-16 (`unsigned short *`, BLOCK_KEY), but they are compared with `_wcsicmp(L"StringFileInfo", ...)`, `L"040904b0"` and `L"040904e4"`, which are 4-byte `wchar_t` literals here. They are measured with `wcslen` (4-byte units). No key ever matches, and the computed `pTable`/`pString`/`pValue` offsets are wrong. Caller note: `RuntimeLib/MfRuntime.cpp:135` passes `(unsigned short *)L"FileVersion"`, which is the same 4-byte literal problem. Fix: 16-bit literals (`u"..."` cast to `const unsigned short *`, or a WCHAR table) plus 16-bit compare and length helpers in an override. Format: keep the on-disk UTF-16 layout of the resource and compare at that width.
- **[data]** `src/mfutil/MfString.h:73, 103, 106, 108, 119, 139-142, 151, 163, 170-171, 182, 199-216, 262`, `src/mfutil/MfString.cpp:36-40, 79, 183, 266-273, 328-330, 513-619` — the mfutil copy of MfString declares its wide API with `wchar_t`. `GetUnicode` allocates `new wchar_t[]` and fills it with `mbstowcs`, so it hands out 32-bit units. The original exports are 16-bit (`??0MfString@@QAE@PBG@Z`, `??BMfString@@QBEPAGXZ`), and so is every consumer header (`PiccoloRT/MfString.h:62-157`, plus the MfActorLib, PiccoloComp, RuntimeLib and MfPoLibrary copies) and mfutil's own MfStringRef. The result is one of two things. The symbols do not match across dylibs (link failure). Or, once unified, callers read `"a\0\0\0b..."` as UTF-16, and wide strings are cut after the first character. Fix: an override of MfString.h/.cpp using `unsigned short` (WCHAR) throughout, with the 16-bit helpers from the MfStringRef fix. Format: no file format involved (ABI between modules).
- **[minor]** `src/mfutil/MfString.cpp:480, 482` — `FormatInt` passes `long v % divisor` to `sprintf("%d")` and `sprintf("%0*d")`. This is a va_arg width mismatch; it works on Apple arm64 only because each variadic slot is 8 bytes and the value is below `divisor`. Fix: `%ld`, or cast to int. Format: no file format involved.

## MBCom

- **[crash]** `src/MBCom/MBCom.h:31-38, 55-59` — `MBGUID::data1` is `unsigned long`, so the struct grows from the 16-byte GUID layout to 24 bytes (data1 8, data2/data3 4, data4 8, 4 bytes of tail padding). `operator<` compares `*(unsigned long *)&guid.data4[0]` and `*(unsigned long *)&guid.data4[4]` as 8-byte loads. The second load covers `data4[4..7]` plus the 4 padding bytes, which are indeterminate in copies (map nodes, stack temporaries). The ordering of `mFactoryMap` then becomes inconsistent: `find` misses registered factories, `MBComFactory::Make` returns 0, and callers dereference null (or the tree is corrupted). Cross-module note: the same 8-byte `data4` pattern in `operator==` reads past the array and past the object, for example `MBEngine/CMBMemFile.h:19-22` and the other MBGUID copies (Steamboat/GameApp.h:18, SteamboatApp/MBGUID.h, MBEngine/*.h). Fix: an override of MBCom.h with `port_u32 data1` (restoring the 16-byte layout), and `data4` compared with 32-bit loads (`port_u32`, or memcpy/memcmp). The other copies need the same change. Format: no file format involved here (keep the 16-byte layout in case MBEngine streams GUIDs).
- **[minor]** `src/MBCom/MBCom.cpp:13, 39-48` — `unsigned long gRefCount` (64-bit) is updated through `InterlockedIncrement((LPLONG)&gRefCount)` and `InterlockedDecrement((LPLONG)&gRefCount)`, which are 32-bit atomics on the low half. This works on little-endian while the count stays in range, but an extra Uninitialize at 0 leaves 0x00000000FFFFFFFF instead of wrapping the whole word, and the mixed-width access is UB. Fix: declare `gRefCount` as `LONG` in an override. Format: no file format involved.

## MfActorLib

- **[crash]** `src/MfActorLib/MfFileStream_unit.h:85` (also `:86`, `src/MfActorLib/MfActorRecordList_unit.h:90-93`) — `MfStream::Status()` has exactly one definition in the DLL: the inline one in the MfFileStream_unit.h view, which `MfAnimEvent.cpp` emits because the class is dllexport. The canonical `src/MfActorLib/MfStream.h:20` only declares it, and `MfStream.cpp` never defines it. In the view `m_status` sits behind four `int`s and lands at +0x18 on LP64. In the real class (`MfStream.h:57-62`: int, `long objectType`, int, `void *userData`) it is at +0x28 (measured). So every `Status()` call in the DLL reads the `objectSize` slot instead: `MfStream.cpp:286,320,325`, every `ReadFromStream`/`WriteToStream` `return s.Status()`, and `MfAnimSequence_2.cpp:218`. Both views also define `SetStatus` inline with a different body than `MfStream.h:21` (+0x18, and +0x1c behind `char m_pad[0x14]`), so the linker's pick of the duplicate decides where the status is written. The same TU emits implicit copy ctor/operator= for `MfStream`/`MfFileStream`/`MfMemoryStream` from 32-bit-shaped views (MfMemoryStream view uses `int m_capacity, m_pos` against `unsigned long size, position` in `MfMemoryStream.h:19-20`). On 32-bit the offsets agreed only by accident. Fix: override `MfFileStream_unit.h` and `MfActorRecordList_unit.h` to include the canonical `MfStream.h`/`MfFileStream.h`/`MfMemoryStream.h`, and define `Status()` once in the canonical header. Format: no file format involved.
- **[crash]** `src/MfActorLib/MfAnimSequence_2.cpp:213`, `:241`, `:121`, `:292`; `src/MfActorLib/MfCamera.cpp:29`, `:54` — objects allocated with the size of a TU-local view smaller than the real class, then built by the real constructor in another TU (sizes measured with the port flags):
  - `:213`: stack `MfFileStream`. The view is `MfAnimSequence_2_unit.h:140` (`int m_24..m_30` over a 4-int MfStream view), 72 B. The real class is 104 B, and `MfFileStream.cpp:65` writes all 104 B, so this is a stack overflow.
  - `:241`: stack `MfCopyFolder` with `char m_data[0xc]`, 24 B (`MfAnimSequence_2_unit.h:86`, also `src/MfActorLib/MfCopyFolder.h:13`). mfutil's real class (vptr + 3 MfString, `src/mfutil/MfCopyFolder_unit.h:202-204`) is 32 B.
  - `:121`, `:292`: `new MfAnimFile` gets 128 B (view with `int m_bufferSize/m_pos`). `MfAnimFile::MfAnimFile` (`MfFile.cpp:28`, canonical, 136 B) stores `m_dataEnd` past the end of the block.
  - `MfCamera.cpp:29`, `:54`: `new MfCamera` gets 64 B. The view's `MfPropertyList` is `int m_04..m_10` (`MfCamera_unit.h:134-150`). The real `MfPropertyList` ctor (`MfPropertySet.cpp:58`) initialises a 40-B `MfDblList` at view offset +0x28, so it writes past the block.

  Why: pointers inside the real classes grew, and the opaque int/char fillers did not. Fix: overrides of these unit headers that include the canonical class headers (`MfFileStream.h`, `MfFile.h`, `MfCamera.h`/`MfPropertyList.h`, mfutil's real `MfCopyFolder` declaration). Format: no file format involved.
- **[crash]** `src/MfActorLib/MfAnimSequence_2_unit.h:250-280` (`int m_bufferSize; int m_pos;`, base of `MfAnimSequence` at `:304`), `src/MfActorLib/MfAnimSequence_unit.h:104` (`char m_pad00[0x34]`) — three different layouts of `MfFile`/`MfAnimSequence`:
  - The canonical `MfFile.h:129-130` uses `unsigned long` and is 88 B; the MfAnimSequence_2 view is 80 B.
  - So `MfAnimSequence_2.cpp` puts `m_numFrames`/`m_timeStep` at +0x50/+0x58. But `MfFile::ReadBytes`/`WriteBytes`/`Open` (`MfFile_2.cpp:63-64,379,428`) store the 8-byte `m_pos` at +0x50 and `m_bufferSize` at +0x48, so every buffered read or write overwrites `m_numFrames` and the upper half of the view's buffer fields.
  - The `char m_pad00[0x34]` view puts `m_numFrames` at +0x3c. Its dllexport inline accessors (GetNumFrames, SetTimeStep, GetTimeStep, NextFrame, GetPath, Get*Events, emitted by `MfAnimSequence.cpp`) conflict with the MfAnimSequence_2 ones.

  Fix: one `MfAnimSequence` declaration on top of canonical `MfFile.h`, through an override of both unit headers. Format: no file format involved.
- **[crash]** `src/MfActorLib/MfProtoActor_unit.h:53-62`, `:65-105` (emitted by `src/MfActorLib/MfProtoActor.cpp:20`) — a view of `MfProtoActor` that fills 32-bit-sized holes with `char m_0c[0x10]`, `m_48[0x14]`, `m_ac[8]`, `m_c4[0x2c]`, `m_f4[8]` and `MfPropertyTable { char m_data[0x18]; }`.
  - Its inline accessors are dllexport, and the same names exist inline in the canonical `MfProtoActor.h:48-98` (GeomTraversal, Properties, Instances, BoundingRadius, BehaviorMaps, ProximityDefs, MaxProximityDef, Skills, BhvrList, Get/SetUserData). On LP64 the two copies use different offsets, e.g. `GeomTraversal()`: view +0x28 (`pSkeletonRoot`'s slot) against real `pGeomTraversal` at +0x38 (measured). Whichever duplicate the linker keeps is used everywhere.
  - The view's `MfProtoProxDef` has `long m_flags; long m_id;` where the real one (`MfProtoProxDef.h`) has `float`/`int`: 48 B against 40 B (measured). Its implicit `operator=` (emitted by this TU, comment at `MfProtoActor.cpp:3`) copies 8 B past a real `MfProtoProxDef`.

  Same class of bug: the `MfTypedValue() { m_04 = 0; m_08 = 0; }` view ctors (`MfCamera_unit.h:107`, `MfFSMSpaceToBhvr_unit.h:70`, `MfAnimSequence_2_unit.h:168`, `MfAnimSequence_unit.h:78`) zero only +0x8..+0xf. The real `MfTypedValue.h` has `long lModCBackId` +0x8 and `pModCBack` +0x10, so if the linker keeps a view ctor, the callback pointer stays garbage and gets called later. Fix: drop the inline-only unit TUs from the port build (`MfProtoActor.cpp`, `MfAnimSequence.cpp`, `MfAnimEvent.cpp`) or override their headers to include the canonical headers. Format: no file format involved.
- **[crash]** `src/MfActorLib/MfJointKeyframe.cpp:188`, `:196` — `ReadBytes(sizeof(MfJointKeyframe) - 0xc, &mFlags)` assumes the base (`vptr`, `long m_flags`, `unsigned long m_time`) is 0xc bytes. On LP64 the base is 0x18 B and the class 0xe0 B (measured), so the call reads/writes 0xd4 B from +0x18. That is a 12-byte heap overflow per keyframe, plus 12 extra bytes taken from the skill stream, which desyncs the rest. Fix: `sizeof(MfJointKeyframe) - offsetof(MfJointKeyframe, mFlags)` (= 0xc8, all 4-byte float/int fields). Format: keep the on-disk 0xc8-byte record, which this fix does.
- **[crash]** `src/MfActorLib/MfTreeTraversal.cpp:24`, `:31`; `src/MfActorLib/MfProtoTraversal.cpp:11` — the node pointer arrays are allocated as `operator new(n << 2)`, then filled with 8-byte pointers (`MfTreeTraversal.cpp:42`, `MfProtoTraversal.cpp:26`). That is a heap overflow by half the array. (`MfProtoTraversal.cpp:84` is an `int` array and is fine.) Fix: `n * sizeof(*mNodes)`. Format: no file format involved.
- **[data]** `src/MfActorLib/MfStream.cpp:99-103`, `:178-182` — `operator>>(long &)` / `operator<<(long)` move `sizeof(long)` = 8 bytes, while `SizeOf(long)` (`:239-242`) still says 4. Every 32-bit field typed `long` therefore takes 8 bytes in the stream:
  - object record type: `StreamType()` returns `long`, written at `MfStream.cpp:39`, `:47`; read into `long type` at `:283-293`.
  - `MfKeyframe` `m_time`/`m_flags` (`src/MfActorLib/MfKeyframe.cpp:49`, `:56`; fields at `MfKeyframe.h:19-20`).

  Reading the original .act/.skl/property data puts the next field into the upper half of `type`, so `MfStreamConstructorEntry::Find` fails and `ReadObject` returns 0: no actor, skill or property loads. Sized-object sizes from `StreamSize()` (4 per long) no longer match the bytes, so `Seek(size)` skips wrong. Files we write are unreadable by the original, and the desync can escalate to huge counts or allocations. Fix: override both operators to read/write an `int32_t` and sign-extend into the `long` (or make `StreamType()` and the type fields 32-bit). Format: keep the on-disk 32-bit layout and convert at the boundary.
- **[data]** `src/MfActorLib/MfFile_2.cpp:316-332`, `:337-345` — `ReadHeader`/`WriteHeader` use `long header[4]` with `sizeof(header)`, which is 32 bytes against the 16-byte on-disk header. Read: `header[0]` gets magic|format, so the `MF_FILE_MAGIC` test fails and every `MfFile::Open` for reading (sequence/.anm files, `MfAnimSequence`) returns `MF_FILE_BAD_FORMAT`. Write: 32 bytes of header. Fix: `int32_t header[4]`. Format: keep the on-disk 32-bit layout (16 bytes).
- **[data]** `src/MfActorLib/MfFile_2.cpp:162-163` (also `:233`, `:264`; type `src/MfActorLib/MfFile.h:39`, views `MfAnimEventList_unit.h:131`, `MfAnimSequence_2_unit.h:204`, `MfAnimSequence_unit.h:59`) — `MfFileType::m_type` and the local `long type` are 64-bit, but the file holds 4-byte four-character codes that are read with `ReadBytes(4, &type)`. At `:162` the local is uninitialised, so its upper half is garbage, `obj.TypeInFile() != MfFileType(type)` holds, and `MfFile::Read(MfFileObject &)` always fails with `MF_FILE_WRONG_TYPE`. `PeekType`/`ReadType` work only while the target's upper half happens to be 0. Writes (`:210`, `:242`) send the low 4 bytes on little-endian, which is correct. Fix: make `m_type` (and the local) `int32_t` in `MfFile.h` and in all views. Format: keep the on-disk 32-bit layout.
- **[data]** `src/MfActorLib/MfSpline.cpp:279`, `:293` — `MfSplineControlPoint` is read/written raw with `sizeof`, and its first field is `unsigned long time` (`src/MfActorLib/MfSpline.h:21`). On LP64 the record is 0x90 B against 0x88 B on disk (measured): the transforms shift by 4, and every control point after the first desyncs the spline node and the rest of the actor stream. Fix: make `time` `uint32_t` (no other code depends on its width), or read field by field. Format: keep the on-disk 0x88-byte record.
- **[data]** `src/MfActorLib/MfProtoNode.cpp:387` (also `src/MfActorLib/MfProtoNode_2.cpp:425`, `:494`) — `s >> (int &)geom->mTriangles` puts a 4-byte flag into the low half of the 8-byte pointer `MfProtoGeomInfo::mTriangles` (`MfProtoGeomInfo.h:10`). The field is only tested for nonzero (sprite node: `MfProtoNode_2.cpp:211,312`, `MfProtoActor_2.cpp:548,563`). In the old-format reader the struct comes from `new MfProtoGeomInfo` (`MfProtoNode.cpp:384`) with no initialisation, so the upper half is garbage. A plain geometry node then reads as a sprite: it gets a `MfTriSquare` instead of its mesh, and `ComputeBounds` skips it. `MfProtoNode_2.cpp:425` is safe only because `SetStyle` zeroed the field first. `:494` `(int)pointer` is a truncating cast (diagnosed by clang). Fix: read into an `int` and store `(MfTriangleSet *)(intptr_t)v`; write `(int)(mTriangles != 0)`. Format: keep the on-disk 32-bit flag.
- **[data]** `src/MfActorLib/MfFile_2.cpp:387`, `:432`; `src/MfActorLib/MfFile.cpp:156`; `src/MfActorLib/MfFileStream.cpp:105`, `:112`, `:143`, `:151`, `:186` — `ReadFile`/`WriteFile` get an `unsigned long *` (8 B) where `LPDWORD` (4 B) is expected. This does not compile as is. The likely quick fix, an `(LPDWORD)` cast, writes only the low half. At `MfFile_2.cpp:432`, `unsigned long n;` is uninitialised, so `n != count` and every unbuffered `MfFile` write is reported as failed. The other sites are pre-zeroed or reuse values below 2^32, so they would happen to work. Fix: `DWORD` temporaries copied into the `unsigned long` fields. Format: no file format involved.

## MfImageLib

- **[crash]** `src/MfImageLib/MfImageHandler_unit.h:102-110` + `src/MfImageLib/MfImageHandler.cpp:35` — `DllMain` does `new MfJpgHandler` with the registry TU's view `char m_state[0x23c]`, which is vptr + 0x23c = 584 B on LP64. The real class (`src/MfImageLib/MfJpgHandler_unit.h:85-97`) holds `int`, `jpeg_decompress_struct` (600 B on LP64 against 0x1b0, measured), `jpeg_error_mgr` (168 B against 0x84) and `int`, about 792 B. `MfJpgHandler::Handle` (`MfJpgHandler.cpp:31-47`: `jpeg_std_error(&m_jerr)`, `jpeg_create_decompress(&m_cinfo)`) therefore writes about 200 B past the heap block on the first JPEG load. Fix: override `MfImageHandler_unit.h` to include `MfJpgHandler_unit.h`'s real declaration, or create the handler through a factory in `MfJpgHandler.cpp`. Format: no file format involved.
- **[data]** `src/MfImageLib/MfBmpHandler.cpp:37`, `:46` — `fread(&header, sizeof(BITMAPFILEHEADER))`, and `infoSize = bfOffBits - sizeof(BITMAPFILEHEADER)`, both rely on the 14-byte `pack(2)` layout with 32-bit `DWORD` fields. The info block is cast to `BITMAPINFO`, which relies on a 40-byte `BITMAPINFOHEADER` with 32-bit `LONG`/`DWORD`. `port/compat/include/windef.h` does not define these structs yet. A natural-aligned (16 B) or `unsigned long`-based definition shifts `bfOffBits`, mis-sizes the palette and misreads every BMP. Fix: compat definitions with `#pragma pack(push, 2)` and `static_assert(sizeof(BITMAPFILEHEADER) == 14 && sizeof(BITMAPINFOHEADER) == 40 && sizeof(RGBQUAD) == 4)`. Format: keep the on-disk 32-bit layout.

Checked and clean, not listed:
- The TGA header is read field by field.
- `MfImage` holds only `int`s and pointers.
- `src/MfImageLib/jpeg/*` is unmodified IJG 6b, which is LP64-clean: `INT32 = long` only gets wider, `JDIMENSION` is `unsigned int`, and the alignment type is `double`. `jpeg_CreateDecompress`'s structsize check compares against the same compile's `sizeof`.

## PiccoloRT

VM data model on LP64. `MfScDataHandle` (`src/PiccoloRT/MfScDataHandle.h:107-123`) is `unsigned short m_refCount` (+0), `unsigned short m_flags` (+2, a union with a bitfield) and a union `{ float m_float; long m_long; MfScData *m_data; MfScDataHandle *m_pointer; }`. On arm64 the union is 8 bytes and 8-aligned, so it moves to +8 and **sizeof(MfScDataHandle) becomes 16** (32-bit: 8, value at +4). Nothing hard-codes the 8: the handle heaps (`MfScHandleHeap`, `MfScDataHandle_2.cpp`) walk handles as `MfScDataHandle *` and size them with `sizeof(MfScDataHandle)`, the free list goes through `m_pointer`, and no code copies a handle as two DWORDs, an `__int64` or a double. The float/long aliasing is safe in practice: integers are written with `SetInt` (a sign-extended full 8-byte store) and floats with a 4-byte store into a value that `operator new` zeroed (`MfScDataHandle_2.cpp:373`). Every reader truncates to the low 32 bits (`GetInt`, `(int)m_long`, `GetNumber`), and on little-endian those are the right bytes. Handles are always real pointers into the three heaps, never indexes, and no handle is kept in a 32-bit field. The one cost is capacity: a heap reserves `PageSize()*512` bytes (`MfScHandleHeap.cpp:10`), so with 4 KB pages it holds half as many handles as before. **The VM stack word is `unsigned long` / `MfScDataHandle *`, which is 8 bytes on LP64.** `m_sp` is `MfScDataHandle **`, `PushWord`/`PopWord` store `unsigned long`, frames are five words (ip, object, function, constant pool, previous frame) read back as `unsigned long *` (`MfScVmEnv_3.cpp:48-115`), and `m_frame + 7` / `Local(env, 6)` index 8-byte slots. All of these agree, so the stack works as it is, apart from the initial `m_sp` (first bullet). Fix: change those `unsigned long` stack/frame words (MfScVmEnv.h, MfScVmFrame.h, MfScDebugger.h `stackBase`, `MfScDebugInfo::GetVariable`) to `uintptr_t` in an override. That records the intent and keeps working if `long` is ever narrowed. Do not shrink the slots to 32 bits. Bytecode operands hold no pointers: they are 8/16/32-bit immediates, relative jump offsets and 16-bit constant-pool indexes (`Fetch8/16/32`, `MfScVirtualMachine.cpp:130-153`), and loading patches nothing into the code. The constant pool is a heap array of `MfScDataHandle *`. So the bytecode format needs no side table. The real LP64 breakage is in the `.pobj/.pexe` loader, which reads the file through `unsigned long *` everywhere (bullets below).

- **[crash]** `src/PiccoloRT/MfScVmEnv_3.cpp:33` — `m_sp = (MfScDataHandle **)&m_stackBase[committed / 4 - 1];` treats the stack as 4-byte words, but `m_stackBase` is `unsigned long *` (8 bytes). On arm64 the index is in 8-byte units, so `m_sp` starts about `committed` bytes past the end of the committed (and reserved) block, and the first push of every script thread faults. Fix: `committed / sizeof(*m_stackBase) - 1` (or `committed / sizeof(MfScDataHandle *)`) in an override. Format: no file format involved.
- **[crash]** `src/PiccoloRT/MfScPiccoloObjectFile.cpp:156-177, 214, 333-339, 369-389, 402-440, 461, 470-474` (read path: `Open`, `IsValidVersion`, `GrabFunctions`, `PObjModuleRecord`/`ReadModuleNameTable`, `BuildFunction`, both `GetDebugInfo`) — the memory-mapped object file is read through `unsigned long *` and a struct with `unsigned long mSize, mNameSize` (`:369-372`), but every field on disk is 32 bits. On arm64 each read takes 8 bytes. The magic check `header[0] != 0x50534a4d` (`:157`) sees the next field in the high half, so every `.pobj/.pexe` is rejected with kPOFBadMagic. Past that, every `record[n]`, `section + 8` count, `code[0]`/`pool[0]` size and `mName` offset (+16 instead of +8) reads the wrong fields, which means wild offsets, huge memcpy sizes and a crash. `IsValidVersion` happens to give the right answer, but it also reads 8 bytes. Fix: in an override, read the file as `port_u32` (uint32_t) words. Turn `unsigned long *header/record/code/pool/info` into `const uint32_t *` and `PObjModuleRecord` into `uint32_t` fields, and widen offsets to `size_t` only when adding them to `mBase`. Format: keep the on-disk 32-bit layout and convert at the boundary.
- **[crash]** `src/PiccoloRT/MfScPiccoloObjectFile.cpp:630-850` (`WriteClose`) — the mapping is sized from `ComputeModuleNameSectionSize`/`ComputeFunctionSectionSize`/`ComputeObjectFileInfoSectionSize`, which count 4 bytes per field (`:872-945`, `0x38` at `:43`). The writer then stores 8-byte `unsigned long` words through `header`, `moduleSection`, `functionSection`, `record[0..11]` (`record + 12` at `:675`), `*(unsigned long *)p` length prefixes, `var[0..2]`, `info[0..16]` (`memset(&info[7], 0xb0, 10 * sizeof(unsigned long))` at `:841`) and `PObjModuleRecord`. That overruns the mapping, and anything that does get written is not a valid 32-bit `.pobj`. `info[6] = now.Seconds()` (`:840`) also puts a 64-bit `long` time in a 32-bit file slot. Fix: the same `uint32_t` override as the read path, with an explicit `(uint32_t)` on the timestamp. Format: keep the on-disk 32-bit layout and convert at the boundary.
- **[crash]** `src/PiccoloRT/MfScBytecodeRoutine_unit.h:117` (used by `src/PiccoloRT/MfScBytecodeRoutine.cpp:35-42, 57-63, 70-95`) — the unit header's private copy of `MfScBytecodeRoutine` puts `long m_unk24` where the real class (`MfScBytecodeRoutine.h:34`) has `int mFlags`. On arm64 that pushes `m_str28..m_str38` and `m_debugInfo` 8 bytes later than the real `mModuleName..mObjectFileName`/`mDebugInfo` (+72..+104 against +64..+96). Its sizeof is 112 against 104. Every TU but this one sees the real layout: `new MfScBytecodeRoutine` in `MfScDataHandle_2.cpp:300` allocates 104 bytes, and this constructor then writes 8 bytes past the end and leaves `ModuleName()`/`DebugInfo()` pointing at the wrong fields. The destructor, `Attach` and `MemoryUsage` use the wrong offsets too. Fix: build this TU against the real `MfScBytecodeRoutine.h` in the port (override or drop the unit header), or make the placeholder `int`. Format: no file format involved.
- **[crash]** `src/PiccoloRT/MfScLinkData.cpp:3-16` — `MfScLinkData::operator new` returns `&g_MfScLinkDataPool`, a single `void *` (8 bytes). It is used to hold an `MfScLinkData` (vptr + int = 16 bytes on arm64) or an `MfScListLinkData<T>` (24 bytes), so every `CountCircLinks` during container GC (`MfScDataHandle_2.cpp:583`) writes 8-16 bytes over the neighbouring globals (32-bit: 8-12 bytes into a 4-byte slot). Fix: an override with an aligned static buffer that is big enough (for example `alignas(16) static unsigned char pool[64]`) or ordinary heap allocation. Format: no file format involved.
- **[minor]** `src/PiccoloRT/MfScDataHandle.h:86-87, 99-100, 128-130`; `src/PiccoloRT/MfScGCWork.h:14`; `src/PiccoloRT/MfScDataHandle_2.cpp:565, 594-600, 804, 824, 831` — `InterlockedIncrement/Decrement/Exchange` are called on `long` (8 bytes) variables (`lDisableGC`, `lGCTimeOut`, `lGCrunning`, `mQuit`), but the compat API takes `LONG volatile *` (32 bits). Without a cast this does not compile; with a cast it changes only the low half, so a counter that goes below 0 would read as 4294967295 through the plain `long` reads (`IsGCRunning`, `if (lDisableGC)`). Fix: declare them `LONG` in an override. Format: no file format involved.
- **[minor]** `src/PiccoloRT/MfScDataHandle.h:75`, `src/PiccoloRT/MfScDataHandle_2.cpp:331`, `src/PiccoloRT/MfScLinkData.h:11`, `src/PiccoloRT/MfScLinkData.cpp:11` (also redeclared in `src/plugins/PscStdlib/PscStdlib.h:271`) — the class `operator new` takes `unsigned int` as its size, and clang requires `size_t` for that parameter, so these fail to compile. Fix: `size_t` in the override. Format: no file format involved.
- **[minor]** `src/PiccoloRT/MfScDataHandle_2.cpp:142, 154` — `Format("\tHandle 0x%08x...", this, ...)` and `sprintf(address, "0x%08x", m_pointer)` print pointers with `%x`, so the diagnostics show only the low 32 bits. Fix: `%p` (the 16-byte `address` buffer is then too small, so size it to 24). Format: no file format involved.
- **[minor]** `src/PiccoloRT/MfScDataHandle_unit.h:86-88` — the stand-in `MfScVmEnv` uses `long m_unk0[3]` for vptr+pad+`m_ip` and `long m_unk10[4]`. On arm64 that puts `m_stack`(=`m_sp`) at +24 and `m_reg0` at +64, while the real class (`MfScVmEnv.h`) has them at +16 and +56. Today this is latent: the TU (`MfScDataHandle.cpp`) calls none of the inlines, and the port's `dllexport` does not force them out. If something emits them (a use, or a `used` attribute), `GetSPHandle`/`ClearScratchREGn` would be an ODR clash against the real ones. Fix: do not compile `*_unit.h` stand-ins in the port, or delete this placeholder. Format: no file format involved.
- **[minor]** `src/PiccoloRT/MfScPiccoloObjectFile.cpp:34, 40` (`OffsetToAddress`, `AddressToOffset`) — `(int)mBase + (int)offset` and `(int)address - (int)mBase` truncate the 64-bit mapping address. Nothing calls them at present (they are private and unused), but the first caller would get a wrong pointer. Fix: `mBase + offset` / `(uint32_t)((char *)address - mBase)`. Format: keep the on-disk 32-bit layout and convert at the boundary.
- **[minor]** `src/PiccoloRT/MfScVirtualMachine.cpp:1599, 1627`; `src/PiccoloRT/MfIntList.h:12, 19` — `(int)env`, `(int)env->m_ip` and `(int)pli->GetData()` cast pointers to `int`, which clang rejects in C++. At run time they do no harm: the debugger's `int` parameter is unused (`PscDebugger/MfPdbBreakpointManager.cpp:337, 359`), and `MfIntList` only round-trips ints that were stored with `(void *)int`. Fix: `(int)(intptr_t)` / `(intptr_t)` in the override. Format: no file format involved.

## PiccoloComp

Note on code emission (checked, no hazard): operands are built from `int` through `Put8/Put16/Put32` (`MfPcCodeChunk.cpp:560-587`) byte by byte, so they are little-endian, fixed-width and independent of `long`. The constant pool is an `MfScDataHandle *` array that PiccoloRT fills and serializes (`MfPcConstantTable.cpp:76`). The compiler does not emit pointers or `long`s into the bytecode, so all the hazards below come from layouts of foreign classes that are declared again locally, and from the parser.

- **[crash]** `src/PiccoloComp/MfHashTable.h:32-35` — PiccoloComp declares mfutil's `MfHashTable` again with four `int field_4..field_10` in place of the real `int nNumberOfBuckets, int nBucketSize, void **apBuckets, MfChainNode **apmfcnChain` (`src/mfutil/MfHashTable_unit.h:60-63`). On LP64 PiccoloComp computes `sizeof(MfDictionary)` = 24, but the real object is 32. The mfutil constructor then writes `apmfcnChain` at +24, past the end of the object: on the heap at `MfPcSymbolTable.cpp:10` (`new MfDictionary(2000)`) and on the stack at `MfPcCompiler.cpp:667` and `:678` and `MfPcChunkList.cpp:241`. On 32-bit, `int` and pointer had the same size, so this did not show. Fix: an override header that includes mfutil's real `MfHashTable`/`MfDictionary` declaration (or declares the two pointer fields). Format: no file format involved.
- **[crash]** `src/PiccoloComp/MfScPiccoloObjectFile.h:10-32` and the copy in `src/PiccoloComp/CodeLoader_unit.h:212-233` — a second declaration of PiccoloRT's `MfScPiccoloObjectFile`, with invented fields (`void *mFile, *mMapping, *mView; unsigned mSize, mPosition; MfString mObjectFileName; void *mFunctionTable...`) and an inline constructor and `ObjectFileName()`. The real class (`src/PiccoloRT/MfScPiccoloObjectFile.h:93-108`) is laid out differently. On 32-bit the two happened to agree (size 0x38, MfString at +0x18, the zeroed range +0x1c..+0x34). On LP64 they no longer agree:
  - the local copy is 88 bytes and the real one is 96;
  - the inline constructor builds the MfString at +40, while the real `mName` is at +32 and is left unconstructed;
  - the real `mDebugFile` at +88 lies outside the stack object.

  As a result, `Open()` assigns to a garbage `MfStringRef*` and the out-of-line destructor destroys garbage. The stack instances are `MfPcCompiler.cpp:298` and `CodeLoader.cpp:108`. Fix: drop both local declarations (override) and include PiccoloRT's header, with its own inline default constructor. Format: no file format involved (the .pobj writer lives in PiccoloRT).
- **[crash]** `src/PiccoloComp/CodeLoader_unit.h:155` — mfutil's `MfRegularExpression` is declared as `vptr + char m_body[0x80]`, which is 136 bytes on LP64. The real object (`src/mfutil/MfRegularExpression_unit.h:81-88`) holds 2 MfStrings, a `re_pattern_buffer` with 3 pointers, the registers and `char *m_fastmap`, about 168 bytes. The stack instance at `CodeLoader.cpp:32` overflows when the constructor and `Optimize()` run. Fix: override the declaration with the real member list (or make it opaque storage of the real size). Format: no file format involved.
- **[crash]** `src/PiccoloComp/CodeLoader_unit.h:174-176` — mfutil's `MfDirectory` is declared as `MfDblList + int m_10, int m_14, MfDblListItem *m_cursor`. The real class is `MfString m_path; int m_unk14; MfDblListItem *m_pliCur` (`src/mfutil/MfDirectory.h:57-59`). On LP64 the local copy is 48 bytes and the real one 56. The stack object at `CodeLoader.cpp:36` gets overrun by the real constructor/`Open` writing `m_pliCur` at +48. The inline `GetFirstFile/GetNextFile` also keep their cursor at +40, which is the real `m_unk14`. Fix: override with the real members (`MfString m_path` in place of `int m_10`). Format: no file format involved.
- **[data]** `src/PiccoloComp/MfPcParser.h:18-34` against `src/PiccoloComp/MfPcTokenInfo.h:17-21` — `YYSTYPE` is "laid out like `MfPcTokenInfo`" and is aliased through `TOKEN(i) ((MfPcTokenInfo *)&yyvsp[i])` (`MfPcParser.cpp:689`) and `YY_USER_ACTION ... (MfPcTokenInfo *)&yylval` (`MfPcLexer.cpp:367`). On LP64 the value union is 8 bytes, which moves the fields:

  | Field | `YYSTYPE` offset | `MfPcTokenInfo` offset |
  |---|---|---|
  | line | +8 | +4 |
  | startColumn | +12 | +8 |
  | endColumn | +16 | +12 |
  | text | +24 | +16 |

  The effects:
  - `LexMatchedText` (`MfPcCompiler.cpp:440-450`) writes the line through `MfPcTokenInfo`.
  - The grammar actions read `yyvsp[i].line` and get the start column instead. This affects line info, block lines and if/while/switch lines at `MfPcParser.cpp:924,929,938,944,950,959,969,979,993,1055,1066,1077,1092,1101`, so the debug line tables the compiler builds (and the .pdbg) carry columns, not lines.
  - `yyvsp[i].text` (`MfPcParser.cpp:1026,1039,1043,1053,1064,1075,1621`) is never written and is always NULL, so error messages lose the token.
  - The lexer's 8-byte `yylval.sval`/`yylval.str` stores (`MfPcLexer.cpp:537,751`) overwrite `MfPcTokenInfo::mLine` (+4) with the high half of the pointer, so `TOKEN(i)->mLine` for identifiers and strings is garbage.

  Fix: in the override, make `MfPcTokenInfo::mToken` pointer-sized (`intptr_t`), so both layouts put line at +8, columns at +12/+16 and text at +24. Alternatively, replace `TOKEN()` with a struct that embeds the union and then an `MfPcTokenInfo` tail. Format: no on-disk layout change, but the line numbers written into the debug info are wrong until this is fixed.
- **[minor]** `src/PiccoloComp/MfPcLexer.cpp:513,518` and `src/PiccoloComp/MfPcConstSetLexer.cpp:498,503` — `yylval.ival = strtoul(...)`. `unsigned long` is 64-bit, so a hex or octal literal wider than 32 bits no longer saturates to `0xFFFFFFFF` (-1) as on Win32. It silently becomes its low 32 bits. Fix: clamp to `UINT_MAX` (or use `strtoul` through a compat wrapper returning 32-bit with saturation). Format: no file format involved, but compiled constants can differ.

## MfPoLibrary

- **[crash]** `src/MfPoLibrary/MfThread.h:12-18` with `src/MfPoLibrary/MfScThread.h:13,16` — mfutil's `MfThread` is declared again as `vptr + 4 int + unsigned mState + 2 int`, so the inline `MfScThread::GetVmEnv()` reads `mVmEnv` at +40 on LP64. The real `MfThread` (`src/mfutil/MfThread.h:50-56`) is `unsigned long dwThreadId; int iFlags; MfThread *pThread; HANDLE hThread; unsigned uiState; MfEvent *pevtWork; MfThreadWork *pWork`, which puts `MfScThread::mVmEnv` at +64. (PiccoloRT's own copy, `src/PiccoloRT/MfThread.h:33-39`, gives +48, a third answer.) Every script exception thrown from a Po object therefore calls `ThrowException` through a garbage `MfScVmEnv*`. The call sites are `MfPoBase.cpp:56,80,101,123,129,227,264`, `MfPoActor.cpp:95`, `MfPoActivity.cpp:242`, `MfPoActivityWrapper.cpp:139`, `MfPoEvent.cpp:194,196,206,209,218,243,245,255,258` and `MfPoList.cpp:186,201,232,236,256,260,264,291,295,299,311,316,319,340,344`. Fix: one shared `MfThread` declaration with the real pointer/HANDLE members, or make `GetVmEnv()` out-of-line in PiccoloRT (override). Format: no file format involved.
- **[crash]** `src/MfPoLibrary/MfListValue.h:24` against `src/MfActorLib/MfListValue.h:26` — MfPoLibrary sees the trailing member of `MfListValue` as `MfDblListItem *pliCursor` (8 bytes). MfActorLib, which builds most lists (copy constructor `MfActorLib/MfTypedValue.cpp:392`, stream reads, `Copy()` at `MfPoBase.cpp:303,309`), sees it as `int flags` and zeroes only 4 bytes. On LP64 the upper half of `pliCursor` is heap garbage. `MfPoList::PMDelete` tests `if (mList->pliCursor)` and then calls `GetCurrent`/`Delete` on it (`MfPoList.cpp:212-216`), and `SearchListForItem` passes it as the search start (`MfPoList.cpp:361`). Both use a garbage list item. Fix: make MfActorLib's member a pointer-sized cursor (one shared header) so every constructor zeroes 8 bytes. Format: no file format involved (`flags` is not streamed).
- **[crash]** `src/MfPoLibrary/MfPoBase.cpp:179` (va_arg `const type_info*`) with callers that pass a literal `0` as a variadic pointer: `MfPoActivity.cpp:453`, `MfPoActor.cpp:1193,1673,2642,2662,2670,2719,2747,2756,2827,3148,3224,3341`, `MfPoRotation3.cpp:267,293` and `MfPoTransform.cpp:286`. A bare `0` in `...` is an `int`. On Darwin arm64, variadic arguments go to 8-byte stack slots with the upper 4 bytes unspecified, so `va_arg(ap, const type_info*)` can read a non-null garbage pointer. `InvalidType` then dereferences it (`*want != got`, `want->name()`, and `getType` at `MfPoBase.cpp:134-141`). The crash comes on the script-error path, while the original message is being reported. Fix: pass `(const type_info *)0` (or `nullptr`) at those call sites through an override, or read the slot as `intptr_t` and test it for 0 in `InvalidType`. Format: no file format involved.
- **[minor]** `src/MfPoLibrary/MfHookData.h:23-31` against `src/RuntimeLib/MfHookManager.h:135-145` — MfPoLibrary's `MfMiscHookData` has `MfAiActorNode *pNode` at 0x18, where RuntimeLib has `int m18`. On LP64 this shifts `kind`, `str`, `value`, `property` and the transform by 4 to 8 bytes relative to RuntimeLib's view. Any hook consumer of `MF_HOOK_MISC_DATA` (0x1b) built against RuntimeLib's struct would read the wrong `kind` and a misaligned `MfString` from the hook MfPoLibrary sends at `MfPoActorNode.cpp:350-357`. No consumer is registered in the shipped code, so this is latent. Fix: one shared definition with a pointer-sized slot at 0x18. Format: no file format involved.
- **[minor]** `src/MfPoLibrary/MfPoSystem.cpp:110,115` — `(int)memory.dwTotalPhys` / `(int)memory.dwAvailPhys`. The compat `MEMORYSTATUS` fields are 64-bit `SIZE_T`. On a Mac with 8 or 16 GB the truncation gives 0 or a negative number, while Win32 `GlobalMemoryStatus` clamps. Scripts reading `System.totalMemory` or `availMemory` get nonsense. Fix: clamp to `0x7fffffff` (in the compat `GlobalMemoryStatus` or in an override). Format: no file format involved.
- **[minor]** `src/MfPoLibrary/MfLock.h:12,18` — `long mLockCount` is passed to `InterlockedExchange(LONG volatile *, LONG)`. Compat `LONG` is `int`, so `long *` does not convert and the inline constructor fails to compile. If someone "fixes" it with a cast, only 4 of the 8 bytes are exchanged. Fix: declare the member as `LONG` (mfutil's `lwLockWord` needs the same change, so the sizes stay equal). Format: no file format involved.
- **[minor]** `src/MfPoLibrary/MfScDataHandle.h:72` — `static void *operator new(unsigned int, int)`. On LP64, the first parameter of `operator new` must be `size_t` (64-bit), so clang rejects this declaration. Even if accepted, it would not match PiccoloRT's definition. Fix: `size_t` in both declarations (override). Format: no file format involved.

## plugins/PscStdlib

- **[crash]** `src/plugins/PscStdlib/PscStdlib.h:176, 189` (used at `:250-253` `ThrowScriptException` and `src/plugins/PscStdlib/MfPlMath.cpp:142, 146, 160, 164, 178, 182, 196, 200, 214, 218, 232, 236, 250, 254, 272, 281, 297, 301, 317, 321, 337, 341, 366, 369, 374`) — the plugin's own view of `MfThread` is `char m_pad[0x20]`, which puts `MfScThread::mVmEnv` at +0x20. On arm64 the real `MfThread` (`src/PiccoloRT/MfThread.h`: vptr, 3 ints, `void *mHandle`, 3 ints) is 48 bytes, so `mVmEnv` is at +48. Every script error a native function reports therefore loads `mState`/`field_18` as an `MfScVmEnv *` and crashes. Fix: include the real `MfThread.h`/`MfScThread.h` or call `MfScThread::GetThreadVmEnv()` in an override instead of hard-coding the 32-bit padding. Format: no file format involved.
- **[crash]** `src/plugins/PscStdlib/PscStdlib.h:494-501` (allocated at `MfPlList.cpp:159, 162, 165, 168, 171, 174, 175`, `MfPlDictionary.cpp:58, 59, 62, 65`, `MfPlLogger.cpp:37, 40`, `MfPlMath.cpp:94, 97, 100, 103, 106, 109, 112, 115, 116, 119, 122, 125`) — the plugin declares `MfScMethodParameterInfo` as base + `int m_8; int m_c;`, which is 24 bytes on arm64. The real class (`src/PiccoloRT/MfScClassInfo.h:64-71`: `MfString m_name; int m_optional`) is 32 bytes, so the plugin's `new` allocates 24 and PiccoloRT's constructor writes `m_optional` at +24, past the end of the block (heap corruption). Fix: declare the field as `MfString` + `int` (or include the real header) in an override. Format: no file format involved.

## plugins/PxtMath

- **[crash]** `src/plugins/PxtMath/PxtMath_10001000_unit.h:68-69` (used at `src/plugins/PxtMath/PxtMath_10001000.cpp:99, 103, 119, 123, 139, 143, 159, 168`) — `MfThread` is declared as `char m_pad[0x20]; MfScVmEnv *m_env;`, so the VM env is read from +0x20. On arm64 the real `MfScThread::mVmEnv` is at +48, so every `ThrowException` path of Sinh/Cosh/Tanh/Hypot dereferences garbage. Fix: use the real `MfScThread` layout or `MfScThread::GetThreadVmEnv()` in an override. Format: no file format involved.
- **[crash]** `src/plugins/PxtMath/PxtMath_10001000_unit.h:82, 101-103` (allocated at `src/plugins/PxtMath/PxtMath_10001000.cpp:189` `new MfPxtMath`) — the plugin's `MfScData`/`MfScDataObject` are vptr + `int m_4` + `MfString *m_className` + `int m_c, m_10`, which is 32 bytes on arm64. The real `MfScDataObject` (vptr, `MfLock` (pointer), `MfString *`, `MfScHandleDict *`, `unsigned mFlags`) is 40 bytes. `new MfPxtMath` allocates 32, and the imported `MfScDataObject(0x200)` constructor writes `mFlags` at +32, past the end. Fix: declare `void *` + `unsigned int` for `m_c`/`m_10` (or include the real header) in an override. Format: no file format involved.
- **[crash]** `src/plugins/PxtMath/PxtMath_10001000_unit.h:139-145, 147-157, 160-168` (allocated at `src/plugins/PxtMath/PxtMath_10001000.cpp:46, 48, 50, 51, 53, 54, 56, 57, 59, 60`) — the plugin stand-ins for `MfScMethodParameterInfo` (`int m_8, m_c`), `MfScCiMethod` (`int m_c, m_10, m_14`) and `MfScClassInfo` (`int m_10, m_14`) use 32-bit ints where the real classes have `MfString`/pointer members. On arm64 they are 24/40/40 bytes against the real 32/48/48, so each `new` here is 8 bytes short and PiccoloRT's constructors (`m_optional`, `m_params`, `m_items`) write past the end of the block. Fix: declare the real member types in an override. Format: no file format involved.

## plugins/PscCompiler

- No LP64 hazards. The module is `src/plugins/PscCompiler/PscCompiler_10001000.cpp` (3 exports, 44 lines) plus its header. `DllMain(void *, unsigned long reason, void *)` uses `long` only as an in-memory parameter. `PscInitialize(int, MfString *)` and `PscShutdown(MfString *)` pass only an MfString pointer and call `MfPcCompiler::Initialize/Shutdown`. There are no casts, no I/O and no structs. Checked for pointer/int casts, `long` widths, varargs and layout declarations.

## plugins/PscContentInfo

- **[crash]** `src/plugins/PscContentInfo/PscContentInfo.h:418-419` (used at `src/plugins/PscContentInfo/PcContent.cpp:188, 195, 221, 228, 261, 268, 283, 290, 297, 304, 311`) — `MfThread` is declared as `char m_pad[0x20]; MfScVmEnv *m_env;`, but on arm64 the real `MfScThread::mVmEnv` is at +48. Every script error path of the Content object crashes. Fix: use the real `MfScThread` layout or `MfScThread::GetThreadVmEnv()` in an override. Format: no file format involved.
- **[crash]** `src/plugins/PscContentInfo/PscContentInfo.h:432, 451-453, 710` (allocated at `src/plugins/PscContentInfo/PcContent.cpp:9` `new MfScDataObject(0x400)` and `:162` `new PcContent`) — the plugin's `MfScDataObject` is 32 bytes on arm64 (`int m_4`, `MfString *`, `int m_c, m_10`); the real one is 40. `new MfScDataObject(0x400)` allocates 32 and the real constructor writes `mFlags` at +32, past the end. In `PcContent`, `m_content` (+0x14 in 32-bit, here +32) sits on the real `mFlags`: the base constructor's flags (0x200/0x400, which decide the GC heap and container handling) are overwritten by a pointer, and later flag updates corrupt `m_content`. Fix: declare `void *` + `unsigned int` (or include the real `MfScDataObject.h`) in an override. Format: no file format involved.
- **[crash]** `src/plugins/PscContentInfo/MfContent.cpp:176, 196` — `SetWindowLong(hDlg, GWL_USERDATA, lParam)` stores the `MfContent *` (from `DialogBoxParam(..., (LPARAM)this)` at `:238`) in a 32-bit `LONG`, and `(MfContent *)GetWindowLong(...)` reads back a truncated pointer in `WM_COMMAND`. Fix: `SetWindowLongPtr`/`GetWindowLongPtr` with `GWLP_USERDATA` (both in compat `winuser.h`) in an override. Format: no file format involved.
- **[crash]** `src/plugins/PscContentInfo/PscContentInfo.h:501-507, 509-519, 521-529` (allocated at `src/plugins/PscContentInfo/PcContent.cpp:82, 88, 94, 100` and the further `*ci + new MfScCiMethod(...)` lines after them) — the stand-ins for `MfScMethodParameterInfo`, `MfScCiMethod` and `MfScClassInfo` use `int` members where the real classes have `MfString`/pointers. On arm64 they are 24/40/40 bytes against the real 32/48/48, and PiccoloRT's constructors write past the plugin's allocation. Fix: declare the real member types in an override. Format: no file format involved.

## 64-bit survey report

Counts (the survey modules only; zlib and plugins/PscCompiler are clean):

| Module | crash | data | minor | total |
|---|---|---|---|---|
| mbruntime | 1 | 1 | 2 | 4 |
| zlib | 0 | 0 | 0 | 0 |
| mflinalg | 2 | 0 | 0 | 2 |
| mfutil | 3 | 2 | 1 | 6 |
| MBCom | 1 | 0 | 1 | 2 |
| MfActorLib | 6 | 6 | 0 | 12 |
| MfImageLib | 1 | 1 | 0 | 2 |
| PiccoloRT | 5 | 0 | 6 | 11 |
| PiccoloComp | 4 | 1 | 1 | 6 |
| MfPoLibrary | 3 | 0 | 4 | 7 |
| plugins/PscStdlib | 2 | 0 | 0 | 2 |
| plugins/PxtMath | 3 | 0 | 0 | 3 |
| plugins/PscCompiler | 0 | 0 | 0 | 0 |
| plugins/PscContentInfo | 4 | 0 | 0 | 4 |
| **total** | **35** | **11** | **15** | **61** |

The VM data model holds on LP64. `MfScDataHandle` becomes 16 bytes (it was 8), the stack word
becomes 8 bytes, and nothing assumes either old size. Bytecode carries no pointers. What breaks
is the code around the VM: the object-file loader, the initial stack pointer, and the plugins'
views of the thread object.

Top 10 hazards:
1. **[crash]** `src/PiccoloRT/MfScPiccoloObjectFile.cpp:156-474, 630-850`: the `.pobj/.pexe`
   reader and writer use `unsigned long` for 32-bit fields. Every object file fails the magic
   check, so no script loads. Fix: `uint32_t` words in an override.
2. **[crash]** `src/PiccoloRT/MfScVmEnv_3.cpp:33`: `committed / 4` indexes 8-byte stack words,
   so the first push of every script thread faults.
3. **[crash]** The plugins and MfPoLibrary find the script's VM env through a 32-bit view of
   `MfThread`: `PscStdlib.h:176`, `PxtMath_10001000_unit.h:68`, `PscContentInfo.h:418` and
   `MfPoLibrary/MfThread.h:12`. They read it at +0x20 or +40, while PiccoloRT's copy puts it at
   +48 and mfutil's real class at +64. Every script exception raised in native code calls
   through garbage. There should be one shared `MfThread` declaration.
4. **[data]** MfActorLib streams: `MfStream.cpp:99,178` move 8-byte `long`s, `MfFile_2.cpp:316`
   has `long header[4]`, `MfFile_2.cpp:162` has a 64-bit `MfFileType`, and `MfSpline.cpp:279`
   reads raw records. No actor, skill, property or animation file loads. Fix: 32-bit fields at
   the stream boundary.
5. **[crash]** `src/mbruntime/MBWinDebug.cpp:17`: `(va_list)(&fmt + 1)` is not a va_list on
   arm64. MBDebugStr logs from DllMain in every MB DLL, so it fails at load. Fix: `va_start`.
6. **[crash]** Some objects are allocated from an undersized view and then built by the real
   constructor, which writes past the end. In MfActorLib these are `MfFileStream`, `MfAnimFile`,
   `MfCamera` and `MfCopyFolder` (`MfAnimSequence_2.cpp:121,213,241,292`, `MfCamera.cpp:29`). In
   MfImageLib it is `MfJpgHandler` (`MfImageHandler.cpp:35`, about 200 B over). In PiccoloComp
   it is `MfDictionary`, `MfScPiccoloObjectFile`, `MfRegularExpression` and `MfDirectory`. In
   the plugins it is `MfScDataObject`, `MfScMethodParameterInfo` and `MfScClassInfo`. Fix:
   include the real class headers in the port.
7. **[crash]** `src/MBCom/MBCom.h:31-59`: `MBGUID::data1` is `unsigned long`, so the GUID is
   24 bytes and `operator<` reads padding. Factory lookups miss, and `MBComFactory::Make`
   returns null.
8. **[crash]** `this` pointers are stored in `GWL_USERDATA` through `SetWindowLong`, which
   truncates them: `mfutil/MfNameDialog.cpp:52`, `MfKeyDialog.cpp:43`, `MfLoggerUI.cpp:56,474,558`
   and `PscContentInfo/MfContent.cpp:176`. Fix: `Set/GetWindowLongPtr`.
9. **[crash]** mfutil's wide strings: `MfStringRef.cpp` hands 16-bit strings to the 4-byte
   `wcslen`/`mbstowcs`, and `DuplicateUnicode` overflows the heap by 2x. `MfString.h` uses
   `wchar_t` where every consumer expects 16-bit units. Fix: 16-bit helpers throughout.
10. **[crash]** `src/mflinalg/MfMath.cpp:44-51, 172-178`: float bits go through
    `unsigned long *`, an 8-byte access to a 4-byte float. The sqrt table and `MfMath::sqrt` are
    wrong and the stack gets clobbered. Fix: `uint32_t` with memcpy.

Also crash-level, outside the top 10: the keyframe and pointer-array sizes in MfActorLib
(`MfJointKeyframe.cpp:188`, `MfTreeTraversal.cpp:24`), `MfScBytecodeRoutine_unit.h:117`,
`MfScLinkData::operator new`, and the bare `0` passed as a variadic pointer in MfPoLibrary.

## Startup sequence

What the game does from launch to the first screen (read from the decompiled source; file and
key names as the code spells them; the paths are DOS paths relative to the current directory
unless said otherwise).

1. **Steamboat.exe, WinMain** (src/Steamboat/GameApp.cpp)
   - Single instance: mutex `OneMuffinAtATime`; a second copy brings the window of class
     `IDC_STEAMBOAT` (string resource) to the front and exits.
   - `MBComInitialize`, `MBComLoadServer("mbengine.dll")` (fails: message box "Could not find
     the engine component").
   - Current directory = the directory of the executable (`GetModuleFileName`, cut at the last
     `\`, `SetCurrentDirectory`).
   - Reads `engine.ini` (MBOpenFile mode 5 = read, must exist; through IMBPref):
     `[Screen] EnableDirect3D` (default 0), `Fullscreen` (1), `DDEnumCallback` ("");
     `[Boot] GameTitle`, `Game` (the game DLL, default `gamex86.dll`).
   - `LoadLibrary(<Game>)` and `MBComLoadServer(<Game>)`; looks up the exports
     `OnMBEngineCreateScreen`, `OnMBEngineStartup`, `OnMBEngineShutdown` and hands them to the
     IMBGameEngine, then `FreeLibrary` (the COM load keeps it).
   - DirectDraw screen manager (`EnableDirect3D`, full screen or windowed), a 640x480 client
     window, `game->CreateScreen`, Miles sound manager on the window, `game->Init()`,
     `game->Start()`, then the message loop: `Idle()` whenever no message is waiting and the
     window is active; `IsQuitting()` ends it.
   - 64-bit: `SetWindowLong(hWnd, GWL_USERDATA, (LONG)game)` and WndProc's
     `(IMBGameEngine *)GetWindowLong(...)` keep the engine pointer in a 32-bit window long; the
     port needs a pointer-sized slot (SetWindowLongPtr semantics).
2. **SteamboatApp, OnMBEngineCreateScreen** (dllmain.cpp): requests screen 1, 640x480, 16 bit.
3. **OnMBEngineStartup**: `SimpleNode::Startup`, creates the CSBNodeManager and adds node `MV`
   (the movie node; codes: MV movie, UI main menu, CP control panel, HK soup recipe, MP map,
   CD credits, TB/GL/LL/SB/SH/SM minigames, anything else a SteamboatNode scene).
4. **First node start, SteamboatNode::OnStart → Init()** (once per run):
   - `SBDialoguePropertyManager::Create`.
   - `ReadPrefs`: `steamboat.ini`, `[directories] hd` and `cd` (both default to the current
     directory), `fly` (default `fly`), `mofa` (default `mofa`), `muffin` (default
     `resources`): the scene graph directory is hd+fly, the Motivate data directory hd+mofa,
     the resource directories cd+resources and hd+resources (the strings are concatenated as
     they are, so the ini values carry their own trailing `\`). `[sound] sfx volume`,
     `dialog volume`, `music volume`; `[i18n] locale` (default `eng_usa`); the inventory and
     recycle item lists; `[AbbreviationMap] key0, key1, ...`.
   - Resource databases `GD`, `PR`, `MV`: SteamboatNode::AddDatabase(name) adds
     `universal\<name>` and `<locale>\<name>`; each is looked for under the CD resource
     directory, then the HD one, as `<name>.mbd` (zip, CMBResourceZipfile) and as a directory
     `<name>` (CMBResourceTree). Resources are found by MBHashString(name) and a four-character
     type (`'anm '`, `'smk '`, `'bik '`, ...).
   - String table `steamboat.stl`, locale from steamboat.ini.
   - CreateDisplay: screen 1's DirectDraw/D3D surfaces; `engine.ini [Screen] ForceSoftware`
     forces the software (system memory) renderer. 640x480x16, one back buffer, flipping.
   - Scene graph: DXGfx::NodeFactory with file header `*VisFly Project #00.25  bin*`,
     Graph::DataBase, RenderManager view 0 on the display surface.
   - Deletes `Steamboat.log` and `SteamboatD.log`.
   - Motivate (SBMotivateDriver::Init): `MfInitializeRuntime`, `Initialize(license)`, a world
     with data path hd+mofa, `LoadSpace("steamboat")`. The runtime loads its plugins from the
     directory in the `PLUGINS` environment variable, else the registry string
     `HKEY_CURRENT_USER\Software\MotionFactory\<app name or "Global Data">\PATHS\PLUGINS`
     (mfutil UtilityApp, app name = the executable's file name), else `PLUGINS` under the current
     directory: `psc*.dll` (Piccolo script libraries: PscStdlib, PxtMath, PscCompiler,
     PscContentInfo) and `hoo*.dll` (hook managers).
   - Font `Arial_Black_12` for the debug text line; random seed from the game clock.
   - **CD check** (VerifyCD): adds database `bk`, opens resource `BK_window` of type `'anm '`;
     if it is missing, shows string `NO_CD` (title `ERROR`) and exits. So the CD check is just
     "the bk database is reachable through the cd or hd resource directory".
5. **Then each node's start**: AddDatabase(<space code>), cursor anim `GDcursor`, the scene's
   hotspots from `<space code>scene.ini`.
6. **First screen**: SBMovieNode with previous node "none" plays `disneybumper` (PlayMovie
   tries a Smacker resource of that name, then a Bink one, from the databases), then goes to
   `UI`, the main menu (SBMainMenuNode). From there: `gamestate.ini`, `SGDB.ini` (saved game
   database) and `savedgame<N>.ini` through SteamboatNode::CreatePrefAndStream, in the current
   directory (created when missing for the saves); a new game plays `intro_mic`
   (`intro_min` for Minnie) and continues in `MX` (`HL`).
- Other registry use: MBEngine's MBGetDesktopDirectory reads
  `HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders\Desktop`.

What must work first for the port: the current directory set to the install directory,
`engine.ini` and `steamboat.ini` read through MBOpenFile/IMBPref, case-insensitive lookup of
the `resources\universal\GD.mbd`-style paths (DOS separators, CD and HD roots), the zip
resource reader, the `PLUGINS` directory (or an environment variable for it), Smacker for the
bumper movie, and the `bk` database for the CD check.

