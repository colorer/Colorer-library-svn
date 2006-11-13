
#include <colorer/parsers/dynamic/DynamicGrammarProvider.h>
#include<common/Logging.h>

struct ProviderStep : RefCounter
{
    ProviderStep *parent;
    ProviderStep *virtualPrev;

    bool inheritStep;
    bool virtualized, virtPushed;
    SchemeImpl *scheme;
    int nodePosition;

    ProviderStep()
    {
        parent = null;
        virtualPrev = null;
    }

    ~ProviderStep()
    {
        if (parent) parent->rmref();
        if (virtualPrev) virtualPrev->rmref();
    }
};



DynamicGrammarProvider::DynamicGrammarProvider(DynamicHRCModel *hrcModel, SchemeImpl *baseScheme) :
    hrcModel(hrcModel), baseScheme(baseScheme), top(null), leaveBlockRequired(false)
{
}

DynamicGrammarProvider::~DynamicGrammarProvider()
{
}

void DynamicGrammarProvider::popInherit()
{
    assert(top->inheritStep);
    
    /*
    if (top->virtualized)
    {
        vtList.popvirt();
    }
    if (!top->virtualized && top->virtPushed)
    {
        vtList.pop();
    }
    */
    
    CLR_TRACE("DGP", "traverse inherit <<%s", top->scheme->getName()->getChars());
    
    ProviderStep *newtop = top->parent;
    newtop->addref();

    top->rmref();
    top = newtop;
}

void DynamicGrammarProvider::validateInherit()
{
    // Check agains empty sets
    if (top->inheritStep && top->nodePosition == 0 && top->scheme->nodes.size() == 0) {
        popInherit();
        nextItem();
        return;
    }

    if (top->scheme->nodes.size() == 0) return;

    SchemeNode *node = top->scheme->nodes.elementAt(top->nodePosition);

    if (node->type == SNT_INHERIT) {
        ProviderStep *newtop = new ProviderStep();
        
        newtop->parent = top;
        top->addref();

        // reassign
        top->rmref();
        top = newtop;
        top->addref();

        top->inheritStep = true;
        
        adviceScheme(node->scheme);

        /*
        top->virtPushed = false;
        top->virtualized = true;
        top->scheme = vtList.pushvirt(node->scheme);
        if (!top->scheme) {
            top->virtualized = false;
            top->virtPushed = vtList.push(node);
            top->scheme = node->scheme;
        }
        */
        
        top->nodePosition = 0;
        
        CLR_TRACE("DGP", "traverse inherit >>%s", top->scheme->getName()->getChars());
        validateInherit();
    }
}

bool DynamicGrammarProvider::nextItem()
{
    if (leaveBlockRequired) return false;

    top->nodePosition++;
    if (top->nodePosition >= top->scheme->nodes.size())
    {
        if (top->inheritStep) {
            popInherit();
            return nextItem();
        }else{
            //mark, do nothing
            leaveBlockRequired = true;
            return false;
        }
    }
    validateInherit();
    return true;
}

void DynamicGrammarProvider::restart()
{
    while (top->inheritStep) {
        popInherit();
    }
    top->nodePosition = 0;
    leaveBlockRequired = false;
    validateInherit();
}                              	




void DynamicGrammarProvider::adviceScheme(SchemeImpl *scheme)
{
    SchemeImpl *ret = scheme;
    ProviderStep *curvl = null;
    
    for(ProviderStep *vl = top->parent->virtualPrev; vl; vl = vl->virtualPrev)
    {
        SchemeNode *node = vl->scheme->nodes.elementAt(vl->nodePosition);
        assert(node->type == SNT_INHERIT || node->type == SNT_SCHEME);
        
        for(int idx = 0; idx < node->virtualEntryVector.size(); idx++)
        {
            VirtualEntry *ve = node->virtualEntryVector.elementAt(idx);
            if (ret == ve->virtScheme && ve->substScheme){
              ret = ve->substScheme;
              curvl = vl;
            }
        }
    }
    
    top->virtualPrev = curvl ? curvl->virtualPrev : top->parent;
    if (top->virtualPrev) top->virtualPrev->addref();

    top->scheme = ret;
}

