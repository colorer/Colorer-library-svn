<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
	version="2.0"
	exclude-result-prefixes="xsl l p f dtd"
	xpath-default-namespace='http://colorer.sf.net/2010/dtdxml'
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs='http://www.w3.org/2001/XMLSchema'
	xmlns:dtd='http://colorer.sf.net/2010/dtdxml'
	xmlns:l='colorer://xslt.ns.xml/lib'
	xmlns:p='colorer://xslt.ns.xml/dtdxml/prep'
	xmlns:f='colorer://xslt.ns.xml/dtdxml/func'
 >
<xsl:import href='func.xsl'/>

<xsl:output method="xml" indent="yes" encoding="UTF-8"/>
<xsl:strip-space elements="*"/>

<!-- xsi not allowed in XML schema -->
<xsl:variable name='xsi-ns'>http://www.w3.org/2001/XMLSchema-instance</xsl:variable>



<!-- 
 !
 ! entity
 !
 !-->

<xsl:template match="entity[@role = 'element']">
	<xsl:apply-templates mode='element'/>
</xsl:template>

<xsl:template match="entity[@role = 'item']">
	<xs:group name='{@name}'>
		<xsl:apply-templates mode='item'/>
	</xs:group>
</xsl:template>

<xsl:template match="entity[@role = 'attlist']">
	<xs:attributeGroup name='{@name}'>
		<xsl:apply-templates mode='attlist'/>
	</xs:attributeGroup>
</xsl:template>

<xsl:template match="entity[@role = 'attrib']">
	<xs:simpleType name='{@name}'>
		<xsl:apply-templates mode='type'/>
	</xs:simpleType>
</xsl:template>

<xsl:template match='entity'>
	<xsl:message>Warning: unknow entity '<xsl:value-of select='@name'/>' detect.</xsl:message>
</xsl:template>
 



<!-- 
 !
 ! type
 !
 !-->

<xsl:template match='type[@type]' mode='type'>
	<xs:restriction base='{f:dtd2xsType(@type)}'>
		<xs:whiteSpace value='preserve'/>
	</xs:restriction>
</xsl:template>

<xsl:template match='choice' mode='type'>
	<xs:restriction base='{if(../@type) then f:dtd2xsType(../@type) else $f:token}'>
		<xsl:apply-templates mode='type'/>
	</xs:restriction>
</xsl:template>

<xsl:template match='item' mode='type'>
	<xs:enumeration value='{@name}'/>
</xsl:template>




<!-- 
 !
 ! item
 !
 !-->

<!-- mode item - level 1 -->
<xsl:template match="entity[@role = 'item']/*[@occurs]" mode='item' priority='2'>
	<xs:sequence>
		<xsl:next-match/>
	</xs:sequence>
</xsl:template>

<!-- mode item - level 2 -->
<xsl:template match='seq' mode='item'>
	<xs:sequence>
		<xsl:next-match/>
		<xsl:apply-templates mode='item'/>
	</xs:sequence>
</xsl:template>

<xsl:template match='choice' mode='item'>
	<xs:choice>
		<xsl:next-match/>
		<xsl:apply-templates mode='item'/>
	</xs:choice>
</xsl:template>

<xsl:template match='item' mode='item'>
	<xs:element ref='{f:itemName(@name, @pre)}'>
		<xsl:next-match/>
	</xs:element>
</xsl:template>

<xsl:template match='ref' mode='item'>
	<xs:group ref='{f:refName(@name)}'>
		<xsl:next-match/>
	</xs:group>
</xsl:template>

<!-- mode item - level 3 -->
<xsl:template match='*' mode='item'>
	<xsl:call-template name='occurs'/>
</xsl:template>


<!-- occurs -->

<xsl:template name='occurs'>
	<xsl:param name='occurs' select='@occurs'/>
	
	<xsl:choose>
		<xsl:when test="$occurs = 'nul-one'">
			<xsl:attribute name="minOccurs">0</xsl:attribute>
			<xsl:attribute name="maxOccurs">1</xsl:attribute>
		</xsl:when>
		<xsl:when test="$occurs = 'nul-inf'">
			<xsl:attribute name="minOccurs">0</xsl:attribute>
			<xsl:attribute name="maxOccurs">unbounded</xsl:attribute>
		</xsl:when>
		<xsl:when test="$occurs = 'one-inf'">
			<xsl:attribute name="minOccurs">1</xsl:attribute>
			<xsl:attribute name="maxOccurs">unbounded</xsl:attribute>
		</xsl:when>
	</xsl:choose>
