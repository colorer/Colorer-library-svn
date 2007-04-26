<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xs="http://www.w3.org/2001/XMLSchema"
                >

<xsl:template match="*" mode="xsdoc"/>

<xsl:template match="xs:annotation" mode="xsdoc"/>

<xsl:template match="xs:complexType[@name and xs:*[1][self::xs:annotation]]" mode="xsdoc">

 <xsl:variable name='reftype' select='/xs:schema/xs:element/@name'/>
 <xsl:variable name='ann' select='xs:*[1][self::xs:annotation]'/>

 <anchor id='ref.{$reftype}.{@name}'/>
 <informalexample role='xsdocwrap'>
  <para role='xsdocdecl'>Element:
    <link linkend='xsid_{$reftype}_{@name}'><literal>&lt;<xsl:value-of select='@name'/>&gt;</literal></link>
  </para>
  <para role='xsdoc'>
    <xsl:value-of select='normalize-space($ann/xs:documentation)'/>
  </para>
  <xsl:apply-templates select='.//xs:attribute[@name]' mode='xsdoc'/>
  <xsl:apply-templates select='.//xs:complexContent' mode='xsdoc'/>
  <xsl:if test='.//xs:element[@name]'>
    <para role='xsdocdecl'>Content:</para>
    <xsl:apply-templates select='.//xs:element[@name]' mode='xsdoc'/>
  </xsl:if>
 </informalexample>
</xsl:template>


<xsl:template match="xs:complexContent" mode='xsdoc'>
  <xsl:variable name="this_att" select="xs:extension/xs:attribute" />
  <xsl:variable name="extd_att" select="/*/xs:complexType[@name = current()/xs:extension/@base]//xs:attribute" />

  <!-- process intersection for extended elements -->
  <xsl:apply-templates select='$extd_att[@name != $this_att/@name]' mode='xsdoc'/>
</xsl:template>


<xsl:template match="xs:element[@name and not(ancestor::xs:element)]" mode="xsdoc">

  <xsl:variable name='reftype' select='/xs:schema/xs:element/@name'/>
  <xsl:variable name='ann'>

    <xsl:variable name='anni'
          select='//xs:complexType[@name = current()/@type] /
                  xs:*[1][self::xs:annotation]/xs:documentation'/>
    <xsl:choose>
     <xsl:when test='xs:*[1][self::xs:annotation]'>
       <xsl:value-of select='xs:*[1][self::xs:annotation]/xs:documentation'/>
     </xsl:when>
     <xsl:when test='$anni'>
       <xsl:choose>
        <xsl:when test='contains($anni, ".")'>
          <xsl:value-of select='concat(substring-before($anni,"."),".")'/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select='$anni'/>
        </xsl:otherwise>
       </xsl:choose>
     </xsl:when>
    </xsl:choose>
  </xsl:variable>

  <para role='xsdochead'>Element:
   <xsl:choose>
    <xsl:when test='@type = @name'>
      <link linkend='ref.{$reftype}.{@type}'><literal><xsl:value-of select='@type'/></literal></link>
    </xsl:when>
    <xsl:when test='@type'>
      <literal><xsl:value-of select='@name'/></literal>, type:
      <link linkend='ref.{$reftype}.{@type}'><literal><xsl:value-of select='@type'/></literal></link>
    </xsl:when>
    <xsl:otherwise>
      <literal><xsl:value-of select='@name'/></literal>
    </xsl:otherwise>
   </xsl:choose>
  </para>
  <para role='xsdoc'>
    <xsl:value-of select='normalize-space($ann)'/>
  </para>
</xsl:template>


<xsl:template match="xs:attribute[@name and not(ancestor::xs:element)]" mode="xsdoc">

  <xsl:variable name='reftype' select='/xs:schema/xs:element/@name'/>
  <xsl:variable name='ann'>

    <xsl:variable name='anni'
          select='//xs:complexType[@name = current()/@type] /
                  xs:*[1][self::xs:annotation]/xs:documentation
                  |
                  //xs:simpleType[@name = current()/@type] /
                  xs:*[1][self::xs:annotation]/xs:documentation
                  '/>
    <xsl:choose>
     <xsl:when test='xs:*[1][self::xs:annotation]'>
       <xsl:value-of select='xs:*[1][self::xs:annotation]/xs:documentation'/>
     </xsl:when>
     <xsl:when test='$anni'>
       <xsl:choose>
        <xsl:when test='contains($anni, ".")'>
          <xsl:value-of select='concat(substring-before($anni,"."),".")'/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select='$anni'/>
        </xsl:otherwise>
       </xsl:choose>
     </xsl:when>
    </xsl:choose>
  </xsl:variable>

  <para role='xsdochead'>Attribute:
   <literal><xsl:value-of select='@name'/></literal>
   <xsl:choose>
    <xsl:when test='substring(@type, 1, 3) = "xs:"'>, type:
     <ulink url='http://www.w3.org/TR/xmlschema-2#{substring(@type, 4)}'><literal><xsl:value-of select='@type'/></literal></ulink>
    </xsl:when>
    <xsl:when test='@type'>, type:
     <link linkend='xsid_{$reftype}_{@type}'><literal><xsl:value-of select='@type'/></literal></link>
    </xsl:when>
   </xsl:choose>
    <xsl:if test='@default'>, default:
     <literal><xsl:value-of select='@default'/></literal>
    </xsl:if>
  </para>
  <xsl:if test="string-length($ann) > 0">
    <para role='xsdoc'>
      <xsl:value-of select='normalize-space($ann)'/>
    </para>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
