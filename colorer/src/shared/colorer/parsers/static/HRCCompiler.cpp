
#include<stdio.h>
#include<colorer/parsers/static/HRCCompiler.h>
#include<colorer/parsers/static/GrammarBuilder.h>
#include<colorer/parsers/dynamic/FileTypeImpl.h>


Vector<String*> schemeQueue;
//Vector<VirtualEntryVector**> vtlistQueue;

Hashtable<SchemeImpl*> schemeQueueHash;

HRCCompiler::HRCCompiler(HRCModel *hrcParser)
{
    this->hp = (DynamicHRCModel*)hrcParser;
    builder = null;
}

HRCCompiler::~HRCCompiler()
{

}

void HRCCompiler::compile(GrammarBuilder *builder)
{
    assert(hp);
    assert(this->builder == null);
    
    this->builder = builder;

    builder->onStart(hp);

    for(int idx = 0;; idx++){
      FileTypeImpl *type = (FileTypeImpl*)hp->enumerateFileTypes(idx);
      if (type == null ) break;
//      vtList = new VTList();
      enqueue((SchemeImpl*)type->baseScheme);
      compile();
//      delete vtList;
    };
    

    builder->onFinish();

}

void HRCCompiler::compile()
{
    for(int idx = 0; idx < schemeQueue.size(); idx++)
    {
        SchemeImpl *scheme = schemeQueueHash.get(schemeQueue.elementAt(idx));
        
        builder->schemeStart(schemeQueue.elementAt(idx), scheme);
        
//        vtList->clear();
//        vtList->restore(vtlistQueue.elementAt(idx));
        putScheme(scheme);
        
        builder->schemeEnd(schemeQueue.elementAt(idx), scheme);
        
//        delete[] vtlistQueue.elementAt(idx);
        delete schemeQueue.elementAt(idx);
    }
    schemeQueue.setSize(0);
//    vtlistQueue.setSize(0);
}

void HRCCompiler::putScheme(SchemeImpl *scheme)
{
    for (int idx = 0; idx < scheme->nodes.size(); idx++) 
    {
        SchemeImpl *subst;
        SchemeNode *node = scheme->nodes.elementAt(idx);
        switch(node->type)
        {
            case SNT_RE:
                builder->visitRE(node->start_re, node);
                break;
            case SNT_SCHEME:
//                subst = vtList->pushvirt(node->scheme);
                if (!subst) subst = node->scheme;
                
                builder->visitBlock(qualifyName(subst), node);

                enqueue(subst);
//                if (subst != node->scheme) vtList->popvirt();
                break;
            case SNT_KEYWORDS:
                builder->visitKeywords(node);
                break;
            case SNT_INHERIT:
//                subst = vtList->pushvirt(node->scheme);
                // TODO: Recursion validation??
                if (!subst){
//                  bool b = vtList->push(node);
                  putScheme(node->scheme);
//                  if (b) vtList->pop();
                }else{
                  putScheme(subst);
//                  vtList->popvirt();
                };
                break;
        }
    }
}

void HRCCompiler::enqueue(SchemeImpl *scheme)
{
    String *qname = qualifyName(scheme);
    
    if (schemeQueueHash.get(qname) == null)
    {
        schemeQueue.addElement(qname);
//        vtlistQueue.addElement(vtList->store());
        schemeQueueHash.put(qname, scheme);
    }
}

String *HRCCompiler::qualifyName(SchemeImpl *scheme)
{
    StringBuffer *sb = new StringBuffer(scheme->getName());
/*
    for(VTList *list = vtList->next; list; list = list->next){
      
      for(int idx = 0; idx < list->vlist->size(); idx++){
          sb->append(DString("@@"));
          sb->append(list->vlist->elementAt(idx)->virtScheme->getName());
          sb->append(DString("@"));
          sb->append(list->vlist->elementAt(idx)->substScheme->getName());
      }

      if (list == vtList->last) break;
    };
*/

    return (String*)sb;
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
 * Igor Russkih <irusskih at gmail dot com>
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