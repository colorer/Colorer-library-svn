use strict;
package Helper::Uri;
use Exporter;

our $VERSION = 0.03;
our @ISA = qw(Exporter);
our @EXPORT= qw(parse_uri parse_path normalize_path);


#Создает из строки список URI 
# поля:
# scheme    = протокол
# authority = сайт
# path      = путь
# query     = запрос
# fragment  = якорь
#

#  ^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?
#        12            3  4          5    6   7      8  9
#
#   scheme    = $2
#   authority = $4
#   path      = $5
#   query     = $7
#   fragment  = $9
#
# return (scheme, authority, path, query, fragment)

sub parse_uri($)
{
	$_[0] =~ m{^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?};
	return ($2, $4, $5, $7, $9);
}


sub parse_path($)
{
	my $path = shift;
	return ('.','') unless $path;
	return ($path, '') if $path =~ /^\.{1,2}$/;
	
	$path =~ m<((.*)/)?([^/]*)$>;
	my ($d, $f, $p) = ($2, $3, $1);
	
	$d = '.' unless $d || $p;
	return ($d, $f);
}


# normalize_path('./../foo') returns '.foo' -- fixed??

sub normalize_path($)
{
	my $base = shift;
	#print "normalis path $base\n";
	
	#my ($base, $name) = ($path =~ m<(.*/)?([^/]+)$>)
	$base=~s</+></>g;
	#print "Step 1 $base\n";
	
	$base=~s/^(\.\/)*((\.{2}\/)+)//gx;
	my $pre = $2;
	#print "Step 2 $base ($pre)\n";
	
	my $del = qr<([^/?#.]+/\.{2}|\.)>;
	1 while $base =~ s<$del/><>;
	#print "Step 3 $base\n";
	
	$base =~s</$del$><>;
	#print "Step 4 $base\n";
	
	$base = $pre.$base;
	
	#print "normalis res $base\n\n";
	return $base;
}



#for class
sub as_class($)
{
	ref($_[0]) ? $_[0] : new(__PACKAGE__, $_[0])
}


#class

use overload 
	'""' => "makeStr";


my %exclude_shemas = 
(
	mailto=>':',
	uri=>':',
	urn=>':',
	jar=>':',
	zip=>':',
	dbi=>':',
	odbc=>':',
	jdbc=>':'
);

my @urnames = qw(scheme authority path query fragment);


sub setBaseUri
{
	my ($this, $base) = @_;
	$this->setBase($base);
	$this->normUriPath;
}

sub makeStr
{
	my $this=shift;
	my ($uri, $au, $ph, $qr, $fr) = @{$this}{@urnames};
	
	$ph =~ s<^\./><>;
	
	$uri .= ($exclude_shemas{$uri}) ? ($exclude_shemas{$uri}) : ('://') if $uri;
	$uri .= $au; # . '/' if $au;
	$uri .= $ph;
	$uri .= '?' . $qr if $qr;
	$uri .= '#' . $fr if $fr;
	
	#print "\n-> Make URI: $uri\n";
	return $uri;
}

sub makeList
{
	my $this=shift;
	return @{$this}{@urnames};
}


# this is base ot that?
sub isBase
{
	my ($this, $that) = @_;
	$that = as_class($that);
	
	if($this->{type} == 1)
	{
		return undef unless $this->{scheme} eq $that->{scheme} || $this->{authority} eq $that->{authority};
	}
	#print "Ckeck: $that->{path} =~ /$this->{repath}/;\n";
	return $that->{path} =~ /$this->{repath}/;
}

sub getRelative
{
	my ($this, $that) = @_;
	return unless $this->isBase($that);
	$that = as_class($that);
	
	return new(ref($that), $that->{path}) unless $this->{basepath};
	
	my ($rel) = ($that->{path} =~ /$this->{repath}(.*)$/);
	return unless $rel;
	#$rel =~ s<^/><>;
	return new(ref($that), $rel);
}



sub new 
{
	#print "Create uri @_ ...\n";
	my ($class, $uri, $base) = @_;
	my $this;
	#print "\n-> New URI: ($class, $uri, $base)\n";
	
	if(ref($uri))
	{
		$this = { map{ $_, $uri->{$_} } @urnames };
	}
	else
	{
		my @up = parse_uri($uri);
		#print "parsed uri: '@up'\n";
		$this = { map{ $urnames[$_], $up[$_] }(0..$#up) };
	}
	bless $this, $class;
	
	$this->setType();
	$this->setBase($base) if $base;
	
	$this->normUriPath;
	
	#$this->{Path} = new Helper::Uri::Path($this->{path});
	
	#$this->setType();
	#$this->setBase($base) if $base;
	
	#print "* $_: '$this->{$_}'\n" foreach keys %$this;
	#print "Urinames is: @{$this}{@urnames}\n";
	#print "URI is ".$this->makeStr()."\n";
	
	return $this;
}




sub normUriPath
{
	my $this = shift;
	$this->{path} =  normalize_path($this->{path});
	my ($base, $name) = parse_path($this->{path});
	$this->{basepath} = $base;
	$this->{name} = $name;
	
	my $qpath = quotemeta($this->{basepath}.'/');
	$this->{repath} = qr/^$qpath/;
	#$this->{path} = $this->{basepath} . '/' . $this->{name};
}




#private

# 1 - full, 2 - absolute, 3 - realtive

sub setType
{
	my $this = shift;
	
	if($this->{scheme} =~ /^[A-Z]$/i && $this->{path} =~ /^\\/)
	{
		$this->{path} =~ tr[\\][/];
		$this->{path} = "/$this->{scheme}:".$this->{path};
		$this->{scheme} = 'file';
	}
	
	if($this->{authority})
	{
		$this->{scheme} = 'http' unless $this->{scheme};
		$this->{path} = '/' unless $this->{path};
	}
	
	$this->{type} = 
		($this->{scheme}) 
		? 1
		: ($this->{path} =~ m(^/))
		? 2 : 3;
}



sub setBase
{
	my ($this, $base) = @_;
	return if $this->{type} == 1;
	
	$base = new(ref($this), $base) unless ref($base);
	return if $this->{type} < $base->{type};
	
	my $type = $this->{type};
	
	$this->{scheme} = $base->{scheme};
	$this->{authority} = $base->{authority};
	$this->{type} = $base->{type};
	
	return if $type == 2;
	
	$this->{path} = $base->{basepath} . '/' . $this->{path}
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
