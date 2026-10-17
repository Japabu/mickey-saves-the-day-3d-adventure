# MBEngine: source patterns that match (VC6 /O2)

Read this before a new MBEngine unit. Every item was verified by match.sh in the
unit named in brackets. Add a line when you find a new pattern; keep it short.

## Declarations to copy, not re-derive
- `MBGUID` and `IMBUnknown` (with `MBIID_IMBUnknown`, the first GUID of every unit's .rdata):
  include `../MBCom/IMBUnknown.h` where the unit's GUIDs start. Interface classes, inline global
  `operator new`/`delete` over `extern "C" __declspec(dllimport)` `MBmalloc_release` /
  `MBfree_release`: CMBWinClock.h.
- `MBComPtr<T>` with `CreateObject`, `Release` inline in the class and
  `_QueryInterface` declared only (calls go to the out-of-line copy): MBSoundUtils.h,
  CMBKeyboardDispatcher.h. Local pointers use the `MBComPtr(const MBGUID&)` ctor.
- GUID values: the `MBIID_*` constants are per-unit `static const` (PDB `ldata`); read
  the 16 bytes from the DLL (`match.Image(...).u32(va)`, addresses from
  `pdbtypes.py data`). Known: CMBKeyboardDispatcher.h (event manager/listener,
  keyboard), CMBMemoryBlock.h, CMBResourceStreamLocator.cpp (`MBIID_CMBResourceMgr`).
- Classes declared in more than one unit have one header each, `<Class>.h` (interfaces, MBPoint/MBRect/
  MBColor..., the owned classes; `<X>_unit.h` holds the rest of a unit header whose name is a class's).
  Include it where the declaration goes; it needs ../MBCom/IMBUnknown.h and the `MBIID_*` its GetGUID returns
  declared first (still per unit, in the unit's .rdata order), as its first comment lists. A copy marked
  `// own copy:` is a second version this unit's matches depend on; keep it.

## Code shapes
- `Make<C>` (not a singleton): `IMBUnknown *p = new C; if (p) p->AddRef(); return p;`
- `Release`: `unsigned long r = --_mRefCnt; if (!_mRefCnt) delete this; return r;`
  (test the member, not `r`) [CMBWinClock, CMBResourceStreamLocator].
- `QueryInterface`: compare `MBIID_IMBUnknown` first, then the specific interface;
  some classes use a run of `if (iid == X) p = this;` then
  `if (p) { AddRef(); *obj = p; return 0; } return -3;` [CMBResourceStreamLocator].
- Constructor store order: members in the init-list are stored before the vptr, body
  assignments after it [CMBMemoryBlock, CMBResourceStreamLocator, CMBKeyboardDispatcher].
- `return p ? p->Get() : 0` as a tail call needs a result variable:
  `id = 0; if (p) id = p->GetID(); return id;` [CMBSoundDescription].
- A member read through a call may need a local copy first (`GetParent`) and operand
  order matters (`mPosition + mParent->GetPosition()`) [CMBObject].
- `cmp`/`lea` select: an inline `min` returning `const T&` [CMBMemoryBlock].

## Types and names
- `_N` in a mangled name is `bool`; the PDB listing prints `unsigned char`: use `bool`.
- Exported C functions (`MBPlaySound` etc.) and the GUID constants they use need
  `extern "C"`. `MBComUtilAddSingleton` is `extern "C"` without dllimport (called
  through a thunk).
- `$E<n>` static initializers come from the STL headers (`<string>`, `<fstream>`):
  include the header the original evidently used; the numbering must agree (match.sh
  checks it). `--sym` needs the leading underscore (`_$E20`).
- Closed with stand-ins (one `sUnknown...` each): MBSoundUtils, CMBFileStreamLocator,
  MBMilesSoundEngine (IMBStreamLocator), symtable (TSymtab), CMBWinDebugConsole.
- `$E` counter, what else takes a number: implicitly instantiating a class template that has static
  data members, at its first use (MBString = `basic_string<char,traits,MBAllocator<char> >` gives +1
  for npos, even from an unreferenced inline or a class member of that type; a pure virtual returning
  it by value does not; `std::string` is already counted). Not counted: constant-initialized local or
  file statics, extern declarations, `= 0` on static member definitions. A dynamic local static of
  a class type with a ctor gives +2 (guard + its `$E`). Check the obj's PDB data (`$S` guards, statics)
  first [CMBFileStreamLocator, MBSoundUtils, symtable].
- Still open [CMBFileStreamLocator, MBSoundUtils]: PDB data has no guard/static, so the +1 is
  header-borne. Every STL unit that includes the IMBStreamLocator header (these two and
  MBMilesSoundEngine) has it, and no baseline (`$E19`/`$E18`) unit does. That header pulls in no STL
  (CMBResourceStreamLocator/CMBSoundDescription include it and have no `$E`), so the likely cause is a
  static data member declaration of a class that is never used there; its name is unknown.
  symtable: one item beyond TSymtab's 2 statics, also not in the PDB data.
- A member `MBComPtr<T>` gives the original ctor order (zeroed before the vptr) and the
  dtor's tail-call Release; `operator=(T*)` keeps old, stores new, AddRefs new, Releases
  old [CMBFileStreamLocator].
- `MBDebugStr` (import, cdecl): `extern "C" __declspec(dllimport) void MBDebugStr(const char *)` [stack].
- Union-array store then `cursor++` as a separate statement: float gives `inc [mem]`, short/ptr give load/inc/store; pop is `cursor--; *p = a[cursor].x;` [stack].
- `MBList<T*>` (CMBResourceMgr.h `MBAllocator`) reproduces ctor/dtor/push_back/erase inline; `std::find(...) == end()`
  then `push_back` matches, but "find and set" is a hand loop with `*it = 0; return;` inside [CMBPeriodicalManager].
- `$E20`/`$E19` gap closes when the unit has a `Make<C>` singleton (`static IMBUnknown *instance = ...`) plus
  `<fstream>`, as in CMBResourceMgr [CMBPeriodicalManager].
- Scalar deleting dtor with `test esi, esi` before `MBfree_release`: the unit's inline
  `operator delete` has `if (p)` [MBMilesStreamingSoundEngine].
- Miles (mss32) calls: `extern "C" __declspec(dllimport) ... __stdcall AIL_*`, `HSTREAM` =
  `_STREAM *`; declarations in MBMilesStreamingSoundEngine_unit.h [MBMilesStreamingSoundEngine].
- `bool r = false; if (a == b) r = true; return r;` gives the `xor bl,bl; mov al,1; je`
  shape [MBMilesStreamingSoundEngine].
- `??_G` with `test esi,esi` before `MBfree_release`: `operator delete(void *p) { if (p) MBfree_release(p); }` [MBSoundEngine].
- Inlined `MBComPtr(guid)` ctor with out-of-line `Release` calls on both QI paths (`push esi` keeps
  the new pointer): `_QueryInterface` uses `Attach(T *p) { Release(); mInterface = p; }` for success
  and `Attach(0)` for failure, with `Release` inline in the class (inline budget runs out) [MBSoundEngine].
- QueryInterface may test only its own IID (no `MBIID_IMBUnknown`); still the
  `p = 0; if (iid == X) p = this; if (p) ...` shape (gives the `test esi` on this) [CMBWorld].
- Lazy COM member: `IMBUnknown *u; MBComCreateInstance(0, clsid, &u); if (u) { u->QueryInterface(iid,
  (void **)&mMember); u->Release(); }` with `u` uninitialized; dtor `if (mRoot) mRoot->Release();` [CMBWorld].
- `MBRect` is all-inline (no out-of-line copies): default ctor zeroes, `operator=` with self
  check (`SetBounds`), plain copy for return by value; inline `Intersect` uses ref-returning
  max/min (`lea` select). Declarations: CMBWorld.h [CMBWorld].
- Const getter forwarding to a virtual: `return ((C *)this)->GetRoot();` gives `jmp [eax+N]` [CMBWorld].
- `$E` numbering: every static *data member declaration* in the TU (even unused, no
  definition) takes one number before the STL `$E`s; static member functions, classes,
  inlines, `static const` GUIDs and extra #includes do not. Baseline `<string>` = 18/19
  [symtable: TSymtab's 2 statics give 20/21; original 21/22, gap still open].
- Recursive tree code (Mak-style `TSymtabNode`): a tail-recursive `right->Print()` becomes
  a loop; `new T(args)` with inline `operator new` gives the null-checked ctor call;
  a cursor parameter advanced in place (`buffer += 4`, keep `start`) beats a copy [symtable].
- `return p ? p->F(x) + 4 : 4;` (ternary) gets the original register choice where
  `if (p) return ...; return 4;` swaps esi/edi [symtable].
- Ctor that calls `MBComPtr` without the leading `Release` call: default ctor + explicit `p.CreateObject(guid)`
  (the `MBComPtr(guid)` form leaves `Release` out-of-line); `~MBComPtr() { Release(); }` (store 0 after) [CMBWinDebugConsole].
- `list::erase` emitted out-of-line: call `pop_front()`, not `erase(begin())` (that one inlines) [CMBWinDebugConsole].
- BOOL API to `bool` with `test/setne`: `return f(...) ? true : false;` (`!= 0` gives neg/sbb/neg) [CMBWinDebugConsole].
- Undecorated file-scope helpers in the PDB (`SetBit`, `TestBit`): `extern "C" { static ... }`, `--sym _SetBit` [MBAnimFrame].
- Property get/set: `long result = 0; switch (p) { case ...: break; default: result = -11008; } return result;` [MBAnimFrame].
- Open gap [CMBWinDebugConsole]: all `$E`s one short (Make `$S20` vs `$S21`); 6 class statics + Make static leave one
  number unexplained before Make. Code of the 6 `$E`s is byte-identical.
- Squish types (`SquishPixel`, `SquishImageDescriptor`, `SquishRect`, `SquishImageBlock`/`Frame`/`Movie`,
  `MBVector<T>` over `std::vector<T, MBAllocator<T>>`): Squish.h. `<string>` gives the $E19/$E18 pair [Squish].
- Switch case bodies are laid out in source order: read the jump-table targets to recover the case order
  (Squish pixel formats are 1,2,8,16,32,4,64,128) [Squish].
- `unsigned long result = 0; switch ... break; return result;` gives `push esi; xor eax,eax; lea esi,[edx-1]` [Squish].
- `and reg, 0x3ffffffe` after `shr 2` = `((unsigned long)b >> 3) << 1`; an int operand gives `and -2` [Squish].
- Iterator loops with `*it++` inside the call/assignment (`(*it++)->Remap(...)`, `block = *it++`) give the
  `add esi,4` before the call and `this` spilled to `push ecx` [SquishImageFrame].
- Search loop without `++it` (original bug) reproduces the `cmp edi,ecx; je; cmp esi,eax; jne` self-loop [SquishImageFrame].
- A block-scoped local can reuse a dead parameter's stack slot (`mov [esp+argN], -2`) [SquishMovie].
- `$E` numbering: each `std::map`/`MBMap` instantiation adds 2 (`_Tree::_Nil`, `_Nilrefs`); three MBMaps in
  CMBBaseSoundManager give `$E24/$E25` with `<string>` [CMBMilesSoundManager]. No STL header adds exactly 1.
- Singleton with failure path: `if (instance) { delete (C *)(IMBSoundManager *)instance; instance = 0; }` (guarded,
  reset inside) matches the shared reload; casts through the primary interface avoid the ambiguous IMBUnknown [CMBMilesSoundManager].
- `MBList` pop: `x = front(); pop_front();` calls the out-of-line `iterator::operator++(int)` (inline depth), while
  `it = erase(it)` loops inline it fully [MBSoundChannel].
- MBComPtr `Release` inline + dtor `{ Release(); }`: VC6 still calls Release out of line from CreateObject in big
  functions (ctor/dtor with two ComPtrs), inlines it in small ones [CMBMilesSoundManager]; in MBMilesSoundEngine::LoadSound
  only a declared-only Release reproduces the call.
- `if (call()) {...} flag = false;`: a store after the if (not inside) — check the je target [MBMilesSoundEngine::Play].
- `MBString` (`basic_string<char, char_traits<char>, MBAllocator<char>>`, `deallocate` with `if (p)`) and the
  IMBFileSpec/IMBFile/IMBStream/IMBWinFile vtables (IMBFileSpec `SetPath` overloads reversed): CMBWinFile.h. A named
  `MBString fullPath = spec->GetPath();` plus a bool/long result variable gives the dtor tails [MBFileSystemWin].
- `<windows.h>` + `<string>` gives the original `$E20`/`$E19` numbering (no extra static needed) [CMBWinFile, MBFileSystemWin].
- Member `MBComPtr` dtor that stores 0 after Release: `~MBComPtr() { Release(); }` (dead store kept for members,
  dropped for locals/temps). `MBComPtr(T *, bool addRef)` local over a raw out-param gives the register copy +
  duplicated epilogues (CMBWinFile::Clone, return inside the branch) [CMBWinFile].
- Uninitialized switch result read on `default` = reload from the param slot (`int seekOrigin;` no default) [CMBWinFile].
- Inline depth: `MBComPtr<T> data(guid)` inside an inline helper (Grow) leaves `CreateObject` out of line; an
  out-of-line `_QueryInterface` appears when reached via the guid ctor in a plain function [CMBMemFile, MBFileSystemWin].
- `operator=(T *p) { if (m != p) { old = m; m = p; if (p) AddRef; if (old) old->Release(); } }`: `= 0` gives the
  "store 0 inside the branch, then Release" shape (Reset/_QueryInterface null path) [CMBMemFile].
- Inline helper failure tails: `if (!mData) { create } else if (mData->Grow()) {...; return true;} SetLastError; return false;`
  keeps the create-path dtor before SetLastError and the original block order [CMBMemFile].
- The original `MBAllocator` has no `construct`/`destroy` (PDB lists none): units whose containers emit
  `std::_Construct` out of line need an allocator without them (MBAnimSequence_unit.h, MBAnimTracker_unit.h) [MBAnimSequence].
- `std::map` code with `_Lockit` in `_Inc`/`_Dec`/`_Lbound` needs `/MT` (`_MT` enables the locks) [MBAnimTracker].
- Out-of-line range `_Tree::erase` from a dtor: `if (m.size() > 0) m.clear();` (direct `erase(begin(), end())`
  inlines it) [MBAnimTracker].
- Header colour statics (`kCM_*` MBColorMask, `kWhite`.. MBColor; 16 `$E` pairs per unit): MBScreenUtils.h. Both classes assign
  members in red, blue, green, alpha order; `MBColor` ctor calls an inline `Convert(kCM_ARGB32)` [MBScreenUtils].
- `bool result = false; ... if (x) { ...; result = true; } return result;` keeps `bl` and the two-exit shape [MBScreenUtils].
- `MBPoint() { x = y = 0; }` gives the y-then-x zero stores [MBMouseUtils].
- `$E` shift by one before the colour statics: the MBMsgBox declaration's default `title = MBString16()` instantiates
  the string (npos) before the header statics [MBMsgBoxWin]. One-case `switch (r) { case IDOK: return ...; }` gives `dec/je`.
- MBComPtr template ctor from another interface (`template <class U> MBComPtr(U *const &)`) gives the VC6 mangled
  `_QueryInterface@...ABQAVIMBScreen@@@Z`; its null path is `*this = 0` (store before Release) [MBMsgBoxWin].
- MBColor header (`MBColorMask`, `kCM_*` masks, `kWhite`..`kGrey75` colours, all `static const` with ctors = 16 `$E`
  pairs $E1..$E47, MBColor ctor inlines `Convert(kCM_ARGB32)`), `MBRect(MBPoint3, MBSize)`, `IMBSurface` vtable:
  CMBSurfaceObject.h. Store order follows "member order R,B,G,A" in Set/ctor (2nd statement hoisted otherwise) [CMBSurfaceObject].
- A zeroing `MBPoint3()` default ctor is `x = y = z = 0;` (stores z,y,x) [CMBSurfaceObject]. `MBRect` needs its user copy
  ctor, or `return MBRect(...)` copies through a temp with extra registers [CMBColorBackgroundObject].
- A non-trivially-dead base vptr store survives in a ctor when a global is read after it (`mColor(kBlack)`) [CMBColorBackgroundObject].
- STL `_Tree` code with `_Lockit` calls needs `/MT` (no `/MT` = no locks); `MBMultimap<K,V,Pr>` over
  `std::multimap<..., MBAllocator<V>>`: CMBEventManager.h. The tree helpers (erase/_Insert/_Inc/...) then match for free [CMBEventManager].
- Inline budget seems to be spent bottom-up: the *later* of two identical expansions in a function gets fully
  inlined, the earlier one stays a call (BroadcastEvent loops, MakeFont's two `MBComPtr<U>` QIs) [CMBEventManager, CMBFPSMonitor].
- Erase while iterating: `next = iter; ++next; erase(iter); iter = next;` (not `erase(iter++)`) [CMBEventManager].
- `MBComPtr(const MBComPtr<U> &)` + member-template `_QueryInterface(const MBComPtr<U> &)` (Attach(0)/Attach(p), null
  path `*this = 0`) mangles as a plain overload; colour statics go *before* `<string>` (their $E1..$E47 come first) [CMBFPSMonitor].
- Open [CMBFPSMonitor]: Make guard `$S63` = colours(48) + `<string>` baseline(14) + 1; closed with a declared-only static
  data member placeholder in CMBFPSMonitor.h (real name unknown).
- Stand-ins: when the PDB numbering (`$S<n>`/`$E<n>`) proves one more numbered item
  than the source has and no evidence names it, a declared-but-never-defined static
  data member may stand in. Name it `sUnknown...`, comment the evidence, and only for
  the numbering; `grep -r sUnknown src` lists them for replacement [CMBFPSMonitor].
- Header color statics (`kCM_*` MBColorMask, `kWhite`.. MBColor; 16 objects = `$E1..$E48`, 3 numbers each) plus
  inline MBColorMask/MBColor, screen interfaces, CMBDirectDrawScreen and DirectDraw 7 decls (SDK headers are DX5):
  CMBDirect3DScreen.h. Include it *before* `<string>`: STL numbers then follow (ctype `$E66/$E67`) [CMBDirectDrawScreenManager].
- `<vector>` alone gives no ctype `$E`. Vector insert calling out-of-line `std::_Construct`: MBAllocator without
  `construct`/`destroy` (CMBDirectDrawScreenManager.h) [CMBDirectDrawScreenManager].
- Failure paths that jump to one shared `return 0` epilogue: nest the success path (`if (hr >= 0) {...; return 1;}`)
  and end with `return 0;`; two same-named block-scoped structs need their own `{}` to share a stack slot [CMBDirect3DScreen].
- Find-loop with the found block after the loop and a shared epilogue (`je exit; jmp top`): named local
  `MBRect r = ...GetGlobalBounds(); if (r.Contains(p)) {...; return;}` with an inline `Contains` using
  `if (...) return true; return false;` [CMBDirectDrawScreenManager].
- Colour statics (`kCM_*` MBColorMask x5, `kWhite`.. MBColor x11, `$E1..$E47`): internal-linkage `const` objects with inline
  ctors; MBColorMask::Set stores masks r,b,g,a and shifts `GetShiftCount+GetBitCount-8` (shift loop `while (!(m&1)) { if
  (count >= 32) break; ...}`); MBColor::Set stores r,b,g,a; Convert = `mColor = 0; if (s > 0) mColor |= (c << s) & m; else
  mColor |= (c >> (char)-s) & m;` per channel (r,g,b,a); `(char)` gives `neg cl`. Declarations: CMBView.h [CMBView].
- `MBRect(const MBPoint &, const MBSize &)` ctor reproduces the bounds store order (+8,+0,+4,+c) [CMBView].
- `a + (b - c)` with MBPoint: name the inner temp (`MBPoint offset = b - c; return a + offset;`) [CMBView].
- MBComPtr where `operator=` calls Release out of line in a big loop function (Idle): Release `{ if (m) { m->Release(); m = 0; } }`
  (store inside the if), dtor `{ if (m) m->Release(); m = 0; }`, guid ctor for the first ptr (CreateObject stays out of line);
  ctor/dtor use default ctor + `CreateObject` (inlined) [CMBViewManager].
- Colour header + `<string>` + `<vector>` + a `Make` singleton gives ctype `$E67/$E68` and guard `$S62` [CMBViewManager].
- Out-of-line `MBComPtr::_QueryInterface` (hr kept in edi, `mov edi,-3; mov eax,edi`, `pop ebx` before `mov eax,edi`):
  `long hr; if (p) { hr = QI; if (hr) { Release()/Attach(0); return hr; } Attach(pI); } else { *this = 0 (store
  before Release); hr = -3; } return hr;` - only the failure path returns early; the success and null paths share the
  final return [CMBDirectDrawScreenManager, MBMsgBoxWin, MBSoundUtils, MBFileSystemWin].
- CMBDirect3DScreen.h now has the full IDirectDraw7 vtable to SetDisplayMode, `bool CreateSurface`, and MBColor
  `GetRed/GetGreen/GetBlue/GetAlpha` inlines; DirectDrawCreateEx/EnumerateExA/IID_IDirectDraw7 decls: CMBDirectDrawScreen.h [CMBDirectDrawScreen].
- A secondary-base virtual (`ListenToEvent` of IMBEventListener at +4) is compiled with `this` = subobject (offsets -4) for free [CMBDirectDrawScreen].
- Open [CMBDirectDrawScreen]: ctor/dtor want `_QueryInterface` out of line while Flip/ListenToEvent inline it with Release
  out of line; `_QI` with `Attach` matches Flip, `_QI` calling `Release(); mInterface = x;` directly matches ctor/dtor.
- Reset members after the if (`if (h) {...DeleteObject(h);} hOld = 0; h = 0;`) gives the `xor edi,edi; cmp [mem],edi` shape [CMBWinDraw].
- `RGB(c.GetRed(), c.GetGreen(), c.GetBlue())` via inline getters gives the param-slot spill of red (`mov [esp+8], cl`) [CMBWinDraw].
- `mWinDCPtr = target` (IMBUnknown *): member template `operator=(U *const &)` -> inline `_QueryInterface` (Attach(0)/Attach(p),
  null path `*this = 0` with an out-of-line `AddRef()` member) [CMBWinDraw].
- DD7 surface vtable through Unlock (+0x80), IMBWinDC/IMBWinDIBSurface/IMBMemorySurface, MBColorMask/MBColor with default args
  (PDB `__dflt_ctor_closure`), MBBlit* (return `bool`): CMBDirectDrawSurface.h. Default `MBColor()` = `mColor = 0; Set(...)` with
  `Set` calling `Convert(kCM_ARGB32)` as a statement [CMBDirectDrawSurface].
- `return hr >= 0 ? 0 : -10002;` gives `setl; dec; and 0x2712; add -10002` (the `<` form gives setge) [CMBDirectDrawSurface].
- `a = b = x` stores b first; for low-then-high order write `a = x; b = a;` (DDCOLORKEY) [CMBDirectDrawSurface].
- Inline budget is top-down in an if/else: the earlier `Set` inlines fully, the later one gets out-of-line calls [CMBDirectDrawSurface].
- MBComPtr `_QueryInterface` reached via an inline guid ctor stays out of line while `CreateObject` called directly inlines it:
  guid ctor written as `{ IMBUnknown *unk; MBComCreateInstance(...); if (unk) { _QueryInterface(unk); unk->Release(); } }` with
  `_QueryInterface` declared only, `CreateObject` doing the QI inline (`if (hr) Attach(0); else Attach(p);`) [CMBGameEngine].
- VC6 rejects a copy ctor declared *after* `template <class U> MBComPtr(const MBComPtr<U> &)` (C2535); declare the template first [CMBGameEngine].
- `$E`/`$S` stand-ins: CMBGameEngine has 2 numbers before the colours ($E16) and 2 after (ctype $E70/71) with `<string>` first;
  CMBNodeManager 2 before Make (guard $S64, ctype $E69/70). Declared-only static members stand in [CMBGameEngine, CMBNodeManager].
- `std::deque` in `MBQueue<T>` (= `std::queue<T, std::deque<T, MBAllocator<T> > >`): inline element ctors (NodeAction) are what
  lets push_back's `_Construct`/copy expand like the original; out-of-line ctors defined in the .cpp do not [CMBNodeManager].
- `list::erase(begin())` inlines (with `??_G` of the element), `pop_front()` calls erase out of line; operator delete with `if (p)`
  for `??_G` [CMBNodeManager]. Debug loops `MBDebugStr(fmt, ++i, (T *)*it++, typeid(T *).name())` give `it` advanced before the call.
- `static MBPixelFormat sCached...` (MBColorMask default ctor `Set(0,0,0,0)`) defined after colours + `<string>` gives `$E63/$E64`,
  ctype `$E70/$E71`; no stand-in needed. `SquishStream(const CMBStreamSwapLSB &)`: the PDB's by-value parameter stops VC6
  inlining the ctor. Loop `Read(&a[i++], size)` order: `p = &a[i++]; size = 2; Read(p, size);` [CMBFont].
- Colours + `<windows.h>` + `<string>` + MBString + one static data member gives ctype `$E68/$E69`. With `<windows.h>` before
  IMBFont, every `DrawText` is `DrawTextA` (vtable slots too). Wide-text wrappers: `MBString str;` before the convert call,
  then `str = p; delete p;`. Nested API call as argument: hoist it (`LCID l = LOWORD(GetKeyboardLayout(0));`) [CMBWinSystemFont].
- Pixel fetch `(pitch*y + x*4) + bits` (`lea; mov [eax+edx]`): put the offset in the result variable first,
  `value = GetPitch() * y + x * 4; value = *(unsigned long *)(bits + value);` (direct forms reassociate) [CMBWinDIBSurface].
- `switch (bpp) { case 16: pd.Convert(m); break; case 8: case 24: case 32: break; default: return -10002; }` + shared tail
  gives the byte-table jump and the duplicated tail of SetTransparentColor [CMBMemorySurface].
- `CreateEmpty(r->Width(), r->Height(), s->GetFormat())` (inline MBRect Width/Height) computes the sizes to stack temps
  before the call; plain `right - left` does not [CMBWinDIBSurface]. `return p ? p->x : 1;` loads into eax directly.
- VC6 calls a non-template `_QueryInterface(IMBUnknown *const &)` plus a member template `_QueryInterface(U *const &)`
  ambiguous: give the one source pointer type its own non-template overload (CMBMouseMonitor.h) [CMBMouseMonitor].
- `return p ? p->F().z : 0x7fffffff;` in a static inline helper taking the pointer by value reproduces the arg copy +
  two-ComPtr shape (CompareMouseablesGreaterThan); if/else assignments do not [CMBMouseMonitor].
- `if (a) {...; return;}` then the else body unbraced (not if/else) gives the duplicated world-dtor epilogue (CheckRollovers) [CMBMouseMonitor].
- Out-of-line `MBComPtr<T>::_QueryInterface` (113 bytes, `mov edi,-3; mov eax,edi`, `pop ebx` before `mov eax,edi`) written
  inside the class: `if (p) { hr = QI; if (hr) { Attach(0); return hr; } Attach(pInterface); } else { *this = 0; hr = -3; }
  return hr;` (the if/else `Attach` form is 13 bytes off) [CMBMouseMonitor, CMBDragObject].
- `MBPoint3 &operator+=` for `pos += parent->GetPosition()` and `mOffset += point` (Move); component math as
  `MBPoint3 newpos = base; newpos.x += a - b; ...` (not ctor/operator+) [CMBDragObject].
- `return a != b` with an `||` chain operator!= gives `xor bl,bl / mov bl,1` (operator== negated gives sete) [CMBDragObject].
- Find+erase loop with shared epilogue: inline `MBPoint::operator==` written `if (...) return true; return false;` [CMBDragObject].
- CMBDragObject ctype `$E67/$E68` is one above the colour+<string>+<list> baseline: stand-in `sUnknownDragObjectNumbering` [CMBDragObject].
- Two IMBObject subobjects (IMBAnimButton + IMBObjectGroup bases): pass `(IMBAnimButton *)this` to ObjectMoved/RemoveObject
  (original pushes the +0 pointer). Switch `sub eax,0; je; dec; je; dec; jne` = separate identical case bodies (merged
  `case 1: case 2:` gives a range test) [CMBAnimButton].
- `bool result = false; if (p) { MBRect b = ...; result = b.Contains(pt); } return result;` gives the early `xor al,al` [CMBAnimButton].
- Early `return false;` guards (not one nested if) give a separate `xor al,al` epilogue; name `long x = ..., y = ...;` before a
  call taking both differences to compute them before the pushes [CMBAnimButton::IsClicked].
- Open [CMBAnimButton]: SetAnim wants the first `mgr.CreateObject` (IMBResourceFileList) out of line, the later mAnimation one
  inlined; ours inlines both. Stand-in `sUnknownAnimButtonNumbering` closes ctype `$E67/$E68`.
- `npos` compares: write `pos != -1` (gives `cmp edi,-1`); `MBString::npos` loads the static. `const char *` globals
  (`kMBDirectorySeparator`, mangled `3PBDB`) are `extern const char *`, not `*const` [CMBWinFileSpec].
- Byte stores into a dword via a separate down-counting index (`((char *)&type)[j]`, `j = 3; j--`) give `[esp+edi+N]` [CMBWinFileSpec].
- MBString npos numbering: a `= MBString()` default argument in a header interface (IMBPref) instantiates it early and
  shifts a later static's `$E` by one; drop the default when the static must come first [CMBUniFont].
- `find(':')` then the cursor init: `end = s.find(':'); start = 0;` (init after the call) [CMBUniFont].
- Char-to-wide wrappers: `MBString16 unicode; MBString utf8(text); MB_utf8_to_ucs2(text, unicode); return F(..., unicode.c_str(), ...);`
  (extern "C" `MB_utf8_to_ucs2(const MBString &, MBString16 &)`, temp from `text`) [CMBUniFont].
- STL without exceptions: the muffin code compiled the library with `_TRY_BEGIN`/`_CATCH` as plain blocks (no /GX, yet
  `basic_string::_Copy` has no EH frame and is inlined into `_Grow`). Include `MBStl.h` before any STL header in a unit whose
  string `_Grow`/`_Copy` (or anything that inlines them) must match [SimpleNode's MBString/MBString16 `_Grow`/`_Copy` verified].
- `delete p;` with an inline null-checking `operator delete`: the plain delete-expression drops the inner check
  (`if (p) delete p;` keeps both) [mbruntime MBDebugHeapTerminate].
- Inline budget: a postfix `it++` in a loop costs more than `++it`; it is what leaves a later `begin()` out of line in the
  member map's dtor [MBCom ~MBComFactory].
- `DllMain` (Runtime/Win/MBWinDllMain.cpp) is the same source in every MB DLL: MBEngine/MBWinDllMain.cpp includes
  ../mbruntime/MBWinDllMain.cpp with `MBRUNTIME_IMPORT` [MBWinDllMain].
- CVTUTF.c is Unicode, Inc.'s 1996 CVTUTF.C, compiled as C; constants are external `const UCS4` globals, the tables
  non-const [CVTUTF].
- Switch with a byte index table: write the cases in the order of the *return values* (case bodies are laid out in source
  order) and let `default` share the final return [MBKeyboardWin].
- Global `operator new`/`delete` defined inline in a header are still emitted out of line in every object (replaceable
  functions), so their unreferenced copies in an obj need no extra source [MBCom, SquishDiff].
- Unreferenced out-of-line copies of a class template's inline member beside fully inlined uses: an explicit
  instantiation `template class X<T>;` emits them [SquishDiff].
- Pointer walks: declare the running pointers first (`const T *s = ...; T *d = ...;` in the original's load order), keep
  row starts as block locals and step them after the inner loop (`s = srcSOL + rowbytes;`); a plain `while (n--) *d++ = v;`
  becomes `rep stos` (8-bit: the memset expansion) [SquishDiff, SquishBlit].
- Bulk reuse: many `$E<n>` (colour-static initializers, ctype) and template instances are byte-identical to another
  unit's. Compile each matched source once and test every open function against it (`--sym _$E<n>` for initializers);
  the numbering check in match.sh keeps it honest. Run match-all at -j 4 or less: -j 10 gave spurious failures.
- Loop-generated units (`O_std_...` placeholder structs) rewrite well from a hand-made header of another unit with the
  same `$E` numbering (CMBChooseSaveFile.h from CMBTextControl.h: colours $E15.., ctype $E67/$E68) [CMBChooseSaveFile].
- Overloaded virtuals: VC6 lays each overload group out in *reverse* declaration order; IMBPref declares
  GetString(MBString16)/SetString(MBString16) before the MBString versions [CMBPref].
- Tree iterator loops: an increment in the `for` header (`++it`/`it++`) leaves `_Inc` out of line; a statement
  `it++;` at the end of a `while` body inlines it (with `_Min`). Capture before advancing: `MBString *value =
  (*k).second; k++; delete value;` [CMBPref CleanUpTree, SquishPalette, CMBResourceTree].
- `map.clear()` and `map.erase(map.begin(), map.end())` inline differently: the explicit erase reloads begin()/end()
  for its test (and may call `begin()` out of line for the return value) [CMBResourceTree/CMBResourceZipfile Close];
  `clear()` keeps them CSE'd [SquishPalette EndStreamOut].
- PDB-truncated claim names (255 chars + 8 hex) that two overloads share must be written out in full (get the object
  symbol); check.sh still resolves a call from one to the other by the truncated prefix and can pick the wrong one
  (CMBPref `_Tree::erase(iterator, iterator)` left as TODO for that reason).
- Squish.h `MBAllocator` has no construct/destroy (SquishPalette's maps need `std::_Construct`; the other Squish units
  are unaffected). SquishPalette.h: `MBMap` + the `SquishPaletteManager` (all inline) [SquishPalette].
- Block-scoped locals reuse dead parameter slots: `{ unsigned long seed = mSeed; file->WriteBytes(&seed, 4); }` frees
  the slot for a later local [SquishPalette StreamOut]; a nested `if (!found) {...} return palette;` fixed the esi/edi
  choice where early returns did not [SquishPalette Create].
- Distance sums: `long delta = a - b; long d = delta * delta; delta = ...; d += delta * delta;` (squares computed as
  they go); declare `bestDistance` before `bestIndex` for the store order [SquishPalette GetNearestIndex].
- Shared failure tails (`MBDebugStr(...); block->Release(); block = 0;` reached from two checks) come from two
  sequential `if (block && !ok)` / `if (block)` blocks, not if/else [CMBResourceTree/Zipfile LoadResource].
- MBComPtr with `CreateObject` calling an inline `_QueryInterface(IMBUnknown *const &)` (Attach(0)/Attach(p)) gives the
  out-of-line `Release` calls inside an inlined CreateObject; QI written directly in CreateObject inlines them
  [CMBResourceTree/Zipfile Open]. Bytes read field by field through a local raw `IMBStream *in = mStream;` keep the
  stream in a register [CMBZipEntry LocateStartOfZipEntry].
- zlib from MBEngine: `#define ZLIB_DLL` + `_WINDOWS` before ../zlib/zlib.h (WINAPI entry points through thunks)
  [CMBResourceZipfile].
- `x = y = f()` with the value reused: `mDataOffset = entry.fileDataOffset = GetPosition(); return true;` inside the
  branch keeps eax live [CMBZipEntry LocateStartOfZipEntry]. Inline helpers defined *after* the caller in the .cpp
  are not inlined (write the two stores of RingBuffer::Clear directly in Init).
- Default-then-override results: `short volume = 0x7f; if (found) volume = (*it).second; return volume;` keeps the
  default in a callee-saved register and duplicates the return (`mov ax,[..]` vs `mov ax,bx`); an early
  `return (*it).second;` folds the constant. Same for `channel = 0; ... { channel = *it; break; } return channel;`
  [CMBBaseSoundManager]. `iterator it = m.find(k); if (it == m.end())` evaluates find before end().
- TExecutor VM comparisons: `Push((float)(d1 == d2 ? 1 : 0))` (int 1/0 in the dead param slot, fild); math ops
  `Push((float)sin(d1))` with <math.h> give fsin/fcos/fsqrt [exec].
- Script units (CMBScript, exec) share CMBScript.h: `<string>`, stand-in statics (4) + the runtime's static members
  (TSymtab 2, TExecutor::handlers 1) before the colour statics give `$E21`; one stand-in after them; `static
  MBAnimTracker sAnimTracker` + a keeper struct whose ctor does `sAnimTracker.AddRef()` give `$E72..$E79` [CMBScript, exec].
- Channel loops: `int i = 0; while (i < 5) broadcaster->AddListener(kChannels[i++], this);` (signed pointer compare,
  this-adjust before the element load) [CMBScript StartListening].
- `Execute`: the `if (!exec->ok && !mIsWaiting) done = true;` test sits after the guarded block, not in it [CMBScript].
- VM handlers pop in the script's push order, not the callee's parameter order: decode the ftol push order to map
  pops to arguments (StartSound pops force, queue, volume, channel, sound, loop, wait) [exec].
- flex 2.5 scanner (scriptc): tables from the binary, `YY_BREAK` after *every* rule action (also after `return`):
  the dead `break`s are what give the original inline budget (UpdateCharacterCount inlined in exactly the first
  38 actions). File-scope statics of the skeleton (yy_get_next_buffer...) have undecorated PDB names: `extern "C"
  static`. Older flex text: "fatal flex scanner internal error--no action found" [scriptc].
- Script codegen: AddBackpatch/StartFunctionBlock *prepend* (`list.insert(list.begin(), node)`); EndFunctionBlock is
  `for (it...; it++) { ...; it = list.erase(it); it--; }`; `map.erase(begin(), end())` (not clear()) [scriptc].
- MBFileUtils.h: `MBComPtr` with a copy ctor and `operator=(const MBComPtr &p) { return *this = p.mInterface; }`:
  vector insert/fill then call the out-of-line `??4(T *)` like the original. Both `newSpec` locals use the guid ctor;
  VC6 inlines `CreateObject` in the second and calls it in the first. The PDB's `_stricmp` callee is the old name
  `stricmp`. `MBGetDesktopDirectory` tests `path[MBstrlen(path)]` (original bug) [MBFileUtils].
- Depth dispatch `if (depth != 8 && depth != 16 && depth != 32) return false;` then an `if/else if` chain gives the
  original's threaded re-test (32 jumps to the `== 16` compare). Advancing the `void *` parameter itself
  (`dest = (unsigned char *)dest + pitch`) keeps the load-test-load-test order of the null checks; locals load both
  first [MBMemoryBlit].
- Fill loops: 8-bit rows `for (y = height; y; y--)`, 16/32-bit rows `for (y = height; y--; )`, pixels
  `for (n = width; n--; ) *d++ = v;` (gives `lea ecx,[ebp]` before the rep stos) [MBMemoryBlit MBFill].
- Squish.h: `ImageDataPtr` (inline: `mData = 0; if (mBlock) mData = mBlock->Lock();`, dtor `Unlock()`) is the
  lock-guard behind every `test esi,esi; je; call Lock` / `Unlock` pair; Lock/Unlock are defined at the end of the
  unit so they stay calls. `GetTransColor()` is `SquishPixel(TransEnabled() ? bgra32 : 0xffffffff)` (the re-test
  through a second base register survives). `SetCompFmt` as two statements (`flags &= ~mask; flags |= f;`) gives
  `and ch,0xf1; or` instead of the xor merge [SquishImageBlock].
- A function whose first test jumps forward over the bigger block: invert the condition (`if (comp != 0x200)
  {tempBlock path; return} ...`) [SquishImageBlock Load].
- Functions with `__try` keep dead stores: the `MBComPtr` dtor `{ Release(); }` shows its `mInterface = 0`, an ignored
  `key = "..."` result is stored to a temp (so `MBString key; key = "x";`, not `MBString key("x")`), and every
  `CreateObject` result is stored. WinMain's string/COM-pointer templates and ctype `$E68/$E69` come with its body
  even while WinMain itself stays TODO [Steamboat GameApp].
- SimpleNode.h is the real engine header (colours, MBStl.h + <string> + <map>, interfaces, MBComPtr with guid ctor
  `{ CreateObject(clsid); }`, CreateObject calling an inline 113-byte `_QueryInterface`). Out-of-line ComPtr templates
  (CreateObject, _QueryInterface, Release) then match for free [SimpleNode, ScriptHelpNode].
- Return a ComPtr filled from a raw out-param: declare the result first (`MBComPtr<T> r;` = register 0), fill a raw
  pointer, `r.Attach(raw)`, `return r;` (gives AddRef into the return slot, then the release of r after the other
  locals) [SimpleNode GetStream, CloneAnim].
- `mWorld->AddObject(anim)` through IMBWorld's inline `AddObject(o) { GetRoot().AddObject(o); }` (PDB lists it) loads the
  argument before the GetRoot call; `mWorld->GetRoot().AddObject(anim)` loads it after [SimpleNode CreateAnim].
- `if (a && b->Load(...) == 0) {...} else p.Release();` gives the shared Release for the null and the failure path [CreateAnim].
- Inline budget: PDB-declared inline helpers that the code evidently used (ScriptManager::AddScript/RemoveScript/
  FindScript) restore the original out-of-line calls (lower_bound, operator++(int), find) in their callers [ScriptHelpNode].
- An empty `if (channel == -6) {} else if (channel == -7) ...` keeps the -6 compare; an empty switch case does not [ScriptHelpNode].
- `while (it != end) (*it++).second->Stop();` gives the copy-then-increment (ebx = old node) with the inline ++ [ScriptManager].
- `$E` order: define a class's static data members (`MBLocale SimpleNode::sLocale;`) before a class whose multimap
  member instantiates `_Nil`/`_Nilrefs`, or the map's two numbers come first [SimpleNode $E66..$E70, $E74..$E77].
- User dtor of a static object with a map: `for (it = m.begin(); it != m.end(); it++) delete (*it).second;
  m.erase(m.begin(), m.end());` [SimpleNode ~DatabaseMgr in $E75].
- Template RLE decoders: an explicit `template class SquishDecode<T>;` emits the out-of-line copies; declare the
  `T count;` run length outside the literal/repeat branches (slot choice) [SquishDecode, 16-bit RLEDecode].
- Inline budget, code-neutral: `void Release(void) { do { if (mInterface != 0) mInterface->Release(); } while (0);
  mInterface = 0; }` in every MBComPtr copy (MBComPtr.h too) is net positive (+7, none broken): same code, more IL
  nodes. In SimpleNode.h also `Attach(T *p) { do { Release(); } while (0); mInterface = p; }` (Database ctor; the
  same change elsewhere only breaks), in CMBPref.h a do/while around `if (old) old->Release();` in operator=(T *)
  (SetString). Wrapping QI, AddRef, the dtor, CreateObject's Release or unk->Release gained nothing [wave 5].
- With that Release, ~DatabaseMgr needs the iterator declared in the for (`for (DatabaseMap::iterator it = ...`) [SimpleNode $E75].
- SimpleNode.h template `_QueryInterface(const MBComPtr<U> &)`: `do { *this = 0; } while (0);` on the null path (same code)
  keeps it out of line in CreateSystemFont, which then needs `MBComPtr<IMBFont> font; font.CreateObject(...)`
  (the guid ctor stays a call). Applied to every header it only breaks CMBWinDraw [SimpleNode].
- DatabaseMgr::AddDatabase tries all four (zip/dir x CD/HD) without early returns and ends with
  `while (it != end) (*it++).second->AddDBRef(); return true;` [SimpleNode].
- operator=(T *) that calls the member `AddRef()` (PDB has it) matches the out-of-line AddRef call seen in Initialize.
- scriptc yyparse is bison 1.25 (bison.simple, `_alloca` + `__yy_memcpy` stack growth, YYTRANSLATE default 122, YYNTBASE 85,
  YYFINAL 311, YYLAST 1051); tables are copied from the binary. The skeleton compiles to the same shape; the actions use
  inline evaluators (`NumAdd(a, b) { Write((short)vmAdd); return a + b; }` etc.): the operands are copied to frame slots
  before the opcode write, which only parameters of an inline function explain. Open: which Write(short) sites stay calls.
- A search loop that runs at least once (`AnimLowMemHandler`) is a `do { ... } while (!result && n < 4);`; a `for`
  with the test in front adds the initial check [CMBAnim].
- Operand order of compares and sums follows the load order: `CurrentFrameNum() == NumTotalFrames()` (cmp eax, edi),
  `mPos + mParent->GetPosition()` (parent's value loaded first) [CMBBinkMovie, CMBAnim].
- Stream reads `in->Read(&x, size)` whose `size` sits in dead slots (even the stack slot of the enregistered `in`
  parameter) are block locals: `{ unsigned long size = 4; ... }` per group [CMBAnim Load, 7 bytes left].
- Two pointers into the same call (MB_utf8_to_ucs2): name the c_str() first (`utf8char`) and copy it into the
  cursor variable, as MB_ucs2_to_utf8 does; that fixes the slot order [mbi18n].
- `MBComPtr::_QueryInterface` from a raw pointer is a member template (`template <class U> long _QueryInterface(U *const &p)`):
  MBPrintingWin calls `?_QueryInterface@?$MBComPtr@VIMBDirectXScreen@@@@AAEJABQAVIMBScreen@@@Z` with `&screen` and no
  temporary. A non-template `IMBUnknown *const &` parameter makes VC6 copy the derived pointer to a temp (+4 frame), and
  a template next to a non-template overload is ambiguous (C2668). VC6 mangles member templates like plain overloads.
- A file-static C function shows up in the PDB with its bare name (`GetDefaultPrinterDC`): declare it
  `extern "C" static` and claim it as `_GetDefaultPrinterDC` (match.py accepts the C underscore).
- MBPrintSurfaceOneCopy: the QueryInterface failure jumps to the shared `return 0` (nest the body in
  `if (QI == 0) {...}`), `DOCINFO docInfo = {sizeof(DOCINFO), "Print", 0, 0, 0}` gives the original store order, and
  `(x + 31) / 32 * 32` compiles to shr/shl where `& ~31` gives `and`.
- Template and STL instances that our compile never emits (every call inlined) get a comment-only TODO line in the
  unit the PDB names, with "not emitted" as the reason; the body is the template in the header or the VC6 STL.
- Squish codecs (worker B): the SquishDecode/SquishEncode<T> instance TODO lines sit directly
  above the in-class template member, so they count as written (their body is the template).
  RLESkipLines starts with `if (lines == 0) return src;` (the original's early ret); the clipped
  runs test `pixelsToSkipNear > 0` / `pixelsToSkipFar > 0` (jbe, not je). Reviewed EQUIVs: a lea
  copy where VC6 uses mov, a dead parameter slot, independent descriptor loads.
- The remaining 0.6% not written are STL/MBComPtr instances our compile never emits (inline
  budget); each has a "not emitted" TODO line in the unit the PDB names.
- IMBEventManager's two RemoveListener overloads: MSVC puts overloaded virtuals into the vftable
  in reverse declaration order, and the original calls RemoveListener(long, IMBEventListener *)
  through slot 0x18, so it is declared first (as in CMBDirectDrawScreen.h). Seven unit headers had
  the other order; ~CMBNodeManager and ~CMBDirectDrawSurface called the wrong slot (fixed).
- logicdiff's `str:'L'` for our `""` literals (CMBUniFont, CMBPref) is a reading artifact: the
  sources pass "" and the original's ??_C@_00A@ is the empty string.
