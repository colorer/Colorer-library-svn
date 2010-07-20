#ifndef _HRC_SETTINGS_H_
#define _HRC_SETTINGS_H_

#include<xml/xmldom.h>
#include<colorer/parsers/helpers/FileTypeImpl.h>
#include<colorer/HRCParser.h>
#include<colorer/ParserFactory.h>

const wchar_t FarCatalogXml[]=L"\\base\\catalog.xml";

StringBuffer *GetPluginPath();

class HRCSettingsException : public Exception{
public:
  HRCSettingsException(){};
  HRCSettingsException(const String& msg){
    message->append(DString("HRCSettingsException: ")).append(msg);
  };
};

class HRCSettings
{
  friend class FileTypeImpl;
public:
  HRCSettings(ParserFactory *_parserFactory){parserFactory=_parserFactory;}
  void readXML(String *file);
  void readProfile();
  void readUserProfile(String *userProfile);

private:
  void UpdatePrototype(Element *elem);
  ParserFactory *parserFactory;


};
#endif