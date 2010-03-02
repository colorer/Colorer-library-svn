<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
	version="2.0"
	exclude-result-prefixes="xsl s f"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs='http://www.w3.org/2001/XMLSchema'
	xmlns:cxr="http://colorer.sf.net/2010/cxrcs"
	xmlns:s='colorer://xslt.ns.xml/cxr/xsd'
	xmlns:f='colorer://xslt.ns.xml/cxr/func'
 >
<xsl:import href='func.xsl'/>

<xsl:output 
	method="xml" indent="yes" encoding="UTF-8"
	cdata-section-elements='cxr:documentation'
/>
<xsl:strip-space elements="*"/>


<xsl:param name='element-occurs' select="'no'"/>
<xsl:param name='copy-annotation' select="'no'"/>
<xsl:param name='support-xsi' select="'yes'"/>

<xsl:variable name='f:root-el' select='$f:root/xs:schema'/>
<xsl:variable name='f:tns-real' select='$f:root/xs:schema/@targetNamespace'/>




<!--
 !
 ! annotation
 !
 !-->


<xsl:template match="xs:annotation[$copy-annotation = 'yes']" mode='#default content attlist'>
	<xsl:apply-templates select='.' mode='annotation'/>
</xsl:template>

<xsl:template match="xs:*" mode='annotation'>
	<xsl:element name='{QName("http://colorer.sf.net/2010/cxrcs", concat("cxr:",local-name()))}'>
		<xsl:apply-templates select='@*|node()' mode='#current'/>
	</xsl:element>
</xsl:template> 

<xsl:template match="@*|node()" mode='annotation'>
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" mode='#current'/>
	</xsl:copy>
</xsl:template>

 

<!--
 !
 ! include
 !
 !-->

<xsl:function name='s:uri-xsd2cxr'>
	<xsl:param name='uri'/>
	
	<xsl:value-of select="replace($uri, '\.xsd$', '.cxr')"/>
</xsl:function>


<xsl:template match='xs:import'>
	<cxr:import type='{@namespace}'>
		<xsl:apply-templates select='@schemaLocation'/>
	</cxr:import>
</xsl:template>

<xsl:template match='xs:import/@schemaLocation'>
	<xsl:attribute name='href' select='s:uri-xsd2cxr(.)'/>
</xsl:template>



<xsl:template match='xs:include | xs:redefine' priority='2'>
	<!-- may be redefined for multi-doc support -->
	<cxr:include href='{s:uri-xsd2cxr(@href)}'>
		<xsl:next-match/>
	</cxr:include>
</xsl:template>

<xsl:template match='xs:redefine'>
	<cxr:redefine>
		<xsl:apply-templates/>
	</cxr:redefine>
</xsl:template>
 


 
<!-- 
 !
 ! element
 !
 !-->

<xsl:key  name='abst-elements' match="xs:element[@abstract = 'true']" use='f:nn2qn(@name)'/>
<xsl:key name='subst-elements' match="xs:element[@substitutionGroup]" use='f:qn2qn(@substitutionGroup)'/>
<xsl:variable name='abst-element-suffix'>-abstract-element</xsl:variable>



<!-- level 0 (optional) -->
<xsl:template match="xs:element[not(parent::xs:schema)][$element-occurs = 'yes']" priority='2'>
	<cxr:group>
		<xsl:call-template name='copy-occurs'/>
		<xsl:next-match/>
	</cxr:group>
</xsl:template>
 

<!-- level 1 -->
<xsl:template match="xs:element[@abstract = 'true'] | xs:element[@ref][key('abst-elements',f:qn2qn(@ref))]" priority='1.5'>
	<!-- TODO: xs:element[@ref = (abstract element)] for other docs  -->
	<cxr:any-element> 
		<xsl:call-template name='copy-attr'/>
		<xsl:apply-templates/>
	</cxr:any-element>
</xsl:template>

<xsl:template match="xs:element[not(@abstract = 'true')][key('subst-elements',if(@name)then f:nn2qn(@name) else f:qn2qn(@ref))]">
	<cxr:any-element>
		<xsl:call-template name='copy-attr'/>
		<xsl:apply-templates select='@name | @ref' mode='copy-subst-attr'/>
		<xsl:apply-templates/>
	</cxr:any-element>
	<xsl:if test='@name'>
		<cxr:element name='{f:nn2qn(@name)}'/>
	</xsl:if>
</xsl:template>

