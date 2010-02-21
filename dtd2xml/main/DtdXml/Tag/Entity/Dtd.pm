use strict;
package DtdXml::Tag::Entity::Dtd;

our $VERSION = 0.01;
use base qw(DtdXml::Tag::Entity);
use Helper::Class;
use DtdXml::Tag::Entity;
use DtdXml::Entities qw(addEntity hasPrior forced is_forced);

our $tagname = 'entity';

sub new
{
	$_[1]->{content} =~ s/^%\s+//;
	my $this = super @_;
	$this->{role} = 'unknow';
	$this->{external} = $this->{attribs}{system} || $this->{attribs}{public};
	$this->{parsed} = [];
	addEntity($this);
	return $this;
}


use Helper::Switch;
use DtdXml::SubNode;
use DtdXml::Names;
use DtdXml::MixParse;
use DtdXml::MixParse::SubNode;

sub setRole
{
	my ($this, $role) = @_;
	return if $this->{role} eq $role;
	my $forced = is_forced;
	
	if($this->{role} ne 'unknow')
	{
		if($forced || hasPrior($role, $this->{role})) #($this->{role} eq 'attrib' || $this->{role} eq 'name') && $role eq 'attlist')
		{
			#upgrade attrib to attlist
			$this->{parsed} = [];
			forced(1);
		}
		else
		{
			print STDERR "Warning: redfine role for $this->{name}: $this->{role} -> $role\n";
		}
	}
	$this->{role} = $this->{attribs}{role} = $role;
	$this->parseForRole();
	forced($forced);
}


sub parseVirtual
{
	my ($this, $parser) = @_;
	return $parser->mixParse($this->{content});
}



sub parseForRole
{
	my $this = shift;
	
	my @parsed = switch
	{
		case 'attlist' => sub
		{
			$this->parseVirtual(new DtdXml::MixParse::AttList);
		},
		case 'attrib' => sub
		{
			$this->parseVirtual(new DtdXml::MixParse::Attrib);
		},
		case 'element' => sub
		{
			$this->parseVirtual(new DtdXml::MixParse::Element);
			#$this->DtdXml::MixParse::Element::mixParse($this->{content})
		},
		case 'item' => sub
		{
			$this->parseVirtual(new DtdXml::MixParse::SubNode);
			#$this->DtdXml::MixParse::SubNode::mixParse($this->{content})
		},
		
		case 'string' => sub
		{
			new DtdXml::SubNode::Fixed($this->{content})
		},
		
		test 'conduction',
		test 'name',
		test 'prefix',
		where
		{
			new DtdXml::SubNode::Name($this->{content}, $_)
		},
		
		test 'include',
		test 'unknow',
		where
		{
			no_return;
		},
		
		default 
		{
			die "Unknow role $_\n";
		}
	} $this->{role};
	
	$this->setParsed(@parsed);
	#push @{$this->{parsed}}, @parsed;
	#$this->{is_parsed} = scalar @{$this->{parsed}};
}



sub toStr
{
	my $this = shift;
	
	if($this->{role} eq 'unknow')
	{
		print STDERR "Warning: using entity $this->{name} in unknow role, content '$this->{content}'\n";
		$this->setParsed(new DtdXml::SubNode::Unknow($this->{content})) if $this->{content};
	}
	
	$this->SUPER::toStr;
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
