#ifndef _COLORER_DYNAMICBUILDER_H_
#define _COLORER_DYNAMICBUILDER_H_

#include <colorer/parsers/HRCCompiler.h>
#include <common/DynamicArray.h>

enum {
    PN_RE,
    PN_KW,
    PN_BLOCK,
    PN_BLOCK_DIRTY,
    PN_SCHEME_END,
};

enum {
    PN_FLAG_INNER,
    PN_FLAG_LOW_PRIOR,
    PN_FLAG_LOW_CONTENT_PRIOR,
};

struct ParserNode
{
    int target_node;

    int region_node;

    int start_node, end_node;
    
    char type;
    char flag;
};

struct RegionNode
{
    int region;
    int regions[REGIONS_NUM];
    int regionsn[NAMED_REGIONS_NUM];
    int regione[REGIONS_NUM];
    int regionen[NAMED_REGIONS_NUM];
};

#define MAX_REGION_LENGTH 64

struct RegionDescription
{
    wchar name[MAX_REGION_LENGTH];
};

class DynamicBuilder: public GrammarBuilder
{
public:
    DynamicBuilder();
    
    virtual ~DynamicBuilder();

    void onStart(HRCParser *hrcParser);

    void onFinish();

    void schemeStart(String *schemeName, Scheme *scheme);

    void schemeEnd(String *schemeName, Scheme *scheme);

    void visitRE(String *re, SchemeNode *node);

    void visitBlock(String *scheme, SchemeNode *node);

    void visitKeywords(SchemeNode *node);
protected:
    int getREindex(String *re);
    int getRegionIndex(SchemeNode *node);

    Hashtable <int> schemeHash;
    Hashtable <int> reHash;
    Vector<String*> reVector;
    int reHashIndex;

    DynamicArray<ParserNode> *parserNodes;
    DynamicArray<RegionNode> *regionNodes;
    DynamicArray<RegionDescription> *regionDescriptions;

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
 * Igor Russkih <irusskih at gmail dot com>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2006
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
