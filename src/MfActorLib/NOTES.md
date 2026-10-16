# MfActorLib: source patterns that match (VC5 /O2 /GX /MD)

Read this before a new MfActorLib unit. Every item was verified by match.sh in the
unit named in brackets. Add a line when you find a new pattern; keep it short.

## Shared headers (use them, do not redeclare the class in a unit header)
- `MfStreamObject.h` (vtable StreamType, ReadFromStream, WriteToStream, WriteAsObject;
  MfStreamConstructorEntry), `MfStreamSizedObject.h`, `MfStream.h` (m_version at +4 is
  public: the stream objects test it directly) [MfColModel, MfJL*].
- mfutil imports: `MfString.h` (inline Length() through MfStringRef), `MfDblList.h`
  (inline ctor, GetFirst/GetNext, Prepend/Add), `MfStringList.h` (inline dtor
  `{ Clear(); }`, First/Next) [MfFile_2, MfFSMSpaceToBhvr_2].
- mflinalg imports: `MfVector3.h`, `MfRotation3.h`, `MfTransform3.h`, declared from the
  SteamboatApp PDB (`_x`, accessors `x()`) with the members mflinalg's first object
  emits made inline. MfTransform3::operator= assigns the rotation first [MfColModel].
- `MfAiHook.h`: MfAi, MfInterfaceRegistry, MfAiHookManager and the MF_HOOK_* ids (PDB).
- `MfFile.h`, `MfColModel.h`, `MfJLimit.h` (all five joint limits), `MfFSMSpaceToBhvr.h`.

## Vtables
- Overloaded virtuals: VC5 lays an overload group out in reverse declaration order.
  Declare MfDblList's overloads in mfutil's order (`Copy(list)`, `Copy(list, cb)`,
  `Copy(list, lii)`, ...), MfFile's `Open(const MfString &, int)` before
  `Open(int, char *, int)` [MfFileStream, MfFile].
- MfTypedValue: `~`, Copy, Destroy (pure), Compare `{ return -1; }`,
  ConvertTo `{ return 0; }` after StreamSize; derived value classes do not override
  Compare unless the vtable says so [MfStringValue, MfTransform3Value].
- A class whose ??_E sits in the inline block declares its dtor inline (`virtual ~X() {}`),
  otherwise no deleting dtor is emitted there [MfJLimit_2].
- Inline functions folded by the linker (ICF) into an exported name: the verifier only
  accepts the vtable slot if the names agree, so slots pointing at e.g.
  MfListItemInterface's inline `return 0` stay open (see MfListValue.todo).

## Code shapes
- Object layout: inline members of the unit's header first, then the functions in source
  order; a stream class ends with `static MfStreamObject *fn_<va>() { return new X; }`,
  `static MfStreamConstructorEntry s_entry('type', fn_<va>);` (match the creator with
  `--sym ?fn_<va>@@YAPAVMfStreamObject@@XZ`), then StreamType/ReadFromStream/WriteToStream.
- Static objects with a dtor: `$E<n>` wrapper, ctor, atexit, dtor. Number them with
  `dis.sh`-style `dumpbin /disasm` of the object; a static data member's dtor tests a
  guard bit, a file static's does not [MfFile_2].
- `GetInterface("MfAiHookManager", &ai)` (implicit MfString temporary): an explicit
  `MfString(...)` reuses the ctor's return value instead of a second `lea` [MfColModel].
- Hook override: `CallHooks(MF_HOOK_..., &data) > 0` returns early [MfColModel].
- `delete[] p; p = 0;` without an `if`: the null test is inside the vector delete.
- `return 0` after a test that left eax = 0 needs no `xor eax, eax`: write `return 0`.
- Statement order is not always emission order: in MfFile::ReadBytes the original's
  `m_pos += n` comes first in the code but `count -= n` first in the source.
- Stack slots: a variable that should get its own slot (not share one with a dead
  variable of an inner scope) is declared at function scope [MfColModel::ReadFromStream].
  Separate block-scoped variables let VC5 keep one in a register [MfFileProgress].
- `int count; MfString s; count = 0;` gives the original's slot order and store position
  where `int count = 0;` does not [MfFSMSpaceToBhvr_2].
- Unsigned index from a pointer: `((unsigned long)p - (unsigned long)base) / sizeof(T)`
  (mul by reciprocal, `shr`) [MfColModel::WriteToStream].
- Compiler-generated operator= of a class with a `float[3]` member copies with a loop
  [MfJLEuler].

## Wave 4 additions
- A class's `STREAM_TYPE` defined with an initializer is a known constant from that
  point on: code before the definition loads it from memory, code after it uses the
  immediate. The value classes (MfTypedValue.cpp) define each STREAM_TYPE right
  after the class's ConvertTo/constructors and before its stream creator [MfTypedValue].
