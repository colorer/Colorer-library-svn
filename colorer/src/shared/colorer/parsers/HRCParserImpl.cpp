
#include<stdio.h>
#include<colorer/parsers/helpers/HRCParserHelpers.h>
#include<colorer/parsers/HRCParserImpl.h>


HRCParserImpl::HRCParserImpl()
 : fileTypeHash(200), fileTypeVector(150), schemeHash(4000),
   regionNamesHash(1000), regionNamesVector(1000, 200)
{
  parseType = null;
  versionName = null;
  errorHandler = null;
  updateStarted = false;
};

HRCParserImpl::~HRCParserImpl()
{
  int idx;
  for(idx = 0; idx < fileTypeVector.size(); idx++)
    delete fileTypeVector.elementAt(idx);
  for(SchemeImpl *scheme = schemeHash.enumerate(); scheme; scheme = schemeHash.next()){
    delete scheme;
  };
  for(idx = 0; idx < regionNamesVector.size(); idx++)
    delete regionNamesVector.elementAt(idx);
  for(String *se = schemeEntitiesHash.enumerate(); se; se = schemeEntitiesHash.next()){
    delete se;
  };
  delete versionName;
};

void HRCParserImpl::setErrorHandler(ErrorHandler *eh){
  errorHandler = eh;
};

void HRCParserImpl::loadSource(InputSource *is){
  InputSource *istemp = curInputSource;
  curInputSource = is;
  if (is == null){
    if (errorHandler != null) errorHandler->error(StringBuffer("Can't open stream for type without location attribute"));
    return;
  };
  const byte *data = is->openStream();
  try{
    parseHRC(data, is->length());
  }catch(Exception &e){
    is->closeStream();
    throw e;
  };
  curInputSource = istemp;
};

void HRCParserImpl::loadFileType(FileType *filetype){
  if (filetype == null) return;

  FileTypeImpl *thisType = (FileTypeImpl*)filetype;
  if (thisType->typeLoaded || thisType->loadBroken) return;

  try{

    loadSource(thisType->inputSource);

  }catch(InputSourceException &e){
    if (errorHandler != null) errorHandler->fatalError(StringBuffer("Can't open source stream: ")+e.getMessage());
    thisType->loadBroken = true;
  }catch(HRCParserException &e){
    if (errorHandler != null) errorHandler->fatalError(StringBuffer(e.getMessage())+" ["+thisType->inputSource->getLocation()+"]");
    thisType->loadBroken = true;
  }catch(Exception &e){
    if (errorHandler != null) errorHandler->fatalError(StringBuffer(e.getMessage())+" ["+thisType->inputSource->getLocation()+"]");
    thisType->loadBroken = true;
  }catch(...){
    if (errorHandler != null) errorHandler->fatalError(StringBuffer("Unknown exception while loading ")+thisType->inputSource->getLocation());
    thisType->loadBroken = true;
  };
};


FileType *HRCParserImpl::chooseFileType(const String *fileName, const String *firstLine, int typeNo)
{
FileTypeImpl *best = null;
double max_prior = 0;
const double DELTA = 1e-6;
  for(int idx = 0; idx < fileTypeVector.size(); idx++){
    FileTypeImpl *ret = fileTypeVector.elementAt(idx);
    double prior = ret->getPriority(fileName, firstLine);

    if (typeNo > 0 && (prior-max_prior < DELTA)){
      best = ret;
      typeNo--;
    };
    if (prior-max_prior > DELTA || best == null){
      best = ret;
      max_prior = prior;
    };
  };
  if (typeNo > 0) return null;
  return best;
};

FileType *HRCParserImpl::getFileType(const String *name) {
  if (name == null) return null;
  return fileTypeHash.get(name);
};
FileType *HRCParserImpl::enumerateFileTypes(int index) {
  if (index < fileTypeVector.size()) return fileTypeVector.elementAt(index);
  return null;
};

