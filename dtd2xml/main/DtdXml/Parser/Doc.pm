use strict;
package DtdXml::Parser::Doc;

our $VERSION = 0.01;

#use DtdXml::Parser::File;
use base qw(DtdXml::Parser::File DtdXml::Parser::Catalog);
use Helper::Class;
use DtdXml::Names;



# hash:
# uri, fpi, fsi
#

sub new
{
	my $this=super @_;
	#print "Construct $this DOC $this->{uri}\n";
	$this->{is_parsed} = 0;
	$this->{raw} = '';
	$this->{root} = '';
	$this->addDocObj();
	return $this;
}


sub addId
{
	my $this=shift;
	my %par = @_;
	$this->remDocObj();
	$this->{$_} = $par{$_} foreach keys %par;
	$this->addDocObj();
#	print "make Id @_: '$this->{fpi}' '$this->{fsi}' '$this->{uri}'\n";
}

sub makeAttr
{
	my $this=shift;
	
	my $attr='';
	$attr .= "\n\tpublic=\"$this->{fpi}\"" if $this->{fpi};
	$attr .= "\n\tsystem=\"$this->{fsi}\"" if $this->{fsi};
	$attr .= "\n\troot=\"$this->{root}\""  if $this->{root};
	
	return $attr;
}

sub setRoot
{
	my ($this, $root) = @_;
	$root =~ s/^$NCName://;
	$this->{root} = $root;
}


# virtual functions

sub setFpi 
{
	my ($this, $fpi) = @_;
	$this->addId(fpi=>$fpi);
}



