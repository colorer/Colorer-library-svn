
#include<colorer/editor/BaseEditor.h>

//#define LOG_DEBUG

ErrorHandler *eh;

BaseEditor::BaseEditor(ParserFactory *parserFactory, LineSource *lineSource)
{
  if (parserFactory == null || lineSource == null)
    throw Exception(DString("Bad BaseEditor constructor parameters"));
  this->parserFactory = parserFactory;
  this->lineSource = lineSource;

  hrcParser = parserFactory->getHRCParser();
  textParser = parserFactory->createTextParser();
  textParser->setRegionHandler(this);
  textParser->setLineSource(lineSource);

  lrSupport = null;

  invalidLine = 0;
  changedLine = 0;
  backParse = -1;
  lineCount = 0;
  lrSize = 200;
  wStart = 0;
  wSize = 0;
  internalRM = false;
  regionMapper = null;
  regionCompact = false;
  currentFileType = null;

  breakParse = false;
  validationProcess = false;

  def_Text = hrcParser->getRegion(&DString("def:Text"));
  def_Syntax = hrcParser->getRegion(&DString("def:Syntax"));
  def_Special = hrcParser->getRegion(&DString("def:Special"));
  def_PairStart = hrcParser->getRegion(&DString("def:PairStart"));
  def_PairEnd = hrcParser->getRegion(&DString("def:PairEnd"));

  rd_def_Text = rd_def_HorzCross = rd_def_VertCross = null;
  eh = parserFactory->getErrorHandler();
}

BaseEditor::~BaseEditor(){
  textParser->breakParse();
  breakParse = true;
  while(validationProcess);// wait until validation is finished
  if (internalRM) delete regionMapper;
  delete lrSupport;
  delete textParser;
}

void BaseEditor::setRegionCompact(bool compact){
  if (regionCompact != compact){
    regionCompact = compact;
    remapLRS(true);
  };
}
void BaseEditor::setRegionMapper(RegionMapper *rs){
  if (internalRM) delete regionMapper;
  regionMapper = rs;
  internalRM = false;
  remapLRS(false);
}
void BaseEditor::setRegionMapper(const String *hrdClass, const String *hrdName){
  if (internalRM) delete regionMapper;
  regionMapper = parserFactory->createStyledMapper(hrdClass, hrdName);
  internalRM = true;
  remapLRS(false);
}
void BaseEditor::remapLRS(bool recreate){
  if (recreate || lrSupport == null){
    delete lrSupport;
    if (regionCompact) lrSupport = new LineRegionsCompactSupport();
    else lrSupport = new LineRegionsSupport();
    lrSupport->resize(lrSize);
  };
  lrSupport->setRegionMapper(regionMapper);
  lrSupport->setSpecialRegion(def_Special);
  invalidLine = 0;
  rd_def_Text = rd_def_HorzCross = rd_def_VertCross = null;
  if (regionMapper != null){
    rd_def_Text = regionMapper->getRegionDefine(DString("def:Text"));
    rd_def_HorzCross = regionMapper->getRegionDefine(DString("def:HorzCross"));
    rd_def_VertCross = regionMapper->getRegionDefine(DString("def:VertCross"));
  };
}

void BaseEditor::setFileType(FileType *ftype){
#ifdef LOG_DEBUG
  eh->warning(StringBuffer("setFileType:")+ftype->getName());
#endif
  currentFileType = ftype;
  textParser->setFileType(currentFileType);
  invalidLine = 0;
};
FileType *BaseEditor::setFileType(const String &fileType){
  currentFileType = hrcParser->getFileType(&fileType);
  setFileType(currentFileType);
  return currentFileType;
}
FileType *BaseEditor::chooseFileType(const String *fileName){
  if (lineSource == null){
    currentFileType = hrcParser->chooseFileType(fileName, null);
  }else{
    StringBuffer textStart;
    int totalLength = 0;
    for(int i = 0; i < 4; i++){
      String *iLine = lineSource->getLine(i);
      if (iLine == null) break;
      textStart.append(iLine);
      textStart.append(DString("\n"));
      totalLength += iLine->length();
      if (totalLength > 500) break;
    };
    currentFileType = hrcParser->chooseFileType(fileName, &textStart);
  };
  setFileType(currentFileType);
  return currentFileType;
}
FileType *BaseEditor::getFileType(){
  return currentFileType;
}

