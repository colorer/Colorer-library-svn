#!/usr/bin/perl 
while(<>)
{
 push @pack,$1 if m{^<a href="http://search\.cpan\.org/search\?module=(\w[\w\:]*)">};
}
$,="\n";
print sort @pack;
