# MfPoLibrary notes

Patterns verified against MfPoLibrary.dll (vc5 `/O2 /GX /MD /GR`, no PDB). Read
this before a new unit; add a line for a new pattern.

## Headers

- Canonical headers, one per class (no per-unit declaration files remain):
  imports `MfString.h`, `MfLock.h`, `MfCallBack.h`, `MfHashTable.h`,
  `MfDictionary.h`, `MfListItemInterface.h`, `MfDblList.h`, `MfStringList.h`,
  `MfThread.h` (mfutil); `MfScData.h`, `MfScDataObject.h`, `MfScObject.h`,
  `MfScDataHandle.h`, `MfScPropToken.h`, `MfScPropTokenTable.h`,
  `MfScPropertyExtList.h`, `MfScClassInfo.h`, `MfScVmEnv.h`, `MfScThread.h`,
  `MfScHandleDict.h`, `MfScObjString.h` (PiccoloRT); `MfStreamObject.h`,
  `MfStreamSizedObject.h`, `MfTypedValue.h`, `MfColor.h`, `MfMaterial.h`
  (MfTexture too), `MfLight.h`, `MfCamera.h`, `MfListValue.h`,
  `MfPropertySet.h`, `MfPropertyList.h`, `MfIntValue.h`, `MfFloatValue.h`,
  `MfStringValue.h`, `MfVector3Value.h`, `MfTransform3Value.h`,
  `MfTriangleSet.h`, `MfColModel.h` (MfActorLib); `MfScStdLib.h`,
  `MfScDataArray.h`, `MfScMethodRoutine.h` (PiccoloRT); `MfActorData.h`,
  `MfHookData.h` (structs from the SteamboatApp PDB); `MfVector3.h`,
  `MfRotation3.h`, `MfTransform3.h` (mflinalg); `MfAi*.h` (application
  interfaces; copies of RuntimeLib's, from the SteamboatApp PDB).
  `MfAiMouse.h`, `MfAiKeyboard.h`, `MfAiPath2.h` are not in the PDB: slot
  layout from the calls, names descriptive.
- `MfPoBase.h` and one header per script class (`MfPoColor.h` ...).
- MfScData/MfScDataObject/MfScObject declare no copy constructor, assignment
  or (MfScObject) destructor: the derived classes' implicit copies are inline
  and store `??_7MfScData` (imported) themselves.
- MfColor has no declared destructor (the implicit one stores only the
  imported MfTypedValue vtable); MfMaterial/MfLight declare theirs.
- MfScClassInfo vtables: MfSortedListItem has `GetKey() const = 0` (not
  Compare); MfScClassInfoItem adds `DisplayInTree(HWND__ *, _TREEITEM *)` and
  `GetTreeImage()`; MfListItemInterface has four inline virtuals.
- MfLock declares no operator= (assignments copy the lock word inline).
- MfDblList::Prepend/Append are not inline (as in mfutil): a local list's
  Append is a virtual call through its vptr (PMCreateCameraActivity).
- Script lists from `pslliListInterface->GetList` are MfScHandleList
  (PscStdlib, `MfScHandleList.h`): inline First()/Next() walk a cursor at
  +0x10. MfPlList is declared there for its typeid. MfPoActor.cpp includes
  the header just before PMCreateCameraActivity: included with the others it
  changes PMRequestSitPlan's instruction order.
- MfScDataHandle: `SetReadOnly()`/`SetReadWrite()` toggle 0x8 (Initialize /
  Shutdown of every class); `GetData()` is `(udwFlags & 0xfc0) ? pdData : 0`;
  `GetNumber()` reads an integer handle (0x10) as float.
- `MfScThread::GetThread()->GetVmEnv()` (both inline; the import called is
  MfThread::GetThread).
- MfScData::Lock()/Unlock() are inline (MfLock::Lock(-1)/Unlock imported);
  MfScDataHandle::SetInt/SetFloat/SetData/SetPointer are inline stores.
- The member-function-pointer typedef is `MfScMethod` (as in PiccoloRT);
  `MfScDataMethod` is the PiccoloRT class.
- `MfScStdLib::pslliListInterface` is private; the classes that build script
  lists are friends (imported data symbol `?pslliListInterface@MfScStdLib@@0...`).
- MfPoBase's constructor has the default argument (`= 0`): its exported
  default-constructor closure `??_FMfPoBase` needs it.
- windows.h must be included before any class with GetClassName/SendMessage
  members (exported names GetClassNameA/SendMessageA).

## The script class pattern (every MfPoX.cpp)

- Object layout: `$E` group (static `MfString strClassName`), GetClassName,
  constructors, exemplar constructor, destructor, Initialize, Shutdown,
  GetProperty, SetProperty, PM methods, FilloutPropertyNameList,
  InitConstants. The class's inline members (GetPropTokenTable, copy
  constructor, assignment, `??_E`) are usually emitted earlier by another
  object (MfPoActivity.cpp, MfPoActor.cpp, MfPoActorNode.cpp, MfPoBase.cpp,
  MfPoLibrary.cpp): claim them in the file of that object.
- Exemplar constructor: `: MfPoBase(0)` (plus member init list when the
  original stores members before the vptr), `uiFlags = (uiFlags & ~0x400) |
  0x200; Name() = strClassName + MfString("<class>"); poExemplar = this;`,
  then `SetProperty(0, "M", MfScDataHandle::CreateData((MfScMethod)&X::PMM,
  "Script::M", nArgs), 1)` per method, `*ci + new MfScCiMethod/CiProperty`,
  `*m + new MfScMethodParameterInfo(text, 0)`, `pttPropTokens = new
  MfScPropTokenTable; pttPropTokens->Add(name, token)`,
  `MfScDataObject::AddClassInfo(ci)` (before or after the tokens: follow the
  binary). Variable argument count is -1.
