#include<colorer/parsers/TextParserImpl.h>
#include<common/Logging.h>

/* should be removed */
#define MATCH_NOTHING 0
#define MATCH_RE 1
#define MATCH_SCHEME 2


TextParserImpl::TextParserImpl()
{
  CLR_TRACE("TextParserImpl", "constructor");
  //cache = new ParseCache();
  //clearCache();
  lineSource = null;
  regionHandler = null;
  picked = null;
  provider = null;
}

TextParserImpl::~TextParserImpl()
{
  //clearCache();
  delete provider;
  //delete cache;
}

void TextParserImpl::setFileType(FileType *type)
{
  delete provider;
  provider = type->createGrammarProvider();
  //clearCache();
}

void TextParserImpl::setLineSource(LineSource *lh){
  lineSource = lh;
}


void TextParserImpl::setRegionHandler(RegionHandler *rh){
  regionHandler = rh;
}

#define dynamic_assert(a) if (!(a)) { throw Exception(DString("dynamic_assert: "#a)); }

int TextParserImpl::parse(int from, int num, TextParseMode mode)
{
  int i;

  gx = 0;
  gy = from;
  gy2 = from+num;

  invisibleSchemesFilled = false;
  schemeStart = -1;
  breakParsing = false;
  updateCache = (mode == TPM_CACHE_UPDATE);

  CLR_TRACE("TextParserImpl", "parse from=%d, num=%d", from, num);
  /* Check for initial bad conditions */
  dynamic_assert(regionHandler);
  dynamic_assert(lineSource);
  dynamic_assert(provider);

  lineSource->startJob(from);
  regionHandler->startParsing(from);

  ParseStep *ctop = new ParseStep();
  ctop->closingRE = null;
  ctop->lowContentPriority = false;

  push(ctop);

  colorize();

  while(top != null) {
      pop();
      provider->leaveBlock();
  }
  /* Init cache *
  parent = cache;
  forward = null;
  cache->scheme = baseScheme;

  if (mode == TPM_CACHE_READ || mode == TPM_CACHE_UPDATE) {
    parent = cache->searchLine(from, &forward);
    if (parent != null){
      CLR_TRACE("TPCache", "searchLine() parent:%s,%d-%d", parent->scheme->getName()->getChars(), parent->sline, parent->eline);
    }
  }
  cachedLineNo = from;
  cachedParent = parent;
  cachedForward = forward;
  CLR_TRACE("TextParserImpl", "parse: cache filled");


  do{
    if (!forward) {
      if (!parent) {
        return from;
      }
      if (updateCache){
        delete parent->children;
        parent->children = null;
      }
    }else{
      if (updateCache){
        delete forward->next;
        forward->next = null;
      }
    };
    baseScheme = parent->scheme;

    CLR_TRACE("TextParserImpl", "parse: goes into colorize()");
    if (parent != cache) {
      vtlist->restore(parent->vcache);
      parent->clender->end->setBackTrace(parent->backLine, &parent->matchstart);

      colorize();
      vtlist->clear();
    }else{
      colorize();
    };

    if (updateCache){
      if (parent != cache){
        parent->eline = gy;
      };
    }
    if (parent != cache && gy < gy2){
      leaveScheme(gy, &matchend, parent->clender);
    };
    gx = matchend.e[0];

    forward = parent;
    parent = parent->parent;
  }while(parent);
  */

  regionHandler->endParsing(endLine);
  lineSource->endJob(endLine);

  return endLine;
}


void TextParserImpl::clearCache()
{
  delete cache->children;
  delete cache->backLine;
  cache->backLine = null;
  cache->sline = 0;
  cache->eline = 0x7FFFFFF;
  cache->children = cache->parent = cache->next = null;
}


void TextParserImpl::breakParse()
{
  breakParsing = true;
}


void TextParserImpl::addRegion(int lno, int sx, int ex, const Region* region){
  if (sx == -1 || region == null) return;
  regionHandler->addRegion(lno, str, sx, ex, region);
}
void TextParserImpl::enterScheme(int lno, int sx, int ex, const Region* region){
  regionHandler->enterScheme(lno, str, sx, ex, region, provider->scheme());
}
void TextParserImpl::leaveScheme(int lno, int sx, int ex, const Region* region){
  regionHandler->leaveScheme(lno, str, sx, ex, region, provider->scheme());
  if (region != null) picked = region;
}


