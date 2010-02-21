<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
	version="2.0"
	exclude-result-prefixes="xsl l p"
	xpath-default-namespace='http://colorer.sf.net/2010/dtdxml'
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:dtd='http://colorer.sf.net/2010/dtdxml'
	xmlns:p='colorer://namespace.xml/xslt/dtdxml/prep'
	xmlns:l='colorer://namespace.xml/xslt/lib'
>
<xsl:import href='lib.xsl'/>

<xsl:output method="xml" indent="yes" encoding="UTF-8"/>
<xsl:strip-space elements="*"/>


<!-- for all -->

<xsl:key name='p:ent-all' match="entity | xml-ref" use='@name'/>

<!-- we cannot use #all: it's overloading -->
<xsl:template match='*|@*' priority='-0.6' 
	mode='p:names p:names-attr p:colls p:colls-change p:dup p:cond p:cond-inc p:cond-inc-copy p:include p:copy' 
 > <!-- default priority -0.5 -->
	
	<xsl:copy>
		<xsl:apply-templates select='@*' mode='#current'/>
		<xsl:apply-templates mode='#current'/>
	</xsl:copy>
</xsl:template>





<!-- 
 ! 
 ! Phase 5: subst names 
 !
 !-->

<xsl:template match="dtd:*[l:has-name(.,('element', 'attlist', 'item', 'attrib'))][@ref or @pre-ref]" mode='p:names'>
	<xsl:copy>
		<xsl:apply-templates select='@*' mode='p:names-attr'>
			<xsl:with-param	name='ref' select='.' tunnel='yes' as='item()'/>
		</xsl:apply-templates>
		<xsl:apply-templates mode='#current'/>
	</xsl:copy>
</xsl:template>


<xsl:template match="fixed/ref" mode='p:names'>
	<xsl:apply-templates select="key('p:ent-all', @name)/fixed/node()" mode='#current'/>
</xsl:template>



<!-- names-attr mode -->

<xsl:key name='p:back-ref' match="attrib|name" use='@ref'/>


<xsl:template match='@ref | @pre-ref' mode='p:names-attr'>
	<xsl:apply-templates select="key('p:ent-all', .)/name" mode='#current'/>
</xsl:template>

<!-- level 1 -->
<xsl:template match="entity[@role = 'prefix'][key('p:back-ref', @name)]/name" mode='p:names-attr'>
	<xsl:param name='ref' as='item()' tunnel='yes'/>
	
	<xsl:choose>
		<xsl:when test="some $x in key('p:back-ref', ../@name) satisfies $x is $ref">
			<xsl:attribute name='name' select='(@name|@pre)[1]'/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:next-match/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<!-- level 2 -->
<xsl:template match='name' mode='p:names-attr'>
	<xsl:apply-templates select='@*' mode='#current'>
		<xsl:with-param name='ref' select='.' tunnel='yes' as='item()'/>
	</xsl:apply-templates>
</xsl:template>

<xsl:template match="entity[@role = 'prefix']/name/@name" mode='p:names-attr'>
	<xsl:attribute name='pre' select='.'/>
</xsl:template>



<!-- for hard prefix... -->

<xsl:template match="attrib[@pre = 'xmlns'][not(@ref)]" mode='p:names'>
	<p:fix-xmlns-pre pre='{@name}'>
		<xsl:attribute name='ns'>
			<xsl:apply-templates select="fixed/node()" mode='#current'/>
		</xsl:attribute>
	</p:fix-xmlns-pre>
	<xsl:next-match/>
</xsl:template>



<!-- now not need // final clean -->

<xsl:template match="entity[@role = ('string', 'prefix', 'name', 'include', 'conduction')]" mode='p:names'/>





<!-- 
 ! 
 ! Phase 4: collision resolved 
 !
 !-->

<xsl:template match="ref[@role != key('p:ent-all', @name)/@role]" mode='p:colls'>
	<xsl:apply-templates select='.' mode='p:colls-change'/>
</xsl:template>


<xsl:template match="element/ref[@role = 'element']" mode='p:colls-change'>
	<dtd:content type="strict">
		<dtd:ref name="{@name}" role="item"/>
	</dtd:content>
</xsl:template>

<xsl:template match="seq/ref[@role = 'item'] | choice/ref[@role = 'item']" mode='p:colls-change'>
	<dtd:item ref="{@name}"/>
</xsl:template>

