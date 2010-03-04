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
<xsl:import href='prep.xsl'/>

<!-- for unprefixed namespace in source -->
<xsl:param name='default-prefix' select="'def'"/> 


<xsl:variable name='f:root'>
	<xsl:apply-templates select='/' mode='p:main'/>
</xsl:variable>




<!-- namespace/qname stuff -->

<xsl:variable name='f:xml-ns'>
	<f:root>
		<xsl:copy-of select="$f:root//attrib[(@name, @pre) = 'xmlns']"/>
	</f:root>
	<dtd:attrib mode="fixed" name="xml" pre="xmlns" type='cdata'>
		<dtd:fixed>http://www.w3.org/XML/1998/namespace</dtd:fixed>
	</dtd:attrib>
</xsl:variable>


<xsl:key name='f:namespaces' match="attrib[@pre = 'xmlns'] | attrib[@name = 'xmlns']" use='@name'/>
<xsl:key name='f:namespace-uri' match="attrib[@pre = 'xmlns'] | attrib[@name = 'xmlns']" use='fixed'/>


<!-- namespace functions -->

<xsl:function name='f:pre2uri'>
	<xsl:param name='pre'/>
	
	<xsl:variable name='k-pre' select="if($pre) then $pre else 'xmlns'"/>
	<xsl:value-of select="key('f:namespaces', $k-pre, $f:xml-ns)/fixed"/>
</xsl:function>


<xsl:function name='f:uri2pre'>
	<xsl:param name='uri'/>
	
	<xsl:variable name='k-pre' select="(key('f:namespace-uri', $uri, $f:xml-ns)[@name != 'xmlns']/@name)[1]"/>
	
	<xsl:value-of select="
		if($uri) then 
			if($k-pre) then $k-pre
			else $default-prefix
		else ''
	"/>
</xsl:function>


<!-- QName functions -->

<xsl:function name='f:itemName'>
	<xsl:param name='name'/>
	<xsl:param name='pre' />
	
	<xsl:variable name='ns' select='f:pre2uri($pre)'/>
	<xsl:variable name='rpre' select='f:uri2pre($ns)' /> 
	
	<xsl:value-of select="
		if($ns != '') then QName($ns, concat($rpre, ':', $name))
		else QName('',$name)
	"/>
</xsl:function>


<xsl:function name='f:refName'>
	<xsl:param name='name'/>
	
	<xsl:value-of select="f:itemName($name, $f:doc-pre)"/>
</xsl:function>




<!-- global defines -->

<xsl:key name='f:elements' match='element' use='@name'/>
<xsl:key name='f:attlists' match='attlist' use='@name'/>

<xsl:variable name='f:doc-elem' select="key('f:elements', $f:root/doctype/@root, $f:root)"/>
<xsl:variable name='f:doc-pre' select="$f:doc-elem/@pre"/>
<xsl:variable name='f:doc-ns' select='f:pre2uri($f:doc-pre)'/>
<xsl:variable name='f:has-doc-ns' select="$f:doc-ns != ''" as='xs:boolean'/>


<!-- xsd types -->

<xsl:variable name='f:string' select="QName('http://www.w3.org/2001/XMLSchema', 'xs:string')"/>
<xsl:variable name='f:token' select="QName('http://www.w3.org/2001/XMLSchema', 'xs:NMTOKEN')"/>

<xsl:function name='f:dtd2xsType'>
	<xsl:param name='dType'/>
	
	<xsl:value-of select="
		if($dType = 'cdata') then $f:string
		else QName('http://www.w3.org/2001/XMLSchema', concat('xs:', upper-case($dType)))
	"/>
</xsl:function>



<!-- annotation -->
<xsl:template match='/doctype' mode='f:annotation'>
	<xsl:param name='product' select="'This XML schema autogenerate by DTD2XML'"/>
	<xsl:param name='author' select="'Written © Eugene Efremov, 2009-2010'"/>
	
	<xsl:text>&#10;&#9;&#9;</xsl:text>
	<xsl:value-of select='$product'/>
	<xsl:text>&#10;&#9;&#9;</xsl:text>
	<xsl:value-of select='$author'/>
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
</xsl:template>


<!-- main -->

<xsl:template match='/'>
	<xsl:apply-templates select='$f:root/doctype'/>
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
