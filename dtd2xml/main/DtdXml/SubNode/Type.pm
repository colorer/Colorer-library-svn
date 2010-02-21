use strict;
package DtdXml::SubNode::Type;

#nodes in attlist

our $VERSION = 0.01;
use base qw(DtdXml::SubNode);
use Helper::Class;
use Helper::Switch;
use DtdXml::Names;
use DtdXml::SubNode::Element;

our $tagname = 'type';

sub new 
{
	my ($class, $data, @groups) = @_;
	my $this = super($class, $data);
	#$this->{content} = "$data <!-- @groups -->"; 
	
	$this->{unparsed} = [split /\s+/, $data];
	$this->setParsed($this->parse(@groups));
	
	return $this;
}


sub parse
{
	my ($this, @groups) = @_;
	
	#my @content = split /\s+/, $data;
	#$this->setNameRef($this->{name} = shift @{$this->{unparsed}});
	
	return switch
	{
		when{/<<\d+>>/} sub 
		{
			my $choice = shift @groups;
			#$this->{attribs}{default} = shift @groups;
			new DtdXml::SubNode::Fixed($choice)
		},
		when{/<\d+>/} sub
		{
			my $choice = shift @groups;
			#print "Choice: $_ $choice\n";
			new DtdXml::SubNode::Element::Choice($choice)
		},
		when{isPERef($_)} sub 
		{ 
			new DtdXml::SubNode::Ref($_, 'attrib') 
		},
		when{/ \b(CDATA | ID(REFS?)? | ENTIT(Y|IES) | NMTOKENS? | NOTATION)\b /x} sub
		{
			$this->{attribs}{type} = lc;
			no_return
		},
		when{/#(REQUIRED|IMPLIED|FIXED)\b/} sub
		{
			#print "SET mode $_ for @{$this->{unparsed}}\n";
			s/#//;
			$this->{attribs}{mode} = lc;
			no_return
		}
		
	} @{$this->{unparsed}};
}


1;
package DtdXml::SubNode::Type::Attribute;

our $VERSION = 0.01;
use base qw(DtdXml::SubNode::Type);

our $tagname = 'attrib';

sub parse
{
	my $this = shift;
	$this->setNameRef($this->{name} = shift @{$this->{unparsed}});
	
	return $this->SUPER::parse(@_);
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
