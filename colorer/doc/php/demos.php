<?require("paths.php");?>
<?header("Content-type: text/html; charset=\"UTF-8\"");?>
<html>
<head>
<title>Colorer-take5 on sourceforge.net</title>
<link rel="stylesheet" type="text/css" href="../styles/styles.css"/>
<link rel="icon" href="../images/logo-small.png"/>
</head>
<body>
<div class='back'><a href='../index.html'><img alt='back' src='../images/back.png' width='40' height='18'/></a></div>
<div>
<a href="http://sourceforge.net"><img align=right src="http://sourceforge.net/sflogo.php?group_id=34855&amp;type=2" width="125" height="37" border="0" alt="SourceForge.net Logo"/></a>
</div>

<h1>Colorer-take5 demos page</h1>

<p>
Colorer supports highlighting and editing capabilities of
numerous programming languages, scripts and markups.
These sets are stored in the internal language (XML-based HRC files),
describing target's language syntax. HRC database is open and ready to
modify and growth.
</p>

<h2>Static highlighting samples</h2>

<table border='0'><tr>
<td valign='middle'><img alt='sample' src='../images/sample.png' height='60' width='60'/></td>
<td>
<a href="../lang-list.html">List of supported languages</a><br/>
<a href="../samples/BaseEditor.java.html">Java language with href doclinks</a><br/>
<a href="../samples/ipo-errors.xsd.html">XML Schema with errors and W3C site references</a><br/>
<a href="../samples/Unicode.html.html">XHTML in UTF-8</a>
</td>
</tr></table>


<h2>Dynamically generated demos</h2>

<form action='demos.php' method="get">
  <h3>Demo File:</h3>
  <select name="filename">
<?
exec("ls -1 demos", $list);
for($i = 0; $i < count($list); $i++){
  $fname = $list[$i];
  $cur = ($fname == $filename)?"selected":"";
  ?><option <?=$cur?> value='<?=$fname?>'><?=$fname?></option><?
};
?>
  </select>
  <h3>Coloring style:</h3>
  <select name="hrd_color">
    <option <?=($hrd_color == 'default')?"selected":""?> value='default'>white</option>
    <option <?=($hrd_color == 'navy')?"selected":""?> value='navy'>navy</option>
    <option <?=($hrd_color == 'black')?"selected":""?> value='black'>black</option>
    <option <?=($hrd_color == 'neo')?"selected":""?> value='neo'>neo</option>
    <option <?=($hrd_color == 'eclipse')?"selected":""?> value='eclipse'>eclipse</option>
    <option <?=($hrd_color == 'fmx')?"selected":""?> value='fmx'>FMX</option>
    <option <?=($hrd_color == 'hs')?"selected":""?> value='hs'>HomeSite</option>
    <option <?=($hrd_color == 'bred3')?"selected":""?> value='bred3'>Bred3</option>
    <option <?=($hrd_color == 'grayscale')?"selected":""?> value='grayscale'>grayscale</option>
    <option <?=($hrd_color == 'tags')?"selected":""?> value='tags'>html tags</option>
    <option <?=($hrd_color == 'tokens')?"selected":""?> value='tokens'>tokenize</option>
  </select>
  <input type='submit' value='show'/>
</form>
<?
if ($filename != ''){
    echo "<hr/>";

    $filename = check($filename);
    $hrd_color = check($hrd_color);

    $fpath = "./demos/".$filename;
    print("<h2><a name='$i'></a>filename:".$filename."</h2>");
    print("<pre>");
    passthru("$colorer_path -db -dc -dh -i $hrd_color -ei UTF-8 -eo UTF-8 -h \"$fpath\"");
    print("</pre>");
};
?>

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
