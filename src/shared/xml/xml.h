#ifndef _COLORER_XML_H_
#define _COLORER_XML_H_
#include<common/Hashtable.h>

/** @addtogroup xml XML Parser
    Simple XML Parser class.
    Doesn't supports any of w3c standards, just
    creates tree of elements and their attributes.
*/


/// saves all the characters in document
//#define SAVE_WHITESPACES
/// saves all comment nodes in document
//#define SAVE_COMMENTS

//#define MAXPARAMS 0x20
#define MAXTAG 0x20

enum ElType{
  EL_ROOT, EL_SINGLE, EL_BLOCKED, EL_PLAIN, EL_PI, EL_COMMENT
};

/** Simple XML Parser implementaion

@par Features:
<ol>
<li> Unicode support.
According to the  w3c  Recommendations,  this parser  supports  most
Unicode  character encodings. They are detected in next order:
   - Through the byte order mark (U+FEFF) or sequence "<?".
     UTF-8, UTF-16 (LE and BE), UTF-32 (LE and BE) charsets are detected
     in this way.
   - With "charset" parameter in xml declaration PI:
     samples:
       - <?xml charset="UTF-8"?>
       - <?xml charset="cp1251"?>
     Only UTF-8 and ANSI codepages are detected with this rule.
     All ANSI codepages are come from Colorer Unicode support classes.
   - Explicit ANSI codepage information from application, wich uses parser,
     with CXmlEl::parse() method.
   - In case of unavailable charset information, UTF-8 encoding is used.


<li> Entities and References parsing.
   - Simple Character References:
     - &#x0431;
     - &#32;
   - According to the w3c Predefined Entities, next DTD is used implicitly:
     - <!ENTITY lt     "&#38;#60;">
     - <!ENTITY gt     "&#62;">
     - <!ENTITY amp    "&#38;#38;">
     - <!ENTITY apos   "&#39;">
     - <!ENTITY quot   "&#34;">

<li> W3C CDATA section are supported:
     - <![CDATA[    This is an \<unescaped> data    ]]>
</ol>



@par Dislikes:
<ol>
<li> This parser doesn't conform w3c Recommendations.
     It is neither well-forming nor validating parser.
     So, it can parse any HTML-like code.

<li> Parser treats comment nodes as simple text nodes
     According to this, all entities and char references in
     comments would be replaced.

<li> All PI nodes are treated as simple element nodes with
     names "?PIName" and parameters.

<li> Parser allows any non-Whitespace characters in node
     and parameter names. It doesn't checks character class
     in document.
     At all, any stream of bytes would be transformed
     into some kind of tree.
</ol>

    @ingroup xml
*/
class CXmlEl
{
public:
  CXmlEl();
  ~CXmlEl();
  /** Base parse method.
      constructs XML tree from @c src byte stream.
      Object, for which this method was called, becomes
      root of new XML tree.
      All created xml elements accessible from it's next()
      element.
      @param src Byte stream of xml source.
      @param sz Size of input source in bytes.
      @param codepage Default codepage to use.
  */
  bool parse(const byte *src, int sz, const char *codepage = 0);

  /** Gets element's parent node */
  virtual CXmlEl *parent();
  /** Gets element's following sibling node */
  virtual CXmlEl *next();
  /** Gets element's preceding sibling node */
  virtual CXmlEl *prev();
  /** Gets element's first child node */
  virtual CXmlEl *child();

  /** Current element's type */
  ElType getType();
  /** Gets element's name */
  const String *getName();
  /** Gets content of character data nodes */
  const String *getContent();

  /** Gets element's number of parameters */
  int getParamCount();
  /** Returns name of parameter with index @c no */
  const String *getParamName(int no);
  /** Returns value of parameter with index @c no */
  const String *getParamValue(int no);
  /** Returns parameter's value by name */
  const String *getParamValue(const String &par);
  /** Returns integer parameter's value */
  bool getParamValue(const String &par, int *result);
  /** Returns double parameter's value */
  bool getParamValue(const String &par, double *result);

  /** Searches for @c tagname element
      @deprecated Old unused method.
  */
  CXmlEl *search(const String &tagname);
  /** Enumerates element's childred by @c no index
  */
  CXmlEl *enumChildred(int no);
  /** Moving in full tree hierarchy */
  virtual CXmlEl *fPrev();
  /** Moving in full tree hierarchy */
  virtual CXmlEl *fNext();
  /** Moving in full tree hierarchy */
  virtual CXmlEl *fFirst();
  /** Moving in full tree hierarchy */
  virtual CXmlEl *fLast();

protected:
  /** for derived classes */
  virtual bool init();
  /** for derived classes */
  virtual CXmlEl *createNew(ElType type, CXmlEl *parent, CXmlEl *after);

  /** Recursive object cleanup */
  void destroyLevel();
  void insert(CXmlEl *El);

  String *content;
  String *name;

  CXmlEl *eparent;
  CXmlEl *enext;
  CXmlEl *eprev;
  CXmlEl *echild;
  int chnum;
  ElType type;

  Hashtable<const String*> params;
  friend class CXmlEdit;
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
 * Cail Lomecb <cail@nm.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2003
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
