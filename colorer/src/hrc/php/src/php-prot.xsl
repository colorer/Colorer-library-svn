<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
	xmlns="http://colorer.sf.net/2003/hrc"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 >

<xsl:output indent="yes" method="xml" encoding="Windows-1251"/>

<xsl:template match="php">
	<prototype name="php" group="inet" description="PHP: Hypertext preprocessor">
		<location link="inet/php.hrc"/>
		<filename>/\.(php\d?|phtml|inc)$/i</filename>
		<firstline>/^&lt;\?(php)/</firstline>
		<firstline>/^#!\S*(php)/</firstline>
		<parameters>
			<param name="posix-re" value="false" description='Use eregs highlightning'/>
			<param name="include-base-consts"	value="true" description='PHP Predefined Constants'/>
			<xsl:apply-templates select="packages"/>
		</parameters>
	</prototype>
</xsl:template>

<xsl:template match="package">
	<param name="Include-{@name}" description='{@number}. {@desc}'>
		<xsl:attribute name="value">
			<xsl:choose>
				<xsl:when test="@depr">false</xsl:when>
				<xsl:otherwise>true</xsl:otherwise>
			</xsl:choose>
		</xsl:attribute>
	</param>
</xsl:template>

</xsl:stylesheet>