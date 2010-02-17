<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:l="urn:local"
	exclude-result-prefixes="l"
>

<xsl:param name='docbook-path'/>

<xsl:template match="*[@l:path]">
	<!-- todo (for future): check param name -->
	<xsl:copy>
		<xsl:attribute name='href'>
			<xsl:value-of select='concat($docbook-path,@href)'/>
		</xsl:attribute>
	</xsl:copy>
</xsl:template>

<xsl:template match="@*|node()">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()"/>
	</xsl:copy>
</xsl:template>

</xsl:stylesheet>
