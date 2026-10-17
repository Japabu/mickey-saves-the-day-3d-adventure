# SteamboatApp notes

Source patterns known to match (vc6 `/O2 /GX /MT`).

## Scene library (EAIStd, Utilities, Graph)

- PDB header paths: `sdk/scene/EAIStd/Text.h`, `EAIStd/vector.cxx` (Vec<T>), `Utilities/StdBase.h`,
  `Utilities/ValList.h`, `Utilities/StdString.h`, `muffin/include/Runtime/MBNew.h`.
- `BasicObject`, `StdBase`: constructors and the empty virtual destructors are inline. An
  out-of-line `??1BasicObject`/`??1StdBase` is only the emitted inline copy; match it from any
  unit that emits it (Text.cpp, Graph_BasicNode.cpp).
- `Vec<T>` (Vec.h): every member is inline in the class. `reallocMem` duplicates the
  delete/assign/init tail in its `copy` and `!copy` branches; ctors assign `e/len/alloclen`
  in the body (vtable store comes first); `operator const T *() const` is `len ? e : 0`.
- `T zero = T(); for (...) e[i] = zero;` in `initElements`; VC6 turns the loop into
  `rep stos`/`rep movs` where it can.
- `#include <new>` (also via `<list>`) declares `operator delete` `throw()`: destructors that only
  `delete` then need no EH frame. A unit without it (Text.cpp) gets the EH frame.
- Inline run-time type test: `StdBaseCast(p, (T *)0)` in StdBase.h (null check, then the
  TypeInfo chain compared against `T::GetStaticTypeInfo()`), e.g. `CanAddChild`, `ScheduleRefresh`.
  `StdBase::IsOfType` itself is an ordinary call.
- Each class has its own `m_TypeHandle`, `s_TypeInfo`, inline `GetStaticTypeInfo()`.
- Registers save only on the paths that need them: write single-exit `if (...) { ... } return`
  instead of early returns when the original pushes a register late (TextPrimitive::remove).
- Temporaries passed to non-const references: `Refresh(BasicRefreshTraverser())` (pushes the ctor
  result) differs from a named local.
- `Graph::Stream` overloaded virtuals: MSVC lays overloads out in reverse declaration order.
- Inline-budget effects: using an inline accessor (`CountParents()`) instead of `list.size()`
  changes which later calls VC6 still inlines (~BasicNode).

## Graphics library (Gfx)

- `SimpleNode.obj`/`ScriptHelpNode.obj` are the same source as `src/MBEngine/SimpleNode.cpp`
  and `ScriptHelpNode.cpp` (compiled here with /GX); not duplicated, left for the coordinator.
- `/Oa` question (open, for the coordinator): the out-of-line `Vec<T>` instances of Gfx objects
  (`Vec<float>::reallocMem`/`initElements`/`append`, `Vec<LODNode*>::reallocMem` in LODManager.obj,
  `Vec<int>::setLength/append` in Node.obj) match only with `/O2 /GX /MT /Oa`, or with
  `#pragma optimize("a", on)` anywhere *after* the code that instantiates them (template bodies
  are compiled at the end of the unit with the pragma state there). Ordinary functions of the same
  objects match *without* `/Oa` (Gfx::Attribute::WriteInternals and Gfx::Node::UseBoundingBox
  reload members after calls; BVolHierNode() inlined into Node() reloads `len`), and Graph/EAIStd
  objects break with it. One exception: Node::WriteHeaderInfo's inlined Vec::append caches `len`
  across `operator new`. So it is not a whole-library option.
- `MathConstants.h` (PI and five derived `static const double`s) gives every Gfx unit its five
  `$E` initializer pairs (jmp stub + body); include it right after the unit's own header.
- Units ending with `$E` for `ctype<unsigned short>::id` + `collate<char>::id` (47+12+12 bytes)
  include `<locale>` after their other headers. Gfx objects have no PDB `$E` names, so any number
  works, but header changes renumber them: rerun the `$E` claims after touching shared headers.
  Game objects (PDB-named `$E342` etc.) need the exact numbering, not done.
- `clamp(v, lo, hi)` (Vec.h) is a ternary chain; the if-chain form gives the same clamp but other
  EH funclet stack slots in Path() and Node(const Node &).
- `StdBaseCast`: `if (object) { ...loop...; return (T *)object; } return 0;` (null case last);
  matches Gfx::Node::CanAddChild and the Graph users.
- `Graph::NodeContext<T,V>`: no vtable, a `std::vector<V>` at offset 0 (`at()` gives `_Xran`).
- `DataBase::GetInstance()` (with `new`) is inlined only into functions that already have an EH
  frame (Node::ReadInternals), called elsewhere (ReadVis/Read).
- BoundingBox(): init-list `m_Minima(GetMaximum(), ...)` matches the inlined copies; the
  out-of-line ??0BoundingBox does not match yet.
- Base-class calls from a `Gfx::` member: `ActiveNode::ChildAdded(...)`, not `Graph::ActiveNode::`
  (VC6 C2352).
- `WriteTraverser(const char *)`: `try { m_pStream = new ...; } catch (...) { throw; }` (ebp frame,
  `_CxxThrowException(0, 0)`).
- Some units inline `operator delete` as `if (p) MBfree_release(p)` (MBNew.h); their `??_G`
  deleting destructors call `[import MBfree_release]`.

## Vector math (EAIMath / EAIStd, Gfx bounding volumes)

- Headers: `FixedVec.h`, `Vec3.h`, `Vec4.h`, `TMatrix4.h` (SquareMatrix + TMatrix4), `Gfx_MatrixStack.h`,
  `AVec.h` (AVec/RVec), `Gfx_Ray3f.h` (Ray3f, LineSegment3f), `Gfx_Plane3f.h`, all from the PDB.
- `Vec3::operator-(v)` = copy `*this`, then `-=` (written per component); `operator+(v)` and
  `operator*(T)` construct the result directly (`Vec3<T>(e[0] + v.e[0], ...)`).
- `FixedVec<float, 3>::multScal` is an explicit specialization without the loop (the generic
  loop is not unrolled by VC6; the binary's 3-component multScal is, the 4-component one is not).
  `normalize()` = `multScal(1 / sqrt(mag2()))`; `mag()` = `sqrt(mag2())` (no abs).
- Statics that VC6 would constant-fold (`Version`) are defined at the end of the unit when the
  original loads them from memory (LODNode::WriteInternals).
- `MatrixStack()` (identity on top) inlines into one `rep stosd` of the whole array when
  `SquareMatrix()` zeroes with memset.
- A static member copied from a `const` member of another object, returned by value
  (`return sphere += point`), copies from the reference `+=` returns.
- `$E` claims after header changes: the object's `$E` symbols in section order map one to one
  onto the file's `$E` claims in file order (renumber mechanically).
- Unsolved (left as TODO, same reason each time):
  - inlined `multVec3PointLeft(Affine)` (and the out-of-line copies): the original sums each
    row as `m[3][j] + m[2][j]*z + m[1][j]*y + x*m[0][j]` with `fadd st(1)`/`fstp st(0)`; every
    source order tried makes VC6 add `m[3][j]` last as a memory operand.
  - BoundingVolume-derived constructors that copy a centre: the original computes `&m_Center`
    into a register and stores the vftable before the copy; ours stores word 0 at `[this+4]`.
  - Inlined `Vec` growth code (verify/reallocMem in LODNode::SetRangeLimit, the RVec copy in
    SetRangeLimits) keeps `len`/`e` in registers as with /Oa; `#pragma optimize("a", on)` at the
    top of LODNode.cpp gets closer but breaks its constructor/destructor (not applied).
- EAIRefFrame's destructor is inline (EAICamera's destructor does not call it); its out-of-line
  copy in refFrame.obj comes from a function not written yet.
- CurrentTransformationMatrix (no PDB): ProjectionMatrix base, ViewMatrix at 0x40,
  `std::stack<TMatrix4<float>, std::vector<...> >` at 0x80, a TMatrix4 at 0x90, three
  changed flags at 0xd0; `s_pInstance` is protected (MatrixTransform is a friend).

## No-alias code (/Oa) per unit

