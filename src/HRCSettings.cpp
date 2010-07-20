#include"HRCSettings.h"
#include<windows.h>

StringBuffer *GetPluginPath()
{
  wchar_t cname[256];
  HMODULE hmod;
  hmod = GetModuleHandle(TEXT("colorer"));
  if (hmod == null) hmod = GetModuleHandle(null);
  int len = GetModuleFileName(hmod, cname, 256) - 1;
  DString module(cname, 0, len);
  int pos;
  pos = module.lastIndexOf('\\');
  pos = module.lastIndexOf('\\',pos);
  StringBuffer *path=new StringBuffer(DString(module, 0, pos));

  return path;
}

void HRCSettings::readProfile()
{
  StringBuffer *path=GetPluginPath();
  path->append(DString("\\bin\\profile.xml"));

  readXML(path, false);

  delete path;
}

void HRCSettings::readUserProfile(String *userProfile)
{
  if (!userProfile || !userProfile->length()){
    StringBuffer *path=GetPluginPath();
    path->append(DString("\\bin\\profile-user.xml"));

    readXML(path, true);
    delete path;
  }
  else{
    readXML(userProfile, true);
  }
}

void HRCSettings::readXML(String *file, bool userValue)
{
  DocumentBuilder docbuilder;
  InputSource *dfis = InputSource::newInstance(file);
  Document *xmlDocument = docbuilder.parse(dfis);
  Element *types = xmlDocument->getDocumentElement();

  if (*types->getNodeName() != "hrc-settings"){
    docbuilder.free(xmlDocument);
    throw HRCSettingsException(DString("main '<hrc-settings>' block not found"));
  }
  for (Node *elem = types->getFirstChild(); elem; elem = elem->getNextSibling()){
    if (*elem->getNodeName() == "prototype"){
      UpdatePrototype((Element*)elem, userValue);
      continue;
    }
  };
  docbuilder.free(xmlDocument);
}

void HRCSettings::UpdatePrototype(Element *elem, bool userValue)
{
  const String *typeName = elem->getAttribute(DString("name"));
  if (typeName == null){
    return;
  }
  HRCParser *hrcParser = parserFactory->getHRCParser();
  FileTypeImpl *type = (FileTypeImpl *)hrcParser->getFileType(typeName);
  if (type== null){
    return;
  };
  for(Node *content = elem->getFirstChild(); content != null; content = content->getNextSibling()){
    if (*content->getNodeName() == "param"){
      const String *name = ((Element*)content)->getAttribute(DString("name"));
      const String *value = ((Element*)content)->getAttribute(DString("value"));
      if (name == null || value == null){
        continue;
      };

      if (type->getParamValue(SString(name))==null){
        type->addParam(name);
      }
      if (userValue){
        type->setParamValue(SString(name), value);
      }
      else{
        type->setParamDefaultValue(SString(name), value);
      }
    };
  };

}