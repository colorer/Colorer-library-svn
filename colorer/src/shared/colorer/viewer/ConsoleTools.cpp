
#include<colorer/ParserFactory.h>
#include<colorer/editor/BaseEditor.h>
#include<colorer/viewer/TextLinesStore.h>
#include<colorer/viewer/ParsedLineWriter.h>
#include<colorer/viewer/TextConsoleViewer.h>

#include<colorer/viewer/ConsoleTools.h>

ConsoleTools::ConsoleTools(){
  copyrightHeader = true;
  htmlEscaping = true;
  bomOutput = true;
  htmlWrapping = true;

  typeDescription = null;
  inputFileName = outputFileName = null;
  inputEncoding = outputEncoding = null;
  inputEncodingIndex = outputEncodingIndex = -1;
  catalogPath = null;
  hrdName = null;

  docLinkHash = new Hashtable<String*>;
}
ConsoleTools::~ConsoleTools(){
  delete typeDescription;
  delete catalogPath;
  delete hrdName;
  delete inputEncoding;
  delete outputEncoding;
  delete outputFileName;
  delete inputFileName;

  for(int i = 0; i < docLinkHash->size(); i++){
    delete docLinkHash->get(docLinkHash->key(i));
  };
}


void ConsoleTools::setCopyrightHeader(bool use) { copyrightHeader = use; }

void ConsoleTools::setHtmlEscaping(bool use) { htmlEscaping = use; }

void ConsoleTools::setBomOutput(bool use) { bomOutput = use; }

void ConsoleTools::setHtmlWrapping(bool use) { htmlWrapping = use; }

void ConsoleTools::setTypeDescription(const String &str) {
  delete typeDescription;
  typeDescription = new SString(str);
}

void ConsoleTools::setInputFileName(const String &str) {
  delete inputFileName;
  inputFileName = new SString(str);
}

void ConsoleTools::setOutputFileName(const String &str) {
  delete outputFileName;
  outputFileName = new SString(str);
}

void ConsoleTools::setInputEncoding(const String &str) {
  delete inputEncoding;
  inputEncoding = new SString(str);
  inputEncodingIndex = Encodings::getEncodingIndex(inputEncoding->getChars());
  if (inputEncodingIndex == -1) throw Exception(StringBuffer("Unknown input encoding: ")+inputEncoding);
  if (outputEncoding == null) outputEncodingIndex = inputEncodingIndex;
}

void ConsoleTools::setOutputEncoding(const String &str) {
  delete outputEncoding;
  outputEncoding = new SString(str);
  outputEncodingIndex = Encodings::getEncodingIndex(outputEncoding->getChars());
  if (outputEncodingIndex == -1) throw Exception(StringBuffer("Unknown output encoding: ")+outputEncoding);
}

void ConsoleTools::setCatalogPath(const String &str) {
  delete catalogPath;
  catalogPath = new SString(str);
}

void ConsoleTools::setHRDName(const String &str) {
  delete hrdName;
  hrdName = new SString(str);
}
void ConsoleTools::setLinkSource(const String &str){
  InputSource *linkSource = null;
  const byte *stream = null;
  try{
    linkSource = InputSource::newInstance(&str);
    stream = linkSource->openStream();
  }catch(InputSourceException &e){
    throw Exception(*e.getMessage());
  };

  CXmlEl *linkSourceTree = new CXmlEl();
  linkSourceTree->parse(stream, linkSource->length());
  linkSource->closeStream();
  
  CXmlEl *elem = linkSourceTree;

  while(elem = elem->next()){
   if (elem == null || elem->getType() == EL_BLOCKED &&
       elem->getName() && *elem->getName() == "doclinks") break;
  };
  if (elem == null) throw Exception(DString("Bad doclinks data file structure"));

  elem = elem->child();
  while(elem != null){
    if (elem->getType() == EL_BLOCKED && elem->getName() && *elem->getName() == "links"){
      const String *url = elem->getParamValue(DString("url"));
      const String *scheme = elem->getParamValue(DString("scheme"));
      CXmlEl *eachLink = elem->child();
      while(eachLink != null){
        if (eachLink->getName() && *eachLink->getName() == "link"){
          const String *l_url = eachLink->getParamValue(DString("url"));
          const String *l_scheme = eachLink->getParamValue(DString("scheme"));
          const String *token = eachLink->getParamValue(DString("token"));
          StringBuffer fullURL;
          if (url != null) fullURL.append(url);
          if (l_url != null) fullURL.append(l_url);
          if (l_scheme == null) l_scheme = scheme;
          if (token == null) continue;
          StringBuffer hkey(token);
          if (l_scheme != null){
            hkey.append(DString("--")).append(l_scheme);
          };
          docLinkHash->put(&hkey, new SString(&fullURL));
        };
        eachLink = eachLink->next();
      };
    };
    elem = elem->next();
  };
  delete linkSource;
  delete linkSourceTree;
}


