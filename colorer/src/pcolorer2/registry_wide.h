#ifndef __REGISTRY_WIDE_H__
#define __REGISTRY_WIDE_H__

#include <windows.h>

DWORD rOpenKey(HKEY hReg, const wchar_t *Name, HKEY &hKey);
LONG rSetValue(HKEY hReg, const wchar_t *VName, DWORD val);
LONG rSetValue(HKEY hReg, const wchar_t *VName, DWORD Type, const void *Data, DWORD Len);
wchar_t *rGetValueSz(HKEY hReg, const wchar_t *name, const wchar_t *DefaultValue);
DWORD rGetValueDw(HKEY hReg, const wchar_t *name, DWORD DefaultValue);

#endif