<!-- level 2 -->
<xsl:template match='xs:element'>
	<cxr:element>
		<xsl:call-template name='copy-attr'/>
		<xsl:apply-templates/>
	</cxr:element>
</xsl:template>



<!-- subst -->

<xsl:template match='@name' mode='copy-subst-attr'>
	<xsl:attribute name='name'>
		<xsl:value-of select='.'/>
		<xsl:next-match/>
	</xsl:attribute>
</xsl:template>

<xsl:template match='@ref' mode='copy-subst-attr'>
	<xsl:attribute name='ref'>
		<xsl:value-of select='f:qn2qn(.)'/>
		<xsl:next-match/>
	</xsl:attribute>
</xsl:template>

<xsl:template match='@*' mode='copy-subst-attr'>
	<xsl:value-of select='$abst-element-suffix'/>
</xsl:template>



<!-- otehr (group, any, etc) -->

<xsl:template match='xs:any'>
	<cxr:any/> <!-- TODO namespace -->
</xsl:template>


<xsl:template match="xs:group">
	<cxr:group>
		<xsl:call-template name='copy-attr'/>
		<xsl:call-template name='copy-occurs'/>
		<xsl:apply-templates/>
	</cxr:group>
</xsl:template>


<xsl:template match='xs:choice | xs:sequence | xs:all'>
	<cxr:group use-sequence='{local-name()}'>
		<xsl:call-template name='copy-occurs'/>
		<xsl:apply-templates/>
	</cxr:group>
</xsl:template>



<!-- 
 !
 ! attributes
 !
 !-->

<xsl:template match='xs:schema/xs:attribute'>
	<cxr:attlist global='true' name='{f:qn2nn(@name)}'>
		<!--xsl:call-template name='copy-attr'/-->
		<xsl:apply-templates mode='attlist' select='.'/>
	</cxr:attlist>
</xsl:template>


<xsl:template match='xs:attributeGroup' mode='#default attlist'>
	<cxr:attlist>
		<xsl:call-template name='copy-attr'/>
		<xsl:apply-templates mode='attlist'/>
	</cxr:attlist>
</xsl:template>


<xsl:template match='xs:attribute' mode='attlist'>
	<cxr:attrib>
		<xsl:call-template name='copy-attr'/>
		<xsl:apply-templates mode='#default'/>
		<xsl:apply-templates mode='attlist' select='@default'/>
	</cxr:attrib>
</xsl:template>

<xsl:template match='xs:anyAttribute' mode='attlist'>
	<cxr:any/>
</xsl:template>


<xsl:template match='xs:attribute[@ref]' mode='attlist'>
	<cxr:attlist>
		<xsl:call-template name='copy-attr'/>
	</cxr:attlist>
</xsl:template>


<xsl:template match='xs:attribute/@default' mode='attlist'>
	<cxr:default value='{.}'/>
</xsl:template>



<!-- 
 !
 ! complex type -> content
 !
 !-->


<!-- level 1 -->
<xsl:template match='xs:complexType' priority='2'>
	<cxr:content>
		<xsl:call-template name='copy-attr'/>
		<xsl:next-match/>
	</cxr:content>
</xsl:template>


<!-- level 2 -->
<xsl:template match='xs:complexType'>
	<xsl:apply-templates mode='content' select='.'/>
</xsl:template>

<xsl:template match='xs:complexType[xs:complexContent|xs:simpleContent]'>
	<xsl:apply-templates mode='content' select='xs:*/xs:*'/>
</xsl:template>


<!-- level 3 -->
<xsl:template match='xs:complexContent/xs:extension | xs:complexContent/xs:restriction' mode='content' priority='5'>
	<xsl:if test='not(ancestor::xs:redefine)'>
		<xsl:attribute name='inherit' select='f:qn2qn(@base)'/>
	</xsl:if>
	
	<xsl:next-match/>
</xsl:template>


<!-- level 4 -->
<xsl:template match='xs:complexContent/xs:restriction' mode='content' priority='4'>
	<xsl:attribute name='action'>replace</xsl:attribute>
	
	<xsl:next-match/>
</xsl:template>


<!-- level 5 -->
<xsl:template match='xs:complexType | xs:extension | xs:restriction' mode='content' priority='3'>
	
	<xsl:apply-templates select='xs:choice | xs:sequence | xs:all' mode='#default'/>
	
	<xsl:if test='xs:attribute | xs:attributeGroup | xs:anyAttribute'>
		<cxr:attlist>
			<xsl:apply-templates mode='attlist'/>
		</cxr:attlist>
	</xsl:if>
	
	<xsl:next-match/>
