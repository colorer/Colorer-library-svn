<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
     version="1.0"
     exclude-result-prefixes="c hrc xsl"
     xmlns="http://colorer.sf.net/2003/hrc"
     xmlns:c="uri:colorer:custom"
     xmlns:hrc="http://colorer.sf.net/2003/hrc"
     xmlns:xs="http://www.w3.org/2001/XMLSchema"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:include href="xsd2hrc.params.xsl"/>
  <xsl:include href="replace-string.xsl"/>

  <xsl:include href="xsd2hrc.callable.xsl"/>
  <xsl:include href="xsd2hrc.include-content.xsl"/>
  <xsl:include href="xsd2hrc.include-attr.xsl"/>
  <xsl:include href="xsd2hrc.typedefs.xsl"/>
  <xsl:include href="xsd2hrc.root.xsl"/>
  <xsl:include href="xsd2hrc.root-elements.xsl"/>


  <xsl:output indent="yes"
              doctype-public="-//Cail Lomecb//DTD Colorer HRC take5//EN"
              doctype-system="http://colorer.sf.net/2003/hrc.dtd"  encoding="utf-8"/>

  <!-- ignore spaces -->

  <xsl:strip-space elements="*"/>

  <!-- version -->
  <xsl:variable name="version" select="'0.9'"/>

  <!-- path to prototype catalog -->
  <xsl:variable name="catalog" select="document($catalog-path)"/>

  <!-- path to replace patterns list -->
  <xsl:variable name="replace-patterns" select="document('xsd2hrc.replace-patterns.xml')"/>

  <!-- Schema's targetNamespace
    In case of noTargetNamespace schema, transformation results
    may need some manual fixes. Also it is impossible to link
    such a schema with other generated schemas
  -->
  <xsl:variable name="targetNamespace" select="string(/xs:schema/@targetNamespace)"/>

  <!-- path to custom defines catalog -->
  <xsl:variable name="custom-type" select="document($custom-defines)/c:custom/c:custom-type[@targetNamespace = $targetNamespace]"/>
  <xsl:variable name="custom-type-schemes" select="$custom-type/hrc:type/*"/>


  <!-- possible used namespace prefixes -->

  <xsl:variable name="ns-map" select="$custom-type/c:prefix | $custom-type/c:empty-prefix | $custom-type/c:any-prefix"/>
  <xsl:variable name="ns-real-prefix">
    <xsl:if test="$ns-map">
      <xsl:text>(?{}</xsl:text>
      <xsl:choose>
        <xsl:when test="$ns-map/self::c:any-prefix">
          <xsl:text>%xml:NCName;</xsl:text>
        </xsl:when>
        <xsl:when test="$ns-map/self::c:prefix">
          <xsl:text>(?{}</xsl:text>
          <xsl:for-each select="$ns-map/self::c:prefix">
            <xsl:value-of select="."/>
            <xsl:if test="position() != last()">|</xsl:if>
          </xsl:for-each>
          <xsl:text>)</xsl:text>
        </xsl:when>
      </xsl:choose>
      <xsl:text>:)</xsl:text>
    </xsl:if>
  </xsl:variable>
  <xsl:variable name="nsprefix">
    <xsl:if test="$ns-map">
      <xsl:value-of select="$ns-real-prefix"/>
      <xsl:if test="$ns-map/self::c:empty-prefix">?</xsl:if>
    </xsl:if>
  </xsl:variable>

  <!-- New line character -->
  <xsl:template name="crlf">
    <xsl:text>&#10;</xsl:text>
  </xsl:template>

  <!-- insert comments -->
  <xsl:template match="comment()">
    <xsl:call-template name='crlf'/><xsl:copy/><xsl:call-template name='crlf'/>
  </xsl:template>


  <!-- root template
  Creates basic defines, schemes,
  and calls recursive processing
  -->

  <xsl:template match="/">
    <xsl:if test="$hrctype = 'default-type'">
      <xsl:message>warning: Default Type Name is not specified (use param hrctype=name) </xsl:message>
    </xsl:if>

    <hrc version="take5"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://colorer.sf.net/2003/hrc http://colorer.sf.net/2003/hrc.xsd">

      <xsl:if test="$include-prototype = 'yes'">
        <prototype name="{$hrctype}" group="group" description="{$hrctype}" targetNamespace="{$targetNamespace}">
          <location link="link-to-{$hrctype}/>"/>
          <filename>/ filemask /ix</filename>
        </prototype>
      </xsl:if>

      <xsl:if test="$include-prototype = 'no'">
