<?xml version="1.0" ?>
<xsl:transform 
	version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
 >

<xsl:output method="text"/>     
<xsl:strip-space elements="*"/>

<xsl:param name="path" select="'Console'"/>
<!--xsl:param name="path" select="'Software\ConEmu'"/-->

<xsl:template match='hcd'>
	<xsl:text>REGEDIT4</xsl:text>
	<xsl:call-template name="cr-lf"/>
	<xsl:call-template name="cr-lf"/>
	
	<xsl:text>[HKEY_CURRENT_USER\</xsl:text>
		<xsl:value-of select='$path'/>
	<xsl:text>]</xsl:text>
	<xsl:call-template name="cr-lf"/>
	
	<xsl:apply-templates/>
</xsl:template>


<xsl:template match='color'>
	<xsl:variable name='val' select="substring-after(@con, '#')"/>
	<xsl:variable name='prep' select="translate($val, 'ABCDEF', '012345')"/>
	
	<xsl:variable name='name'>
		<xsl:choose>
			<xsl:when test='$prep = $val'>0</xsl:when>
			<xsl:otherwise>1</xsl:otherwise>
		</xsl:choose>
		<xsl:value-of select='$prep'/>
	</xsl:variable>
	
	<xsl:variable name='val' select="substring-after(@rgb, '#')"/>
	
	<xsl:text>"ColorTable</xsl:text>
	<xsl:value-of select='$name'/>
	<xsl:text>"=dword:00</xsl:text>
	<xsl:value-of select='$val'/>
	<xsl:call-template name="cr-lf"/>
</xsl:template>


<xsl:template name="cr-lf">
<xsl:text>
</xsl:text>
</xsl:template>

</xsl:transform>
