<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
	version="2.0"
	exclude-result-prefixes="xsl r p n f a rng"
	xpath-default-namespace='http://relaxng.org/ns/structure/1.0'
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:rng='http://relaxng.org/ns/structure/1.0'
	xmlns:a="http://relaxng.org/ns/compatibility/annotations/1.0"
	xmlns:xs='http://www.w3.org/2001/XMLSchema'
	xmlns:cxr="http://colorer.sf.net/2010/cxrcs"
	xmlns:r='colorer://xslt.ns.xml/cxr/rng'
	xmlns:f='colorer://xslt.ns.xml/cxr/func'
	xmlns:n='colorer://xslt.ns.xml/cxr/names'
	xmlns:p='colorer://xslt.ns.xml/cxr/rng-simple'
 >

<xsl:import href='template.xsl'/>
<xsl:import href='rng-simple.xsl'/>

<xsl:param name='use-rng-ns' select="'no'"/>
<xsl:param name='use-arng-ns' select="'no'"/>


<xsl:variable name='start-group-name' select="'cxrcs-rng-start-group'"/>

<xsl:variable name='f:root-el' select='$f:root/rng:*'/>
<xsl:variable name='f:tns-real' select='$f:root-el/@ns'/>




<xsl:function name='r:qn2xsd'>
	<xsl:param name='name' as="attribute()"/>
	
	<xsl:value-of select="
		if(contains($name, ':'))
			then f:qn2qn($name) 
			else QName('http://www.w3.org/2001/XMLSchema', concat('xs:', $name))
	"/>
</xsl:function>



<!--
 !
 ! check patterns
 !
 !-->

<xsl:function name='r:isPattern' as='xs:boolean'>
	<xsl:param name='item' as='node()'/>
	<xsl:variable name='names' as='xs:string*'>
		<xsl:sequence select="('group','interleave','choice','optional','zeroOrMore','oneOrMore','list','mixed')"/>
	</xsl:variable>
	
	<xsl:value-of select="some $i in $names satisfies $i = local-name($item)"/>
</xsl:function>


<xsl:function name='r:chkPattern' as='xs:string*'>
	<xsl:param name='item' as='node()*'/>
	
	<!--xsl:if test='r:isPattern($item)'-->
		<xsl:apply-templates select='$item' mode='chkp'/>
	<!--/xsl:if-->
</xsl:function>

<xsl:function name='r:chkData' as='xs:string*'>
	<xsl:param name='item' as='node()*'/>
	<xsl:apply-templates select='$item' mode='chkd'/>
</xsl:function>




<xsl:template match='attribute|element' mode='chkp'>
	<xsl:sequence select='local-name(.)'/>
</xsl:template>
<xsl:template match='text|value|data' mode='chkp'><!-- mixed| -->
	<xsl:sequence select="'data'"/>
</xsl:template>
<xsl:template match='mixed' mode='chkp'>
	<xsl:sequence select="'mixed'"/>
	<xsl:apply-templates mode='#current'/>
</xsl:template>

<xsl:template match='oneOrMore|zeroOrMore|value|param' mode='chkd'>
	<xsl:sequence select="'simple'"/>
</xsl:template>



<xsl:template match='ref' mode='chkp'>
	<xsl:param name='ref-name' tunnel='yes' as='xs:string*'/>
	
	<xsl:if test="some $i in $ref-name satisfies $i = @name">
		<xsl:message>Recursion detect: @ref to '<xsl:value-of select='@name'/>' after sequence '<xsl:value-of select='$ref-name'/>'</xsl:message>
	</xsl:if>
	
	<xsl:if test="every $i in $ref-name satisfies $i != @name">
		<xsl:apply-templates select='key("p:def", @name)' mode='#current'>
			<xsl:with-param name='ref-name' select='($ref-name, @name)' tunnel='yes' as='xs:string*'/>
		</xsl:apply-templates>
	</xsl:if>
</xsl:template>




<xsl:function name='r:itsa' as='xs:boolean'>
	<xsl:param name='item' as='node()'/>
	<xsl:param name='type' as='xs:string'/>
	
	<xsl:value-of select='some $i in r:chkPattern(if(local-name($item) ="ref")then $item else $item/*) satisfies $i = $type'/>
