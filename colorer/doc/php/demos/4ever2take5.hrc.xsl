<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE xsl:stylesheet [
<!ENTITY separate '<xsl:text>&#10;</xsl:text>'>
]>
<xsl:stylesheet
     version="1.0"
     xmlns="http://colorer.sf.net/2002/hrc"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output indent="yes"
              cdata-section-elements="regexp start end"
              doctype-public="-//Cail Lomecb//DTD colorer take5//EN"
              doctype-system="../hrc.dtd" encoding="windows-1251"/>


  <!-- Optional previous colorer.hrc path to easy find type name -->
  <xsl:variable name="root" select="document('file:/d:/programs/&amp;devel/colorer.4ever.patch2/hrc/colorer.hrc')"/>

  <xsl:template match="text()"/>
  <xsl:template match="comment()">
    &separate;
    <xsl:copy/>
  </xsl:template>
  <xsl:template match="@*">
    <xsl:attribute name="{name()}"><xsl:value-of select="."/></xsl:attribute>
  </xsl:template>


  <xsl:template match="/hrc">
    <hrc version="take5"
         xmlns="http://colorer.sf.net/2002/hrc"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://colorer.sf.net/2002/hrc ../hrc.xsd">
      <xsl:choose>
        <xsl:when test="count(./type) > 0">
          <xsl:apply-templates select="type|comment()"/>
        </xsl:when>
        <xsl:when test="count(./type) = 0">
          <type name="{/hrc/scheme[@name = $root/hrc/type/@name]/@name}" access="public">
            &separate;
            <import type="default"/>
            <xsl:apply-templates select="include"/>
              &separate;
            <xsl:apply-templates select="define"/>
              &separate;
            <xsl:apply-templates select="entity"/>
              &separate;
            <xsl:apply-templates select="scheme"/>
          </type>
        </xsl:when>
      </xsl:choose>
    </hrc>
  </xsl:template>



  <xsl:template match="type">
    <xsl:element name="prototype">
      <xsl:variable name="group" select="normalize-space(preceding-sibling::comment()[following-sibling::type[1]/@separator or not(preceding-sibling::type)][1])"/>
      <xsl:variable name="sgroup" select="substring-before($group, ' ')"/>
      <xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
      <xsl:attribute name="group">
        <xsl:if test="string-length($sgroup) = 0">
          <xsl:value-of select="$group"/>
        </xsl:if><xsl:value-of select="$sgroup"/>
      </xsl:attribute>
      <xsl:attribute name="description"><xsl:value-of select="@descr"/></xsl:attribute>

      <xsl:apply-templates select="load/@name"/>
      <xsl:apply-templates select="@exts"/>
      <xsl:apply-templates select="*"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="@exts">
    <filename><xsl:value-of select="."/></filename>
  </xsl:template>

  <xsl:template match="type/@*" priority="-1"/>

  <xsl:template match="load/@name">
    <location link="{.}"/>
  </xsl:template>

  <xsl:template match="switch">
    <switch type="{@type}">
      <xsl:value-of select="@match"/>
    </switch>
  </xsl:template>

<!--
  <xsl:template match="params|colors">
    <xsl:element name="ex:{name()}">
      <xsl:apply-templates select="@*"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="colors/@useht">
    <xsl:attribute name="useht">yes</xsl:attribute>
  </xsl:template>

  <xsl:template match="params/@fullback">
    <xsl:attribute name="fullback">yes</xsl:attribute>
  </xsl:template>
-->




  <xsl:template match="include">
    <import>
      <xsl:attribute name="type"><xsl:value-of select="substring-after(substring-before(@name,'.'),'/')"/></xsl:attribute>
    </import>
  </xsl:template>

  <xsl:template match="define">
    <define region="{@name}">
      <xsl:if test="string-length(@value) != 0 and string(number(@value)) = 'NaN'">
        <xsl:attribute name="value"><xsl:value-of select="@value"/></xsl:attribute>
      </xsl:if>
    </define>
  </xsl:template>

  <xsl:template match="entity">
    <entity name="{@name}" value="{@value}"/>
  </xsl:template>

  <xsl:template match="hrc/scheme">
    <scheme>
      <xsl:apply-templates select="@*|node()"/>
    </scheme>
    &separate;
  </xsl:template>

  <xsl:template match="scheme//*">
    <xsl:element name="{name()}">
      <xsl:apply-templates select="@*|node()"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="regexp/text() | start/text() | end/text()">
    <xsl:value-of select="."/>
  </xsl:template>

  <xsl:template match="virtual/@subst" priority="2">
    <xsl:attribute name="substScheme"><xsl:value-of select="."/></xsl:attribute>
  </xsl:template>

  <xsl:template match="@lowpriority" priority="2">
    <xsl:attribute name="priority">low</xsl:attribute>
  </xsl:template>

  <xsl:template match="regexp/@region0" priority="2">
    <xsl:attribute name="region"><xsl:value-of select="."/></xsl:attribute>
  </xsl:template>

  <xsl:template match="@worddiv" priority="2">
    <xsl:attribute name="worddiv"><xsl:value-of select="substring-after(substring(.,1,string-length(.)-1),'/')"/></xsl:attribute>
  </xsl:template>

  <xsl:template match="@ignorecase" priority="2">
    <xsl:attribute name="ignorecase">yes</xsl:attribute>
  </xsl:template>

</xsl:stylesheet>