void ConsoleTools::RETest(){
  SMatches match;
  CRegExp *re;
  bool res;
  char text[255];

  re = new CRegExp();
  do{
    printf("\nregexp:");
    gets(text);
    if (!re->setRE(&DString(text))) continue;
    printf("exprn:");
    gets(text);
    res = re->parse(&DString(text), &match);
    printf("%s\nmatch:  ",res?"ok":"error");
    for(int i = 0; i < match.cMatch; i++){
      printf("%d:(%d,%d), ",i,match.s[i],match.e[i]);
    };
  }while(text[0]);
  delete re;
}

void ConsoleTools::listTypes(bool load){
  Writer *writer = null;
  try{
    writer = new StreamWriter(stdout, outputEncodingIndex, bomOutput);
    ParserFactory pf(catalogPath);
    HRCParser *hrcParser = pf.getHRCParser();
    fprintf(stderr, "\nloading file types...\n");
    for(int idx = 0;; idx++){
      FileType *type = hrcParser->enumerateFileTypes(idx);
      if (type == null) break;
      writer->write(StringBuffer(type->getGroup()) + ": " + type->getDescription()+"\n");
      if (load) type->getBaseScheme();
    };
    delete writer;
  }catch(Exception &e){
    delete writer;
    fprintf(stderr, "%s\n", e.getMessage()->getChars());
  };
}

FileType *ConsoleTools::selectType(HRCParser *hrcParser, String *fline){
  FileType *type = null;
  if (typeDescription != null){
    type = hrcParser->getFileType(typeDescription);
    if (type == null){
      for(int idx = 0;; idx++){
        type = hrcParser->enumerateFileTypes(idx);
        if (type == null) break;
        if (type->getDescription() != null &&
            type->getDescription()->length() >= typeDescription->length() &&
            DString(type->getDescription(), 0, typeDescription->length()).equalsIgnoreCase(typeDescription))
          break;
        if (type->getName()->length() >= typeDescription->length() &&
            DString(type->getName(), 0, typeDescription->length()).equalsIgnoreCase(typeDescription))
          break;
        type = null;
      };
    };
  };
  if (typeDescription == null || type == null)
    type = hrcParser->chooseFileType(inputFileName, fline, 0);
  return type;
}

void ConsoleTools::viewFile(){
  try{
    // Source file text lines store.
    TextLinesStore textLinesStore;
    textLinesStore.loadFile(inputFileName, inputEncoding, true);
    // parsers factory
    ParserFactory pf(catalogPath);
    // Base editor to make primary parse
    BaseEditor baseEditor(&pf, &textLinesStore);
    // HRD RegionMapper linking
    if (hrdName == null) hrdName = new DString("default");
    baseEditor.setRegionMapper(&DString("console"), hrdName);
    FileType *type = selectType(pf.getHRCParser(), textLinesStore.getLine(0));
    baseEditor.setFileType(type);
    // Initial line count notify
    baseEditor.lineCountEvent(textLinesStore.getLineCount());

    int background;
    const StyledRegion *rd = StyledRegion::cast(baseEditor.rd_def_Text);
    if (rd != null && rd->bfore && rd->bback) background = rd->fore + (rd->back<<4);
    else background = 0x1F;
    // File viewing in console window
    TextConsoleViewer viewer(&baseEditor, &textLinesStore, background, outputEncodingIndex);
    viewer.view();
  }catch(Exception &e){
    fprintf(stderr, "%s\n", e.getMessage()->getChars());
  }catch(...){
    fprintf(stderr, "unknown exception ...\n");
  };
}

void ConsoleTools::forward(){
  InputSource *fis = InputSource::newInstance(inputFileName);
  const byte *stream = fis->openStream();
  DString eStream(stream, fis->length(), inputEncodingIndex);

  Writer *outputFile = null;
  try{
    if (outputFileName != null) outputFile = new FileWriter(outputFileName, outputEncodingIndex, bomOutput);
    else outputFile = new StreamWriter(stdout, outputEncodingIndex, bomOutput);
  }catch(Exception &e){
    fprintf(stderr, "can't open file '%s' for writing:\n", outputFileName->getChars());
    fprintf(stderr, e.getMessage()->getChars());
    return;
  };

  outputFile->write(eStream);

  delete outputFile;
  delete fis;
}

