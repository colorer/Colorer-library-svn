<?

  function w($s = '') {
    global $fpw;
    fwrite($fpw, $s."\n");
  }

  include_once($argv[1]);

  if (!isset($GROUPS)) {
    die("variable \$GROUPS must be defined");
  }

  if (!is_array($GROUPS)) {
    die("variable \$GROUPS must be array");
  }

  $fpw = fopen($argv[2],"w");

  reset ($GROUPS);
  $WHOLE=array();
  while(list($i,) = each($GROUPS)) {
    $WHOLE["LANGUAGE"] = array_merge($WHOLE["LANGUAGE"], $GROUPS[$i]["LANGUAGE"]);
    $WHOLE["TYPES"] = array_merge($WHOLE["TYPES"], $GROUPS[$i]["TYPES"]);
    $WHOLE["CONSTANTS"] = array_merge($WHOLE["CONSTANTS"], $GROUPS[$i]["CONSTANTS"]);
    $WHOLE["FUNCTIONS"] = array_merge($WHOLE["FUNCTIONS"], $GROUPS[$i]["FUNCTIONS"]);
    $WHOLE["DEPRECATED"] = array_merge($WHOLE["DEPRECATED"], $GROUPS[$i]["DEPRECATED"]);
  }

  print count($WHOLE["LANGUAGE"])."\n";
  print count($WHOLE["TYPES"])."\n";
  print count($WHOLE["CONSTANTS"])."\n";
  print count($WHOLE["FUNCTIONS"])."\n";
  print count($WHOLE["DEPRECATED"])."\n";

  w('<!-- keyword defines begin here... -->');
  w('<scheme name="base-keywords">');

  reset ($GROUPS);
  while(list($i,) = each($GROUPS)) {
    w('  <inherit scheme="'.$GROUPS[$i]["NAME"].'"/>');
  }
  w('</scheme>');
  w();

  reset ($GROUPS);
  while(list($i,) = each($GROUPS)) {
    w('<scheme name="'.$GROUPS[$i]["NAME"].'" if="Include-'.$GROUPS[$i]["NAME"].'">');
    $add_nl = false;

    if (sizeof($GROUPS[$i]["LANGUAGE"]) > 0){
      $add_nl = true;

      natcasesort($GROUPS[$i]["LANGUAGE"]);
      w('  <keywords region="dKeyWord" ignorecase="yes" worddiv="[^\w_]">');
      while (list($j,) = each($GROUPS[$i]["LANGUAGE"])) {
        w('    <word name="'.$GROUPS[$i]["LANGUAGE"][$j].'"/>');
      }
      w('  </keywords>');
    }

    if (sizeof($GROUPS[$i]["TYPES"]) > 0){
      if ($add_nl) w();
      $add_nl = true;

      natcasesort($GROUPS[$i]["TYPES"]);
      w('  <keywords region="def:TypeKeyword" ignorecase="yes">');
      while (list($j,) = each($GROUPS[$i]["TYPES"])) {
        w('    <word name="'.$GROUPS[$i]["TYPES"][$j].'"/>');
      }
      w('  </keywords>');
    }

    if (sizeof($GROUPS[$i]["CONSTANTS"]) > 0){
      if ($add_nl) w();
      $add_nl = true;

      natcasesort($GROUPS[$i]["CONSTANTS"]);
      w('  <keywords region="dParam2"  worddiv="[^\w_]">');
      while (list($j,) = each($GROUPS[$i]["CONSTANTS"])) {
        w('    <word name="'.$GROUPS[$i]["CONSTANTS"][$j].'"/>');
      }
      w('  </keywords>');
    }

    if (sizeof($GROUPS[$i]["FUNCTIONS"]) > 0){
      if ($add_nl) w();
      $add_nl = true;

      natcasesort($GROUPS[$i]["FUNCTIONS"]);
      w('  <keywords region="dKeyWord2" ignorecase="yes" worddiv="[^\w_]">');
      while (list($j,) = each($GROUPS[$i]["FUNCTIONS"])) {
        w('    <word name="'.$GROUPS[$i]["FUNCTIONS"][$j].'"/>');
      }
      w('  </keywords>');
    }

    if (sizeof($GROUPS[$i]["DEPRECATED"]) > 0){
      if ($add_nl) w();
      $add_nl = true;

      natcasesort($GROUPS[$i]["DEPRECATED"]);
      w('  <keywords region="dDepr" ignorecase="yes" worddiv="[^\w_]">');
      while (list($j,) = each($GROUPS[$i]["DEPRECATED"])) {
        w('    <word name="'.$GROUPS[$i]["DEPRECATED"][$j].'"/>');
      }
      w('  </keywords>');
    }

    w('</scheme>');
    w();
  }

  fclose($fpw);

/*-- ***** BEGIN LICENSE BLOCK *****
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