<xsl:call-template name='crlf'/>
<xsl:comment>
  insert this define into HRC base catalog file (colorer.hrc)

  &lt;prototype name="<xsl:value-of select='$hrctype'/>" group="group" description="<xsl:value-of select='$hrctype'/>" targetNamespace="<xsl:value-of select='$targetNamespace'/>">
    &lt;location link="<xsl:value-of select='$hrctype'/>.hrc"/>
    &lt;filename>/\./ix&lt;/filename>
  &lt;/prototype>
</xsl:comment>
      </xsl:if>

      <type name="{$hrctype}">
        <annotation>
         <documentation>
           XSLT Generated HRC scheme for language '<xsl:value-of select="$hrctype"/>'
           from XML Schema with xsd2hrc.xsl version <xsl:value-of select="$version"/> (c) Cail Lomecb

           Scheme parameters:
             targetNamespace             : <xsl:value-of select="$targetNamespace"/>
             hrctype                     : <xsl:value-of select="$hrctype"/>
             allow-common-attr           : <xsl:value-of select="$allow-common-attr"/>
             allow-any-attr              : <xsl:value-of select="$allow-any-attr"/>
             allow-unknown-elements      : <xsl:value-of select="$allow-unknown-elements"/>
             allow-unknown-root-elements : <xsl:value-of select="$allow-unknown-root-elements"/>
             force-single-root           : <xsl:value-of select="$force-single-root"/>
             default prefixes            : <xsl:value-of select="$nsprefix"/>
               you can change them with entity 'nsprefix'

         </documentation>
         <documentation>
          Schema documentation:<xsl:value-of select="xs:schema/xs:annotation/xs:documentation"/>
         </documentation>
         <contributors>
          <xsl:choose>
           <xsl:when test="$custom-type/c:contributors">
            <xsl:value-of select="$custom-type/c:contributors"/>
           </xsl:when>
           <xsl:otherwise>
            <xsl:text>None</xsl:text>
           </xsl:otherwise>
          </xsl:choose>
         </contributors>
        </annotation>
        <xsl:call-template name='crlf'/>

        <import type="def"/>
        <xsl:call-template name='crlf'/>

        <region name="element.start.name" parent="xml:element.defined.start.name"/>
        <region name="element.end.name"   parent="xml:element.defined.end.name"/>
        <region name="element.start.lt"   parent="xml:element.start.lt"/>
        <region name="element.start.gt"   parent="xml:element.start.gt"/>
        <region name="element.end.lt"     parent="xml:element.end.lt"/>
        <region name="element.end.gt"     parent="xml:element.end.gt"/>
        <region name="element.nsprefix"   parent="element.start.name"/>
        <region name="element.nscolon"    parent="xml:element.nscolon"/>

        <region name="Attribute.name"     parent="xml:Attribute.defined.name"/>
        <region name="Attribute.nsprefix" parent="xml:Attribute.nsprefix"/>

        <region name="AttValue"           parent="xml:AttValue.defined"/>
        <region name="AttValue.start"     parent="xml:AttValue.defined.start"/>
        <region name="AttValue.end"       parent="xml:AttValue.defined.end"/>

        <region name="Enumeration"        parent="xml:Enumeration" description="Enumerated type values"/>

        <xsl:for-each select='$custom-type/c:outline/c:element[@name]'>
          <region name="{@name}Outlined" parent="def:Outlined" description="{@description}"/>
        </xsl:for-each>
        <xsl:call-template name='crlf'/>
        <entity name="ns-real-prefix" value="{$ns-real-prefix}"/>
        <entity name="nsprefix" value="{$nsprefix}"/>
        <xsl:call-template name='crlf'/>

        <!-- These schemes was cloned from xml.hrc
             and allows to reassign new regions to they constructs
             Ideally, we should provide mechanism to virtualize region (like schemes)
        -->
        <scheme name="{$anonymous}elementContent">
          <block start="/~( (&lt;) (  ((%xml:NCName;) (:) )? (%xml:Name;) ) \M &gt;?   )/x"
                 end="/( (&lt;\/) (\y3\b)?= ( (%xml:NCName;) (:) )? (%xml:Name;) \b \M \s* (&gt;?)
                  | (\/ \M &gt;) )/x"
                 region01="PairStart" region02="element.start.lt"
                 region05="element.nsprefix" region06="element.nscolon" region07="element.start.name"
                 region11="PairEnd" region12="element.end.lt" region15="element.nsprefix" region16="element.nscolon"
                 region17="element.end.name" region18="element.end.gt" region19="element.start.gt"
                 scheme="xml:elementContent2"/>
          <inherit scheme="xml:badChar"/>
        </scheme>
        <scheme name="{$anonymous}AttValue">
          <block start="/(&quot;)/" end="/(\y1)/"
                 region00="PairStart" region10="PairEnd"
                 region01="AttValue.start" region11="AttValue.end"
                 scheme="xml:AttValue.content.quot" region="AttValue"/>
          <block start="/(&apos;)/" end="/(\y1)/"
                 region00="PairStart" region10="PairEnd"
                 region01="AttValue.start" region11="AttValue.end"
                 scheme="xml:AttValue.content.apos" region="AttValue"/>
        </scheme>
        <!-- this one is simple internal service scheme -->
        <scheme name="AttributeContent">
          <inherit scheme="xml:AttributeContent">
            <virtual scheme="xml:AttValue" subst-scheme="{$anonymous}AttValue"/>
          </inherit>
        </scheme>

        <xsl:copy-of select="$custom-type-schemes"/>

        <!-- Schema datatypes: -->

        <xsl:apply-templates mode="root"/>
        <xsl:apply-templates mode="typedefs"/>

        <xsl:call-template name='crlf'/><xsl:call-template name='crlf'/>

        <scheme name="{$hrctype}-root" access="public">
          <xsl:choose>
            <xsl:when test="$custom-type/c:top-level">
              <xsl:for-each select="$custom-type/c:top-level/*">
                <xsl:if test="self::c:element">
                  <inherit scheme="{.}-element"/>
                </xsl:if>
                <xsl:if test="self::c:group">
                  <inherit scheme="{.}-group"/>
                </xsl:if>
              </xsl:for-each>
            </xsl:when>
            <xsl:when test="$top-level-element">
              <inherit scheme="{$top-level-element}-element"/>
            </xsl:when>
            <xsl:otherwise>
              <annotation><documentation>
               You can replace these elements with needed single root element
               with customizing HRC generation process.
              </documentation></annotation>
              <xsl:apply-templates mode="root-elements"/>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:if test="$allow-unknown-root-elements = 'yes'">
            <inherit scheme="xml:element">
              <virtual scheme="xml:element" subst-scheme="{$hrctype}-root"/>
            </inherit>
          </xsl:if>
        </scheme>

        <scheme name="{$hrctype}" access="public">
          <xsl:choose>
            <xsl:when test="$force-single-root = 'yes'">
              <inherit scheme="xml:singleroot">
                <virtual scheme="xml:element" subst-scheme="{$hrctype}-root"/>
              </inherit>
            </xsl:when>
            <xsl:otherwise>
              <inherit scheme="xml:xml">
                <virtual scheme="xml:element" subst-scheme="{$hrctype}-root"/>
              </inherit>
            </xsl:otherwise>
          </xsl:choose>
        </scheme>

      </type>
    </hrc>
  </xsl:template>


