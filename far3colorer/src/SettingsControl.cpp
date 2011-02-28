#include "SettingsControl.h"

SettingsControl::SettingsControl()
{
  FarSettingsCreate fsc;
  fsc.Guid = MainGuid;
  fsc.StructSize=sizeof(FarSettingsCreate);
  if (Info.SettingsControl(INVALID_HANDLE_VALUE, SCTL_CREATE, NULL, (INT_PTR)&fsc)){
    farSettingHandle = fsc.Handle;
  }
  else{
    //error
  }

}

SettingsControl::~SettingsControl()
{
  Info.SettingsControl(farSettingHandle, SCTL_FREE, NULL, NULL);
}

DWORD SettingsControl::rGetValueDw(size_t Root, const wchar_t *name, DWORD DefaultValue)
{
  FarSettingsItem fsi;
  fsi.Root = Root;
  fsi.Type = FST_QWORD;
  fsi.Name = name;

  if (Info.SettingsControl(farSettingHandle, SCTL_GET, NULL, (INT_PTR)&fsi)){
    return fsi.Number;
  }
  else{
    return DefaultValue;
  }
}

const wchar_t *SettingsControl::rGetValueSz(size_t Root, const wchar_t *name, const wchar_t *DefaultValue)
{
  FarSettingsItem fsi;
  fsi.Root = Root;
  fsi.Type = FST_STRING;
  fsi.Name = name;

  if (Info.SettingsControl(farSettingHandle, SCTL_GET, NULL, (INT_PTR)&fsi)){
    return fsi.String;
  }
  else{
    return DefaultValue;
  }

}

bool SettingsControl::rSetValueDw(size_t Root, const wchar_t *name, DWORD val)
{
  FarSettingsItem fsi;
  fsi.Root = Root;
  fsi.Type = FST_QWORD;
  fsi.Name = name;
  fsi.Number = val;

  return !!Info.SettingsControl(farSettingHandle, SCTL_SET, NULL, (INT_PTR)&fsi);

}

bool SettingsControl::rSetValueSz(size_t Root, const wchar_t *name, const wchar_t *val)
{
  FarSettingsItem fsi;
  fsi.Root = Root;
  fsi.Type = FST_STRING;
  fsi.Name = name;
  fsi.String = val;

  return !!Info.SettingsControl(farSettingHandle, SCTL_SET, NULL, (INT_PTR)&fsi);

}

int SettingsControl::rGetSubKey(size_t Root, const wchar_t *name)
{

  FarSettingsValue fsv;
  fsv.Root = Root;
  fsv.Value = name;
  
  return Info.SettingsControl(farSettingHandle, SCTL_SUBKEY, NULL, (INT_PTR)&fsv);

}

bool SettingsControl::rEnum(size_t Root, FarSettingsEnum *fse)
{
  fse->Root = Root;
  return !!Info.SettingsControl(farSettingHandle, SCTL_ENUM, NULL, (INT_PTR)fse);
}

bool SettingsControl::rDeleteSubKey(size_t Root,const wchar_t *name)
{
  FarSettingsValue fsv;
  fsv.Root = Root;
  fsv.Value = name;
  
  return !!Info.SettingsControl(farSettingHandle, SCTL_DELETE, NULL, (INT_PTR)&fsv);
}