</xsl:template>


<!-- level 6 -->
<xsl:template match="xs:simpleContent/xs:restriction" mode='content' priority='2'>
	<cxr:data ref='{f:qn2qn(@base)}'/>
</xsl:template>

<xsl:template match="xs:complexType[@mixed = 'true'] | xs:complexType[@mixed = 'true']//xs:*" mode='content'>
	<cxr:data/>
</xsl:template>




<!-- 
 !
 ! other( data, notation)
 !
 !-->

<xsl:template match="xs:simpleType">
	<cxr:data>
		<xsl:call-template name='copy-attr'/>
		<xsl:apply-templates select='xs:annotation'/>
		<xsl:copy>
			<xsl:copy-of select='xs:*[not(self::xs:annotation)]'/>
		</xsl:copy>
	</cxr:data>
</xsl:template>

<xsl:template match='xs:notation'>
	<cxr:notation>
		<xsl:call-template name='copy-attr'/>
	</cxr:notation>
</xsl:template>




<!--
 !
 ! copy  attr
 !
 -->

<xsl:variable name='elem-unq' 
	select="not($f:root/xs:schema/@elementFormDefault) or $f:root/xs:schema/@elementFormDefault='unqualified'"
/>
 
<xsl:template name='copy-attr'>
	<xsl:apply-templates mode='copy-attr' select='@*'/>
</xsl:template>

<xsl:template match="xs:element[not(@abstract = 'true')][not(parent::xs:schema)]/@name[$elem-unq]" mode='copy-attr' priority='2'>
	<xsl:attribute name='name' select="concat($default-prefix,':',.)"/>
</xsl:template>

<xsl:template match="xs:element[not(@abstract = 'true')]/@name | xs:attribute/@name" mode='copy-attr'>
	<xsl:attribute name='name' select="f:nn2qn(.)"/>
</xsl:template>


<xsl:template match='@name' mode='copy-attr'>
	<xsl:attribute name='name' select="."/>
</xsl:template>

<xsl:template match='@ref' mode='copy-attr'>
	<xsl:attribute name='ref' select="f:qn2qn(.)"/>
</xsl:template>

<xsl:template match='@form' mode='copy-attr'>
	<xsl:variable name='cvt'>
		<s:c k='qualified' a='element' v='optional'/>
		<s:c k='unqualified' a='element' v='prohibited'/>
		<s:c k='qualified' a='attribute' v='required'/>
		<s:c k='unqualified' a='attribute' v='default'/>
	</xsl:variable>
	
	<xsl:attribute name='use-prefix' select="$cvt/s:c[@k = current()][@a = local-name(current()/..)]/@v"/>
</xsl:template>


<xsl:template match='xs:attribute/@fixed' mode='copy-attr'>
	<xsl:attribute name='use' select="'fixed'"/>
</xsl:template>

<xsl:template match='xs:attribute/@type' mode='copy-attr'>
	<xsl:attribute name='data' select="f:qn2qn(.)"/>
</xsl:template>

<xsl:template match='xs:element/@type' mode='copy-attr'>
	<xsl:attribute name='content' select="f:qn2qn(.)"/>
</xsl:template>


<xsl:template match='xs:notation/@public | xs:notation/@system | xs:attribute/@use' mode='copy-attr'>
	<xsl:attribute name='{name()}' select="."/>
</xsl:template>


<xsl:template match='@*' mode='copy-attr'/>



<!-- copy ossurs attr -->

<xsl:function name='s:copy-occurs'>
	<xsl:param name='occurs'/>
	
	<xsl:value-of select="
		if($occurs != '') then
			if($occurs = 'unbounded') then 'inf'
			else $occurs
		else 1
	"/>
</xsl:function>
 
<xsl:template name='copy-occurs'>
	<xsl:if test="not(parent::xs:schema) and $element-occurs = 'yes'">
		<xsl:attribute name='use-occurs' select='s:copy-occurs(@minOccurs), "", s:copy-occurs(@maxOccurs)'/>
	</xsl:if>
</xsl:template>




<!-- 
 !
 ! names
 !
 !-->
 
<xsl:template match="xs:schema[not(@targetNamespace) or not(@elementFormDefault) or @elementFormDefault='unqualified']" mode='make-names'>
	<cxr:namespace uri='##default' alias='{$f:def-ns-alias}'>
		<cxr:prefix name='##default' use='required'/>
	</cxr:namespace>
	<xsl:next-match/>