void TextParserImpl::enterScheme(int lno, SMatches *match)
{
  int i;

  if (provider->innerRegion() == false){
    enterScheme(lno, match->s[0], match->e[0], provider->region());
  }

  for (i = 0; i < match->cMatch; i++)
    addRegion(lno, match->s[i], match->e[i], provider->regions(i));
  for (i = 0; i < match->cnMatch; i++)
    addRegion(lno, match->ns[i], match->ne[i], provider->regionsn(i));

  if (provider->innerRegion() == true){
    enterScheme(lno, match->e[0], match->e[0], provider->region());
  }
}

void TextParserImpl::leaveScheme(int lno, SMatches *match)
{
  int i;

  if (provider->innerRegion() == true){
    leaveScheme(gy, match->s[0], match->s[0], provider->region());
  }

  for (i = 0; i < match->cMatch; i++)
    addRegion(gy, match->s[i], match->e[i], provider->regione(i));
  for (i = 0; i < match->cnMatch; i++)
    addRegion(gy, match->ns[i], match->ne[i], provider->regionen(i));

  if (provider->innerRegion() == false){
    leaveScheme(gy, match->s[0], match->e[0], provider->region());
  }
}

void TextParserImpl::fillInvisibleSchemes(ParseCache *ch)
{
  if (!ch->parent || ch == cache){
    return;
  }
  /* Fills output stream with valid "pseudo" enterScheme */
  fillInvisibleSchemes(ch->parent);
  enterScheme(gy, 0, 0, ch->clender->region);
  return;
}


int TextParserImpl::searchKW(int lowlen)
{
  KeywordList *kwList = provider->kwList();

  if (!kwList->num) return MATCH_NOTHING;

  if (kwList->minKeywordLength+gx > lowlen) return MATCH_NOTHING;
  if (gx < lowlen && !kwList->firstChar->inClass((*str)[gx])) return MATCH_NOTHING;

  int left = 0;
  int right = kwList->num;
  while(true){
    int pos = left + (right-left)/2;
    int kwlen = kwList->kwList[pos].keyword->length();
    if (lowlen < gx+kwlen) kwlen = lowlen-gx;

    int cr;
    if (kwList->matchCase)
      cr = kwList->kwList[pos].keyword->compareTo(DString(*str, gx, kwlen));
    else
      cr = kwList->kwList[pos].keyword->compareToIgnoreCase(DString(*str, gx, kwlen));

    if (cr == 0 && right-left == 1){
      bool badbound = false;
      if (!kwList->kwList[pos].isSymbol){
        if (!kwList->worddiv){
          if (gx && (Character::isLetterOrDigit((*str)[gx-1]) || (*str)[gx-1] == '_')) badbound = true;
          if (gx + kwlen < lowlen &&
          (Character::isLetterOrDigit((*str)[gx + kwlen]) || (*str)[gx + kwlen] == '_')) badbound = true;
        }else{
        // custom check for word bound
          if (gx && !kwList->worddiv->inClass((*str)[gx-1])) badbound = true;
          if (gx + kwlen < lowlen &&
            !kwList->worddiv->inClass((*str)[gx + kwlen])) badbound = true;
        };
      };
      if (!badbound){
        addRegion(gy, gx, gx + kwlen, kwList->kwList[pos].region);
        gx += kwlen;
        return MATCH_RE;
      };
    };
    if (right-left == 1){
      left = kwList->kwList[pos].ssShorter;
      if (left != -1){
        right = left+1;
        continue;
      };
      break;
    };
    if (cr == 1) right = pos;
    if (cr == 0 || cr == -1) left = pos;
  };
  return MATCH_NOTHING;
}

