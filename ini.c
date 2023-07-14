#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <windows.h>
#include <tchar.h>
#include <shlobj.h>
#include <shlwapi.h>

#include "main.h"
#include "options.h"
#include "openvpn-gui-res.h"
#include "ini.h"
#include "localization.h"

#define BUFFER_SIZE 32768

extern options_t o;

extern struct regkey_str {
    const WCHAR *name;
    WCHAR *var;
    int len;
    const WCHAR *value;
} regkey_str[3];

extern struct regkey_int {
    const WCHAR *name;
    DWORD *var;
    DWORD value;
} regkey_int[13];

static int
bin2hex(const BYTE *data, int datalen, TCHAR *buf, int buflen)
{
    int m, n;
    TCHAR dict[] = _T("0123456789ABCDEF");
    int buflen2 = buflen >> 1 << 1;
    if(buflen == buflen2)
        buflen2 = buflen - 2;
    for(m = 0, n = 0; m < datalen && n < buflen2; m++, n++)
      {
        buf[n] = dict[data[m] >> 4 & 0x0F];
        buf[++n] = dict[data[m] & 0x0F];
      }
    buf[n] = _T('\0');
    return n;
}

static int
hex2bin(const TCHAR *data, int datalen, BYTE *buf, int buflen)
{
    int m, n;
    BYTE t, odd = 0;
    TCHAR *p;
    TCHAR dict[] = _T("0123456789ABCDEFabcdef");
    for(m = 0, n = 0; m < datalen && n < buflen; m++)
      {
        if(data[m] == _T('\0'))
            break;
        if(data[m] == _T(' '))
            continue;
        p = _tcschr(dict, data[m]);
        if((p = _tcschr(dict, data[m])))
            t = p - dict;
        else
          {
            PrintDebug(L"unknown hex char: %lc = 0x%04X", data[m], data[m]);
            continue;
          }
        if(t > 0x0F)
            t -= 6;
        if(odd)
            buf[n++] |= t;
        else
            buf[n] = t << 4;
        odd = !odd;
      }
    return n;
}

static int
IniValueExists (const TCHAR *section, const TCHAR *key)
{
    TCHAR buf[4];
    if(GetPrivateProfileString(section, key, NULL, buf, 4, o.ini_path) == 0)
        if(GetPrivateProfileString(section, key, _T("1"), buf, 4, o.ini_path) == 1)
            return false;
    return  true;
}

static int
IniSectionExists(const TCHAR *section)
{
    TCHAR buf[4];
    return GetPrivateProfileSection(section, buf, 3, o.ini_path);
}

BOOL SetStandalone()
{
    GetModuleFileName(NULL, o.ini_path, _countof(o.ini_path));
    PathRenameExtension(o.ini_path, _T(".ini"));
    o.standalone = GetIniValueNumeric(_T("global"), _T("standalone"), NULL);
    PrintDebug(L"o.ini_path: %ls, o.standalone: %lu", o.ini_path, o.standalone);

    for (int i = 0 ; i < (int) _countof (regkey_int); ++i)
    {
        if (!_tcscmp(_T("iservice_admin"), regkey_int[i].name))
            regkey_int[i].value = 0;
    }
    return o.standalone;
}

static int
GetGlobalIniKeys()
{
  TCHAR windows_dir[MAX_PATH];
  TCHAR openvpn_path[MAX_PATH];

  if (!GetWindowsDirectory(windows_dir, _countof(windows_dir))) {
    /* can't get windows dir */
    ShowLocalizedMsg(IDS_ERR_GET_WINDOWS_DIR);
    /* Use a default value */
    _sntprintf_0(windows_dir, L"C:\\Windows");
  }

  /* set default editor and log_viewer as a fallback for opening config/log files */
  _sntprintf_0(o.editor, L"%ls\\%ls", windows_dir, L"System32\\notepad.exe");
  _sntprintf_0(o.log_viewer, L"%ls", o.editor);
  GetModuleFileName(NULL, openvpn_path, MAX_PATH);
  TCHAR *p = _tcsrchr(openvpn_path, _T('\\'));
  *p = _T('\0');
  p = _tcsrchr(openvpn_path, _T('\\'));
  if(!_tcsnicmp(p, _T("\\bin"), _countof(_T("\\bin"))))
    {
      *p = _T('\0');
    }
  if (openvpn_path[_tcslen(openvpn_path) - 1] != _T('\\'))
    _tcscat(openvpn_path, _T("\\"));

  _sntprintf_0(o.global_config_dir, _T("%lsconfig"), openvpn_path);
  _sntprintf_0(o.config_dir, _T("%lsconfig"), openvpn_path);
  _sntprintf_0(o.log_dir, _T("%lslog"), openvpn_path);

  if (!GetIniValue(_T("global"), _T("ovpn_admin_group"), o.ovpn_admin_group, _countof(o.ovpn_admin_group)))
    {
      _tcsncpy(o.ovpn_admin_group, OVPN_ADMIN_GROUP, _countof(o.ovpn_admin_group)-1);
    }

  _sntprintf_0(o.exe_path, _T("%lsbin\\openvpn.exe"), openvpn_path);

  if (!GetIniValue(_T("global"), _T("priority"), o.priority_string, _countof(o.priority_string)))
    {
      _tcsncpy(o.priority_string, _T("NORMAL_PRIORITY_CLASS"), _countof(o.priority_string)-1);
    }
  if (!GetIniValueNumeric(_T("global"), _T("disable_save_passwords"), &o.disable_save_passwords))
    {
      o.disable_save_passwords = 0;
    }
  return true;
}

