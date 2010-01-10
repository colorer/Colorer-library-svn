#ifndef __REGISTRY_WIDE_H__
#define __REGISTRY_WIDE_H__

#include <windows.h>

LONG rSetValue(HKEY hReg, const wchar_t *VName, DWORD val);
LONG rSetValue(HKEY hReg, const wchar_t *VName, DWORD Type, const void *Data, DWORD Len);
DWORD rGetValue(HKEY hReg, wchar_t *name);
DWORD rGetValue(HKEY hReg, const wchar_t *name, wchar_t *Data, DWORD Len);
HKEY rOpenKey(HKEY hReg, const wchar_t *Name);
HKEY rOpenKeyEx(HKEY hReg, const wchar_t *Name);
DWORD rCheckAndSet(HKEY hReg, const wchar_t *Name, DWORD Val);
DWORD rCheckAndSet(HKEY hReg, const wchar_t *Name, DWORD Type, wchar_t* Data, DWORD Len);

DWORD rGetValueSz(HKEY hReg, const wchar_t *name, wchar_t *&Data);
DWORD rGetValueDw(HKEY hReg, wchar_t *name);

#endif