int HRCParserImpl::getRegionCount() {
  return regionNamesVector.size();
};
const Region *HRCParserImpl::getRegion(int id) {
  return regionNamesVector.elementAt(id);
};
const Region* HRCParserImpl::getRegion(const String *name) {
  if (name == null) return null;
  return getNCRegion(name, false); // regionNamesHash.get(name);
};

const String *HRCParserImpl::getVersion() {
  return versionName;
};


// protected methods


void HRCParserImpl::parseHRC(const byte *data, int len)
{
  CXmlEl *xmlBase = new CXmlEl;
  xmlBase->parse(data, len);
  CXmlEl *types = xmlBase;
  while(types = types->next())
    if (!types || types->getType() == EL_BLOCKED && types->getName() && *types->getName() == "hrc") break;

  if (!types){
    delete xmlBase;
    throw HRCParserException(DString("main '<hrc>' block not found"));
  }else{
    if (versionName == null)
      versionName = new SString(types->getParamValue(DString("version")));
  };

  for (CXmlEl *elem = types->child(); elem; elem = elem->next()){
    if (!elem->getName()) continue;
    if (*elem->getName() == "prototype"){
      addPrototype(elem);
      continue;
    };
    if (*elem->getName() == "package"){
      addPrototype(elem);
      continue;
    };
    if (*elem->getName() == "type"){
      addType(elem);
      continue;
    };
  };
  delete xmlBase;
  updateLinks();
};


void HRCParserImpl::addPrototype(CXmlEl *elem)
{
  const String *typeName = elem->getParamValue(DString("name"));
  const String *typeGroup = elem->getParamValue(DString("group"));
  const String *typeDescription = elem->getParamValue(DString("description"));
  if (typeName == null){
    if (errorHandler != null) errorHandler->error(DString("unnamed prototype "));
    return;
  }
  if (typeDescription == null) typeDescription = typeName;
  if (fileTypeHash.get(typeName) != null){
    if (errorHandler != null) errorHandler->error(StringBuffer("Duplicate prototype '")+typeName+"'");
    return;
  };
  FileTypeImpl *type = new FileTypeImpl(this);
  type->name = new SString(typeName);
  type->description = new SString(typeDescription);
  if (typeGroup != null) type->group = new SString(typeGroup);
  if (*elem->getName() == "package") type->isPackage = true;

  for(CXmlEl *content = elem->child(); content != null; content = content->next()){
    if (*content->getName() == "location"){
      const String *locationLink = content->getParamValue(DString("link"));
      if (locationLink == null){
        if (errorHandler != null) errorHandler->error(StringBuffer("Bad 'location' link attribute in prototype '")+typeName+"'");
        continue;
      };
      type->inputSource = InputSource::newInstance(locationLink, curInputSource);
//      const String *typeLoad = content->getParamValue(DString("load"));
//      if (typeLoad != null && *typeLoad == "auto") autoLoad = true;
    };
    if (*content->getName() == "filename" || *content->getName() == "firstline"){
      if (content->child() == null || content->child()->getType() != EL_PLAIN){
        if (errorHandler != null) errorHandler->warning(StringBuffer("Bad '")+content->getName()+"' element in prototype '"+typeName+"'");
        continue;
      };
      const String *match = content->child()->getContent();
      CRegExp *matchRE = new CRegExp(match);
      matchRE->setPositionMoves(true);
      if (!matchRE->isOk()){
        if (errorHandler != null) errorHandler->warning(StringBuffer("Fault compiling chooser RE '")+match+"' in prototype '"+typeName+"'");
        delete matchRE;
        continue;
      };
      int ctype = *content->getName() == "filename" ? 0 : 1;
      double prior = *content->getName() == "filename" ? 2 : 1;
      content->getParamValue(DString("weight"), &prior);
      FileTypeChooser *ftc = new FileTypeChooser(ctype, prior, matchRE);
      type->chooserVector.addElement(ftc);
    };
    if (*content->getName() == "parameters"){
      for(CXmlEl *param = content->child(); param != null; param = param->next()){
        if (*param->getName() == "param"){
          const String *name = param->getParamValue(DString("name"));
          const String *value = param->getParamValue(DString("value"));
          if (name == null || value == null){
            if (errorHandler != null) errorHandler->warning(StringBuffer("Bad parameter in prototype '")+typeName+"'");
            continue;
          };
          type->parametersHash.put(name, new SString(value));
        };
      };
    };
  };

  type->protoLoaded = true;
  fileTypeHash.put(typeName, type);
  fileTypeVector.addElement(type);
//  if (autoLoad) loadFileType(type);
};