</xsl:template>


<xsl:template match='xs:schema' mode='make-names'>
	<xsl:variable name='current' select='.' as='element()'/>
	
	<xsl:apply-templates select='xs:import | @targetNamespace' mode='#current'/>
</xsl:template>


<xsl:template match='@targetNamespace' mode='make-names' priority='3'>
	<cxr:namespace uri='{.}'>
		<cxr:prefix name='{f:ns2pre(.)}'>
			<xsl:next-match/>
		</cxr:prefix>
		<cxr:prefix name='##any'>
			<xsl:next-match/>
		</cxr:prefix>
	</cxr:namespace>
</xsl:template>

<xsl:template match="xs:schema[not(@elementFormDefault) or @elementFormDefault='unqualified']/@targetNamespace" mode='make-names' priority='2'>
	<xsl:attribute name='use'>required</xsl:attribute>
	<xsl:next-match/>
</xsl:template>

<xsl:template match="xs:schema[@attributeFormDefault='qualified']/@targetNamespace" mode='make-names'>
	<xsl:attribute name='attrib-use'>required</xsl:attribute>
</xsl:template>

<xsl:template match='@*' mode='make-names'/>


<xsl:template match='xs:import' mode='make-names'>
	<xsl:variable name='pre' select='f:ns2pre(@namespace)'/>
	
	<cxr:namespace uri='{@namespace}'>
		<cxr:prefix name='{$pre}'/>
		<cxr:prefix name='##any'/>
	</cxr:namespace>
</xsl:template>

<xsl:template match="xs:import[@namespace = 'http://www.w3.org/XML/1998/namespace']" mode='make-names'>
	<cxr:namespace uri='{@namespace}'>
		<cxr:prefix name='xml' use='required' attrib-use='required'/>
	</cxr:namespace>
</xsl:template>



<xsl:template match='xs:schema' mode='copy-namespace'>
	<xsl:variable name='ns-lsit'>
		<xsl:apply-templates select='.' mode='make-names'/>
	</xsl:variable>
	
	<xsl:apply-templates select='$ns-lsit/cxr:namespace' mode='#current'/>
</xsl:template>

<xsl:template match='cxr:namespace' mode='copy-namespace'>
	<xsl:namespace name="{f:ns2pre(f:pre2ns(cxr:prefix[@name != '##any']/@name))}" select='@uri'/>
</xsl:template>

<xsl:template match="cxr:namespace[@uri = '##default']" mode='copy-namespace'>
	<xsl:namespace name='{$default-prefix}' select='$f:def-ns-alias'/>
</xsl:template>




<!--
 !
 ! main
 !
 -->


<xsl:template match='xs:schema'>
	<cxr:schema>
		<xsl:apply-templates select='.' mode='copy-namespace'/>
		
		<cxr:type name='{$f:tns}'>
			<xsl:if test="$support-xsi = 'yes'">
				<xsl:call-template name='support-xsi'/>
			</xsl:if>
			
			<xsl:apply-templates select='.' mode='make-names'/>
			<xsl:apply-templates/>
			
			<xsl:for-each-group select='xs:element' group-by='@substitutionGroup'>
				<xsl:variable name='qn' select='f:qn2qn-strict(resolve-QName(current-grouping-key(), $f:root/xs:schema))'/>
				
				<cxr:substitution name="{$qn}{if(not(key('abst-elements',$qn))) then $abst-element-suffix else ''}">
					
					<xsl:if test="not(key('abst-elements',$qn))">
						<cxr:element ref='{$qn}'/>
					</xsl:if>
					
					<xsl:apply-templates select='current-group()' mode='subst-group'/>
				</cxr:substitution>
			</xsl:for-each-group>
		</cxr:type>
		
	</cxr:schema>
</xsl:template>


<xsl:template name='support-xsi'>
	<cxr:apply-template name='xsi' href='xsi.cxr'/>
</xsl:template>


<!-- subst -->

<xsl:template match='xs:element' mode='subst-group'>
	<cxr:element ref='{f:nn2qn(@name)}'/> 
</xsl:template>

<xsl:template match="xs:element[@abstract = 'true']" mode='subst-group'>
	<cxr:any-element ref='{f:nn2qn(@name)}'/> 
</xsl:template>


<!-- all -->

<xsl:template match='text()|*' mode='content attlist'/>

<xsl:template match='text()' mode='#default content attlist'/>

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
