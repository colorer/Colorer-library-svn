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
<p>This is the dynamically generated demos of Colorer Library take5.</p>

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
    <option <?=($hrd_color == 'grayscale')?"selected":""?> value='grayscale'>grayscale</option>
    <option <?=($hrd_color == 'tags')?"selected":""?> value='tags'>html tags</option>
    <option <?=($hrd_color == 'tokens')?"selected":""?> value='tokens'>tokenize</option>
  </select>
  <input type='submit' value='show'/>
</form>
<?
if ($filename != ''){
    echo "<hr/>";
    $fpath = "./demos/".$filename;
    print("<h2><a name='$i'></a>filename:".$filename."</h2>");
    print("<pre>");
    passthru("$colorer_path -db -dc -dh -i $hrd_color -ei UTF-8 -eo UTF-8 -h \"$fpath\"");
    print("</pre>");
};
?>

</body>
</html>