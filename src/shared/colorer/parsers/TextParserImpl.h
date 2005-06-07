#ifndef _COLORER_TEXTPARSERIMPL_H_
#define _COLORER_TEXTPARSERIMPL_H_

#include<colorer/TextParser.h>
#include<colorer/parsers/helpers/TextParserHelpers.h>

/**
 * Implementation of TextParser interface.
 * This is the base Colorer syntax parser, which
 * works with parsed internal HRC structure and colorisez
 * text in a target editor system.
 * @ingroup colorer_parsers
 */
class TextParserImpl : public TextParser
{
public:
  TextParserImpl();
  ~TextParserImpl();

  void setFileType(FileType *type);

  void setLineSource(LineSource *lh);
  void setRegionHandler(RegionHandler *rh);

  int  parse(int from, int num, TextParseMode mode);
  void breakParse();
  void clearCache();

private:
  int len, lowlen;
  String *str;
  int gx, gy, gy2;
  int clearLine, endLine, schemeStart;
  SchemeImpl *baseScheme;

  bool breakParsing;
  bool first, invisibleSchemesFilled;
  bool drawing, updateCache;

  const Region *picked;

  ParseCache *cache;
  ParseCache *parent, *forward;

  int cachedLineNo;
  ParseCache *cachedParent,*cachedForward;

  VTList *vtlist;

  LineSource *lineSource;
  RegionHandler *regionHandler;

  void fillInvisibleSchemes(ParseCache *cache);
  void addRegion(int lno, int sx, int ex, const Region* region);
  void enterScheme(int lno, int sx, int ex, const Region* region);
  void leaveScheme(int lno, int sx, int ex, const Region* region);
  void enterScheme(int lno, SMatches *match, const SchemeNode *schemeNode);
  void leaveScheme(int lno, SMatches *match, const SchemeNode *schemeNode);

  void TextParserImpl::incrementPosition();
  int searchKW(const SchemeNode *node, int lowLen);
  //int searchRE(SchemeImpl *cscheme, int no, int lowLen, int hiLen);
  bool colorize();



private:
  ParseStep *top;
  Vector<ParseStep*> parseSteps;

  void push(ParseStep *step){
    parseSteps.addElement(step);
    top = step;
  }

  void pop(){
    if (top == null){
      throw Exception(DString("Invalid parser state: ParseState::pop() from null"));
    }
    while (top->inheritStack.size()){
      restoreVirtual();
      top->pop();
    }
    delete top;
    parseSteps.setSize(parseSteps.size()-1);
    if (parseSteps.size() > 0){
      top = parseSteps.lastElement();
    }else{
      top = null;
    }
  }

  void restoreVirtual(){
    /*
     * Restore virtual table on parser pop
     */
    if (top->itop->vtlistPushedVirtual){
      top->itop->vtlistPushedVirtual = false;
      vtlist->popvirt();
    }
    if (top->itop->vtlistPushed){
      top->itop->vtlistPushed = false;
      vtlist->pop();
    }
  }

  SchemeNode *currentSchemeNode(InheritStep *itop)
  {
    if (itop->schemeNodePosition >= itop->scheme->nodes.size()){
      return null;
    }
    return itop->scheme->nodes.elementAt(itop->schemeNodePosition);
  }

  void restart()
  {
    while (top->inheritStack.size() > 1){
      restoreVirtual();
      top->pop();
    }
    top->itop->schemeNodePosition = 0;
  }

  void move()
  {
    restart();
    if (top->closingREmatched && gx+1 >= lowlen) {
      /* Reached block's end RE */
      restoreVirtual();

      if (top->itop->vtlistPushedVirtual){
        vtlist->popvirt();
      }
      gx = top->matchend.e[0];

      InheritStep *prev_itop = parseSteps.elementAt(parseSteps.size()-2)->itop;
      SchemeNode *schemeNode = currentSchemeNode(prev_itop);
      leaveScheme(gy, &top->matchend, schemeNode);

      schemeNode->end->setBackTrace(top->o_str, top->o_match);
      delete top->backLine;

      top->pop();
      pop();
/*
      if (updateCache){
        if (ogy == gy){
          delete OldCacheF;
          if (ResF) ResF->next = null;
          else ResP->children = null;
          forward = ResF;
          parent = ResP;
        }else{
          OldCacheF->eline = gy;
          OldCacheF->vcache = vtlist->store();
          forward = OldCacheF;
          parent = OldCacheP;
        };
      }else{
*/
//      };
      restart();
      return;
    }
    incrementPosition();
    top->itop->schemeNodePosition = 0;
  }

  void cont(){
    top->itop->schemeNodePosition++;
    if (top->itop->schemeNodePosition >= top->itop->scheme->nodes.size()){
      if (top->inheritStack.size() > 1){
        restoreVirtual();
        top->pop();
        cont();
      }else{
        move();
      }
    }
  }

};

#endif

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
 * Portions created by the Initial Developer are Copyright (C) 1999-2005
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
