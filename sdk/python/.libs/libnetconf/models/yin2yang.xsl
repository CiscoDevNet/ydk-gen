<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
  xmlns:yin="urn:ietf:params:xml:ns:yang:yin:1"
  xmlns:nacm="urn:ietf:params:xml:ns:yang:ietf-netconf-acm"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xalan="http://xml.apache.org/xslt">
<xsl:output method="text"/>

<xsl:template name="indent">
  <xsl:param name="count"/>
  <xsl:if test="$count &gt; 0">
    <xsl:text>  </xsl:text>
    <xsl:call-template name="indent">
      <xsl:with-param name="count" select="$count - 1"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template match="yin:namespace">
  <xsl:param name="pLevel"/>
  <xsl:call-template name="indent"><xsl:with-param name="count" select="$pLevel"/></xsl:call-template>
  <xsl:text>namespace "</xsl:text><xsl:value-of select="@uri"/><xsl:text>";
</xsl:text>
</xsl:template>

<xsl:template match="yin:*[@name]|yin:*[@value]|yin:*[@date]|yin:*[@condition]|yin:*[@module]|yin:*[@target-node]|yin:*[@tag]">
  <xsl:param name="pLevel" select="0"/>
  <xsl:call-template name="indent"><xsl:with-param name="count" select="$pLevel"/></xsl:call-template>
  <xsl:value-of select="local-name(.)" />
  <xsl:choose>
    <xsl:when test="@name">
      <xsl:text> "</xsl:text><xsl:value-of select="@name"/><xsl:text>"</xsl:text>
    </xsl:when>
    <xsl:when test="@value">
      <xsl:text> "</xsl:text><xsl:value-of select="@value"/><xsl:text>"</xsl:text>
    </xsl:when>
    <xsl:when test="@date">
      <xsl:text> "</xsl:text><xsl:value-of select="@date"/><xsl:text>"</xsl:text>
    </xsl:when>
    <xsl:when test="@condition">
      <xsl:text> "</xsl:text><xsl:value-of select="@condition"/><xsl:text>"</xsl:text>
    </xsl:when>
    <xsl:when test="@module">
      <xsl:text> "</xsl:text><xsl:value-of select="@module"/><xsl:text>"</xsl:text>
    </xsl:when>
    <xsl:when test="@target-node">
      <xsl:text> "</xsl:text><xsl:value-of select="@target-node"/><xsl:text>"</xsl:text>
    </xsl:when>
    <xsl:when test="@tag">
      <xsl:text> "</xsl:text><xsl:value-of select="@tag"/><xsl:text>"</xsl:text>
    </xsl:when>
  </xsl:choose>

  <xsl:choose>
    <xsl:when test="./node()" ><xsl:text> {
</xsl:text>
      <xsl:apply-templates select="*">
        <xsl:with-param name="pLevel" select="$pLevel +1"/>
      </xsl:apply-templates>
      <xsl:call-template name="indent"><xsl:with-param name="count" select="$pLevel"/></xsl:call-template>
      <xsl:text>}

</xsl:text>
    </xsl:when>
    <xsl:otherwise><xsl:text>;
</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="yin:*[yin:text]|yin:error-message[yin:value]" >
  <xsl:param name="pLevel"/>
  <xsl:call-template name="indent"><xsl:with-param name="count" select="$pLevel"/></xsl:call-template>
  <xsl:value-of select="local-name(.)" /><xsl:text>
</xsl:text>
  <xsl:call-template name="indent"><xsl:with-param name="count" select="$pLevel"/></xsl:call-template>
  <xsl:text>  "</xsl:text>
  <xsl:choose>
    <xsl:when test="yin:text">
      <xsl:value-of disable-output-escaping="no" select="yin:text" />
    </xsl:when>
    <xsl:when test="yin:value">
      <xsl:value-of disable-output-escaping="no" select="yin:value" />
    </xsl:when>
  </xsl:choose>
  <xsl:text>";
</xsl:text>
</xsl:template>

<xsl:template match="yin:input|yin:output">
  <xsl:param name="pLevel"/>
  <xsl:call-template name="indent"><xsl:with-param name="count" select="$pLevel"/></xsl:call-template>
  <xsl:value-of select="local-name(.)"/>
  <xsl:choose>
    <xsl:when test="./node()" ><xsl:text> {
</xsl:text>
      <xsl:apply-templates select="*">
        <xsl:with-param name="pLevel" select="$pLevel +1"/>
      </xsl:apply-templates>
      <xsl:call-template name="indent"><xsl:with-param name="count" select="$pLevel"/></xsl:call-template>
      <xsl:text>}

</xsl:text>
    </xsl:when>
    <xsl:otherwise><xsl:text>;
</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="nacm:*">
  <xsl:param name="pLevel"/>
  <xsl:call-template name="indent"><xsl:with-param name="count" select="$pLevel"/></xsl:call-template>
  <xsl:value-of select="name(.)"/><xsl:text>;
</xsl:text>
</xsl:template>

<!-- skip complete subtree of unknown nodes -->
<xsl:template match="*"/>

</xsl:stylesheet>
