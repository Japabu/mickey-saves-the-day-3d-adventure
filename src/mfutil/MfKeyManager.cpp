// MfKeyManager (mfutil.dll 0x10004bb0-0x10005580).
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "MfKeyDialog.h"
#include "MfKeyManager.h"

static char *szTitle = "Motivate";

// 0x10004bb0 _$E24 (calls the constructor and registers the destructor)
// 0x10004bc0 _$E21
// 0x10004bd0 _$E23
// 0x10004be0 _$E22
static MfString strAccessInfo("AccessInfo");
// 0x10004bf0 _$E29
// 0x10004c00 _$E26
// 0x10004c10 _$E28
// 0x10004c20 _$E27
static MfString strAccessKey("AccessKey");
// 0x10004c30 _$E34
// 0x10004c40 _$E31
// 0x10004c50 _$E33
// 0x10004c60 _$E32
static MfString strSerialNumber("SerialNumber");

// EQUIV 0x10004c70 ?CheckKey@MfKeyManager@@QAEHXZ: reviewed: MessageBoxA import cached in a register and this spilled (frame +4); same calls, arguments and branches, shared return block laid out differently
// Returns 1 when a valid key is stored or has been entered.
int MfKeyManager::CheckKey()
{
    UtilityApp app;
    MfString key = app.GetProfileString(strAccessInfo, strAccessKey, NULL_STRING);
    int serial = app.GetProfileInt(strAccessInfo, strSerialNumber, 0);
    switch (DoCheckKey(key, serial)) {
    case 0:
        return 1;
    case 1:
        if (MessageBoxA(NULL, "Your current access key has expired. You must enter a valid\naccess key before using this program.",
                        szTitle, MB_OKCANCEL | MB_TASKMODAL) != IDOK)
            return 0;
        break;
    case 2:
        if (MessageBoxA(NULL, "Your current access key is invalid. You must enter a valid\nserial number and access key before using this program.",
                        szTitle, MB_OKCANCEL | MB_TASKMODAL) != IDOK)
            return 0;
        break;
    }
    for (;;) {
        MfString serialText;
        if (serial > 0)
            serialText.Format("%06d", serial);
        MfKeyDialog *dlg = new MfKeyDialog(szTitle, serialText, key, NULL);
        int answer = dlg->DoModal();
        serialText = dlg->GetSerial();
        key = dlg->GetKey();
        delete dlg;
        if (answer != IDOK)
            return 0;
        serial = atoi(serialText);
        int year, month, day, keySerial;
        if (DecodeKey(key, year, month, day, keySerial) && serial > 0 && serial == keySerial &&
            DateIsValid(year, month, day))
            break;
        if (MessageBoxA(NULL, "You have entered an invalid serial number or access key.\nPlease try again.",
                        szTitle, MB_OKCANCEL | MB_TASKMODAL) != IDOK)
            return 0;
    }
    app.WriteProfileString(strAccessInfo, strAccessKey, key);
    app.WriteProfileInt(strAccessInfo, strSerialNumber, serial);
    return 1;
}

// EQUIV 0x10004fb0 ?DoCheckKey@MfKeyManager@@AAEHABVMfString@@H@Z: reviewed: stack slots only: day lives in a local instead of the dead parameter slot (frame +4)
int MfKeyManager::DoCheckKey(const MfString &key, int serial)
{
    int year, month, day, keySerial;
    if (key != NULL_STRING && serial > 0 && DecodeKey(key, year, month, day, keySerial) &&
        keySerial == serial) {
        if (DateIsValid(year, month, day)) {
            int days;
            for (days = 1; days <= 14; days++)
                if (DateOccurs(year, month, day, days))
                    break;
            if (days <= 14) {
                MfString msg;
                msg << "Your current access key expires in " << days
                    << " days.\nPlease contact the Motion Factory to obtain a new access key.";
                MessageBoxA(NULL, msg, szTitle, MB_TASKMODAL);
            }
            return 0;
        }
        return 1;
    }
    return 2;
}

