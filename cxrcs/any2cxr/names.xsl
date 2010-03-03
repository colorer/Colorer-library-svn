<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
	version="2.0"
	exclude-result-prefixes="xsl f n"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs='http://www.w3.org/2001/XMLSchema'
	xmlns:cxr="http://colorer.sf.net/2010/cxrcs"
	xmlns:f='colorer://xslt.ns.xml/cxr/func'
	xmlns:n='colorer://xslt.ns.xml/cxr/names'
>

<xsl:import href='func.xsl'/>


<!--
 !
 ! make names
 !
 !-->

<xsl:variable name='n:unused-names' as='xs:anyURI*'>
	<xsl:apply-templates select='/' mode='n:unused-names-ugly'/> 
	<!-- xsl:apply-imports/>  It's dont work.  :-E~~~~~~ -->
</xsl:variable>

<xsl:template match='/' mode='n:unused-names-ugly'>
	<xsl:value-of select="'http://www.w3.org/XML/1998/namespace'"/>
</xsl:template>




<xsl:template match='/*' mode='n:make-names'>
	<xsl:param name='tns-defined' tunnel='yes' select='"no"'/>
	
	<xsl:variable name='xnss' as='xs:anyURI*'>
		<xsl:sequence select='
			for $i in in-scope-prefixes(.) 
				return namespace-uri-for-prefix($i,$f:root-el)
		'/>
	</xsl:variable>
	
	<xsl:for-each-group select='$xnss' group-by='$xnss'>
		<xsl:variable name='nsuri' select='current-grouping-key()'/>
		
		<xsl:if test="not(
			(some $i in $n:unused-names satisfies $i = $nsuri)
			or ($nsuri = $f:tns and $tns-defined = 'yes')
		)">
			<!--cxr:namespace uri='{$nsuri}'>
				<cxr:prefix name='{f:ns2pre($nsuri)}'/>
				<cxr:prefix name='##any'/>
			</cxr:namespace-->
			<xsl:call-template name='n:make-name'>
				<xsl:with-param name='nsuri' select='$nsuri'/>
			</xsl:call-template>
		</xsl:if>
	</xsl:for-each-group>
</xsl:template>


<xsl:template match='@*|node()' mode='n:make-names'/>


<xsl:template name='n:make-name' match='@*|node()' mode='n:make-name'>
	<xsl:param name='nsuri' select='.'/>
	
	<cxr:namespace uri='{$nsuri}'>
		<cxr:prefix name='{f:ns2pre($nsuri)}'/>
		<cxr:prefix name='##any'/>
	</cxr:namespace>
</xsl:template>


<xsl:template match="@*[. = 'http://www.w3.org/XML/1998/namespace']" mode='n:make-name'>
	<cxr:namespace uri='{.}'>
		<cxr:prefix name='xml' use='required' attrib-use='required'/>
	</cxr:namespace>
</xsl:template>


<xsl:template name='n:make-def-name'>
	<cxr:namespace uri='##default' alias='{$f:def-ns-alias}'>
		<cxr:prefix name='##default' use='required'/>
	</cxr:namespace>
</xsl:template>



<!--
 !
 ! copy namespace
 !
 !-->

<xsl:template match='/*' mode='n:copy-namespace'>
	<xsl:variable name='ns-lsit'>
		<xsl:apply-templates select='.' mode='n:make-names'/>
	</xsl:variable>
	
	<xsl:apply-templates select='$ns-lsit/cxr:namespace' mode='n:copy-namespace-cnt'/>
</xsl:template>

<xsl:template match='cxr:namespace' mode='n:copy-namespace-cnt'>
	<xsl:namespace name="{cxr:prefix[@name != '##any']/@name}" select='@uri'/>
</xsl:template>

<xsl:template match="cxr:namespace[@uri = '##default']" mode='n:copy-namespace-cnt'>
	<xsl:namespace name='{$default-prefix}' select='$f:def-ns-alias'/>
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
