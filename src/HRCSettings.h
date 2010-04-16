#ifndef _HRC_SETTINGS_H_
#define _HRC_SETTINGS_H_

#include<xml/xmldom.h>
#include<colorer/parsers/helpers/FileTypeImpl.h>
#include<colorer/HRCParser.h>
#include<colorer/ParserFactory.h>

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
  void readProfile();

private:
  void UpdatePrototype(Element *elem);
  ParserFactory *parserFactory;


};
#endif