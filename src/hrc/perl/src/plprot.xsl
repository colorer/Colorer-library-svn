<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
     version="1.0"
     exclude-result-prefixes="xsl"
     xmlns="http://colorer.sf.net/2003/hrc"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="yes" encoding="windows-1251"/>
<xsl:strip-space elements="*"/>

<xsl:include href="plparam.xsl"/>

<xsl:template match="/">
    <parameters>
     <param name="{$pack-all}" value="true" description="Use perl packages (if unset, other parameters ignored)"/>
     <xsl:apply-templates select="packages/groups/group"/>
    </parameters>
</xsl:template>

<xsl:template match="group">
 <param name="{$pack-grp}-{@name}" value="true" description="Use packages {@name}::*"/>
</xsl:template>

</xsl:stylesheet>