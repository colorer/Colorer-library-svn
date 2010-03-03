<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
	version="2.0"
	exclude-result-prefixes="xsl p"
	xpath-default-namespace='http://relaxng.org/ns/structure/1.0'
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:rng='http://relaxng.org/ns/structure/1.0'
	xmlns:p='colorer://xslt.ns.xml/cxr/rng-simple'
 >


<xsl:key name='p:def' match='rng:define' use='@name'/>


<xsl:template match='define[@combine]' mode='p:root'>
	<xsl:if test=". is key('p:def', @name)[1]">
		<xsl:copy>
			<xsl:attribute name='name' select='@name'/>
			<xsl:element name='{@combine}' namespace='http://relaxng.org/ns/structure/1.0'>
				<xsl:apply-templates select="key('p:def', @name)/*" mode='#current'/>
			</xsl:element>
		</xsl:copy>
	</xsl:if>
</xsl:template>



<xsl:template match="@*|node()" mode='p:root'>
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" mode='#current'/>
	</xsl:copy>
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
