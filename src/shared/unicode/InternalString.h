#ifndef _COLORER_INTERNALSTRING_H_
#define _COLORER_INTERNALSTRING_H_

#include "String.h"

class InternalString{
public:

  InternalString(const String *str);
  ~InternalString();

  void init();

  size_t length() const { return len;}
  wchar operator[](size_t i) const { return wstr[i];}
  wchar *getWChars() const { return wstr; }
  String *getString() const { return cstring;}

  int compareTo(size_t Pos, size_t Len, const wchar* Data, size_t DataLen) const;
  int compareTo(const InternalString* Str) const {return compareTo(0,len,Str->wstr,Str->len);}
  int compareTo(size_t Pos, size_t Len, const InternalString *Str) const { return compareTo(Pos, Len,Str->wstr,Str->len); }
  int compareTo(size_t Pos, size_t Len, const SString *Str) const { return compareTo(Pos, Len,Str->getWChars(), Str->length()); }
  int compareToIgnoreCase(size_t Pos, size_t Len, const wchar* Data, size_t DataLen) const;
  int compareToIgnoreCase(size_t Pos, size_t Len, const InternalString *Str) const { return compareToIgnoreCase(Pos, Len,Str->wstr,Str->len); }
  int compareToIgnoreCase(const InternalString* Str) const {return compareToIgnoreCase(0,len,Str->wstr,Str->len);}
  int compareToIgnoreCase(size_t Pos, size_t Len, const SString *Str) const { return compareToIgnoreCase(Pos, Len,Str->getWChars(), Str->length()); }
  bool equalsIgnoreCase(size_t Pos, size_t Len, const wchar_t* Data, size_t DataLen)  const;
  bool equalsIgnoreCase(size_t Pos, size_t Len, const SString *Str) const { return equalsIgnoreCase(Pos, Len,Str->getWChars(), Str->length()); }
  bool equalsIgnoreCase(size_t Pos, size_t Len, const String *Str) const { return equalsIgnoreCase(Pos, Len,Str->getWChars(), Str->length()); }

private:
  wchar *wstr;   //buffer for string
  size_t len;    //string length

  String* cstring; //old string , remove after full rebuild project
};


#endif
