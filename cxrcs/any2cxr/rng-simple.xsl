<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
	version="2.0"
	exclude-result-prefixes="xsl p"
	xpath-default-namespace='http://relaxng.org/ns/structure/1.0'
	xmlns='http://relaxng.org/ns/structure/1.0'
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:rng='http://relaxng.org/ns/structure/1.0'
	xmlns:p='colorer://xslt.ns.xml/cxr/rng-simple'
 >


<xsl:output 
	method="xml" indent="yes" encoding="UTF-8"
/>

<xsl:key name='p:def' match='rng:define' use='@name'/>


<!--
 !
 ! phase 1 - include
 !
 !-->

<xsl:template match='include|externalRef' mode='p:include'> 
	<xsl:param name='defines' tunnel='yes' as='node()*'/>
	
	<xsl:apply-templates select='document(@href)' mode='p:include-content'>
		<xsl:with-param name='defines' tunnel='yes'>
			<xsl:apply-templates select='./*|$defines' mode='#current'/>
		</xsl:with-param>
	</xsl:apply-templates>
	
	<xsl:apply-templates mode='p:include-content'/>
</xsl:template>


<xsl:template match='div' mode='p:include p:include-content'> 
	<xsl:apply-templates mode='#current'/>
</xsl:template>



<xsl:template match='grammar' mode='p:include-content'>
	<xsl:apply-templates mode='#current'/>
</xsl:template>


<xsl:template match='define' mode='p:include-content'>
	<xsl:param name='defines' tunnel='yes' as='node()*'/>
	<xsl:if test="not($defines) or not(key('p:def', @name, $defines))">
		<xsl:next-match/>
	</xsl:if>
</xsl:template>


<xsl:template match='*' mode='p:include-content'>
	<xsl:apply-templates select='.' mode='p:include'/>
</xsl:template>




<!--
 !
 ! phase 2 - normalization
 !
 !-->


<xsl:template match='/grammar | /grammar/start' mode='p:norm' priority='2'>
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" mode='#current'/>
		<xsl:apply-templates select='.' mode='p:norm-define'/>
	</xsl:copy>
</xsl:template>


<xsl:template match='/rng:*' mode='p:norm'>
	<grammar>
		<xsl:apply-templates select='.' mode='p:include-xmlns'/>
		<xsl:apply-templates select='@*' mode='#current'/>
		<start>
			<xsl:apply-templates mode='#current'/>
		</start>
		<xsl:apply-templates select='.' mode='p:norm-define'/>
	</grammar>
</xsl:template>


<xsl:template match='grammar | start' mode='p:norm'>
	<xsl:apply-templates mode='#current'/>
</xsl:template> 
 

<xsl:template match='define' mode='p:norm'/>



<xsl:template match='/rng:*' mode='p:norm-define'>
	<xsl:apply-templates select='//define' mode='p:norm-define'/>
</xsl:template>


<xsl:template match='define' mode='p:norm-define'>
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" mode='p:norm'/>
	</xsl:copy>
</xsl:template>

<xsl:template match='*' mode='p:norm-define'/>




<!--
 !
 ! final phase - define
 !
 !-->

<xsl:template match="define[count(key('p:def', @name)) &gt; 1]" mode='p:define'>
	<xsl:if test=". is key('p:def', @name)[1]">
		<xsl:variable name='combine' select="(key('p:def', @name)/@combine)[1]"/>
		
		<xsl:copy>
			<xsl:attribute name='name' select='@name'/>
			<!--xsl:message>new element (<xsl:value-of select='@name'/>): (<xsl:value-of select='$combine'/>)</xsl:message-->
			
			<xsl:choose>
				<xsl:when test='$combine'>
					<xsl:element name='{$combine}' namespace='http://relaxng.org/ns/structure/1.0'>
						<xsl:apply-templates select="key('p:def', @name)/*" mode='#current'/>
					</xsl:element>
				</xsl:when>
				
				<xsl:otherwise>
					<xsl:message>Warning: define '<xsl:value-of select='@name'/>' has no combine.</xsl:message>
					<xsl:apply-templates mode='#current'/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:copy>
	</xsl:if>
</xsl:template>




<!--
 !
 ! root
 !
 !-->


<xsl:template match='/rng:*' mode='p:root'>
	<xsl:copy>
		<!-- phase 0 -->
		<xsl:apply-templates select='.' mode='p:include-xmlns'/>
		<!-- phase 1 -->
		<xsl:apply-templates select="@*|node()" mode='p:include'/>
	</xsl:copy>
</xsl:template>


<!-- include xmlns -->

<xsl:template match='/rng:*' mode='p:include-xmlns'>
	<xsl:variable name='root' select='.'/>
	
	<xsl:for-each select='in-scope-prefixes(.)'>
		<xsl:namespace name='{.}' select='namespace-uri-for-prefix(., $root)'/>
	</xsl:for-each>
	
	<xsl:apply-templates mode='#current'/>
</xsl:template>


<xsl:template match='include|externalRef' mode='p:include-xmlns'> 
	<xsl:apply-templates select='document(@href)' mode='#current'/>
</xsl:template>

<xsl:template match='text()' mode='p:include-xmlns'/> 



<!-- root -->

<xsl:template match="@*|node()" mode='p:include p:define p:norm'>
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" mode='#current'/>
	</xsl:copy>
</xsl:template>


<xsl:template match='/' mode='p:root'>
	<!-- phase 1 -->
	<xsl:variable name='p1'>
		<xsl:apply-templates mode='#current'/>
	</xsl:variable>
	
	<!-- phase 2 -->
	<xsl:variable name='p2'>
		<xsl:apply-templates mode='p:norm' select='$p1'/>
	</xsl:variable>
	
	<!-- final phase -->
	<xsl:apply-templates mode='p:define' select='$p2'/>
</xsl:template>


<xsl:template match='/'>
	<xsl:apply-templates mode='p:root' select='/'/>
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
