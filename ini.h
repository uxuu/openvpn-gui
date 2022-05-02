/*
 *  OpenVPN-GUI -- A Windows GUI for OpenVPN.
 *
 *  Copyright (C) 2004 Mathias Sundman <mathias@nilings.se>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program (see the file COPYING included with this
 *  distribution); if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INI_CONFIG_H
#define INI_CONFIG_H

BOOL SetStandalone(void);
int GetIniKeys(void);
int SaveIniKeys(void);
LONG GetIniValue(const TCHAR *section, const TCHAR *key, TCHAR *data, DWORD len);
LONG GetIniValueNumeric(const TCHAR *section, const TCHAR *key, DWORD *data);
LONG GetIniValueBinary(const TCHAR *section, const TCHAR *key, BYTE *data, DWORD len);
int SetIniValue(const TCHAR *section, const TCHAR *key, const TCHAR *data);
int SetIniValueNumeric(const TCHAR *section, const TCHAR *key, DWORD data);
int SetConfigIniValueBinary(const WCHAR *config_name, const WCHAR *name, const BYTE *data, DWORD len);
int SetIniValueBinary(const TCHAR *section, const TCHAR *key, const BYTE *data, DWORD len);
int DeleteIniValue(const TCHAR *section, const WCHAR *key);
int DeleteIniSection(const TCHAR *section);

#endif