- The STREAM_TYPE constants of consecutive classes lie next to each other in .rdata
  when (and only when) one object defines them: MfIntValue ... MfProperty are one
  object (MfTypedValue.cpp) [MfTypedValue].
- A constructor that assigns members in its body stores the vptr first and drops the
  base class's vptr store; an init list stores members before the vptr [MfArrayValue].
- Calling an inline virtual wrapper (MfDblList::Add -> Append) from a constructor
  keeps the inner call virtual; calling Append directly is bound statically [MfListValue].
- `return x ? true : false` gives `mov; test; setne al` for a bool from an int;
  `!= 0` and `(bool)` give `xor eax; test; setne` [MfKeyList::KeyframesBetween].
- An inline accessor (`key->time()`) loads into a register before a compare where
  the member access compares memory directly [MfKeyList::KeyframeAtTime].
- A local declared in an inner block may share the stack slot of a parameter that
  is dead by then (`{ MfDblListItem *pli; ... }`) [mfutil UtilityApp::PluginPath].
- Conditional arguments (`p ? p->x : 0`) to the same call: write them as
  conditional expressions; VC5 duplicates the call into both arms itself.
- MfString's `operator int`/`operator long` are inline (`atoi(mfsr->m_str)`).
- Statics: `$E` numbers depend on everything before them in the object; list them
  in order with a failing `match.sh --sym` (the "defined:" list) and assign pairs
  (jmp stub, constructor) in order.
- `if (...) status = A; else status = B; return status;` (one return) reloads the
  member before returning, matching MfMemoryStream::ReadBytes.
- A flag set in one branch only and tested later (`int hooked = 0;` declared next
  to the code that sets it) shares the register holding the constant 0 for member
  stores; as `bool`, or declared at the top, it gets its own register/slot
  [MfFileStream ctor].
- `count = n; if (n > space) count = space;` gives `mov eax, n; cmp; jbe`, while
  `count = space; if (n <= count) count = n;` gives the reverse [MfFileStream::WriteBytes].
- VC5 decides per call site whether to expand an inline function (MfRotation3 /
  MfVector3 operator=, the MfRotation3 constructor), and the decisions depend on
  the order of the statements in the function: the same assignments came out as
  calls of the imported operator= or inline copies depending on whether the
  switch case with them came first or second [MfJointKeyframe::ComputeBlends].
  Write the statements in a natural order first and permute blocks before
  touching the expressions.
- VC5 reassociates float expressions written as one statement
  (`a*s*s*t*3 + b*s*t*t*3 + t*t*t` became a factored form); the original's
  separately evaluated products come from separate statements with float locals
  [MfJointKeyframe::Interpolate].
- Variables the original keeps in the dead parameter slot: declare them in the
  innermost block that uses them; one function-scope local shared by two uses
  gets its own slot [MfProtoSkill::ReadFromStream].
- dllexport classes get vector deleting destructors (??_E) in their vtables and
  emit their inline members in every object that includes the header, so the
  first (folded) copy of an inline accessor can be claimed from the class's own
  object [MfTreeNode, MfTreeTraversal, MfProtoTraversal].
- MfStringRef has an inline virtual destructor (`delete szString`), so importing
  objects build a local MfStringRef vtable; MfString::ConvertToUpper/Lower and
  CopyOnWrite are inline (CopyOnWrite through Increment/Decrement/GetCount and
  MfStringRef::CopyOnWrite). Its statics are protected (`?nNumPadChars@MfStringRef@@1HA`).
- The stream constructor of each class ('Create...', a static function) and its
  MfStreamConstructorEntry follow the class's constructor in the object; the
  STREAM_TYPE must be defined before them (the entry pushes it as an immediate).
- MfProtoActor: SetupFromPrototype/SetParentIndices use the geometry traversal
  (+0x1c, GeomTraversal), not NodeTraversal (+0x18).

## Wave 6 (MfProtoActor, MfProtoDB, MfProtoNode accessors)
- MfProtoActor.h is the PDB declaration; the unit is MfProtoActor_2.cpp
  (0x10010bf0-0x10015e60, with MfProtoDB). MfProtoActor.cpp only holds the inline
  members emitted at the start of the MfAnimSequence_2 object.
- `Properties().Add(new MfProperty("MF_x", new MfStringValue(NULL_STRING)))`
  matches with implicit conversions for both strings; MfNullString must be a
  complete class (MfString.h) for the NULL_STRING conversion [MfProtoActor()].
- Member lists (skills, proxDefs, ...) are walked through virtual GetFirst/GetNext
  calls even as members of this; loops test `pli` or the item as the original does.
