#include<colorer/parsers/TextParserImpl.h>
#include<common/Logging.h>

#define MAX_LINE_LENGTH 0x7FFFFFF

#define dynamic_assert(a) if (!(a)) { throw Exception(DString("dynamic_assert: "#a)); }

/**
 * Sinle step, or state, of the parser. Each state is described by a set of
 * attributes, which affect parsing process.
 */
struct ParseStep
{
    /**
     * Tree structure references in parse cache
     */
    ParseStep *parent;
    ParseStep *children;
    ParseStep *next, *prev;
    
    GrammarProvider *provider;

    /* Data duplication with provider?? */
    CRegExp *schemeClosingRE;
    const Region *schemeRegion;
    const Scheme *scheme;

    bool closingREmatched;
    bool closingREparsed;
    SMatches matchstart;
    SMatches matchend;

    bool lowContentPriority;
    int contextStart;

    /** Start and end lines of this scheme match */
    int sline, eline;
    /** provider's state at this parse position */
    void *providerState;

    SMatches *o_match;
    DString *o_str;
    SString *backLine;

    ParseStep(GrammarProvider *provider) : provider(provider)
    {
        closingREmatched = closingREparsed = false;
        lowContentPriority = false;
        schemeClosingRE = null;
        schemeRegion = null;
        backLine = null;
        o_match = null;
        o_str = null;
        sline = 0;
        eline = MAX_LINE_LENGTH;
        children = next = prev = parent = null;
        providerState = null;
    }

    ~ParseStep()
    {
        if (providerState) provider->freeState(providerState);
        
        delete backLine;
        backLine = null;

        ParseStep *child_iter = children;
        while (child_iter) {
            ParseStep *child = child_iter;
            child_iter = child_iter->next;
            delete child;
            if (child_iter == children) break;
        }
    }

    void addChild(ParseStep *child)
    {
        if (children == null) {
            // Single child
            children = child;
            child->next = child;
            child->prev = child;
            child->parent = this;
            return;
        }
        ParseStep *last = children->prev;
        assert(last);
        // Add to the tail
        last->next = child;
        child->prev = last;
        child->next = children;
        child->parent = this;
        children->prev = child;
    }

};


TextParserImpl::TextParserImpl()
{
    CLR_TRACE("TextParserImpl", "constructor");
    proot = null;
    lineSource = null;
    regionHandler = null;
    picked = null;
    provider = null;
}

TextParserImpl::~TextParserImpl()
{
    delete proot;
    delete provider;
}

void TextParserImpl::setFileType(FileType *type)
{
    delete proot;

    delete provider;
    provider = type->createGrammarProvider();

    proot = new ParseStep(provider);
}

void TextParserImpl::setLineSource(LineSource *lh){
    lineSource = lh;
}

void TextParserImpl::setRegionHandler(RegionHandler *rh){
    regionHandler = rh;
}

ParseStep *TextParserImpl::searchTree(int ln, ParseStep *step)
{
    ParseStep *iterate = step;
    
    while(iterate)
    {
        //CLR_TRACE("TPCache", "  searchLine() tmp:%s,%d-%d", tmp->scheme->getName()->getChars(), tmp->sline, tmp->eline);
        if (iterate->sline == ln) return iterate->parent ? iterate->parent : iterate;
        if (iterate->sline < ln && iterate->eline >= ln)
        {
            ParseStep *child = searchTree(ln, iterate->children);
            if (child)
            {
                return child;
            }
            return iterate;
        }
        iterate = iterate->next;
        if (iterate == step) break;
    }
    return null;
};


int TextParserImpl::parse(int from, int num, TextParseMode mode)
{
    gx = 0;
    gy = from;
    gy2 = from+num;

    invisibleSchemesFilled = false;
    breakParsing = false;
    updateCache = (mode == TPM_CACHE_UPDATE);

    CLR_TRACE("TextParserImpl", "parse from=%d, num=%d", from, num);
    /* Check for initial bad conditions */
    dynamic_assert(regionHandler);
    dynamic_assert(lineSource);
    dynamic_assert(provider);

    lineSource->startJob(from);
    regionHandler->startParsing(from);

    provider->reset();
    
    top = searchTree(gy, proot);
    if (top->providerState != null)
    {
        provider->restoreState(top->providerState);
    }
    //all tree!
    ParseStep *child = top->children;
    while (child) {
        ParseStep *cc = child;
        child = child->next;
        delete cc;
        if (child == top->children) break;
    }
    top->children = null;

    fillInvisibleSchemes(top);

    colorize();

    /*

    if (mode == TPM_CACHE_READ || mode == TPM_CACHE_UPDATE) {

    }

    */

    regionHandler->endParsing(endLine);
    lineSource->endJob(endLine);

    return endLine;
}


void TextParserImpl::clearCache()
{
}


void TextParserImpl::breakParse()
{
  breakParsing = true;
}


void TextParserImpl::addRegion(int lno, int sx, int ex, const Region* region){
  if (sx == -1 || region == null) return;
  regionHandler->addRegion(lno, str, sx, ex, region);
}

