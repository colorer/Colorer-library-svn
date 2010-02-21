use strict;
package DtdXml::Names;
use Exporter;

our $VERSION = 0.01;
our @ISA = qw(Exporter);
our @EXPORT= qw($QName $NCName $Name $Nmtoken $PERef isPERef);


my $NSNameStartChar = 'A-Z_a-z\x{C0}-\x{D6}\x{D8}-\x{F6}\x{F8}-\x{2FF}\x{370}-\x{37D}\x{37F}-\x{1FFF}\x{200C}-\x{200D}\x{2070}-\x{218F}\x{2C00}-\x{2FEF}\x{3001}-\x{D7FF}\x{F900}-\x{FDCF}\x{FDF0}-\x{FFFD}\x{10000}-\x{EFFFF}';
my $NSNameChar = $NSNameStartChar.'\-\.\d\x{B7}\x{0300}-\x{036F}\x{203F}-\x{2040}';
my $NameStartChar = ':'.$NSNameStartChar;
my $NameChar = ':'.$NSNameChar;

our $NCName = qr/[$NSNameStartChar][$NSNameChar]*/o;
our $Name = qr/[$NameStartChar][$NameChar]*/o;
our $QName = qr/(?:$NCName:)?$NCName/o;
our $Nmtoken = qr/[$NameChar]+/o;

our $PERef = qr/%$Name;/;


sub isPERef($)
{
	return ($_[0] =~ /^\s*%($Name);\s*$/)[0];
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