// 0x10005120 ?CheckRuntimeKey@MfKeyManager@@QAEHPBDPAH@Z
// Runtime keys are not checked: every key is valid.
int MfKeyManager::CheckRuntimeKey(const char *key, int *info)
{
    if (info)
        memset(info, 0, 4 * sizeof(int));
    info[0] = 1;
    return 1;
}

// 0x10005150 ?GenerateKey@MfKeyManager@@QAE?AVMfString@@HHHH@Z
MfString MfKeyManager::GenerateKey(int year, int month, int day, int serial)
{
    char digits[14];
    int i, j;

    year -= 1995;
    int shift = (year % 100 + month) % 10;
    int step = (day + month) % 9 + 1;
    sprintf(digits, "%02d%02d%02d%06d%01d", year, month, day, serial, (serial + day + month + year) % 10);
    for (i = 0; i < 13; i++)
        digits[i] = (shift + digits[i] - '0') % 10 + '0';
    char key[16];
    for (i = 0, j = step; i < 13; i++, j += step)
        key[i + 1] = digits[j % 13];
    key[0] = shift + '0';
    key[14] = step + '0';
    key[15] = 0;
    return MfString(key);
}

// EQUIV 0x10005240 ?DecodeKey@MfKeyManager@@QAEHABVMfString@@AAH111@Z: reviewed: register allocation and instruction order only; same checks in the same order, same results
int MfKeyManager::DecodeKey(const MfString &key, int &year, int &month, int &day, int &serial)
{
    char digits[16];
    char text[16];
    int n = 0;
    int i, j;
    for (i = 0; i < key.Length(); i++) {
        if (isdigit(key.GetCharacter(i)) && n < 15)
            text[n++] = key.GetCharacter(i);
    }
    if (n == 15) {

    int step = text[14] - '0';
    int shift = text[0] - '0';
    for (i = 0, j = step; i < 13; i++, j += step)
        digits[j % 13] = text[i + 1];
    for (i = 0; i < 13; i++)
        digits[i] = (digits[i] - shift - '0' + 10) % 10;

    year = digits[0] * 10 + digits[1];
    month = digits[2] * 10 + digits[3];
    day = digits[4] * 10 + digits[5];
    serial = ((((digits[6] * 10 + digits[7]) * 10 + digits[8]) * 10 + digits[9]) * 10 + digits[10]) * 10 + digits[11];
    int check = digits[12];
    if (month >= 1 && month <= 12 && day >= 1 && day <= 31 &&
        (year + day + month + serial) % 10 == check &&
        (year % 100 + month) % 10 == shift &&
        (day + month) % 9 + 1 == step) {
        year += 1995;
        return 1;
    }
    year += 1995;
    }
    return 0;
}

// 0x10005410 ?DateIsValid@MfKeyManager@@QAEHHHH@Z
// The date (the key's expiry) is today or later.
int MfKeyManager::DateIsValid(int year, int month, int day)
{
    MfTime now;
    struct tm *t = now.LocalTime();
    unsigned long today = ((t->tm_year + 1900) * 100 + t->tm_mon + 1) * 100 + t->tm_mday;
    unsigned long date = (year * 100 + month) * 100 + day;
    return date >= today;
}

// 0x100054c0 ?DateOccurs@MfKeyManager@@QAEHHHHH@Z
// The date lies before the day daysAhead days from now.
int MfKeyManager::DateOccurs(int year, int month, int day, int daysAhead)
{
    MfTime now;
    MfTime then(now.Seconds() + daysAhead * 24 * 60 * 60, now.Milli());
    struct tm *t = then.LocalTime();
    unsigned long date = (year * 100 + month) * 100 + day;
    unsigned long until = ((t->tm_year + 1900) * 100 + t->tm_mon + 1) * 100 + t->tm_mday;
    return date < until;
}
