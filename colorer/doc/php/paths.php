<?
  if (getenv('COMSPEC') == ''){
    // unix/linux
    //$colorer_path = "/usr/bin/colorer"
    $colorer_path = "/home/groups/c/co/colorer/cgi-bin/colorer -c /home/groups/c/co/colorer/files/catalog.xml";
  }else{
    //win32
    $colorer_path = "d:/programs/devel/colorer/bin/colorer.exe";
  };
  echo $colorer_path;
?>