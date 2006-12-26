<?xml version="1.0" encoding="windows-1251"?>
<stylesheet
     version="1.0"
     xmlns="http://www.w3.org/1999/XSL/Transform"
     xmlns:xs="http://www.w3.org/2001/XMLSchema"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <include href="xmlbin.params.xsl"/>

  <output method="text"/>

  <xsl:key name="ids" match="xs:element|xs:attribute" use="@name"/>

  <variable name="nodes" select="//(xs:element|xs:attribute)[generate-id(key('ids', @name)[1]) = generate-id(.)]"/>




  <template name="id">
    <param name="name"/>
    <value-of select='translate($name, "-:", "__")'/>
  </template>



  <template match="/xs:schema">

#include "xmlbin.h"

class BinaryXML_<value-of select="$root-element"/> : public BinaryXMLWriter {

public:

<apply-templates select="$nodes" mode="const"/>
#define TOKEN_VALUE_MAX <value-of select="count(//(following::xs:element|following::xs:attribute)[generate-id(key('ids', @name)[1]) = generate-id(.)])"/>

    const static char* TOKEN_VALUES[];

    DString *getToken(int id)
    {
        if (id &lt; 0 || id >= TOKEN_VALUE_MAX) return null;
        return new DString(TOKEN_VALUES[id]);
    }

protected:
<apply-templates/>

   void writeElementCount(Document *doc){
       int elcount = 0;
       bool skip_childred = false;

       Node *rmnext = doc->getDocumentElement();
       while(rmnext != doc &amp;&amp; rmnext != null)
       {
         if (!skip_childred){
           while(rmnext->getFirstChild() != null){
             rmnext = rmnext->getFirstChild();
           }
         };
         skip_childred = false;

         Node *el = rmnext->getNextSibling();
         if(el == null){
           el = rmnext->getParent();
           skip_childred = true;
         }
         elcount++;
         rmnext = el;
       }
       putInteger(elcount);
   }

public:

void toBinary(Document *doc, Writer *writer){
   setWriter(writer);

//   writeElementCount(doc);

   toBinary_<value-of select="$root-element"/>(doc->getDocumentElement());

   completed();
}

};

const char* BinaryXML_<value-of select="$root-element"/>::TOKEN_VALUES[] = {
<apply-templates select="$nodes" mode="const-arr"/>
};

  </template>


  <template match="xs:element|xs:attribute" mode="const">
#define TOKEN_VALUE_<call-template name='id'><with-param name="name" select="@name"/></call-template>
<text>  </text> <value-of select="count((preceding::xs:element|preceding::xs:attribute)[generate-id(key('ids', @name)[1]) = generate-id(.)])"/>
  </template>


  <template match="xs:element|xs:attribute" mode="const-arr">
"<value-of select="@name"/><text>",</text>
  </template>


  <template match="xs:element">
void toBinary_<call-template name='id'><with-param name="name" select="@name"/></call-template>(Element *el){

    Element *child = (Element*)el;

    <apply-templates select="." mode="per"/>

}
  </template>


  <template match="xs:complexType">
void toBinaryType_<call-template name='id'><with-param name="name" select="@name"/></call-template>(Element *el){
    <apply-templates select="." mode="per"/>
}
  </template>


  <template match="xs:complexType" mode="per">


    int attrCount = 0 <apply-templates mode="count-attr"/>;

    putInteger(attrCount);

    <apply-templates select="xs:attribute" mode="per"/>


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        <for-each select=".//xs:element">
        if (child->getNodeType() == Node::ELEMENT_NODE &amp;&amp;
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_<call-template name='id'><with-param name="name" select="@name"/></call-template>])){
            <apply-templates select="." mode="per"/>
        }
        </for-each>
        child = child->getNextSibling();
    }
  </template>



  <template match="xs:element" mode="per">

        putInteger(TOKEN_VALUE_<call-template name='id'><with-param name="name" select="@name"/></call-template>);

    <if test="@type">
        toBinaryType_<value-of select="@type"/>((Element*)child);
    </if>
    <if test="not(@type)">
    {
      Element *el = (Element*)child;
      <apply-templates select="xs:complexType" mode="per"/>
    }
    </if>
  </template>



  <template match="xs:attribute" mode="per">
    {
        putInteger(TOKEN_VALUE_<call-template name='id'><with-param name="name" select="@name"/></call-template>);
        const String *attr = el->getAttribute(&amp;DString(TOKEN_VALUES[TOKEN_VALUE_<call-template name='id'><with-param name="name" select="@name"/></call-template>]));
        putString(attr);
    }
  </template>



  <template match="xs:attribute" mode="count-attr">
    <text>+1</text>
  </template>

  <template match="xs:simpleContent" mode="count-attr">
    <apply-templates mode="count-attr"/>
  </template>

  <template match="xs:extension|xs:restriction" mode="count-attr">
    <variable name="base-type" select="@base"/>
    <apply-templates select="/xs:schema/xs:simpleType[@name = $base-type]/*" mode="count-attr"/>
    <message>WARNING: Not implemented</message>
  </template>

  <template match="xs:attributeGroup" mode="count-attr">
    <text>+1ag</text>
  </template>

  <template match="*|text()|comment()" mode="count-attr">
  </template>


  <template match="xs:simpleType">
  </template>





  <template match="text()">
  </template>
  <template match="text()" mode="per">
  </template>
  <template match="text()" mode="const">
  </template>

  <template match="comment()">
  </template>

</stylesheet>
