<?require("paths.php");?>
<html>
<head>
<title>Colorer on sourceforge.net</title>
<link rel="stylesheet" type="text/css" href="../styles/styles.css"/>
<link rel="icon" href="../images/logo-small.png"/>
</head>
<body>
<div class='back'><a href='../index.html'><img alt='back' src='../images/back.png' width='40' height='18'/></a></div>

<div>
<a href="http://sourceforge.net"><img align=right src="http://sourceforge.net/sflogo.php?group_id=34855&amp;type=2" width="125" height="37" border="0" alt="SourceForge.net Logo"/></a>
</div>

<h2>Colorer-take5 HTML generator</h2>
<p>This is a web interface to the Colorer-take5 Library.<br/>
It uses newly released <a href='../index.html'>Colorer-take5.beta2</a> version.<br/>
It produces highlighted source text through the web interface.</p>

<form enctype="multipart/form-data" action="generator.php" method="post">
 <table border="1">
  <tr><td colspan="2"><input value="colorize!" type="submit"/></td></tr>

  <tr><td>upload(max 1mb):</td>
  <td>
   <input type="hidden" name="MAX_FILE_SIZE" value="1000000"/>
   <input size='25' name="upload" type="file"/>
  </td></tr>

  <tr>
  <td>or file content:</td>
  <td><textarea rows="10" cols="60" name="file_content"></textarea></td>
  </tr>

  <tr><td>input encoding:</td><td>
  <select name="i_encoding">
    <option selected='selected' value='UTF-8'>UTF-8</option>
    <option value='ISO-8859-1'>ISO-8859-1</option>
    <option value='koi8-r'>koi8-r</option>
    <option value='cp1251'>cp1251</option>
    <option value='cp866'>cp866</option>
    <option value='UTF-16'>UTF-16</option>
    <option value='UTF-16BE'>UTF-16BE</option>
    <option value='UTF-32'>UTF-32</option>
    <option value='UTF-32BE'>UTF-32BE</option>
  </select>
  </td></tr>

  <tr><td>style:</td><td>
  <select name="hrd_color">
    <option selected='selected' value='default'>white</option>
    <option value='navy'>navy</option>
    <option value='black'>black</option>
    <option value='neo'>neo</option>
    <option value='eclipse'>eclipse</option>
    <option value='grayscale'>grayscale</option>
    <option value='fmx'>FMX</option>
    <option value='hs'>HomeSite</option>
    <option value='bred3'>Bred3</option>
    <option value='tags'>html tags</option>
    <option value='tokens'>tokenize</option>
  </select>
  </td></tr>

  <tr><td>type name:</td><td>
  <select name="type" size='20'>
    <option selected='selected' value=''>Autodetect</option>
<?
    exec("$colorer_path -lt", $list);
    for($i = 0; $i < count($list); $i++)
    print "<option value='".substr(strstr($list[$i],":"),2)."'>$list[$i]</option>\n";?>
  </select>
  </td></tr>

  <tr><td>output encoding:</td><td>
  <select name="o_encoding">
    <option selected='selected' value='UTF-8'>UTF-8</option>
    <option value='ISO-8859-1'>ISO-8859-1</option>
    <option value='cp1251'>cp1251</option>
    <option value='cp866'>cp866</option>
    <option value='UTF-16'>UTF-16</option>
    <option value='UTF-16BE'>UTF-16BE</option>
    <option value='UTF-32'>UTF-32</option>
    <option value='UTF-32BE'>UTF-32BE</option>
  </select>
  </td></tr>

  <tr><td colspan="2"><input value="colorize!" type="submit"/></td></tr>
 </table>
</form>

</body>
</html>