- Instance constructors pass flag 1 (`MfPoBase(1)`); member stores before
  the vptr mean an init list, after it body assignments.
- GetProperty: `err = 0; *value = 0; if (this == poExemplar) err =
  MfScDataObject::GetProperty(); else switch (token) { ... default: err =
  poExemplar->GetProperty(name, value); }` and one `return err`. Classes
  without an exemplar test first (Mouse, Keyboard) switch first and test in
  `default:`.
- SetProperty: exemplar first, `if (value) switch ... else err =
  PEC_INVALID_PROPERTY_TYPE;`, `if (release && value) value->Release();`. A
  type error inside a case that falls into the else branch is a `goto`.
- PM methods: `MfScDataHandle *result = 0;` assigned in the success branch and
  returned at the end (the original keeps it in memory); `CheckNull(MfString
  ("Script.Method()"), nArgs, args, 0)` returns 0; InvalidType gets
  `&typeid(...)` per argument.
- Colour components are clamped to [0, 1] by an inline helper.
- Exceptions and messages taking `const MfString &`: an implicit conversion
  from `const char *` recomputes the temporary's address (System, List).
- `$E` numbers change with header content; renumber with a failing `match.sh
  --sym XX` (the "defined:" list gives wrapper, constructor, atexit, dtor).

## Code patterns (PM methods)

- Optional-argument arrays: `int *optional = new int[nArgs]; for (...) optional[i]
  = 1; optional[k] = 0; if (CheckNull(..., optional)) { delete[] optional; return
  ...; } delete[] optional;` (the loop becomes `rep stosd`).
- `return CreateData(1)`/`CreateData(0)` merged into one call with `push 1/push 0`:
  `MfScDataHandle *result; if (x) result = CreateData(1); else result =
  CreateData(0); return result;` (a `?:` argument gives neg/sbb instead).
- Block placement: an `if (ok) {...} else { error; return; }` puts the error
  block at the end of the function, `if (!ok) { error; return; }` inline. An
  `if (a || b) { WrongNumber...; } else {...}` also goes to the end, and its EH
  states are numbered in source order.
- A result kept in a callee-saved register and copied at every success exit
  (`mov ebx,eax` / `mov eax,ebx; jmp epilogue`): one `MfScDataHandle *result`
  assigned in an if/else-if chain and a single `return result;` at the end,
  errors `return 0;` (PMRotation3).
- Optional trailing arguments: `if (nArgs == n) { if (args[k]->IsType(...))
  flags = ...; else { InvalidType(...); return 0; } }` (the error goes to the
  end of the function); a check written as `if (!IsType) {error}` stays inline.
- `angle += (float)atan2(...)` keeps the sum in st0; a named `float yaw =
  atan2(...)` followed by `angle += yaw;` stores `angle` (SetForwardFacingDirection).
- `if (args[i]) s = *(MfScObjString *)args[i]->GetData();` skips the
  assignment when the handle is null; `*(MfScObjString *)(args[i] ? ... : 0)`
  does not match.
- Stack slots: a variable declared at function scope (e.g. a list cursor) does
  not share its slot with temporaries of inner blocks; declare where the
  original's frame needs it. Scope a local object in `{}` when the original
  ends its EH state early.
- Evaluation order: named locals force the original order (GetData before
  GetInt, a GetInt before a temporary MfTransform3 etc.).
- `const char *` passed to `const MfString &` (SetProperty names, Find,
  ThrowException, CreateData(NULL_STRING)) recomputes the temporary's address
  (`lea reg,[esp+x]`); an explicit `MfString(...)` uses the constructor's eax.
- Type checks of plain script objects: `MfScData *data = args[0]->GetData(); if
  (!data || typeid(*data) != typeid(MfScDataObject))` (include <typeinfo.h>).
  The verifier masks type-descriptor relocations: check the typeid targets
  against the original's `.?AV...` names.
- `new MfDblList` / `new MfScDataObject(0)` store a local vftable after the
  imported constructor (the class's ??_G is emitted in the object).
- Function-local `static MfString` objects (Material.Get/SetColor ...): each
  gets an atexit destructor `_$E` right after the function.

- A `??_E<class>@@W<n>AEPAXI@Z` adjustor thunk (`sub ecx, n; jmp ??_G`) comes
  with the local vftable of a class whose virtual destructor overrides one of
  a second base (MfListValue in MfPoBase.cpp): claim it in that object.
- Argument blocks that end in the same `InvalidType(...); return 0;` are
  merged by the compiler, and identical trailing pushes of neighbouring
  InvalidType calls are hoisted (PMRequestPlacePlan's third call passes the
  full list with 0 for the goal type).

## Known problems

- VC5 inline budget: in very long exemplar constructors VC5 stops expanding
  inline constructors at some point; canonical headers fixed MfPoCamera and
  MfPoActor, MfPoActorNode still differs at its last property. The same
  decisions show in ordinary functions (imported MfRotation3()/MfVector3 copy
  constructors and assignments in MfPoActor::SetForwardFacingDirection and
  MfPoActorNode::SetProperty): they come out of the same source file with the
  same inline headers, so write the code normally and do not change the
  headers for them.
- The original keeps a function's argument count in memory and an error flag
  or getActivity value in a register where ours does the opposite
  (PMRequestGraspPlan/TouchPlan/PlacePlan, PMCreate3dPathActivity); no
  source form found yet.
- `MfScThread::GetThread()->GetVmEnv()->ThrowException(...)` with the message
  built first: the original loads the environment straight into ecx after
  pushing the arguments; no source form found yet (MfPoEvent PMCopy/PMEvent/
  PMTest).
