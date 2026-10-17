# RuntimeLib notes

Patterns verified against RuntimeLib.dll (vc5 `/O2 /GX /MD`; files that use
`dynamic_cast` need `/GR`, e.g. `MfSkeletonNode_2.cpp`).

## Class headers

- Canonical headers replace the old per-unit fake declarations; use them in new
  code: `MfAi*.h` (application interfaces, from the SteamboatApp PDB),
  `MfRootObj.h`, `MfActor.h`, `MfSkeletonNode.h`, `MfColNode.h`, `MfRuntime.h`,
  and the import headers `MfVector3.h`, `MfRotation3.h`, `MfTransform3.h`,
  `MfString.h`, `MfDblList.h`, `MfTreeNode.h`, `MfProtoNode.h`, `MfColModel.h`,
  `MfProperty*.h`, `MfScData.h`, `MfScDataHandle.h`, `MfIndex.h`, `MfLock.h`;
  `MfSpace.h`, `MfWorld.h`, `MfActivity.h`, `MfLightNode.h`, `MfLiveActorList.h`,
  `MfSpaceList.h`, `MfMsg.h` (message IDs), `MfRenderInfo.h`, `MfRenderScene.h`,
  `MfDataPath.h`, `MfMsgDispatcher.h`, imports `MfColor.h`, `MfLight.h`,
  `MfMaterial.h`, `MfPoLight.h`, `MfPoActivity.h`.
- The activity headers derived from MfActivity (MfAtomicActivity.h ...) are
  still generated stubs; their `unknown_60` padding stands for the members the
  real classes have from 0x60. Replace them class by class.
- Virtual methods called through a secondary base use offsets relative to that
  subobject: an accessor that reads `[ecx+0x28]` in MfSpace::ShowAxis is
  mShowAxis at 0x40. Read inline accessors with that in mind.
- Overloaded virtuals: VC5 groups all overloads of a name at the first
  declaration and lays them out in **reverse** declaration order. PDB headers
  list them in vtable order, so reverse each overload group when writing source.
  A derived class's new overload of a base virtual does not join the group; it
  is appended (MfPropertyTable::Add(const MfProperty &)).
- Multiple inheritance: interface methods called through the second base use
  offsets relative to that subobject (MfSkeletonNode: MfAiActorNode at 0x14;
  MfActor: MfAiActor at 0x14; MfSpace: MfAiSpace at 0x18).
- `windows.h` renames GetClassName/SendMessage (exports say ...A); PlaySound
  keeps its name, so headers include windows.h with WIN32_LEAN_AND_MEAN. A unit
  that includes the full windows.h before MfAiActor.h gets PlaySoundA.
- mflinalg (MfVector3/MfRotation3/MfTransform3) constructors, copies and
  destructors are inline. MfTransform3::operator= copies the rotation first.
- dllimport classes with a virtual destructor: after `new X` the compiler
  stores a local copy of X's vtable and emits a local `??_G`/`??_E`
  (MfColModel, MfPoActorNode, MfIndex, MfDblList). Match those with
  `--sym ??_GX@@UAEPAXI@Z`.
- Small helpers in a .cpp that the image leaves unnamed: give them a
  descriptive external name and match with `--sym` (e.g. ShowAccessKeyMessage).