/*
int TextParserImpl::searchRE(SchemeImpl *cscheme, int no, int lowLen, int hiLen)
{
  int i, re_result;
  SchemeImpl *ssubst = null;
  SMatches match;
  ParseCache *OldCacheF = null;
  ParseCache *OldCacheP = null;
  ParseCache *ResF = null;
  ParseCache *ResP = null;

  CLR_TRACE("TextParserImpl", "searchRE: entered scheme \"%s\"", cscheme->getName()->getChars());

  if (!cscheme){
    return MATCH_NOTHING;
  }
  for(int idx = 0; idx < cscheme->nodes.size(); idx++){
    SchemeNode *schemeNode = cscheme->nodes.elementAt(idx);
    CLR_TRACE("TextParserImpl", "searchRE: processing node:%d/%d, type:%s", idx+1, cscheme->nodes.size(), schemeNodeTypeNames[schemeNode->type]);
    switch(schemeNode->type){
      case SNT_INHERIT:
      case SNT_KEYWORDS:
      case SNT_RE:
      case SNT_SCHEME:{

        gx = match.e[0];
        ssubst = vtlist->pushvirt(schemeNode->scheme);
        if (!ssubst) ssubst = schemeNode->scheme;

        SString *backLine = new SString(str);
        if (updateCache){
          ResF = forward;
          ResP = parent;
          if (forward){
            forward->next = new ParseCache;
            OldCacheF = forward->next;
            OldCacheP = parent?parent:forward->parent;
            parent = forward->next;
            forward = null;
          }else{
            forward = new ParseCache;
            parent->children = forward;
            OldCacheF = forward;
            OldCacheP = parent;
            parent = forward;
            forward = null;
          };
          OldCacheF->parent = OldCacheP;
          OldCacheF->sline = gy+1;
          OldCacheF->eline = 0x7FFFFFFF;
          OldCacheF->scheme = ssubst;
          OldCacheF->matchstart = match;
          OldCacheF->clender = schemeNode;
          OldCacheF->backLine = backLine;
        };

        int ogy = gy;

        SchemeImpl *o_scheme = baseScheme;
        int o_schemeStart = schemeStart;
        SMatches o_matchend = matchend;
        SMatches *o_match;
        DString *o_str;
        schemeNode->end->getBackTrace((const String**)&o_str, &o_match);

        baseScheme = ssubst;
        schemeStart = gx;
        schemeNode->end->setBackTrace(backLine, &match);

        enterScheme(no, &match, schemeNode);

        colorize(schemeNode->end, schemeNode->lowContentPriority);

        if (gy < gy2){
          leaveScheme(gy, &matchend, schemeNode);
        };
        gx = matchend.e[0];

        schemeNode->end->setBackTrace(o_str, o_match);
        matchend = o_matchend;
        schemeStart = o_schemeStart;
        baseScheme = o_scheme;

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
          delete backLine;
        };
        if (ssubst != schemeNode->scheme){
          vtlist->popvirt();
        }
        return MATCH_SCHEME;
      };
    };
  };
  return MATCH_NOTHING;
}
*/

/*
  ~ParseState(){
    while(parseSteps.size() > 0){
      delete parseSteps.lastElement();
      parseSteps.setSize(parseSteps.size()-1);
    }
  }
*/

void TextParserImpl::push(ParseStep *step){
  parseSteps.addElement(step);
  top = step;
  provider->enterBlock();
}

void TextParserImpl::pop(){
  if (top == null){
    throw Exception(DString("Invalid parser state: ParseState::pop() from null"));
  }
  delete top;
  parseSteps.setSize(parseSteps.size()-1);
  if (parseSteps.size() > 0){
    top = parseSteps.lastElement();
  }else{
    top = null;
  }
}


void TextParserImpl::move()
{
  if (top->closingREmatched && gx+1 >= lowlen) {
    /* Reached block's end RE */
    provider->leaveBlock();

    gx = top->matchend.e[0];
    leaveScheme(gy, &top->matchend);

    provider->endRE()->setBackTrace(top->o_str, top->o_match);
    delete top->backLine;
//    top->backLine = null;

    pop();

    provider->restart();
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
  } else {
    provider->restart();
    incrementPosition();
  }
}

void TextParserImpl::cont()
{
  provider->nextItem();
  if (provider->nodeType() == SNT_EMPTY) {
    move();
  }
}

void TextParserImpl::restart()
{
  provider->restart();
}

void TextParserImpl::incrementPosition() {
  gx++;
  CLR_TRACE("TextParserImpl", "X: %d", gx);
  if (gx < 0 || gx >= str->length()){
    gx = 0;
    gy++;
    CLR_TRACE("TextParserImpl", "Line: %d", gy);
    top->contextStart = -1;
    top->closingREparsed = false;
    if (gy >= gy2){
      return;
    }
    // get line data from LineSource interface
    clearLine = gy;
    str = lineSource->getLine(gy);
    len = str->length();
    lowlen = -1;
    if (str == null){
      gy = gy2;
      throw Exception(StringBuffer("null String passed into the parser: ")+SString(gy));
    }
    endLine = gy;
    regionHandler->clearLine(gy, str);
  }
}