void ConsoleTools::genOutput(bool useTokens){
  try{
    // Source file text lines store.
    TextLinesStore textLinesStore;
    textLinesStore.loadFile(inputFileName, inputEncoding, true);
    // parsers factory
    ParserFactory pf(catalogPath);
    // HRC loading
    HRCParser *hrcParser = pf.getHRCParser();
    // HRD RegionMapper creation
    bool useMarkup = false;
    RegionMapper *mapper = null;
    if (hrdName == null) hrdName = new DString("default");
    if (!useTokens){
      try{
        mapper = pf.createStyledMapper(&DString("rgb"), hrdName);
      }catch(ParserFactoryException &e){
        useMarkup = true;
        mapper = pf.createTextMapper(hrdName);
      };
    };
    // Base editor to make primary parse
    BaseEditor baseEditor(&pf, &textLinesStore);
    // Using compact regions
    baseEditor.setRegionCompact(true);
    baseEditor.setRegionMapper(mapper);
    baseEditor.lineCountEvent(textLinesStore.getLineCount());
    // Choosing file type
    FileType *type = selectType(hrcParser, textLinesStore.getLine(0));
    baseEditor.setFileType(type);
    // parsing all the text
    baseEditor.validate(-1);

    //  writing result into HTML colored stream...
    const RegionDefine *rd = null;
    if (mapper != null) rd = baseEditor.rd_def_Text;

    Writer *escapedWriter = null;
    Writer *commonWriter = null;
    try{
      if (outputFileName != null) commonWriter = new FileWriter(outputFileName, outputEncodingIndex, bomOutput);
      else commonWriter = new StreamWriter(stdout, outputEncodingIndex, bomOutput);
      if (htmlEscaping) escapedWriter = new HtmlEscapesWriter(commonWriter);
      else escapedWriter = commonWriter;
    }catch(Exception &e){
      fprintf(stderr, "can't open file '%s' for writing:\n", outputFileName->getChars());
      fprintf(stderr, e.getMessage()->getChars());
      return;
    };

    if (htmlWrapping && useTokens){
      commonWriter->write(DString("<html>\n<head>\n<style></style>\n</head>\n<body><pre>\n"));
    }else if (htmlWrapping && rd != null){
      if (useMarkup){
        commonWriter->write(TextRegion::cast(rd)->stext);
      }else{
        commonWriter->write(DString("<html><body style='"));
        ParsedLineWriter::writeStyle(commonWriter, StyledRegion::cast(rd));
        commonWriter->write(DString("'><pre>\n"));
      };
    };

    if (copyrightHeader){
      commonWriter->write(DString("Created with colorer-take5 library. Type '"));
      commonWriter->write(type->getName());
      commonWriter->write(DString("'\n\n"));
    };

    for(int i = 0; i < textLinesStore.getLineCount(); i++){
      if (useTokens){
        ParsedLineWriter::tokenWrite(commonWriter, escapedWriter, docLinkHash, textLinesStore.getLine(i), baseEditor.getLineRegions(i));
      }else if (useMarkup){
        ParsedLineWriter::markupWrite(commonWriter, escapedWriter, docLinkHash, textLinesStore.getLine(i), baseEditor.getLineRegions(i));
      }else{
        ParsedLineWriter::htmlRGBWrite(commonWriter, escapedWriter, docLinkHash, textLinesStore.getLine(i), baseEditor.getLineRegions(i));
      };
      commonWriter->write(DString("\n"));
    };

    if (htmlWrapping && useTokens){
      commonWriter->write(DString("</pre></body></html>\n"));
    }else if (htmlWrapping && rd != null){
      if (useMarkup){
        commonWriter->write(TextRegion::cast(rd)->etext);
      }else{
        commonWriter->write(DString("</pre></body></html>\n"));
      };
    };

    if (htmlEscaping) delete commonWriter;
    delete escapedWriter;

    delete mapper;
  }catch(Exception &e){
    fprintf(stderr, "%s\n", e.getMessage()->getChars());
  }catch(...){
    fprintf(stderr, "unknown exception ...\n");
  };
}


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
 * The Original Code is the Colorer Library
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