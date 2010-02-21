use strict;
package DtdXml::SubNode::Element;

our $VERSION = 0.01;
use DtdXml::MixParse::SubNode;
use base qw(DtdXml::SubNode DtdXml::MixParse::SubNode::Element);
use Helper::Class;
use Helper::Switch;
use DtdXml::Names;

our $tagname = '#in_element';


sub new
{
	my ($class, $data, $pre, @groups) = @_;
	my $this = super($class, $data);
	#print "new elemnet: ($class, $data, $pre, @groups)\n";
	
	$this->setParsed($this->mixParse($data, @groups));
	$this->setOccurs($pre);
	
	return $this;
}


1;
package DtdXml::SubNode::Element::Content;

our $VERSION = 0.01;
use DtdXml::MixParse::SubNode;
use base qw(DtdXml::SubNode DtdXml::MixParse::SubNode::Content);
use Helper::Class;
use DtdXml::Names;

our $tagname = 'content';


sub new
{
	my ($class, $data) = @_;
	my $this = super($class, $data);
	
	#$data =~ s/\s*(EMPTY|ANY|$PERef)//; #|($bracket)([*+?]?))\s*//;
	
	
	if($data =~ s/^\s*(EMPTY|ANY|$PERef)//)
	{
		my $type = $1;
		
		if(isPERef($type))
		{
			$this->{attribs}{type} = 'ref';
			$this->{attribs}{ref} = $this->setNameRef($type);
		}
		else
		{
			$this->{attribs}{type} = lc($type);
		}
	}
	else
	{
		$this->{attribs}{type} = ($data =~ s/#PCDATA//) ? 'pcdata' : 'strict';
		$this->setParsed($this->mixParse($data));
	}
	#print "WARNING content $data\n" if $data;
	return $this;
}


1;
package DtdXml::SubNode::Element::Seq;

our $VERSION = 0.01;
use base qw(DtdXml::SubNode::Element);
our $tagname = 'seq';


1;
package DtdXml::SubNode::Element::Choice;

our $VERSION = 0.01;
use base qw(DtdXml::SubNode::Element);
our $tagname = 'choice';


1;
package DtdXml::SubNode::Element::Item;

our $VERSION = 0.01;
use base qw(DtdXml::SubNode);
use Helper::Class;

our $tagname = 'item';

sub new
{
	my ($class, $data) = @_;
	my $this = super($class, $data);
	$data =~ s/\s*([+*?])\s*$//;
	$this->setOccurs($1);
	$this->setNameRef($data);
	return $this;
}


1;
__END__
	my @parsed = switch
	{
		when{isPERef($_)} sub { new DtdXml::SubNode::Ref($_, 'item') },
		when{m/$QName/} sub { new DtdXml::SubNode::Element::Item($_) },
		when{m/<\d+>/} sub 
		{
			my ($i) = (/<(\d+)>/);
			my ($bra, $ket) = @{$groups[$i]};
			#print "SUBGOUP $i: $groups[$i][0] - $groups[$i][1]\n";
			return $this->choice($bra, $ket);
		}
	} split /\s*[|,\s]\s*/, $data;
	#print "CONTENT Parse content: @parsed\n";

	
# global defines

my $bracket;
$bracket = qr{\((?:(?>[^()]+)|(??{$bracket}))*\)}s;

my $choice = sub
{
	my ($data, $pre) = @_;
	
	$data =~ s/^\((.+)\)/$1/s;
	
	my @groups;
	my $i=-1;
	$data =~ s{($bracket)([*+?]?)}
	{
		$i++;
		$groups[$i] = [$1, $2];
	"<$i>"}ges;
	
	#return new DtdXml::SubNode::PcData($data, $pre, @groups) if $data =~ /#PCDATA/;
	#$data =~ s/#PCDATA(\s*\|)?//; 
	
	return new DtdXml::SubNode::Element::Choice($data, $pre, @groups) if $data =~ /\|/;
	return new DtdXml::SubNode::Element::Seq($data, $pre, @groups); # if $data =~ /,/;
	#return new DtdXml::SubNode::Element::Seq($data, $pre) unless @groups;
	
	#print STDERR "WARNING: cannot parse $data\n";
};



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
