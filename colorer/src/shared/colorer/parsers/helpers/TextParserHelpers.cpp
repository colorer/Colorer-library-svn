
#include<colorer/parsers/helpers/TextParserHelpers.h>
#include<common/Logging.h>


/////////////////////////////////////////////////////////////////////////
// parser's cache structures
ParseCache::ParseCache()
{
  children = next = parent = null;
  backLine = null;
  vcache = 0;
}

ParseCache::~ParseCache()
{
  CLR_TRACE("TPCache", "~ParseCache():%s,%d-%d", scheme ? scheme->getName()->getChars() : "", sline, eline);
  delete backLine;
  backLine = (SString*)0xdead;
  delete children;
  delete next;
  delete[] vcache;
}

ParseCache *ParseCache::searchLine(int ln, ParseCache **cache)
{
ParseCache *r1, *r2, *tmp = this;
  *cache = null;
  while(tmp)
  {
    CLR_TRACE("TPCache", "  searchLine() tmp:%s,%d-%d", tmp->scheme ? tmp->scheme->getName()->getChars() : "", tmp->sline, tmp->eline);
    if (tmp->sline <=ln && tmp->eline >= ln) {
      r1 = tmp->children->searchLine(ln, &r2);
      if (r1){
        *cache = r2;
        return r1;
      }
      *cache = r2; // last child
      return tmp;
    }
    if (tmp->sline <= ln) *cache = tmp;
    tmp = tmp->next;
  }
  return null;
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