void HRCParserImpl::addType(CXmlEl *elem)
{
  const String *typeName = elem->getParamValue(DString("name"));
  const String *accessTypeName = elem->getParamValue(DString("access"));

  if (typeName == null){
    if (errorHandler != null) errorHandler->error(DString("Unnamed type found"));
    return;
  };
  FileTypeImpl *type_ref = fileTypeHash.get(typeName);
  if (type_ref == null){
    if (errorHandler != null) errorHandler->error(StringBuffer("type '")+typeName+"' without prototype");
    return;
  };
  FileTypeImpl *type = type_ref;
  if (type->typeLoaded){
    if (errorHandler != null) errorHandler->warning(StringBuffer("type '")+typeName+"' is already loaded");
    return;
  };
  type->accessType = SAT_PRIVATE;
  if (accessTypeName != null){
    if (*accessTypeName == "public") type->accessType = SAT_PUBLIC;
    else if (*accessTypeName != "private"){
      if (errorHandler != null) errorHandler->warning(StringBuffer("Unknown access specifier in type '")+typeName+"'");
    };
  };
  type->typeLoaded = true;

  FileTypeImpl *o_parseType = parseType;
  parseType = type;

  for(CXmlEl *xmlpar = elem->child(); xmlpar; xmlpar = xmlpar->next()){
    if (xmlpar->getName() == null) continue;
    if (*xmlpar->getName() == "region"){
      const String *regionName = xmlpar->getParamValue(DString("name"));
      const String *regionParent = xmlpar->getParamValue(DString("parent"));
      const String *regionDescr = xmlpar->getParamValue(DString("description"));
      if (regionName == null){
        if (errorHandler != null) errorHandler->error(DString("No 'name' attribute in <region> element"));
        continue;
      };
      String *qname1 = qualifyOwnName(regionName);
      if (qname1 == null) continue;
      String *qname2 = qualifyForeignName(regionParent, QNT_DEFINE, true);
      if (regionNamesHash.get(qname1) != null){
        if (errorHandler != null) errorHandler->warning(StringBuffer("Duplicate region '") + qname1 + "' definition in type '"+parseType->getName()+"'");
        continue;//
      };

      const Region *region = new Region(qname1, regionDescr, getRegion(qname2), regionNamesVector.size());
      regionNamesVector.addElement(region);
      regionNamesHash.put(qname1, region);

      delete qname1;
      delete qname2;
    };
    if (*xmlpar->getName() == "entity"){
      const String *entityName  = xmlpar->getParamValue(DString("name"));
      const String *entityValue = xmlpar->getParamValue(DString("value"));
      if (entityName == null || entityValue == null){
        if (errorHandler != null) errorHandler->error(DString("Bad entity attributes"));
        continue;
      };
      String *qname1 = qualifyOwnName(entityName);
      String *qname2 = useEntities(entityValue);
      if (qname1 != null && qname2 != null){
        schemeEntitiesHash.put(qname1, qname2);
        delete qname1;
      };
    };
    if (*xmlpar->getName() == "import"){
      const String *typeParam = xmlpar->getParamValue(DString("type"));
      if (typeParam == null || fileTypeHash.get(typeParam) == null){
        if (errorHandler != null) errorHandler->error(StringBuffer("Import with bad '")+typeParam+"' attribute in type '"+typeName+"'");
        continue;
      };
      type->importVector.addElement(new SString(typeParam));
    };
    if (*xmlpar->getName() == "scheme"){
      addScheme(xmlpar);
      continue;
    };
  };
  String *baseSchemeName = qualifyOwnName(type->name);
  if (baseSchemeName != null) type->baseScheme = schemeHash.get(baseSchemeName);
  delete baseSchemeName;
  if (type->baseScheme == null)
    if (errorHandler != null) errorHandler->warning(StringBuffer("type '")+typeName+"' has no default scheme");
  type->loadDone = true;
  parseType = o_parseType;
};

