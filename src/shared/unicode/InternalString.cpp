#include "InternalString.h"

void InternalString::init()
{
  len = 0;
  wstr = null;
  cstring = null;
}

InternalString::~InternalString()
{
  delete[] wstr;
}

InternalString::InternalString(const String *str)
{
  if (str!=null)
  {
    len = str->length();
    wstr = new wchar[len];
    wmemcpy(wstr,str->getWChars(),len);
    cstring =(String*) str;
  }
  else init();
}

int InternalString::compareTo(size_t Pos, size_t Len, const wchar_t* Data, size_t DataLen) const
{
  size_t i;
  for(i = 0; i < DataLen && i < Len; i++){
    int cmp = Data[i] - this->wstr[Pos+i];
    if (cmp > 0) return -1;
    if (cmp < 0) return 1;
  };
  if (i < DataLen) return -1;
  if (i < Len) return 1;
  return 0;
}

int InternalString::compareToIgnoreCase(size_t Pos, size_t Len, const wchar_t* Data, size_t DataLen) const
{
  size_t i;
  for(i = 0; i < DataLen && i < Len; i++){
    int cmp = Character::toLowerCase(Data[i]) - Character::toLowerCase(this->wstr[Pos+i]);
    if (cmp > 0) return -1;
    if (cmp < 0) return 1;
  };
  if (i < DataLen) return -1;
  if (i < Len) return 1;
  return 0;
}

bool InternalString::equalsIgnoreCase(size_t Pos, size_t Len, const wchar_t* Data, size_t DataLen)  const
{
  if (DataLen != Len) return false;
  for(size_t i = 0; i < DataLen; i++)
    if (Character::toLowerCase(Data[i]) != Character::toLowerCase(this->wstr[Pos+i]) ||
      Character::toUpperCase(Data[i]) != Character::toUpperCase(this->wstr[Pos+i])) return false;
  return true;
}