- Message tables: every MfRootObj subclass with handlers (MfSpace, MfWorld,
  MfActivity) has its own `MsgHandler *mMsgTable` after its bases (8-byte
  pointers to members of a multiple-inheritance class) and a static
  `aMethods[NUM_MSGS]`. InitMsgTable: fill with `&X::DefaultMsgHandler` (a
  vcall thunk `??_9@$B7AE`), set the class's handlers, `mMsgTable = aMethods;
  MfRootObj::InitMsgTable();`. CatchMsg is inline: `MsgHandler handler = mMsgTable[msg.mID]; (this->*handler)(msg);`
  (the one-line form allocates registers differently).
- Empty or trivial virtuals are folded by the linker. The vtable check needs
  them defined in the unit that emits the vtable: inline in the header when the
  function lies in the first object file (10001000-10003700, the COMDAT copies
  of the exported classes' inlines), out of line in the class's .cpp otherwise.
- Implicit copy constructor / operator= of an exported class (emitted in the
  first object): do not declare them; force emission with
  `static X &(X::*s_assign)(const X &) = &X::operator=;` and claim both.
- Calls on a static/global MfDblList object are devirtualized by VC5; where the
  original makes a virtual call it went through a pointer:
  `MfDblList *list = &activityList; list->Append(this);`.
- MfString::Length() is inline (`mfsr->GetLength()`, MfStringRef from the PDB);
  RuntimeLib never imports it.

## Known unmatched shapes

- VC5 sometimes calls an imported inline function instead of expanding it
  (MfSkeletonNode copy ctor: the third MfTransform3 copy calls
  `??0MfRotation3` through the import). No source form found yet.
- ICF in EH funclets: an unwind funclet that calls an empty destructor folded
  with another class's (the image names it ??1MfManipPlanCache@@QAE@XZ) is
  still rejected (MfPoView::CreateClassObject, MfPoSpace class-object ctor and
  Init). Vtable slots folded this way are accepted now.
- Release() of MfSpace/MfWorld (`--mRefCount`, delete when 0): the original
  keeps the count in eax; every form tried compiles to edx.

## Wave 4 (MfLiveActor.cpp)

- MfLiveActor_1..3 are one original object; new and ported code goes into
  `MfLiveActor.cpp` on the canonical `MfLiveActor.h` (all units and their
  unit headers are merged and deleted).
- The pending planner requests are `mLocoRequest`, `mGCRequest`,
  `mReachRequest`, `mMoveRequest`, `mLockRequest` (0xac-0xbc); a planner
  answer is only taken when its ID matches.
- `MfDblList::GetFirst`/`GetNext` are inline virtuals in the real header:
  called on a local or member object they are expanded (pliFirst/pliNext),
  through a pointer they are virtual calls. The ctor uses an initializer list
  (links stored before the vtable). The forms that match the expansions:
  GetFirst `pli = pliFirst; if (pli) return pli->GetData(); return 0;`, GetNext
  `if (pli) { pli = pli->GetNext(); if (pli) return pli->GetData(); } return 0;`
- mflinalg inlines (`MfRotation3::normalize`, `MfVector3(float, float, float)`)
  are inline in the headers; the few imported calls are VC5 not expanding them.
  normalize sums `x*x + y*y + z*z + s*s` (the expansion in SyncTraceUnderHip).
- Exported classes (`__declspec(dllexport)`): every unit that includes the
  header emits all inline members, so Type/IsType/getters of the activities
  (first object region 10005f80-10009000) are claimed from the class's .cpp
  without a body (MfAtomicActivity, MfManipActivity).
- The global at 100f4418 is the MfHookManager (`gHookManager`,
  MfHookManager.h); `CallHooks(id, data)` with the MfHookData structs.
  MF_HOOK_* IDs are in enums.h; other hooks use four-character codes
  ('WCAc', 'WDSR', 'WDAR', 'WDSp').
- `gEventData` (100f59b0): the data of the event being handled (pick data,
  message data blocks), set before the behaviour sees the event.
- Member stores: VC5 emits a group of member stores in an order different
  from the source (a value already in a register is stored first); when only
  the store order differs, search permutations of the assignments
  (RequestManipMoveTo, RequestManipGCPlan, MakeManipActivity).
- A shared `xor eax, eax` just before the epilogue means the function ends
  with `return 0;` and the early returns return something else: nest the
  conditions instead of writing early `return 0`s (ApplyLock,
  MakeFaceActivity, VerifyPlaceSkill).
- `delete p` on a pointer to an incomplete struct calls only operator delete
  (MfPathPoint in HandleStaticActorChange); a type with a declared destructor
  adds a null check before the delete.
- A by-value `MfString` temp passed to a `const MfString &` callee that
  receives a copy: write `f(MfString(name))` (MfFSMManager::IsBehaviorLoaded).
- A virtual call on a member object (not expanded) needs the access through
  a pointer: `(&g_world->mActors)->Find(...)`.
- VC5 does not expand some mflinalg inlines and calls the imports instead
  (`MfRotation3()` and `MfRotation3::operator=` in CorrectPlacement,
  `MfVector3::operator=` in ApplySnapOn); those stay TODO until a header form
  reproduces the import call without breaking the expanded callers.
- MfFloor/MfSplineNode: exported skeleton nodes; ctor, dtor and GetNodeType
  are out of line, Type/IsType/copy ctor inline in the header.

## Wave 5

- VC5's inline budget: in a large function (MfLiveActor ctor) the first few
  inline candidates are expanded and later ones become calls to the imported
  copies (`??0MfRotation3`, `??0MfVector3`, `??0MfTransform2`). Nothing to do:
  with the real member/ctor structure the cut-off falls at the same place.
- Imported mflinalg/mfutil destructors that are inline and empty (`~MfVector3`,
  `~MfTransform3`) still get EH states; the unwind funclets jump to the imports.
- dllimport class `new X` (MfPoActor, MfStringList, MfStringValue): local
  vtable copy plus local `??_G`, and for a second base an adjustor thunk
  `??_E...@@WBA@AEPAXI@Z` (claim with --sym).
- MfMsgDataBlock and its subclasses: `virtual ~X() { FreeContents(); }`; the
  call keeps the vptr resets that an empty destructor drops.
- An if/else whose branches call the same function with different arguments
  is tail-merged into one call (MfActor::GetSkillFromProperty); a conditional
  expression gives a temporary instead.
- A temporary that the original destroys right after the call but whose
  address is recomputed (`lea ecx`) instead of taken from the ctor's eax is a
  named local in an inner block (MfRenderer::LoadPlugin).
- A bool tested with `cmp al, bl` / `test al, al` after `sete`: the original
  stored the comparison in a bool variable (MfRenderScene::Sync).
- Loops over a static array whose pointer compares are signed (`jle`) were
  written with an int index (MfRenderScene::Sync).
- Register choices can depend on unrelated header content: making
  MfProtoActor::Path() inline flips MfActor::ignoreTriangleCollisionCheck
  (TODO there). After changing a shared header, check the whole directory.
- `_$E<n>` numbers of static initializers change with every header that adds
  declarations; re-read them (`match.sh --sym XX` lists the defined symbols)
  after header changes.
- Interfaces of plugins (MfIRenderer.h): IDs are 16-byte name structs passed
  by value to QueryInterface (slot 3); the IID constants are `const` objects
  with external linkage, so the code loads them from .rdata.
- STRICT: the exports take `HWND__ *`; MfRenderer.cpp defines STRICT before
  windows.h.
- MfRenderView: 0x34 is the view number (static counter from 1), 0x3c the z
  order; the views' mouse hooks go to a file-static MfHookManager.
- A `const char *` passed where a `const MfString &` is expected (implicit
  conversion) makes VC5 recompute the temporary's address (`lea`) for the
  call; an explicit `MfString("...")` passes the constructor's eax. Pick the
  form by that (MfSoundManager::LoadPlugin, GetPluginList).
- Two named locals constructed in the order the original constructs them,
  passed by reference, also give the recomputed addresses (SetDriver).
- MfDblList::Append/Prepend are inline (`Insert(p, LP_AFTER, pliLast)`):
  a call on a static list expands to a virtual Insert (MfSoundManager::
  AddCallback). The original StopSounds calls Append on a local list through
  the vtable, which these headers do not reproduce (TODO there).
- MfAiActorNode::GetTransform(local, home, global): the third reference
  receives the global transform.

## Wave 6

- More mflinalg inlines, proved by the expanded code and the missing imports:
  `MfVector3::lengthSquared` (`mX * mX + mY * mY + mZ * mZ`; still imported
  for the few callers where VC5 does not expand it, e.g. MakeYUp),
  `MfTransform3(const MfRotation3 &, const MfVector3 &)` (member copies),
  `MfRotation3(float, const MfVector3 &)` and
  `MfRotation3(const MfVector3 &, const MfVector3 &)` (both call setValue).
- `x = cond ? 1 : 0;` gives `mov eax, 1 / mov [x], eax` and `xor eax, eax /
  mov [x], eax` tails (MfKChain::setEndOffset, create); an if/else gives
  immediate stores.
- A dead parameter's stack slot is reused for locals. Where the original keeps
  a local in a parameter slot, the parameter is not used any more at that
  point: use the member instead of the parameter (`mActor` instead of `actor`
  in MfAtomicActivity::_InitBubbleModel), one variable instead of two for
  flags read one after the other (ReadFromStream).
- `if (p && p->flag && p != last) A; else if (p && p != last) B;` is the shape
  that repeats the null test in the else branch (updateExtraTracks).
- A pointer kept in a register across two inline accessor calls
  (`nodes->node(i)->colSpheres()` twice) differs from a named local `node`:
  try both when the registers differ.
- Implicit copy ctor / operator= of an exported class with only plain members
  and inline-copyable members (MfKChain, MfAtomicActivity): drop the explicit
  declarations, add `s_assign` and claim both. Classes whose implicit copy
  calls the base copy constructors out of line (MfGestureActivity: EH frame,
  calls to the MfRootObj/MfAiActivity copies) do not come out this way yet.
- Constructor body stores of zero can still come before the vtable stores
  (MfHandle ctor matched with the four zero stores in the body).
- An `if (a) x = f(); x = ...` where the original stores the same variable in
  both paths: reassign the variable and store once after the if
  (MfCameraNode::ZoomToTarget `ratio`).
- RuntimeLib's own unnamed classes declared so far: MfKChain::Link,
  MfBubbleModel (MfAtomicActivity.h), MfJointPath (MfActorPath.h, unit
  1002ff40-10030b90), MfSoundKeyframe/MfEventKeyframe (MfSkillKeyframes.h),
  MfColSphere.
- MfApiActor (MfApiActor.cpp): the MfAiActor factories take `this` adjusted
  to the MfAiActor subobject; `new X(this, g_world->GetClock(), ...)`,
  `if (!noSet) SetActivity(activity); return activity;`. A `MfSkill *skill`
  filled by CheckSkill lives in the slot of a parameter that is dead by then
  (often the skill name). Camera factories create MfPanCameraActivity,
  MfOrbitCameraActivity, MfSplineCameraActivity (RTTI names), not the
  exported MfCameraActivity constructor.
- A local initialised to 0 that the original keeps in the zero register
  (`xor ebp, ebp` at the top, used for compares) is declared late, right
  before its first use (`int cycles = 0;` in the loco/3d-path factories);
  declared at the top it gets a stack slot.
- `MfVector3 dir = r * MfVector3(0, 0, 1);` (temporary) and a named
  `MfVector3 forward(...)` differ in the EH state numbering.
- Functions of a secondary interface (MfAiSkill) that test the class: a local
  `MfManipSkill *manip = (MfManipSkill *)this;` makes VC5 compute the full
  object pointer once (`lea esi, [ecx - 4]`), as the original does.
- MfPath2::Point(i) is `if (mOpen) return mPoints[i]; return mPoints[i % 36];`
  (the original tests mOpen even for constant indices).
- MfSkill: the +8 field is MfProtoSkill::dataFormat (MfSkillFormat,
  'SlkS' = sampled); MfKeyList's destructor is inline (`DeleteKeyframes()`),
  slot 4 of its vtable.

## Wave 7

- MfLocoActivity_1..3 merged into `MfLocoActivity.cpp` on the canonical header
  (unit headers deleted). MfPath2 derives from `MfAiPath2` (MfAiPath2.h, slots
  from MfPoLibrary's MfPoPath2 and the vtable at 100b46c4); its destructor is
  not virtual (`delete mPath` calls ~MfPath2 directly).
- The activity classes all follow one scheme: implicit copy ctor/operator=
  (s_assign), `static MfStreamObject *CreateX() { return new X; }` plus
  `static MfStreamConstructorEntry constructorEntry(X::STREAM_TYPE, CreateX);`
  right before StreamType (the `_$E` pair: the 5-byte jmp is `_$E<n+1>`, the
  22-byte initializer `_$E<n>`; match.sh accepts any jmp stub for the 5-byte
  one, so pick the pair), and the MfAi adjustor thunks
  `?Type@X@@WBE@AEHXZ`/`?IsType@X@@WBE@AEHH@Z` (1000aa50-1000ab40 for the
  atomic activities) claimed without a body from the class's .cpp.
- Stream helpers of the first activity object, declared where used:
  WritePath/ReadPath (MfPath2, 10009690/10009b30), WritePath3/ReadPath3
  (10009e70/1000a0a0), WriteNodeReference/ReadNodeReference
  (1000a6e0/1000a760), WriteActorReference/ReadActorReference
  (1000a880/1000a8b0).
- Overrides of MfAi virtuals (Update, Reset) and of MfStreamObject virtuals
  (Read/WriteToStream) get `this` adjusted by 0x14/0x18: subtract when
  mapping offsets to members.
- Constructor stores: members in the init list are stored before member
  objects are constructed; body stores of 0 from the zero register come
  before the vptr stores, body stores of other constants after them in
  source order (Mf3dPathActivity: put `mPathPos = -1.0f; mFirstUpdate = 1;`
  first). Permute when only the store order differs.
- A dead `float` out-parameter lands in a dead parameter's stack slot by
  itself; never take the parameter's address for it (SetupTarget).
- Enumerator count matters: giving `MfActivityStatus` two named enumerators
  in enums.h changes MfJointPath::Key in MfAtomicActivity.cpp; the enum stays
  a placeholder, sources return `(MfActivityStatus)0/1` (0 running, 1 done).
- An inline copy ctor in MfActor.h (protected, copies nothing) changes
  register choices in other units; it is defined out of line in MfActor.cpp.
- Loops `for (n = node; n != skeleton && n != skeletonTraversal()->node(1);
  n = n->parent()) depth++;` count the nodes up to the hip (Orient, Compliant,
  Hold). The normalize in MfOrientActivity::setup sums `y*y + z*z + x*x`.
- MfJLHinge (GetValue at slot 0x28) is declared in MfJLimit.h;
  MfColResultNode (SteamboatApp PDB) in MfColResults.h.
- MfConfigBuffers was the planner's grasp path: renamed MfGCPath (MfGCPath.h),
  its m14 is the grasp transform MfManipActivity copies. MfMTPath (MfMsgData.h)
  holds two transform arrays.
- Still open in the activities: MfManipActivity main ctor/ReadFromStream/Update
  (they use the unnamed path sampler unit 10023ae0-1002ec80), MfGestureActivity
  main ctor, the Hold/Compliant implicit copy ctors (register choice in the
  expanded MfKChain copy), MfLocoActivity/Mf3dPath/Gesture Update and
  _CheckBubble (the imported mflinalg calls of the inline budget).
- The vtable/RTTI map of the image (complete object locators -> class per
  function) names most unnamed functions; regenerate with a small script over
  .rdata (signature 0, type descriptor at +12) when choosing a unit.

## Wave 8

- Classes named from the RTTI map (complete object locators in .rdata,
  signature 0, type descriptor at +12, vtable after the locator pointer):
  MfKeyboard (MfAiKeyboard + MfRootObj, 10090090-10090a70), MfMouse
  (10090a70-10091030, the former "MfPickManager" of MfLiveActor.cpp; gMouse
  at 100f678c), MfProximityManager (10091030-10092230). 1007e000-1008ec00 is
  the state machine code (MfFiniteStateMachine, MfFSMManager, MfFSMNote and
  its ~30 MfState*/MfTrans* subclasses, 40-slot vtables, no names beyond
  the RTTI and a few log strings); 1008ee00-10090090 MfViewEventHandler;
  10023ae0-1002ec80 an unnamed manipulation planner without vtable.
- MfAiKeyboard/MfAiMouse slots: MfAiKeyboard.h/MfAiMouse.h (names from the
  behaviour and MfPoLibrary's calls). MfMouse's destructor is MfMouse's own
  virtual (slot 0x1c); MfAiMouse has none (its implicit ctor still stores
  its vtable).
- Message tables: a class deriving only from MfRootObj has 4-byte member
  pointers (the fill loop is a rep stosd); with MfRootObj as a second base
  they are 8 bytes with an adjustment of 4.
- Log calls: `MfLogger::Log(1, MfString("Keyboard"), NULL_STRING, "text")` —
  the implicitly converted arguments are passed by recomputed address (lea).
- MfSkill's vtable: apply at 0x50, then the sample overloads in reverse
  declaration order: sample(.., const MfTransform3 &) 0x54, sample(.., int)
  0x58, sample(.., int *) 0x5c. MfLocoActivity::Update and the Gesture ctor
  call sample(time, nodes, int).
- MfProtoActor::Name() and ProximityDefs() are inline in the header (the
  original reads +0x0c and +0x5c directly; Name is also imported, which no
  matched RuntimeLib code calls).

## Wave 9 (state machines, 1007dda0-1008c3b0)

- One file per RTTI class: MfFSMMutable, MfStateProperty (+List),
  MfState (+MfStartProperty/MfStateBeginHistory), MfStateChoicePoint,
  MfStateCompiler, MfStateEntryAction/ExitAction/FunctionDef/InitVar
  (declared in MfStateAction.h), MfStateEntryPort, MfStateExitPort,
  MfStateTransition, MfTrans*, MfFSMNote, MfStateDisplayPos. Part types
  (mType): 0x64 property, 0x65 state, 0x66 entry action, 0x67 exit action,
  0x68 variables, 0x69 transition, 0x6a display pos, 0x6b func_def, 0x6c
  start_state, 0x6d exit port, 0x6e entry port, 0x6f choice point, 0x70
  init_var, 0x71 begin_hist, 0x72 note, 0x73 note contents, 0x74 camera
  settings, 0x75 playback; transition parts 1 triggered-by, 2 action, 3 dest
  state, 4 visual connector, 5 trans type, 6 init, 7 exit-by, 8 enter-by,
  9 choice point.
- Base slots (MfFSMMutable): Load(.pobj) 4, Link 5, Write 6, Compile 7,
  Unload 8, CountNamed 9, CanDelete 10, CanConnectTo 11, SetPath 13, Find 16,
  CollectAll 18, GetScript/SetScript 19/20, Search 21; MfStateProperty adds
  FormatForExecution 22, SetParent 23, GetOwner 24, GetVariableStack 25,
  AddPart 37, RemovePart 39.
- Write(ostream&, int &indent, int mode): `MfString tabs; for (..) tabs <<
  '\t'; indent++; os << tabs; os << "kw " << mName << endl; ...` — separate
  statements wherever the original passes os again. `os << "!*!}" << endl`
  (EntryAction) vs `os << "!*!}"; os << endl;` (ExitAction, InitVar).
- Clone: `new X(mName, new MfStatePropertyList(0)); copy->SetProperties(
  mProperties); copy->SetFullPath(mPath); copy->mMachine = mMachine;`.
- Loops over member lists are virtual calls: `MfStatePropertyList *list =
  &mStates; for (p = list->GetFirst(pli); ...)` or `(&mStates)->GetFirst`.
- Members stored before the vptr → initializer list; a by-value MfString
  parameter is destroyed at the end of the ctor, a `const MfString &` is not
  (ChoicePoint, EntryPort take references, ExitPort a value).
- Switch on mType: case bodies are laid out in source order; cases that add
  to different lists (`if (!mX.Contains(p)) mX.AddProperty(p)`) are tail
  merged by VC5 into one block using a callee-saved register for the list.
- `if (!x) A; else B;` keeps A after B in the layout (MfState::Enter's
  fallback substate).
- A call result kept in a local and compared (`int error = f(); if (!error)`)
  compiles to `cmp eax, ebx` (zero register) where `if (!f())` gives `test`.
- Tail recursion becomes a loop (MfState::Activate: SetActiveChild on the
  parent, then parent->Activate()).
- Script runs: `MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->
  mVmEnv; outer = env->mVarStack; env->mVarStack = <state's stack>;
  result = vm->ExecuteFunction(fn, 0); if (result) result->Release();
  env->mVarStack = outer;` (headers MfScThread.h, MfScVmEnv.h,
  MfScVirtualMachine.h, MfScVariableStack.h).
- MfScHandleList (dllimport, inline ctor/First/Next): a stack object stores
  the imported vtable, `new MfScHandleList` the local copy. Next() goes
  through a reference to mCurrentPos.
- `dynamic_cast<MfScDataMethod *>(h->GetData())`: VC5 drops the null check;
  the original's `h ? dynamic_cast<...>(h->GetData()) : 0` (FunctionDef).
- The behaviour parser/scanner (yacc/lex, 1009c940-) is called through
  plain functions; the compiler object is the global gStateCompiler and it
  creates g_msgTypes (MfMsgTypeTable).
- Identical small virtuals of sibling classes are folded (Load/Link/Compile/
  Unload loops of MfState, ChoicePoint, EntryPort, ExitPort; Unload of the
  actions; HasScript); define them in each class, claim the kept copy.

### Known unmatched shapes

- Folded callees: MfStatePropertyList::Contains and MfTransPropList::Contains
  carry their own claims at the folded address 1008a920 (several claims may
  name one address); calls to them are then accepted.
- Functions calling GetThread and ExecuteFunction twice each: the original
  puts ExecuteFunction in the first callee-saved register (MfStateEntryPort::
  Enter, MfStateExitPort::Exit); ours GetThread.
- eax/edx swapped for the compiler interface calls in the actions' Compile.
- MfStateTransition::FormatForExecution: our second loop loads the part's
  vtable before storing mTransition; the original stores, loads mMachine,
  then the vtable.
- Compile of MfTransTriggeredBy: the original keeps a separate
  `compiler->Release()` on each path, VC5 tail-merges ours.

### More patterns (round 5)

- A struct member initialised from the constructor list needs a constructor
  of the struct (MfTransTriggeredBy::mEvent(-1, -1) with
  MfMsgTypeInfo(int, int)); assigning a returned struct
  (`mEvent = g_msgTypes->Lookup(mName)`) copies through the returned pointer.
- Search(): `if (s.ContainsSubString(text, 0) <= -1) { not found } else {
  found }` keeps the not-found branch first (`cmp eax, -1; jg`).
- Two statements where the original evaluates a call fully before loading
  the target object: `MfFSMMutable *copy = p->Clone(); list->Append(copy);`.
- MfFSMManager: MfRootObj message table (4-byte member pointers, rep stosd);
  MSG_RUN_BEHAVIOR (1) runs the queued events/timers, MSG_START_BEHAVIOR
  (59) starts the enabled machines. The empty hooks are inline in the header
  so the vtable's folded slots are defined. Adding enumerators to MfMsg.h
  broke the inline expansion of MfJointPath::Key in MfAtomicActivity.cpp,
  so the two IDs are local to MfFSMManager.cpp.
- The `_$E<n>` names of a file's static initialisers are numbered after
  everything the file includes: every added #include renumbers them. Read
  the current names from the object (`match.sh ... --sym '_$E1'` lists the
  defined symbols) and map them by order: the entry point is the highest
  number, the constructor the lowest, the atexit registration the second
  highest, the destructor the second lowest.


### More patterns (round 6)

- Loops that stop at the first hit: `break` and `return` inside the loop
  compile differently. Where the original jumps out of the loop to code
  placed after it (one shared epilogue, the import pointers loaded once),
  write `break` (MfFSMManager::ReinitBehavior, UnloadBehavior); where the
  original returns from inside, `return` (LinkIfShared).
- Nested `if` blocks give one shared epilogue; early `return`s each get
  their own unwinding (MfFiniteStateMachine::LoadCompiled's handles).
- `name + MfString(" while ") + mActorName + ...`: MfString has only
  operator+(const MfString &) (and conversions to char *), so the literals
  need explicit MfString temporaries; VC5 builds them right to left (the
  last literal first), as the original does.
- CreateFile on an MfString picks the conversion explicitly:
  `(const char *)path` (??BMfString@@QBEPBDXZ) where the original calls that
  import, `(char *)path` for ifstream::open (??BMfString@@QBEPADXZ).
- StateScriptFlags::iFlags is private in the PDB but RuntimeLib tests it
  directly (`test byte ptr [iFlags], 1`): MfFSMManager and
  MfFiniteStateMachine are friends of StateScriptFlags.
- In MfFiniteStateMachine's Compile and LoadCompiled the original expands
  MfString::Truncate inline: CopyOnWrite(0) with a call of the imported
  MfStringRef copy constructor (every other unit inlines that copy, as our
  header does), then the imported MfStringRef::Truncate. Making Truncate
  inline in MfString.h does not match them either and breaks the inline
  expansion of MfJointPath::Key in MfAtomicActivity.cpp (10018990), so
  MfString.h keeps Truncate out of line and both stay TODO.
- MfScPiccoloObjectFile is used on the stack (0x38 bytes); RuntimeLib only
  reads the int at 0x1c (mStatus): 0 means a file opened for reading is
  usable, nonzero a file opened for writing.
- Arguments the function no longer needs can hold locals: the original
  puts the "not terminated" message of FireTransition in the transition
  argument's stack slot (VC5 does this by itself once the argument lives in
  a register).
- Shared headers and inline expansion: a struct with an MfString member
  (so an implicit inline destructor) added to MfHookManager.h changes the
  code of MfViewEventHandler 1008f740; MfString::Truncate made inline breaks
  MfJointPath::Key (10018990). Declarations only one file needs stay in that
  file (MfBehaviorStartHookData in MfFiniteStateMachine.cpp), and every
  shared-header change is followed by the whole-directory check.

## Wave 9 (outside the state machine block)

- New units: MfViewEventHandler.cpp (1008ee00-10090090, RTTI name; the mouse
  hooks, picking, the view option menus), MfManipPlanner.cpp (the unnamed
  path sampler 10023ae0-1002ec80: only the ctor/dtor so far; the methods
  MfManipActivity calls are declared in MfManipPlanner.h), MfRay.h (MfRay::Cast
  at 100921a0 lives in MfProximityManager.cpp), MfMetaColModel.h (exported,
  1005d6f0-1005e1f0, written in MfColNode.cpp with MfColReport). MfColResultRay is in MfColResults.h.
- Hooks registered on a local `MfHookManager hooks;` (the hook lists are
  static) are AddHook calls without a virtual call.
- A virtual call on a local list (Append through the vtable): call it through
  a pointer variable, `MfDblList *list = &local; list->Append(p);`, or use
  `MfLiveActorList::Add` (casts this to MfDblList*). `(&local)->Append` is
  devirtualized and expanded.
- `if (!(~x & 1))` compiles to `not eax / test bl, al`, the form the original
  uses for the check-style tests (Pick, MakeStaticEnv); `(x & 1)` and
  `(x & 1) == 1` do not.
- A member pointer to an MI class (`void (MfLiveActor::*)(int)`) is 8 bytes
  only when MfLiveActor is complete at the declaration.
- Two stores through a member pointer (`mStaticEnv->a = ..; mStaticEnv->b =
  ..`) reload the member between them; the original's single load comes from
  an inline setter (`OwnColModel`).
- MfActor::InstanceName/Name are inline in the original but MfActor.h keeps
  them out of line (making them inline renumbers `_$E` claims in many units);
  ShowCameraOptions spells the expansion out.
- A named local initialised from a by-value return (`MfTransform3 t = a * b;`)
  keeps its EH state to the end of the block; the temporary form does not
  (MfManipActivity::Update, HandleManipGC).
- Open shapes: the sprite test `proto->GeomInfo() && ...->mTriangles &&
  ...->mFacesCamera == 1` reads the style from memory three times in the
  original (VC5 keeps it in a register here); hook results compared `cmp
  eax, ebx` against the zero register (InRange); a local placed in a live
  parameter's stack slot (ReorderActorUpdate).
- Header edits by other workers flip x87 choices (Pick's fimul) back and forth;
  re-run check.sh on the directory before reporting.

## Round 2 (MfColNode, MfManipPlanner, MfWorld)

- MfColNode.cpp holds the whole unit 1005a5c0-1005f2e0: MfColNode, MfColReport
  (1005d170-1005d630), the statics of MfColNode and MfMetaColModel, then
  MfMetaColModel and MfRayColInterface (not written). MfColReport's entry array
  uses `new MfColReportEntry[n]`; its compiler-generated constructor is claimed
  by comment (`// 0x1005d1f0 ??0MfColReportEntry@@QAE@XZ`), and the empty
  element destructor is folded with ??1MfManipPlanCache (1007dd30).
