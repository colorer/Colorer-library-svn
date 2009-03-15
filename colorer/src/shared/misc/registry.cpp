#include<windows.h>

LONG rSetValue(HKEY hReg, const char *VName, DWORD val)
{
  return RegSetValueEx(hReg, VName, 0, REG_DWORD, (UCHAR*)(&val), 4);
};

LONG rSetValue(HKEY hReg, const char *VName, DWORD Type, const char *Data, DWORD Len)
{
  return RegSetValueEx(hReg, VName, 0, Type, (const UCHAR*)Data, Len);
};

DWORD rGetValue(HKEY hReg, char *name)
{
  DWORD data = 0;
  DWORD i = 4;
  RegQueryValueEx(hReg, name, 0, NULL, (PBYTE)&data, &i);
  return data;
};

DWORD rGetValue(HKEY hReg, const char *name, char *Data, DWORD Len)
{
  DWORD i = RegQueryValueEx(hReg, name, 0, NULL, (PBYTE)Data, &Len);
  return i?0:Len;
};

HKEY rOpenKey(HKEY hReg, const char *Name)
{
HKEY hKey = 0;
  RegCreateKeyEx(hReg, Name, 0, NULL, REG_OPTION_NON_VOLATILE,
    KEY_ALL_ACCESS, NULL, &hKey, NULL);
  return hKey;
};

HKEY rOpenKeyEx(HKEY hReg, const char *Name)
{
HKEY hKey = 0;
  RegOpenKeyEx(hReg, Name, 0, KEY_ALL_ACCESS, &hKey);
  return hKey;
};

DWORD rCheckAndSet(HKEY hReg, const char *Name, DWORD Val)
{
DWORD i;
  if (RegQueryValueEx(hReg, Name, 0, NULL, NULL, &i)){
    rSetValue(hReg,Name,Val);
    return TRUE;
  };
  return FALSE;
};

DWORD rCheckAndSet(HKEY hReg, const char *Name, DWORD Type, char* Data, DWORD Len)
{
DWORD i;
  if (RegQueryValueEx(hReg,Name,0,NULL,NULL,&i)){
    rSetValue(hReg,Name,Type,Data,Len);
    return TRUE;
  };
  return FALSE;
};
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Colorer Library.
 *
 * The Initial Developer of the Original Code is
 * Cail Lomecb <cail@nm.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
