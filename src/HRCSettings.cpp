#include"HRCSettings.h"
#include"pcolorer.h"


void HRCSettings::readProfile()
{
  StringBuffer *path=new StringBuffer(PluginPath);
  path->append(DString(FarProfileXml));
  readXML(path, false);

  delete path;
}

void HRCSettings::readUserProfile(String *userProfile)
{
  if (!userProfile || !userProfile->length()){
    StringBuffer *path=new StringBuffer(PluginPath);
    path->append(DString(FarUserProfileXml));
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