// registry.cpp - the registry as a text file, registry.ini in the overlay directory:
// one [section] per key path ("HKEY_CURRENT_USER\Software\..."), one name=type:value
// line per value (type sz, dword or hex). Written back after every change.
#include "internal.h"

#include <map>
#include <mutex>
#include <stdio.h>
#include <string.h>
#include <strings.h>

namespace win32 {

struct NoCase {
    bool operator()(const std::string &a, const std::string &b) const { return strcasecmp(a.c_str(), b.c_str()) < 0; }
};

struct Value {
    DWORD type = REG_SZ;
    std::string data;  // raw bytes (strings with their terminating zero)
};

typedef std::map<std::string, Value, NoCase> Values;
static std::map<std::string, Values, NoCase> s_keys;
static std::recursive_mutex s_mu;
static bool s_loaded;

struct KeyObject {
    std::string path;
};

static std::string file() { return overlay_dir() + "/registry.ini"; }

static int hexval(char c) { return c <= '9' ? c - '0' : (c | 0x20) - 'a' + 10; }

static void load()
{
    if (s_loaded)
        return;
    s_loaded = true;
    FILE *f = fopen(file().c_str(), "r");
    if (!f)
        return;
    char line[8192];
    std::string section;
    while (fgets(line, sizeof(line), f)) {
        std::string l = line;
        while (!l.empty() && (l.back() == '\n' || l.back() == '\r'))
            l.pop_back();
        if (l.empty())
            continue;
        if (l[0] == '[' && l.back() == ']') {
            section = l.substr(1, l.size() - 2);
            s_keys[section];
            continue;
        }
        size_t eq = l.find('='), colon = l.find(':', eq);
        if (eq == std::string::npos || colon == std::string::npos)
            continue;
        std::string name = l.substr(0, eq), type = l.substr(eq + 1, colon - eq - 1), val = l.substr(colon + 1);
        if (name == "@")
            name = "";
        Value v;
        if (type == "dword") {
            DWORD d = (DWORD)strtoul(val.c_str(), 0, 0);
            v.type = REG_DWORD;
            v.data.assign((const char *)&d, 4);
        } else if (type == "sz") {
            v.type = REG_SZ;
            v.data = val;
            v.data += '\0';
        } else {
            v.type = (DWORD)strtoul(type.c_str() + 3, 0, 10);  // hex<type>
            for (size_t i = 0; i + 1 < val.size(); i += 2)
                v.data += (char)(hexval(val[i]) * 16 + hexval(val[i + 1]));
        }
        s_keys[section][name] = v;
    }
    fclose(f);
}

static void save()
{
    FILE *f = fopen(file().c_str(), "w");
    if (!f)
        return;
    for (auto &k : s_keys) {
        fprintf(f, "[%s]\n", k.first.c_str());
        for (auto &v : k.second) {
            const char *name = v.first.empty() ? "@" : v.first.c_str();
            const Value &x = v.second;
            if (x.type == REG_DWORD && x.data.size() == 4) {
                DWORD d;
                memcpy(&d, x.data.data(), 4);
                fprintf(f, "%s=dword:0x%08x\n", name, d);
            } else if (x.type == REG_SZ && !x.data.empty() && x.data.find('\n') == std::string::npos &&
                       x.data.find('\0') == x.data.size() - 1) {
                fprintf(f, "%s=sz:%s\n", name, x.data.c_str());
            } else {
                fprintf(f, "%s=hex%u:", name, x.type);
                for (unsigned char c : x.data)
                    fprintf(f, "%02x", c);
                fprintf(f, "\n");
            }
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

static std::string root_name(HKEY k)
{
    if (k == HKEY_CLASSES_ROOT) return "HKEY_CLASSES_ROOT";
    if (k == HKEY_CURRENT_USER) return "HKEY_CURRENT_USER";
    if (k == HKEY_LOCAL_MACHINE) return "HKEY_LOCAL_MACHINE";
    if (k == HKEY_USERS) return "HKEY_USERS";
    if (k == HKEY_CURRENT_CONFIG) return "HKEY_CURRENT_CONFIG";
    return ((KeyObject *)k)->path;
}

static std::string join(HKEY k, LPCSTR sub)
{
    std::string p = root_name(k);
    if (sub && *sub) {
        std::string s = sub;
        while (!s.empty() && s.back() == '\\')
            s.pop_back();
        while (!s.empty() && s[0] == '\\')
            s.erase(0, 1);
        if (!s.empty())
            p += "\\" + s;
    }
    return p;
}

static bool is_root(HKEY k)
{
    return k == HKEY_CLASSES_ROOT || k == HKEY_CURRENT_USER || k == HKEY_LOCAL_MACHINE || k == HKEY_USERS ||
           k == HKEY_CURRENT_CONFIG;
}

}  // namespace win32

using namespace win32;

extern "C" {

LONG WINAPI RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD, REGSAM, PHKEY phkResult)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    load();
    std::string p = join(hKey, lpSubKey);
    if (s_keys.find(p) == s_keys.end()) {
        log("registry", "open %s: not found\n", p.c_str());
        return ERROR_FILE_NOT_FOUND;
    }
    KeyObject *k = new KeyObject;
    k->path = p;
    *phkResult = (HKEY)k;
    return ERROR_SUCCESS;
}

LONG WINAPI RegOpenKeyA(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult)
{
    return RegOpenKeyExA(hKey, lpSubKey, 0, KEY_ALL_ACCESS, phkResult);
}

LONG WINAPI RegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD, LPSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES,
                            PHKEY phkResult, LPDWORD lpdwDisposition)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    load();
    std::string p = join(hKey, lpSubKey);
    bool existed = s_keys.find(p) != s_keys.end();
    if (!existed) {
        // Every key along the path exists from now on.
        size_t i = 0;
        while (i != std::string::npos) {
            i = p.find('\\', i + 1);
            s_keys[p.substr(0, i)];
        }
        save();
    }
    if (lpdwDisposition)
        *lpdwDisposition = existed ? REG_OPENED_EXISTING_KEY : REG_CREATED_NEW_KEY;
    KeyObject *k = new KeyObject;
    k->path = p;
    *phkResult = (HKEY)k;
    return ERROR_SUCCESS;
}

