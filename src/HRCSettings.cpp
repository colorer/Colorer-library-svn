#include"HRCSettings.h"
#include<windows.h>


void HRCSettings::readProfile()
{
  wchar_t cname[256];
  HMODULE hmod;
  hmod = GetModuleHandle(TEXT("colorer"));
  if (hmod == null) hmod = GetModuleHandle(null);
  int len = GetModuleFileName(hmod, cname, 256) - 1;
  DString module(cname, 0, len);
  int pos;
  pos = module.lastIndexOf('\\');
  StringBuffer *path=new StringBuffer(DString(module, 0, pos));
  path->append(DString("\\profile.xml"));

  DocumentBuilder docbuilder;
  InputSource *dfis = InputSource::newInstance(path);
  Document *xmlDocument = docbuilder.parse(dfis);
  Element *types = xmlDocument->getDocumentElement();

  if (*types->getNodeName() != "hrc-settings"){
    docbuilder.free(xmlDocument);
    throw HRCSettingsException(DString("main '<hrc-settings>' block not found"));
  }
  for (Node *elem = types->getFirstChild(); elem; elem = elem->getNextSibling()){
    if (*elem->getNodeName() == "prototype"){
      UpdatePrototype((Element*)elem);
      continue;
    }
  };
  docbuilder.free(xmlDocument);

}

void HRCSettings::UpdatePrototype(Element *elem)
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

      if (type->getParamDefaultValue(SString(name))==null){
        type->addParam(name);
      }
      type->setParamDefaultValue(SString(name), value);
    };
  };

}