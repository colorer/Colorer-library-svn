use strict;
package DtdXml::Entities;
use Exporter;

our $VERSION = 0.01;
our @ISA = qw(Exporter);
our @EXPORT = qw(makeRole);
our @EXPORT_OK = qw(addEntity hasPrior forced is_forced); #  getEntity

our %Entities;
our %Roles;

our $Force = 0;

our %Priority =
(
#	attlist => ['name', 'attrib'],
	name => ['item'],
	item => ['element']
);


sub forced($)
{
	$Force = $_[1];
}

sub is_forced()
{
	return $Force;
}


sub hasPrior($$)
{
	my ($gt, $lt) = @_;
	return grep{$_ eq $lt} @{$Priority{$gt}};
}



sub addEntity($)
{
	my $ent = shift;
	#my $dp = 0;
	#$dp = 1 if($ent->{name} =~ /SVG.xmlns.attrib/);
	
	#print STDERR "Warning: reset entity $ent->{name}\n\tOld: $Entities{$ent->{name}}{value}\n\tNew: $ent->{value}\n" if defined $Entities{$ent->{name}};
	
	#print "\nAdd $ent->{name}\n" if $dp;
	if(defined $Roles{$ent->{name}})
	{
		#print STDERR "Warning: backward set role '$Roles{$ent->{name}}' for $ent->{name}\n";
		#print "Add $ent->{name} role\n" if $dp;
		
		$ent->setRole($Roles{$ent->{name}});
	}
	
	if(defined $Entities{$ent->{name}})
	{
		#print STDERR "Warning: reset entity $ent->{name}\n" if defined $Entities{$ent->{name}};
		#print "Add $ent->{name} to list [not new]\n" if $dp;
		push @{$Entities{$ent->{name}}}, $ent;
	}
	else
	{
		#print "Add $ent->{name} to list [new]\n" if $dp;
		$Entities{$ent->{name}} = [ $ent ];
	}
}

sub makeRole($$)
{
	my ($name, $role) = @_;
	
	#my $dp = 0;
	#$dp = 1 if($name =~ /SVG.xmlns.attrib/);
	
	#print "\nSet $role to $name\n" if $dp;
	
	if(!is_forced && defined $Roles{$name} && $Roles{$name} ne $role)
	{
		# attlist includes attrib
		#print "Detect role use $Roles{$name} for $name\n" if $dp;
		return if hasPrior $Roles{$name}, $role; # eq 'attlist' && ($role eq 'attrib' || $role eq 'name'); 
		
		print STDERR "Warning: redefine role for $name: $Roles{$name} -> $role\n" unless hasPrior $role, $Roles{$name};
		
		# if defined $Roles{$name} && $Roles{$name} ne $role;
	}
	$Roles{$name} = $role;
	
	unless(defined $Entities{$name})
	{
		#print "Not foind $name\n" if $dp;
	#	print "NOTE: attempt to set role $role to undefined entity $name\n";
	}
	else
	{
		foreach(@{$Entities{$name}})
		{
			#print "$_->{name}: set role (total $#{$Entities{$name}})\n" if $dp;
			$_->setRole($role);
		}
	}
}
1;


__END__

sub getEntity($)
{
	my $name = shift;
	return undef unless defined $Entities{$name};
	return $Entities{$name};
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