void DynamicGrammarProvider::enterBlock()
{
    assert(!leaveBlockRequired);

    if (top == null) {
        top = new ProviderStep();
        
        top->addref();

        top->inheritStep = false;
        top->scheme = baseScheme;
        top->virtualized = top->virtPushed = false;
        top->nodePosition = 0;
        validateInherit();
        return;
    }
    
    SchemeNode *node = top->scheme->nodes.elementAt(top->nodePosition);
    assert(node->type == SNT_SCHEME);
    
    ProviderStep *newtop = new ProviderStep();
    
    newtop->parent = top;
    top->addref();

    // reassign
    top->rmref();
    top = newtop;
    top->addref();

    top->inheritStep = false;
    top->nodePosition = 0;

    adviceScheme(node->scheme);
    assert(top->scheme);

    CLR_TRACE("DGP", "traverse block >>%s (%s)", top->scheme->getName()->getChars(), node->scheme->getName()->getChars());

    validateInherit();
}

void DynamicGrammarProvider::leaveBlock()
{
    leaveBlockRequired = false;
    
    // Rolls back all inherit steps
    while (top->inheritStep) {
        popInherit();
    }

    CLR_TRACE("DGP", "traverse block <<%s", top->scheme->getName()->getChars());

    ProviderStep *newtop = top->parent;
    
    if (newtop) newtop->addref();

    top->rmref();
    top = newtop;
}


void *DynamicGrammarProvider::storeState()
{
    assert(!"never reach");
    top->addref();
    return (void*)top;
}

void DynamicGrammarProvider::restoreState(void *p)
{
    if (top) top->rmref();

    top = (ProviderStep*)p;
    
    assert(!"never reach");
}



SchemeNodeType DynamicGrammarProvider::nodeType()
{
    if (top->nodePosition >= top->scheme->nodes.size()) {
        return SNT_EMPTY;
    }
    SchemeNodeType type = top->scheme->nodes.elementAt(top->nodePosition)->type;
    assert(type != SNT_INHERIT);
    assert(type != SNT_EMPTY);
    return type;
}

KeywordList *DynamicGrammarProvider::kwList()
{
    assert(nodeType() == SNT_KEYWORDS);
    return top->scheme->nodes.elementAt(top->nodePosition)->kwList;
}

CRegExp *DynamicGrammarProvider::startRE()
{
    assert(nodeType() == SNT_RE || nodeType() == SNT_SCHEME);
    return top->scheme->nodes.elementAt(top->nodePosition)->start;
}

CRegExp *DynamicGrammarProvider::endRE()
{
    assert(nodeType() == SNT_SCHEME);
    return top->scheme->nodes.elementAt(top->nodePosition)->end;
}

const Scheme *DynamicGrammarProvider::scheme()
{
    assert(nodeType() == SNT_SCHEME);
    return top->scheme->nodes.elementAt(top->nodePosition)->scheme;
}

bool DynamicGrammarProvider::lowPriority()
{
    assert(nodeType() == SNT_RE || nodeType() == SNT_SCHEME);
    return top->scheme->nodes.elementAt(top->nodePosition)->lowPriority;
}

bool DynamicGrammarProvider::lowContentPriority()
{
    assert(nodeType() == SNT_SCHEME);
    return top->scheme->nodes.elementAt(top->nodePosition)->lowContentPriority;
}

bool DynamicGrammarProvider::innerRegion()
{
    assert(nodeType() == SNT_SCHEME);
    return top->scheme->nodes.elementAt(top->nodePosition)->innerRegion;
}

const Region *DynamicGrammarProvider::region()
{
    //assert(nodeType() == SNT_SCHEME);
    return top->scheme->nodes.elementAt(top->nodePosition)->region;
}

const Region *DynamicGrammarProvider::regions(int idx)
{
    return top->scheme->nodes.elementAt(top->nodePosition)->regions[idx];
}

const Region *DynamicGrammarProvider::regionsn(int idx)
{
    return top->scheme->nodes.elementAt(top->nodePosition)->regionsn[idx];
}

const Region *DynamicGrammarProvider::regione(int idx)
{
    return top->scheme->nodes.elementAt(top->nodePosition)->regione[idx];
}

const Region *DynamicGrammarProvider::regionen(int idx)
{
    return top->scheme->nodes.elementAt(top->nodePosition)->regionen[idx];
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