#include<windows.h>

LONG rSetValue(HKEY hReg, char *VName, DWORD val)
{
  return RegSetValueEx(hReg, VName, 0, REG_DWORD, (UCHAR*)(&val), 4);
};

LONG rSetValue(HKEY hReg, char *VName, DWORD Type, char *Data, DWORD Len)
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

DWORD rGetValue(HKEY hReg, char *name, char *Data, DWORD Len)
{
  DWORD i = RegQueryValueEx(hReg, name, 0, NULL, (PBYTE)Data, &Len);
  return i?0:Len;
};

HKEY rOpenKey(HKEY hReg, char *Name)
{
HKEY hKey = 0;
  RegCreateKeyEx(hReg, Name, 0, NULL, REG_OPTION_NON_VOLATILE,
    KEY_ALL_ACCESS, NULL, &hKey, NULL);
  return hKey;
};

HKEY rOpenKeyEx(HKEY hReg, char *Name)
{
HKEY hKey = 0;
  RegOpenKeyEx(hReg, Name, 0, KEY_ALL_ACCESS, &hKey);
  return hKey;
};

DWORD rCheckAndSet(HKEY hReg, char *Name, DWORD Val)
{
DWORD i;
  if (RegQueryValueEx(hReg, Name, 0, NULL, NULL, &i)){
    rSetValue(hReg,Name,Val);
    return TRUE;
  };
  return FALSE;
};

DWORD rCheckAndSet(HKEY hReg, char *Name, DWORD Type, char* Data, DWORD Len)
{
DWORD i;
  if (RegQueryValueEx(hReg,Name,0,NULL,NULL,&i)){
    rSetValue(hReg,Name,Type,Data,Len);
    return TRUE;
  };
  return FALSE;
};
