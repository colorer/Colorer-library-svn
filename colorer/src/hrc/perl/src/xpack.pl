#!/usr/bin/perl
use strict;

open XMLIN, shift or die $!;

my %packlst;
my %packln;

while(<XMLIN>)
{
 chomp;
 my $name=(split /::/)[0];
 $packlst{$name}{$_}++;
 $packln{$name}++;
}

print<<P1;
<packages>
 <separate>
P1
foreach(sort keys %packln)
{
 next if($packln{$_} != 1);
 print "  <pack name='$_'/>\n";
}
print<<P2;
 </separate>
 <groups>
P2
foreach(sort keys %packlst)
{
 next if($packln{$_} == 1);
 print "  <group name='$_'>\n";
 foreach(sort keys %{ $packlst{$_} })
 {
  print "   <pack name='$_'/>\n";
 }
 print "  </group>\n";
}
print<<P3;
 </groups>
</packages>
P3

