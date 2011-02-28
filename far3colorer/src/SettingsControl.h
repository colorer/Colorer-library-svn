#ifndef _SETTINGSCONTROL_H_
#define _SETTINGSCONTROL_H_

#include"pcolorer.h"

class SettingsControl
{
public:
  SettingsControl();
  ~SettingsControl();
  
  DWORD rGetValueDw(size_t Root, const wchar_t *name, DWORD DefaultValue);
  const wchar_t *rGetValueSz(size_t Root, const wchar_t *name, const wchar_t *DefaultValue);
  bool rSetValueDw(size_t Root, const wchar_t *name, DWORD val);
  bool rSetValueSz(size_t Root, const wchar_t *name, const wchar_t *val);
  int rGetSubKey(size_t Root, const wchar_t *name);
  bool rEnum(size_t Root, FarSettingsEnum *fse);
  bool rDeleteSubKey(size_t Root,const wchar_t *name);

private:
  HANDLE farSettingHandle;



};

#endif 