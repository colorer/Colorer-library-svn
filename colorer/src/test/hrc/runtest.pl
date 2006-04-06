#
# Perl module for automatic HRC changes tests.
# Validates a set of source files against a given
# set of their previous parse strucure.
#

#$colorer  = "D:/projects/colorer/bin/colorer.exe"; -- moved into %path%
$colorer  = "colorer -c D:\\projects\\colorer-4merge\\catalog.xml";

$diff  = 'diff -U 1 -bB';

#%modes = (
#  full  =>   ".",
#  quick =>   '(?!full\/)',
#  perl  =>   '^perl\/',
#);

$validDir = "_valid";

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
$currentDir = sprintf("__%d-%02d-%02d %02d-%02d", $year+1900, $mon+1, $mday, $hour, $min);

if (!mkdir $currentDir, 0777){
#  die "Can't create dir - already exists" ;
}

$runMode = shift @ARGV;

$runMode = "full" if (!$runMode);

if ($runMode ne "quick" and  $runMode ne "full" ){
  $runList = $runMode;
}

if ($runList){
  open LIST, $runList or die "Can't open test list: $runList";
  @retlist = <LIST>;
  close LIST;
}else{
  @retlist = collectDirs('.');
}

print "Running test mode: $runMode\n";

$testRuns = 0;
$testFailed = 0;

$timeStart = time();

unlink "$currentDir/fails.html";

open FAILS, ">$currentDir/fails.html";
print FAILS "<pre>\n";
close FAILS;

foreach (@retlist){
  chomp $_;

  my $origname = "$validDir/$_";
  my $fname = "$currentDir/$_";
  print "Processing (".($testRuns+1)."/".($#retlist+1).") $_:\n";

  open FAILS, ">>$currentDir/fails.html";
  print FAILS "\n<b>$_</b>:</pre></pre><pre>\n";
  close FAILS;
  checkDir($currentDir, $_);
  
  $cres = system "$colorer -h \"$_\" -dc -dh -ln -o \"$fname.html\"";

  $res = system "$diff \"$origname.html\" \"$fname.html\" 1>>\"$currentDir/fails.html\"";

  if ($cres != 0 or $res != 0 or !-r "$fname.html"){
    #print "failed: $cres, $res, ".(-r "$fname.html")."\n";
    $testFailed++;
  }else{
  }
  $testRuns++;
}

$timeEnd = time();

$perc = sprintf "%.0d", ($testRuns-$testFailed)*100/$testRuns;

$result = "Test time: ".($timeEnd-$timeStart)." sec, Executed: $testRuns, Passed: ".($testRuns-$testFailed).", Failed: $testFailed, Passed:".$perc."%\n";

print $result;

open FAILS, ">>$currentDir/fails.html";
print FAILS "</pre></pre></pre><h2>$result</h2>";
close FAILS;

exit;





sub collectDirs{
  my $cdir = shift @_;
  my $prefix = "";
  $prefix = "$cdir/" if ($cdir ne ".");

  opendir DIR, $cdir or die "Can't open dir $cdir";

  my @retlist;
  my @dirlist = readdir DIR;
  my @flist = grep {/^(?!CVS)[^_.]/ && -f "$cdir/$_" } @dirlist;
   @dirlist = grep {/^(?!CVS)[^_.]/ && -d "$cdir/$_" } @dirlist;

  closedir DIR;

  foreach(@flist){
    my $torun = 1;
    my $cname = $prefix.$_;

    if ($runMode eq "quick" and $cname =~ /\/full\//ix){
      $torun = 0;
    }
    if ($torun == 0 || $prefix eq ""){
      next;
    };
    push @retlist, $cname;
  };

  foreach(@dirlist){
    push @retlist, collectDirs($prefix.$_);
  };
  return @retlist;
}

sub checkDir{
  my $currentDir = shift @_;
  my $fname = shift @_;
  $fname =~ /^(.*?)\/[^\/]+$/;
  my $dir = $1;
  if (!-d "$currentDir/$dir"){
    my $dirs = "/";
    foreach(split /\//, $dir){
      $dirs .= $_;
      mkdir $currentDir.$dirs, 0777;
      $dirs .= "/";
    }
  }
}