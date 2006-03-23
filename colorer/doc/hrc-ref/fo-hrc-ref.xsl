<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
     xmlns:fo="http://www.w3.org/1999/XSL/Format"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!--
 This stylesheet is used to transform merged hrc-ref
 docbook version into the result XSL-FO form.
 It imports docbook.xsl stylesheet and introduce some changes into it's logic.
-->

<!--
<xsl:import href="file:/d:\programs\xml\docbook/docbook-xsl/fo/docbook.xsl"/>
-->
<xsl:import href="@docbook-dir@/fo/docbook.xsl"/>


<xsl:include href="fo-titlepage.xsl"/>

<xsl:output method='xml' encoding="UTF-8"/>

<xsl:param name="section.autolabel" select="1"/>
<xsl:param name="generate.component.toc" select="1"/>
<xsl:param name="draft.watermark.image" select="''"/>
<xsl:param name="body.font.master">12</xsl:param>
<xsl:param name="ulink.show" select="0"/>
<xsl:param name="fop.extensions" select="1"/>
<xsl:param name="shade.verbatim" select="1"/>
<xsl:param name="paper.type" select="A4"/>

<xsl:template match="author" mode="titlepage.mode">
  <fo:block keep-with-next="always" font-weight="bold">Author:</fo:block>
  <xsl:call-template name="person.name"/>
  <xsl:text> </xsl:text>
  <xsl:value-of select="authorblurb"/>
  <xsl:apply-templates select="affiliation/address/email" mode="titlepage.mode"/>
</xsl:template>

<xsl:template match="email" mode="titlepage.mode">
  <xsl:text>&#160;</xsl:text>
  <xsl:apply-templates select="."/>
  <xsl:if test="position()&lt;last()">,</xsl:if>
</xsl:template>

<xsl:template match="revhistory" mode="titlepage.mode">
  <fo:block keep-with-next="always" font-weight="bold">This version:</fo:block>
  <xsl:apply-templates select="revision[1]" mode="titlepage.mode"/>

  <xsl:if test="count(revision) &gt; 1">
   <fo:block margin-top="0.4em"  keep-with-next="always" font-weight="bold">Previous versions:</fo:block>
   <xsl:apply-templates select="revision[position() &gt; 1]" mode="titlepage.mode"/>
  </xsl:if>
</xsl:template>

<xsl:template match="revision" mode="titlepage.mode">
  <fo:block margin-left="1.2em">
    <xsl:value-of select="revnumber"/>: <xsl:value-of select="date"/>
  </fo:block>
  <xsl:if test="revremark">
    <fo:block margin-left="1.2em">
      <xsl:apply-templates select="revremark"/>
    </fo:block>
  </xsl:if>
</xsl:template>


<xsl:template match="para[@role='xsdocwrap']">
  <fo:block background-color="#ebebeb" border='1pt solid #d8d8d8'
            space-before="1em" space-after="1em"
            space-start="1em" space-end="1em"
            margin-left="1.2em" margin-right="1.2em"
            padding-left="0.2em" padding-top="0.2em" padding-right="1.0em"
            >
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="para[@role='xsdocdecl']">
  <fo:block font-weight='bold' font-size='115%'>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>


<xsl:template match="para[@role='xsdochead']">
  <fo:block font-weight='bold'
            margin-left="2.7pc"
  >
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="para[@role='xsdoc']">
  <fo:block margin-left="3.5pc"
            space-before="0.1em" space-after="0.3em"
  >
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>


<xsl:template match="programlisting">
  <fo:block font-family="monospace"
            font-size="10pt"
            border-color="#d4d4d4"
            border-style="solid"
            border-width="1pt"
            hyphenate="false"
            background-color="#efefef"
            text-align="start"
            white-space-collapse="false"
            linefeed-treatment="preserve"
            padding-top="0.4em" padding-bottom="0.4em"
            margin-top="0.4em"
            wrap-option="wrap">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<!--
<xsl:attribute-set name="monospace.verbatim.properties" use-attribute-sets="verbatim.properties">
  <xsl:attribute name="font-family">
    <xsl:value-of select="$monospace.font.family"/>
  </xsl:attribute>
  <xsl:attribute name="font-size">10pt</xsl:attribute>
  <xsl:attribute name="border-color">#d4d4d4</xsl:attribute>
  <xsl:attribute name="border-style">solid</xsl:attribute>
  <xsl:attribute name="border-width">1pt</xsl:attribute>
  <xsl:attribute name="hyphenate">false</xsl:attribute>
  <xsl:attribute name="background-color">#efefef</xsl:attribute>
  <xsl:attribute name="text-align">start</xsl:attribute>
  <xsl:attribute name="white-space-collapse">false</xsl:attribute>
  <xsl:attribute name="linefeed-treatment">preserve</xsl:attribute>
  <xsl:attribute name="wrap-option">wrap</xsl:attribute>
</xsl:attribute-set>
-->

<xsl:template match="programlisting//literal">
  <xsl:variable name='color'>
   <xsl:choose>
     <xsl:when test='@role="xs_elem"'>#662222</xsl:when>
     <xsl:when test='@role="xs_attr"'>#226622</xsl:when>
     <xsl:when test='@role="xs_attrval"'>#222299</xsl:when>
     <xsl:otherwise>black</xsl:otherwise>
   </xsl:choose>
  </xsl:variable>
  <fo:inline color="{$color}">
    <xsl:if test="link">
      <xsl:attribute name="background-color">#d8d8d8</xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>


<xsl:template match="literal//link">
  <fo:basic-link internal-destination="{@linkend}">
    <xsl:value-of select="text()"/>
  </fo:basic-link>
</xsl:template>

</xsl:stylesheet>
