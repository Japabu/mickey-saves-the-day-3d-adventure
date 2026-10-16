# PiccoloComp.dll notes (vc5 /O2 /GX /MD /GR, no PDB)

The sources are one file per class, in alphabetical order (MfPcAddExpression.cpp
first, the yacc/lex parser last). Names of non-exported classes come from RTTI;
method names of non-exported classes are ours.

Patterns verified by matches in this binary.

- **List iteration**: mfutil's `GetFirst/GetNext/GetLast` are virtual and not
  inline; called qualified (`MfDblList::GetFirst(pli)`) they are direct calls
  through the IAT. The inline iteration the compiler uses comes from non-virtual
  inline helpers `First/Next/Last/Prev(MfDblListItem *&)` in MfDblList.h, all in
  the form `void *p; ...; if (pli) p = pli->GetData(); else p = 0; return p;`
  (the ternary form gives other registers). `MfPcExpressionList::First()` etc.
  call these with the member cursor (`MfDblList::Next(mCurrent)`).
- `new MfPcCodeChunk; chunk->Op(...); chunks->Append(chunk);` is the shape of
  all code generation; `MfPcChunkList::Append` is the virtual MfDblList::Append.
- **Constant folding** (Optimize): evaluate the right operand first into the
  result variable: `int v = (int)*rv; v = (int)*lv / v;`, `v += (int)*lv;`,
  `int v = (unsigned int)*r; v &= (unsigned int)*l;`. With one expression
  (`*l & *r`) the evaluation order flips whenever an unrelated header changes.
  The checks are nested: `if (lv && rv) { if (lv->IsNumber() && rv->IsNumber())
  { if (lv->IsInt() && rv->IsInt()) ... else ... } }`. MfVariant's IsInt etc.
  read `pvdData->iType` directly.
- Shared tails of switch cases (`case X: kind = "X"; goto value;`) are written
  with goto (MfPcIdentifier::Describe); VC5 does not merge identical case bodies.
- The Generate() flags parameter is unsigned (`(flags >> 1) & 1` is `shr`).
- `MfPcArgumentExpression`'s constructor is defined at the end of
  MfPcAllocExpression.cpp (0x100020c0), not inline.
- `MfString::operator int()` is inline (`atoi(mfsr->m_str)`).

- **Generated scanners and parsers** (MfPcLexer.cpp, MfPcParser.cpp and the
  constant set pair): flex 2.4 and Berkeley yacc 1.9 output with prefixes
  "pc"/"cs". The tables are data copied from the binary; the rules and the
  grammar were recovered by simulating the DFA and the LALR tables (comments at
  the top of each file). flex's YY_FATAL_ERROR checks are compiled out
  (`#define YY_FATAL_ERROR(msg)`), stdin/stdout go through the imported `_iob`
  array. A rule with an empty action `{ }` still gets a case label (the cs
  parser's jump table starts at rule 2).
- The scanner uses the later mfutil MfString header: `StripWhiteSpace` makes
  the body unique inline (`MFSTRING_INLINE_COPY_ON_WRITE` in MfString.h); the
  compiler's own units call the imported `CopyOnWrite(0)`.
- **Frame layout in big functions**: VC5 orders the frame slots by number of
  references, so a named local and a temporary are not interchangeable even
  when the code is the same. Error messages built from `char *` are implicit
  temporaries (`CompileError(t, 0, 8, label->mTokenInfo->mText)`,
  `BeginFunction("<ERROR>")`, `new MfPcLiteralExpression(yyvsp[0].ival)`):
  the implicit conversion passes the temporary by `lea` of its slot and
  references it three times; an explicit `MfString(x)` pushes the constructor's
  `eax` (two references). The state store after the argument pushes marks a
  temporary.
- A named local in its own nested block can share the slot of a dead
  temporary or argument (DeclareIdentifier); at function scope it gets a slot
  of its own.
- `char flags = compiler->Flags(); if (flags & 1) a = 7; else a = 6;` gives
  `movsx; and 1; or 6`; every one-expression form folds the sign extension.
- Identical case bodies (`$$ = $1`) are merged after register allocation,
  which rotates eax/ecx/edx between otherwise identical blocks; the groups
  therefore depend on the code of the whole function, and a change in one
  rule moves registers in rules far away.

- Mult and Mod fold in one expression (`int v = (int)*lv * (int)*rv;`, `%`);
  the shifts need the right operand first in its own statement
  (`int v = (unsigned int)*rv; v = (unsigned int)*lv << v;`), computed before
  `new MfPcLiteralExpression(v)`.
- A store whose value comes from a call through a pointer chain: a named
  pointer local evaluates the target first (`MfPcIdentifier *id =
  IDENTIFIER(array); id->mArraySize = *value;`, pcparse rule 74); without it
  VC5 calls first and the registers of the whole switch shift. Likewise a typed
  local for the expression (`MfPcIdentifierExpression *e = ...; e->mIdentifier->mKind`)
  re-reads the member where the original does (csparse rule 56).
- A search result tested after its temporary dies is a named int
  (`int found = info.ContainsSubString("****", 0); if (found == 0)`), with the
  `char *` arguments converted implicitly; a conversion of a returned MfString
  goes through a named local (`MfString number = info.MidString(...); line = number;`).
- `(kind == X) ? (unsigned char)A : (unsigned char)B` builds the byte in dl
  (`sete dl` without clearing edx); unconverted enum arms clear edx first.

Open problems (see the TODO and EQUIV lines): several functions differ only in
whether VC5 keeps a repeated constant (0, 7) or an import address in a
callee-saved register; no source change tried so far moves that decision. Some
that looked like it were a different source shape (above).
- MfLogger::Log's product and module strings are implicit temporaries
  (`MfLogger::Log(1, "Piccolo", "Source File Reader", messages)`, 0x10005f60)
  where they are built in the call; named locals where they are built before
  the Appends. CodeLoader's mfutil header has `MfString::ReadFromFile` inline
  (MakeUnique) with the MfStringRef constructor and CopyOnWrite imported.
