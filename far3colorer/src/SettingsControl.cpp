#include "SettingsControl.h"

SettingsControl::SettingsControl()
{
  FarSettingsCreate fsc;
  fsc.Guid = MainGuid;
  fsc.StructSize=sizeof(FarSettingsCreate);
  if (Info.SettingsControl(INVALID_HANDLE_VALUE, SCTL_CREATE, NULL, &fsc)){
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

const wchar_t *SettingsControl::Get(size_t Root, const wchar_t *Name, const wchar_t *Default)
{
  FarSettingsItem item={Root,Name,FST_STRING};
  if (Info.SettingsControl(farSettingHandle,SCTL_GET,0,&item))
  {
    return item.String;
  }
  return Default;
}

unsigned __int64 SettingsControl::Get(size_t Root, const wchar_t *Name, unsigned __int64 Default)
{
  FarSettingsItem item={Root,Name,FST_QWORD};
  if (Info.SettingsControl(farSettingHandle,SCTL_GET,0,&item))
  {
    return item.Number;
  }
  return Default;
}

bool SettingsControl::Set(int Root, const wchar_t *Name, const wchar_t *Value)
{
  FarSettingsItem item={Root,Name,FST_STRING};
  item.String=Value;
  return Info.SettingsControl(farSettingHandle,SCTL_SET,0,&item)!=FALSE;
}

bool SettingsControl::Set(int Root, const wchar_t *Name, unsigned __int64 Value)
{
  FarSettingsItem item={Root,Name,FST_QWORD};
  item.Number=Value;
  return Info.SettingsControl(farSettingHandle,SCTL_SET,0,&item)!=FALSE;
}

int SettingsControl::rGetSubKey(size_t Root, const wchar_t *Name)
{
  FarSettingsValue fsv={Root,Name};
  return Info.SettingsControl(farSettingHandle, SCTL_CREATESUBKEY, NULL, &fsv);
}

bool SettingsControl::rEnum(size_t Root, FarSettingsEnum *fse)
{
  fse->Root = Root;
  return !!Info.SettingsControl(farSettingHandle, SCTL_ENUM, NULL, fse);
}

bool SettingsControl::rDeleteSubKey(size_t Root,const wchar_t *Name)
{
  FarSettingsValue fsv={Root,Name};  
  return !!Info.SettingsControl(farSettingHandle, SCTL_DELETE, NULL, &fsv);
}