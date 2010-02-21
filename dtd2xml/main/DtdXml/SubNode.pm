use strict;
package DtdXml::SubNode;

#nodes in element

our $VERSION = 0.01;
use base qw(DtdXml::Node::Element);
use Helper::Class;

our $tagname = '#in_tag';

sub new 
{
	my ($class, $data) = @_;
	return super($class, {type=>'SUBNODE', value=>$data});
}

sub setOccurs
{
	my ($this, $pre) = @_;
	my %ccvt=
	(
		'*' => 'nul-inf',
		'+' => 'one-inf',
		'?' => 'nul-one'
	);
	
	$this->{attribs}{occurs}=$ccvt{$pre} if defined $ccvt{$pre};
};


1;
package DtdXml::SubNode::Ref;

our $VERSION = 0.01;
use base qw(DtdXml::SubNode);
use Helper::Class;
use DtdXml::Names;
use DtdXml::Entities;

our $tagname = 'ref';

sub new
{
	my ($class, $data, $type) = @_;
	my $this = super($class, $data);
	
	#$this->setNameRef($data);
	#$this->{attribs}{type} = $type;
	if(my $ref = isPERef($data))
	{
		$this->{attribs}{name} = $ref;
		makeRole($ref, $type);
	}
	else
	{
		print STDERR "Warning: used $data as PERef in role $type\n";
	}
	$this->{attribs}{role} = $type;
	
	return $this;
}


1;
package DtdXml::SubNode::Unknow;

our $VERSION = 0.01;
use base qw(DtdXml::SubNode);
use Helper::Class;

our $tagname = 'unknow';

sub new
{
	my ($class, $data) = @_;
	my $this = super($class, $data);
	$this->{content} = $data;
	return $this;
}


1;
package DtdXml::SubNode::Name;

our $VERSION = 0.01;
use base qw(DtdXml::SubNode);
use Helper::Class;
use DtdXml::Names;

our $tagname = 'name';

sub new
{
	my ($class, $data, $type) = @_;
	my $this = super($class, $data);
	$data = lc $data if $type eq 'conduction' && $data !~ /$PERef/;
	
	$this->setNameRef($this->{name} = $data, $type);
	return $this;
}


1;
package DtdXml::SubNode::Fixed;

our $VERSION = 0.01;
use base qw(DtdXml::SubNode::Unknow);
use Helper::Class;
use DtdXml::Names;

our $tagname = 'fixed';

sub new
{
	my $this = super(@_);
	$this->{content} =~ s{($PERef)}
	{
		my $ref = new DtdXml::SubNode::Ref($1,'string');
	"$ref"}seg;
	return $this;
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
