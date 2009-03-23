#!/usr/bin/perl 
use strict;

my %files;
my $curd='';

foreach(`ls -R @ARGV`)
{
	chomp;
	next unless $_;
	
	if(m/^(.+?):/)
	{
		$curd=$1;
		$files{$curd} = 'd';
		next;
	}
	
	$files{"$curd/$_"} = 'f' foreach split/\s+/;
}

foreach(sort keys %files)
{
	next if $files{$_} eq 'd';
	print "$_\n";
}
