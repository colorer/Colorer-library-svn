#ifndef _COLORER_DynamicGrammarProvider
#define _COLORER_DynamicGrammarProvider

#include <common/DynamicArray.h>
#include <common/RefCounter.h>
#include <colorer/parsers/GrammarProvider.h>

struct ProviderStep;

/**
 * Provides parser with the grammar, based on HRC language layout.
 */
class DynamicGrammarProvider : public GrammarProvider
{
public:
    DynamicGrammarProvider(DynamicHRCModel *hrcModel, SchemeImpl *baseScheme);
    ~DynamicGrammarProvider();
    
    virtual bool nextItem();

    virtual void restart();
    virtual void reset();

    virtual void enterScheme();
    virtual void leaveScheme();

    virtual void *storeState();
    virtual void restoreState(void *p);
    virtual void freeState(void *p);

    virtual SchemeNodeType nodeType();

    virtual KeywordList *kwList();
    virtual CRegExp *startRE();
    virtual CRegExp *endRE();
    virtual const Scheme *scheme();

    virtual bool lowPriority();
    virtual bool lowContentPriority();
    virtual bool innerRegion();

    virtual const Region *region();
    virtual const Region *regions(int idx);
    virtual const Region *regionsn(int idx);
    virtual const Region *regione(int idx);
    virtual const Region *regionen(int idx);
protected:
    DynamicHRCModel *hrcModel;
    SchemeImpl *baseScheme;
    bool leaveBlockRequired;
    
    ProviderStep *top;

    /**
     * Virtualization/inheritance logic.
     * Searches tree for correct schema substitution for 'top' element
     */
    void adviceScheme(SchemeImpl *scheme);
    /**
     * Comes back from inheritance step
     */
    void popInherit();
    /**
     * Recursively enters all pending inheritance steps
     */
    void validateInherit();
private:
    int s_inheritLevel;

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