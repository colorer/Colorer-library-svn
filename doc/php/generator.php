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