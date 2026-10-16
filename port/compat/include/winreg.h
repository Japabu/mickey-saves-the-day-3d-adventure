// winreg.h - the registry calls of advapi32 for the macOS port (written for the port).
// The platform keeps the game's keys in a preferences file.
#ifndef PORT_WINREG_H
#define PORT_WINREG_H

#include "windef.h"

PORT_BEGIN_C

typedef DWORD REGSAM;
#define HKEY_CLASSES_ROOT ((HKEY)(ULONG_PTR)((LONG)0x80000000))
#define HKEY_CURRENT_USER ((HKEY)(ULONG_PTR)((LONG)0x80000001))
#define HKEY_LOCAL_MACHINE ((HKEY)(ULONG_PTR)((LONG)0x80000002))
#define HKEY_USERS ((HKEY)(ULONG_PTR)((LONG)0x80000003))
#define HKEY_CURRENT_CONFIG ((HKEY)(ULONG_PTR)((LONG)0x80000005))
#define REG_NONE 0
#define REG_SZ 1
#define REG_EXPAND_SZ 2
#define REG_BINARY 3
#define REG_DWORD 4
#define REG_DWORD_LITTLE_ENDIAN 4
#define REG_DWORD_BIG_ENDIAN 5
#define REG_MULTI_SZ 7
#define REG_OPTION_NON_VOLATILE 0x00000000L
#define REG_OPTION_VOLATILE 0x00000001L
#define REG_CREATED_NEW_KEY 0x00000001L
#define REG_OPENED_EXISTING_KEY 0x00000002L
#define KEY_QUERY_VALUE 0x0001
#define KEY_SET_VALUE 0x0002
#define KEY_CREATE_SUB_KEY 0x0004
#define KEY_ENUMERATE_SUB_KEYS 0x0008
#define KEY_NOTIFY 0x0010
#define KEY_CREATE_LINK 0x0020
#define KEY_READ 0x20019
#define KEY_WRITE 0x20006
#define KEY_EXECUTE KEY_READ
#define KEY_ALL_ACCESS 0xF003F

LONG WINAPI RegCloseKey(HKEY hKey);
LONG WINAPI RegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions,
                            REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult,
                            LPDWORD lpdwDisposition);
LONG WINAPI RegCreateKeyA(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult);
LONG WINAPI RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
LONG WINAPI RegOpenKeyA(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult);
LONG WINAPI RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData,
                             LPDWORD lpcbData);
LONG WINAPI RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData,
                           DWORD cbData);
LONG WINAPI RegDeleteKeyA(HKEY hKey, LPCSTR lpSubKey);
LONG WINAPI RegDeleteValueA(HKEY hKey, LPCSTR lpValueName);
LONG WINAPI RegEnumKeyExA(HKEY hKey, DWORD dwIndex, LPSTR lpName, LPDWORD lpcchName, LPDWORD lpReserved,
                          LPSTR lpClass, LPDWORD lpcchClass, PFILETIME lpftLastWriteTime);
LONG WINAPI RegEnumValueA(HKEY hKey, DWORD dwIndex, LPSTR lpValueName, LPDWORD lpcchValueName, LPDWORD lpReserved,
                          LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
LONG WINAPI RegFlushKey(HKEY hKey);

#define RegCreateKeyEx RegCreateKeyExA
#define RegCreateKey RegCreateKeyA
#define RegOpenKeyEx RegOpenKeyExA
#define RegOpenKey RegOpenKeyA
#define RegQueryValueEx RegQueryValueExA
#define RegSetValueEx RegSetValueExA
#define RegDeleteKey RegDeleteKeyA
#define RegDeleteValue RegDeleteValueA
#define RegEnumKeyEx RegEnumKeyExA
#define RegEnumValue RegEnumValueA

PORT_END_C

#endif