</xsl:function>


<xsl:function name='r:isAttrib' as='xs:boolean'>
	<xsl:param name='item' as='node()'/>
	<xsl:value-of select='r:itsa($item, "attribute")'/>
</xsl:function>

<xsl:function name='r:isElement' as='xs:boolean'>
	<xsl:param name='item' as='node()'/>
	<xsl:value-of select='r:itsa($item, "element")'/>
</xsl:function>

<xsl:function name='r:isData' as='xs:boolean'>
	<xsl:param name='item' as='node()'/>
	<xsl:value-of select='r:itsa($item, "data")'/>
</xsl:function>

<xsl:function name='r:isMixed' as='xs:boolean'>
	<xsl:param name='item' as='node()'/>
	<xsl:value-of select='r:itsa($item, "mixed")'/>
</xsl:function>


<xsl:function name='r:itsdt' as='xs:boolean'>
	<xsl:param name='item' as='node()'/>
	<xsl:param name='type' as='xs:string'/>
	
	<xsl:value-of select='some $i in r:chkData($item) satisfies $i = $type'/>
</xsl:function>

<xsl:function name='r:isSimple' as='xs:boolean'>
	<xsl:param name='item' as='node()'/>
	<xsl:value-of select='r:itsdt($item, "simple")'/>
</xsl:function>





<!--
 !
 ! element
 !
 !-->

<!-- todo: mixed choice/any... -->
<xsl:template match='element[name or choice/name]' mode='element'>
	<xsl:apply-templates mode='element-name'>
		<xsl:with-param name='content-ref' tunnel='yes' select='concat("content-",generate-id(.))'/>
	</xsl:apply-templates>
</xsl:template>

<xsl:template match='element[@name]' mode='element'>
	<cxr:element name='{f:qn2qn(@name)}'>
		<xsl:next-match/>
	</cxr:element>
</xsl:template>

<xsl:template match='element[anyName]' mode='element'>
	<cxr:any-element>
		<xsl:next-match/>
	</cxr:any-element>
</xsl:template>

<xsl:template match='element[nsName or choice/nsName]' mode='element'>
	<cxr:any-element namespace='{nsName/@ns | choice/nsName/@ns}'>
		<xsl:next-match/>
	</cxr:any-element>
</xsl:template>


<xsl:template match='element' mode='element'>
	<cxr:content>
		<xsl:call-template name='element-content'/>
	</cxr:content>
</xsl:template>

<xsl:template name='element-content'>
		<xsl:call-template name='make-all'/>
		<xsl:if test="not(r:isData(.)) and r:isMixed(.)">
			<cxr:data/>
		</xsl:if>
</xsl:template>



<xsl:template match='rng:*[r:isPattern(.)][r:isElement(.)]' mode='element'>
	<cxr:group>
		<!-- todo: use-* -->
		<xsl:apply-templates mode='#current'/>
	</cxr:group>
</xsl:template>

<xsl:template match='ref[r:isElement(.)]' mode='element'>
	<cxr:group ref='{f:nn2qn(@name)}'/>
</xsl:template>



<xsl:template match='name'  mode='element-name'>
	<xsl:param name='content-ref' tunnel='yes' select='"foo"'/>
	<cxr:element name='{f:qn2qn(.)}' content='{f:nn2qn($content-ref)}'/>
</xsl:template>

<xsl:template match='choice' mode='element-name'>
	<xsl:apply-templates mode='#current'/>
</xsl:template>



<!--
 !
 ! attribs
 !
 !-->

<xsl:template match='attribute[@name]' mode='attlist'>
	<cxr:attrib name='{f:qn2qn(@name)}'>
		<xsl:apply-templates select='..' mode='att-use'/>
		<xsl:apply-templates mode='att-data'/>
		<xsl:if test='@a:defaultValue'>
			<cxr:default value='{@a:defaultValue}'/>
		</xsl:if>
	</cxr:attrib>
</xsl:template>

<xsl:template match='attribute[anyName]' mode='attlist'>
	<cxr:any/>
</xsl:template>


