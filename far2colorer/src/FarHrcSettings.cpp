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

  // перебираем все разделы в HrcSettings
  while(RegEnumKeyEx(dwKey, dwKeyIndex++, szNameOfKey, &dwBufferSize, NULL, NULL, NULL,NULL)==  ERROR_SUCCESS)
  {
    //проверяем, есть ли у нас такая схема 
    FileTypeImpl *type = (FileTypeImpl *)hrcParser->getFileType(&DString(szNameOfKey));
    if (type== null){
      //восстанавливаем значение размера буфера
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
      // перебираем все параметры в разделе
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

        //восстанавливаем значение размера буфера
        dwNameOfValueBufferSize=MAX_VALUE_NAME;
        dwValueIndex++;
      }
    }
    RegCloseKey(hkKey);

    //восстанавливаем значение размера буфера
    dwBufferSize=MAX_KEY_LENGTH;
  }

}