- VC5 places a base with a vptr before a base without one: MfColNode is
  MfAiColNode (vptr at 0) then MfNode at +4, whatever the declaration order.
  A sphere-tree node of type 2 is `static_cast<MfColNode *>(node)` (node - 4).
- The sphere-tree walks (RayIntersect, SphereIntersect, BallIntersect) use the
  static MfSearchData stack; `while (sp != 0) { ... }` puts the "return true"
  block after the epilogue as the original does, `do { } while (sp != 0)`
  before the "return false". Distances are `float dx = c.x() - p.x(); ...;
  MfMath::sqrt(dx * dx + dy * dy + dz * dz)` (spills x and y like the original).
- ~MfLock is inline and empty (MfLock.h): ~MfColNode, ~MfSpace and the exit
  entries of the static locks do not call it, and the unwind funclets, which
  cannot expand an inline function, jump to the import. The same holds for
  ~MfTypedValue (inline, sets the imported vtable) and MfColor (no declared
  destructor: the implicit one leaves the vptr alone, ~MfSpace's fog).
- Header flips seen this round (all kept out by moving the declarations):
  MfSphereNode in MfNode.h flips MfLocoSkill::CycleLength (now MfSphereNode.h);
  MfColResultSphere/MfDistResult in MfColResults.h flip MfViewEventHandler::Pick
  (now MfColDistResults.h); an inline constructor for MfConfigRecord in
  MfLiveActor.h flips Pick and moving the struct out flips the FSM's 0x1007e790
  (the constructor is MfEmptyConfigRecord, derived, in MfManipPlanner.cpp);
  more than one new member declaration in MfMsgDispatcher.h flips Pick; moving
  MfTimedMsgQueue into its own header (the dispatcher only holds a pointer)
  made room for Start/Resume/Pause/FlushPending. The flip is not a simple
  declaration count; making ~MfLock inline flipped MfJointPath::Key in
  MfAtomicActivity.cpp, which the include order (MfSpace.h, MfWorld.h first)
  restored.
- MfManipPlanner (unnamed unit): helpers on MfConfigRecord (Clear/Get/Match),
  the planner's collision environment (MakeEnvironment/ClearEnvironment around
  MfSpace::MakeEnvMetaModel), the hand configurations (MfHandConfigs) and the
  checks of a request. MfGCPath has an MfActor *mTarget at 0x40 (PlaceTarget),
  which makes MfConfigRecord's tail just m90.