<!--xsl:template match='*[r:isPattern(.)][r:isAttrib(.)]' mode='attlist'>
	<cxr:attlist>
		<xsl:apply-templates mode='#current'/>
	</cxr:attlist>
</xsl:template-->

<xsl:template match='ref[r:isAttrib(.)]' mode='attlist'>
	<cxr:attlist ref='{f:nn2qn(@name)}'/>
</xsl:template>



<!-- att-data -->

<xsl:template match='data[not(*) and @type]' mode='att-data'>
	<xsl:attribute name='data' select='r:qn2xsd(@type)'/>
</xsl:template>
 
<xsl:template match='rng:*' mode='att-data'>
	<xsl:apply-templates select='..' mode='mk-data'/>
</xsl:template>


<!-- att-use -->
<xsl:template match='optional' mode='att-use'>
	<xsl:attribute name='use' select='"optional"'/>
</xsl:template>
<xsl:template match='rng:*' mode='att-use'>
	<xsl:attribute name='use' select='"required"'/>
</xsl:template>




<!--
 !
 ! data
 !
 !-->

<xsl:template match='data[not(*) and @type]' mode='data'>
	<cxr:inherit ref='{r:qn2xsd(@type)}'/>
</xsl:template>

<xsl:template match='ref[r:isData(.)]' mode='data'>
	<!--xsl:attribute name='ref' select='f:qn2qn(@name)'/-->
	<cxr:inherit ref='{f:nn2qn(@name)}'/>
</xsl:template>

<xsl:template match='ref[not(key("p:def", @name))]' mode='data'>
	<!--xsl:attribute name='ref' select='f:qn2qn(@name)'/-->
	<cxr:inherit ref='{r:qn2xsd(@name)}'/>
</xsl:template>


<xsl:template match='rng:*[r:isSimple(.)]' mode='data' priority='2'>
	<cxr:simple>
		<xsl:apply-templates mode='data-ref'/>
		<xsl:apply-templates select='.' mode='data-content'/>
	</cxr:simple>
</xsl:template>


<xsl:template match='rng:*[r:isPattern(.)][r:isData(.)]' mode='data'>
	<xsl:apply-templates mode='#current'/>
</xsl:template>




<xsl:template match="param[@name = 'pattern']" mode='data-content'>
	<cxr:pattern match='{.}'/>
</xsl:template>

<xsl:template match='value[1]' mode='data-content'>
	<cxr:list>
		<xsl:for-each select='../value'>
			<cxr:item value='{.}'/>
		</xsl:for-each>
	</cxr:list>
</xsl:template>


<xsl:template match='oneOrMore' mode='data-ref'>
	<xsl:attribute name='reply'>1 inf</xsl:attribute>
	<xsl:apply-templates mode='data-ref-in'/>
</xsl:template>

<xsl:template match='zeroOrMore' mode='data-ref'>
	<xsl:attribute name='reply'>0 inf</xsl:attribute>
	<xsl:apply-templates mode='data-ref-in'/>
</xsl:template>

<xsl:template match='data[@type]' mode='data-ref-in'>
	<xsl:attribute name='ref' select='r:qn2xsd(@type)'/>
</xsl:template>

<xsl:template match='ref[r:isData(.)]' mode='data-ref-in'>
	<xsl:attribute name='ref' select='f:nn2qn(@name)'/><!-- todo: multi -->
</xsl:template>




<!--
 !
 ! make content
 !
 !-->


<!-- level 1 -->
<xsl:template match='rng:*[r:isAttrib(.)]' mode='mk-attrib' priority='3'>
	<cxr:attlist>
		<xsl:next-match>
			<xsl:with-param name='nm' select='"yes"'/>
		</xsl:next-match>
	</cxr:attlist>
</xsl:template>

<xsl:template match='rng:*[r:isElement(.)]' mode='mk-element' priority='3'>
	<cxr:group>
		<xsl:next-match>
			<xsl:with-param name='nm' select='"yes"'/>
		</xsl:next-match>
	</cxr:group>
</xsl:template>

<xsl:template match='rng:*[r:isData(.)]' mode='mk-data' priority='3'>
	<cxr:data>
		<xsl:next-match>
			<xsl:with-param name='nm' select='"yes"'/>
		</xsl:next-match>
	</cxr:data>