sub load
{
	my $this = shift;
	my $in = $this->getInName();
	
	
	#print "Load DTD $in\n";
	local $/ = '>';
	
	open(DTD,'<:utf8', $in) or print STDERR "WARNING: cannot open $in: $!\n";
	
	local $_;
	while(<DTD>)
	{
		next unless $_;
		
		if(/<\?(xml|CATALOG)\b/)
		{
			my $pre = $1;
			if($pre eq 'xml')
			{
				binmode(DTD, ":encoding($2)") if /encoding\s*=\s*(["'])(.+?)\1/;
			}
			if($pre eq 'CATALOG')
			{
				#print "CATALOG detect\n";
				my ($data) = (/<\?CATALOG\s+(.+?)\s*\?>/s);
				$this->{code} .= $data;
			}
			s/<\?.+?\?>//s;
		}
		$this->{raw} .= $_ if /\S/;
		#print "files load S @DtdXml::Parser::Files: '$_'\n";
	}
	
	close DTD;
}


sub save
{
	my $this = shift;
	#print "call save $this\n";
	return unless $this->{is_parsed};
	return unless $this->{need_save};
	
	my $out = $this->getOutName();
	
	#print "Save file $this->{uri} $this->{fpi}\n";
	my %info    = $this->getXmlInfo();
	my $comment = DtdXml::Parser::getComment();
	
	print "Save XML $out\n";
	
	my $enc = ($info{enc} eq 'UTF-8') ? 'utf8' : "encoding($info{enc})";
	open(XML, ">:$enc", $out) or print STDERR "WARNING: cannot save $out: $!\n";
	
	my $attr = $this->makeAttr();
	
	print XML "<?xml version='1.0' encoding='$info{enc}'?>\n";
	print XML "<$info{prens}:doctype$attr\n\txmlns:$info{prens}='$info{xmlns}'\n >\n";
	print XML $comment if $comment;
	
	local $" = "\n";
	print XML "\n@{$this->{parsed}}\n";
	print XML "</$info{prens}:doctype>\n";
	
	close XML;
}




use Helper::Switch;
use DtdXml::Names;

use DtdXml::Node;
use DtdXml::Node::Ref;
use DtdXml::Tag;
use DtdXml::Tag::Entity;
use DtdXml::Tag::Entity::Dtd;



#old data


sub parse
{
	my $this=shift;
	return if $this->{is_pasred};
	
	my @dtd = $this->txt2lst();
	
	local $_;
	my @xml = switch
	{
		my $cond_tag = 'none';
		
		when {$_->{type} eq 'COMMENT'} sub 
		{
			#find_doctype $_->{value} unless $DtdDef;
			no_return if $_->{value} =~ /<!--\s*-->/s;
			return $_->{value}
		},
		when {$_->{type} eq 'PI'}      sub { new DtdXml::Node::PI $_ },
		when {$_->{type} eq 'ENT'}     sub { new DtdXml::Node::Ref $_ },
		
		when {$_->{type} eq 'TAG' && $_->{name} eq 'ELEMENT' } sub { new DtdXml::Tag::Element $_ },
		when {$_->{type} eq 'TAG' && $_->{name} eq 'ATTLIST' } sub { new DtdXml::Tag::AttList $_ },
		when {$_->{type} eq 'TAG' && $_->{name} eq 'NOTATION'} sub { new DtdXml::Tag::Entity::Notation $_ },
		when {$_->{type} eq 'TAG' && $_->{name} eq 'ENTITY' && $_->{content} =~ /^%\s+/ } sub { new DtdXml::Tag::Entity::Dtd $_ },
		when {$_->{type} eq 'TAG' && $_->{name} eq 'ENTITY'}   sub { new DtdXml::Tag::Entity::Xml $_ },
		
		when {$_->{type} eq 'TAG'}        sub { new DtdXml::Tag $_ },
		
		when {$_->{type} eq 'COND_START'} sub 
		{ 
			my $ret = new DtdXml::Node::Conduct $_;
			$cond_tag = $ret->make_tag();
			return $ret;
		},
		when {$_->{type} eq 'DOCTYPE'} sub 
		{ 
			my $ret = new DtdXml::Tag::Entity::Document $_;
			$cond_tag = $ret->make_tag();
			return $ret;
		},
		when {$_->{type} eq 'COND_END'}   sub { "</$cond_tag>\n" },
		when {$_->{type} eq 'UNKNOW'}     sub 
		{
			print "\n================== E R R O R ======================\n";
			print $_[0]->{value};
			print "\n================== E R R O R ======================\n";
			
			die "INVALID VALUE $_[0]->{value}";
		},
		default{ new DtdXml::Node $_}
	} @dtd;
	
	
	$this->{is_parsed} = 1;
	$this->{parsed} = \@xml;
}





#private

sub txt2lst
{
	my $this = shift;
	
	my $text = $this->{raw};
	
	my @dtd = ();
	my $i = 0;
	my $in_doctype = 0;
	
	while($text =~ /\S/)
	{
		if($text =~ s(^\s*(<!--.+?-->))()s)
		{
			$dtd[$i]=
			{
				type => 'COMMENT',
				value => $1
			};
		}
		elsif($text =~ s(^\s*(<\?($QName)\s+(.+?)\?>))()s)
		{
			$dtd[$i]=
			{
				type => 'PI',
				value => $1,
				name => $2,
				content => $3
			};
		}
		elsif($text =~ s(^\s*<!(DOCTYPE\s+(.+?))(\[|>))()s)
		{
			$in_doctype = $3 ne '>';
			$dtd[$i]=
			{
				type => 'DOCTYPE',
				value => $1,
				name => 'DOCTYPE',
				content => $2,
				is_open => $in_doctype
			};
		}
		elsif($in_doctype && $text =~ s(^\s*(\]\s*>))()s)
		{
			$dtd[$i]=
			{
				type => 'COND_END',
				value => $1
			};
			$in_doctype = 0;
		}
		elsif($text =~ s(^\s*(<!\[\s*(.+?)\[))()s)
		{
			$dtd[$i]=
			{
				type => 'COND_START',
				value => $1,
				content => $2
			};
		}
		elsif($text =~ s(^\s*(\]\]>))()s)
		{
			$dtd[$i]=
			{
				type => 'COND_END',
				value => $1
			};
		}
		elsif($text =~ s(^\s*(<!(\w+)\s+(.+?)>))()s)
		{
			$dtd[$i]=
			{
				type => 'TAG',
				value => $1,
				name => $2,
				content => $3
			};
		}
		elsif($text =~ s(^\s*(%($QName);))()s)
		{
			$dtd[$i]=
			{
				type => 'ENT',
				value => $1,
				name => $2
			};
		}
		elsif($text =~ m(^\s*(\S+))s)
		{
			print "Detect no-DTD content: '$1'.\nStop parsing\n";
			last;
		}
		$dtd[$i]->{ovner} = $this;
		$i++;
	}
	print "Parsed $i items of ".$this->getInName()."\n";
	return @dtd;
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
