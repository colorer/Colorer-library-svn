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
   <region name="packages" parent="WordExt"/>
   <scheme name="all-packages" if="{$pack-all}">
    <keywords region="packages">
     <xsl:apply-templates select="packages/separate/pack"/>
    </keywords>
    <xsl:apply-templates select="packages/groups/group" mode="base"/>
    <regexp match="/\b\w[\w\d]*\s*(::)\s*\w[\w\d]*\b/" region="packages" region1="Symb"/>
   </scheme>
   <xsl:apply-templates select="packages/groups/group"/>
</xsl:template>

<xsl:template match="pack">
 <word name="{@name}"/>
</xsl:template>

<xsl:template match="group">
 <scheme name="package-{@name}" if="{$pack-grp}-{@name}">
  <keywords region="packages">
   <xsl:apply-templates/>
  </keywords>
 </scheme>
</xsl:template>

<xsl:template match="group" mode="base">
 <inherit scheme="package-{@name}"/>
</xsl:template>

</xsl:stylesheet>