<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xs="http://www.w3.org/2001/XMLSchema"
                xmlns:x="uri:custom:schema-db"
                >
<!--
This stylesheet is used to transform original hrc-ref,
resolve all the x:schemaref objects and replace them with
schema source or schema documentation
-->

<xsl:include href="db-xsdprint.xsl"/>
<xsl:include href="db-xsdoc.xsl"/>

<xsl:strip-space elements="xs:*"/>
<xsl:variable name="indent" select="'  '"/>


<xsl:template name="newline"><xsl:text>&#xA;</xsl:text></xsl:template>


<xsl:template match="*">
  <xsl:element name="{local-name()}">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>


<xsl:template match="x:schemaref[@role != '']">
  <xsl:apply-templates select="document(@uri)/xs:schema/xs:complexType[@name=current()/@role]" mode="xsdoc"/>
</xsl:template>

<xsl:template match="x:schemaref">
  <xsl:apply-templates select='document(@uri)' mode="print"/>
</xsl:template>

</xsl:stylesheet>
