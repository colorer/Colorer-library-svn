#ifndef _FARHRCSETTINGS_H_
#define _FARHRCSETTINGS_H_

#include<xml/xmldom.h>
#include<colorer/parsers/helpers/FileTypeImpl.h>
#include<colorer/HRCParser.h>
#include<colorer/ParserFactory.h>

const wchar_t FarCatalogXml[]=L"\\base\\catalog.xml";
const wchar_t FarProfileXml[]=L"\\bin\\hrcsettings.xml";


class FarHrcSettingsException : public Exception{
public:
  FarHrcSettingsException(){};
  FarHrcSettingsException(const String& msg){
    message->append(DString("FarHrcSettingsException: ")).append(msg);
  };
};

class FarHrcSettings
{
  friend class FileTypeImpl;
public:
  FarHrcSettings(ParserFactory *_parserFactory){parserFactory=_parserFactory;}
  void readXML(String *file, bool userValue);
  void readProfile();

private:
  void UpdatePrototype(Element *elem, bool userValue);
  ParserFactory *parserFactory;

};


#endif _FARHRCSETTINGS_H_