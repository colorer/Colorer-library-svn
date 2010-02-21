use strict;
package DtdXml::Node;

our $VERSION = 0.01;
use base qw(Helper::Class);
use Helper::Class;

use overload 
#	'""' => sub{ $_[0]->toStr(@_) };
	'""' => "toStr";


our $tagname = '#unknow';

sub new
{
	my ($class, $data) = @_;
	return object $class, %$data;
}


sub toStr
{
	my $this = shift;
	my $name = $this->make_tag();
	#print "DtdXml::Node::toStr $name\n";
	
	return "\n<!--"
		. "\n===================================================\n"
		. "======= ".$this->{type}." ".$this->{name}
		. "\n===================================================\n-->\n"
		. $this->{value};
	
}

sub make_tag
{
	my $this = shift;
	my %info = DtdXml::Parser::getXmlInfo();
	my $tn = $this->static('tagname');
	return "$info{prens}:$$tn";
}

1;


package DtdXml::Node::PI;

our $VERSION = 0.01;
use base qw(DtdXml::Node);


our $tagname = '#pi';

sub toStr
{
	my $this = shift;
	#print "DtdXml::Node::PI::toStr\n";
	return "<!-- \nNOTE: PI named '$this->{name}' was here.\n\n?$this->{name} $this->{content}?\n-->";
}

1;


package DtdXml::Node::Conduct;

our $VERSION = 0.01;
use base qw(DtdXml::Node);
use Helper::Class;
use DtdXml::Names;
use DtdXml::Entities;

our $tagname = 'conduction';

sub new 
{
	#my $this = $ISA[0]->new(@_);
	my $this = super @_;
	
	$this->{content} =~ s/^(INCLUDE|IGNORE|$PERef)\s*//;
	my $type = $1;
	$this->{name} = $type;
	if(my $ref = isPERef($type))
	{
		$this->{ref} = $ref;
		makeRole($ref, 'conduction');
	}
	else
	{
		$this->{test} = lc($type);
	}
	return $this;
}


sub toStr
{
	my $this = shift;
	my $name = $this->make_tag();
	my $attr=' ';
	
	#print "DtdXml::Node::Conduct\n";
	#print "WARNING content $this->{content}\n" if $this->{content};
	$attr .= "test='$this->{test}'" if $this->{test};
	$attr .= "ref='$this->{ref}'"   if $this->{ref};
	
	return "\n<$name$attr>";
}


1;
package DtdXml::Node::Element;

our $VERSION = 0.01;
use base qw(DtdXml::Node);
use Helper::Class;
use DtdXml::Names;
use DtdXml::Entities;

our $tagname = '#tag';


sub new
{
	my $this = super @_;
	$this->{is_parsed} = 0;
	return $this;
}


sub toStr
{
	my $this = shift;
	my $name = $this->make_tag();
	my $att = '';
	
	foreach my $k (keys %{$this->{attribs}})
	{
		my $v = $this->{attribs}{$k};
		my $s = ($v=~/"/)?"'":'"';
		$att.=" $k=$s$v$s";
	}
	
	#print STDERR "Warning: $this->{type} is external: $this->{external}\n" if $this->{external};
	
	my $q = $";
	local $" = $"."\t";
	
	my $content = $this->{is_parsed} 
		? $"."@{$this->{parsed}}".$q 
		: $this->{content};
	#	? "<$main::preun:unknow>\n$this->{content}$q</$main::preun:unknow>" 
	#	: '';
	
	my $cltg = $this->{is_open} ? '>' : '/>';
	return "<$name$att" . ($content ? ">$content</$name>" : $cltg);
}


sub setParsed
{
	my $this = shift;
	$this->{parsed} = [ ] unless defined $this->{parsed};
	
	push @{$this->{parsed}}, @_;
	$this->{is_parsed} = scalar @{$this->{parsed}};
}



sub setNameRef
{
	my ($this, $name, $type) = @_;
	#print "Set name $name for $this->{type}\n";
	
	if(my $ref = isPERef($name))
	{
		$this->{attribs}{ref} = $ref;
		my $role = $type ? $type : 'name' ;
		#print "make role '$role' for $ref\n" ;
		makeRole($ref, $role);
	}
	elsif($name =~ /^\s*(($NCName):)?($NCName)\s*$/)
	{
		$this->{attribs}{name} = $3;
		$this->{attribs}{pre} = $2 if $2;
	}
	elsif($name =~ /^\s*($PERef):?($NCName)\s*$/)
	{
		$this->{attribs}{name} = $2;
		#$this->{attribs}{pre} = "#ref";
		my $r = isPERef($1);
		$this->{attribs}{'pre-ref'} = $r;
		makeRole($r, 'prefix');
	}
	elsif($name =~ /^\s*($NCName):?($PERef)\s*$/)
	{
		$this->{attribs}{pre} = $1;
		my $r = isPERef($2);
		$this->{attribs}{ref} = $r;
		makeRole($r, 'prefix');
	}
	elsif($name =~ /^\s*(($NCName):|:($NCName))\s*$/)
	{
		$this->{attribs}{pre} = $2 || $3;
	}
	elsif($name =~ /^\s*(($PERef):|:($PERef))\s*$/)
	{
		my $r = isPERef($2 || $3);
		$this->{attribs}{'pre-ref'} = $r;
		makeRole($r, 'prefix');
	}
	elsif($name =~ /^\s*($Nmtoken)\s*$/)
	{
		$this->{attribs}{name} = $1;
		# print STDERR "Warning cannot set name $name for $this->{type}\n";
	}
}

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
