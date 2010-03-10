#!/usr/bin/perl

use strict;
use utf8;

use FindBin;
use lib $FindBin::Bin;

use DtdXml::Parser;
use DtdXml::Props;


my $version = "DTD2XML version 0.1.2 α ";
my $author = "Written © Eugene Efremov, 2009-2010";
my $vb = 0;


# comand line

while($ARGV[0] =~ /^-/)
{
	my $arg = shift;
	if($arg =~ /^-\w/)
	{
		my %keys = (h=>'help',p=>'prop',v=>'verbose');
		$arg =~ s/^-(\w)/--$keys{$1}/;
	}
	
	if($arg eq '--help')
	{
		# todo `chcp 65001` crash some consoles...
		`chcp 65001` if defined $ENV{COMSPEC}; # widnows --> utf8
		binmode(STDOUT,':raw:utf8');
		print STDOUT "$version \n$author\n\nUse: \n\tdtd2xml [options] [files]\n\n";
		print STDOUT "Options:\n\t--prop:name=value (-p) — set DXC property\n\t--verbose (-v) — print extra information\n\t--help (-h) — print this help\n\n";
		exit;
	}
	if($arg =~ /^--prop:/)
	{
		my ($k, $v) = ($arg =~ m/--prop:($PropName)=(.+)/);
		die "Invalid property name in '$arg'." unless($k);
		
		setProperty($k, $v);
		next;
	}
	if($arg eq '--verbose')
	{
		$vb=1;
		next;
	}
	
	print "Unknow param: $arg";
}


# output control

if($vb)
{
	$|=1;
}
else
{
	open(STDOUT,">/dev/null") or open(STDOUT,">nul") or die "Can't disable stdout!";
}
 

# parse

my $comm = "<!--\n\tThis file autogenerate by\n\t$version\n\t$author\n-->\n";

setComment($comm);
addFileNames(@ARGV);
parseAll;

# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is DTD2XML.
#
# The Initial Developer of the Original Code is
# Eugene Efremov <4mirror@mail.ru>
# Portions created by the Initial Developer are Copyright (C) 2009-2010
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the LGPL or the GPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK ***** 
