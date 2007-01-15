#ifndef _COLORER_GRAMMARPROVIDER_H_
#define _COLORER_GRAMMARPROVIDER_H_

#include <cregexp/cregexp.h>
#include <colorer/parsers/helpers/HRCParserHelpers.h>

/**
 * Derivates should provides parser with the grammar, from the random grammar source.
 */
class GrammarProvider {

public:
    /**
     * Move one element forward
     */
    virtual bool nextItem() = 0;

    /**
     * Restart current context, reset its current position
     */
    virtual void restart() = 0;
    /**
     * Resets all the context information - drops to the base context
     */
    virtual void reset() = 0;

    /**
     * Enter the scheme (context). Current node must be SNT_SCHEME
     */
    virtual void enterScheme() = 0;
    /**
     * Leaves the current scheme (context).
     */
    virtual void leaveScheme() = 0;

    /**
     * Stores GrammarProvider's state to allow incremental parsing from
     * the stored position. Returned handle should be freed later with the freeState.
     */
    virtual void *storeState() = 0;
    /**
     * Restores GrammarProvider's state and allow incremental parsing from the saved position in the grammar
     */
    virtual void restoreState(void *p) = 0;
    /**
     * Frees previously stored state
     */
    virtual void freeState(void *p) = 0;

    /**
     * Current node's type
     */
    virtual SchemeNodeType nodeType() = 0;

    /**
     * Returns list of the keywords for SNT_KW
     */
    virtual KeywordList *kwList() = 0;
    /**
     * Returns starting RE token for the SNT_SCHEME node
     */
    virtual CRegExp *startRE() = 0;
    /**
     * Returns closing RE token for the SNT_SCHEME node
     */
    virtual CRegExp *endRE() = 0;
    /**
     * Returns scheme to jump to for the SNT_SCHEME node
     */
    virtual const Scheme *scheme() = 0;

    /**
     * If scheme or RE lowprioritized
     */
    virtual bool lowPriority() = 0;
    /**
     * If scheme to jump to has a low content priority
     */
    virtual bool lowContentPriority() = 0;
    /**
     * If scheme to jump to has an inner region property
     */
    virtual bool innerRegion() = 0;

    /**
     * Base Region for RE or schema node
     */
    virtual const Region *region() = 0;
    /**
     * Bracketed Regions for RE or schema node
     */
    virtual const Region *regions(int idx) = 0;
    /**
     * Named bracket Regions for schema node
     */
    virtual const Region *regionsn(int idx) = 0;
    /**
     * Bracketed Regions for schema node end token
     */
    virtual const Region *regione(int idx) = 0;
    /**
     * Named bracket Regions for schema node end token
     */
    virtual const Region *regionen(int idx) = 0;
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