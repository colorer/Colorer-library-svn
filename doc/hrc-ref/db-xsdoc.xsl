<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xs="http://www.w3.org/2001/XMLSchema"
                >

<xsl:strip-space elements="xs:*"/>


<xsl:template match="*" mode="xsdoc"/>

<xsl:template match="xs:annotation" mode="xsdoc"/>

<xsl:template match="xs:complexType[@name and xs:*[1][self::xs:annotation]]" mode="xsdoc">

 <xsl:variable name='ann' select='xs:*[1][self::xs:annotation]'/>

 <anchor id='type{@name}'/>
 <para role='xsdocwrap'>
  <para role='xsdocdecl'>Element Name:
   <literal><xsl:value-of select='@name'/></literal>
   <xsl:if test='@name'>, type:
   <link linkend='xsid{@name}'><literal><xsl:value-of select='@name'/></literal></link>
   </xsl:if>
  </para>
  <para role='xsdoc'>
    <xsl:value-of select='normalize-space($ann/xs:documentation)'/>
  </para>
  <xsl:apply-templates select='.//xs:attribute[@name]' mode='xsdoc'/>
  <xsl:if test='.//xs:element[@name]'>
    <para role='xsdocdecl'>Content:</para>
    <xsl:apply-templates select='.//xs:element[@name]' mode='xsdoc'/>
  </xsl:if>
 </para>
</xsl:template>


<xsl:template match="xs:element[@name and not(ancestor::xs:element)]" mode="xsdoc">

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
   <literal><xsl:value-of select='@name'/></literal>
   <xsl:choose>
    <xsl:when test='@type'>, type:
    <link linkend='type{@type}'><literal><xsl:value-of select='@type'/></literal></link>
    </xsl:when>
    <xsl:when test='@name'>, type:
    <link linkend='xsid{@name}'><literal><xsl:value-of select='@name'/></literal></link>
    </xsl:when>
   </xsl:choose>
  </para>
  <para role='xsdoc'>
    <xsl:value-of select='normalize-space($ann)'/>
  </para>
</xsl:template>


<xsl:template match="xs:attribute[@name and not(ancestor::xs:element)]" mode="xsdoc">

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
     <link linkend='xsid{@type}'><literal><xsl:value-of select='@type'/></literal></link>
    </xsl:when>
   </xsl:choose>
    <xsl:if test='@default'>, default:
     <literal><xsl:value-of select='@default'/></literal>
    </xsl:if>
  </para>
  <para role='xsdoc'>
    <xsl:value-of select='normalize-space($ann)'/>
  </para>
</xsl:template>

</xsl:stylesheet>