- Copying a node's transform into a struct member: through a local node
  pointer (`MfSkeletonNode *node = ...; c->mTransform = node->mGlobalTransform;`)
  the copy uses the node's offsets; `...->node(i)->mGlobalTransform` forms the
  address with `add eax, 0x44` first.
- Compare operands keep their source order: `if (except != hand ...)` gives
  `cmp except, hand`.
- Reading an imported static into a local before testing an argument
  (`MfAiPdbDebugger *d = MfScDebugger::pdbgTheDebugger; if (!a && d)`) loads it
  first like the original (GoAuthor). pdbgTheDebugger is private (`@@0`), so
  RuntimeLib/MfScDebugger.h makes MfWorld a friend.
- MfDblList's overloaded virtuals are in reverse order: Clear() is slot 0x54.

## Round 3 (MfRayColInterface, MfSpace constructor/destructor, MfWorld)

- MfRayColInterface (1005e1f0-1005e660, in MfColNode.cpp): the slots after
  Release are SetNode, SetTransforms, UpdateTransforms, Enabled (folded
  `return 1`), SetTransform, UpdateTransform, Cast (1005e660, not written).
  RejectBackFace (1005e1f0) is the MF_HOOK_COL_QUERY callback Cast installs.
- MfSpace's constructor matches with the plain init list and a body that sets
  vector members through `v = MfVector3(x, y, z)` (component stores scheduled
  like the original; separate `.x() = ...` stores are not).
- ~MfSpace: the named views in m30 are MfSpaceView {MfString, MfTransform3,
  MfCamera}; the unwind funclets show the MfTransform3 member even though its
  destructor is empty. Iterating m30 through `(&m30)->` (not a local pointer)
  gives the original's EH slot.
- The sprite test (`proto->GeomInfo() && ...->mTriangles && ...->mFacesCamera
  == 1`) is still open in four functions; the if/return and local-variable
  forms of GeomInfo do not change the shared load.

## Round 4 (MfSpace streams, MfWorld::GoLive)

- MfSpace's stream functions match: WriteToStream, ReadFromStream, Load,
  _WriteActorInfo, _WriteActorChildList, plus CreateFloorGraph and
  UpdateMovingActor. Helpers: MfRoadMapBuilder (Build, AddFloor,
  RemoveFloor), MfRoadMap {mNodes, mUnit}, the static `sRootName`
  ("__DLIN_Root", between NextAvailableActor and _ReadActorInfo).
- _WriteActorInfo writes an int that is never set: `int reserved; stream <<
  reserved;` gives the original's read of a dead stack slot.
