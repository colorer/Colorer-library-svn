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
    exec("$colorer_path -l", $list_descr);
    for($i = 0; $i < count($list); $i++)
    print "<option value='$list[$i]'>$list_descr[$i]</option>\n";?>
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
<!-- ***** BEGIN LICENSE BLOCK *****
   - Version: MPL 1.1/GPL 2.0/LGPL 2.1
   -
   - The contents of this file are subject to the Mozilla Public License Version
   - 1.1 (the "License"); you may not use this file except in compliance with
   - the License. You may obtain a copy of the License at
   - http://www.mozilla.org/MPL/
   -
   - Software distributed under the License is distributed on an "AS IS" basis,
   - WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
   - for the specific language governing rights and limitations under the
   - License.
   -
   - The Original Code is the Colorer Library.
   -
   - The Initial Developer of the Original Code is
   - Cail Lomecb <cail@nm.ru>.
   - Portions created by the Initial Developer are Copyright (C) 1999-2005
   - the Initial Developer. All Rights Reserved.
   -
   - Contributor(s):
   -
   - Alternatively, the contents of this file may be used under the terms of
   - either the GNU General Public License Version 2 or later (the "GPL"), or
   - the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
   - in which case the provisions of the GPL or the LGPL are applicable instead
   - of those above. If you wish to allow use of your version of this file only
   - under the terms of either the GPL or the LGPL, and not to allow others to
   - use your version of this file under the terms of the MPL, indicate your
   - decision by deleting the provisions above and replace them with the notice
   - and other provisions required by the LGPL or the GPL. If you do not delete
   - the provisions above, a recipient may use your version of this file under
   - the terms of any one of the MPL, the GPL or the LGPL.
   -
   - ***** END LICENSE BLOCK ***** -->
