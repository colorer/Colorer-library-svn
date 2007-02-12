<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xs="http://www.w3.org/2001/XMLSchema"
                >

<!-- Cloned from James Clark rng spec stylesheets -->

<xsl:template name="value">
  <xsl:param name="id" select="''"/>
  <xsl:choose>
    <xsl:when test="(parent::xs:complexType or parent::xs:complexType) and name() = 'name'">
      <link linkend="typedescr_{generate-id(/)}_{.}"><xsl:value-of select="."/></link>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="."/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<xsl:template match="/" mode="print">
  <xsl:param name="id" select="'xsid'"/>
  <programlisting>
    <xsl:apply-templates mode="print">
      <xsl:with-param name="id" select="$id"/>
    </xsl:apply-templates>
  </programlisting>
</xsl:template>


<xsl:template match="*|xs:annotation" mode="print"/>

<xsl:template match="xs:*" mode="print">
  <xsl:param name="totalIndent" select="''"/>
  <xsl:param name="id" select="''"/>

  <xsl:if test="parent::xs:schema">
    <xsl:call-template name="newline"/>
  </xsl:if>

  <xsl:if test="self::xs:complexType[@name] or self::xs:simpleType[@name]">
  <!-- or self::xs:element[@name and not(@type)] -->
    <anchor id='{$id}_{generate-id(/)}_{@name}'/>
  </xsl:if>

  <xsl:value-of select="$totalIndent"/>
  <xsl:text>&lt;</xsl:text>
  <literal role='xs_elem'><xsl:value-of select="local-name()"/></literal>
  <xsl:apply-templates select="@*" mode="print">
    <xsl:with-param name="totalIndent" select="concat($totalIndent, $indent)"/>
    <xsl:with-param name="id" select="$id"/>
  </xsl:apply-templates>
  <xsl:if test="not(parent::*)">
    <xsl:choose>
      <xsl:when test="@*">
        <xsl:call-template name="newline"/>
        <xsl:value-of select="$totalIndent"/>
        <xsl:value-of select="$indent"/>
      </xsl:when>
      <xsl:otherwise><xsl:text> </xsl:text></xsl:otherwise>
    </xsl:choose>
    <xsl:text>xmlns=&quot;</xsl:text>
    <xsl:value-of select="namespace-uri()"/>
    <xsl:text>&quot;</xsl:text>
    <xsl:for-each select="namespace::*[local-name() and local-name() != 'xml']">
      <xsl:call-template name="newline"/>
      <xsl:value-of select="$totalIndent"/>
      <xsl:value-of select="$indent"/>
      <xsl:text>xmlns:</xsl:text>
      <xsl:value-of select="local-name()"/>
      <xsl:text>=&quot;</xsl:text>
      <xsl:value-of select="."/>
      <xsl:text>&quot;</xsl:text>
    </xsl:for-each>
  </xsl:if>
  <xsl:choose>
    <xsl:when test="text()">
      <xsl:text>&gt;</xsl:text>
      <xsl:value-of select="."/>
      <xsl:text>&lt;/</xsl:text>
      <literal role='xs_elem'><xsl:value-of select="local-name()"/></literal>
      <xsl:text>&gt;</xsl:text>
    </xsl:when>
    <xsl:when test="not(*)">/&gt;</xsl:when>
    <xsl:otherwise>
      <xsl:text>&gt;</xsl:text>
      <xsl:call-template name="newline"/>
      <xsl:apply-templates mode="print">
        <xsl:with-param name="totalIndent" select="concat($totalIndent, $indent)"/>
        <xsl:with-param name="id" select="$id"/>
      </xsl:apply-templates>
      <xsl:if test="self::xs:grammar">
        <xsl:call-template name="newline"/>
      </xsl:if>
      <xsl:value-of select="$totalIndent"/>
      <xsl:text>&lt;/</xsl:text>
      <literal role='xs_elem'><xsl:value-of select="local-name()"/></literal>
      <xsl:text>&gt;</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="newline"/>
</xsl:template>

<xsl:template match="@*" mode="print">
  <xsl:param name="id" select="''"/>
  <xsl:text> </xsl:text>
  <literal role='xs_attr'><xsl:value-of select="local-name()"/></literal>
  <xsl:text>=&quot;</xsl:text>
  <literal role='xs_attrval'>
  <xsl:call-template name="value">
    <xsl:with-param name="id" select="$id"/>
  </xsl:call-template>
  </literal>
  <xsl:text>&quot;</xsl:text>
</xsl:template>

</xsl:stylesheet>