- MfStream has `operator<<(bool)`/`operator>>(bool &)` (the fog's mEnabled);
  the existing `stream << (x != 0)` writes became `(int)(x != 0)`, which is what
  the original wrote.
- MfCamera is 0x34 bytes (new MfSpaceView); MfCameraNode's field_130 and
  unknown_134 were the camera's tail.
- `if (!s.userFormat) s >> s.userFormat; if (s.userFormat < 100) ...` (two
  ifs, not else-if) gives the memory compare of ReadFromStream.
- VC5 does not always expand MfString::CopyOnWrite inside ConvertToUpper: the
  original calls the import in AddActorSync, RemoveActorSync and the first call
  of GetActorList, and expands it in Load and the second call of GetActorList.
  Here it is always expanded (open: AddActorSync, GetActorList).
- The joysticks are `MfJoystick *gJoysticks[2]` (100f5a84, defined in
  MfRuntime.cpp; class without vtable, unit 1007c870-1007dc50); the keyboard
  is the global gKeyboard (100f6590).

## Round 5 (joysticks)

- MfJoystick.cpp (1007c870-1007dc50) plus MfRuntime::InitializeJoysticks,
  PollJoysticks and the EnumDevices callback (10004060). The VC5 headers have
  no DirectInput: MfJoystick.h declares the parts used (DIPROPDWORD,
  DIPROPRANGE, DIJOYSTATE, DIDEVCAPS, DIDEVICEINSTANCEA, IDirectInputA,
  IDirectInputDevice2 with its slots, c_dfDIJoystick and DirectInputCreateA,
  which are statically linked into RuntimeLib).
- HWND is `void *` in this build (no STRICT): ?SetWindow@MfJoystick@@QAEXPAX@Z.
- MfJoystick::Initialize takes (instance, directInput): the callback passes
  its two arguments straight through.
- RemoveRoutes (TODO) converts each route and then clears it without a
  compare; a test whose both outcomes clear is not enough to reproduce it.
- In the MfSpace functions the MfString::CopyOnWrite expansion is partial in
  the original: Save expands CopyOnWrite but calls the imported
  MfStringRef::CopyOnWrite; NextAvailableActor's first call keeps the imported
  MfStringRef copy constructor, its second expands it (MakeCopy). Matching
  NextAvailableActor came out right as written; the budget differences only
  show in the larger functions.


## Round 6 (small units first)

- New units: MfSoundKeyframe.cpp, MfTextureKeyframe.cpp, MfEventKeyframe.cpp
  (the skill's sound/texture/event keyframes), MfPluginActivity.cpp
  (+ MfAiActivitySpec.h, slots Start/Stop/Update invented), MfPath2.cpp (whole
  unit 10049bf0-1004a890), MfPath3.cpp (only SetKeys, 1004c1d0),
  MfAttachActivity.cpp (all but the main ctor and _CheckBubble),
  MfBitArray.cpp (MfBitArray 'BitA' and MfIntArray), MfStateLexer.cpp (the
  flex 2.4 buffer functions of the behaviour-file scanner, 1009bb00-1009cc70).
- `mSound->SetChannelVolume(0, mMute ? -100.0f : mVolume)`: a `?:` of floats as
  an argument gives the original's duplicated call tails.
- MfVector2 (RuntimeLib/MfVector2.h) has a user-defined inline copy ctor and
  operator= (both are exported by mflinalg, and the code calls the import
  where the inline budget runs out). With them a copy followed by a float
  compare goes through the FPU (fld/fst), as in MfPath2::Update; the whole
  directory stayed matching and four old TODOs matched with it
  (ignoreTriangleCollisionCheck, MfJoystick::Initialize, the MfHoldActivity
  copy ctor, MfWorld::Release).
- A ring index is `i - i / RING_SIZE * RING_SIZE`, not `i % RING_SIZE`: VC5
  turns the first into the multiply by 0x38e38e39 the original has, the second
  into idiv. MfPath2::Point is `if (!mOpen) { if (i < RING_SIZE) return
  mPoints[i]; return mPoints[mod]; } return mPoints[i];`. In PointAt the
  result must go to a new variable (`int k = i; if (i >= RING_SIZE) k = ...`),
  assigning it back to i gives a negated-divisor form. (HandleLocoPlan's
  LastPoint still differs elsewhere.)
- if/else that call the same function with different arguments are merged
  into one call (MfPluginActivity::WriteToStream: `if (enableNetwork)
  WriteActorReference(stream, mActor); else WriteActorReference(stream, 0);`).
- IsType of MfPluginActivity skips MfAtomicActivity (`return
  MfActivity::IsType(type)`); its GetSpec override is folded with
  MfLiveActor::IsLive. MfAttachActivity's TYPE is 'AttA' (not 'Atta') and it
  overrides HasMotion with `return 1` (folded with Mf3dPathActivity's).
- Member operators evaluate their argument before the object: in
  `~a * b * ~c * ~d` the original's call order (~c first, ~d late) shows a
  statement split, `MfTransform3 start = ~parent * mTarget * ~handle; mStart =
  start * ~sample;` (MfAttachActivity ctor and ReadFromStream).
- Keeping a member in a local (`MfManipSkill *skill = (MfManipSkill *)mSkill;`)
  gives the original's register choice when the member is used for several
  calls; a named local transform instead of a temporary passed to
  setTransform gives the original's frame and keeps the time parameter in a
  register (MfAttachActivity::Update stores the skill time in the parameter's
  slot).
- MfAiPath2::GetTransform returns MfTransform3 by value (ret 8 with a hidden
  result pointer).
- MfPath2 fields: mStart (MfTransform2) 0x04, mFlags/mRadius/mAngle 0x10-0x18,
  mArc 0x24 (SetArc); MfPath3: mTotalTime 0x04, mKeys 0x1c, mKind 0x28 (2 key
  array, 4 spline; was mNumKeys).
- Left alone: 10069000-1006cfff (MfPo* classes claimed from MfPoLibrary as
  "runtimelib.dll").

## Round 7

- MfCameraActivity.cpp rewritten on the real header (MfCameraActivity.h is
  the exported class; MfCameraActivity_decl.h removed): the base class and
  MfPan/MfOrbit/MfSplineCameraActivity (1001ab90-1001eb10). Type, IsType,
  TrackOrientation and UseNodeOrientation are inline (claimed in
  MfApiActor.cpp). Constructors put every member in the init list (the
  original stores them all before the vptrs). Open: the Update functions,
  GetTargetCenter/Direction, UpdateState, SetCameraTransform, ClearPath,
  OcclusionDistance, TestViewDirection, SetPath, SetBoundary (TODO).
- `damping = (1 - s) * (k * k) / (mass * 4)`: the parenthesized square gives
  the original's operation order; `mass` is a file-static const float (VC5
  loads it from .rdata, next to the exported defaultFocus/defaultStiffness).
- `MfVector3 d = a - b; d.lengthSquared()` on a named local compiles to
  (y*y + z*z) + x*x, the original's order (MfJointPath, SetBoundary's length).
- VC5 turns a zeroing loop over an array into `rep stosd` (MfMsgQueue::Grow);
  memset gives the extra byte loop.
- Guarded atexit destructors (a flag byte tested and set before the
  destructor runs) are VC5's for static data members of classes: all static
  members of a file share one flag byte, a bit each (text-template unit
  10020460: one class with a static array of 82 entries, another with a
  static MfString[300] and four MfStringLists; MfReportError at 10020780 uses
  them). Still open.
- Units: MfJointPath.cpp, MfActorPath.cpp (all but the subtree Sample at
  100132d0), MfMsgTypeTable.cpp (all but Init, 12 KB of registrations),
  MfMsgQueue.cpp (Compact TODO: the original calls the local list's Append
  through the vtable), MfMsgDispatcher.cpp (all but the constructor and the
  thread functions; destructor TODO for register choice).
- MfMsgDispatcher.h now declares the whole dispatcher; MfViewEventHandler::Pick
  flipped with it (any added declaration in its unit changes the local layout
  and the first multiplications) and is a TODO now.
- `new MfMsgTypeName(*info, name)`: an entry whose constructor assigns in its
  body after `mInfo(-1, -1)` gives the original's stores and EH funclet.
- The planner grid MfBfp (10054010-10055d60) contains MfWorkspace (an
  MfIntArray), an MfBitArray, a point list (10055cd0-10055d50) and the
  direction table set up by 10055c50; not written.

## Round 8 (MfColNode)

- MfColNode.cpp now has every function of 1005abb0-1005ec90 (AddColNode is a
  register-only TODO). 1005a5c0-1005ab90 and 1005ec90-1005f2e0 belong to other
  units (they call 10059bf0 / use the static at 100f587c).
- The two-tree walks (CheckTriangleIntersection, MinDistance) descend the
  second tree first in the source: `if ((n1->mRadius <= n2->mRadius &&
  n2->mType != 0) || n1->mType == 0) { second } else { first }`. Testing the
  leaf types inline (no `int type1 = n1->mType`) gives the original's spill.
  Declaring the distance temporaries (dx, dy, dz, dist2) and the gaps at
  function scope gives the original's stack slots.
- MinDistance has no counter after the loop: `while (sp != 0)` places the
  `return false` block after the epilogue; CheckTriangleIntersection needs
  sp after a do-while (`if (sp != 0) return -1.0f`, the hook's break).
- Bitfield words: the type test of a sphere-tree node compiles to `test byte
  ptr [node+0xc], 3` only when the word is not CSE'd with the index; in
  RayIntersect that came from hoisting `float t` to function scope.
  RayIntersect also shares one dist2 between both children (`leftDist2 =
  dist2`) and computes `direction.dot(diff)`.
- `MfVector3 d; d = direction;` (default construction, then assignment) gives
  the mixed integer/FPU copy of CheckCollision(ray); `MfVector3 d = direction`
  keeps the whole copy in the FPU.
- A compare of a temporary's result (`(a - b).length() >= len`) makes VC5
  materialize a bool before the temporary's EH state changes; the original
  names the float first (`float reached = (a - b).length(); if (reached >=
  len)`). The MfString for MfInterfaceRegistry::GetInterface is a named local
  in its own block; a `for (GetFirst; p; GetNext)` loop resets the EH state
  before GetNext (MfRayColInterface::Cast).
- MF_HOOK_COL_QUERY data: 0x10 is the MfColResultNode of a node/node hit (0 for
  sphere and ray hits), 0x14 the sphere or ray result.
- MfColReport.h declares the slots of MfAiColReport and MfAiColInterface
  (Cast(MfAiColNode *, MfAiColReport *)), and what Cast writes into an
  MfColReportEntry. MfColModel.h declares MfOrientedBox::CheckIntersection.

## Round 8 (MfApiActor)

- The planner requests (RequestLocoPlan, Grasp/Place/Touch/Sit) check their
  skills with one `MfSkill *skill` (address taken by CheckSkill) and copy
  each result into its own local right after the check; the last one too
  (`MfSkill *endSkill = skill;`), otherwise the frame is 4 bytes short.
- An unused 3-float local constructed before the `mOwner` test is a
  `MfTransform2` (RequestLocoPlan(target ...) passes that zero goal to the
  planner and gives the helper 10057d30 a second one).
- Three-way choices that end in the same assignment are an else-if chain with
  the assignment in every branch; VC5 merges the copies into one tail
  (RequestPlacePlan's up axis, `dir = ...`).
- `MfTransform3 placed = target * xform; MfTransform3 offset = placed *
  ~sampled;`: two statements where the original evaluates the left product
  before the inverse (one expression evaluates the argument first)
  (SampleAttachSkill). A vector used once as an operand is a temporary, not
  a named local (FacePoint's `MfVector3(0, 0, 1)`).
- A value read through the inline `MfTransform2::x()` (returns by value) is
  loaded before the other operand: `local.z() - path->Point(0).x()` gives
  the original's `fld; fsubr` (CreateLocoActivity).
- `if (!a) a = b; if (!a) return 0;` (two ifs) gives `mov ebx, edi; cmp ebx`
  where the nested form compares first (GoalFloor).
- A handle looked up into a separate `MfSkeletonNode *node`, type-checked and
  then assigned (`handle = (MfHandle *)node;`) keeps the node in a register
  and the handle in a dead parameter's slot (RequestTouchPlan).
- MfActor::allTraversal()/skeletonTraversal() are not expanded in these large
  functions (VC5 calls them); the original reads mAllTraversal directly.
- MfRoadMapBuilder::FindPath (1005fc90, declared in MfRoadMap.h) returns a
  new list of MfPathPoints or 0; GoalFloor deletes the points and the list.
- MfManipPlanCache m24/m28 hold floats (the grasp/touch request's float
  arguments, Sit stores 0.0001f); the header types them int, so the sources
  write `*(float *)&mManipPlan.m24`.

## Round 8 (MfSpace_2)

- New in MfSpace_2.cpp: OldReadFromStream (matches) with the skipped entry
  struct MfOldSpaceEntry (its generated destructor 100945b0 is what the unwind
  funclet calls), ??1MfSpaceHookData (10094bb0, from Save), and TODO bodies
  for Save, _ReadActorInfo, _ReadActorChildList, RemoveActorSync,
  CheckCollision and ReportCollisionEvents.
- An unwind funclet that reaches an enregistered pointer (the `delete view`
  of the m30 loop) reads it from a home slot whose offset depends on how many
  locals are declared: OldReadFromStream matched only with one counter per
  loop (`for (int i ...)`, `int j`, `int k`) instead of one shared `n`.
- VC5 reuses the stack slots of parameters it keeps in registers for later
  locals (an MfString in stream's slot, the loop counter in parent's slot);
  a value read from such a slot before it is written is an uninitialised
  local (_ReadActorInfo's `error`).
- Slot sharing of block locals: a variable declared in an `if` block shares
  its slot with locals of earlier disjoint blocks only if the earlier ones
  were block-scoped too (OldReadFromStream's `count` moved into its block).
- The MfSpace stream/actor functions are dominated by VC5's inline budget:
  the original calls the imported MfString::CopyOnWrite(int), MfStringRef(int),
  MfRotation3 ctor/operator= and the COMDAT allTraversal() where this tree
  expands them. No source form found; probably the original MfSpace.cpp
  (one large unit) had used up more of the budget.
- MfAiColReport's GetEntry overloads: the image's vtable (100b50e4) has
  GetEntry(int) at slot 5 and GetEntry(int, MfColReportEntry &) at 6, so with
  the reversed-overload rule GetEntry(int, entry &) is declared first (done in
  MfColReport.h; ReportCollisionEvents matches with it).
- The `$E` names of sRootName's static initializer shift with the code before
  it (now _$E77/_$E74/_$E76/_$E75); re-check them after adding functions above.
- MfContactRecord (CheckCollision) and MfTriangle are defined in
  MfSpace_2.cpp; MfFilterItem as in MfPoLibrary (types 3-5 name an actor).
- In CheckCollision the second node loop matches the sprite test while the
  first (same source, nested in the list loop) does not: the register choice
  for the constant 1 depends on the loop nesting, not on the test's form.

## MfManipPlanner (unit 10023ae0-1002ec80, every function has a body)

- Entry points: CheckReachable (10024120, MfCheckReachableReqData), PlanGrasp
  (10025d10, MfManipGCReqData), PlanMove (10026730, MfManipMoveRequestData;
  argument order actor, skill, target, release), SampleGrasp/SampleMove (called
  by MfManipActivity). The planners work on stack copies (`MfLiveActor
  copy(3)` + MakeCopy) and keep the result in the actor's MfConfigRecord.
  Members: mMoved (0x14, per node), mGrasps (0x18, per hand), mTargetColNode
  (0x48), mActor (0x68). FindHoldNode's call sites pass (release, actor,
  target, &copy); PrepareGrasp's int arguments are the target copy and handle.
- Inline budget: in the planner functions the original calls the imported
  `??0MfVector3`, `??0MfRotation3`, `lengthSquared` and `operator=` where VC5
  expands them here. Inline accessors spend the budget: writing
  `actor->skeletonTraversal()->numNodes()/node(i)`, `skeleton()->
  globalTransform()`, `ManipType()`, `AnchorList()` instead of the members
  turned SetMidConfig, GrowChain and RestoreHands into matches and moved
  SampleMove from 72% to 83%. Try the accessor forms first in large functions.
  A `while (!HandBelowParent(...) && node != node(1))` loop (not if + do-while)
  matched RestoreHands.
- Unused default-constructed MfTransform3 locals show up only as EH states
  (InterpolateConfigs, SetMidConfig, SampleGrasp/Move): keep them as named
  unused locals. Dead products the original computes with imported operators
  are kept too (MoveHand, MoveHip).
- `drop = drop + (a - b)` places the fadd after the loop counter, `drop += a -
  b` before it. Repeated `x = n / 3;` per branch (not chained) gives the
  original's register copy.
- `if (release) f(a, target, x); else f(a, 0, x);` compiles to two push
  sequences feeding one call; a `?:` argument gives neg/sbb.
- Identical cleanup at the end of each branch of if/else-if/else followed by
  one `return 1` is the original's shape; separate returns are tail-merged.
- A by-value MfVector3 copied into the return slot through the FPU:
  `v = v / f; return MfVector3(v.mX, v.mY, v.mZ);` (AnchorCenter). Computing
  the index into a local first (`int index = ...->nodeIndex(n); n->
  setTransform(config[index]);`) matched SetChildConfigs.
- `MfColResultNode result` declared before a loop (not inside) gave the
  original frame (InterpolateConfigs); `chain.setThreshold(0.0f)` right after
  MfKChain::create.
- Joint limits: MfJLimit slot 0x18 GetType, 0x1c FixTransform; hinge (1) ->
  GetValue(from, to, mode) and `rotation().interpolate(from, to, 0.5f, mode ?
  -1 : 0)`; Euler (2) -> FixTransform then MfJLEuler::CenterTransform (declared
  locally in MfManipPlanner.cpp as a dllimport class).
- Handle styles (`MfHandle::GetStyle()`): 1 the oriented cylinder (identity
  grasp), 2/3 discs (`GetRadius() - skill->GetHandle()->GetRadius()`), the
  cylinders go through OrientGrasp. MfAiActor slot 0x20c is GetAnchorNodeStop.
  A float constant 0.11111111f is `* (1.0f / 9.0f)` in the source.
- MfManipHand is larger than MfManipSkill.h declares: a MfTransform3 at 0x10
  (the hand relative to the handle), a MfVector3 at 0x48 (approach direction /
  palm offset), the finger list (MfDblList) at 0x70. MfManipPlanner.cpp reads
  them through local structs (MfManipHandApproach, MfManipHandData,
  MfManipFingerHand) until the shared header gets the members.
- Header flips: declaring MfJointPath::SetInterval (10030100, folded with
  MfScDebugInfo::SetCodeSize) anywhere in MfJointPath flips MfJointPath::Key
  (MfAtomicActivity.cpp) and SetKey; SampleGrasp/SampleMove store mInterval
  instead (TODO). A `static const int` member in MfManipPlanner.h shifts the
  `_$E` numbers of MfManipActivity.cpp; the 33 ms sample interval is a
  file-static int in MfManipPlanner.cpp. An inline MfGCPath() constructor in
  MfGCPath.h helps PlanGrasp but makes GetConfigRecord differ.
- MfVector3::normalize is not imported by RuntimeLib: it was inline in the
  original header (`l = sqrt(lengthSquared()); x /= l; ...`); here a
  file-static normalizeVector.

## Round 9 (RuntimeLib's own script classes, 1006c040-1007c870)

- RTTI names the classes: MfPoHFSM, MfPoJoystick, MfPoSound, MfPoSoundDriver,
  MfPoSoundResource, MfPoView, MfPoSpace, MfPoWorld, MfPoApp. New units
  MfPoHFSM/Joystick/Sound/SoundDriver/SoundResource/World/App.cpp follow
  src/MfPoLibrary's script class pattern (see its NOTES) on canonical headers:
  MfPoBase.h (dllimport), MfScClassInfo.h, MfScPropTokenTable.h,
  MfScPropertyExtList.h, MfScObjString.h, MfPoTransform.h, MfPoSpace.h (the
  class for other units; MfPoSpace.cpp keeps its unit header), MfPoClasses.cpp
  (MfRuntime's init/shutdown of all classes).
- MF_SCRIPT_CLASS_DECLS: members only the script classes need (MfScData::
  Exemplar, MfScDataObject's LookupPropertyToken/AddClassInfo/static
  FilloutPropertyNameList/Get/SetPropertyExt in MfScData.h; the driver's
  Get/SetParameter in MfSoundManager.h) are inside `#ifdef
  MF_SCRIPT_CLASS_DECLS`, defined by MfPoBase.h (and the sound class headers
  before MfSoundManager.h). Declared for every unit, any of them flips
  MfLiveActor::NewParentAfterUngrasp or MfActor::ignoreTriangleCollisionCheck.
- MfScObject declares no destructor (the implicit one jumps to
  ~MfScDataObject, as in MfPoLibrary). MfScClassInfo overrides
  MfListItemInterface's four virtuals inline (MfDblList.h declares them pure;
  making them non-pure there flips MfJointPath::Key and 1009c790).
- Exemplar constructors: members set before the vptr are init-list members,
  after it body assignments (MfPoSound mResource); a static "constructing" flag
  (SoundDriver, World) is set at the very start or after `new MfScClassInfo`,
  cleared at the end. Instance constructors `MfPoBase(1)`.
- Shared tails: per-case block locals (`case 0: { float f; ... break; }`) keep
  VC5 from cross-jumping identical case tails (MfPoSoundResource::GetProperty).
- Small switches compiled as `sub eax,0 / dec / dec` chains lay the case
  bodies out in reverse; write them in token order.
- Register choice from inline accessors: values pushed straight from `eax`
  came from inline getters (MfJoystick::XAxis() ...); an inline helper with two
  returns (`POVValue`) duplicates the call tail per return path.
- Error-path layout: `if (!t) { error; return; }` inline vs. else-blocks at
  the end; separate `if (nArgs >= 3)` and `if (nArgs >= 4)` blocks (not nested)
  where the original tests nArgs >= 4 after a failed nArgs >= 3 test.
- `f(MfString("lit"), g(), &x)`: VC5 evaluates g() before constructing the
  literal temp; the original constructed a named local in its own block first
  (MfPoInitializeClasses).
- In GetInterface("...", &p) the implicit conversion (recomputed address)
  matches, as in MfPoLibrary.
- Name() is called through the import (?Name@MfScData@@QAEAAVMfString@@XZ)
  where it builds a by-value argument (CheckNull(Name() + MfString(...))) and
  expanded elsewhere; the source is the same `Name()`.
- MfISoundDriver slots 0x20/0x28 take no arguments (Halt/Purge, names
  invented), 0x34/0x38 are Get/SetParameter(which, value) (0 volume, 1 speed
  of sound); MfISoundResource 0x28 is GetParameter (0 volume ... 9 sample
  size), 0x2c the setter (SetChannelVolume).
- Open: MfPoWorld GetProperty (inline GetLast would flip MfJointPath::Key),
  DeleteSpace/SaveSpace and MfPoSoundResource::PMLoadFromFile (inline budget
  in the ConvertToUpper expansion).
- MfPoView_1.cpp (head of MfPoView's unit: class name, GetClassName, class
  object constructor) and MfPoSpace_2.cpp (CastRay ... InitConstants) use the
  canonical MfPoView.h/MfPoSpace.h; MfPoView.cpp/MfPoSpace.cpp keep their
  unit headers. The class name of View is a file static (unguarded atexit
  destructor; a static data member gets the guarded one). Still unwritten
  there: MfPoSpace::CheckCollision (10078010).
- Class-object constructors: AddClassInfo may come after the token table
  (View) or before it; follow the binary. A method registered without
  documentation has no MfScCiMethod (View::DisplayText, World's tool-mouse
  methods).
- EH state numbers follow source order: an error block written first
  (`if (!a || !b) { InvalidType(...); return ...; }`) gets the lower state
  even though VC5 places it at the end (MfPoSpace::CastRay).
- MfPoVector3 is 0x1c bytes (bOwner at 0x18); MfPoColor holds an MfColor at
  0x14; SetFogAttributes fills a local MfRenderFogInfo and assigns it.
- MfPoSpace::CheckCollision (TODO body): script lists are read with
  `MfScStdLib::pslliListInterface->GetList((MfDblList **)&list, handle)` and
  MfScHandleList's inline First()/Next(); the data object is type-checked with
  `typeid(*data) != typeid(MfScDataObject)`; MfPlList is only a typeid.

## Round 10 (MfActor, MfActivity, MfSkeletonNode, small members)

- MfActor.cpp: MfHPTimer.h (RuntimeLib copy of PiccoloRT's), MfFloor.h,
  MfRoadMap.h and the other headers only the later functions need are
  included in the middle of the file (before presampleAllSkills): included at
  the top they flip ignoreTriangleCollisionCheck. MfFileProgress (MfActorLib)
  is declared locally there.
- `x += path->MemorySize()` in presampleAllSkills: an MfActorPath method that
  returns 0, folded at 1000de60 (claimed in MfActorPath.cpp).
- Friend declarations in MfStream.h flip MfJointPath::SetKey; MfActivity's
  direct status store uses `friend class MfActivity` under
  `#ifdef MF_STREAM_STATUS_ACCESS` (defined by MfActivity.cpp only), like
  MF_SCRIPT_CLASS_DECLS.
- Implicit copy members of exported classes: MfRenderNode, MfRenderView and
  MfRenderScene no longer declare copy ctor/operator= (claimed via s_assign or
  as emitted by an including unit); MfMetaColModel's (and ??_E) are claimed in
  MfColNode.cpp. MfColModel's bounding box is two MfVector3 (mMin/mMax): the
  implicit operator= copies them unrolled; float[3] gives copy loops.
- Vector deleting destructors (??_E, 80-83 bytes, `eh vector destructor
  iterator`) of the activities, skills, MfHandle, MfSpace, MfManagerActivity
  are emitted by the class's .cpp; claimed at the end of each file.
- Static members with guarded atexit (one flag byte, a bit each):
  MfActivity::activityList/activityTable, MfSkeletonNode::parentLock (now
  defined; claims are the `_$E` entry/ctor/atexit/dtor quartets).
- `bool on = show ? true : false;` gives `test; setne cl` without the
  `xor ecx, ecx` of `show != 0` (ShowNodes).
- A switch without default leaves the pointer uninitialized; the original's
  default path reads the stack slot of the `new` temporary
  (MfSkeletonNode::CreateFromPrototype).
- `node->mColNode.ColModel() = proto->ColModel();` loads the right side
  before the virtual call; through `colNode()` VC5 calls first.
- Functions ending in `xor eax, eax` after the epilogue of the success path
  (GetSplineStart/End, SetMaterial, GetPrevTransform, Load) are nested
  `if (ok) { ...; return 1; } return 0;`. GetPrevTransform keeps the owner in
  a local (`MfLiveActor *owner = (MfLiveActor *)mOwner;`).
- Hook data (MfMiscHookData) member stores: permute when only the order
  differs (SetRenderStyle: m18, delay, kind).
- A second list walk in a function whose first walk used `pos` gets its own
  `MfDblListItem *pli` in the inner block (SetProximityDefinition); the slot
  lands on a dead parameter.
- New/changed declarations: MfLocoLeg is 0x14c bytes (m0fc[20]);
  MfProtoFootData.h (foot/hip/sole indices, from MfActorLib); MfPermutation
  mMap (0x08, new index per old node, -1 removed); MfFilterItem.h with
  MfFilterHookData (MF_HOOK_FILTER_EDIT, kinds 0 add, 1 clear, 2 delete);
  MSG 11 (activity finished) with MfActivityFinishedData (MfActivity.cpp);
  MfTriangleSet::ChangeID; MfProtoActor friend MfActor (isDirty).
- Open (TODO, inline budget or register shapes): RenameNode, SetHomeTransforms,
  MakeSubTreeColNode (sprite test), SetForwardFacingDirection,
  SnapTraceToSurface (stack layout), SetInstanceName, changeParent,
  ScaleSubtree, both MfSkeletonNode::SetProperty (imported MfProperty::Name).
- Still unwritten nearby: MfLocoSkill::ScaleGait (key frame layout unknown),
  MfManipSkill copy ctor/Clear (the three handle prototypes are cleared in a
  loop, i.e. an array in the original), the planner grid/loco planner/road
  map units (10054010-10063fff).

## Round 8 (MfLiveActor)

- New bodies: UpdateChildList, ChangeParent, ChangeCmdState, OldLoadHFSM,
  AppropriateFloor(MfVector3, ...), RequestLocoPlan, NewParentAfterUngrasp,
  MakeHoldTargetActivity, RespondToBlockedPath, HandleActivityFinished,
  LoadActorMethods (empty, folded at 100228e0). HandleLocoPlan and SkillToEnum
  match now.
- HandleLocoPlan had RequestLocoPlan's arguments 6/7 swapped: the call is
  (..., floor, data->m20 /*mode*/, m00, m20, ...), like RespondToClearPath
  (mbc, m00). Its LastPoint is `path->Point((!mOpen ? m28 : mCount) - 1)`
  (the inline Point gives the multiply-modulo), and radius/height declared at
  the top of the block get their own slots (the original's 8 extra bytes).
- MfDblList slot 0x14 is Add (0x08 Append, 0x0c Insert): mBlockList,
  mChildList additions through a pointer call Add.
- MfAiActor slots: 0x64 ClearActivity, 0x6c SetActivity, 0x78
  RemoveActivity(const MfAiActivity *, int); MfFSMManager 0x58
  ClearBehaviors, 0x64 Hook25, 0x80 CompileBehaviors; MfAiColNode 0x1c Owner,
  0x24 CheckCollision(ray).
- Tail merging: three if/else branches that end in the same statements are
  merged by VC5; the original keeps them apart when each branch ends with a
  different assignment (HandleManipGC: duration first, then motion).
- `switch` with three returns: the original's layout (default 0 first,
  then 2, then 1) comes from an extra `case MfSkill::TYPE: return 0;`
  (SkillToEnum).
- Constant 1 in a register for a whole case (ChangeCmdState): reading
  members directly instead of a local (`mManipPlan.mTarget->...`) frees the
  register for it.
- A passed-by-`const char *` property name (`GetProperty("MF_replanPriority",
  p)`) gives the recomputed lea of the temporary (RespondToBlockedPath).
- Road map types (MfRoadMap.h): MfRoadCell, FindRoadCell (1005fa50);
  MfRoadPortal/MfPathPoint are declared in MfLiveActor.cpp (path items:
  portal, blocked edge). MfLocoPlanReqData takes floats as int bits.
- Message 11 data: MfActivityDoneData {MfActivity *mActivity at 8}.

## The road map (1005ec90-10063d10, MfRoadMap.cpp)

- The unit between MfColNode.cpp and MfMsg.cpp is the floor graph: MfRoadEdge,
  MfRoadPortal, MfRoadCell, MfRoadFloor, MfRoadMap, the search records and
  MfRoadMapBuilder. MfRoadMap.h now declares the builder's real fields (the
  0x58 bytes were an opaque array) and the cell's portal list; MfRoadPortal,
  MfRoadEdge and MfPathPoint are defined privately in MfRoadMap.cpp because
  MfLiveActor.cpp and MfPlanCache.h carry partial copies (MfPathPoint really is
  12 bytes: the cost up to the step is at 8). Merging them belongs with those files.
- MakeCellBox (10060d00) matches: a 12-triangle box MfColNode over the cell.
  FindSegments, Rasterize and RasterizeOther are TODO bodies: FindSegments
  links different floors at the floor prototypes' collision spheres (logging a
  flat floor's sphere that reaches a projected floor) and same-floor cells
  along the grid edges between mGrid and mOtherGrid; Rasterize is a scanline
  triangle fill (index (r - row) + (i - column + 1) * (rows + 2) + 1);
  RasterizeOther fills the other cell's floor and carries each square through
  an MfXformMatrix3 (passed by value) into mOtherGrid.
- Writing the triangle-set pointer into a local before declaring the MfRect
  puts the unwind-state store after the pointer chain and gives the
  `mov eax, fs:[0]` first prologue (MakeCellBox matches, MeasureFloor closer).
- Edge costs are `1 - (int)(-length / 0.001f)` (VC5 turns the division by the
  constant into a multiply by -999.99994f).
- Calls through a pointer to a local list (`MfDblList *list = &found;
  list->Append(x)`) give the virtual call the original makes on a local
  MfDblList (RemoveFloor, AddFloor); GetFirst/GetNext on the local itself are
  expanded.
- Destructors that only `jmp` to the next function call an out-of-line helper
  defined after them (MfRoadFloor::DeleteCells, MfRoadMap::DeleteFloors).
- A list item variable reused for two loops shares one slot (AddFloor); a Find
  result declared at function scope gets its own slot (RemoveFloor).
- `MfRect` locals have no constructor call but an unwind state: RuntimeLib's
  MfRect.h gives it an inline empty destructor.

## Round 8 (worker D): scanner, parser, timers, events, runtime, world

- Behaviour files: MfStateLexer.cpp (flex 2.4 as in PiccoloComp: YY_INPUT =
  MfStateCompiler::ReadInput, YY_FATAL_ERROR active, yywrap returns 1) and
  MfStateParser.cpp (Berkeley yacc 1.9, YYFINAL 2, 115 rules) match, yylex and
  yyparse included. MfStateGrammar.h holds the tokens and YYSTYPE. The grammar
  (recovered from the tables) is at the top of MfStateParser.cpp.
  - The keyword lookup is gperf with duplicate hashes; `*s == *str` (not
    `*str == *s`) in the direct case.
  - Rules 5/6 append to an MfFSMList through an inline that evaluates the
    argument first (MfFSMList::AddItem); `$$ = $1` bodies merge in two groups
    by register, which is out of the source's hands (it happened to match).
  - Mf2DVector(x, y) : pos(x, y) gives the parser's point (member init order).
- Timers: the RTTI "MfTimedMsgQueue" is the inner queue; the outer one is
  MfTimerQueue (invented name). MfMsgQueue's destructor is not virtual and
  slot 0 is `RemoveTimer`.
- Message thread handlers (MfMsgDispatcher.cpp): the check-reachable answer
  keeps the place as an MfTransform2 (x, y, heading) and casts it to the
  MfVector3 the planner fills; a `switch` with the quit case before it
  (`if (id == 3) break;`) gives the compare chain.
- Event data (MfPmh.cpp, 100699d0-1006c040): MakeEventData and the twelve
  MfPmh* handlers, each with a static token table (Add("name", n)). The
  handlers need MfPiccoloMessageHandler with inline members (new
  MfPiccoloMessageHandler.h; MfPmhUser.h keeps its own out-of-line copy for
  the files that use MfPmhUser) and <windows.h> before it (GetClassName is
  GetClassNameA in every vtable slot: without the macro the derived class
  adds a new virtual). The planner failure base has `GetFailureCode() {
  return 0; }` (a folded stub, not a pure virtual). gEventData is defined
  there. MfPdbScriptProvider ("SCRIPT" source type) is in the same unit.
- Plugin loader (MfPluginLoader.cpp): a message built from `MfString(a) + b`
  and handed on is a named local in the original (LoadPlugin).
- MfRuntime: MfApplication ("ApplicationI", MfAiApplication) is defined in
  MfRuntime.cpp; Initialize, Shutdown and LoadHookPlugins have bodies.
  Arguments built from `const char *` pass by `lea` of the temporary:
  `MfFindPlugins("hoo", "Hook Manager", files)`, while
  `RemoveInterface(MfString("MfAiWorld"))` (MfWorld's constructor) pushes the
  constructor's eax.
- MfWorld: mCritSect is mfutil's MfCriticalSection (inline InitializeCriticalSection
  between the members, imported destructor in the unwind funclet); new
  MfCriticalSection.h. The data path functions use the "ContentInfoI"
  interface (MfAiContent.h, slots as in the PscContentInfo plugin).
  GetSpaceFileName and the Piccolo source helpers match; the world's
  destructor would match if MfSpaceList had no user-declared destructor, but
  that changes MfLiveActor::NewParentAfterUngrasp.
- MfCameraNode is exported (__declspec(dllexport) in the header): the
  original's vtable has the vector deleting destructor ??_E.
- MfIRenderView slot 5 takes the window messages WndProc passes on
  (HandleMessage(MfWindowMessage *), not Destroy).
- MfFloor: the projected floor's info gets the source's height
  (MfProtoFloorInfo::mHeight at 4).
- Near misses left as TODO with a body: the inline budget (imported
  MfVector3::lengthSquared, MfProperty::Name, MfRotation3::normalize in
  ConstructRay, SetProperty, CreateFloor) and frame-slot differences; the
  EQUIV reviewed lines (Initialize, ManipPlanningThread, MfPmhContour::
  GetProperty) differ only in stack slots or register choice.

## Round 8 (fork D2): activities and paths
- `MfPath3` layout from its constructor (MfPath3.h rewritten from the padding
  arrays; the names other units use are kept): kinds ARC 0 (keys every 10
  degrees, a ring of 36), MOVE 1 (ballistic: mFrame, mVelocity, mVertical,
  mGravity), KEYS 2, SPLINE_KEYS 3, SPLINE 4 (MfSplineNode + its MfSpline).
- VC5 compiles `i % 36` (signed) with `idiv`; the original's magic-multiply
  sequence comes from `i - i / 36 * 36` (MfPath3::Key, MfPath2::Point).
- Stream helpers of the first activity object are in MfActivityStream.cpp
  (10009050-1000aa50): grasp/move paths, configurations, 2D/3D paths, node and
  actor references, ignore masks. `stream << (int)(p != 0)`: without the cast
  the bool overload is called. `if (a) stream << a->UniqueID(); else stream << 0;`
  gives the original's single call with pushes in the arms; the `?:` form
  computes into a register first.
- A pre-declared copy constructor/assignment (from the PDB) can be written out
  as the memberwise form and matches (MfGestureActivity 10006780/10006a00);
  removing the declarations to get the implicit ones flips unrelated functions
  in MfLiveActor.cpp (declaration count).
- `delete [] path->m.. / new MfGCPath`: the original expands the MfGCPath
  constructor and destructor inline in ReadGCPath; MfGCPath.h has the
  destructor out of line (MfLiveActor.cpp), so ReadGCPath stays a TODO.
- MfManagerActivity's removal functions call MfDblList::Append through the
  vtable (not inline) in the original; mfutil declares Append out of line,
  RuntimeLib's MfDblList.h inline.
- The bubble checks (_CheckBubble of the atomic activities) share a skeleton:
  m98 = the space's snapshot environment; per bubble the global center, a ray
  CheckCollision (slot 9) along the step, the hit triangle's normal, two
  MfCollisionMsgData messages (35 to the actor, 36 to the other) with a
  CreateUniqueID event ID; Manip and Attach are written (TODO), Gesture, Loco
  and 3dPath add time searches of their own (two passes, bisection) and are
  not written yet.

## Round 8 (planner grid and distance grid, fork B)

- MfBfp.cpp (10054010-10055d60): the best-first grid search. Nodes come from
  a pool (MfBfpNodePool, 12-byte nodes: short x, y, parent, next), the queue
  is an array of buckets by rounded distance to the goal (mMinBucket the
  lowest non-empty). IsBlocked/Climb/Step/FindFreeCell move a blocked start
  or goal to a free cell (FindFreeCell searches the eight directions, first
  the heading's when given). Step only matches with the bounds/Climb test
  repeated in every direction branch (VC5 then cross-jumps the odd and even
  branches into two tails). InitDirections stores the Y table first, the X
  table second. `x + 0.5` rounding compiles to `fsub qword -0.5`.
- MfBitArray::SetBit (10055160) is defined in MfBfp.cpp (it sits in this
  unit); MfBitArray.h has inline Set/Get (Get returns an uninitialized value
  for an impossible bit, as the original reads a dead slot).
- MfMath has an imported constructor and an inline empty destructor: the
  MfBfp constructor's EH funclet jumps to the imported ??1MfMath while the
  destructor emits nothing.
- MfWorkspace.cpp (10058b40-1005abb0): the distance grid (MfIntArray of
  chamfer distances 10/14). Init sizes the grid (at most 150 cells a side,
  1/147 of the extent otherwise), Rasterize/ScanConvert fill the cells of the
  floor's and the obstacles' triangles cut to a height slab (ClipTriangle, a
  static with nine cases, matches), DistanceTransform runs the two passes.
  Rasterize matches only with the call written in both branches of
  `if (clear)` (the push of the fill value is cross-jumped).
- Open: ScanConvert (frame), DistanceTransform (registers), MfBfp::Plan,
  Climb, BuildPath, Smooth, Shortcut (TODOs with reasons); the MfLocoPlanner
  unit (100560d0-10058b40: ctor, dtor, helpers, PlanToTarget, Plan,
  FindPlace) is decoded but not written yet.

## Round 8: the skill units (MfSkill, MfLocoSkill, MfManipSkill)

- Several functions that use MfJointPath/collision headers flip the operand
  order of `mInterval * mNumKeys` in MfLocoSkill::CycleLength when they come
  before it in MfLocoSkill.cpp (BestMatchIndex, factor); they are defined after
  it. ComputeGaitInfo needs MfColModel.h and MfTriangleSet.h, whose
  declarations flip CycleLength wherever it sits, so it is in MfLocoSkill_2.cpp.
- The frequent near miss of these units: VC5 calls the imported
  MfTransform3/MfRotation3/MfVector3 constructors and member assignments in
  one place and expands them in another (inline budget). Several TODOs
  (MfSkill::factor, MfLocoSkill::factor/normalize/ScaleGait,
  MfManipSkill::ScaleJoint/ReadFromStream/ComputeInfo) differ only that way.
- MfManipHand (0x80 bytes) and MfManipFinger are now declared in
  MfManipSkill.h (copy constructor, destructor, stream members at
  10048a70-10048cd0); MfManipAnchor is a single int.
- MfLocoSkill::BestMatchIndex matches with index loops (`current[k]`), not
  with walking pointers.

MfLocoPlanner (fork B, after round 8): MfLocoPlanner.cpp + MfLocoPlanner.h
with the real signatures of PlanToTarget/Plan (floats, MfTransform2 *,
MfApiActor *goalActor, float *maxDistance, MfDblList *ignore). Match: ctor,
??_G, dtor, OutsideSlab, FindFloorChild, PlanToTarget. TODO: EncloseTarget
(87%, the sum adds the target radius first whatever the source order), Plan
(41%, frame and register choice from the start), FindPlace (55%, the
!useAngles branch keeps 2*pi on the FPU stack; a separate local for the end
angle made it worse, 48%). No shared header changed: MfTriCircle is declared
in the .cpp as an imported class, MaxProximityDef is called imported and cast
to MfProtoProxDef &. MfMsgDispatcher.cpp's LocoPlanningThread now passes the
request words through FLOAT_OF casts to the real float parameters; that costs
it 82% -> 69% (fld/fstp instead of mov/push), still a TODO.

Fork D2, the big unwritten activity bodies (all TODO): MfLocoActivity::_CheckBubble
(27%), MfLocoActivity::_ConformToFloor (59%), Mf3dPathActivity::Update (64%).
- |x| in the Update functions is a compare and fchs, not the fabs intrinsic
  (which this build expands to the fabs instruction): a file-local
  `static inline float Abs(float x) { return x < 0.0f ? -x : x; }` in
  Mf3dPathActivity.cpp and MfLocoActivity.cpp. MfLocoActivity::Update went from
  48% to 63% with it.
- `x + 1.0f` compiles to `fsub -1.0`; the `fsub` with a negative constant in
  the disassembly is not a sign of `-= -1`.
- Mf3dPathActivity::Update: kinds MOVE and unlooped KEYS/SPLINE_KEYS/SPLINE follow
  the path by time (frame = elapsed / 33 ms, KEYS: elapsed / mTotalTime) and are
  finished when the frame passes NumKeys() - 1; the others walk the skill root's
  forward distance along the keys like MfLocoActivity. The path transform is
  set on the skeleton itself (not node 0), spline paths are first taken into the
  parent's frame, and a skill-driven backwards path turns by pi.
- MfLocoActivity::_ConformToFloor: feet whose sole contact flag (MfLocoLeg::m0fc)
  is set between the last and the current foot sample cast a ray down; the
  nearest floor lowers the actor, a second ray from below raises it out of the
  floor it sinks into, feet without floor are sent MSG 0x39 (m38 = 1) with an
  MfContourFailMsgData, and _CheckBubble posts MSG 0x39 with m38 = 2 when no
  checked foot found a floor. MfActor::GetCylinder (slot 0x38 of the MfAiActor
  base) gives the radius used for the 1% clearance.
- MfMsgData.h (add-only): MfContourFailMsgData(MfSkeletonNode *) inline
  constructor; the 14 units that include it check the same before and after.
- The remaining distance in these TODOs is VC5's partial inline expansion (the
  imported MfVector3/MfRotation3 constructors and operator= are called in some
  places, MfDblList::Append through the vtable) and the slot assignment of the
  locals; the call sequences and branch senses were compared one by one.

## TODO review pass (worker D and forks G1-G4)

Checked largest first, and with `logicdiff.sh` and a string-literal screen (every quoted
string the original references must appear in the source; only a float constant came up as a false hit).

- Header placement: MfRoadPortal and MfPathPoint are defined once, in MfRoadMap.h.
  MfRoadEdge's full definition is in MfPlanCache.h, because a forward declaration there flips
  MfLocoSkill::CycleLength (imul operand order), and so does including MfRoadMap.h from it.
  MfRoadMap.h includes MfPlanCache.h.
- Real differences fixed:
  - MfTransTrigAction::Compile's minimum script length (`>= 3`, was 5).
  - The period at the end of HandleTeleportActorRequest's log message.
  - MfManipPlanner::CloseFingers read a stale second mFingers (0xec), not MfManipHand's at 0x70, and called firstTime/lastTime in the wrong order.
  - MfJoystick::SetAxisRange is an if/else chain, not a switch.
  - MfRoadMapBuilder::Search compared against a/b, not the previous edge's ends.
  - MfManipSkill::ComputeInfo returns early for manip type 4.
  - The call sequences of MfLocoPlanner::Plan, MfRoadMapBuilder::FindSegments and MfSkill::Sample.
- Source forms that matter:
  - `int hasExtension = s.ContainsSubString(x, 0) > -1; if (!hasExtension)` keeps the flag in a register the way the original does (ConstructSpaceFilePath now matches); `if (!(... > -1))` folds it.
  - `DeleteFile((char *)path)` picks MfString's `operator char *` like the original.
- EQUIV reviewed patterns:
  - Register or stack-slot choice.
  - Independent loads and stores scheduled in another order.
  - A local MfDblList's Append called through the vtable (slot 8) in the original, inline here as its body `Insert(pData, LP_AFTER, pliLast)`, which is the same as mfutil's Append.
  - Imported small helpers (MfVector3/MfRotation3 constructors and copies, MfProperty::Name, the MfStringRef copy constructor) where our headers inline the same body.
- Left TODO under the strict rule:
  - Tail duplication or merged exits (MfTransTriggeredBy::Compile, MfTimerQueue, MfTransEnterBy).
  - The not-found path that reloads a stack slot (MfTransPropList::Remove, MfLiveActorList::Remove).
  - Inline values that go through a temporary register (First()/GetFirst() results, MfStateFunctionDef, FindPart). This looks like the inline-budget question.
  - EH state offsets (MfCameraNode::WarpToTarget, PointAt).
- Open items:
  - Making MfProtoActor::MaxProximityDef inline, as the original Plan does, flips MfActor::ignoreTriangleCollisionCheck (and more inline members in a shared header could do the same), so it stays out of line.
  - 0x10003590 SetPlannedActivityOptions is now written in MfRuntime.cpp and matches. It is slot 116
    of the MfAiActor vtable (MfLiveActor at 100b3e04, MfApiActor at 100b3134), so its `this` is the
    MfAiActor part at +0x14: [this + 0x2e8..0x2f4] are the object's 0x2fc..0x308, which the header
    now names mPlannedLocoOptions, mPlannedManipOptions and the floats mPlannedLocoParam and
    mPlannedLocoReach (they were m2fc..m308, read as floats by casts). Offsets in any function
    reached through a secondary vtable are relative to that base.
