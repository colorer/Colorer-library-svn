<?require("paths.php");?>
<?header("Content-type: text/html; charset=\"UTF-8\"");?>
<html>
<head>
<title>colorer on sourceforge.net</title>
<link rel="stylesheet" type="text/css" href="../styles/styles.css"/>
<link rel="icon" href="../images/logo-small.png"/>
</head>
<body>
<div class='back'><a href='../index.html'><img alt='back' src='../images/back.png' width='40' height='18'/></a></div>
<div>
<a href="http://sourceforge.net"><img align=right src="http://sourceforge.net/sflogo.php?group_id=34855&amp;type=2" width="125" height="37" border="0" alt="SourceForge.net Logo"/></a>
</div>

<h2>Colorer-take5 demos page</h2>
<p>This is the dynamically generated demos of Colorer Library take5.</p>

<h2>Available demo files:</h2>
<?

exec("ls -1 demos", $list);

for($i = 0; $i < count($list); $i++){
  $fname = $list[$i];
  print "<a href='demos.php?filename=$fname'>".$fname."</a><br>";
};
if ($filename != ''){
    $fpath = "./demos/".$filename;
    print("<h2><a name='$i'></a>filename:".$filename."</h2>");
    print("<pre>");
    passthru("$colorer_path -db -dc -dh -ei UTF-8 -eo UTF-8 -h \"$fpath\"");
    print("</pre>");
};
?>

</body>
</html>