void HRCParserImpl::addScheme(CXmlEl *elem)
{
  const String *schemeName = elem->getParamValue(DString("name"));
  String *qSchemeName = qualifyOwnName(schemeName);
  if (qSchemeName == null){
    if (errorHandler != null) errorHandler->error(StringBuffer("bad scheme name in type '")+parseType->getName()+"'");
    return;
  };
  if (schemeHash.get(qSchemeName) != null){
    if (errorHandler != null) errorHandler->error(StringBuffer("duplicate scheme name '")+qSchemeName+"'");
    delete qSchemeName;
    return;
  };
  SchemeImpl *scheme = new SchemeImpl(qSchemeName);
  delete qSchemeName;
  scheme->fileType = parseType;

  const String *accessTypeName = elem->getParamValue(DString("access"));
  scheme->accessType = parseType->accessType;
  if (accessTypeName != null){
    scheme->accessType = SAT_PRIVATE;
    if (*accessTypeName == "public") scheme->accessType = SAT_PUBLIC;
    else if (*accessTypeName != "private"){
      if (errorHandler != null) errorHandler->warning(StringBuffer("unknown access type in scheme '")+scheme->schemeName+"'");
    };
  };
  schemeHash.put(scheme->schemeName, scheme);
  addSchemeNodes(scheme, elem->child());
};

