#include"FarHrcSettings.h"

void FarHrcSettings::readProfile()
{
  StringBuffer *path=new StringBuffer(PluginPath);
  path->append(DString(FarProfileXml));
  readXML(path, false);

  delete path;
}

void FarHrcSettings::readXML(String *file, bool userValue)
{
  DocumentBuilder docbuilder;
  InputSource *dfis = InputSource::newInstance(file);
  Document *xmlDocument = docbuilder.parse(dfis);
  Element *types = xmlDocument->getDocumentElement();

  if (*types->getNodeName() != "hrc-settings"){
    docbuilder.free(xmlDocument);
    throw FarHrcSettingsException(DString("main '<hrc-settings>' block not found"));
  }
  for (Node *elem = types->getFirstChild(); elem; elem = elem->getNextSibling()){
    if (*elem->getNodeName() == "prototype"){
      UpdatePrototype((Element*)elem, userValue);
      continue;
    }
  };
  docbuilder.free(xmlDocument);
}

void FarHrcSettings::UpdatePrototype(Element *elem, bool userValue)
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
      const String *descr = ((Element*)content)->getAttribute(DString("description"));
      if (name == null || value == null){
        continue;
      };

      if (type->getParamValue(SString(name))==null){
        type->addParam(name);
      }
      if (descr != null){
        type->setParamDescription(SString(name), descr);
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

void FarHrcSettings::readUserProfile()
{
  wchar_t key[MAX_KEY_LENGTH];
  _snwprintf(key,MAX_KEY_LENGTH, L"%s\\colorer\\HrcSettings", Info.RootKey);
  HKEY dwKey;

  if (RegOpenKeyEx( HKEY_CURRENT_USER, key, 0, KEY_READ, &dwKey) == ERROR_SUCCESS ){
    readProfileFromRegistry(dwKey);
  }

  RegCloseKey(dwKey);
}

void FarHrcSettings::readProfileFromRegistry(HKEY dwKey)
{
  DWORD dwKeyIndex=0;
  wchar_t szNameOfKey[MAX_KEY_LENGTH]; 
  DWORD dwBufferSize=MAX_KEY_LENGTH;

  HRCParser *hrcParser = parserFactory->getHRCParser();

  // enum all the sections in HrcSettings
  while(RegEnumKeyEx(dwKey, dwKeyIndex++, szNameOfKey, &dwBufferSize, NULL, NULL, NULL,NULL)==  ERROR_SUCCESS)
  {
    //check whether we have such a scheme
    FileTypeImpl *type = (FileTypeImpl *)hrcParser->getFileType(&DString(szNameOfKey));
    if (type== null){
      //restore buffer size value
      dwBufferSize=MAX_KEY_LENGTH;
      continue;
    };

    wchar_t key[MAX_KEY_LENGTH];
    HKEY hkKey;

    _snwprintf(key,MAX_KEY_LENGTH, L"%s\\colorer\\HrcSettings\\%s", Info.RootKey,szNameOfKey);

    if (RegOpenKeyEx( HKEY_CURRENT_USER, key, 0, KEY_READ, &hkKey) == ERROR_SUCCESS ){

      DWORD dwValueIndex=0;
      wchar_t szNameOfValue[MAX_VALUE_NAME]; 
      DWORD dwNameOfValueBufferSize=MAX_VALUE_NAME;
      DWORD dwValueBufferSize;
      DWORD nValueType;
      // enum all params in the section
      while(RegEnumValue(hkKey, dwValueIndex, szNameOfValue, &dwNameOfValueBufferSize, NULL, &nValueType, NULL ,&dwValueBufferSize)==  ERROR_SUCCESS)
      { 
        if (nValueType==REG_SZ){
          
          wchar_t *szValue= new wchar_t[(dwValueBufferSize / sizeof(wchar_t))+1];
          if (RegQueryValueExW(hkKey, szNameOfValue, 0, NULL, (PBYTE)szValue, &dwValueBufferSize)==  ERROR_SUCCESS){
            if (type->getParamValue(SString(szNameOfValue))==null){
              type->addParam(&SString(szNameOfValue));
            }
            type->setParamValue(SString(szNameOfValue), &SString(szValue));
          }
          delete [] szValue;
        }

        //restore buffer size value
        dwNameOfValueBufferSize=MAX_VALUE_NAME;
        dwValueIndex++;
      }
    }
    RegCloseKey(hkKey);

    //restore buffer size value
    dwBufferSize=MAX_KEY_LENGTH;
  }

}

void FarHrcSettings::writeUserProfile()
{
  wchar_t key[MAX_KEY_LENGTH];
  _snwprintf(key,MAX_KEY_LENGTH, L"%s\\colorer\\HrcSettings", Info.RootKey);
  HKEY dwKey;

  //create or open key
  if (RegCreateKeyEx(HKEY_CURRENT_USER, key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
            NULL, &dwKey, NULL)==ERROR_SUCCESS ){
    writeProfileToRegistry();
  }

  RegCloseKey(dwKey);
}

void FarHrcSettings::writeProfileToRegistry()
{
  HRCParser *hrcParser = parserFactory->getHRCParser();
  FileTypeImpl *type = NULL;

  // enum all FileTypes
  for (int idx = 0; ; idx++){
    type =(FileTypeImpl *) hrcParser->enumerateFileTypes(idx);

    if (!type){
      break;
    }

    const String *p, *v;
    if (type->getParamCount() && type->getParamNotDefaultValueCount()){// params>0 and user values >0
      // enum all params
      for (int i=0;;i++){
        p=type->enumerateParameters(i);
        if (!p){
          break;
        }
        v=type->getParamNotDefaultValue(*p);
        if (v!=NULL){
          wchar_t key[MAX_KEY_LENGTH];
          HKEY hkKey;

          _snwprintf(key,MAX_KEY_LENGTH, L"%s\\colorer\\HrcSettings\\%s", Info.RootKey,type->getName()->getWChars());

          if (RegCreateKeyEx(HKEY_CURRENT_USER, key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkKey, NULL)==ERROR_SUCCESS ){
              RegSetValueExW(hkKey, p->getWChars(), 0, REG_SZ, (const BYTE*)v->getWChars(), sizeof(wchar_t)*(v->length()+1));
          }
          RegCloseKey(hkKey);

        }
      }
    }
  }

}