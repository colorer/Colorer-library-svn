
@excludes = qw(
 registry.cpp
 colorer-doc.cpp
 compress.c
 deflate.c
 example.c
 gzio.c
 maketree.c
 minigzip.c
 trees.c
 iowin32.c
 minizip.c
 zip.c
);

open F, ">makefile.lines";

open OB, ">makefile.objs";

print OB "coreobjects = \\\n";

traverse('.');

print F "\n\n\n\n";

traverse('../zlib');

print OB "\n\n";

close OB;
close F;


sub traverse
{
    my $dir = shift;
    my @list = `ls $dir`;
    foreach $entry (@list){
        chomp $entry;
        traverse($dir.'/'.$entry) if (-d $dir.'/'.$entry);
        next unless ($entry =~ /^(.*)\.(c|cpp)$/);
        next if grep(/$entry/, @excludes);
        if (-f $dir."/".$entry)
        {
           
           print OB <<X;
  \$(obj_output_slash)$1.\$(obj) \\
X

           print F <<X;
\$(obj_output_slash)$1.\$(obj):
	\$(CPP) \$(CPPFLAGS) \$(shared_path)/$dir/$entry

X
        
        }
    }

}