</xsl:template>




<!-- 
 !
 ! attribs
 !
 !-->

<xsl:template match='attlist'/>


<!-- attlist mode -->

<xsl:template match='attlist' mode='attlist'>
	<xsl:apply-templates  mode='attlist'/>
</xsl:template>

<xsl:template match="attrib[(@name, @pre) = 'xmlns'] | attrib[@pre][f:pre2uri(@pre) = $xsi-ns]" mode='attlist' priority='2'/>

<xsl:template match="attrib[@pre][f:pre2uri(@pre) != $f:doc-ns]" mode='attlist'>
	<xs:attribute ref='{f:itemName(@name, @pre)}'/>
</xsl:template>

<xsl:template match='attrib' mode='attlist'>
	<xs:attribute name='{@name}'>
		<xsl:apply-templates select='.' mode='attrib'/>
	</xs:attribute>
</xsl:template>

<xsl:template match="ref[@role = 'attlist']" mode='attlist'>
	<xs:attributeGroup ref='{f:refName(@name)}'/>
</xsl:template>

<xsl:template match="ref[@role = 'attrib']" mode='attlist'>
	<xs:attribute ref='{f:refName(@name)}'/>
</xsl:template>


<!-- attrib mode -->

<!-- attrib mode - level 1 -->
<xsl:template match="attrib[@mode = 'fixed']" mode='attrib'>
	<xsl:attribute name='fixed' select='fixed'/>
</xsl:template>

<xsl:template match="attrib" mode='attrib'>
	<xsl:apply-templates select='*|@*' mode='attrib'/>
	
	<xsl:if test='choice'>
		<xs:simpleType>
			<xsl:apply-templates select='choice' mode='type'/>
		</xs:simpleType>
	</xsl:if>
</xsl:template>


<!-- attrib mode - level 2a -->
<xsl:template match="@mode" mode='attrib'>
	<xsl:variable name='mode'>
		<f:item key='required' val='required'/>
		<f:item key='implied'  val='optional'/>
	</xsl:variable>
	<xsl:attribute name='use' select="$mode/f:item[@key = current()]/@val"/>
</xsl:template>

<xsl:template match='@type' mode='attrib'>
	<xsl:attribute name='type' select='f:dtd2xsType(.)'/>
</xsl:template>


<!-- attrib mode - level 2b -->
<xsl:template match='ref' mode='attrib'>
	<xsl:attribute name='type' select='f:refName(@name)'/>
</xsl:template>

<xsl:template match='fixed' mode='attrib'>
	<xsl:attribute name='default' select='.'/>
</xsl:template>




<!-- 
 !
 ! element 
 !
 !-->

<xsl:template match="element">
	<xs:element name="{@name}">
		<xsl:apply-templates mode='element'/>
	</xs:element>
</xsl:template>



<!-- mode element - level 1 -->
<xsl:template match="element[not(key('f:attlists',@name))]/ref" mode='element' priority='4'>
	<xsl:attribute name='type' select='f:refName(@name)'/>
</xsl:template>

<xsl:template match="element[not(key('f:attlists',@name))]/content[@type = 'pcdata'][not(*)]" mode='element' priority='4'>
	<xsl:attribute name='type' select='$f:string'/>
</xsl:template>


<!-- mode element - level 2 -->
<xsl:template match='*' mode='element' priority='3'>
	<xs:complexType>
		<xsl:next-match/>
	</xs:complexType>
</xsl:template>

<xsl:template match='entity/*' mode='element' priority='2.5'>
	<xsl:attribute name='name' select='../@name'/>
	<xsl:next-match/>
</xsl:template>


<!-- mode element - level 3  -->
<xsl:template match='ref' mode="element" priority='2'>
	<xs:complexContent>
		<xs:extension base="{f:refName(@name)}">
			<xsl:next-match/>
		</xs:extension>
	</xs:complexContent>
</xsl:template>

<xsl:template match="content[@type = 'pcdata'][*]" mode='element'>
	<xsl:attribute name='mixed'>true</xsl:attribute>
	<xsl:next-match/>
</xsl:template>

<xsl:template match="content[@type = 'pcdata'][not(*)]" mode='element'>
	<xs:simpleContent>
		<xs:extension base="{$f:string}">
			<xsl:next-match/>
		</xs:extension>
	</xs:simpleContent>
</xsl:template>