void HRCParserImpl::addSchemeNodes(SchemeImpl *scheme, CXmlEl *elem)
{
  SchemeNode *next = null;
  for(CXmlEl *tmpel = elem; tmpel; tmpel = tmpel->next()){
    if (!tmpel->getName()) continue;

    if (next == null) next = new SchemeNode();

    if (*tmpel->getName() == "inherit"){
      const String *nqSchemeName = tmpel->getParamValue(DString("scheme"));
      next->type = SNT_INHERIT;
      next->schemeName = new SString(nqSchemeName);
      String *schemeName = qualifyForeignName(nqSchemeName, QNT_SCHEME, false);
      if (schemeName == null){
//        if (errorHandler != null) errorHandler->warning(StringBuffer("forward inheritance of '")+nqSchemeName+"'. possible inherit loop with '"+scheme->schemeName+"'");
//        delete next;
//        continue;
      }else
        next->scheme = schemeHash.get(schemeName);
      if (schemeName != null){
        delete next->schemeName;
        next->schemeName = schemeName;
      };

      if (tmpel->getType() == EL_BLOCKED){
        for(CXmlEl *vel = tmpel->child(); vel; vel = vel->next()){
          if (!vel->getName() || *vel->getName() != DString("virtual")) continue;
          const String *schemeName = vel->getParamValue(DString("scheme"));
          const String *substName = vel->getParamValue(DString("subst-scheme"));
          if (schemeName == null || substName == null){
            if (errorHandler != null) errorHandler->error(StringBuffer("bad virtualize attributes in scheme '")+scheme->schemeName+"'");
            continue;
          };
          next->virtualEntryVector.addElement(new VirtualEntry(schemeName, substName));
        };
      };
      scheme->nodes.addElement(next);
      next = null;
      continue;
    };

    if (*tmpel->getName() == "regexp"){
      const String *matchParam = tmpel->getParamValue(DString("match"));
      if (matchParam == null && tmpel->getType() == EL_BLOCKED
          && tmpel->child() && tmpel->child()->getType() == EL_PLAIN)
        matchParam = tmpel->child()->getContent();
      if (matchParam == null){
        if (errorHandler != null) errorHandler->error(StringBuffer("no 'match' in regexp in scheme ")+scheme->schemeName);
        delete next;
        continue;
      };
      String *entMatchParam = useEntities(matchParam);
      next->lowPriority = DString("low").equals(tmpel->getParamValue(DString("priority")));
      next->type = SNT_RE;
      next->start = new CRegExp(entMatchParam);
      next->start->setPositionMoves(false);
      if (!next->start || !next->start->isOk())
        if (errorHandler != null) errorHandler->error(StringBuffer("fault compiling regexp '")+entMatchParam+"' in scheme '"+scheme->schemeName+"'");
      delete entMatchParam;
      next->end = 0;
      loadRegions(next, tmpel);
      scheme->nodes.addElement(next);
      next = null;
      continue;
    };

    if (*tmpel->getName() == "block"){
      const String *sParam = tmpel->getParamValue(DString("start"));
      const String *eParam = tmpel->getParamValue(DString("end"));

      if (!sParam && tmpel->getType() == EL_BLOCKED &&
          tmpel->child() && tmpel->child()->getName() != null && *tmpel->child()->getName() == "start" &&
          tmpel->child()->child() != null &&
          tmpel->child()->child()->getType() == EL_PLAIN)
        sParam = tmpel->child()->child()->getContent();
      if (!eParam && tmpel->getType() == EL_BLOCKED &&
          tmpel->child() && tmpel->child()->next() &&
          tmpel->child()->next()->getName() != null && *tmpel->child()->next()->getName() == "end" &&
          tmpel->child()->next()->child() != null &&
          tmpel->child()->next()->child()->getType() == EL_PLAIN)
        eParam = tmpel->child()->next()->child()->getContent();

      String *startParam;
      String *endParam;
      if (!(startParam = useEntities(sParam))){
        if (errorHandler != null) errorHandler->error(StringBuffer("'start' block attribute not found in scheme '")+scheme->schemeName+"'");
        delete next;
        continue;
      };
      if (!(endParam = useEntities(eParam))){
        if (errorHandler != null) errorHandler->error(StringBuffer("'end' block attribute not found in scheme '")+scheme->schemeName+"'");
        delete next;
        continue;
      };
      const String *schemeName = tmpel->getParamValue(DString("scheme"));
      if (schemeName == null){
        if (errorHandler != null) errorHandler->error(StringBuffer("block with bad scheme attribute in scheme '")+scheme->getName()+"'");
        delete startParam;
        delete endParam;
        continue;
      };
      next->schemeName = new SString(schemeName);
      next->lowPriority = DString("low").equals(tmpel->getParamValue(DString("priority")));
      next->lowContentPriority = DString("low").equals(tmpel->getParamValue(DString("content-priority")));
      next->type = SNT_SCHEME;
      next->start = new CRegExp(startParam);
      next->start->setPositionMoves(false);
      if (!next->start->isOk()) if (errorHandler != null) errorHandler->error(StringBuffer("fault compiling regexp '")+startParam+"' in scheme '"+scheme->schemeName+"'");
      next->end = new CRegExp();
      next->end->setPositionMoves(true);
      next->end->setBackRE(next->start);
      next->end->setRE(endParam);
      if (!next->end->isOk()) if (errorHandler != null) errorHandler->error(StringBuffer("fault compiling regexp '")+endParam+"' in scheme '"+scheme->schemeName+"'");
      delete startParam;
      delete endParam;

      loadRegions(next, tmpel);
      scheme->nodes.addElement(next);
      next = null;
      continue;
    };

    if (*tmpel->getName() == "keywords"){
      bool isCase = tmpel->getParamValue(DString("ignorecase")) ? false : true;
      next->lowPriority = !DString("normal").equals(tmpel->getParamValue(DString("priority")));
      const Region *brgn = getNCRegion(tmpel, DString("region"));
      if (brgn == null) continue;
      const String *worddiv = tmpel->getParamValue(DString("worddiv"));

      next->worddiv = null;
      if (worddiv){
        String *entWordDiv = useEntities(worddiv);
        next->worddiv = CharacterClass::createCharClass(*entWordDiv, 0, null);
        if(next->worddiv == null)
          if (errorHandler != null) errorHandler->warning(StringBuffer("fault compiling worddiv regexp '")+entWordDiv+"' in scheme '"+scheme->schemeName+"'");
        delete entWordDiv;
      };

      next->kwList = new KeywordList;
      for(CXmlEl *keywrd_count = tmpel->child(); keywrd_count; keywrd_count = keywrd_count->next())
        if (keywrd_count->getName()  &&  (*keywrd_count->getName() == "word" ||
            *keywrd_count->getName() == "symb"))
          next->kwList->num++;

      next->kwList->kwList = new KeywordInfo[next->kwList->num];
      next->kwList->num = 0;
      KeywordInfo *pIDs = next->kwList->kwList;
      next->kwList->matchCase = isCase;
      next->kwList->kwList = pIDs;
      next->type = SNT_KEYWORDS;

      for(CXmlEl *keywrd = tmpel->child(); keywrd; keywrd = keywrd->next()){
        int type = 0;
        if (keywrd->getName() == null) continue;
        if (*keywrd->getName() == "word") type = 1;
        if (*keywrd->getName() == "symb") type = 2;
        if (!type) continue;
        const String *param;
        if (!(param = keywrd->getParamValue(DString("name"))) || !param->length()) continue;

        const Region *rgn = brgn;
        if (keywrd->getParamValue(DString("region")))
          rgn = getNCRegion(keywrd, DString("region"));

        int pos = next->kwList->num;
        pIDs[pos].keyword = new SString(param);
        pIDs[pos].region = rgn;
        pIDs[pos].isSymbol = (type == 2);
        next->kwList->firstChar->addChar((*param)[0]);
        if (!isCase){
          next->kwList->firstChar->addChar(Character::toLowerCase((*param)[0]));
          next->kwList->firstChar->addChar(Character::toUpperCase((*param)[0]));
          next->kwList->firstChar->addChar(Character::toTitleCase((*param)[0]));
        };
        next->kwList->num++;
        if (next->kwList->minKeywordLength > pIDs[pos].keyword->length())
          next->kwList->minKeywordLength = pIDs[pos].keyword->length();
      };
      next->kwList->sortList();
      next->kwList->substrIndex();
      scheme->nodes.addElement(next);
      next = null;
      continue;
    };
  };
  // drop last unused node
  if (next != null) delete next;
};

