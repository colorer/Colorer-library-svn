<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
     xmlns:x="uri:custom:schema-db"
     xmlns:xs="http://www.w3.org/2001/XMLSchema"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!--
This stylesheet is used to transform merged hrc-ref docbook
version into the result HTML form.
It imports docbook.xsl stylesheet and introduce some changes
into it's logic
-->

<!--<xsl:import href="file:/d:\programs\xml\docbook/docbook-xsl/xhtml/docbook.xsl"/>-->
<xsl:import href="@docbook-dir@/xhtml/docbook.xsl"/>

<xsl:include href="html-titlepage.xsl"/>

<xsl:output method='html' encoding="KOI8-r"/>


<xsl:param name="html.stylesheet">../styles/styles.css ../styles/hrc-ref.css</xsl:param>
<xsl:param name="section.autolabel" select="1"/>
<xsl:param name="generate.component.toc" select="1"/>



<xsl:template name="newline"><xsl:text>&#xA;</xsl:text></xsl:template>


<xsl:template match="pubdate" mode="titlepage.mode">
  <h2>
    <xsl:value-of select="/*/@status"/>
    <xsl:text>&#160;</xsl:text>
    <xsl:apply-templates mode="titlepage.mode"/>
  </h2>
</xsl:template>

<xsl:template match="revhistory" mode="titlepage.mode">
  <dl>
    <dt>This version:</dt>
    <dd>
      <xsl:apply-templates select="revision[1]" mode="titlepage.mode"/>
    </dd>
  <xsl:if test="count(revision) &gt; 1">
    <dt>Previous versions:</dt>
    <dd>
     <xsl:apply-templates select="revision[position() &gt; 1]" mode="titlepage.mode"/>
    </dd>
  </xsl:if>
  </dl>
</xsl:template>

<xsl:template match="revision" mode="titlepage.mode">
  <xsl:apply-templates select="revnumber" mode="titlepage.mode"/>
  <xsl:text>: </xsl:text>
  <xsl:apply-templates select="date" mode="titlepage.mode"/>
  <xsl:if test="revremark">
    <br/>
  </xsl:if>
  <xsl:apply-templates select="revremark" mode="titlepage.mode"/>
  <xsl:if test="position()&lt;last()">
    <br/>
  </xsl:if>
</xsl:template>

<xsl:template match="acronym">
  <abbr title='{@role}'>
    <xsl:apply-templates select='text()'/>
  </abbr>
</xsl:template>

<xsl:template match="author" mode="titlepage.mode">
  <dl><dt>Author:</dt>
  <dd>
  <xsl:call-template name="person.name"/>
  <xsl:text> </xsl:text>
  <xsl:value-of select="authorblurb"/>
  <xsl:apply-templates select="affiliation/address/email" mode="titlepage.mode"/>
  </dd>
  </dl>
</xsl:template>

<xsl:template match="email" mode="titlepage.mode">
  <xsl:text>&#160;</xsl:text>
  <xsl:apply-templates select="."/>
  <xsl:if test="position()&lt;last()">,</xsl:if>
</xsl:template>

<xsl:template name="component.toc">
  <xsl:if test="$generate.component.toc != 0">
    <xsl:variable name="nodes" select="section|sect1"/>
    <xsl:variable name="apps" select="bibliography|glossary|appendix"/>

    <xsl:if test="$nodes">
      <div class="toc">
        <h2>
          <xsl:call-template name="gentext">
            <xsl:with-param name="key">TableofContents</xsl:with-param>
          </xsl:call-template>
        </h2>
        <xsl:if test="$nodes">
          <xsl:element name="{$toc.list.type}">
            <xsl:apply-templates select="$nodes" mode="toc"/>
          </xsl:element>
        </xsl:if>
        <xsl:if test="$apps">
          <h2>Appendixes</h2>
          <xsl:element name="{$toc.list.type}">
            <xsl:apply-templates select="$apps" mode="toc"/>
          </xsl:element>
        </xsl:if>
      </div>
      <hr/>
    </xsl:if>
  </xsl:if>
</xsl:template>




<xsl:template match="programlisting//literal">
  <span class='{@role}'><xsl:apply-templates/></span>
</xsl:template>

<xsl:template match="programlisting//literal/link">
  <a href='#{@linkend}'><xsl:apply-templates/></a>
</xsl:template>

<xsl:template match="programlisting">
  <pre class='source'>
    <xsl:apply-templates/>
  </pre>
</xsl:template>

<xsl:template match="para[@role='xsdocwrap']">
  <div class='xsdocwrap'>
    <xsl:apply-templates/>
  </div>
</xsl:template>


</xsl:stylesheet>
