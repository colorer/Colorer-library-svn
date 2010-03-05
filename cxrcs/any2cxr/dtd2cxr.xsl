<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
	version="2.0"
	exclude-result-prefixes="xsl d f n r dtd"
	xpath-default-namespace='http://colorer.sf.net/2010/dtdxml'
	xmlns:cxr="http://colorer.sf.net/2010/cxrcs"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs='http://www.w3.org/2001/XMLSchema'
	xmlns:dtd='http://colorer.sf.net/2010/dtdxml'
	xmlns:f='colorer://xslt.ns.xml/cxr/func'
	xmlns:n='colorer://xslt.ns.xml/cxr/names'
	xmlns:d='colorer://xslt.ns.xml/dtdxml/func'
	xmlns:r='colorer://xslt.ns.xml/cxr/dtd'
 >

<xsl:import href='template.xsl'/>
<xsl:import href='../../dtd2xml/dtd2xsd/func.xsl'/>

<xsl:param name='tns-uri'/>

<xsl:variable name='f:root-el' select='$f:root/doctype'/>
<xsl:variable name='f:tns-real' select='if($tns-uri) then $tns-uri else $d:doc-ns'/>

<xsl:variable name='def-ns-tpl' select="'colorer://alias.ns.xml/'"/>
<xsl:variable name='unk-ns-tpl' select="'colorer://unknow.ns.xml/'"/>


<xsl:variable name='xns'>
	<r:names>
		<xsl:apply-templates select='$f:root/*' mode='n:copy-namespace'/>
	</r:names>
</xsl:variable>


<xsl:function name='r:dtd2qn'>
	<xsl:param name='pre'/>
	<xsl:param name='name'/>
	<xsl:variable name='rpre' select='if($pre)then $pre else $default-prefix'/>
	
	<xsl:value-of select="resolve-QName(concat($rpre, ':', $name), $xns/r:names)"/>
</xsl:function>


<xsl:function name='r:pre2uri'>
	<xsl:param name='pre'/>
	
	<xsl:value-of select='namespace-uri-for-prefix($pre, $xns/r:names)'/>
</xsl:function>





<!-- 
 !
 ! entity
 !
 !-->

<xsl:template match="entity[@role = 'element']">
	<xsl:apply-templates mode='element'/>
</xsl:template>

<xsl:template match="entity[@role = 'attlist']">
	<cxr:attlist name='{@name}'>
		<xsl:apply-templates mode='attlist'/>
	</cxr:attlist>
</xsl:template>

<xsl:template match="entity[@role = 'attrib']">
	<cxr:data name='{@name}'>
		<xsl:apply-templates mode='type'/>
	</cxr:data>
</xsl:template>

<xsl:template match="entity[@role = 'item']">
	<cxr:group name='{@name}'>
		<xsl:apply-templates mode='item'/>
	</cxr:group>
</xsl:template>

<xsl:template match='entity'>
	<xsl:message>Warning: unknow entity '<xsl:value-of select='@name'/>' detect.</xsl:message>
</xsl:template>




<!-- 
 !
 ! type
 !
 !-->

<xsl:template match='type' mode='type'>
	<xsl:apply-templates select='*|@*' mode='#current'/>
</xsl:template>

<xsl:template match='choice' mode='type'>
	<cxr:simple><cxr:list>
		<xsl:apply-templates mode='#current'/>
	</cxr:list></cxr:simple>
</xsl:template>

<xsl:template match='item' mode='type'>
	<cxr:item value='{@name}'/>
</xsl:template>

<xsl:template match='@type' mode='type'>
	<cxr:inherit ref='{d:dtd2xsType(.)}'/>
</xsl:template>




<!-- 
 !
 ! item
 !
 !-->

<xsl:template match='choice|seq' mode='item'>
	<xsl:variable name='mode'>
		<r:item key='choice' val='choice'/>
		<r:item key='seq'  val='sequence'/>
	</xsl:variable>
	
	<cxr:group use-sequence="{$mode/r:item[@key = local-name(current())]/@val}">
		<xsl:apply-templates mode='#current'/>
	</cxr:group>
</xsl:template>


<xsl:template match='ref' mode='item'>
	<cxr:group ref='{f:nn2qn(@name)}'/>
</xsl:template>