<xsl:template match="content[@type = 'any']" mode='element'>
	<xsl:attribute name='mixed'>true</xsl:attribute>
	<xs:sequence>
		<xsl:call-template name='occurs'>
			<xsl:with-param name='occurs' select="'nul-inf'"/>
		</xsl:call-template>
		<xs:any processContents="lax"/>
	</xs:sequence>
	<xsl:next-match/>
</xsl:template>


<!-- mode element - level 5  -->
<xsl:template match='*' mode='element'>
	<xsl:apply-templates mode='item'/>
	<xsl:apply-templates select="key('f:attlists',../@name)" mode='attlist'/>
</xsl:template>




<!-- 
 !
 ! main
 !
 !-->

<xsl:template match='/doctype'>
	<xsl:variable name='ns-list' as='xs:string*'>
		<xsl:for-each-group select=".//*[@pre]" group-by='@pre'>
			<xsl:value-of select='current-grouping-key()'/>
		</xsl:for-each-group>
	</xsl:variable>
	
	<xs:schema>
		<xsl:if test='$f:has-doc-ns'> 
			<xsl:attribute name='targetNamespace' select="$f:doc-ns"/>
			<xsl:attribute name='elementFormDefault'>qualified</xsl:attribute>
		</xsl:if>
		
		<xsl:for-each select='$ns-list'>
			<xsl:variable name='ns-uri' select='f:pre2uri(.)'/>
			
			<xsl:if test="$ns-uri != '' and $ns-uri != $xsi-ns">
				<xsl:namespace name="{f:uri2pre($ns-uri)}" select="$ns-uri"/>
			</xsl:if>
		</xsl:for-each>
		
		<xsl:for-each select='$ns-list'>
			<xsl:variable name='ns-uri' select='f:pre2uri(.)'/>
			<xsl:if test="$ns-uri != $f:doc-ns and $ns-uri != $xsi-ns and $ns-uri != ''">
				<xs:import namespace='{$ns-uri}'/>
			</xsl:if>
		</xsl:for-each>
		
		<xsl:call-template name='annotation'/>
		<xsl:apply-templates/>
	</xs:schema>
</xsl:template>

<xsl:template match='/'>
	<xsl:apply-imports/>
</xsl:template>

<xsl:template name='annotation'>
	<xs:annotation><xs:documentation>
		<xsl:text>&#10;&#9;&#9;</xsl:text>
		<xsl:text>This XML schema autogenerate by DTD2XML</xsl:text>
		<xsl:text>&#10;&#9;&#9;</xsl:text>
		<xsl:text>Written © Eugene Efremov, 2009-2010</xsl:text>
		<xsl:text>&#10;&#9;&#9;</xsl:text>
		<xsl:text>&#10;&#9;&#9;</xsl:text>
		<xsl:text>Original DTD properties:</xsl:text>
		<xsl:if test='@public'>
			<xsl:text>&#10;&#9;&#9;&#9;</xsl:text>
			<xsl:text>PUBLIC: </xsl:text>
			<xsl:value-of select='@public'/>
		</xsl:if>
		<xsl:if test='@system'>
			<xsl:text>&#10;&#9;&#9;&#9;</xsl:text>
			<xsl:text>SYSTEM: </xsl:text>
			<xsl:value-of select='@system'/>
		</xsl:if>
		<xsl:if test='@root'>
			<xsl:text>&#10;&#9;&#9;&#9;</xsl:text>
			<xsl:text>root:   </xsl:text>
			<xsl:value-of select='@root'/>
		</xsl:if>
		<xsl:if test="$f:has-doc-ns">
			<xsl:text>&#10;&#9;&#9;&#9;</xsl:text>
			<xsl:text>xmlns:  </xsl:text>
			<xsl:value-of select='$f:doc-ns'/>
		</xsl:if>
		<xsl:text>&#10;&#9;&#9;</xsl:text>
		<xsl:text>&#10;&#9;&#9;</xsl:text>
	</xs:documentation></xs:annotation>
</xsl:template>

<!-- rare/unused -->

<xsl:template match='notation'>
	<xs:notation>
		<xsl:apply-templates select='@*' mode='notation'/>
	</xs:notation>
</xsl:template>

<xsl:template match='@name | @public | @system' mode='notation'>
	<xsl:copy/>
</xsl:template>

<xsl:template match='xml-ref'>
	<xs:annotation>
		<xs:documentation>Declaration of XML entity named '<xsl:value-of select='@name'/>' was here. XML Schema not support it.</xs:documentation>
	</xs:annotation>
</xsl:template>
 

<!-- def for all -->

<xsl:template match='@*|text()' mode='attrib notation'/>

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
   - The Original Code is DTD2XML.
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