</xsl:stylesheet>
<!-- ***** BEGIN LICENSE BLOCK *****
   - Version: MPL 1.1/GPL 2.0/LGPL 2.1
   -
   - The contents of this file are subject to the Mozilla Public License Version
   - 1.1 (the "License"); you may not use this file except in compliance with
   - the License. You may obtain a copy of the License at
   - http://www.mozilla.org/MPL/
   -
   - Software distributed under the License is distributed on an "AS IS" basis,
   - WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
   - for the specific language governing rights and limitations under the
   - License.
   -
   - The Original Code is the Colorer Library xsd2hrc module.
   -
   - The Initial Developer of the Original Code is
   - Cail Lomecb <cail@nm.ru>.
   - Portions created by the Initial Developer are Copyright (C) 1999-2003
   - the Initial Developer. All Rights Reserved.
   -
   - Contributor(s):
   -
   - Alternatively, the contents of this file may be used under the terms of
   - either the GNU General Public License Version 2 or later (the "GPL"), or
   - the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
   - in which case the provisions of the GPL or the LGPL are applicable instead
   - of those above. If you wish to allow use of your version of this file only
   - under the terms of either the GPL or the LGPL, and not to allow others to
   - use your version of this file under the terms of the MPL, indicate your
   - decision by deleting the provisions above and replace them with the notice
   - and other provisions required by the LGPL or the GPL. If you do not delete
   - the provisions above, a recipient may use your version of this file under
   - the terms of any one of the MPL, the GPL or the LGPL.
   -
   - ***** END LICENSE BLOCK ***** -->