<xsl:template match='item' mode='item'>
	<cxr:element ref='{r:dtd2qn(@pre, @name)}'/>
</xsl:template>




<!-- 
 !
 ! attlist
 !
 !-->

<xsl:template match='attlist'/>

<!-- attlist -->

<xsl:template match='attlist' mode='attlist'>
	<cxr:attlist>
		<xsl:apply-templates  mode='attlist'/>
	</cxr:attlist>
</xsl:template>

<xsl:template match="attrib[(@name, @pre) = 'xmlns']" mode='attlist' priority='2'/>

<!--xsl:template match="attrib[@pre][]" mode='attlist'>
</xsl:template-->

<xsl:template match="attrib" mode='attlist'>
	<cxr:attrib name='{if(@pre) then r:dtd2qn(@pre, @name) else f:nn2qn(@name)}'>
		<xsl:apply-templates select='.' mode='attrib'/>
	</cxr:attrib>
</xsl:template>

<xsl:template match="ref[@role = 'attlist']" mode='attlist'>
	<cxr:attlist ref='{f:nn2qn(@name)}'/>
</xsl:template>

<xsl:template match="ref[@role = 'attrib']" mode='attlist'>
	<xsl:message>attrib ref: <xsl:value-of select="@name, ../@name"/></xsl:message>
	<!-- ????? -->
	<!--cxr:attrib ref='{f:nn2qn(@name)}'/-->
</xsl:template>


<!-- attrib -->

<xsl:template match='attrib' mode='attrib'>
	<xsl:apply-templates select='@*|*' mode='#current'/>
</xsl:template>


<xsl:template match='choice' mode='attrib'>
	<cxr:data>
		<xsl:if test='../@type'>
			<xsl:attribute name='inherit' select='d:dtd2xsType(../@type)'/>
		</xsl:if>
		
		<xsl:apply-templates select='.' mode='type'/>
	</cxr:data>
</xsl:template>


<xsl:template match='fixed' mode='attrib'>
	<cxr:default value='{.}'/>
</xsl:template>


<xsl:template match='ref' mode='attrib'>
	<xsl:attribute name='data' select='f:nn2qn(@name)'/>
</xsl:template>

<xsl:template match='attrib[not(choice)]/@type' mode='attrib'>
	<xsl:attribute name='data' select='d:dtd2xsType(.)'/>
</xsl:template>


<xsl:template match='@mode' mode='attrib'>
	<xsl:variable name='mode'>
		<r:item key='required' val='required'/>
		<r:item key='implied'  val='optional'/>
		<r:item key='fixed'    val='fixed'/>
	</xsl:variable>
	<xsl:attribute name='use' select="$mode/r:item[@key = current()]/@val"/>
</xsl:template>

<xsl:template match='node()|@*' mode='attrib'/>




<!-- 
 !
 ! element
 !
 !-->

<xsl:template match='element'>
	<cxr:element name='{r:dtd2qn(@pre, @name)}'>
		<xsl:apply-templates mode='element'/>
	</cxr:element>
</xsl:template>


<xsl:template match="element[not(key('d:attlists',@name))]/ref" mode='element' priority='4'>
	<xsl:attribute name='content' select='f:nn2qn(@name)'/>
</xsl:template>

<xsl:template match='element/* | entity/content' mode='element' priority='3'>
	<cxr:content>
		<xsl:next-match/>
	</cxr:content>
</xsl:template>


<xsl:template match='entity/content' mode='element' priority='2'>
	<xsl:attribute name='name' select='../@name'/>
</xsl:template>

<xsl:template match="element/ref" mode='element' priority='2'>
	<xsl:attribute name='inherit' select='f:nn2qn(@name)'/>
</xsl:template>


<xsl:template match="content[*]" mode='element'>
	<cxr:group>
		<xsl:apply-templates mode='item'/>
	</cxr:group>
</xsl:template>


<xsl:template match="content" mode='element'>
	<xsl:apply-templates select='@*' mode='#current'/>
	<xsl:next-match/>
</xsl:template>


<xsl:template match='*' mode='element'>
	<xsl:apply-templates select="key('d:attlists',../@name)" mode='attlist'/>
</xsl:template>



<xsl:template match="@type[.='pcdata']" mode='element'>
	<cxr:data/>
</xsl:template>

