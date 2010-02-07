#ifndef __REGISTRY_H__
#define __REGISTRY_H__

#include <windows.h>

LONG rSetValue(HKEY hReg, const char *VName, DWORD val);
LONG rSetValue(HKEY hReg, const char *VName, DWORD Type, const char *Data, DWORD Len);
DWORD rGetValue(HKEY hReg, char *name);
DWORD rGetValue(HKEY hReg, const char *name, char *Data, DWORD Len);
HKEY rOpenKey(HKEY hReg, const char *Name);
HKEY rOpenKeyEx(HKEY hReg, const char *Name);
DWORD rCheckAndSet(HKEY hReg, const char *Name, DWORD Val);
DWORD rCheckAndSet(HKEY hReg, const char *Name, DWORD Type, char* Data, DWORD Len);



#endif