- VC5 limits inline expansion per function: with many inline accessor calls
  (x(), y(), setValue) later inline constructors become calls. `v[i]` used fewer
  expansions than `v.x()` in ComputeBounds; MoveSubtree still differs.
- `if (s.Status() == MF_STREAM_OK) { ... actor = ReadObject(0); } if (actor) {...}`
  (second if outside) lets the ints of the first block share slots with the
  strings of the second [ReadActorFile].
- A pointer found in a table and the result variable are separate locals
  (`found` returned early, `proto = 0` afterwards) [MfProtoDB::GetPrototype].
- MfProtoProxDef's destructor is out of line (called from ~MfProtoActor).
- MfProtoNode is MfTreeNode + MfStreamObject (second vptr at +0x14); its
  accessors are inline in declaration order (the old-format reader object at
  0x1000c8a0 emits them). SetHomeTransform expands MfRotation3::normalize inline,
  which our MfRotation3.h does not (making it inline flips an operand order in
  MfJointKeyframe::Interpolate).
- MfProtoGeomInfo/MfProtoHandleInfo layouts as in RuntimeLib.
- MfProtoNode_2.cpp (0x10015e60-0x10017bb0): the info pointer at +0x78 is
  `void *mInfo`; the unit casts it directly (`delete (MfLight *)mInfo`,
  `((MfProtoHandleInfo *)mInfo)->mRadius = ...`): the inline accessors re-test
  the style. DeleteInfo is a switch with `mInfo = 0;` after it (VC5 copies it
  into every case); the case order is the source order of the original.
- The MfTri* primitives' compiler-generated copy constructors, destructor and
  operator= come at the start of the MfProtoNode_2 object (MfTriPrimitives.h,
  MfTriangleSet without a declared operator=).
- MfCamera.h now has MfCamera_unit.h's layout (size 0x34).

## Wave 7
- MfRotation3.h includes <math.h> and has mflinalg's inline normalize()
  (`inv = 1/sqrt(...)`, four `*= inv`); SetHomeTransform expands it.
- Include sets steer VC5's register/x87 tie-breaks (see mfutil NOTES, wave 7):
  MfProtoNode.cpp starts with <string.h>; without it SetHomeTransform differs.
  Every header change can flip such a function elsewhere: run check.sh on the
  whole directory after touching a header (MfJointKeyframe.cpp needed <stdlib.h>
  for a while and lost it again when MfVector3.h got lengthSquared/normalize).
- Some pairs flip together and never both match (the inlined normalize in
  SetHomeTransform and ReadOldProtoNode): keep the claimed one, TODO the other.

## Wave 8 (MfTriangleSet, primitives, MfProtoNode streaming, old node format)
- MfTriangleSet.h has the real layout (mSource shares vertices and triangles;
  MF_TRISET_* flags; MfTriangleSetType values in enums.h). MfTriangle,
  MfTextureVertex and MfOrientedBox stay in MfColModel.h.
- MfVector3.h: lengthSquared and normalize are inline as in mflinalg
  (normalize divides by sqrt(lengthSquared())); it includes <math.h>.
- `$E` names of statics are ours: take them from the failing `--sym` list and
  renumber after adding code before the static (scratch scripts did that).
  Includes placed after the statics keep their numbers low [MfTriangleSet].
- A float constant like 9.9999994e-11 is `EPS * EPS` folded in float
  (EPSILON 1e-5f) [RemoveDegenerateTriangles].
- Arrays get `new T[n]` with the EH vector constructor iterator only for classes
  with constructors; `delete` (not delete[]) is used on plain arrays in this code.
- Values the original copies through integer registers before a call
  (`TextureVertex(i, 0).Set(u, v)`) are setter arguments evaluated before the call.
- Locals that are copied with integer moves and have no EH state are float
  arrays, not MfVector3 (MfVector3 has a destructor) [CheckIntersection].
- A temporary (`setValue(PI / 2, MfVector3(1, 0, 0))`) shows as an EH state set
  before the call and restored after it; named locals of different switch cases
  share slots, temporaries do not [CreateColModel].
- `if (a && b) { ... continue; } delete p;` duplicates the destruction of the
  loop's locals on both paths where an else shares it [LoadAllSkills].
- The old-format reader (0x1000cbf0-0x1000e000) is a set of free functions in
  MfProtoNode.cpp (ReadOldMaterial, ReadOldTriangleSet, ReadOldCamera, ...);
  MfProtoNode makes ReadOldProtoNode a friend, MfCamera's members are public.
- Switch cases are laid out in source order: the order of the case blocks in the
  binary gives the order of the cases [MfProtoNode::Read/WriteToStream].