void BaseEditor::setBackParse(int backParse){
  this->backParse = backParse;
}

void BaseEditor::addRegionHandler(RegionHandler *rh){
  regionHandlers.addElement(rh);
}
void BaseEditor::removeRegionHandler(RegionHandler *rh){
  regionHandlers.removeElement(rh);
}


PairMatch *BaseEditor::getPairMatch(int lineNo, int linePos){
  LineRegion *lrStart = getLineRegions(lineNo);
  if (lrStart == null) return null;
  LineRegion *pair = null;
  for(LineRegion *l1 = lrStart; l1; l1 = l1->next){
    if ((l1->region->hasParent(def_PairStart) ||
         l1->region->hasParent(def_PairEnd)) &&
         linePos >= l1->start && linePos <= l1->end)
      pair = l1;
  };
  if (pair != null){
    PairMatch *pm = new PairMatch();
    pm->start = pair;
    pm->end = pair;
    pm->sline = lineNo;
    pm->pairBalance = -1;
    pm->topPosition = false;
    if (pair->region->hasParent(def_PairStart)){
      pm->pairBalance = 1;
      pm->topPosition = true;
    };
    return pm;
  };
  return null;
}
PairMatch *BaseEditor::getEnwrappedPairMatch(int lineNo, int pos){
  return null;
};
void BaseEditor::releasePairMatch(PairMatch *pm){
  delete pm;
}

void BaseEditor::searchLocalPair(PairMatch *pm)
{
  int lno = pm->sline;
  int end_line = getLastVisibleLine();
  LineRegion *pair = pm->start;
  LineRegion *slr = getLineRegions(lno);
  while(true){
    if (pm->pairBalance > 0){
      pair = pair->next;
      while(pair == null){
        lno++;
        if (lno > end_line) break;
        pair = getLineRegions(lno);
      };
      if (lno > end_line) break;
    }else{
      if(pair->prev == slr->prev){ // first region
        lno--;
        if (lno < wStart) break;
        slr = getLineRegions(lno);
        pair = slr;
      };
      if (lno < wStart) break;
      pair = pair->prev;
    };
    if (pair->region->hasParent(def_PairStart)) pm->pairBalance++;
    if (pair->region->hasParent(def_PairEnd)) pm->pairBalance--;
    if (pm->pairBalance == 0) break;
  };
  if (pm->pairBalance == 0){
    pm->eline = lno;
    pm->end = pair;
  };
}

void BaseEditor::searchGlobalPair(PairMatch *pm)
{
  int lno = pm->sline;
  int end_line = lineCount;
  LineRegion *pair = pm->start;
  LineRegion *slr = getLineRegions(lno);
  while(true){
    if (pm->pairBalance > 0){
      pair = pair->next;
      while(pair == null){
        lno++;
        if (lno > end_line) break;
        pair = getLineRegions(lno);
      };
      if (lno > end_line) break;
    }else{
      if(pair->prev == slr->prev){ // first region
        lno--;
        if (lno < 0) break;
        slr = getLineRegions(lno);
        pair = slr;
      };
      if (lno < 0) break;
      pair = pair->prev;
    };
    if (pair->region->hasParent(def_PairStart)) pm->pairBalance++;
    if (pair->region->hasParent(def_PairEnd)) pm->pairBalance--;
    if (pm->pairBalance == 0) break;
  };
  if (pm->pairBalance == 0){
    pm->eline = lno;
    pm->end = pair;
  };
}


LineRegion *BaseEditor::getLineRegions(int lno){
  if (backParse > wSize && lno - invalidLine > backParse) return null;
#ifdef LOG_DEBUG
  eh->warning(StringBuffer("getLineRegions:")+SString(lno));
#endif
  validate(lno);
  return lrSupport->getLineRegions(lno);
}