LONG WINAPI RegCreateKeyA(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult)
{
    return RegCreateKeyExA(hKey, lpSubKey, 0, 0, 0, KEY_ALL_ACCESS, 0, phkResult, (LPDWORD)0);
}

LONG WINAPI RegCloseKey(HKEY hKey)
{
    if (!is_root(hKey))
        delete (KeyObject *)hKey;
    return ERROR_SUCCESS;
}

LONG WINAPI RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    load();
    auto k = s_keys.find(root_name(hKey));
    if (k == s_keys.end())
        return ERROR_FILE_NOT_FOUND;
    auto v = k->second.find(lpValueName ? lpValueName : "");
    if (v == k->second.end())
        return ERROR_FILE_NOT_FOUND;
    if (lpType)
        *lpType = v->second.type;
    DWORD n = (DWORD)v->second.data.size();
    if (lpData) {
        if (!lpcbData || *lpcbData < n) {
            if (lpcbData)
                *lpcbData = n;
            return ERROR_MORE_DATA;
        }
        memcpy(lpData, v->second.data.data(), n);
    }
    if (lpcbData)
        *lpcbData = n;
    return ERROR_SUCCESS;
}

LONG WINAPI RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD, DWORD dwType, const BYTE *lpData, DWORD cbData)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    load();
    Value v;
    v.type = dwType;
    v.data.assign((const char *)lpData, cbData);
    if ((dwType == REG_SZ || dwType == REG_EXPAND_SZ) && (v.data.empty() || v.data.back() != '\0'))
        v.data += '\0';
    s_keys[root_name(hKey)][lpValueName ? lpValueName : ""] = v;
    save();
    return ERROR_SUCCESS;
}

LONG WINAPI RegDeleteValueA(HKEY hKey, LPCSTR lpValueName)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    load();
    auto k = s_keys.find(root_name(hKey));
    if (k == s_keys.end() || !k->second.erase(lpValueName ? lpValueName : ""))
        return ERROR_FILE_NOT_FOUND;
    save();
    return ERROR_SUCCESS;
}

LONG WINAPI RegDeleteKeyA(HKEY hKey, LPCSTR lpSubKey)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    load();
    if (!s_keys.erase(join(hKey, lpSubKey)))
        return ERROR_FILE_NOT_FOUND;
    save();
    return ERROR_SUCCESS;
}

LONG WINAPI RegEnumKeyExA(HKEY hKey, DWORD dwIndex, LPSTR lpName, LPDWORD lpcchName, LPDWORD, LPSTR, LPDWORD,
                          PFILETIME)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    load();
    std::string prefix = root_name(hKey) + "\\";
    DWORD i = 0;
    for (auto &k : s_keys) {
        if (k.first.size() <= prefix.size() || strncasecmp(k.first.c_str(), prefix.c_str(), prefix.size()))
            continue;
        std::string rest = k.first.substr(prefix.size());
        if (rest.find('\\') != std::string::npos)
            continue;
        if (i++ == dwIndex) {
            if (*lpcchName <= rest.size())
                return ERROR_MORE_DATA;
            strcpy(lpName, rest.c_str());
            *lpcchName = (DWORD)rest.size();
            return ERROR_SUCCESS;
        }
    }
    return ERROR_NO_MORE_ITEMS;
}

LONG WINAPI RegEnumValueA(HKEY hKey, DWORD dwIndex, LPSTR lpValueName, LPDWORD lpcchValueName, LPDWORD,
                          LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
    std::lock_guard<std::recursive_mutex> lock(s_mu);
    load();
    auto k = s_keys.find(root_name(hKey));
    if (k == s_keys.end())
        return ERROR_NO_MORE_ITEMS;
    DWORD i = 0;
    for (auto &v : k->second) {
        if (i++ != dwIndex)
            continue;
        if (*lpcchValueName <= v.first.size())
            return ERROR_MORE_DATA;
        strcpy(lpValueName, v.first.c_str());
        *lpcchValueName = (DWORD)v.first.size();
        if (lpType)
            *lpType = v.second.type;
        if (lpcbData) {
            DWORD n = (DWORD)v.second.data.size();
            if (lpData && *lpcbData >= n)
                memcpy(lpData, v.second.data.data(), n);
            *lpcbData = n;
        }
        return ERROR_SUCCESS;
    }
    return ERROR_NO_MORE_ITEMS;
}

LONG WINAPI RegFlushKey(HKEY) { return ERROR_SUCCESS; }

}  // extern "C"