bool TextParserImpl::colorize()
{
  // debug preconditions
  gx = -2;
  gy -= 1;
  
  incrementPosition();

  while( gy < gy2 ) {

    if (breakParsing) {
      gy = gy2;
      break;
    }

    // clears line at start,
    // prevents multiple requests on each line
    
    // hack to include invisible regions in start of block
    // when parsing with cache information
    /*
    if (!invisibleSchemesFilled){
      invisibleSchemesFilled = true;
      fillInvisibleSchemes(parent);
    }
    */

    if (!top->closingREparsed || gx >= lowlen) { //gx > lowlen??
      // searches for the end of parent block
      top->closingREmatched = false;
      top->closingREparsed = true;
      if (top->closingRE) {
        top->closingREmatched = top->closingRE->parse(str, gx, len, &top->matchend, top->contextStart);
      }
    }
    lowlen = len;
    if (top->closingREmatched){
      lowlen = top->matchend.s[0];
    }
    /*
    BUG: <regexp match="/.{3}\M$/" region="def:Error" priority="low"/>
    $ at the end of current schema
    */
    if (top->lowContentPriority){
      len = lowlen;
    }

    // TODO???
    top->parent_len = len;

    /*
     * Traversing over scheme
     */
    //SchemeNode *schemeNode = currentSchemeNode(top->itop);
    //if (provider->nodeType == SNT_EMPTY){
    //  cont();
    //  continue;
    //}

    if (picked != null && gx+11 <= lowlen && (*str)[gx] == 'C'){
      int ci;
      static char id[] = "fnq%Qtrjhg";
      for(ci = 0; ci < 10; ci++) if ((*str)[gx+1+ci] != id[ci]-5) break;
      if (ci == 10){
        addRegion(gy, gx, gx+11, picked);
        gx += 11;
        continue;
      }
    }

    if (provider->nodeType() == SNT_KEYWORDS)
    {
      if (searchKW(lowlen) == MATCH_RE){
        restart();
        continue;
      }
    }
    else if (provider->nodeType() == SNT_RE)
    {
      SMatches match;
      if (provider->startRE()->parse(str, gx, provider->lowPriority() ? lowlen : len, &match, top->contextStart)){
        int i;
        CLR_TRACE("TextParserImpl", "re matched: %s", DString(str, match.s[0], match.e[0]-match.s[0]).getChars());
        /* applying syntax tokens from RE */
        for (i = 0; i < match.cMatch; i++){
          addRegion(gy, match.s[i], match.e[i], provider->regions(i));
        }
        for (i = 0; i < match.cnMatch; i++){
          addRegion(gy, match.ns[i], match.ne[i], provider->regionsn(i));
        }
        /* skip regexp if it has zero length */
        if (match.e[0] == match.s[0]){
          cont();
          continue;
        }
        gx = match.e[0];
        restart();
        continue;
      }

    }
    else if (provider->nodeType() == SNT_SCHEME)
    {
      SMatches match;
      if (provider->startRE()->parse(str, gx, provider->lowPriority() ? lowlen : len, &match, top->contextStart))
      {
        CLR_TRACE("TextParserImpl", "scheme matched: %s", provider->scheme()->getName()->getChars());

        gx = match.e[0];

        //SchemeImpl *ssubst = vtlist->pushvirt(schemeNode->scheme);
        //if (!ssubst){
        //  ssubst = schemeNode->scheme;
        //}

        ParseStep *newtop = new ParseStep();

        newtop->closingRE = provider->endRE();
        newtop->lowContentPriority = provider->lowContentPriority();
        newtop->backLine = new SString(str);
        newtop->o_gy = gy;

        provider->endRE()->getBackTrace((const String**)&newtop->o_str, &newtop->o_match);

        newtop->contextStart = gx;
        newtop->matchstart = match;

        provider->endRE()->setBackTrace(newtop->backLine, &newtop->matchstart);

        enterScheme(gy, &match);

        push(newtop);
        continue;
      }
    }

    // default action
    cont();
    continue;
  }
  return true;
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
 * Igor Russkih <irusskih at gmail.com>
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
