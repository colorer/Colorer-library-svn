use strict;

# global defines
my $bracket;
$bracket = qr{\((?:(?>[^()]+)|(??{$bracket}))*\)}s;


package DtdXml::MixParse::SubNode;

our $VERSION = 0.01;
use base qw(DtdXml::MixParse);
#use DtdXml::Names;
use DtdXml::SubNode::Element;


sub choice
{
	my ($this, $data, $pre) = @_;
	
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

sub mixParse
{
	my $this = shift;
	my $content = shift;
	
	#print "mixParse $this->{name}: $content\n";
	
	return () unless $content;
	
	if($content =~ /^\s*($bracket)([*+?]?)\s*$/)
	{
		return $this->choice($1, $2);
	}
	else
	{
		return $this->choice("($content)",'');
	}
}


1;
package DtdXml::MixParse::SubNode::Element;

our $VERSION = 0.01;
use base qw(DtdXml::MixParse::SubNode);
use DtdXml::Names;
use Helper::Switch;
use DtdXml::SubNode::Element;


sub mixParse
{
	#my $this = shift;
	#my $content = shift;
	#my @groups = @_;
	my ($this, $content, @groups) = @_;
	
	#print "Element mixparse $this $content -> @groups\n";
	
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
		},
		when{m/$Nmtoken/} sub { new DtdXml::SubNode::Element::Item($_) }
	} split /\s*[|,\s]\s*/, $content;
	#print "CONTENT Parse content: @parsed\n";
	
	return @parsed;
}


1;
package DtdXml::MixParse::SubNode::Content;

our $VERSION = 0.01;
use base qw(DtdXml::MixParse::SubNode);


sub mixParse
{
	my $this = shift;
	my $data = shift;
	
	return () unless $data =~ /[^\(\|\)*+?\s]/;
	
	$data =~ s/^\s*($bracket)([*+?]?)\s*//;
	my ($bra, $ket) = ($1, $2);
	print STDERR "WARNING content $data\n" if $data;
	
	return $this->choice($bra, $ket);
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