<xsl:template match="@type[.='any']" mode='element'>
	<cxr:group><cxr:any/></cxr:group>
	<cxr:data/>
</xsl:template>

<xsl:template match="@*" mode='element'/>





<!-- 
 !
 ! names
 !
 !-->

<xsl:template match='/doctype' mode='n:make-names'>
	<xsl:variable name='xmlns' select='$d:xml-ns/d:root/attrib'/>
	<!--cxr:annotation><cxr:documentation>
		<xsl:copy-of select='$d:xml-ns'/>
		<d:other>
			<xsl:for-each-group select=".//*[@pre]" group-by='@pre'>
				<xsl:variable name='pre' select='current-grouping-key()'/>
				<d:pre name='{$pre}' uri='{d:pre2uri($pre)}'/>
			</xsl:for-each-group>
		</d:other>
	</cxr:documentation></cxr:annotation-->
	
	<xsl:apply-templates select='$xmlns' mode='#current'/>
	<xsl:if test='not($xmlns[fixed])'>
		<xsl:call-template name='n:make-def-name'/>
	</xsl:if>
	
	<xsl:for-each-group select=".//*[@pre]" group-by='@pre'>
		<xsl:variable name='pre' select='current-grouping-key()'/>
		<xsl:if test="(not(some $i in $xmlns/@name satisfies $i = $pre) and $pre != 'xmlns')">
			<cxr:namespace uri='##default' alias='{$def-ns-tpl}{$pre}'>
				<cxr:prefix name='{$pre}' use='required'/>
			</cxr:namespace>
		</xsl:if>
	</xsl:for-each-group>
	
</xsl:template>



<xsl:template match='attrib[fixed]' mode='n:make-names'>
	<xsl:call-template name='n:make-name'>
		<xsl:with-param name='nsuri' select='fixed'/>
		<xsl:with-param name='nspre' 
			select="if(@name != 'xmlns') 
				then @name
				else f:ns2pre(fixed)
			"
		/>
	</xsl:call-template>
</xsl:template>


<xsl:template match='attrib' mode='n:make-names'>
	<xsl:variable name='pre' select="if(@name != 'xmlns') then @name else ''"/>
	<xsl:variable name='uri' select="if($pre and $pre = $tns-prefix) then $f:tns else concat($unk-ns-tpl, $pre)"/>
	<cxr:namespace uri='{$uri}'>
		<cxr:prefix name="{if($pre) then $pre else f:ns2pre($uri) (: ?? :) }" use='required'/>
	</cxr:namespace>
</xsl:template>




<!-- 
 !
 ! main
 !
 !-->

<xsl:template match='notation'>
	<cxr:notation>
		<xsl:copy-of select='@*'/>
	</cxr:notation>
</xsl:template>

<xsl:template match='xml-ref'>
	<cxr:entity>
		<xsl:copy-of select='@*|node()'/>
	</cxr:entity>
</xsl:template>


<xsl:template match='/doctype/@root'>
	<cxr:root>
		<cxr:element ref='{r:dtd2qn($d:doc-pre,.)}'/>
	</cxr:root>
</xsl:template>


<!-- main -->

<xsl:template match='/doctype' mode='root'>
	<xsl:apply-imports/>
	<xsl:copy-of select='@public'/>
</xsl:template>


<xsl:template match='/doctype'>
	<xsl:apply-templates mode='d:annotation' select='.'/>
	<xsl:apply-templates/>
	<xsl:apply-templates select='@root'/>
</xsl:template>



<xsl:template match='/doctype' mode='d:annotation'>
	<cxr:annotation><cxr:documentation>
		<xsl:apply-imports>
			<xsl:with-param name='product' select="'This CXRCS file autogenerate by DTD2CXR, part or CXRCS suite'"/>
		</xsl:apply-imports>
	</cxr:documentation></cxr:annotation>
</xsl:template>


<!-- root -->

<xsl:variable name='f:root' select='$d:root'/>

<xsl:template match='/'>
	<xsl:apply-templates mode='root' select='$f:root'/>
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
   - The Original Code is CXRCS suite.
   -
   - The Initial Developer of the Original Code is
   - Eugene Efremov <4mirror@mail.ru>
   - Portions created by the Initial Developer are Copyright (C) 2010
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
