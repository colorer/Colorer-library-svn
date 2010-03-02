<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
	version="2.0"
	exclude-result-prefixes="xsl f"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs='http://www.w3.org/2001/XMLSchema'
	xmlns:f='colorer://xslt.ns.xml/cxr/func'
>

<xsl:param name='default-prefix' select="'def'"/>
<xsl:param name='tns-prefix'/>

<xsl:variable name='f:def-ns-alias'>colorer://alias.ns.xml/#default</xsl:variable>

<xsl:variable name='f:root' select='/'/>
<xsl:variable name='f:root-el' select='$f:root/*'/>
<xsl:variable name='f:tns-real' select='"redefine me!"'/>

<xsl:variable name='f:tns' select="
	if($f:tns-real != '') 
		then $f:tns-real 
		else if($tns-prefix) 
			then namespace-uri-for-prefix($tns-prefix,$f:root-el)
			else $f:def-ns-alias
"/>


<!-- namespace URI from prefix -->
<xsl:function name='f:ns2pre'>
	<xsl:param name='ns'/>
	<xsl:variable name='pre' 
		select='in-scope-prefixes($f:root-el)[namespace-uri-for-prefix(.,$f:root-el) = $ns][. != ""][1]'
	/>
	<xsl:value-of select="if($pre != '')
		then $pre 
		else if($ns = $f:tns and $tns-prefix) 
			then $tns-prefix
			else $default-prefix
	"/>
</xsl:function>


<!-- prefix form namespace URI -->
<xsl:function name='f:pre2ns'>
	<xsl:param name='pre'/>
	
	<xsl:variable name='ns' 
		select="namespace-uri-for-prefix($pre,$f:root-el)"
	/>
	<xsl:value-of select="if($ns != '')then $ns else $f:def-ns-alias"/>
</xsl:function>



<!-- NCName -> QName (default xmlns) -->
<xsl:function name='f:nn2qn'>
	<xsl:param name='nn'/>
	<xsl:variable name='pre' select='f:ns2pre($f:tns)'/>
	<!--xsl:message>nn2qn: <xsl:value-of select="concat($f:tns,' - ',$pre,':',$nn)"/></xsl:message-->
	
	<xsl:value-of select="QName($f:tns, concat($pre,':',$nn))"/><!--  TODO -->
</xsl:function>


<!-- make strict QName from attribute -->
<xsl:function name='f:qn2qn'>
	<xsl:param name='name' as="node()"/>
	
	<xsl:value-of select="f:qn2qn-strict(resolve-QName($name, $name/..))"/>
	<!--xsl:value-of select="
		if(contains($name, ':')) 
			then f:qn2qn-strict(resolve-QName($name, $name/..))
			else f:nn2qn($name)
		"/-->
</xsl:function>

<xsl:function name='f:qn2qn-strict'>
	<xsl:param name='qn' as="xs:QName"/>
	<xsl:variable name='pre' select='f:ns2pre(f:pre2ns(prefix-from-QName($qn)))'/>
	<xsl:variable name='uri' select='namespace-uri-from-QName($qn)'/>
	
	<xsl:value-of select="QName(if($uri) then $uri else $f:def-ns-alias, concat($pre, ':', local-name-from-QName($qn)))"/>
</xsl:function>


<!-- make NCName from attribute -->
<xsl:function name='f:qn2nn'>
	<xsl:param name="name" as="node()"/>
	<xsl:value-of select="f:qn2nn-strict(resolve-QName($name, $name/..))"/>
</xsl:function>

<xsl:function name='f:qn2nn-strict'>
	<xsl:param name='qn' as="xs:QName"/>
	<xsl:value-of select='local-name-from-QName($qn)'/>
</xsl:function>


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
