use strict;
package DtdXml::Parser::Catalog;

our $VERSION = 0.01;
our @ISA = qw(Exporter);
our @EXPORT= qw(addPubFile addSysFile addOutName setBase setBaseRel getBase);

#use DtdXml::Parser::File;
#use DtdXml::Parser::Doc;

our %FPI;
our %FSI;
our %URI;

our %Base = qw(i . f . o .);

sub setBase
{
	#print "set base @_\n";
	$Base{$_[0]} = $_[1];
}

sub setBaseRel
{
	my ($i, $p) = @_;
	my $u = new Helper::Uri($p, $Base{$i});
	#print "set relative base $u\n";
	$Base{$i} = "$u";
}


sub getBase
{
	#print "get base @_\n";
	return $Base{$_[0]} if $_[0];
	return %Base;
}


sub addDocObj
{
	my $file = shift;
	
	$FPI{$file->{fpi}} = $file if $file->{fpi};
	
	if(!$file->{fsi})
	{
		$file->{fsi}=$file->{uri} if $file->{uri};
	}
	if(!$file->{uri})
	{
		$file->{uri}=$file->{fsi} if $file->{fsi};
	}
	
	$FSI{$file->{fsi}} = $file if $file->{fsi};
	$URI{$file->{uri}} = $file if $file->{uri};
}

sub remDocObj
{
	my $file = shift;
	
	undef $FPI{$file->{fpi}} if $file->{fpi};
	undef $FSI{$file->{fsi}} if $file->{fsi};
	undef $URI{$file->{uri}} if $file->{uri};
}



sub makeFile($)
{
	local $_ = shift;
#	print "make $_\n";
	
	my $file = (/\.(cat|soc|dxc)$/) 
		? new DtdXml::Parser::File(uri=>$_) 
		: (/\.prop(erties)?$/)
		? new DtdXml::Parser::Props(uri=>$_)
		: new DtdXml::Parser::Doc(uri=>$_);
		
	return $file;
}



#public static

sub getFpi($)
{
	my $name = shift;
#	print "Get file FPI $name\n";
	return $FPI{$name} if $FPI{$name};
	return new DtdXml::Parser::Doc(fpi=>$name);
}

sub getFsi($)
{
	my $name = shift;
#	print "Get file FSI $name\n";
	return $FSI{$name} if $FSI{$name};
	return makeFile $name;
}

sub getUri($)
{
	my $name = shift;
#	print "Get file URI $name\n";
	return $URI{$name} if $URI{$name};
	return makeFile $name;
}

sub getDefName
{
	local $_;
	foreach(@_)
	{
		return $FPI{$_} if $FPI{$_};
		return $FSI{$_} if $FSI{$_};
		return $URI{$_} if $URI{$_};
	}
	return undef;
}

# needSave, uri
sub addSysFile
{
	my ($ns, $uri) = @_;
#	print "addSys $ns, $uri\n";
	my $file = getFsi($uri);
	$file->needSave($ns) if $ns;
	return $file;
}


# needSave, pubid, sysid, 


sub addPubFile
{
	my $ns = shift;
	my $file = getDefName @_;
	$file = getFpi $_[0] unless $file;
	$file->addId(fsi => $_[1]) if $_[1];
	$file->needSave($ns) if $ns;
	return $file;
}


sub addOutName
{
	my $out = shift;
	my $file = getDefName @_;
	$file = getFsi $_[0] unless $file;
	$file->setOutName($out);
	return $file;
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
