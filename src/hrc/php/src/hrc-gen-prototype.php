<parameters>
<?php
  include_once($argv[1]);
  reset ($GROUPS);
  while(list($i,) = each($GROUPS))
  {
   $rg=htmlspecialchars($GROUPS[$i]["DESCRIPTION"]);
   echo '   <param name="Include-'.$GROUPS[$i]["NAME"]."\" value='true' description='$rg' />\n";
  }
?>
</parameters>