void HRCParserImpl::loadRegions(SchemeNode *node, CXmlEl *el)
{
int i;
static char rg_tmpl[0x10] = "region";

  node->region = getNCRegion(el, DString("region"));
  for (i = 0; i < REGIONS_NUM; i++){
    if (node->type == SNT_SCHEME){
      rg_tmpl[6] = '0'; rg_tmpl[7] = (i<0xA?i:i+7)+'0'; rg_tmpl[8] = 0;
      node->regions[i] = getNCRegion(el, DString(rg_tmpl));
      rg_tmpl[6] = '1';
      node->regione[i] = getNCRegion(el, DString(rg_tmpl));

      rg_tmpl[6] = (i<0xA?i:i+7)+'0'; rg_tmpl[7] = 0;
      if (el->getType() == EL_BLOCKED &&
          el->child() && el->child()->getName() != null &&
          *el->child()->getName() == "start"){
        node->regions[i] = getNCRegion(el->child(), DString(rg_tmpl));
      };
      if (el->getType() == EL_BLOCKED && el->child() && el->child()->next() &&
          el->child()->next()->getName() != null &&
          *el->child()->next()->getName() == "end"){
         node->regione[i] = getNCRegion(el->child()->next(), DString(rg_tmpl));
      };
    }else if (node->type == SNT_RE){
      rg_tmpl[6] = (i<0xA?i:i+7)+'0'; rg_tmpl[7] = 0;
      node->regions[i] = getNCRegion(el, DString(rg_tmpl));
    };
  };
  for (i = 0; i < NAMED_REGIONS_NUM; i++){
    node->regionsn[i] = getNCRegion(node->start->getBracketName(i), false);
    if (node->type == SNT_SCHEME)
      node->regionen[i] = getNCRegion(node->end->getBracketName(i), false);
  };
  if (node->type == SNT_RE && node->region) node->regions[0] = node->region;
};

