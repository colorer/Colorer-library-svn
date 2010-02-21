use strict;
package DtdXml::Parser::File;

our $VERSION = 0.01;

use base qw(Helper::Class);
use Helper::Class;
use Helper::Uri;
our %XmlInfo = 
(
	xmlns=>'http://colorer.sf.net/2010/dtdxml',
	prens=>'dtd',
	enc=>'UTF-8',
	fpi=>'-//Colorer//DTD2XML DTD scheme//EN'
);


sub getXmlInfo
{
	my $this = shift;
	#print "Get xml from file $this->{uri}: '$this->{defxmlinfo}'\n";
	return %{$this->{defxmlinfo}};
}

# hash:
# uri,
#

sub new
{
	my $this=object(@_);
	my $out = $this->{uri};
	#$out =~ s/\.dtd$//;
	#$out .= '-dtd.xml';
	#$this->{out} = $out;
	$this->{code} = '';
	#print "Construct $this FILE $this->{uri}\n";
	$this->{defxmlinfo} = {%XmlInfo};
	$this->{need_save} = 0;
	$this->setBaseUri;
	
	return $this;
}


sub needSave
{
	$_[0]->{need_save} = 1;
}


sub getOutName
{
	my $this = shift;
#	print "Get name from '$this->{uri}' '$this->{fsi}': '$this->{out}\n";
	
	unless($this->{out})
	{
		my $out = $this->{uri};
		$out =~ s/\.dtd$//;
		$out .= '-dtd.xml';
		$this->{out} = $out;
	}
	
	my $outuri = new Helper::Uri($this->{out}, $this->{baseuri}->{o});
	return "$outuri";
}

sub setOutName
{
	my ($this, $name) = @_;
#	print "Set name to '$this->{uri}' '$this->{fsi}'\n";
	$this->{out} = $name;
}

sub getInName
{
	my $this = shift;
	my $inuri = new Helper::Uri($this->{uri}, $this->{baseuri}->{i});
	
#print "Get in '$inuri', base  $this->{baseuri}->{i}\n";
	return "$inuri";
}


# virtual functions

sub setFpi {}
sub setRoot {}
#sub setFsp {}
#sub setUri {}


sub load
{
	my $this = shift;
	my $in = $this->getInName();
	
	print "Load CATALOG $in\n";
	open(CAT, '<', $in) or print STDERR "WARNING: cannot open $in: $!\n";
	$this->{code} = join('',<CAT>);
	close CAT;
}


use Helper::Switch;

use DtdXml::Parser;
use DtdXml::Entities;
use DtdXml::Parser::Catalog;


sub setBaseUri
{
	my $this = shift;
	my %base = getBase();
	
	$this->{baseuri} = { map{ $_, new Helper::Uri($base{$_}) } keys %base };
	#print "set base uri for $this: ",%{$this->{baseuri}},"\n";
}

sub getBaseUri
{
	my ($this, $i) = @_;
	return $this->{baseuri}->{$i};
}

sub run
{
	my $this = shift;
	my $code = $this->{code};
	return unless $code;
	
	$code =~ s/--.+?--//gs; #remove comments;
	$code =~ s/^\s*//gs;
	
	my @s1 = split/^\s*(?=[A-Z]+)/m, $code;
	my @code = map{/^([A-Z]+)\s+(.+)\s*$/s} split/^\s*(?=[A-Z]+)/m, $code;
	
	#local $"=':';
	#print  "code: #@s1#\n %", %code, '%' ;
	
	local $_;
	my @files = switch
	{
		my @args;
		my $i = -1;
		
		trase{$i++},
		when{/["']/ || !/\S/} sub{no_return},
		
		# nothing to do with:
		test 'NOTATION',
		test 'LINKTYPE',
		test 'SGMLDECL',
		test 'DTDDECL',
		where {no_return},
		
		trase
		{
			@args = grep{/[^"']/} ($code[$i+1] =~ m/(["'])(.+?)\1/g); 
			#print "$_: @args\n";
		},
		
		case 'DOCUMENT' => sub 
		{
		#	print "Set DOC $args[0]\n";
			$this->setFpi(@args);
			no_return;
		},
		case 'PUBLIC' => sub
		{
		#	print "Set PUBLIC @args\n";
			return addPubFile(0,@args)
			# new DtdXml::Parser::Doc(fpi=>$args[0], fsi=>$args[1])
		},
		case 'IMPORT' => sub
		{
		#	print "IMPORT @args\n";
			return addSysFile(1,@args)
			#new DtdXml::Parser::Doc(fsi=>$args[0], need_save=>1)
		},
		case 'SYSTEM' => sub
		{
			my $uri = shift @args;
		#	print "SYSTEM: make @args as $uri\n";
			my $file = addSysFile(0,@args);
			$file->addId(uri=>$uri);
			return $file
		},
		case 'OUTPUT' => sub
		{
			my $out = shift @args;
		#	print "OUT for @args is $out\n";
			return addOutName($out, @args)
		},
		test 'INCLUDE',
		case 'CATALOG' => sub
		{
		#	print "$_ @args\n";
			my $file = addSysFile(0,@args);
			DtdXml::Parser::parseFile($file);
			no_return;
		},
		case 'BASE' => sub
		{
		#	print "$_ @args\n";
			setBase(i=>$args[0]);
			no_return;
		},
		case 'OUTBASE' => sub
		{
		#	print "$_ @args\n";
			setBase(o=>$args[0]);
			no_return;
		},
		case 'ROLE' => sub
		{
		#	print "Set ROLE $args[1] to $args[0]\n";
			makeRole($args[0], $args[1]);
			no_return;
		},
		case 'DOCTYPE' => sub
		{
			my $root = shift @args;
			if(@args)
			{
			#	print "Set DOCTYPE $root for @args\n";
				my $file = addPubFile(1,@args);
				$file->setRoot($root);
				return $file;
			}
			else
			{
			#	print "Set DOCTYPE $root for current\n";
				$this->setRoot($root);
				no_return;
			}
		},
		#todo:
		
		# dxc directives:
		#test 'INCLUDE',
		#test 'IMPORT',
		test 'PARSE',
		test 'SYSBASE',
		#test 'OUTBASE',
		#test 'OUTPUT',
		#test 'ROLE',
		
		# catalog directives:
		#test 'BASE',
		#test 'CATALOG',
		test 'OVERRIDE',
		#test 'PUBLIC',
		#test 'SYSTEM',
		#test 'DOCUMENT',
		#test 'DOCTYPE',
		test 'DELEGATE',
		test 'ENTITY',
		where
		{
			print "Warning: $_ directive not supported\n";
			no_return;
		},
		
		default
		{
			print STDERR "Warning: unknow directive $_\n";
			no_return;
		}
		
	} @code;
	
	#print "Result files: @files\n";
	
	DtdXml::Parser::addFiles(@files);
}

sub parse
{
	#nothing to do with this....
}

sub save
{
	#nothing to do with this....
}

1;

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
