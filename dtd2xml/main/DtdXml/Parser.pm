use strict;
package DtdXml::Parser;

our $VERSION = 0.02;
our @ISA = qw(Exporter);

use Exporter;

use DtdXml::Parser::Catalog;
use DtdXml::Parser::File;
use DtdXml::Parser::Doc;

our @EXPORT= qw(parseAll addFiles addFileNames setComment getComment getXmlInfo);

our @Current;
our @Files; 
our $StartComment = "\n<!--\n\tDTDXML AUTO OUTPUT\n-->\n";



sub setCurrent($)
{
	#$Current = $_[0];
	unshift @Current, $_[0];
}

sub getCurrent()
{
	return $Current[0];
}

sub unsetCurrent()
{
	#return shift @Current;
}

sub getXmlInfo
{
	#print "GetXmlInfo @Current\n";
	return getCurrent()->getXmlInfo();
}


sub setComment($)
{
	$StartComment = $_[0];
}

sub getComment()
{
	return $StartComment;
}




#global
sub addFile
{
	my $file = shift;
	#print "Add file $file->{uri}\n";
	return if grep{$file eq $_} @Files;
	push @Files, $file;
	#print "File $file->{uri} aded\n";
}


sub addFiles
{
	local $_;
	addFile $_ foreach @_;
}

sub addFileNames
{
	addFiles map { addSysFile(1,$_) } @_;
}



sub parseFile($)
{
	my $file = $_[0];
	#print "Parse file $file\n";
	setCurrent($file);
	
	$file->load();
	$file->run();
	$file->parse();
	#$file->save();
	
	unsetCurrent();
	#my $in = getInName($_[0]);
}


sub saveFile($)
{
	my $file = shift;
	#print "Save file $file\n";
	setCurrent($file);
	$file->save();
	unsetCurrent();
}



sub parseAll()
{
	local $_;
	#local $" = '|';
	#print "files 1 @Files\n";
	foreach(@Files)
	{
		#print "files 2 @Files\n";
		parseFile $_; 
		#print "files 3 @Files\n";
	}
	#print "files 4 @Files\n";
	saveFile $_  foreach @Files;
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
