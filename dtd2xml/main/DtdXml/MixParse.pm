use strict;
package DtdXml::MixParse;

our $VERSION = 0.01;
use base qw(Helper::Class);
use Helper::Class;

sub mixParse
{
	print STDERR "Call pure virtaul mixParce...\n";
	return ();
}

1;
package DtdXml::MixParse::Element;

our $VERSION = 0.01;
use base qw(DtdXml::MixParse);
use DtdXml::Names;
use DtdXml::SubNode::Element;


sub mixParse
{
	my $this = shift;
	my $content = shift;
	
	return 
		(
		isPERef($content) 
			? new DtdXml::SubNode::Ref($content, 'element') 
			: new DtdXml::SubNode::Element::Content($content)
		);
}


1;
package DtdXml::MixParse::Attrib;

our $VERSION = 0.01;
use base qw(DtdXml::MixParse);
use Helper::Class;
use Helper::Switch;
use DtdXml::Names;
use DtdXml::SubNode;
use DtdXml::SubNode::Type;

our $maker_role = 'attrib';

sub make_node
{
	my $this = shift;
	return new DtdXml::SubNode::Type(@_);
}


sub prepareContent
{
	my $this = shift;
	my $content = shift;
	
	my $i=-1;
	my @groups = ();
	
	$$content =~ s{\s*(["'])(.*?)\1}
	{
		$i++;
		$groups[$i] = $2;
	" <<$i>>"}seg;
	
	$$content =~ s{\s*\((.*?)\)\s*}
	{
		$i++;
		$groups[$i] = $1;
	" <$i> "}seg;
	
	$$content =~ s/\s+#/ #/gs;
	
	return @groups;
}


sub mixParse
{
	my $this = shift;
	my $content = shift;
	#my $i=-1;
	my @groups = $this->prepareContent(\$content);
	
	#print "$this->{name}: package ".ref($this)." maker_role: $role \n";
	
	return $this->mixParseContent($content, @groups) 
}


# todo: must be complete rewriten for DtdXml::MixParse::AttList for support single striongs

sub mixParseContent
{
	my $this = shift;
	my $content = shift;
	my @groups = @_;
	
	my $role = ${$this->static('maker_role')};
	
	return switch
	{
		when{ isPERef($_) } sub{ new DtdXml::SubNode::Ref($_, $role) },
		default 
		{ 
			no_return unless $_;
			
			my @ii = (m/<(\d+)>/g);
			#new DtdXml::SubNode::Type::Attribute($_, @groups[@ii]) 
			$this->make_node($_, @groups[@ii])
		}
	} split /\s*\n\s*/s, $content;
	
}


1;
package DtdXml::MixParse::AttList;

our $VERSION = 0.01;
use base qw(DtdXml::MixParse::Attrib);
use DtdXml::Names;
#use DtdXml::SubNode::Type;
#use Helper::Class;


our $maker_role = 'attlist';

sub mixParse
{
	my $this = shift;
	my $content = shift;
	#my $i=-1;
	my @groups = $this->prepareContent(\$content);
	
	my @tokens = split /\s+/s, $content;
	
	return $this->mixParseContent($content, @groups) if grep{!isPERef($_)} @tokens;
	return map{ new DtdXml::SubNode::Ref($_, $maker_role) } @tokens;
}


sub make_node
{
	my $this = shift;
	return new DtdXml::SubNode::Type::Attribute(@_);
}


1;
__END__

#todo work: valid AttList::mixParse...

sub mixParse
{
	my $this = shift;
	my $content = shift;
	
	#my $i=-1;
	my @groups = $this->prepareContent(\$content);
	
	my $role = ${$this->static('maker_role')};
	
	
	# [name] [type] [(<i>)] [#mode] ['<<i>>']
	
	my @tokens = split /\s+/s, $content;
	
	my @names = switch
	{
		when{/<<\d+>>/} sub{'string'},
		when{/<\d+>/} sub{'code'},
		when{isPERef($_)} sub{'ref'},
		when{/ \b(CDATA | ID(REFS?)? | ENTIT(Y|IES) | NMTOKENS? | NOTATION)\b /x}sub{'type'},
		when{/#(REQUIRED|IMPLIED|FIXED)\b/} sub{'mode'},
		default{'name'}
	} @tokens;
	
	for(0..$#names)
	{
		
	}
	
	#$this->setParsed(@parsed);
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
