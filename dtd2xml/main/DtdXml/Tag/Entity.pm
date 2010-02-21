use strict;
package DtdXml::Tag::Entity;

our $VERSION = 0.01;
use base qw(DtdXml::Tag);
use Helper::Class;
use DtdXml::Names;

our $tagname = '#entity';


sub new
{
	my $this=super @_;
	return $this;
}


sub parse
{
	my $this = shift;
	if($this->{content} =~ /^(PUBLIC|SYSTEM)/)
	{
		$this->parse_href;
	}
	else
	{
		$this->{content} =~ s/^\s*(["'])(.*)\1\s*$/$2/s;
	}
}


sub parse_href
{
	my $this = shift;
	my ($type, $pub, $sys) = ($this->{content} =~ /^(PUBLIC|SYSTEM)\s+(["'])(.+?)\2(\s+(["'])(.+?)\5)?/)[0,2,5];
	#local $" = ':';
	#print "Entity '$this->{content}' detect: |@lst| $type, $pub, $sys\n";
	
	#my %attr;
	if($type eq "PUBLIC")
	{
		$this->{attribs}{public} = $pub;
		$this->{attribs}{system} = $sys if $sys;
	}
	if($type eq "SYSTEM")
	{
		$this->{attribs}{system} = $pub;
	}
	if($this->{content} =~ /NDATA\s+($Name)/)
	{
		$this->{attribs}{ndata} = $1;
	}
	#$this->{attribs} = {%attr};
	$this->{content} = '';
	
	$this->reg_file unless exists $this->{attribs}{ndata};
}

use DtdXml::Parser::Catalog;

sub reg_file
{
	my $this = shift;
	
	my $file;
	
	if(exists $this->{attribs}{public})
	{
		#print "Entity: Make file from $this->{attribs}{public} $this->{attribs}{system}\n";
		$file = addPubFile(1, $this->{attribs}{public}, $this->{attribs}{system});
	}
	elsif(exists $this->{attribs}{system})
	{
		$file = addSysFile(1, $this->{attribs}{system});
	}
	return unless $file;
	
	$this->{outfile} = $file;
	#print "Entity: Add file: $file\n";
	DtdXml::Parser::addFile($file);
}

sub toStr
{
	my $this = shift;
	if(exists $this->{outfile})
	{
		my $out = $this->{outfile}->getOutName;
		my $base = $this->{ovner}->getBaseUri('o');
		if($base->isBase($out))
		{
			my $rel = $base->getRelative($out);
			$this->{attribs}{href} = "$rel";
		}
		else
		{
			$this->{attribs}{href} = $out;
		}
	}
	$this->SUPER::toStr;
}


1;
package DtdXml::Tag::Entity::Xml;

our $VERSION = 0.01;
use base qw(DtdXml::Tag::Entity);

our $tagname = 'xml-ref';


1;
package DtdXml::Tag::Entity::Document;

our $VERSION = 0.01;
use base qw(DtdXml::Tag::Entity);

our $tagname = 'document';

sub reg_file 
{
	my $this = shift;
	$this->SUPER::reg_file;
	$this->{outfile}->setRoot($this->{attribs}{name});
}


1;
package DtdXml::Tag::Entity::Notation;

our $VERSION = 0.01;
use base qw(DtdXml::Tag::Entity);

our $tagname = 'notation';

sub reg_file {}

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
