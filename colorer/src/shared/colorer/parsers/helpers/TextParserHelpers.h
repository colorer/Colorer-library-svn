#ifndef _COLORER_TEXTPARSERPELPERS_H_
#define _COLORER_TEXTPARSERPELPERS_H_

#include<colorer/parsers/HRCParserImpl.h>

#if !defined COLORERMODE || defined NAMED_MATCHES_IN_HASH
#error need (COLORERMODE & !NAMED_MATCHES_IN_HASH) in cregexp
#endif

/**
 * Internal parser's cache storage. Each object instance
 * stores parse information about single level of Scheme
 * objects. The object always takes more, than a single line,
 * because single-line scheme match doesn't need to be cached.
 *
 * @ingroup colorer_parsers
 */
class ParseCache
{
public:
  /** Start and end lines of this scheme match */
  int sline, eline;
  /** Scheme, matched for this cache entry */
  SchemeImpl *scheme;
  /** Particular parent block object, caused this scheme to
    * be instantiated.
    */
  const SchemeNode *clender;
  /**
   * RE Match object for start RE of the enwrapped &lt;block> object
   */
  SMatches matchstart;
  /**
   * Copy of the line with parent's start RE.
   */
  SString *backLine;

  /**
   * Tree structure references in parse cache
   */
  ParseCache *children, *next, *parent;
  ParseCache();
  ~ParseCache();
  /**
   * Searched a cache position for the specified line number.
   * @param ln     Line number to search for
   * @param cache  Cache entry, filled with last child cache entry.
   * @return       Cache entry, assigned to the specified line number
   */
  ParseCache *searchLine(int ln, ParseCache **cache);
};


/**
 * Sinle inheritance level inside of the parser state.
 * Used to track runtime scheme inheritance structure.
 */
struct InheritStep {
  /**
   * Inherited scheme
   */
  SchemeImpl *scheme;
  /**
   * Current parse position in this inherited scheme
   */
  int schemeNodePosition;
  //bool vtlistPushedVirtual;
  //bool vtlistPushed;
};

/**
 * Sinle step, or state, of the parser. Each state is described by a set of
 * attributes, which affect parsing process.
 */
struct ParseStep{
  CRegExp *closingRE;
  bool closingREmatched;
  bool closingREparsed;
  SMatches matchstart;
  SMatches matchend;

  bool lowContentPriority;
  int contextStart;

  int parent_len;

  InheritStep *inheritTop;
  Vector<InheritStep*> inheritStack;

  int o_gy;
  SMatches *o_match;
  DString *o_str;
  SString *backLine;

  ParseStep(){
    closingREmatched = closingREparsed = false;
    closingRE = null;
    backLine = null;
  }

  /**
   * Pushes addiitonal 'inherit' level into the current parse state.
   */
  void goInherit(InheritStep *istep){
    inheritStack.addElement(istep);
    inheritTop = istep;
  }

  /**
   * Removes topmost inherit level from current parse state.
   * InheritStep element itself is deleted also.
   */
  void leaveInherit(){
    delete inheritStack.lastElement();
    inheritStack.setSize(inheritStack.size()-1);
    if (inheritStack.size()){
      inheritTop = inheritStack.lastElement();
    }else{
      inheritTop = null;
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
