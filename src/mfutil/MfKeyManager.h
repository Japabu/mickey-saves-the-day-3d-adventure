// MfKeyManager (mfutil.dll 0x10004bb0-0x10005580): checks the serial number and
// access key stored in the registry and asks for new ones (MfKeyDialog). A key is
// 15 digits: a shift digit, the 13 shifted and shuffled digits of
// "YYMMDDSSSSSSC" (years since 1995, expiry date, serial number, check digit)
// and the shuffle step. Reconstructed (no PDB): signatures from the exports.
#ifndef MFUTIL_MFKEYMANAGER_H
#define MFUTIL_MFKEYMANAGER_H

#include "MfString.h"

class MFUTIL_API MfKeyManager {
public:
    int CheckKey();
    int CheckRuntimeKey(const char *key, int *info);
    MfString GenerateKey(int year, int month, int day, int serial);
    int DecodeKey(const MfString &key, int &year, int &month, int &day, int &serial);
    int DateIsValid(int year, int month, int day);
    int DateOccurs(int year, int month, int day, int daysAhead);
private:
    // 0 key valid, 1 key expired, 2 no or a wrong key
    int DoCheckKey(const MfString &key, int serial);
};

#endif