void HRCParserImpl::updateLinks()
{
  structureChanged = true;
  if (updateStarted) return;
  updateStarted = true;
  while(structureChanged){
    structureChanged = false;
    for(SchemeImpl *scheme = schemeHash.enumerate(); scheme != null; scheme = schemeHash.next()){
      if (!scheme->fileType->loadDone) continue;
      FileTypeImpl *old_parseType = parseType;
      parseType = scheme->fileType;
      for (int sni = 0; sni < scheme->nodes.size(); sni++){
        SchemeNode *snode = scheme->nodes.elementAt(sni);
        if (snode->schemeName != null && (snode->type == SNT_SCHEME || snode->type == SNT_INHERIT) && snode->scheme == null){
          String *schemeName = qualifyForeignName(snode->schemeName, QNT_SCHEME, true);
          if (schemeName != null){
            snode->scheme = schemeHash.get(schemeName);
          }else{
            if (errorHandler != null) errorHandler->error(StringBuffer("cannot resolve scheme name '")+snode->schemeName+"' in scheme '"+scheme->schemeName+"'");
          };
          delete snode->schemeName;
          snode->schemeName = null; //!!!schemeName;
        };
        if (snode->type == SNT_INHERIT){
          for(int vti = 0; vti < snode->virtualEntryVector.size(); vti++){
            VirtualEntry *vt = snode->virtualEntryVector.elementAt(vti);
            if (vt->virtScheme == null && vt->virtSchemeName != null){
              String *vsn = qualifyForeignName(vt->virtSchemeName, QNT_SCHEME, true);
              if (vsn) vt->virtScheme = schemeHash.get(vsn);
              if (!vsn) if (errorHandler != null) errorHandler->error(StringBuffer("cannot virtualize scheme '")+vt->virtSchemeName+"' in scheme '"+scheme->schemeName+"'");
              delete vsn;
              delete vt->virtSchemeName;
              vt->virtSchemeName = null;
            };
            if (vt->substScheme == null && vt->substSchemeName != null){
              String *vsn = qualifyForeignName(vt->substSchemeName, QNT_SCHEME, true);
              if (vsn) vt->substScheme = schemeHash.get(vsn);
              else if (errorHandler != null) errorHandler->error(StringBuffer("cannot virtualize using subst-scheme scheme '")+vt->substSchemeName+"' in scheme '"+scheme->schemeName+"'");
              delete vsn;
              delete vt->substSchemeName;
              vt->substSchemeName = null;
            };
          };
        };
      };
      parseType = old_parseType;
      if (structureChanged) break;
    };
  };
  updateStarted = false;
};