void BaseEditor::modifyEvent(int topLine){
#ifdef LOG_DEBUG
  eh->warning(StringBuffer("modifyEvent:")+SString(topLine));
#endif
  if (invalidLine > topLine) invalidLine = topLine;
}
void BaseEditor::modifyLineEvent(int line){
  if (invalidLine > line) invalidLine = line;
  // changedLine = topLine;!!!
}
void BaseEditor::visibleTextEvent(int wStart, int wSize){
#ifdef LOG_DEBUG
  eh->warning(StringBuffer("visibleTextEvent:")+SString(wStart)+"-"+SString(wSize));
#endif
  this->wStart = wStart;
  this->wSize = wSize;
}

void BaseEditor::lineCountEvent(int newLineCount){
#ifdef LOG_DEBUG
  eh->warning(StringBuffer("lineCountEvent:")+SString(newLineCount));
#endif
  if (lrSize < newLineCount){
    lrSize = newLineCount*2;
    lrSupport->resize(lrSize);
  };
  lineCount = newLineCount;
}


inline int BaseEditor::getLastVisibleLine(){
  int r1 = (wStart+wSize);
  int r2 = lineCount;
  return ((r1 > r2)?r2:r1)-1;
}

void BaseEditor::validate(int lno)
{
  if (lno == -1) lno = lineCount;
  int parseNum = 1;
  if (lno >= wStart && lno < wStart+wSize){
    if (invalidLine > getLastVisibleLine()) return;
    parseNum = getLastVisibleLine()+1 - invalidLine;
  }else{
    parseNum = lno+wSize*2;
    if (parseNum > lineCount) parseNum = lineCount;
    parseNum -= invalidLine;
    if (invalidLine > lno || parseNum <= 0) return;
  };
#ifdef LOG_DEBUG
  eh->warning(StringBuffer("parse:")+SString(invalidLine)+"-"+SString(parseNum));
#endif
  int stopLine = textParser->parse(invalidLine, parseNum);
#ifdef LOG_DEBUG
  eh->warning(StringBuffer("parsedone:")+SString(stopLine));
#endif
  invalidLine = stopLine+1;
}



void BaseEditor::startParsing(int lno){
  lrSupport->startParsing(lno);
  for(int idx = 0; idx < regionHandlers.size(); idx++)
    regionHandlers.elementAt(idx)->startParsing(lno);
}
void BaseEditor::endParsing(int lno){
  lrSupport->endParsing(lno);
  for(int idx = 0; idx < regionHandlers.size(); idx++)
    regionHandlers.elementAt(idx)->endParsing(lno);
}
void BaseEditor::clearLine(int lno, String *line){
  lrSupport->clearLine(lno, line);
  for(int idx = 0; idx < regionHandlers.size(); idx++)
    regionHandlers.elementAt(idx)->clearLine(lno, line);
}
void BaseEditor::addRegion(int lno, String *line, int sx, int ex, const Region *region){
  lrSupport->addRegion(lno, line, sx, ex, region);
  for(int idx = 0; idx < regionHandlers.size(); idx++)
    regionHandlers.elementAt(idx)->addRegion(lno, line, sx, ex, region);
}
void BaseEditor::enterScheme(int lno, String *line, int sx, int ex, const Region *region, const Scheme *scheme){
  lrSupport->enterScheme(lno, line, sx, ex, region, scheme);
  for(int idx = 0; idx < regionHandlers.size(); idx++)
    regionHandlers.elementAt(idx)->enterScheme(lno, line, sx, ex, region, scheme);
}
void BaseEditor::leaveScheme(int lno, String *line, int sx, int ex, const Region *region, const Scheme *scheme){
  lrSupport->leaveScheme(lno, line, sx, ex, region, scheme);
  for(int idx = 0; idx < regionHandlers.size(); idx++)
    regionHandlers.elementAt(idx)->leaveScheme(lno, line, sx, ex, region, scheme);
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
 * decision by deleting thd provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
