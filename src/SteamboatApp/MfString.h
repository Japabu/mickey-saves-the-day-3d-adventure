#pragma once
// MfString.h: Motivate's reference-counted string (MfActorLib.dll). Declaration from
// SteamboatApp.pdb, without the cursor and scanner members the game never calls.
class MfNullString;
class MfStringList;
class MfStringRef;
class MfStringSize;
class MfVariant;

class MfString {
public:
    MfString(const MfVariant &);
    MfString(const MfStringSize &);
    MfString(const double);
    MfString(const float);
    MfString(const unsigned int);
    MfString(const int);
    MfString(const MfNullString &);
    MfString(const unsigned short *);
    MfString(const char *);
    MfString(const MfString &);
    MfString();
    ~MfString();
    MfString &operator=(const double);
    MfString &operator=(const float);
    MfString &operator=(const int);
    MfString &operator=(const MfNullString &);
    MfString &operator=(const unsigned short *);
    MfString &operator=(const char *);
    MfString &operator=(const MfString &);
    operator const char *() const;
    operator char *() const;
    operator const unsigned short *() const;
    operator unsigned short *() const;
    operator int() const;
    operator long() const;
    operator unsigned int() const;
    operator float() const;
    operator double() const;
    int __cdecl Format(char *const, ...);
    MfString &operator+=(const double);
    MfString &operator+=(const float);
    MfString &operator+=(const int);
    MfString &operator+=(const unsigned short);
    MfString &operator+=(const char);
    MfString &operator+=(const unsigned short *);
    MfString &operator+=(const char *);
    MfString &operator+=(const MfString &);
    MfString operator+(const MfString &) const;
    int operator==(const MfNullString &) const;
    int operator==(const unsigned short *) const;
    int operator==(const char *) const;
    int operator==(const MfString &) const;
    int operator!=(const MfNullString &) const;
    int operator!=(const unsigned short *) const;
    int operator!=(const char *) const;
    int operator!=(const MfString &) const;
    int operator<(const MfString &) const;
    void ConvertToUpper();
    void ConvertToLower();
    void Truncate(int);
    void StripWhiteSpace();
    int ContainsSubString(const MfString &, int) const;
    int CaseContainsSubString(const MfString &, int) const;
    int CaseCompare(const MfString &) const;
    int Compare(const MfString &) const;
    int Length() const;
    MfString LeftString(int) const;
    MfString RightString(int) const;
    MfString MidString(int, int) const;
    char GetCharacter(int) const;
    char operator[](int) const;
    int SetCharacter(int, char);
    void ConvertToLines(MfStringList *) const;
    unsigned long Hash(int) const;
    void SplitPath(MfString *, MfString *, MfString *, MfString *) const;
private:
    MfStringRef *pmfsrString;
};