</xsl:template>


<!-- level 2 -->
<xsl:template match='define' mode='mk-attrib mk-element mk-data'  priority='2'>
	<xsl:param name='nm' select='"no"'/>
	<xsl:if test='$nm = "yes"'>
		<xsl:attribute name='name' select='@name'/>
		<xsl:next-match/>
	</xsl:if>
</xsl:template>


<!-- level 3 -->
<xsl:template match='rng:*[r:isAttrib(.)]' mode='mk-attrib'>
	<xsl:apply-templates mode='attlist'/>
</xsl:template>

<xsl:template match='rng:*[r:isElement(.)]' mode='mk-element'>
	<xsl:apply-templates mode='element'/>
</xsl:template>

<xsl:template match='rng:*[r:isData(.)]' mode='mk-data'>
	<xsl:apply-templates mode='data'/>
</xsl:template>

<!-- stop level -->


<!-- make all -->
<xsl:template name='make-all'>
	<xsl:apply-templates select='.' mode='mk-attrib'/>
	<xsl:apply-templates select='.' mode='mk-element'/>
	<xsl:apply-templates select='.' mode='mk-data'/>
</xsl:template>







<!-- 
 !
 ! names
 !
 !-->


<xsl:template match='/' mode='n:unused-names-ugly'>
	<xsl:if test="$use-rng-ns = 'no'">
		<xsl:value-of select="'http://relaxng.org/ns/structure/1.0'"/>
	</xsl:if>
	<xsl:if test="$use-arng-ns = 'no'">
		<xsl:value-of select="'http://relaxng.org/ns/compatibility/annotations/1.0'"/>
	</xsl:if>
	<xsl:apply-imports/> 
</xsl:template>


 
<xsl:template match="/rng:*[not(@ns) and ($f:tns = $f:def-ns-alias)]" mode='n:make-names' priority='2'>
	<xsl:call-template name='n:make-def-name'/>
	<xsl:next-match/>
</xsl:template>


<xsl:template match="/rng:*[@ns]" mode='n:make-names'  priority='2'>
	<xsl:apply-templates select='@ns' mode='n:make-name'/>
	<xsl:next-match>
		<xsl:with-param name='tns-defined' select='"yes"'/>
	</xsl:next-match>
</xsl:template>





<!--
 !
 ! main
 !
 -->

<xsl:template match='element[name or choice/name]' mode='content'>
	<xsl:variable name='name' select='concat("content-",generate-id(.))'/>
	
	<cxr:content name='{$name}'>
		<xsl:call-template name='element-content'/>
	</cxr:content>
</xsl:template>
 

<xsl:template match='define'>
	<xsl:call-template name='make-all'/>
</xsl:template>

<xsl:template match='start'>
	<cxr:group name='{$start-group-name}'>
		<xsl:apply-templates mode='element'/>
	</cxr:group>
</xsl:template>

<xsl:template match='start' mode='root'>
	<cxr:root>
		<cxr:group ref='{f:nn2qn($start-group-name)}'/>
	</cxr:root>
</xsl:template>



<!-- main -->

<xsl:template match='/rng:*'>
	<xsl:apply-templates/>
	<xsl:apply-templates select='//element[name or choice/name]' mode='content'/>
	<xsl:apply-templates select='start' mode='root'/>
</xsl:template>



<!-- root -->

<xsl:variable name='f:root'>
	<xsl:apply-templates select='/' mode='p:root'/>
</xsl:variable>

<xsl:template match='/'>
	<xsl:apply-templates mode='root' select='$f:root'/>
</xsl:template>



<!-- all -->

<xsl:template match='text()' 
	mode='#default chkp chkd element element-name attlsit att-use att-data data data-content data-ref data-ref-in mk-attrib mk-element mk-data content'
/>

<xsl:template match='*' 
	mode='mk-attrib mk-element mk-data data element-name'
/>


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
   - The Original Code is CXRCS suite.
   -
   - The Initial Developer of the Original Code is
   - Eugene Efremov <4mirror@mail.ru>
   - Portions created by the Initial Developer are Copyright (C) 2009-2010
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
