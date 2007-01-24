
#include <colorer/parsers/dynamic/DynamicGrammarProvider.h>
#include<common/Logging.h>

/**
 * HRC traverse node element. Reference counting strategy is used to free ProviderStep objects.
 */
struct ProviderStep : RefCounter
{
    /** Parent in the hierarchy */
    ProviderStep *parent;
    /** Parent in the currently active virtualization tree */
    ProviderStep *virtualPrev;
    /** Active node position of used <inherit> in that virtualPrev reference */
    SchemeNode *nodePrev;

    /** true for 'abstract' inheritance steps,
     *  false for 'real' scheme switch steps
     */
    bool inheritStep;
    /** This node's schema */
    SchemeImpl *scheme;
    /** This node schema's position */
    int nodePosition;
    /** Schema's position for parent node (to keep virtual linked lists) */
    int parentNodePosition;

    ProviderStep()
    {
        parent = null;
        virtualPrev = null;
        nodePrev = null;
    }

    /** Destructor, removes references */
    ~ProviderStep()
    {
        if (parent) parent->rmref();
        if (virtualPrev) virtualPrev->rmref();
    }

    /** Destructor, removes references */
    void initNodePosition()
    {
        nodePosition = 0;
        // Recursive node position storage in child
        assert(parent != null);
        assert(parent->nodePosition != -1);
        // save parent's node position, since it can be overwritten by other saved parses.
        parentNodePosition = parent->nodePosition;
        // initial value
        parent->nodePosition = -1;
    }
};



DynamicGrammarProvider::DynamicGrammarProvider(DynamicHRCModel *hrcModel, SchemeImpl *baseScheme) :
    hrcModel(hrcModel), baseScheme(baseScheme), top(null), leaveBlockRequired(false),
    inheritLevel(0)
{
    top = new ProviderStep();
    
    top->addref();

    top->inheritStep = false;
    top->scheme = baseScheme;
    top->nodePosition = top->parentNodePosition = 0;
    top->parent = null;
    validateInherit();
}

DynamicGrammarProvider::~DynamicGrammarProvider()
{
    top->rmref();
    top = null;
}

void DynamicGrammarProvider::popInherit()
{
    assert(top->inheritStep);
    
    inheritLevel--;
    CLR_TRACE("DGP", "traverse inherit %03d<<%s", inheritLevel, top->scheme->getName()->getChars());
    
    ProviderStep *newtop = top->parent;
    newtop->addref();
    newtop->nodePosition = top->parentNodePosition;

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
    
    // checks for recursion
    if (top->parent && top->scheme == top->parent->scheme && top->inheritStep) {
        throw Exception(StringBuffer("DynamicGrammarProvider: Self loop: ")+top->scheme->getName());
    }
    if (inheritLevel > DGP_MAX_INHERIT_LEVEL){
        StringBuffer s("DynamicGrammarProvider: DGP_MAX_INHERIT_LEVEL:");
        
        ProviderStep *track = top;
        int idx = 20;
        
        while(idx--) {
            s.append(DString(", ")) + track->scheme->getName();
            track = track->parent;
        }
        throw Exception(s);
    }

    assert(top->scheme != null);
    if (top->scheme->nodes.size() == 0) return;

    SchemeNode *node = top->scheme->nodes.elementAt(top->nodePosition);

    if (node->type == SNT_INHERIT) {
        
        if (node->scheme == null) {
            nextItem();
            return;
        }
        
        ProviderStep *newtop = new ProviderStep();
        
        newtop->parent = top;
        top->addref();

        // reassign
        top->rmref();
        top = newtop;
        top->addref();

        top->inheritStep = true;
        
        top->initNodePosition();

        adviceScheme(node);
        
        inheritLevel++;
        CLR_TRACE("DGP", "traverse inherit %03d>>%s", inheritLevel, top->scheme->getName()->getChars());
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

void DynamicGrammarProvider::reset()
{
    while (top->parent != null) {
        while (top->inheritStep) {
            popInherit();
        }
        ProviderStep *newtop = top->parent;
        if (newtop != null) {
            newtop->addref();
            newtop->nodePosition = top->parentNodePosition;
            top->rmref();
            top = newtop;
        }
    }
    validateInherit();
}


void DynamicGrammarProvider::adviceScheme(SchemeNode *rootnode)
{
    // final scheme to apply
    SchemeImpl *ret = rootnode->scheme;
    // step, containing applied virtualization node
    ProviderStep *curvl = null;
    SchemeNode *curnode = null;
                                    
    // Initial conditions to search
    ProviderStep *vl = top->parent->virtualPrev;
    SchemeNode *node = top->parent->nodePrev;

    // skip virtualization for <include> tags
    if (rootnode->type == SNT_INHERIT && rootnode->includeTag) {
        vl = null;
    }
    
    // Searching inheritance tree back to top to make all required schema virtualizations
    while(vl != null)
    {
        assert(node->type == SNT_INHERIT || node->type == SNT_SCHEME);
        
        for(int idx = 0; idx < node->virtualEntryVector.size(); idx++)
        {
            VirtualEntry *ve = node->virtualEntryVector.elementAt(idx);
            if (ret == ve->virtScheme && ve->substScheme){
              ret = ve->substScheme;
              curvl = vl;
              curnode = node;
            }
        }
        node = vl->nodePrev;
        vl = vl->virtualPrev;
    }
    
    // top->virtualPrev will store link to the next active virtualization node
    top->virtualPrev = curvl ? curvl : top->parent;
    if (top->virtualPrev) top->virtualPrev->addref();
    top->nodePrev = curvl ? curnode : top->parent->scheme->nodes.elementAt(top->parentNodePosition);

    top->scheme = ret;
}

void DynamicGrammarProvider::enterScheme()
{
    assert(!leaveBlockRequired);

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
    top->initNodePosition();

    adviceScheme(node);
    assert(top->scheme);

    CLR_TRACE("DGP", "traverse block >>%s (%s)", top->scheme->getName()->getChars(), DString(node->scheme->getName()).getChars());

    validateInherit();
}

void DynamicGrammarProvider::leaveScheme()
{
    leaveBlockRequired = false;
    
    // Rolls back all inherit steps
    while (top->inheritStep) {
        popInherit();
    }

    CLR_TRACE("DGP", "traverse block <<%s", top->scheme->getName()->getChars());

    ProviderStep *newtop = top->parent;
    
    assert(newtop != null);
    newtop->addref();

    newtop->nodePosition = top->parentNodePosition;

    top->rmref();
    top = newtop;
}

void *DynamicGrammarProvider::storeState()
{
    top->addref();
    CLR_TRACE("DGP", "storeState, scheme=%s", top->scheme->getName()->getChars());
    return (void*)top;
}

void DynamicGrammarProvider::restoreState(void *p)
{
    if (top) top->rmref();
    top = (ProviderStep*)p;
    top->addref();
    CLR_TRACE("DGP", "restoreState, scheme=%s", top->scheme->getName()->getChars());
}

void DynamicGrammarProvider::freeState(void *p)
{
    ProviderStep *step = (ProviderStep*)p;
    step->rmref();
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