- Five scene-library/EAI units match only with `#pragma optimize("a", on)` at the very top (CSBWorld,
  the sixth, now gets it from MBContainers.h, see the game-class section) (the flag must be on both
  where the header's inline functions are parsed and in the calling function; a pragma only around
  the include, or only around a function, changes nothing). Reviewed in wave 5: without the pragma
  exactly these claims break, all with the same signature (a load scheduled above a store to
  another object, or the last two words of a copy loaded together):
  - CSBWorld: GetBounds, SetBounds (inlined MBRect copy/assign: `right` and `bottom` loaded before
    `right` is stored). MBEngine's CMBWorld::GetBounds, same source, has the plain order. Solved:
    the game code is /Oa code, the library headers in front of it are not (pragma in MBContainers.h).
  - EAICamera: the copy constructor and (frame, fov, near, far) (the bool loaded before m_FOV is
    stored). A compiler-generated copy constructor gives the same code as the written one.
  - EAIRefFrame: operator= (upLock copied in the middle of the Up copy).
  - Gfx_BoundingVolume: BoundingSphere(const BoundingVolume &), FindClosestPoint(plane), operator+,
    operator+=, BoundingCylinder(center, r, h), BoundingCylinder(const &), BoundingBox::Reset.
  - Gfx_PickTraverser: ~PickTraverser, ClearHits (`_Last` kept in a register across the inlined
    `vector::erase` copy: STL code, nothing to change in the source).
  - Gfx_RenderTraverser: ~RenderTraverser (the stack's vector pointer loaded before the vftable store).
- Tried without success (each restores the order of one copy but not the whole function): accessor
  references (`frame.getUpVec()`), the source order upLock-before-Up, caching the source member in a
  local first, a local copy of the vector, `memcpy` in MBRect's copy/assign, the 4-argument MBRect
  constructor, `SetRect(l, t, r, b)` inline, compiler-generated copy constructors. In
  BoundingBox::Reset two named locals (`Vec3<float> minima(...), maxima(...)`, then the two
  assignments) give the exact instruction order but separate stack slots (the original reuses one).
- `#pragma optimize("w", on)` (aliasing across calls only) instead of "a": the same in CSBWorld,
  EAICamera, EAIRefFrame, RenderTraverser; worse in PickTraverser (ClearHits) and BoundingVolume.
  The whole directory with "w" breaks Graph_*, Text, Gfx_Node, Gfx_LODNode (53 claims), so it is
  not a project option either.
- Functions in those units that need it off again (`#pragma optimize("a", off)` before, `("a", on)`
  after, each with its evidence comment): BoundingSphere::Contains(sphere)/Intersects(sphere)/
  FindClosestPoint(point) keep the vftable load after stores through a reference.
- Wave 6: the PDB's module list (DBI, second name = the library) shows the scene code comes from
  static libraries, each a project of its own, all compiled without debug info:
  `SDK\Scene\EAIMath\Release\EAIMath.lib` (EAICamera, refFrame), `Graphics\Release\Graphics.lib`
  (43 objects: Gfx_*), `Graphics\DXGraphics\Release\DXGraphics.lib` (12: DXGfx_*),
  `DirectX\Release\DirectX.lib` (9: DX*), `Graph\Release\Graph.lib` (13: Graph_*),
  `EAIStd\Release\EAIStd.lib` (text), `Utilities\Release\Utilities.lib` (StdBase),
  `eagle2\...\EG2Resource.lib` (EG2Asset, EG2Bitmap), `\tugboat\code\sbsquish\Release\SBSquish.lib`
  (ScriptHelpNode, SimpleNode). The game objects (CSB*, SB*, Steamboat*) are the project itself.
  Experiment (pragma on top of every unit of a library, whole directory checked): EAIMath.lib needs
  /Oa for all of its code (both units carry the pragma, nothing switches it off) - a candidate
  for an options line `SteamboatApp.dll EAIMath/ /O2 /GX /MT /Oa` if its two units move into
  `SteamboatApp/EAIMath/` (options.txt is the coordinator's). DXGraphics.lib: no claim changes
  either way. Graph.lib (26 breaks in Graph_BasicNode alone) and EAIStd (10) are not /Oa.
  Graphics.lib is mixed: /Oa everywhere breaks only 7 claims (Gfx_Attribute WriteInternals,
  LODNode ctor/dtor, Node ctor/copy ctor, ~BVolHierarchy<BoundingBox>, Vec<int>(int, float)),
  e.g. ~LODNode stores its vftable before loading m_RangeLimits.e (no /Oa) while ~RenderTraverser
  loads before the store (/Oa); so per-unit pragmas stay for Graphics.lib.
- Units where it makes things worse (stay without): Gfx_Node, Gfx_LODNode, Graph_*, Text.
  (The game-class constructors listed here before failed because of the generated headers.)

## Game classes: shared headers from the PDB (wave 5)

- Shared headers, one per original header where the PDB names it: `MBPoint.h`, `MBRect.h`, `MBColor.h`,
  `MBContainers.h` (MBAllocator, MBVector, MBList, MBMap, MBMultimap, MBString, MBString16),
  `MBNew.h`, `MBGUID.h`, `MBComPtr.h`, `MBInterfaces.h` (the GUID statics in the order the objects' ldata shows,
  then MBEngine's IMB* interface headers; wave 2 of one declaration per class), `SBInterfaces.h` (ISB*, IMBScreenPoly), `MBRuntime.h`,
  `MBSoundUtils.h`, `SBMouseEvent.h`, then one header per class: `SimpleNode.h`,
  `ScriptHelpNode.h`, `SteamboatNode.h`, `SBActivityNode.h`, `SBDialogue.h`, `CSBWorld.h`,
  `SBObjectGroup.h`, `CSBAnimButton.h`, `CSBClickable.h`. Units converted: SBActivityNode,
  SteamboatNode, CSBWorld, SBObjectGroup, CSBAnimButton, CSBClickable. The other game units still
  use their generated monolithic headers; convert them one class family at a time (a unit must
  not include both kinds).
- Declarations come from `pdbx.py` (scratch tool of wave 5, see the report): PDB order kept,
  overloaded virtuals emitted in reverse PDB order (that fixes the StopScript/OpenPrefStream/
  SetClickMessage vftable orders), `bool` restored from the mangled names (the PDB writes
  `unsigned char`), flag members (`mIsActive`, `mDirty` ...) are `bool` (a getter returning
  `unsigned char` as `bool` adds `setne`).
- Names ending in A (`GetObjectA`, `DrawTextA`, `GetClassNameA`) are the windows.h renames.
- `SimpleNode.h`/`ScriptHelpNode.h` collide with the MBEngine headers of the same name: VC6 finds a
  quoted include in the compiled file's directory first, so `SimpleNode.cpp`/`ScriptHelpNode.cpp`
  include the MBEngine header themselves and define the game header's guard before including the
  MBEngine source.
- No-alias code: the game's code is /Oa code, the library code in front of it (STL headers and
  MBNew.h's operator new) is not. One `#pragma optimize("a", on)` in MBContainers.h after those
  includes reproduces it for every converted unit (CSBWorld's constructor, SBActivityNode's string
  constructor, the MBRect copies, the QueryInterface `iid.data1` load ahead of `*obj = 0`, the
  member copies of CSBAnimButton all match only that way). The per-unit pragmas in CSBWorld and
  CSBAnimButton are gone. Probably the PCH (Steamboat_pch.obj) was built without /Oa.
- Inlining of MBComPtr: see the wave-6 section (CreateObject calls _QueryInterface; the
  `#pragma inline_depth(3)` of CSBAnimButton.cpp is gone).
- MBComPtr: the GUID constructor calls `CreateObject`, which calls `_QueryInterface(unk)`; `operator=(int)` is `*this = (T *)0` (stores before Release).
- `MBComPtr<IMBStream> stream; mgr->GetStream(id, 'anm ', (IMBStream **)&stream);` (the original
  passes the smart pointer's address).
- `PlaceCentered`: `Place(point - MBPoint3(bounds.Width() / 2, bounds.Height() / 2, 0))`.
- Switches: cases with identical bodies written separately (LeaveRollover) give the `sub/dec`
  chain; `case kPushButtonBehavior: case kClickableBehavior:` first gives the jump-table sharing.

## Game classes (wave 6)

- SBObjectGroup's code is in CSBWorld.obj (PDB module), after CSBWorld's own functions; kept in
  SBObjectGroup.cpp (the multimap template instances are claimed there). `typedef MBMultimap<long,
  IMBObject *, std::less<long> > ObjectMap` (private) for the iterator types.
- Loop forms: `ObjectMap::iterator it; while ((it = mObjects.begin()) != mObjects.end())
  RemoveObject((*it).second);` (~SBObjectGroup); `obj = (*i++).second` keeps the node pointer across
  `_Inc` (Draw); counters: `unsigned long count = size(); if (mBackground) count++;` gives the
  `je/inc` form (GetBounds), `unsigned long n = index; if (mBackground) n = index - 1;` (GetObject).
- Declaration order of locals decides register allocation (SBObjectGroup::Draw: iterator, object,
  err), and `int cursorInt = 0;` before a string local stores it first (SetCursorItem).
- MBRect::Grow: `left = MBmin(left, r.left)`, `top = MBmin(...)`, `right = MBmax(Right(), r.Right())`,
  `bottom = MBmax(Bottom(), r.Bottom())` (the original copies right/bottom into temporaries).
- Restoring rectangles with `SetRect(const MBRect &)` (no self-assignment test), not operator=.
- MBComPtr::CreateObject calls `_QueryInterface(unk)` (evidence: the out-of-line CreateObject copies
  in SteamboatNode.obj list only `unk` as a local, the inlined `pInterface` is _QueryInterface's).
  With it CSBAnimButton needs no `#pragma inline_depth` and CMBScreenPoly/CSBClickable
  Enter/LeaveRollover/StopTrackingClick match.
- Open (MBComPtr inlining, see the Release entry below for what the do/while form fixed):
  PlaySfx and GetStream (SteamboatNode) still call `_QueryInterface` out of line from the GUID
  constructor where VC6 inlines it; FindFreeChannel calls both Release and _QueryInterface out of
  line from an inlined CreateObject; CreateAndLoadAnim inlines CreateObject's first Release.
- Two values computed before a call: `IMBObject *object = iObject;` before
  `GetActiveWorld()->GetRoot().AddObject(object)` (the original loads the argument before calling
  GetActiveWorld; probably an inline helper in the original).
- New small headers: `SBItem.h` (accessors inline: SteamboatNode.obj has an out-of-line
  GetClassName), `SBCursorItems.h`.
- `SBObjectGroup::ClipRectsToObject` and `CSBWorld::Draw` (and MBEngine's CMBObjectGroup/CMBWorld
  twins) are open: same operations, the original keeps the blit rectangle in registers.

- Gridlock family converted (wave 6): `CGameBoardPos.h`, `CVectorPath.h`, `CMBButtonGroup.h`,
  `CSBGridlockGameTile.h`, `CSBGridlockActor.h`, `CSBGridLockGameBoard.h`, `SBGridlockNode.h`,
  `CActivityLeveler.h`, plus `d3dxmath.h` (D3DXVECTOR3/D3DXPLANE with the SDK's inline bodies and the
  d3dumath library functions). Units that only include their own small header get no-alias code
  by including `MBContainers.h` first (the precompiled header of the game, see CGameBoardPos.cpp).
- Functions without a return statement: the original `int operator++() { NextPos(); }` (a tail
  jump, no bool-to-int conversion) and `int CSBGridlockGameTile::operator=` end without `return`;
  VC6 reports C4716 as an error, so they carry `#pragma warning(disable : 4716)` (push/pop).
- A computed `bool` kept in a variable gives the shared-return form (`bool bMoved = true; ...
  return bMoved;`); `direction eDirection = GBInvalid; if (...) eDirection = ...; return eDirection;`
  keeps the default in a register (GetDirectionTo).
- Evaluation order of inline getters: `CSBGridlockGameTile::GetOrigin` takes `top` and `left` into
  locals before calling `Lev()` (`long y = top; long x = left; return MBPoint3(x, y, Lev() + 150);`);
  GetCenter assigns the members of a default MBPoint3 one by one (x, y, then z). The out-of-line
  `??0MBPoint3@@QAE@JJJ@Z` of SBGridlockActor.obj comes from GetOrigin.
- `for (init; flag && cond; step)` puts `flag` at the bottom and jumps back to the `cond` test at
  the top (BuildPath); `cond && flag` duplicates `cond` at the bottom.
- Switch case order: the EH state numbers of the temporaries follow source order, so they give
  the order of `case` labels when the compare chain does not (CSBGridlockActor::MoveTo).
- `std::find(list.begin(), list.end(), x)` compared as `list.end() == std::find(...)` gives the
  original operand order; `MBMap::find` results likewise `map.end() != it`.
- Template instances with names the PDB shortened (prefix + hash) are claimed with the object's
  full symbol; scratch helper `freebies2.sh` finds them (prefix match, overloads tried in turn).
- Inline budget: several functions differ only in what VC6 still inlines at the end of a big
  function (CSBGridlockActor::MoveTo inlines the last push_back but not the ActionStep
  constructor; HandleClick inlines map::find; GetTokenList inlines list::push_back). Our VC6
  makes the other choice; left as TODO.

- A global `const char *kSBUniversal = "universal";` (non-const pointer) mangles as
  `?kSBUniversal@@3PBDB` and is loaded from memory; `const char *const` gives `3QBDB`.
- CSBGameState (CSBGameState.h from the PDB; SBRecycleItem.h / SBInventoryItem.h are two-line
  SBItem subclasses): `Release` as `r = --_mRefCnt; if (!_mRefCnt) delete this; return r;`;
  QueryInterface tests IMBUnknown, IMBEventListener, ISBGameState in that order into one `p`;
  a count parameter that the original loads before a preceding loop is a local copy made at the
  top (`int nToPlace = nPossibleInstances;`, RandomizeRecycleItems); `MakeCSBGameState` is the
  engine singleton form (`static IMBUnknown *instance = MBComUtilAddSingleton(new ...)`).
- Open in CSBGameState: VisitScene (inlined `+=` adds `_Len + n` in the other order),
  EnableItemHotspots (the original calls `compare(pos, n, s, len)` out of line and inlines the
  multimap iterator `++`), ReadFromFile (MBComPtr inlining, below).
- Motivate declarations of the game's own (from SteamboatApp.pdb, not MfPoLibrary's headers):
  `MfString.h` (calls through the import thunks, no dllimport), `MfTypedValue.h` (MfStreamObject,
  MfStreamSizedObject, MfTypedValue), `MfIntValue.h`, `MfStringValue.h`, `MfProperty.h`, `MfAi.h`
  (MfAi, MfAiActor, MfAiSpace, all pure virtual) and `SBMotivateDriver.h` (GetMotivateSpace and
  GetMotivateWorld inline). The value classes' inline members are emitted by CSBGameState.obj and
  claimed there. Forms: `MfTypedValue()` assigns its two members in the body (vtable store
  first), `MfIntValue(int val) : value(val)`; a value handed to an MfProperty is a local copied
  with `Copy()` (`MfIntValue intValue(value); MfProperty prop(name, intValue.Copy());`, the name
  converted implicitly); an MfString key passed by reference is a named local in its own block
  (`{ MfString propName(name); found = actor->GetProperty(propName, prop); }`), not a temporary.
- MBComPtr GUID constructors: in many functions the original calls the first CreateObject out of
  line and inlines a later one (with Release and _QueryInterface out of line); VC6 makes the same
  choice in big functions (LoadLayout, LoadAnims, the board constructor match) but inlines
  everything in some smaller ones (ReadFromFile, InitGameGrid, SBMainMenuNode::OnStart: TODO).
- Gridlock board: `char buf[N] = { 0 }` (byte store + rep stosd + tail) gives the buffer size
  exactly (LoadLayout 30, LoadAnims 16); `std::string` operator[] writes (`strKey[3] = '0' + r`)
  inline `_Freeze`; a key passed to `AddItemToInventory(MBString &, ...)` as a temporary
  (`MBString("PRcabinet0")`) shares its slot with other temporaries, a named local does not;
  the constructor's 29 `m_mapIdToSound[key] = "..."` lines match as written (VC6 inlines the
  first 21 operator[] calls and calls the last 8, like the original).
- ConfirmationBox.h and SBSongbirdNode.h (new unit) are from the PDB. VC6 calls virtual members
  of a member object (`mEraseSongDialog.Destroy()`) through the vtable, as the original does.
  A loop whose iterator is incremented before a block-scoped MBComPtr dies is a `while` with
  `note++` as the last statement (ClearSongboard); a `switch` on a char with two cases lists them
  in EH-state order ('x' before 'C') while the compare chain tests them by value (DrawSong).
- `Create` of CActivityLeveler and ConfirmationBox match: each step is
  `THROW_ON_ERROR(expr)` = `if ((expr) != 0) throw exception(__LINE__, expr)` (the `!= 0` makes
  the saved condition a bool, as in the original), `SAFE_RELEASE(p)` = `if (p) { p->Release();
  p = 0; }`, a `try` around the steps and `catch (exception e)` that releases everything and
  returns `e.nError`. The statements sit on the original lines (CActivityLeveler 66 .. 97,
  ConfirmationBox 70 .. 106); gaps are filled with comments, never `#line`. ConfirmationBox's mouse
  set-up is before the `try`, the background is placed only `if (mBackground)`.
- Function order inside a unit changes code: ConfirmationBox's SetBackgroundName and
  SetButtonNames match only when they come before Create (their inlined string assignment).
  Keep units in address order unless a `__LINE__` layout forces otherwise.
- MBComPtr inlining, solved for most cases: VC6 decides per function from the callees' node
  counts, so a code-neutral rewrite of MBComPtr::Release changes which members get inlined.
  `void Release() { do { if (mInterface != 0) mInterface->Release(); } while (0); mInterface = 0; }`
  (a release macro in the original, it seems) makes the callers inline CreateObject, fold the
  Release away and call _QueryInterface out of line like the originals; 12 former TODOs match
  with it (Actor's constructor and ListenToEvent, SBTimeBomb::Defuse, CSBGridlockGameTile::Update,
  SBActivityNode and SBSongbirdNode Start/StopListening, CActivityLeveler's destructor,
  CSBGameState::LoadRecycleItemsFromPref, CSBTrashTile, SteamboatNode::KillSound) and no claim
  breaks. Tested and rejected: MBEngine's GUID constructor and CreateObject shapes, member order,
  the RTM front end, `this->`, `(void)` casts, `!= 0`, temporaries, extra braces/else, a
  `((void)0)` statement; `if/else` forms with a second store change the code. Some functions still
  differ (SteamboatNode SetCursorItem, CreateAndLoadAnim, GetStream, PlaySfx, FindFreeChannel,
  SBSongbirdNode::OnActivityEvent, Actor::PlayDialog, CSBGameState::ReadFromFile); try them
  again in new forms with this header.
- RenderManager.h, BitmapBuffer.h, SBActorManager.h are from the PDB; RenderManager.h includes
  MBContainers.h (no-alias game code, needed for SetRenderTraverser and ViewExists). EndRenderView
  catches `DX::Exception &` and `Gfx::TextureException &` (Gfx_TextureException.h); it is TODO
  only because of the checker (the catch blocks). Gfx::Display got the PDB's inline
  GetInstance(); enums.h has BufferEnum's real values (cBackground, cZbuffer).
- Actor.h (Actor : Gfx::Node, IMBEventListener, 0x1f4) and SBLipScript.h are from the PDB; the
  accessors the PDB lists without out-of-line copies are inline (SetCurrentBuffer, SetScreenRect,
  Get/SetActiveHead, Get/SetMotivateActor; SteamboatNode::GetSfx/Dialog/MusicVolume). Actor's
  default constructor has an empty body; the (actor, parent, file) constructor's loop sets
  `mScreenRect[1].right` for both rectangles (sic, matches as written). A float multiply by 0.03
  is named after the double value in the original (`__real@4@3ff9f5c28f5c28f5c000`); every form
  tried gives the float name (UpdateHeadData TODO). Gfx::Shape got its PDB screen-rect members
  (mCalculateScreenRect, mScreenRect, mScreenRectValid + inline accessors), made before the
  graphics files moved to another worker. enums.h now only holds BufferEnum and
  ViewFrustumIntersect (its placeholder MB* enums clashed with MBInterfaces.h).
- CSBInventory.h and SBTimeBomb.h are from the PDB. Access matters in the mangled names:
  CSBInventory's IMBObject overrides (Show(), Hide(), IsVisible, ...) are private (`E`), Show(bool)
  is public. Actor::ListenToEvent reads `*(IMBSoundDescription **)data` (channel -4 carries the
  sound description; event 0 start, 1 end). SteamboatNode.cpp, CSBInventory.cpp and SBTimeBomb.cpp
  are now in address order (sorting changed no matched function).
- `$E` initializers of SteamboatNode.obj (`$E82` RadiansToDegrees, `$E296` kCursorItemString[31],
  sDirtyRectVector, sFlyDir and the other static MBStrings): the PDB numbers count every
  initializer of the original header set (81 before the first one), so they cannot be claimed
  until the headers match it.
- MBComPtr has a template constructor from a raw pointer of another interface,
  `template <class U> MBComPtr(U *const &p) : mInterface(0) { _QueryInterface(p); }` (the PDB lists
  `MBComPtr<IMBMouseable>(IMBObject *const &)`); `MBComPtr<IMBMouseable> m(mGameButtons[i])` and
  `MBComPtr<IMBObject>(mEasyButton)->Move(...)` use it. No existing claim changed.
- SBMainMenuNode.h, SavedGameDB.h, SBTrashBashNode.h, SBTrashSquare.h are from the PDB (new units).
  SBMainMenuNode::CreateButton/CreateHotRect release the new object twice (once through the
  interface, once through the IMBUnknown) and return an unset pointer when the create fails.
  `space->GetActor("GDmickey0")` / `actor->SendMessageA(0, "UIgoBack")` (implicit MfString
  temporaries, not `MfString(...)`). Open: OnStart (calls broadcaster's CreateObject out of line,
  inlines gameState's) and CreateDifficultyButtons (inlines the first CreateAnim temporary's
  Release, calls the second); OnLMouseDown/Up (TODO: the original calls the MfVector3 constructor
  of data.dir out of line and inlines pickPoint's and data.pos's; VC6 inlines all three). Budget
  probe: a constructor body that calls an inline setValue moves the cut-off to the nested setValue
  of data.pos (nested expansions go first); an empty MfHookData() constructor changes nothing. So
  the original spends more expansions before data.dir than our headers do; which ones is open.
- MfVector3.h (inline constructors, destructor and copy: the game's units inline them or emit them
  as instances), MfHookData.h (MfHookData, MfMouseHookData, MfPickData) and ViewEventHandler.h are
  real now; MfVector3.cpp (three instance claims) is gone until a unit emits them again.
- Linking logs family: CKeyFrameInterp.h, CLinkingLogsTile.h, CSBLinkingLogsActor.h,
  CSBLinkingLogsGameBoard.h, SBLinkingLogsNode.h and the six CSBLL*Tile.h are from the PDB. The
  tiles reach the base's private members only through inline accessors (SetActor, LockState,
  IsWalkable, GetCenter with an if/else on m_pAnim, GetCurrentSequence as
  `!m_pAnim ? 0 : m_pAnim->GetCurrentSequenceIndex()`); D3DXVECTOR3 has the d3dx constructors and
  operator-. Sequence tests are switches where the original keeps the null path (OnAnimEvent of
  the one-way door and the steam tile) and ifs where it folds it (trapdoor).
- CLinkingLogsTile.cpp (LinkingLogsTile.obj): tileinfo's default and copy constructors and its
  operator= are inline in the header (the constructor calls the operator= instance, operator= of
  the tile inlines it); the static m_mapSequenceRoot/m_bSequenceMapInit are defined there. The
  steam tile reads m_objProperties directly, so it is protected here (private in the PDB).
  Door events are `MAKELONG(kSwitch, 1/2)` broadcast on channel 204 and decoded as
  `(event >> 16) & 0xffff`; OnActorEnter tests the tile type with if/else, not a switch. Inline in the original: CLinkingLogsTile::GetBoardPosition, GetRect,
  IsFinish (as `if (...) return true; return false;`), CKeyFrameInterp::GetStart/EndPoint and
  ActionStep's constructor, operator=, == and !=. Forms: map compares `m_map.end() != it`;
  PlaceCenterd is `MBPoint3 ptTopLeft = ptPosition - MBPoint3(w / 2, h / 2, 0)`; BuildPath copies
  posLast from posCurrent; OnIdle is `(m_pActor && idle + last < now) || start + total < now`.
- SBSandwichNode.h is from the PDB; its globals kLaunchPoint, kMickeyPlateOffsets and
  kStartingPlatePos are dynamically initialized MBPoint3/MBPoint objects.
- RenderManager::GetBitmapBuffer checks `find(viewID) == end()` and returns 0 before `operator[]`
  (SteamboatNode::AddDirtyRect).
- CSavedGameDB: `sgdbinfo(const sgdbinfo &refToCopy) : strSceneCode(strSceneCode), nIcon(nIcon) {}`
  (initialized from itself: no EH frame, the self-assign path of assign); the default constructor
  is inline `nIcon(-1)`. IsGameUsed is `it = find(n); return m_mapSaveData.end() != it;` (operand
  order). Load and Save are `try` blocks with `#define THROW_IF(cond) if (cond) throw
  (long)__LINE__` on the original lines (Load 171 .. 184, Save 220 .. 241, blank lines between) and
  `catch (long nLine)` printing `"%s(%d) : ERROR! ..."` with the original path
  `D:\\tugboat\\code\\steamboat\\SavedGameDB.cpp`; `strdup` and the first `strtok` are separate
  statements. The sgdbinfo copy-constructor comment sits in the header so Load's lines fit. Save
  is TODO: the original keeps the map iterator above the two interface pointers on the stack.
- CSBInventory throws `(long)__LINE__` on failing steps (`THROW_ON_ERROR`), so CreateInventoryAnim,
  DestroyInventoryAnim, CreateAndShowInventoryItem and HideAndDestroyInventoryItem sit on the
  original lines 331, 352, 434 and 463 (blank lines in between). HideAndDestroyInventoryItem
  indexes `m_vecItemsInInventory[nSlot].pAnim` each time (a reference variable caches the value).
- SteamboatNode cursor: UpdateCursorRegpoints switches directly on `alignment[sequence][0]` and
  `[1]` (local copies move the table pointer); case 1 subtracts the full width/height, case 2 adds
  `-((long)size / 2)`. ResetCursor builds the property values as
  `new MfStringValue(cursorString.c_str())` (implicit conversion; an explicit `MfString(...)`
  temporary reuses eax for the constructor argument) in a separate statement before SetValue.
- Actor::InitMaps: `MBString nodeName = (const char *)mfNode->GetNodeName();`, cluster names via
  Text's non-const `operator char *` (copyOnWrite), `nodeName == clusterName`, map inserts with
  `value_type(mfNode, ...)` and `new Vec3<float>(cluster->m_CPos[0])`. MfAiActorNode.h (game-side
  copy of the PDB declaration with its two enums) serves it. MfRotation3.h and MfTransform3.h are
  real declarations now (inline constructors, destructor, accessors as in mflinalg; MfVector3 has
  x()/y()/z()); their instances are claimed in Actor.cpp (CopyMotivateData emits them), the old
  MfRotation3.cpp/MfTransform3.cpp instance units are gone. CopyMotivateData (TODO, 26%) uses the
  PDB local names; the original calls Quaternion::toMat3 and Vec4's constructor out of line.
- Actor::LoadSceneData is `_SS<char> path = pathName.Left(pathName.ReverseFind('\\') + 1)` and
  `((DXGfx::DisplayD3D *)Gfx::Display::GetInstance())->GetDirect3D()->GetTextureMapManager()->AddPath(...)`;
  ReverseFind is CStdString's inline (StdString.h). TODO only because of the npos fold (see "Open
  (npos)").
- Linking logs: CLinkingLogsTile::IsStart is inline (`if ((m_nProperties & kStart) == kStart)
  return true; return false;`), tileattributes has an inline constructor
  `nTypeID(0), nProperties(0), bWalkable(false), nTimeout(-1), fVelocity(0.0f)`;
  LL_DIFF_EASY = 0.25f and LL_DIFF_HARD = 0.0f are file constants of the game board.
- Graph::SearchTraverser results are walked as `search.GetPaths()[i].GetTop()` (the implicit
  `operator const T *` of Vec, `len ? e : 0` per iteration); `GetPaths()(i)` indexes e directly.
  FindReplacementHeads keeps the last LODNode found (no break) and builds an unused MBString of
  each head cluster's name.
- SteamboatNode.cpp keeps its __LINE__ statements on the original lines: OnPreRender's two
  MBDebugStr calls on 1373 and 1384, CreatePrefAndStream's throws on 3390 .. 3402 (OnItemClick, not
  written, has MBDebugStr on 3118 and 3133). Blank padding sits after the comment lines that start
  "// statements sit on the original lines 1373" and "// sit on the original lines 3390"; re-pad
  after every edit above them. CreatePrefAndStream: `throw (long)__LINE__`, catch prints the
  original path; `IMBFileSpec *iFileSpec = 0;` is declared before iUnknown.
- SteamboatNode: OnEnd stores `bool noSceneMusic = sBackgroundMusicName == MBString("");` before
  testing the stream (the temporary dies first). Set*BackgroundMusic compare `name !=
  sBackgroundMusicName`, open the stream with `MBComPtr<IMBFile> file(stream)` and `if (file) {
  GetFileSpec; file.Release(); }`, copy the path with strcpy into `char path[256]`; the scene
  variant clears the name with `MBString("")`. LoadMiscellaneous: `stream.Attach(OpenPrefStream())`
  and `pref->GetString(stream, MBString("miscellaneous"), MBString("ambient"), ambientName,
  MBString(""))`. SetSpaceName: `MBstrrchr(spacename, '\\')`, falling back to `spacename - 1`.
  OnPostRender calls BitmapBuffer::SwapScreenRects, which is inline (clear, insert, clear).
  OnPreRender/OnItemClick call SBMotivateDriver::GetInstance() around Update with unused results
  (the original's AcquireLock/ReleaseLock compile to nothing); written as plain GetInstance() calls.
  CreateAndLoadAnim was already a TODO; a duplicate draft was removed.

## Game classes (generated headers, not converted yet)

- Adjustor thunks (`?F@C@@W3AE...`, 8 bytes) come with the vftables, which the unit emits once
  its constructor is compiled; a constructor still marked TODO is enough to claim them.
- Generated headers declare small inline members (MBPoint3, MBRect constructors and copies)
  without bodies; give them the bodies from the MBEngine headers (MBRect::operator= checks for
  self-assignment). Units that instantiate MBAllocator strings need the MBmalloc_release /
  MBfree_release declarations before the allocator template. `MBNew.h` has the engine's
  operator new/delete (used where `??_G` calls `[import MBfree_release]`).
- Constructor store order (CMBScreenPoly, CSBSongbirdNote still open): CSBAnimButton's and
  CSBClickable's matched once the unit was /Oa code with the PDB header (see above).

## Graphics library (wave 7)

- `obj\i386\d3dumath.obj` (the DirectX SDK's D3DX math, 98 functions) was not built by VC5/VC6:
  loops count a negative index up to zero, `ffree st(n)` after FPU-cached loops, `leave` epilogues,
  ebp frames in some leaf functions and none in others, interleaved 4-accumulator x87 code. No VC
  option set reproduces it (tried /O2, /Ox, /Oy-, /G5, /G6, vc5, vc6-rtm); probably the Intel
  compiler. Its 6 trivial members (empty constructors) match from the D3DX*.cpp stubs; the rest is
  readable source with TODO claims in d3dumath.cpp (types and inline helpers in d3dxmath.h).
- Units with no-alias code: some need `#pragma optimize("a", on)` at the very top (MatrixTransform,
  MaterialAttribute, DisplayQuality, VisibilityList), others right AFTER the includes (LODManager,
  TextureAttribute: header inlines without /Oa, the unit's own code with it; at the top their
  constructors and ReadInternals break). Try both when a Graphics.lib unit shows loads hoisted
  above stores.
- `Vec::setLength` is `else if (n > len)` (compare order seen in LODManager::CreateGroup; the whole
  directory still matches). `Vec::append(const T *, int)` and `insert(const T *, int, int)` are
  defined out of the class (vector.cxx); the original calls them out of line from every unit, VC6
  here inlines them (in-class, out-of-class and `auto_inline(off)` tried): TODO in LODManager.
- `TextureAttribute` (from the PDB): its inline setters call `SetTexture()` (SetFilename,
  SetHasColorKey, SetColorKey, SetMipMap, SetAlpha; not SetFilter, SetBlendMode, SetWrapU/V,
  SetTextureStage) - operator= is the list of Set(Get()) calls.
- bool results: `bool equal = strcmp(...) == 0; return equal;` gives `sete al` (a direct return
  gives `sete cl; mov al, cl`); a `bool want = true;` updated in `switch` cases and returned once
  keeps the flag in bl (WantTexturing). `if (...) return true; return false;` gives `mov al, 1`.
- A float member copied or compared through an inline getter (`m_Power = m.GetPower()`,
  `GetPower() == m.GetPower()`) goes through the FPU (fld/fstp, fld/fld/fcompp); plain member
  access copies with mov and compares with `fcomp mem`.
- StdString.h (CStdString): `Format`/`FormatV` (two stack buffers of 256/512, then GetBuffer with
  +2048 per try, 4 tries) match LODManager's out-of-line `_SS<char>::Format`; `Find(PCSTR)` returns
  `find()` directly (DelVisChunk compares the out-of-line find's result with -1 immediates);
  `operator+=(const MYTYPE &)` is `append(str.c_str())`; Left/Right are declared only (their
  instances live in Actor.obj and DXDirectDrawSurface.obj).
- Open (npos): where `find` is inlined the original folds `basic_string::npos` to -1 (GetVisChunkVis,
  GetVisChunkLod, DataBase::VerifyFile/ReadNode); VC6 here loads npos from memory, also in a
  minimal test unit. Not reproduced.
- Open (inline budget): LODManager's constructors inline `_SS<char>()` but call
  `basic_string(const allocator &)`; VC6 calls `_SS<char>()` (even alone in a unit).
  DelVisChunk inlines assign/_Tidy for the second of two `ssasn` copies, VC6 for the first.
- MaterialAttribute (no PDB type): D3DMATERIAL7-like colours `m_Ambient` (0x78), `m_Diffuse` (0x88),
  `m_Specular`, `m_Emissive`, `m_Power` (MaterialAttributeD3D::SetMaterial maps them into its
  D3DMATERIAL7 at 0xcc), `m_ShadeMode` (2), `m_BackfaceCulling` (short, D3DRENDERSTATE_CULLMODE in
  the D3D shapes), `m_ZBuffer` (ZENABLE/ZWRITEENABLE in Apply), `m_Textured` (set by
  DXGfx::NodeFactory::MakeTextureMaterialAttribute); 0xc2/0xc5 unknown.
- Static stacks (`std::stack<...> s_Stack`, `s_Stack[8]`) are defined at the end of the unit: their
  `$E` initializer/atexit functions come after the methods and before the stack instances.
- `GFX_ASSERT` (Gfx_Assert.h) and the per-unit `kThisFile`/`THIS_FILE` strings: the original source
  paths are `D:\tugboat\code\SDK\Scene\Graphics\<Unit>.cpp`, `...\Graphics\DXGraphics\View.cpp`,
  `...\SDK\Scene\DirectX\DXDevice.cpp`; DX::Exception is (hr, file, line, message) and 0x108
  bytes (`m_szDetails[256]`), which sets the frame size of every function that throws one.
- Geometry.obj is one unit (Plane3f, Polygon3f, Ray3f, LineSegment3f): Gfx_Geometry.cpp.
- `TMatrix4`/`SquareMatrix`: `operator[]` clamps the row index; `operator=` is the nested
  element loop; `multRight` keeps a copy made with the copy constructor (rep movsd).
  CurrentTransformationMatrix::PushLeft/PushRight copy the factor with an out-of-line
  `setValues()` instead (open). Tested: a copy constructor written as `{ setValues(...); }`
  with an in-class memcpy setValues reproduces the out-of-line copy constructor's rep movsd,
  but VC6 then inlines setValues everywhere (PushLeft unchanged) and BoundingVolume.cpp stops
  emitting the ??0SquareMatrix(const SquareMatrix &) instance it claims, so the loop form stays.
- d3d7.h: the DirectX 7 declarations (DDSURFACEDESC2, DDSCAPS2, D3DLIGHT7, D3DMATERIAL7,
  D3DVIEWPORT7, the stage-state enums, D3DTA_*, IDirectDrawSurface7, IDirect3DDevice7) on top of
  VC6's DirectX 5 <ddraw.h>/<d3d.h>. Do not define D3D_OVERLOADS (D3DVECTOR gains constructors
  and the copies change).
- DX layer headers from the PDB: DX_Device.h (`LightEnable`/`SetLight`/`GetLight` and
  `RenderingIsEnabled`/`EnableRendering` are inline), DX_DirectDrawSurface.h (`Release`,
  `GetCOMInterface`, `Width`/`Height` inline), Gfx_Surface.h (+SurfaceDesc), DXGfx_Surface.h
  (derives from Gfx::Surface), DXGfx_DisplayD3D.h. The `*_decl.h` side headers are being folded
  into real headers (DXGfx_Surface_decl.h is gone).
- ViewD3D (no PDB type): 0x7c Viewport, 0x80 Surface, 0x84 Device, 0x88 max lights (<= 10),
  0x8c `bool *` light slots; LightD3D: 0x144 slot index, 0x148 ViewD3D, 0x14c D3DLIGHT7.
  ViewD3D::Refresh catches `Gfx::TextureException &` (not `...`) to end the scene and rethrow.
- `Gfx::View::DrawBox` is an empty inline (emitted in DXGraphics' View.obj); `Gfx::Light()` is an
  empty inline constructor (LightD3D calls `Node()` directly). `View::SetTime()` is the inline
  that moves m_CurrentTime to m_LastTime (RenderManager::BeginRenderView).
- `ValList::Remove()` (removes the head and returns its element) is what Device's destructor
  loops over to delete the texture format records.
- VC6 refuses `Gfx::View::Refresh()` inside a `try` block of a derived member ("illegal call of
  non-static member function"); write `View::Refresh()`.
- /Oa (`#pragma optimize("a", on)`) in the graphics units is per source file, not from a shared
  header. Measured over the whole directory (2026-10-15, all per-file pragmas removed first):
  no pragma 70 differ; pragma at the top of StdBase.h 88 (breaks the Graph library units and game
  units such as Actor.cpp); in Vec3.h 33, in FixedVec.h 28 (both break Actor.cpp's 11 claims:
  Actor includes Gfx_Node.h/Vec3.h and was not built with /Oa, so no header it includes can carry
  the pragma); at the top of every Gfx/DXGfx/DX/EAI/VisMisc unit 33 (Gfx_Node.cpp,
  LODManager, TextureAttribute, DX_Device, ... break). The per-file placements give 0 differ.
  The placements themselves show the original changed the setting inside units: after the
  includes (LODManager, TextureAttribute, Geometry: header inlines without /Oa, the unit's code
  with it), before a later function (RenderState before Push, VisMisc before writeVeci), and
  switched off around single functions (RenderList::AddShape, three BoundingSphere functions,
  FileNode::ReadInternals; whole-unit /Oa breaks exactly those). Rule: when a Graphics.lib
  function shows loads hoisted above stores, try the pragma at the top of the unit, then after
  the includes, then just before the function; keep the evidence comment. Units with no hoisting
  (Gfx_Node.cpp, DX_Device.cpp, LODNode, View) must stay without it.
- A vector filled from matrix entries or members: element stores (`x.e[0] = m(0, 0); ...`) give
  the original's FPU copy of the first component and mov copies of the rest; assigning a
  `Vec3(a, b, c)` temporary does not (EAIRefFrame::scrXYZ).
- VC6 evaluates the right operand of `+` first: `x * v.e[0] + y * v.e[1] + z * v.e[2]` computes
  `z * v.e[2]` first, as the original's screenTranslate/atScreenRotate do. When the original
  computes the left operand first, name it as a local (EAICamera::updateClipPlanes: `toCenter`,
  then `viewVec`, then the second `getViewVec()` inline).
- `radius / (float)sin(...)` gives the original's fdivr (sin first); `radius / sin(...)` loads
  radius first (EAICamera::viewAll).
- EAIRefFrame inlines (PDB members not in the binary): setAtPoint/setFromPoint (plain copies),
  setUpVec (validateNewUpVec), translate (At += v; From += v), getViewVec (At - From), and the
  default and (at, from, up) constructors that set `type` and `upLock` in the body after the
  vectors. Camera::SetAt/SetFrom/SetUp/SetNearClip/SetFarClip forward to them.
- EAICamera::interp calls `EAIRefFrame::slerpTo` explicitly (EAICamera::slerpTo is a different
  member); Camera's read/write leave the field of view out (version < 2 streams carry a float
  and a short that are skipped).
- RenderList (no PDB type): `std::vector<std::vector<Shape *> > m_ShapeLists` indexed by the
  shape's RenderState type handle; RenderState needs its own GetStaticTypeInfo/GetTotalTypeCount/
  GetTypeHandle inlines, otherwise StdBaseCast and the constructor pick Attribute's statics.

## Engine-side classes in the game (CMBCursor, CMBScreenPoly; wave 7)

- CMBCursor.cpp holds all of CMBCursor.obj, FlipWatcher included (FlipWatcher.cpp/.h are gone).
  The statics are defined in the order of their `$E` numbers (sDXScreen, sAnim, sScratchBuffer,
  sScreenRect, sCursorWidth/Height, sCursorRect, sScreenOrigin, flipWatcher; bss addresses run the
  other way); `sStayAlive` is initialized `true` (.data). Its `$E285`..`$E326` initializers stay
  unclaimed: our unit numbers them from `$E14` (the header-set problem above).
- MBComPtr template assignments: `sDXScreen = screen` (MBComPtr<IMBScreen>) and
  `sScratchBuffer = tempSurf` (IMBSurface *) call `_QueryInterface` directly, so MBComPtr.h has
  `template <class U> operator=(const MBComPtr<U> &)` and `operator=(U *const &)`. VC6 reports
  C2535 for the MBComPtr<U> template unless it comes before the copy assignment; a non-const
  `MBComPtr<U> &` version compiles but steals same-type assignments (broke SetBird, SetCursor,
  std::copy instances). `_QueryInterface(const MBComPtr<U> &)` has the raw-pointer form's shape
  (`long hr; if (p) {...} else { *this = 0; hr = -3; } return hr;`).
- `MBPoint() { x = y = 0; }` (stores y first: CursorThread's localPos, `$E306`).
- Accessors change register allocation: CursorThread clamps against `sScreenRect.Right()` /
  `Bottom()` (the members directly swap eax/edx); Initialize writes the rectangle members one by
  one (`SetRect(0, 0, 1, 1)` costs inline budget: the broadcaster's Release in the inlined
  FlipWatcher constructor is then called out of line). FlipWatcher's constructor assigns
  `_mRefCnt = 0` in the body (vftable store first).
- InBackground: `if (soundManager == 0) return false;` (early return, no release on that path).
- SetCursor: `const MBRect &bounds = sAnim->GetBounds();` (register locals are not in the PDB),
  `IMBSurface *tempSurf;` uninitialized.
- CMBScreenPoly: SetRect is clear + four `push_back(MBPoint(...))` (VC6 inlines the fourth, like the
  original); GetPosition is `return mParent ? ptPos + mParent->GetPosition() : ptPos;`;
  IsPointInPoly is the usual crossing test with float x/y.
- Open: DrawCursor (the original spills sAnim to two stack temporaries and shares the store of the
  inlined `_QueryInterface`), CMBScreenPoly::LoadFromStream (the original keeps `stream` in ebx and
  inlines the last string constructions; all forms tried keep `pref` in the register).
- SBTrashBashNode: the voice-over names are global `const char *` tables (kBadMatchVO, kGoofyVO[3],
  kMickeyVO[5], kMinnieVO[5]) defined after kLeftCan/kRightCan in that order; OnLMouseUp matches
  with them. InitGameboard is TODO (the original walks `kSlots` by pointer and keeps &mLeveler and
  &mTrashSquares in stack slots). Idle (the clock's CreateObject is out of line in the original),
  LoadTrash (stack slots) and SBMainMenuNode::ListenToEvent (same size, other stack slots for the
  string temporaries) are written and TODO; CActivityLeveler::GetMinLevel/GetMaxLevel are inline.
- Linking-logs conveyer: GetVelocity is a single return (`float fVelocity = 60.0f;` and nested
  switches); GetStartPoint/GetEndPoint offset by `GetRect().Height() / 2 - 1` (the inline GetRect
  call, not a cached member, changes the inline budget). `kmapGBDirection[9]` is a `static const`
  array in CLinkingLogsTile.h (every unit has its own copy, as in the original);
  `kmapDirectionVec[9]` (D3DXVECTOR3 unit vectors) is a global of the conveyer unit. GetStartVec /
  GetEndVec are TODO (the original reads the direction bits twice and builds the scaled vector in
  dead parameter slots). D3DXVECTOR3 has the SDK's `operator*(float)`.
- CKeyFrameInterp.h uses d3dxmath.h (its own D3DXVECTOR3 copy is gone; d3dxmath.h gained the SDK's
  inline D3DXVec3Lerp and the D3DXVec3Hermite import). KeyFrameInterp.cpp needs no-alias code
  (`#include "MBContainers.h"` first): SetPoints schedules the loads before the stores only then.
- CSBTrashTile: Translate divides by `(float)time` twice (a `float duration` local adds a stack
  slot); ListenToEvent declares `MBPoint3 pos = mOffset;` before the float local and compares
  `mETA > now` (operand order); the destructor removes the periodical `if (manager)` and the
  listener without a test.
- CSBNodeManager.h is real now (generated header replaced). GetActiveNode keeps the engine's
  node manager in a named `MBComPtr<IMBNodeManager>` local (a temporary is read through the
  returned address); AddNode(void *) reads `MfSpaceHookData::pSpace` (MfSpaceHookData added to
  MfHookData.h) and builds the code with `(char *)spaceName.LeftString(2)` (the original calls
  `operator char *`).
- CSBLinkingLogsTileFactory (new unit): the PDB's kConveyer bit makes a one-way door and
  kOneWayDoor a conveyer. CreateTile's code matches; its unwind table lists the conveyer's
  cleanup funclet after the one-way door's (TODO). DestroyTile is a bare `delete p; p = 0;`.
- CSBLLDoorTile::CanMoveTo: `if (IsWalkable() && GetType(*this) == kDoor) {...} else if
  (IsWalkable()) bCanMove = true;` (the second test is not folded, as in the original).
- dllmain.h is real now; OnMBEngineCreateScreen returns through `long err = 0; ... err = -2;`.
  MBScreenAttributes (width, height, depth) is MBEngine's (MBInterfaces.h includes it).
- SBCursorItems.h holds `static MBString kCursorItemString[31]` (every including unit has its own
  copy and initializer, as SteamboatNode.obj, dllmain.obj and SBCursorItems.obj do).
- SBActorManager: LoadActor(name) is declared before LoadActor(name, actor) (VC6 gives the later
  overload the earlier slot); the constructor's GUID MBComPtr calls Release out of line as in the
  original. SBItem's constructor is TODO (the original still inlines _Grow in its last string
  assignment).

## Dialogue system, Motivate driver, map/credits/soup/movie nodes (wave 7, worker B)

- PDB headers now: `SBDialoguePropertyManager.h` (GetInstance inline: its out-of-line copy is
  SteamboatNode.obj's), `SBdLineGroup.h` (AddLine, Set/GetExitEvent(Data), Set/GetPropertyVectors,
  GetLineVector, MakeActive, IsActive, GetLineGroupName inline: their copies are in
  SBdBlockRandomSequential.obj), `SBdBlock.h` (GetBlockName, ClearClick inline) and one header per
  block type, `SBMapNode.h`, `SBCreditsNode.h`, `SBSoupRecipeNode.h`, `SBSoupMachineNode.h`
  (anonymous enum member `mMachineState`), `SBMovieNode.h`. Added to shared headers: MfAiWorld and
  MfAiRuntime (MfAi.h), MfActorHookData/MfActorRWHookData/MfFileHookData (MfHookData.h),
  MfString::SplitPath, MBGetScreenManager/MBGetScreenMouseLocation (MBRuntime.h), IDirectDraw7
  (d3d7.h), AIL_quick_handles/AIL_digital_master_volume (mss.h).
- MBComPtr::operator=(T *) calls the member `AddRef()` (SBMovieNode::ListenToEvent calls the
  out-of-line MBComPtr<IMBMovie>::AddRef from the inlined assignment); with it DrawCursor matches and
  no claim in the directory changed.
- MBComPtr template assignments (CMBCursor): `template <class U> operator=(const MBComPtr<U> &)`
  (must come before the copy assignment, C2535 otherwise; a non-const `MBComPtr<U> &` steals
  same-type assignments) and `operator=(U *const &)`; `btnMouseable = (ISBAnimButton *)mButtons[i]`
  inlines the raw-pointer query, `btnMouseable = mBigRedButton` calls `_QueryInterface(MBComPtr<U>)`.
- Forms that matched: `vector.clear()` for the inlined element loop (ClearMaps, ~SBdBlockRandomSequential)
  but `erase(begin(), end())` where the original's list loop is inlined differently
  (SBSoupMachineNode::ResetButtons); `std::find(...) != end()` (ButtonPressed); `mNextNode.empty()`
  (not `size() == 0`, SBMovieNode::Done); nested `if (MBString(a) == MBString("PE")) { if (...) }`
  saves the comparison in a stack byte (temporaries die after the outer condition, SBMapNode);
  `new int[n]` in SBLipScript::LoadLipScript (not MBmalloc_release, eax vs ecx);
  Convert4BytesToInt walks a `char *` down from `bytes + 3` with a separate counter;
  `int tries = 0;` declared before `maxTries` (register choice, FirstSubsequentRandom);
  LoadFromPref defaults `GetLong(..., numLineGroups - 1)` from a local copied before the string
  temporaries (SBdBlockOrdered); `return mParent ? a + b : a;` (CMBScreenPoly::GetPosition);
  LipScript/Credits/Map/SoupRecipe constructors `SBActivityNode(engine, spaceCode)` + nodemgr block.
- Open, all inline-budget or frame-layout questions: SBdBlock ctor/dtor/LoadFromPref/ParseTrigger/
  ListenToEvent, SBdLineGroup dtor/LoadFromPref/PlayDialogue, SBDialogue::Update,
  SBDialoguePropertyManager::SetProperty (inlined iterator ++), SBdBlockFirstSubsequent::GetLineGroup,
  SBdBlockRandomSequential::GetLineGroup, SBMotivateDriver hooks (VC6 inlines `_QueryInterface` into
  the broadcaster constructor; same open problem as SteamboatNode::PlaySfx), SBMovieNode ctor
  (`currentNode` packed into the unused `space` parameter slot in the original) and OnKeyDown,
  SBMapNode::OnActivityEvent (`_Eos`), SBSoupMachineNode::ListenToEvent/OnStart.
- Not written yet: SBMapNode::OnStart, CMBScreenPoly::LoadFromStream.
- The dialogue conditional parser (SBdBlock.obj) is bison 1.25 (`-p SBd`: SBdparse, SBdlex,
  SBderror, SBdlval, SBdchar, SBdnerrs; yywrap and the flex names unprefixed) and flex 2.5 with the
  skeletons of MBEngine/scriptc.cpp; the tables are copied from the binary. Forms: the scanner's
  `<<EOF>>` rule is the first rule of the .l file, so `case YY_STATE_EOF(INITIAL)` comes before
  `case 1` (the case bodies follow rule order); YY_INPUT copies from the string being parsed with
  `char *where = buf; unsigned long wanted = max_size; unsigned long left = length - pos;
  int n = MBmin(wanted, left);` (other orders change the stack slots); the parser's string `!=`
  is `(bool)a->compare(*b)` (compare inlined, `setne cl; and ecx, 0xff`), all other string
  comparisons are the operators. `__yy_memcpy` cannot be claimed: match.py strips the C underscore
  from the PDB's plain static name as well (reported).
- RenderApp.h is converted (MfAiAppWorld added to MfAi.h). MfAiHook.h declares the hook manager,
  MfHookFunc, MfHookPosition and MfInterfaceRegistry (not dllimport: the game calls the thunks);
  MfLogger.h only EnableWindowOutput; MfInitializeRuntime is in MfAi.h. SBMotivateDriver passes
  string literals to the `const MfString &` parameters (implicit conversion: the temporary's
  address is recomputed with `lea` and the EH state stored after the pushes); an explicit
  `MfString("...")` temporary uses the constructor's eax instead. Hook ids are four-character
  literals ('WLSp', 'WDSp', 'WLAc', 'WTel', 'WDAc', 'WAAc', 'WCAc') and MF_HOOK_USER_EVENT (18),
  MF_HOOK_FILE_DESTINATION (32).
- SBMovieNode::OnStart: `MBComPtr<T> p(MBIID_X)` calls CreateObject out of line, `p.CreateObject()`
  written out is inlined; an argument loaded before the object expression (`mov esi, [ebp+0xb0]`
  before `GetRoot()`) is a local assigned in a statement of its own. The function's inliner budget
  shows: the later else-if branches get the deeper inlining (compare inlined, assign(const char *)
  inlined, the inventory's Release inlined) and `==` versus `.compare() == 0` makes no difference;
  open only for the RR branch's Release.
- CSBNodeManager::AddNode(const char *) matches: an if/else chain of `spaceCode == "MV"` .. "UI"
  creating each node with `new X(mEngine, spaceCode)` (SteamboatNode as the default), then
  `nodeManager->QueueSwap(node)` and a Release of `GetActiveNode()`. SBControlPanelNode.h is from the
  PDB (only its constructor is used so far).
- CSBInventory.cpp keeps the original lines of its `THROW_ON_ERROR` functions: new functions go into
  the blank gap before CreateInventoryAnim and take as many blank lines out of it (scratch helper
  invgap.py); an added `#include` costs a blank line too. Forms: the constructor zeroes the GUI anims
  with `memset` and calls `resize(10)`; StartTrackingClick sets `m_bIsTrackingClick` whenever the bar
  is active (outside the visible test); ListenToEvent falls from the anim-event switch into the
  timer case; Show catches `long` and prints the SBInventory.cpp path. The item positions are a
  `static const MBPointStruct kItemPositions[10][13]` (hide frames first, then the pop-up frames).
- SteamboatNode::LoadDialogue matches and emits SBDialoguePropertyManager::GetInstance out of line
  (claimed in SteamboatNode.cpp).

## Graphics library (wave 8: shapes, vertex clusters, bounding volumes)

- Shape family headers from the PDB: Gfx_IndexedVertexShape.h, Gfx_IndexedTriangleSet.h,
  Gfx_PackedVertexShape.h, Gfx_PackedTriStripSet.h, Gfx_VertexClusterAttribute.h (+ ClusterData);
  Gfx_PolyLineSet.h is not in the PDB (members follow PackedTriStripSet: an Array<int> and an
  Array<BoundingBox>). Array.h (EAIStd Array<T>: Vec<T> with indexing, no own members).
  The DXGfx shapes have no PDB type either; DXGfx_PackedTriStripSetD3D.cpp uses the real headers now.
- `int clamp(int, int, int)` (Vec.h) has the float clamp's ternary shape; `SquareMatrix::operator[]`
  is `elements[clamp(i, 0, N - 1)]`. VC6 calls clamp out of line once a function's inline budget is
  spent (the shape constructors): the constant-index stores after that are direct. PackedVertexShape()
  needs `SS(3, 0) = 0.0f` (operator(), one expansion less) to spend the budget like the original.
- `Vec::verify`: `if (i >= len) { if (i >= alloclen) { int n = i + 1; ...reallocMem } else
  initElements(len, i + 1); len = i + 1; }` (the `n` only inside the growth branch).
- The `static const short` binding masks and `Version` of the shape units are defined after the
  code (the original loads them from memory). `Get*Binding()` is `(Bind)(short)((m_Binding & Mask)
  >> Shift)`; SetBindings is one expression that updates all three fields.
- No-alias code in these units: `#pragma optimize("a", on)` at the top, `("a", off)` around the
  destructors (they store each member's BasicObject vftable before loading the next pointer) and a few
  others (VertexClusterAttribute's constructor, PackedVertexShape's constructor, NormalizeNormals).
  Header inline code must be parsed with /Oa on for the inlined `initElements` of float/int to become
  rep stosd and for member pointers to stay in registers across calls (pragma after the includes is
  not enough there).
- Writes with a WriteHeaderInfo copy of the name (`unsigned char header[22] = "GfxIndexedVertexShape"`)
  test `<= -1`; the ones that pass the literal test the result for non-zero.
- Copy loops: three-float copies through `float *src/*dst` locals (`dst[2] = src[2]; dst[1] = ...`,
  ReadInternals); colour packing indexes the list (`m_CList[k + 3]`), a pointer local changes the
  base offset. `(int)(c * 255.0f)` per channel, then `color = (color << 8) | ...`.
- `float elapsed = (float)(time - m_LastTime);` (HandleTime converts before the m_Play test).
- BoundingVolume: VC6 evaluates the right operand first, so `a.GetCenter() - GetCenter()` calls
  GetCenter() of this first; the original's order needs `Vec3<float> d = a.GetCenter(); d -= GetCenter();`
  and `Vec3<float> p = c.GetCenter(); p += d * r;`. FindClosestPoint results are often taken from the
  out parameter (`FindClosestPoint(poly, a, b); return (a - GetCenter()).mag2();`). A direct call of a
  virtual overload from a member needs the qualified name (`BoundingBox::Intersects(Ray3f(...))`).
  Locals declared inside the loops give the original frame layout (Intersects(Polygon3f)).
  `return cond;` and `&&` chains give `mov eax, 1 / xor eax, eax`; `if (...) return true; return
  false;` gives `mov al, 1`.
- Open (multVec3PointLeft and the inverse-affine row sums): the original loads `m[3][j]` first and adds
  the last product with `fadd st(1)` + `fstp st(0)`. Measured in a test unit: VC6 /O2 reassociates the
  float sums of every form tried (left-to-right, right-nested, separate locals, `+=` chains) so that the
  constant term is added last from memory. Not reproduced.
- Text::operator+=(const char *) is inline (copyOnWrite check, then TextPrimitive::append).
- New scratch helper idea that paid off: diff the instruction text with offsets dropped
  (difflib over match.sh --show columns) to see moved instructions instead of shifted listings.
- `#pragma optimize("a", off)` around single functions that reload members across calls in the
  original (SBActorManager::DeleteActorByIterator, BitmapBuffer::LoadBackgroundSurface); EG2_Bitmap.cpp
  (EG2Bitmap.obj, library code) turns it off for the whole unit after MBInterfaces.h.
- CSBInventory: AddInventoryItem has the same `try`/`catch (long)` as Show; RemoveItem compares
  `strInstanceName == item.strInstanceName` (operand order); StopTrackingClick's switch maps the five
  GUI ids and the ten item ids (written, TODO for registers).
- BitmapBuffer (new functions): MergeRectangleLists sorts both lists with `std::ptr_fun(CompareRects)`
  and coalesces with IntersectRect/UnionRect; the constructor reserves 20 rectangles per list.
- CLinkingLogsTile::tileinfo's 8-argument constructor is out of line in SBLinkingLogsGameBoard.obj
  (defined in CSBLinkingLogsGameBoard.cpp).

## DirectX wrapper layer (wave 9)

- New headers: DX_DirectDraw.h (DirectDraw + D3DDriverInfo, DXDisplayModeInfo, DDDeviceInfo from the PDB),
  ddutil.h (the SDK sample's C helpers, `_DDSetColorKey` etc.), DX_Viewport.h (no PDB type: Direct3D,
  Device, a D3DMATERIAL7, a D3DVIEWPORT7, the background D3DCOLOR). d3d7.h gained DDCAPS_DX7,
  D3DDEVICEDESC7 (from the PDB) and HMONITOR. ValList::operator[] is inline (its exact form is under
  wave 10).
- COM calls go through small inline wrappers of the DX classes (Device::Clear/SetViewport/GetViewport/
  BeginScene/EndScene, DirectDrawSurface::IsLost, DirectDraw::GetCaps/CreateSurface(..., outer));
  calling the interface directly changes the register that holds the object.
- Catch clauses: the handler's adjectives in the EH data tell `catch (DX::Exception)` (0, by value)
  from `catch (DX::Exception &)` (8) and `catch (...)` (no type). Viewport::BeginScene catches by value.
  This may also be what the RenderManager/ViewD3D catch-block TODOs need.
- Error message switches (DX::Exception::ErrorToString, Device::GetDDErrorString): the case values can
  be recovered by emulating the compare tree and jump tables (scratch tool emu/sw.py); writing the cases
  in the order of their return blocks in the original reproduces nearly all of the layout.
- `DDSCAPS2 caps; memset(&caps, 0, sizeof(caps)); caps.dwCaps = ...` keeps the dead zero store of
  dwCaps (the original's form); `= { 0 }` drops it. Struct init order: the original assigns members in
  source order without /Oa (Surface::Create stores dwSize last).
- `switch` with an explicit `case 16:` body identical to `default:` keeps its own jump-table entry.
- CSBLinkingLogsGameBoard.cpp defines the prefs format constants of the original
  (`const char PSZ_PROPERTIES_SECTION_NAME[] = "level %d types"`, PSZ_LAYOUT_SECTION_NAME,
  PSZ_TYPE_VALUE_FORMAT, PSZ_ROW_COL_FORMAT, in that .rdata order) and the MBStrings
  STR_ANIM_FILENAME_PREFIX ("LLAN") and STR_DEFAULT_PROPERTIES. Spaces are stripped with
  `str.resize(std::remove(str.begin(), str.end(), ' ') - str.begin())`. LoadTypeAttributes and
  LoadTile are written (TODO: register and stack-slot choices).
- SteamboatNode: `clamp` is `if (upper <= lower) return lower; if (n >= upper) return upper; if (n <=
  lower) return lower; return n;`; DrawDirtyRects walks a file-static `MBVector<tagRECT>
  sDirtyRectVector`; CalculateFrameRate prints with a file-static `bool sUseLayered3DRender = true`.
  OnQuit (matches) inlines Gfx::Display::Destroy (`delete mInstance;` only, no reset) and
  Graph::DataBase::Destroy (`delete sCurrentDataBase; sCurrentDataBase = 0;`).
- `?clamp@@YAHHHH@Z` is Vec.h's inline clamp; SteamboatNode.cpp claims its out-of-line copy as an
  instance (it has no clamp of its own).
- CSBInventory::Save reuses one index variable for both loops; Load builds the first "recycle
  item %d" key before the loop and the next one at the end of the loop body (the original's two
  sprintf calls share one tail).
- AddAllObjectDirtyRects compares two IMBUnknown
  identities as `pObjectUnknown - pRenderUnknown` and `continue`s on equality.

## SimpleNode/ScriptHelpNode copies, game modules (wave 8, worker B)

- The unnamed functions of SimpleNode.obj and ScriptHelpNode.obj are static initializers: claim them
  by our `_$E<n>` names (the original has no names there, so match.sh accepts any of ours with the
  same bytes; map them by bytes, scratch efind.py). The pairs are jmp stub `_$E<n+1>` then body
  `_$E<n>`. The ctype guard pair (`_$E78`/`_$E77` in ScriptHelpNode, the same at 0x10117740 and
  0x100e1e00) is listed as one 44-byte function (guard, nop padding, atexit stub): not claimable
  until the tool splits it (reported).
- MBEngine's SimpleNode.cpp serves both builds: OnPostFlip has an empty body (matches /GX and
  non-/GX); CloneAnim declares the MBComPtr after the Clone call. Open in the /GX copy (inline
  budget): the constructor, ListenToEvent, Database ctor/Initialize, `_Grow<unsigned short>`,
  `MBComPtr<IMBFileSpec>::AddRef` (not emitted), ScriptManager's destructor (CreateObject inlined).
- Graph::FileStream (Graph_FileStream.cpp/.h, typed members now): the scene library is not no-alias
  code, so the unit turns "a" off after its includes (MBInterfaces.h pulls in MBContainers.h's "a"
  on); Close and Open reload members after every call only then. `if (mode == ReadMode)
  Open(spec, 1); else Open(spec, 2);` (not `?:`). The unit defines the binary's only `operator
  delete[]` (??_V). Open(Text, Vec<Text>, mode) stays open: Vec<Text>::append(const T *, int) is
  called out of line in the original, inlined by VC6 (as in Gfx_LODManager).
- SBRenderObject.h is converted (from the generated header); SteamboatNode.h befriends
  SBRenderObject (Draw calls the protected RenderCharacters). `mPos + mParent->GetPosition()`
  (operand order), the Release of SBTimeBomb.
- SBControlPanelNode.cpp, SBGridlockNode.cpp are new. Forms: `mCodes` cleared with a loop (it
  becomes rep stosd/stosb), ConvertDigitsIntoCode with one `return code` after an if/else;
  `MBComPtr<IMBMouseable> btnMouseable(mButtons[i])` inside the loop; the Gridlock statics
  (`MAX_LEVEL_PROP_NAME`, `BOARD_ORIGIN`, `BOARD_TILE_SIZE`) are file statics of each node (the
  LinkingLogs header's extern declarations are wrong); a stream released with a nested
  `if (stream) { stream->Release(); stream = 0; }` inside `if (stream)` (try it for
  SBLinkingLogsNode::InitGame); `if (x) StartSound("a"); else StartSound("b");` (not `?:`).
- Geodesic (no debug information in the PDB: layout and names reconstructed; it uses the classic
  iostreams of <iostream.h>/<fstream.h>/<iomanip.h>): `const double DEG2RAD` (a variable: loaded
  first, `DEG2RAD * x`) but `#define RAD2DEG` (a literal); expressions written out in full where
  the original recomputes them (`acos(-1.0 / 3.0) * RAD2DEG` per call, the radius fields read
  three times in axial_angle_A); chord_length's angle copies in named locals (face_angle_A/B);
  `(value < 0.0) & (value > -1e-13)` with the bitwise and; face_quantity returns an uninitialized
  local (it lives in the parameter slot); iostream chains break where the original starts a new
  statement on `out` (the manipulator calls are rebuilt from `out` each time, a chained call uses
  the returned reference); the save_ascii loop counter is `int` (operator<<(int)).
- Steamboat_pch.obj (45 open): only static initializers ($E70..83 the math constants PIOver2,
  PIOver4, InversePI, DegreesToRadians, RadiansToDegrees; $E232..279 the colour constants and
  colour masks; $E290..297 the ctype/collate ids). Their numbers need the original precompiled
  header set (as for SteamboatNode.obj's `$E`), not attempted.

## DirectX layer, display, Direct3D (wave 10, graphics worker)

- ValList::operator[] is `if (cnum == i) return curr->elem; if (cnum >= i) { curr = head; cnum = 0; }
  while (cnum < i) { cnum++; curr = curr->next; } return curr->elem;`. The early return, the `>=`
  and the reset order each change the code (jl and not jle, curr reloaded at the end, register of
  the head load). The out-of-line operator[] and GetCurrentDevice instances match only with this form.
- DirectDraw inlines (no out-of-line copies): GetCurrentDeviceIndex = `m_DirectDrawDevices.Index()`,
  GetCurrentDevice = `m_DirectDrawDevices[GetCurrentDeviceIndex()]`, GetDisplayMode,
  GetDeviceInfoPtr(i) = `&m_DirectDrawDevices[i]`, GetCurrentDeviceInfo = `m_DirectDrawDevices.Curr()`;
  Direct3D: Driver() / Driver(int) / DriverInfo() / DriverInfo(int) (DriverInfo(int) saves the
  driver cursor, copies the entry, restores the cursor), CreateDevice, GetZBufferFormat.
- Code that walks a device's lists keeps the device index in a local and indexes
  `m_DirectDrawDevices[device]` again for every field (EnumDisplayModes, PickDisplayMode,
  DisplayD3D::SetDisplayMode). Calling GetCurrentDevice() again re-reads the cursor and does not match.
- DisplayD3D reaches IDirectDraw7 through `m_pDirect3D->GetCOMInterface()` (DirectDraw makes
  DXGfx::DisplayD3D and DX::DirectDrawSurface friends) and checks it for null before
  SetCooperativeLevel/CreateClipper/CreatePalette/SetDisplayMode. The inline wrappers have no null test.
- The clipper's window comes from `((DisplayD3D *)Gfx::Display::GetInstance())->GetMainWnd()`, not this->mhMainHwnd.
- SetRenderState(type, value): `HRESULT hr = DD_OK; switch ... hr = ...; break; ... return hr;` (the
  original keeps hr in a zeroed register); returning from each case gives another layout.
- Display.obj: Initialize and SetDisplayMode match only when the whole unit is /Oa (with the pragma
  above `#include "DX_Direct3D.h"`). A per-function pragma, or the pragma inside the headers, has no
  effect. With the pragma, six other functions of the unit break, so it is left out (TODO lines).
- Device::Create / CreateZBuffer: the original calls ValList<D3DDriverInfo>::operator[] and
  operator[]<DDDeviceInfo> out of line inside DriverInfo(int) and Driver(int), but inlines them
  elsewhere in the same function. VC6 here inlines them all; these stay TODO.
- TextureMap has no PDB type: it derives from DirectDrawSurface (+0x8c the texture surface it
  releases, +0x90 a bool). DirectDrawSurfaceAttrib is defined in DX_TextureMapManager.h.
- ddutil's DDLoadBitmap takes a fifth argument (the DDSCAPS flags; DirectDrawSurface passes
  DDSCAPS_OFFSCREENPLAIN).
- `Gfx::Display` has an inline empty destructor. The out-of-line copies are emitted in Display.obj with DisplayD3D.

## Minigames (wave 10, worker E)

- `IMBWorld::AddObject/RemoveObject` are inline helpers (`GetRoot().AddObject(o)`, defined in MBEngine's
  IMBWorld.h): `mWorld->AddObject(button)` loads the argument before the GetRoot call, `mWorld->GetRoot().AddObject`
  after it (SBSongbirdNode::OnStart and LoadSongs matched only with the helper form).
- Virtuals reached through a secondary base get `this` adjusted in the listing: Idle (IMBPeriodical, +4) and
  ListenToEvent (IMBEventListener, +8) address members 4/8 bytes lower than the class layout says.
- `MBGetWorldMouseLocation(MBPoint &, IMBWorld **)` is called through the import thunk (MBRuntime.h).
- A hint for the dialogue is `SBMouseEvent *e = new SBMouseEvent; e->data = "..."; broadcaster->
  BroadcastEvent(700, 710, e);` (SBTrashBashNode::OnLMouseDown, SBSongbirdNode::OnSongbookClick).
- `int iSong = 0;` declared before `char key[8] = "song0";` stores the counter first (LoadSongs).
- The SBSongbirdNode constructor from a space code starts with `mCurrentSong(1)`, the MfAiSpace one with 0.

## Menu and Gridlock leftovers (wave 11)

- Loop forms: `while (true) { x = find(...); if (npos == x) { last; break; } ...; }` keeps the loop
  unrotated (one find call, `jmp` back to the top) where `for (;;)` with the same body duplicates the
  test at the bottom (GetTokenList). Source order of the two push_backs gives their EH states.
- `a == b` on strings (operator== inline, compare out of line) vs `a.compare("x") == 0` (compare's
  const char * overload inlined to compare(0, len, s, n)): LoadTileProperties uses both in one
  if-chain, GetLoadGameSelectorData the operator== form.
- `mapTileProp[nType] = kNone` is the inlined map::operator[] (pair of key and `T()`, insert out of
  line); `(int &)mapTileProp[nType] |= x << 24` the read-modify-write on the node.
- `strData.resize(std::remove(strData.begin(), strData.end(), ' ') - strData.begin())` gives the
  erase/append pair of an inlined resize (needs `<algorithm>`).
- A local declared at function scope instead of inside the `if` can take an extra stack slot
  (~CSBGridLockGameBoard's iUnknown: four bytes more frame, as in the original).

## Sandwich node (wave 10)

- A file-scope `const float k = 0.001;` (double literal) is folded: the code multiplies by a 4-byte
  constant named after the double value (`__real@4@3ff583126e978d4fe000`), where `0.001f`, `(float)0.001`
  give the float name and `* 0.001` a double constant (SBSandwichNode::Idle). `const float`s initialized
  with float literals stay named variables loaded from memory (kLaunchDelay ...). Try this for
  Actor::UpdateHeadData's 0.03.
- `MBPoint3(x, y, z)` from three floats evaluates z, y, x into temporaries; when the original converts
  x, y, z in order straight into the point, the source had `long x = ...; long y = ...; long z = ...;`
  first (Idle).
- An abs written as a static inline function (`Abs(float)`) compares once with `fcom` and negates the
  same copy; the open-coded ternary reloads. `sqrt(dx * dx + dy * dy)` through an inline
  `Distance(dx, dy)` evaluates dy first and compares `< max` without reversing (Idle).
- `while (anim->Show()) ;` (show until visible) also in ActivateGame; switch cases that end with the same
  call are written with the call after the switch (ListenToEvent's Beulah states); a sound id test reads
  `(*desc)->GetSoundID()` through `IMBSoundDescription **` each time.

## SteamboatNode's big functions, LoadFromStream (wave 12, worker A)

- Temporaries die at the end of the full expression: when the original destroys a call's MBString
  temporaries *before* it evaluates the rest of a condition or the next call, the result went into a
  local first (`long afterAct = pref->GetLong(stream, MBString(key), MBString("after act"), 0); if
  (afterAct <= gs->GetProperty(...))`, `long behavior = pref->GetLong(...); SetBehavior((MBButtonBehavior)
  behavior);`). This made LoadHotspots and both LoadFromStream functions match.
- `cmp reg, ebx` (ebx = 0) instead of `test reg, reg` for a call result: the result is a named local
  (`bool talking = mDialogue.Update(sMfPrimaryActor); if (talking)` matched OnPreRender; same for
  OnStart's GetStream results).
- Read the jump targets before writing an if: in On3dLMouseUp ResetCursor sits after the
  `if (receiveItem && ...)` block (both branches jump to it), not inside it.
- `atan(x)` compiles to `fld x; fld1; fpatan`; `atan2(x, 1.0)` loads 1.0 from memory (SetView).
- A `static const double` computed from another constant is folded by VC6; the original's
  RadiansToDegrees is loaded from memory (dynamic initializer, $E82), so it is a non-const static
  here (SteamboatNode.cpp).
- Switch case bodies are laid out in source order; the original's order is visible from the body
  addresses (ListenToEvent: -1 before -9, 300 before 200, events 203/202/201 in that order).
- `__LINE__` users in SteamboatNode.cpp (OnStart 637, OnPreRender 1373/1384, On3dLMouseUp 1631/1648,
  UserEventHook 2056/2061/2066 with Allman braces there, Idle 2086, ListenToEvent 2867/3020, OnItemClick
  3118/3133, CreatePrefAndStream's throws 3390 .. 3402) have marker comments "... original lines N ..."
  followed by blank padding. To re-pad after an edit above a marker: for each marker, change the number
  of blank lines after it so the first `__LINE__`/`THROW_*` statement after it (not a `#define`) lands
  on N (worker A keeps a script, repad.py, in its scratch directory; it is a no-op on the committed file).
- RenderManager::GetView and GetViewCamera have the find test (`find == end` → 0) like
  GetBitmapBuffer; Begin/EndRenderView index mViewMap directly (no claim changed).
- SetView is not no-alias code (`#pragma optimize("a", off)` around it): the original reloads
  mMfCameraNode after the calls and calls map::find out of line.
- MBLocale (MBEngine's MBLocale.h now; inline ctors, generated dtor whose out-of-line copy SteamboatNode.obj
  emits), MfCamera.h, MfAiViewEventHandler (MfAi.h), MBsrand / MBGetCurrentDirectory (MBRuntime.h).

## Game classes (wave 13, worker A)

- An animation loaded by name (CSBClickable::SetAnim, CSBAnimButton::SetAnim): `IMBStream *stream;
  MBComPtr<IMBStream> tempStream; mgr->GetStream(id, 'anm ', &stream); tempStream.Attach(stream);`, and
  the "NULL animation in ..." sprintf runs unconditionally at the end (both originals fall into it).
- ViewEventHandler: HandleLButtonUp copies the pick point per component (`pickData.pickPoint.x() =
  hit.pos.x();` ...), Down assigns the vector; ConstructRay zeroes position/direction per component and
  uses RenderManager::GetView's find test. MfColResults.h and MfAiColNode.h are the game-side copies.
- Piccolo objects in the game: MfScDataHandle.h (SetType stores the whole flag word: CleanupExitSceneEvent
  writes 0x12), MfScDataObject.h (MfScData/MfScDataObject/MfScObject/MfPoBase in vftable order),
  MfPoEvent.h, MfPoVector3.h. CleanupExitSceneEvent is alias code (`#pragma optimize("a", off)` around
  it: the original reloads the handle pointers after each store), destroys the objects with explicit
  destructor calls and frees them with MBfree_release.
- CStdStr(const std::string &) assigns through `ssasn(*this, str.c_str())` (the char-pointer form):
  _SS::Left's out-of-line copy (Actor.obj) takes that path. Left calls substr(0, nCount) directly
  (no min with size(), as in the original); its copy is still a register-choice TODO.
- SBCRTNew.cpp: the CRT debug `operator new(size_t, int, const char *, int)` calls MBmalloc_debug.
- SBLinkingLogsNode::OnStart, SteamboatNode's near misses whose only differences are inlined/called
  helpers, registers and slots are `EQUIV reviewed` (see each line).

## Scene graph: Node, BVolHierarchy, Camera (wave 14, worker A)

- Gfx_Node.cpp ends with `template class Gfx::BVolHierarchy<Gfx::BoundingBox>;`. The explicit
  instantiation moves the unit's `_$E<n>` counter, so the three trailing static-initializer claims
  (0x100e4f00/4f30/4f40) are renumbered whenever a function is added; read the new names from the
  compiled object, never by guessing.
- Prune and the hierarchy Builds assert with GFX_ASSERT (Gfx_Assert.h) and the literal
  "D:\\tugboat\\code\\SDK\\Scene\\Graphics\\BVolHierarchy.cxx": Prune's `this != NULL` is spelled
  `pHierNode2 != NULL` (line 1428), BVolHierNode::Build's is `nLeafAtoms > 0` (line 1139).
- BVolHierNodeFactory<T> (12 bytes): the constructor allocates 2 * nAtoms nodes, Reset() rewinds,
  NextNode() hands out m_ppNodes[m_nUsed++]. BVolHierarchy::Build doubles with `shl` where VC6 gives
  `add` (the only difference, reviewed EQUIV).
- BVolHierNode::Build assigns its volume through BVolHierNode::operator=(const T &), which the linker
  folded into BoundingBox::operator= (the call goes to ??4BoundingBox). The rest is TODO: the original
  calls BoundingVolume() and Vec<int>::append out of line where this compile inlines them.
- TMatrix4.h carries GenMatrix<T> (the general matrix behind SquareMatrix::invert/gaussJordan) and the
  SquareMatrix/TMatrix4 helpers the camera needs (multVecLeft, extractRow, transposeCopy, invert,
  setTranslation); Vec2.h is Vec2<T> : FixedVec<T, 2>. Camera::CalcWorldPointFromScreen and Refresh stay
  TODO: the original calls those template helpers out of line, VC6 inlines them here.
- SquareMatrix::gaussJordan (TMatrix4.h) is Numerical Recipes' gaussj with full pivoting: three
  Vec<int>(N) index arrays, |pivot| < FLT_EPSILON fails, b's rows go through GenMatrix::operator[]
  (clamped row times m_Capacity). First draft, 32%.

## Static initializers of the game units (wave 14, worker A)

- Every node unit of the original has its own kCursorItemString copy (SBCursorItems.h) and the
  ctype<unsigned short>::id / collate<char>::id initializers (`#include <locale>`). Units whose code
  uses `__LINE__` (ConfirmationBox, SavedGameDB, CSBInventory, SteamboatNode) include them after the
  code so the original line numbers stay put. Claim them by bytes: `// TODO 0x<va> $E0: ...` lines,
  then tools/recomp/claim_e.py; after an include change, turn the file's `_$E<n>` claims back into
  TODO lines and claim again (EQUIV lines on $E functions carry our own name and need renaming too).
- Class statics need a guard bit each ($S): define them after the unit's function-local statics
  (CSBInventory after MakeCSBInventory) or the bits shift. Definitions added: CSBInventory's anim
  names/positions and kGUIBarPosition, PathTile's own kmapDirectionVec, SBLinkingLogsNode's
  BOARD_ORIGIN/BOARD_TILE_SIZE, SteamboatNode's sFlyDir(".")/sMofaDir(".")/sTextControl/... ("").
- MBColor.h is inline (the engine's bodies). The colour constants live in the precompiled header
  (Steamboat_pch.obj initializes them all); a unit re-emits only the ones it uses, so SteamboatNode.cpp
  defines kCM_ARGB32 (declared extern in the header, MBColor's constructor converts through it) and
  kYellow. Convert takes the mask by const reference: by value VC6 copies kCM_ARGB32 to the stack
  first, which the original does not.
- MBComPtr::QueryInterface (-2 when empty, the result through a local): a MBComPtr built from a
  pointer to another MBComPtr uses it; SteamboatNode::ClearHotspots passes its iterators, which gives
  the original's three _QueryInterface<MBComPtr<...> *> instances.

- Minigame statics: SBSandwichNode's points are copy-initialized (`MBPoint3 kLaunchPoint =
  MBPoint3(108, 206, 200);`): the original loads the three values into registers and stores them,
  direct initialization stores immediates. SBTrashBashNode has `kGoofyVOID[]`, the MBHashString of
  each kGoofyVO line, right after kGoofyVO.
- Inline budget, measured on the dialogue classes: `~SBdBlock` and `~SBdLineGroup` empty their
  vectors with `v.clear()`, not `v.erase(v.begin(), v.end())`. The extra clear() level costs enough
  that the erase bodies stay calls, as in the original; both destructors and the three erase
  instances (0x10048fd0, 0x10049030, 0x10053db0) then match. Defining Vec::append(const T *, int)
  after its users (to keep it out of line) changes nothing in LODManager::SetRanges.

## Graphics library (wave 12, worker G)

- BoundingVolume: the Ritter sphere, Arvo box transforms (`TransformAffine`/`TransformInverseAffine` add
  the smaller/larger product per matrix entry), the slab tests in double precision and the division-free
  segment slab test are written out. Two literal quirks of the original are kept and commented: the
  RVec constructor reads y/z from `p[3]`/`p[6]`, the box ray test indexes the ray and the box with the
  face number (2 * axis), `BoundingSphere::operator+=` weights with `m_Radius - r / (2 d)`.
- Forms that matched there: explicit per-component arithmetic (`d[0] = a[0] - b[0]; ...` and
  `length[0] = (float)fabs(d[0])`) where the original does not copy through a Vec3 temporary; the
  discriminant as one expression (`r * r - (toCenter.mag2() - p * p)`); `if (...) return true; return
  false;` after a slab test; a ternary chain for the far/near slab picks; the outer face switch with a
  `return true` per case; `best = t1 < t2 ? t1 : t2` in the `i == 0` branch and a separate `distance`
  local in the other (FindClosestPoint(point)).
- `TMatrix4::multVec3PointLeftInverseAffine` is `multVector3Right(Vec3(v[0] - m[3][0], ...))` (the
  out-of-line copy and the callers that call multVector3Right out of line show it); multVector3Left
  and multVector3Right have inline bodies now (all their callers are TODO because of the row sums).
- The row-sum problem (multVec3PointLeft, multVector3Left/Right, inverse affine): the original adds left
  to right and keeps the partial sum on the FPU (`fadd st(1)` + `fstp st(0)`). Tested without success:
  /Op (`#pragma optimize("p", on)`), /Oa, vc6-rtm and vc5, loops, separate locals per row, `+=`
  chains, `r.e[i] +=` forms (closest: three `+=` statements, 84%). A pointer-to-member initializer
  (`Vec3<float> (TMatrix4<float>::*p)(const Vec3<float> &) const = &TMatrix4<float>::multVector3Left;`)
  forces an out-of-line instance in a scratch unit for such experiments.
- `LineSegment3f::Transform`/`TransformInverseAffine` have no out-of-line copies: inline in
  Gfx_Ray3f.h (end points through multVec3PointLeft / multVec3PointLeftInverseAffine); the callers copy
  the returned segment with `rep movsd`, written as `segment = edge.Transform(m)`.
- `BoundingBox(const float *, const float *)` and `BoundingBox(const Vec3, const Vec3)` are inline
  (no out-of-line copies). `FixedVec::unhomo` is `multScal(1 / e[N - 1])`.
- IndexedTriangleSet's sphere constructor uses Geodesic (A's Geodesic.h): its `parameters` struct lumps
  +0x0c..+0x63 into `char filename[88]`; the constructor initializes a long, an 80-character name
  ("test") and another long there, so the unit declares a layout-compatible `GeodesicSetup` and casts
  (Geodesic.h should get the real split). The unit has its own `const double DEG2RAD`.
- `IndexedVertexShape::InitOffsets(int version = Version)`: the default argument is how the derived
  constructors pass the private `IndexedVertexShape::Version`.
- IndexedTriangleSetD3D (no PDB type) members are named now (m_Textured, m_nTriangles, m_D3DIndices,
  m_FVF, m_DrawFlags, m_StridedData); d3d7.h got D3DDRAWPRIMITIVESTRIDEDDATA, the D3DFVF_* flags and
  D3DRENDERSTATE_EXTENTS. The three converting constructors leave m_Textured uninitialized.
  The cull mode is three `hr = device->SetRenderState(CULLMODE, ...)` calls in if/else (a ternary
  argument gives the `neg/sbb` form instead of the original's three pushes).
- Gfx::Camera::SetFOV/SetWorldMatrix and Quaternion::toMat have bodies now (A's SteamboatNode::SetView
  comment); no claim changed.
- Round 2 (G): Graph::DataBase::Create is inline now (SteamboatNode::Init 63% -> 81%). Inline
  DisplayD3D::SetFrontBuffer/SetBackBuffer raise SteamboatNode::CreateDisplay from 54% to 70% but make
  VC6 inline ValList<DDDeviceInfo>::operator[] there, so the instance claimed in SteamboatNode.cpp
  (0x100b4210) is no longer emitted: left out-of-line (declarations only) until A decides.
- TextureAttributeD3D (no PDB type): texture stage state copies at 0xb8..0xcc (colour op/args, alpha
  op/args), DirectDrawSurfaceAttrib at 0xd0. SetTexture assigns hasColorKey before isMipMap (store
  order); SetTextureStageCompositeMode is an if/else chain on the mode after the common defaults.
- Round 2 (worker A): many template instances (map/_Tree insert, _Insert, _Construct, pair ctors,
  list _Buynode ...) of SBActorManager.obj, SBLinkingLogsGameBoard.obj and SBLinkingLogsActor.obj
  already matched but were unclaimed; a scratch loop (match.sh --sym over every open function of a
  module against its file) finds them. Run it after writing a unit's big functions.
- A call result whose temporaries die before the next call goes into a local first (again):
  `MfAiActor *actor = mofaSpace->GetActor(mbActorName.c_str()); LoadActor(mbActorName, actor);`,
  `Actor *actor = (*it).second; actor->PlayDialog(MBString(line));` (the object pointer is loaded before
  the temporary is built).
- A `const float` initialized with a double literal gives the original's `fmul dword [__real@4...]`
  inside a double expression (CSBLinkingLogsActor::Update's GOHOME_VELOCITY = 120.0); `120.0f` written
  inline is widened to a double constant.
- Actor::SetActiveHead also sets mForceHead (SBActorManager::ListenToDialogueEvent inlines both stores).
- Geodesic's `parameters`: +0x0c long, +0x10 char filename[80], +0x60 long (the constructor reads
  neither); the old `reserved` at 0x74 was the alignment gap before the double.
- Complete-source round (worker B): every open function of these modules now has a body or a TODO
  line. The kCursorItemString initializers ($E296..299, 1180 bytes) sit in every node object of the
  original: the node units include SBCursorItems.h (the original SteamboatNode.h must have included
  it). Steamboat_pch.cpp holds the precompiled-header statics (angle constants under their PDB
  names, the colours and colour masks of MBEngine's SimpleNode.h, NULL_VARIABLE). Static initializers
  and header instances that cannot be claimed are listed at the end of each unit with what they
  initialize ("its $E number depends on the original header set"). Geodesic: bucky_factor matches;
  face_factor and the three *_sphere functions are written (60-91% similar).
  equiv.sh --todos found none of the 38 TODOs renaming-equivalent.

## Actor, SBActorManager, RenderManager; reviewed EQUIVs (wave 13, worker B)

- Actor.cpp and SBActorManager.cpp include SBCursorItems.h (its kCursorItemString initializers);
  SBActorManager.cpp and RenderManager.cpp include <locale> for the ctype/collate id initializers.
  claim_e.py claims all their $E initializers. Actor.cpp does not need <locale>: its collate pair
  comes from the other headers.
- Reviewed EQUIVs: each one was checked on a full --show listing, with registers, stack offsets and
  relocations normalized and immediates kept. Only these were accepted: a helper inlined on one
  side and called on the other (Release, _Eos, _Tidy, _QueryInterface, CreateObject, _Inc, assign,
  compare), register and slot choices, and independent instructions moved.
- Kept as TODO: functions where a loop test is written in another form (SBSoupMachineNode and
  SBMapNode OnStart: the original tests i - 100 < n), where size() is recomputed differently
  (SBdBlockRandomSequential), and the destructors with several inlining swaps.
- SBDialogue::Update: the GetProperty result goes into a local before the test, so the MfString
  temporary dies before (int)property as in the original.

## Traversers and attributes (wave 14, worker B)

- AttributeStack (no PDB type): its own vftable (destructor only) and a `Vec<const Attribute *>` at 4;
  Gfx_AttributeStack.cpp. MaterialAttributeD3D: a D3DMATERIAL7 copy at 0xcc and 32 unused bytes
  (size 0x130, NodeFactory allocates it). ShadowAttributeD3D: material at 0x8c, TMatrix4 at 0xd0.
- Operand order: in `a * b + c * d` float statements VC6 here computes the left product first
  (OverrideClusterFrames matched with `w * Interp + t * m`); in Vec3 operator+ calls the right
  operand is built first.
- Header inlines without out-of-line copies: Text::toLower, Text::contains (`int found = ...;
  return found;` keeps the original's setge), Vec4(const FixedVec<T, 3> &, T),
  SquareMatrix::outerProduct/multLeft, TMatrix4::operator*(T)/operator-(TMatrix4).
- Unsolved, the TMatrix4 copy puzzle again: the original builds returned matrices with an
  out-of-line setValues straight into the result (ShadowAttributeD3D::Push, RenderTraverser::Visit,
  InterpolateClusterFrames); `SquareMatrix(const T *) { setValues(m); }` changes nothing.
- STL/Vec template instances compiled at the end of /Oa units: some match only without /Oa
  (vector<float>::insert in PickTraverser, AVec/IVec constructors in VertexClusterAttribute), others
  only with it; one end-of-unit setting cannot satisfy both, so they stay TODO.
- AddHit matches with the TraversalPath temporary in its own block scope.
- Per-function `#pragma optimize("a", ...)` does not reach template or inline STL bodies: VC6
  generates them at the end of the unit with the pragma state there (moving PickTraverser's pragma
  around single functions breaks ClearHits, ~PickTraverser and the TraversalPath vector, whose
  inlined vector code is compiled late too). Explicit class instantiation (`template class
  AVec<Vec3<float> >;`) between off/on changes nothing either; member-level explicit
  instantiation of a constructor is rejected by VC6 (C2909).

## DX layer, shapes, sphere maps (wave 15, graphics worker G2)

- ValList.h `operator[]` matches in the form: early return when out of range (`>=`), then
  `curr = head` and the walk. Copy constructor via AddList; `Add` returns -1 when `!curr`.
- Quaternion(T, T, T, T) must stay a plain declaration: an inline body changes VC6's inline budget in
  Actor.cpp and breaks the clamp claim there.
- `#pragma optimize("a", ...)` around a single function has no effect in the DXGfx shape units
  (PolyLineSetD3D, PackedTriStripSetD3D, as noted for IndexedTriangleSetD3D): only the state at the top
  of the unit (or, for template instances, at its end) counts. These units are mixed: a unit-wide /Oa
  matches some functions (SetupStrips, initElements) and breaks others (Clean, the reallocMem
  instances), so they stay without /Oa and the no-alias functions are TODOs.
- Gfx_RefreshTraverser.h is a real header now (m_pView at +0x44, inline GetView). SteamboatApp/zlib.h
  forwards to ../zlib/zlib.h so that libpng/png.h compiles from SteamboatApp.
- DXGfx_PolyLineSetD3D.h / DXGfx_PackedTriStripSetD3D.h are real headers (the _decl.h files are
  gone). The PackedTriStripSetD3D vtable has 33 slots (no `_slot33`). Rebuild matches only with the
  single `m_Dirty = false` after an if/else (no early return).
- PackedVertexShape::SeparateVertexInfo(V3): one counter per stream (coord, colour, normal, texture),
  each incremented inside its binding test; with that V3 matches.
- ApplySphereMap (IndexedVertexShape, PackedVertexShape): eye vector reflected about the eye-space
  normal. `TMatrix4::inverseAffine()` (PDB name) is the inline rigid inverse: transposeCopy, zero the
  last column, translation = rotated negative translation (operator[] with clamp). The PackedVertexShape
  copy keeps several dead matrices (model, view copies, a second inverse, S*P / SS*P products and an
  unused projection of the reflection); they are in the source because their copies are in the bytes.
  CTM's m_Matrix (identity) is read through the new GetMatrix().
- Display.obj's <locale> initializers are linked apart (0x100e6550/80/90) and match DXGfx_DisplayD3D's
  _$E33/_$E29/_$E32; claim_e.py does not find them (claimed by hand after match.sh confirmed).
- GetDDErrorString: VC6 splits a switch's compare tree at the median of its case groups, and a
  shared body (`case A: case B:`) counts once. The original repeats the D3DRMERR_BADMINORVERSION
  string as its own case for 0x88760319 (D3DRMERR_UNSUPPORTED), and only that gives its split point.
  The case set was extracted by emulating the original's compare tree and jump tables.
- Decorated names over 255 characters (STL `_Tree<string, pair<...>>::erase`) are truncated with a
  hash in the PE/PDB, while VC6's .obj keeps the full name. Claim such instances with the .obj's full
  name; the truncated one gives "cannot choose a function".
- Gfx_PackedVertexShape: a `#pragma optimize("a", off)` at the end of the unit changes the whole
  unit's code, not just the template instances (ReadInternals, SetBindings and others break), so the
  Vec/AVec/RVec(int, float) constructors, whose originals reload `e` around operator new, stay TODOs.
- Shape::Pick tests the ray, moved into the shape's space, against GetBoundingBox() with a virtual
  call. TextureException's constructor lives in Gfx_TextureException.cpp.


## View, VisibilityList, projection, small units (wave 15, worker E)

- x87 values: VC6 keeps a common subexpression on the FPU stack (`fld 2.0; fdiv st(1)` ... `fstp st(0)`)
  but spills a named float local to memory. SetPerspective/SetOrthographic match their FPU code only
  with the repeated expression (`2.0f / (zFar - zNear)`, `-(zFar / (zFar - zNear))`), not with a
  `depth` local. Both stay TODO: the original keeps the constant element stores in statement order,
  and VC6 here hoists them. `1.0f / (float)tan(x)` gives the dword 1.0 constant (no rounding store).
- The same `fld 1.0; fdiv st(1); fstp [arg]; fstp st(0)` around `multScal` is the original's inline
  normalize (View::CalcViewFrustumPlanes, ViewMatrix::SetView). Ours gives `fdivr [1.0]`. Still open.
- View::OverlapPlane: three `float x/y/z = plane.GetNormal()(i)` locals, a nested 8-leaf if tree over
  their signs, `Vec3::setValues(T, T, T)` (now inline in Vec3.h) for the two corners, then `if (d <= 0)
  return Out; return Partial;`.
- VisibilityList Load/SaveVisList use the old iostreams (`<fstream.h>`, `ofstream::open(name, ios::out)`,
  `out << '0'` is operator<<(unsigned char)). The file code is not no-alias code: a `#pragma optimize("a",
  off)` around the two functions. `if (m_Chunks && (n = m_NumChunks) > 0)` gives SaveVisList's load order.
  `<fstream.h>` uses up 16 more `$E` numbers; run claim_e.py on the file after adding it.
- StdString.h: `_SS::operator+=(PCSTR)` is `ssadd`, and that is `if (p) append(p)` (the null test is in
  the bytes of LoadVisList's `m_Chunks[i] += "A"`).
- vector<TMatrix4>::insert: the original calls `setValues(const float *)` where ours calls
  `SquareMatrix::operator=` out of line, and every other byte is identical. So the original's operator= is
  probably `setValues(m)` with setValues not inlined. An alias claim does not work: our operator= returns
  `*this` (22 bytes, setValues has 20). Making operator= call setValues inlines the memcpy instead.
- The vector<Shape *> copy constructor and operator= in RenderList, the vector<float> insert in
  PickTraverser and the vector<ViewFrustumIntersect> insert in RenderTraverser match with "a" off at the
  end of the unit, but that breaks other claims in the same unit. An explicit instantiation inside an
  "a" off region does not help.
- RenderApp needs MBNew.h (??_G calls the MBfree_release import) and <locale> (its three $E).
- Float sum order (wave 16): VC6, like VC5, regroups an unparenthesized `a + b + c` (for
  multVector3Left it starts with the last product). It keeps `(a + b) + c` and a sum built up in a
  local (`T s = a; s += b; r = s + c;`). multVector3Left goes from 57% to 84% that way. The rest is
  the `fadd st(1)` / `fstp st(0)` that keeps the partial sum alive, and no source form tried
  reproduces it (loop, `+=` chain, Vec3(x, y, z) return).
- x87 operand order follows the source in VC6 for a plain `a + b` of two memory loads: the
  BoundingSphere(RVec) centre matched its fld/fadd order only as `dia1[i] + dia2[i]`.
- The multVec3PointLeft(Affine) row-sum problem, measured on scratch units (out/g2t, /FAs): VC6
  always multiplies first and adds `m[3][j]` last as a memory operand, whatever the grouping or
  parentheses (`((m3 + z*m2) + y*m1) + x*m0`), operand order (`v[i]*m[i][j]`), a partial-sum local,
  or `+=` accumulation. The original's shape (fld m[3][j] first, the partial sum kept in st(1)
  and popped after the store) appears only for a float local that is live across statements
  (VC6 holds it in an FPU register, then `fadd st(0), st(1)` / `fstp st(0)`). So the original
  inline probably carries the translation or partial sum in such a value; not reproduced yet.

