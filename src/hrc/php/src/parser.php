<?

  function w($s) {
    global $fpw;
    fwrite($fpw, $s."\n");
  }

  $fp = fopen($argv[1],"r");
  $fpw = fopen($argv[2],"w");

  w('<?');
  w('  $i = 0;');
  $group_n = 0;

  while ($str = fgets($fp, 2048)) {
    $str = preg_replace("/\r|\n/m","",$str);
    if (preg_match("/<!--.*-->/m", $str)) {
      // START: first case: group
      $group_n++;
      $str = preg_replace("/^\s*<!--\s*(.+?)\s*-->\s*/m","$1",$str);
      w('  $i++;');
      w('  $GROUPS[$i]["DESCRIPTION"] = "'.$str.'";');
      $groupname = ucwords($str); //changing case - upper first letters
      $groupname = preg_replace("/^[XLIVC]*\.\s+/m","",$groupname); //trim the number
      $groupname = preg_replace("/[^a-z_-]/im","-",$groupname);
      $groupname = preg_replace("/-{2,}/im","-",$groupname);
      $groupname = preg_replace("/^[\s\.\-]*/m","",$groupname); //trim starting points, spacers, ..
      $groupname = preg_replace("/[\s\.\-]*$/m","",$groupname); //trim ending points, spacers, ..
      w('  $GROUPS[$i]["NAME"] = "'.$groupname.'";');
      w('  $GROUPS[$i]["FUNCTIONS"] = array();');
      w('  $GROUPS[$i]["CONSTANTS"] = array();');
      w('  $GROUPS[$i]["DEPRECATED"] = array();');
      w('  $GROUPS[$i]["LANGUAGE"] = array();');
      // END:   first case: group
    } elseif (preg_match("/ -- /m",$str)) { //if we have " -- " so we hit on function
      // START: second case: function (deprecated and not deprecated)
      $funcname = preg_replace("/\s+--.*$/m","",$str); //trimming " -- description"
      $funcname = preg_replace("/^.*((\:\:)|(-\>))/m","",$funcname); //trimming "class->" and "class::" in the start of a string
      if (preg_match("/\Wdepreca/i", $str)) {
        w('    $GROUPS[$i]["DEPRECATED"][] = "'.$funcname.'"; //'.$str);
      } else {
        w('    $GROUPS[$i]["FUNCTIONS"][] = "'.$funcname.'"; //'.$str);
      }
      // END:   second case: function (deprecated and not deprecated)
    } elseif (preg_match("/~\w+/im",$str)) { //types cast
      $typename = preg_replace("/~/im","",$str);
      w('    $GROUPS[$i]["TYPES"][] = "'.$typename.'"; //'.$str);
    } elseif (preg_match("/^([a-z_\d]+)/im",$str)) { //else we interpret string as CONSTANT
      // START: third case: function
      $constname = preg_replace("/^([a-z_\d]+).*?$/im","$1",$str); //getting only the first part of a string
      if ($group_n == 1) {
        w('    $GROUPS[$i]["LANGUAGE"][] = "'.$constname.'"; //'.$str);
      } else {
        w('    $GROUPS[$i]["CONSTANTS"][] = "'.$constname.'"; //'.$str);
      }
    }
  }

  w('?>');

  fclose($fp);
  fclose($fpw);


/* - ***** BEGIN LICENSE BLOCK *****
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
   - The Original Code is the PHP keywords to HRC parser.
   -
   - The Initial Developer of the Original Code is
   - Alexey Kupershtokh <wicked@ngs.ru>
   - Portions created by the Initial Developer are Copyright (C) 2004
   - the Initial Developer. All Rights Reserved.
   -
   - Contributor(s):
   - Eugene Efremov <4mirror@mail.ru>
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
   - ***** END LICENSE BLOCK *****/
?>