void TextParserImpl::enterScheme(int lno, SMatches *match)
{
  int i;

  if (provider->innerRegion() == false){
    regionHandler->enterScheme(lno, str, match->s[0], match->e[0], provider->region(), provider->scheme());
  }

  for (i = 0; i < match->cMatch; i++)
    addRegion(lno, match->s[i], match->e[i], provider->regions(i));
  for (i = 0; i < match->cnMatch; i++)
    addRegion(lno, match->ns[i], match->ne[i], provider->regionsn(i));

  if (provider->innerRegion() == true){
    regionHandler->enterScheme(lno, str, match->e[0], match->e[0], provider->region(), provider->scheme());
  }
}

void TextParserImpl::leaveScheme(int lno, SMatches *match)
{
  int i;

  if (provider->innerRegion() == true){
    regionHandler->leaveScheme(gy, str, match->s[0], match->s[0], provider->region(), provider->scheme());
  }

  for (i = 0; i < match->cMatch; i++)
    addRegion(gy, match->s[i], match->e[i], provider->regione(i));
  for (i = 0; i < match->cnMatch; i++)
    addRegion(gy, match->ns[i], match->ne[i], provider->regionen(i));

  if (provider->innerRegion() == false){
    regionHandler->leaveScheme(gy, str, match->s[0], match->e[0], provider->region(), provider->scheme());
    if (provider->region() != null) picked = provider->region();
  }
}


void TextParserImpl::fillInvisibleSchemes(ParseStep *step)
{
    if (step->parent == null || step == proot){
        return;
    }
    /* Fills output stream with valid "pseudo" enterScheme */
    fillInvisibleSchemes(step->parent);
    regionHandler->enterScheme(gy, null, 0, 0, step->schemeRegion, step->scheme);
    return;
}

#define MATCH_NOTHING 0
#define MATCH_RE 1

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


void TextParserImpl::removeTop()
{
    if (top == null) {
      throw Exception(DString("Invalid parser state: ParseState::pop() from null"));
    }
    assert(top->parent);
    // Remove from sibling
    top->next->prev = top->prev;
    top->prev->next = top->next;
    // Remove from parent
    if (top->parent && top == top->parent->children) {
        if (top->next == top) top->next = null;
        top->parent->children = top->next;
    }

    ParseStep *parent = top->parent;
    delete top;
    top = parent;
  
}


void TextParserImpl::cont()
{
    provider->nextItem();
}

void TextParserImpl::restart()
{
    provider->restart();
}

void TextParserImpl::incrementPosition()
{
    gx++;
    CLR_TRACE("TextParserImpl", "X: %d", gx);
    /* Next line */
    if (gx < 0 || gx >= str->length())
    {
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
  // initial preconditions
  gx = -2;
  gy -= 1;
  
  incrementPosition();

  while( gy < gy2 ) {

    if (breakParsing) {
      gy = gy2;
      break;
    }

    if (!top->closingREparsed || gx > lowlen) {
      // searches for the end of parent block
      top->closingREmatched = false;
      top->closingREparsed = true;
      if (top->schemeClosingRE) {
        top->closingREmatched = top->schemeClosingRE->parse(str, gx, len, &top->matchend, top->contextStart);
      }
    }
    lowlen = len;
    if (top->closingREmatched){
      lowlen = top->matchend.s[0];
    }

    if (top->lowContentPriority){
      len = lowlen;
    }

    /*
     * Traversing over scheme
     */

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

    if (provider->nodeType() == SNT_EMPTY)
    {
        if (top->closingREmatched && gx+1 >= lowlen) 
        {
            /* Reached block's end RE */
            provider->leaveScheme();

            top->eline = gy;
            
            bool zeroLength = (top->matchstart.s[0] == top->matchend.e[0] && top->sline == top->eline);

            gx = top->matchend.e[0];
            leaveScheme(gy, &top->matchend);

            provider->endRE()->setBackTrace(top->o_str, top->o_match);

            if (top->eline == top->sline) {
                removeTop();
            }else{
                top = top->parent;
            }
            assert(top != null);
            top->closingREparsed = false;

            if (zeroLength) { 
                cont();
            }else {
                restart();
            }
        }else{
            incrementPosition();
            restart();
        }
        continue;
    }
    else if (provider->nodeType() == SNT_KEYWORDS)
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

        ParseStep *newtop = new ParseStep(provider);

        newtop->schemeClosingRE = provider->endRE();
        newtop->schemeRegion = provider->region();
        newtop->scheme = provider->scheme();
        newtop->lowContentPriority = provider->lowContentPriority();

        newtop->backLine = new SString(str);
        newtop->sline = gy;
        newtop->eline = MAX_LINE_LENGTH;

        provider->endRE()->getBackTrace((const String**)&newtop->o_str, &newtop->o_match);

        newtop->contextStart = gx;
        newtop->matchstart = match;

        provider->endRE()->setBackTrace(newtop->backLine, &newtop->matchstart);

        enterScheme(gy, &match);

        top->addChild(newtop);
        top = newtop;
        provider->enterScheme();
        top->providerState = provider->storeState();
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