String *HRCParserImpl::qualifyOwnName(const String *name) {
  if (name == null) return null;
  int colon = name->indexOf(':');
  if (colon != -1){
    if (parseType && DString(name, 0, colon) != *parseType->name){
      if (errorHandler != null) errorHandler->error(StringBuffer("type name qualifer in '")+name+"' doesn't match type '"+parseType->name+"'");
      return null;
    }else return new SString(name);
  }else{
    if (parseType == null) return null;
    StringBuffer *sbuf = new StringBuffer(parseType->name);
    sbuf->append(DString(":")).append(name);
    return sbuf;
  };
};
bool HRCParserImpl::checkNameExist(const String *name, FileTypeImpl *parseType, QualifyNameType qntype, bool logErrors) {
  if (qntype == QNT_DEFINE && regionNamesHash.get(name) == null){
    if (logErrors)
      if (errorHandler != null) errorHandler->error(StringBuffer("region '")+name+"', referenced in type '"+parseType->name+"', is not defined");
    return false;
  }else if (qntype == QNT_ENTITY && schemeEntitiesHash.get(name) == null){
    if (logErrors)
      if (errorHandler != null) errorHandler->error(StringBuffer("entity '")+name+"', referenced in type '"+parseType->name+"', is not defined");
    return false;
  }else if (qntype == QNT_SCHEME && schemeHash.get(name) == null){
    if (logErrors)
      if (errorHandler != null) errorHandler->error(StringBuffer("scheme '")+name+"', referenced in type '"+parseType->name+"', is not defined");
    return false;
  };
  return true;
};
String *HRCParserImpl::qualifyForeignName(const String *name, QualifyNameType qntype, bool logErrors){
  if (name == null) return null;
  int colon = name->indexOf(':');
  if (colon != -1){ // qualified name
    DString prefix(name, 0, colon);
    FileTypeImpl *prefType = fileTypeHash.get(&prefix);

    if (prefType == null){
      if (logErrors && errorHandler != null) errorHandler->error(StringBuffer("type name qualifer in '")+name+"' doesn't match any type");
      return null;
    }else
      if (!prefType->typeLoaded) loadFileType(prefType);
    if (prefType == parseType || prefType->typeLoaded)
      return checkNameExist(name, prefType, qntype, logErrors)?(new SString(name)):null;
  }else{ // unqualified name
    for(int idx = -1; parseType != null && idx < parseType->importVector.size(); idx++){
      const String *tname = parseType->name;
      if (idx > -1) tname = parseType->importVector.elementAt(idx);
      FileTypeImpl *importer = fileTypeHash.get(tname);
      if (!importer->typeLoaded) loadFileType(importer);

      StringBuffer *qname = new StringBuffer(tname);
      qname->append(DString(":")).append(name);
      if (checkNameExist(qname, importer, qntype, false)) return qname;
      delete qname;
    };
    if (logErrors && errorHandler != null) errorHandler->error(StringBuffer("unqualified name '")+name+"' doesn't belong to any imported type");
  };
  return null;
};


String *HRCParserImpl::useEntities(const String *name)
{
  int copypos = 0;
  int epos = 0;

  if (!name) return null;
  StringBuffer *newname = new StringBuffer();

  while(true){
    epos = name->indexOf('%', epos);
    if (epos == -1){
      epos = name->length();
      break;
    };
    if (epos && (*name)[epos-1] == '\\'){
      epos++;
      continue;
    };
    int elpos = name->indexOf(';', epos);
    if (elpos == -1){
      epos = name->length();
      break;
    };
    DString enname(name, epos+1, elpos-epos-1);

    String *qEnName = qualifyForeignName(&enname, QNT_ENTITY, true);
    const String *enval = null;
    if (qEnName != null){
      enval = schemeEntitiesHash.get(qEnName);
      delete qEnName;
    };
    if (enval == null){
      epos++;
      continue;
    };
    newname->append(DString(name, copypos, epos-copypos));
    newname->append(enval);
    epos = elpos+1;
    copypos = epos;
  };
  if (epos > copypos) newname->append(DString(name, copypos, epos-copypos));
  return newname;
};

const Region* HRCParserImpl::getNCRegion(const String *name, bool logErrors){
  if (name == null) return null;
  const Region *reg = null;
  String *qname = qualifyForeignName(name, QNT_DEFINE, logErrors);
  if (qname == null) return null;
  reg = regionNamesHash.get(qname);
  delete qname;
  /** Check for 'default' region request.
      Regions with this name are always transparent
  */
  if (reg != null){
    const String *name = reg->getName();
    int idx = name->indexOf(DString(":default"));
    if (idx != -1  && idx+8 == name->length()) return null;
  };
  return reg;
};

const Region* HRCParserImpl::getNCRegion(CXmlEl *el, const String &tag){
  const String *par = el->getParamValue(tag);
  if (par == null) return null;
  return getNCRegion(par, true);
};

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Colorer Library.
 *
 * The Initial Developer of the Original Code is
 * Cail Lomecb <cail@nm.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2003
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
