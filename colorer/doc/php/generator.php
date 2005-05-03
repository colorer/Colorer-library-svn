<?require("paths.php");?>
<?
header("Content-type: text/html; charset=$o_encoding");

$type = check($type);
$hrd_color = check($hrd_color);
$i_encoding = check($i_encoding);
$o_encoding = check($o_encoding);
$upload = check($upload);

print $upload;

$col_name = $upload;
srand((double)microtime()*1000000);
$tempname = "./temp/temp-".rand(0,30);

$command = "-h";

if ($hrd_color == "") $hrd_color = "default";
if ($hrd_color == "tokens") $command = "-ht";

if ($upload == 'none' || $upload == ''){
  $file = fopen($tempname, "w");
  $file_content = str_replace('\"', '"', $file_content);
  $file_content = str_replace('\\\'', '\'', $file_content);
  $file_content = str_replace('\\\\', '\\', $file_content);
  fwrite($file, $file_content);
  fclose($file);
  $col_name = $tempname;
};

$out_type = "";
if ($type != ""){
  $out_type = "\"-t$type\"";
};

$cmd = "$colorer_path $out_type -i $hrd_color -ei $i_encoding -eo $o_encoding  $command  $col_name";
// utf-16 and utf-32 should fail on win32 systems because of \r\n stdout writing

passthru($cmd);

?>
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
