<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
     version="1.0"
     exclude-result-prefixes="c hrc xsl"
     xmlns="http://colorer.sf.net/2003/hrc"
     xmlns:c="uri:colorer:custom"
     xmlns:hrc="http://colorer.sf.net/2003/hrc"
     xmlns:xs="http://www.w3.org/2001/XMLSchema"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!-- 
 script&style support in custom.*.xml 
 XSL-based version of xmlss.hrc package
 
 Written by Eugene Efremov <4mirror@mail.ru>
--> 

<!--
 Mode 'scriptdef':
-->


<!-- custom/custom-type/script-n-style/element: -->

<xsl:template match="c:element" mode="scriptdef">
 <xsl:variable name="script-elem" select="@name"/>
 
 <xsl:variable name="script-type-named">
  <xsl:for-each select="$root">
   <xsl:choose>
    <xsl:when test="generate-id(key('script',$script-elem)/xs:complexType)">
     <xsl:value-of select="concat($anonymous, generate-id(key('script',$script-elem)/xs:complexType))"/>
    </xsl:when>
    <xsl:when test="key('script',$script-elem)/@type">
     <xsl:value-of select="key('script',$script-elem)/@type"/>
    </xsl:when>
    <!--<xsl:otherwise>
     <xsl:text>-shit-</xsl:text>
    </xsl:otherwise>-->
   </xsl:choose>
  </xsl:for-each>
 </xsl:variable>
 
 <!-- Note!! Namespace must be defined in custom.*.xml -->
 <xsl:variable name="script-type">
  <xsl:call-template name="qname2hrcname">
   <xsl:with-param name="qname" select="$script-type-named"/>
  </xsl:call-template>
 </xsl:variable>
 

 <xsl:call-template name='crlf'/>
  <xsl:comment>
   xmlss.hrc: support &apos;<xsl:value-of select="$script-elem"/>&apos; element
  </xsl:comment>
 <xsl:call-template name='crlf'/>
 
 <xsl:apply-templates select="c:language" mode="scriptdefscheme"/>
  
 <scheme name="xmlss-{$script-elem}">
  <xsl:apply-templates mode="scriptdef"><!--  select="./language"-->
   <xsl:with-param name="tag" select="$script-elem"/>
  </xsl:apply-templates>
 </scheme>
 
 <scheme name="{$script-elem}-element">
  <inherit scheme="xmlss-{$script-elem}">
   <virtual scheme="xml:Attribute.any" 
    subst-scheme="{$script-type}-Attributes"/>
  </inherit>    
 </scheme>

</xsl:template>


<!-- custom/custom-type/script-n-style/element/language - part I: -->

<xsl:template match="c:language" mode="scriptdefscheme">
 <scheme name="xmlss-{@name}-elementContent">
  <inherit scheme="{$anonymous}elementContent">
   <virtual scheme="xml:element" subst-scheme="def:empty"/>
   <virtual scheme="xml:content.cdata.stream" subst-scheme="{@name}:{@name}"/>
   <virtual scheme="xml:CDSect.content.stream" subst-scheme="{@name}:{@name}"/>
  </inherit>
 </scheme> 	
</xsl:template>


<!-- custom/custom-type/script-n-style/element/language - part II: -->

<xsl:template match="c:language" mode="scriptdef">
 <xsl:param name="tag" select="script"/>
 
 <block start="/\M (&lt;({$tag}\b [^&gt;]+ ({@expr}[\d\.]*) [^&gt;]*) &gt;) /ix"
  end="/&gt;/" scheme="xmlss-{@name}-elementContent"
 /> 
</xsl:template>


<!-- custom/custom-type/script-n-style/element/default: -->

<xsl:template match="c:default" mode="scriptdef">
 <xsl:param name="tag" select="script"/>
 
 <block start="/\M &lt;{$tag}\b [^&gt;]* (&gt;|$)/ix" 
         end="/&gt;/" scheme="xmlss-{@name}-elementContent"/> 
</xsl:template>


<!-- custom/custom-type/script-n-style/attribute: -->
<!--  
Note!! We need include to custom/custom-type/type:

   <scheme name="[sipleType/@name]-content">
    <inherit scheme="xmlss-[language]-attr"/>
   </scheme>

-->

<xsl:template match="c:attribute" mode="scriptdef">
 <xsl:call-template name='crlf'/>
  <xsl:comment>
   xmlss.hrc: support attributes for language &apos;<xsl:value-of select="@language"/>&apos; 
  </xsl:comment>
 <xsl:call-template name='crlf'/>

 <scheme name="xmlss-{@language}-attr">
  <block start="/&apos;?#1\M[^&apos;]/" end="/\M&apos;/" scheme="{@language}:Quot{@scheme-suffix}">
   <xsl:if test="@region">
    <xsl:attribute name="region">
     <xsl:value-of select="@region"/>
    </xsl:attribute>
   </xsl:if>
  </block>
  <block start="/&quot;?#1\M[^&quot;]/" end="/\M&quot;/" scheme="{@language}:Apos{@scheme-suffix}">
   <xsl:if test="@region">
    <xsl:attribute name="region">
     <xsl:value-of select="@region"/>
    </xsl:attribute>
   </xsl:if>
  </block>
 </scheme>
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
   - The Original Code is the xmlss.hrc package.
   -
   - The Initial Developer of the Original Code is
   - Eugene Efremov <4mirror@mail.ru>
   - Portions created by the Initial Developer are Copyright (C) 2003-2004
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