<xsl:template match="*|@*" mode='p:colls-change'>
	<xsl:message terminate='yes'>Fatal: call <xsl:value-of select='name()'/> in ivalid mode</xsl:message>
</xsl:template>





<!-- 
 ! 
 ! Phase 3: remove duplications
 !
 !-->

<xsl:template match='entity | xml-ref' mode='p:dup'>
	<xsl:if test=". is key('p:ent-all', @name)[1]">
		<xsl:next-match/>
	</xsl:if>
</xsl:template>





<!-- 
 ! 
 ! Phase 2: exclude conductios
 !
 !-->

<!-- names search (internal, used in phases 1,2 ) -->

<xsl:key name='p:ent-names' match="entity[name]" use='@name'/>


<xsl:function name='p:get-ent-name'>
	<xsl:param name='curr' as='node()'/>
	
	<xsl:apply-templates select='$curr' mode='p:ent-name'/>
</xsl:function>


<xsl:template mode='p:ent-name' match='*'>
	<xsl:apply-templates mode='p:ent-name' select='@*'/>
</xsl:template>

<xsl:template match='@*' mode='p:ent-name'/>

<xsl:template match='@name' mode='p:ent-name'>
	<xsl:value-of select='.'/>
</xsl:template>

<xsl:template match='@ref' mode='p:ent-name'>
	<xsl:variable name='ref' select="key('p:ent-names', .)[not(ancestor::conduction[p:get-ent-name(.) = 'ignore'])][1]"/>
	<xsl:if test="$ref/name">
		<xsl:value-of select="p:get-ent-name($ref/name)"/>
	</xsl:if>
</xsl:template>



<!-- phase 2 itself -->

<xsl:template match="conduction" mode='p:cond'>
	<xsl:if test="p:get-ent-name(.) = 'include'">
		<xsl:apply-templates mode='#current'/>
	</xsl:if>
</xsl:template>





<!-- 
 ! 
 ! Phase 1: move conduction includes
 !
 !-->
 
<xsl:key name='p:ent-inc' match="entity[@role = 'include']" use='@name'/>

<xsl:template match="include-ref" mode='p:cond-inc'>
	<xsl:variable name='ref' select="key('p:ent-inc', @name)[not(ancestor::conduction[p:get-ent-name(.) = 'ignore'])][1]"/>
	<xsl:apply-templates select='$ref/p:included-content' mode='p:cond-inc-copy'/>
</xsl:template>

<xsl:template match="document" mode='p:cond-inc'>
	<xsl:apply-templates mode='#current'/>
	<xsl:apply-templates select='p:included-content' mode='p:cond-inc-copy'/>
</xsl:template>

<xsl:template match="p:*" mode='p:cond-inc'/>

<xsl:template match="p:*" mode='p:cond-inc-copy'>
	<xsl:copy>
		<xsl:apply-templates select='@*|*' mode='p:cond-inc'/>
	</xsl:copy>
</xsl:template>





<!-- 
 ! 
 ! Phase 0: include contents
 !
 !-->
 
<xsl:template mode='p:include' match="entity[@role = 'include'] | document ">
	<xsl:copy>
		<xsl:apply-templates select='@*' mode='#current'/>
		<p:included-content>
			<xsl:apply-templates select='@*' mode='#current'/>
			<xsl:apply-templates select='document(@href)' mode='#current'/>
		</p:included-content>
		<xsl:apply-templates select='*' mode='#current'/>
	</xsl:copy>
</xsl:template>





<!-- 
 ! 
 ! main template
 !
 !-->
 
<xsl:template match='/' mode='p:main'>
	<xsl:call-template name='p:main'/>
</xsl:template>

<xsl:template name='p:main' match='/'>
	<xsl:variable name='p1'>
		<xsl:apply-templates mode='p:include'/>
	</xsl:variable>
	
	<xsl:variable name='p2'>
		<xsl:apply-templates mode='p:cond-inc' select='$p1'/>
	</xsl:variable>
	
	<xsl:variable name='p3'>
		<xsl:apply-templates mode='p:cond' select='$p2'/>
	</xsl:variable>
	
	<xsl:variable name='p4'>
		<xsl:apply-templates mode='p:dup' select='$p3'/>
	</xsl:variable>
	
	<xsl:variable name='p5'>
		<xsl:apply-templates mode='p:colls' select='$p4'/>
	</xsl:variable>
	
	<xsl:apply-templates mode='p:names' select='$p5'/>
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