## Wave 9 (MfTriangleSet geometry, MfOrientedBox, old actor format)
- The geometry routines are textbook code; following the source's own shape
  matters more than tidy C++: RAPID tri.cpp (tri_contact with float* helpers
  VcrossV/VdotV/max3/min3/project6 kept out of line), RAPID obb_disjoint
  (Bf = ABS(B) + reps, which VC5 compiles as fsub of -1e-6), Badouel
  ray-triangle (CheckRayIntersectionCommon), Ritter's sphere, Haines' ray-convex
  polyhedron slab test (MfOrientedBox::CheckRayIntersection).
- Inline helpers that are too big for every call site get an out-of-line copy
  after the last user; claim it like a function (PointSegmentDistance2 and
  SegmentDistance2 after FindDistance). A copy of a value the original moves
  through an integer register into the same slot is a second variable
  (`sa = ta` in the parallel case of SegmentDistance2).
- VC5 has a per-function budget of inline expansions. Nested expansions (the
  members' constructors inside an inline constructor) are dropped first, and a
  dropped dllimport inline becomes a call through the import. So an import call
  of an inline member in the original means the budget ran out, not that the
  member is out of line. Removing any one other inline constructor from
  ReadOldProperty makes the MfDblList constructor of MfListValue expand: the
  budget counts expansions. Which ones the original spends is still open
  (FindDistance, MfOrientedBox::CheckIntersection, ReadOldProperty, MoveSubtree).
- MfTriangleSet.cpp includes <float.h> (FLT_MAX in FindDistance and the ray
  test); with it VcrossV and VdotV keep matching after the MfOrientedBox
  declarations went into MfColModel.h.
- Separate stack slots for reads of the same stream field mean separate
  variables (`type` and `flag` in SkipOldSphereTree).
- ReadOldActorFormat (friend of MfProtoActor) and ReadOldProperty are the last
  two old-format readers in MfProtoNode.cpp. Adding them flipped the
  normalize pair: ReadOldProtoNode is now the claim, SetHomeTransform the TODO.

## Wave 10 (sphere tree build, joint limits, near misses)
- Read the original's control flow before blaming register allocation: two old
  "register allocation" TODOs were structure. MfTriangleSet::Create allocates
  the material indices even without vertices (`if (numVertices > 0) {...}`, no
  early return); ComputeBounds runs its box loop after the if/else, not inside
  the else (the if/else tails merge into one store).
- The matcher masks float constants behind fixups, so swapped factors only show
  as swapped loads: CreateColModel's flag size is (0.005, length * 0.625,
  radius * 1.25), not (.., radius * 1.25, length * 0.625).
- `MfTransform3 xf(MfRotation3(1, 0, 0, 0), MfVector3(0, 0, 0));` compiles to
  x87 constant stores with one integer 0 (translation x), no temporaries
  [CreateColModel]. Where the inline budget is spent (InsertControlPoint) the
  same line calls the imported constructors instead; still open there.
- Statement order steers load order: Merge's sums match with numVertices
  computed before numTriangles. After a memcpy the member update comes before
  the pointer advance (`bufferPos += count; dst += count;`) [MfFileStream].
- `while (i < j) { while ...; while ...; if (i < j) { swap } }`: the back edge
  goes to the loop test; `if (i >= j) break;` makes VC5 drop the inner test
  [MfColModel::ReorderTriangles].
- Writing `&vertices[3 * first]` in both calls (no pointer local) keeps
  3 * count in a register and the address in a stack temp; a local pointer
  takes the register instead [DivideAndConquer].
- `d.length() >= fabs(a - b)` (the call first) compares with fcompp;
  `fabs(a - b) <= d.length()` spills the fabs result to a double temp.
- A class exported in the original (MfRect: dllexport, operator= not declared)
  gets its compiler-generated operator= emitted in its own object; it folds
  with MfPermutation's identical one.
- Unsolved register choices (TODOs): the pointer of a `?:` between two members
  goes to eax in the original, VC5 here reuses the dead member pointer
  register (MfJLHinge::FixTransform); fixed/this/&t.rotation() in
  MfJLEuler::FixTransform come out esi/edi/ebx there, ebx/esi/edi here.

## Review r12b
- An inline wrapper adds a nesting level, and VC5 drops nested expansions first:
  `node->SetHomeTransform(~toParent * toNode)` (not the two statements of its body)
  makes MoveSubtree call MfVector3/MfRotation3::operator= and normalize through the
  imports and expand MfRotation3() in the MfTransform3 constructors, as the original.
- `#include <stdio.h>` first in MfProtoNode_2.cpp gives ReadFromStream the original's
  inlined normalize (length and inverse spilled to memory); no claim there moves.
- MfOrientedBox::CheckIntersection: the B1 x B2 = B0 sum is
  `(B03*B00 + B13*B10) + B23*B20` (parenthesized; VC5 regrouped it otherwise), and
  `return r ? true : false;` gives the final `test eax, ecx; setne al`.
