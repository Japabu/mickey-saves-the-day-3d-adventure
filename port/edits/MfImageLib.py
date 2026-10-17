# MfImageLib edits for the port (see port/rewrite.py, EDITS).
EDITS = {}

# MfImageLib/MfImageHandler.cpp: the MfDblList view lists only the slots MfImageLib calls; mfutil's
# real vtable has more between Insert and Clear
EDITS['MfImageLib/MfImageHandler.cpp'] = [
    ('    virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli); // +0x0c\n'
     '    virtual void Clear();\n'
     '\n',
     '    virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli); // +0x0c\n'
     "    // PORT: mfutil's other slots, so that Clear is in its slot (vtablecensus)\n"
     '    virtual void Merge(MfDblList &, EnumListPosition, MfDblListItem *);\n'
     '    virtual void Add(void *const);\n'
     '    virtual void Copy(const MfDblList &);\n'
     '    virtual void Copy(const MfDblList &, class MfCallBack &);\n'
     '    virtual void Copy(const MfDblList &, class MfListItemInterface &);\n'
     '    virtual void *GetFirst(MfDblListItem *&) const;\n'
     '    virtual void *GetNext(MfDblListItem *&) const;\n'
     '    virtual void *GetCurrent(MfDblListItem *&) const;\n'
     '    virtual void *GetPrev(MfDblListItem *&) const;\n'
     '    virtual void *GetLast(MfDblListItem *&) const;\n'
     '    virtual void Iterate(MfCallBack &) const;\n'
     '    virtual void Iterate(MfListItemInterface &) const;\n'
     '    virtual void *DeleteFirst();\n'
     '    virtual void *DeleteLast();\n'
     '    virtual void *Delete(MfDblListItem *&);\n'
     '    virtual void Clear();\n'
     '\n'
     '#line 29\n', 1),
]

# MfImageLib/MfJpgHandler.cpp: Win32 structured exceptions (__try/__except, RaiseException) do not exist on
EDITS['MfImageLib/MfJpgHandler.cpp'] = [
    ('// MfJpgHandler: JPEG reader/writer (MfImageLib.dll 0x10002ac0-0x10003040).\n'
     '#include "MfJpgHandler.h"\n'
     '\n'
     '#define MF_JPEG_EXCEPTION 0xE0000001\n',
     "// macOS; libjpeg's error exit now unwinds with setjmp/longjmp, the usual libjpeg way.\n"
     '// Also fixed: the write path destroyed m_cinfo instead of its own compress object.\n'
     '// MfJpgHandler: JPEG reader/writer (MfImageLib.dll 0x10002ac0-0x10003040).\n'
     '#include "MfJpgHandler.h"\n'
     '#include <setjmp.h>\n'
     '#include <stdlib.h>\n'
     '\n'
     '// where JpgErrorExit returns to (one decode or encode at a time per thread)\n'
     'static __thread jmp_buf *sJpgErrorJump;\n'
     '\n'
     '#define MF_JPEG_EXCEPTION 0xE0000001\n'
     '#line 5\n', 1),
    ('{\n'
     '    if (op == MfImageRead) {\n'
     '        FILE *file = fopen(params.path, "rb");\n'
     '        if (!file)\n'
     '            return MfImageCantOpen;\n',
     '{\n'
     '    if (op == MfImageRead) {\n'
     '        FILE *file = port_fopen(params.path, "rb");\n'
     '        if (!file)\n'
     '            return MfImageCantOpen;\n', 1),
    ('\n'
     '        MfImageError result = MfImageOk;\n'
     '        __try {\n'
     '            jpeg_create_decompress(&m_cinfo);\n'
     '            jpeg_stdio_src(&m_cinfo, file);\n',
     '\n'
     '        MfImageError result = MfImageOk;\n'
     '        jmp_buf jump;\n'
     '        sJpgErrorJump = &jump;\n'
     '        if (setjmp(jump) == 0) {\n'
     '            jpeg_create_decompress(&m_cinfo);\n'
     '            jpeg_stdio_src(&m_cinfo, file);\n'
     '#line 39\n', 1),
    ('                result = MfImageBadFile;\n'
     '            jpeg_finish_decompress(&m_cinfo);\n'
     '        } __except (GetExceptionCode() == MF_JPEG_EXCEPTION) {\n'
     '            result = MfImageBadFile;\n'
     '        }\n'
     '        jpeg_destroy_decompress(&m_cinfo);\n'
     '        fclose(file);\n',
     '                result = MfImageBadFile;\n'
     '            jpeg_finish_decompress(&m_cinfo);\n'
     '        } else {\n'
     '            result = MfImageBadFile;\n'
     '        }\n'
     '        sJpgErrorJump = 0;\n'
     '        jpeg_destroy_decompress(&m_cinfo);\n'
     '        fclose(file);\n'
     '#line 49\n', 1),
    ('            return MfImageBadFile;\n'
     '\n'
     '        FILE *file = fopen(params.path, "wb");\n'
     '        if (!file)\n'
     '            return MfImageCantOpen;\n',
     '            return MfImageBadFile;\n'
     '\n'
     '        FILE *file = port_fopen(params.path, "wb");\n'
     '        if (!file)\n'
     '            return MfImageCantOpen;\n', 1),
    ('\n'
     '        MfImageError result = MfImageOk;\n'
     '        __try {\n'
     '            jpeg_create_compress(&cinfo);\n'
     '            jpeg_stdio_dest(&cinfo, file);\n',
     '\n'
     '        MfImageError result = MfImageOk;\n'
     '        jmp_buf jump;\n'
     '        sJpgErrorJump = &jump;\n'
     '        if (setjmp(jump) == 0) {\n'
     '            jpeg_create_compress(&cinfo);\n'
     '            jpeg_stdio_dest(&cinfo, file);\n'
     '#line 74\n', 1),
    ('            }\n'
     '            jpeg_finish_compress(&cinfo);\n'
     '        } __except (GetExceptionCode() == MF_JPEG_EXCEPTION) {\n'
     '            result = MfImageBadFile;\n'
     '        }\n'
     '        jpeg_destroy_compress((j_compress_ptr)&m_cinfo);\n'
     '        fclose(file);\n'
     '        return result;\n',
     '            }\n'
     '            jpeg_finish_compress(&cinfo);\n'
     '        } else {\n'
     '            result = MfImageBadFile;\n'
     '        }\n'
     '        sJpgErrorJump = 0;\n'
     '        jpeg_destroy_compress(&cinfo);   // the original destroys (j_compress_ptr)&m_cinfo\n'
     '        fclose(file);\n'
     '        return result;\n'
     '#line 93\n', 1),
    ('{\n'
     '    (*cinfo->err->output_message)(cinfo);\n'
     '    RaiseException(MF_JPEG_EXCEPTION, 0, 0, NULL);\n'
     '}\n'
     '\n',
     '{\n'
     '    (*cinfo->err->output_message)(cinfo);\n'
     '    if (sJpgErrorJump)\n'
     '        longjmp(*sJpgErrorJump, 1);\n'
     '    abort();\n'
     '}\n'
     '\n'
     '#line 104\n', 1),
]
