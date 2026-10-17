// MfImageLib_mfutil.h - MfImageLib's view of mfutil.dll's MfString (MfDblList's view is
// in MfImageHandler.cpp, MfLogger's in MfJpgHandler.h: MfTgaHandler.cpp must not see them,
// VC5's register allocation in MfTgaHandler::Write reacts to them). Wave 2 replaces this view
// with mfutil's canonical header.
#ifndef MFIMAGELIB_MFUTIL_H
#define MFIMAGELIB_MFUTIL_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif

// mfutil.dll
class MFUTIL_API MfString {
public:
    MfString();
    MfString(const char *s);
    MfString(const MfString &s);
    ~MfString();
    MfString &operator=(const MfString &s);
    operator const char *() const;
    void SplitPath(MfString *drive, MfString *dir, MfString *name, MfString *ext) const;
    int CaseContainsSubString(const MfString &s, int start) const;
private:
    char *m_str;
};

#endif
