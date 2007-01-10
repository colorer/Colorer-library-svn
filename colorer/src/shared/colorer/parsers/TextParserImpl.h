#ifndef _COLORER_TEXTPARSERIMPL_H_
#define _COLORER_TEXTPARSERIMPL_H_

#include<colorer/TextParser.h>
#include<colorer/parsers/GrammarProvider.h>

struct ParseStep;
struct ParseCache;

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
  int endLine;

  GrammarProvider *provider;
  LineSource *lineSource;
  RegionHandler *regionHandler;

  bool breakParsing;
  const Region *picked;

  ParseStep *proot, *top;

  void fillInvisibleSchemes(ParseStep *step);
  void addRegion(int lno, int sx, int ex, const Region* region);
  void enterScheme(int lno, SMatches *match);
  void leaveScheme(int lno, SMatches *match);

  bool searchKW(int lowLen);

  ParseStep *searchTree(int ln, ParseStep *step);

  /** General highlighting loop */
  bool colorize();

  /**
   * Cleanup invalid nodes from previous parses on this level:
   * remove all sibling elements from this 'top' till the end.
   */
  void cleanupDeadNodes(ParseStep *node);

  /**
   * Enters new parser state \c step.
   * This step parameters should be already setted up.
   */
  void push(ParseStep *step);
  /**
   * Finishes current parser state and moves parser on parent level.
   */
  void removeTop();
  /**
   * moves to the next position in a parsed text
   */
  void incrementPosition();

  /**
   * Restart parser on the same position from current state
   * initial node.
   */
  void restart();
  /**
   * Moves single position forward and restarts parse process
   * from initial scheme node.
   * Before move checks for possible state finish condition. If found,
   * finishes current parse step and returns to parent step.
   */
  void move();
  /**
   * Moves to the next node in current parse tree,
   * Return to the parent node, if current inheritance node traverse is finished.
   */
  void cont();

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