int
GetIniKeys ()
{
    int i;

    if (!GetGlobalIniKeys())
        return false;


    if(!GetIniValue (_T("user"), regkey_str[1].name, regkey_str[1].var, regkey_str[1].len))
    {
        wcsncpy (regkey_str[1].var, regkey_str[1].value, regkey_str[1].len);
        regkey_str[1].var[regkey_str[1].len-1] = L'\0';
        PrintDebug(L"default: %ls = %ls", regkey_str[1].name, regkey_str[1].var);
    }

    for (i = 0 ; i < (int) _countof (regkey_int); ++i)
    {
        if (!GetIniValueNumeric (_T("user"), regkey_int[i].name, regkey_int[i].var))
        {
            /* no value found in config, use the default */
            *regkey_int[i].var = regkey_int[i].value;
            PrintDebug(L"default: %ls = %lu", regkey_int[i].name, *regkey_int[i].var);
        }
        else
            PrintDebug(L"from config: %ls = %lu", regkey_int[i].name, *regkey_int[i].var);
    }

    if ((o.disconnectscript_timeout == 0))
    {
        ShowLocalizedMsg(IDS_ERR_DISCONN_SCRIPT_TIMEOUT);
        o.disconnectscript_timeout = 10;
    }
    if ((o.preconnectscript_timeout == 0))
    {
        ShowLocalizedMsg(IDS_ERR_PRECONN_SCRIPT_TIMEOUT);
        o.preconnectscript_timeout = 10;
    }
    if (o.mgmt_port_offset < 1 || o.mgmt_port_offset > 61000)
    {
        o.mgmt_port_offset = 25340;
    }

    ExpandOptions ();
    return true;
}

int
SaveIniKeys ()
{
    int i;
    int ret = false;

    if ( CompareStringExpanded (regkey_str[1].var, regkey_str[1].value) != 0 ||
         IniValueExists(_T("user"), regkey_str[1].name) )
    {
        if (!SetIniValue (_T("user"), regkey_str[1].name, regkey_str[1].var))
            goto out;
    }

    for (i = 0 ; i < (int) _countof (regkey_int); ++i)
    {
        if ( *regkey_int[i].var != regkey_int[i].value ||
            IniValueExists(_T("user"), regkey_int[i].name))
        {
            if (!SetIniValueNumeric (_T("user"), regkey_int[i].name, *regkey_int[i].var))
                goto out;
        }
    }
    ret = true;

out:
    return ret;
}

static BOOL
GetIniVersion (version_t *v)
{
    return GetIniValueBinary(_T("user"), _T("version"), (BYTE *)v, sizeof(*v));
}

static BOOL
SetIniVersion (const version_t *v)
{
    return SetIniValueBinary(_T("user"), _T("version"), (BYTE *)v, sizeof(*v));
}

LONG GetIniValue(const TCHAR *section, const TCHAR *key, TCHAR *data, DWORD len)
{
  return GetPrivateProfileString(section, key, NULL, data, len, o.ini_path);
}

LONG
GetIniValueNumeric(const TCHAR *section, const TCHAR *key, DWORD *data)
{
  DWORD ret = GetPrivateProfileInt(section, key, 0, o.ini_path);
  if(data)
    *data = ret;
  return ret;
}

LONG GetIniValueBinary(const TCHAR *section, const TCHAR *key, BYTE *data, DWORD len)
{
    TCHAR buf[BUFFER_SIZE];
    DWORD hexlen;
    if((hexlen = GetPrivateProfileString(section, key, NULL, buf, BUFFER_SIZE, o.ini_path)))
      {
        if(!data && ! len)
            return (hexlen / 2);
        return hex2bin(buf, hexlen, data, len);
      }
    return 0;
}

int SetIniValue(const TCHAR *section, const TCHAR *key, const TCHAR *data)
{
  return WritePrivateProfileString(section, key, data, o.ini_path);
}

int
SetIniValueNumeric(const TCHAR *section, const TCHAR *key, DWORD data)
{
    TCHAR buf[MAX_PATH];
    _sntprintf_0(buf, _T("%ld"), data);
    return WritePrivateProfileString(section, key, buf, o.ini_path);
}

int
SetIniValueBinary(const TCHAR *section, const TCHAR *key, const BYTE *data, DWORD len)
{
    TCHAR buf[BUFFER_SIZE];
    if(bin2hex(data, len, buf, BUFFER_SIZE))
        return WritePrivateProfileString(section, key, buf, o.ini_path);
    return 0;
}

int
DeleteIniValue(const TCHAR *section, const WCHAR *key)
{
    return WritePrivateProfileString(section, key, NULL, o.ini_path);
}

int
DeleteIniSection(const TCHAR *section)
{
    return WritePrivateProfileString(section, NULL, NULL, o.ini_path);
}
