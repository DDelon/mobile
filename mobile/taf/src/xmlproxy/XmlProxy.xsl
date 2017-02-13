<?xml version="1.0" encoding="GBK"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>
<xsl:template match="/">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<title></title>
<style type="text/css">
.style1 {
	font-size: 12px;
	font-weight: bold;
}
.style2 {font-size: 12px}
.jive-table {
	BORDER-RIGHT: #bbb 1px solid; BORDER-TOP: #bbb 1px solid; BORDER-LEFT: #bbb 1px solid; BORDER-BOTTOM: #bbb 1px solid
}
</style>
</head>

<body>
<xsl:apply-templates/>
<p class="style2">˵����<br/>
�����ڵ�Ӣ�ģ���ʾ��ӦxmlԪ�ص����ơ�</p>
</body>
</html>
</xsl:template>

<xsl:template match="request">
<table width="100%"  border="0" cellspacing="1" cellpadding="5" class="jive-table">
  <tr>
    <td bgcolor="#eaf1f8" class="style1">����(request)</td>
  </tr>
  <tr>
    <td>
	<table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="20%"><span class="style1">������(servant)</span></td>
        <td width="80%" class="style2"><xsl:value-of select="locator"/></td>
      </tr>
	  <tr>
		<td colspan="2"><hr size="1" noshade="noshade" style="border:1px #000000 dotted;"/></td>
	  </tr>
      <tr>
        <td width="20%"><span class="style1">������(function_name)</span></td>
        <td width="80%" class="style2"><xsl:value-of select="function_name"/></td>
      </tr>
	  <tr>
		<td colspan="2"><hr size="1" noshade="noshade" style="border:1px #000000 dotted;"/></td>
	  </tr>
      <tr>
        <td class="style1">����ֵ(return)</td>
        <td class="style2">
			<xsl:apply-templates select='return' mode="three"/>
		</td>
      </tr>
	  <tr>
		<td colspan="2"><hr size="1" noshade="noshade" style="border:1px #000000 dotted;"/></td>
	  </tr>
      <tr>
        <td class="style1">�����б�(parameters)</td>
        <td class="style2"><xsl:apply-templates select='parameters' mode="five"/></td>
      </tr>
    </table>
	</td>
  </tr>
</table>
<hr size="1" noshade="noshade" style="border:1px #cccccc dotted;"/>
</xsl:template>

<xsl:template match="parameters" mode="five">
<table width="100%"  border="0" cellspacing="1" cellpadding="5" class="jive-table">
  <tr>
    <td bgcolor="#eaf1f8" class="style1" width="20%">��������</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">��������(name)</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">����(in)</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">���(out)</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">����ֵ</td>
  </tr>
   <xsl:apply-templates select="*" mode="five"/>
</table>
</xsl:template>

<xsl:template match="*" mode="five">
  <tr>
    <td><xsl:value-of select="name()"/></td>
    <td><xsl:value-of select="@name"/></td>
    <td><xsl:value-of select="@in"/></td>
    <td><xsl:value-of select="@out"/></td>
    <td><xsl:value-of select="text()"/></td>
  </tr>

<xsl:if test="name()='struct'"> 
  <tr>
    <td colspan="5"><xsl:apply-templates select="." mode="four"/></td>
  </tr>
</xsl:if>

<xsl:if test="name()='vector' or name()='map'"> 
  <tr>
    <td colspan="5"><xsl:apply-templates select="." mode="three"/></td>
  </tr>
</xsl:if>

  <tr>
    <td colspan="5"><hr size="1" noshade="noshade" style="border:1px #cccccc dotted;"/></td>
  </tr>
</xsl:template>

<!--three start-->
<xsl:template match="vector" mode="three">
<table width="100%"  border="0" cellspacing="1" cellpadding="5" class="jive-table">
  <tr>
    <td bgcolor="#eaf1f8" class="style1" width="20%">����</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">����(name)</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">��ֵ</td>
  </tr>
   <xsl:apply-templates mode="three_2"/>
</table>
</xsl:template>


<xsl:template match="map" mode="three">
<table width="100%"  border="0" cellspacing="1" cellpadding="5" class="jive-table">
  <tr>
    <td bgcolor="#eaf1f8" class="style1" colspan="3">key</td>
  </tr>
  <tr>
    <td bgcolor="#eaf1f8" class="style1" width="20%">����</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">����(name)</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">��ֵ</td>
  </tr>
   <xsl:apply-templates select="entry/*[last() - 1]" mode="three_2"/>
</table>
<table width="100%"  border="0" cellspacing="1" cellpadding="5" class="jive-table">
  <tr>
    <td bgcolor="#eaf1f8" class="style1" colspan="3">value</td>
  </tr>
  <tr>
    <td bgcolor="#eaf1f8" class="style1" width="20%">����</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">����(name)</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">��ֵ</td>
  </tr>
   <xsl:apply-templates select="entry/*[last()]" mode="three_2"/>
</table>
</xsl:template>


<xsl:template match="return" mode="three">
<table width="100%"  border="0" cellspacing="1" cellpadding="5" class="jive-table">
  <tr>
    <td bgcolor="#eaf1f8" class="style1" width="20%">��������</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">��������(name)</td>
    <td bgcolor="#eaf1f8" class="style1" width="20%">����ֵ</td>
  </tr>
   <xsl:apply-templates mode="three_2"/>
</table>
</xsl:template>


<xsl:template match="*" mode="three_2">
  <tr>
    <td><xsl:value-of select="name()"/></td>
    <td><xsl:value-of select="@name"/></td>
    <td><xsl:value-of select="text()"/></td>
  </tr>
	<xsl:if test="name()='struct'"> 
	  <tr>
		<td colspan="3"><xsl:apply-templates select="." mode="four"/></td>
	  </tr>
	</xsl:if>

	<xsl:if test="name()='vector'"> 
	  <tr>
		<td colspan="3"><xsl:apply-templates select="." mode="three"/></td>
	  </tr>
	</xsl:if>

	<xsl:if test="name()='map'"> 
	  <tr>
		<td colspan="3"><xsl:apply-templates select="." mode="three"/></td>
	  </tr>
	</xsl:if>
</xsl:template>


<!--four start-->
<xsl:template match="struct" mode="four">
<table width="100%"  border="0" cellspacing="1" cellpadding="5" class="jive-table">
  <tr>
    <td bgcolor="#eaf1f8" class="style2" colspan="4">�ṹ��ϸ��Ϣ</td>
  </tr>
  <tr>
    <td bgcolor="#eaf1f8" class="style2" width="20%">�ֶ�����</td>
    <td bgcolor="#eaf1f8" class="style2" width="20%">�ֶ�����(name)</td>
    <td bgcolor="#eaf1f8" class="style2" width="20%">tag(tag)</td>
    <td bgcolor="#eaf1f8" class="style2" width="20%">�ֶ�ֵ</td>
  </tr>
   <xsl:apply-templates mode="four_2"/>
</table>
</xsl:template>

<xsl:template match="*" mode="four_2">
  <tr>
    <td><xsl:value-of select="name()"/></td>
    <td><xsl:value-of select="@name"/></td>
    <td><xsl:value-of select="@tag"/></td>
    <td><xsl:value-of select="text()"/></td>
  </tr>
	<xsl:if test="name()='struct'"> 
	  <tr>
		<td colspan="4"><xsl:apply-templates select="." mode="four"/></td>
	  </tr>
	</xsl:if>
</xsl:template>



</xsl:stylesheet>
