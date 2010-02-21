use strict;
package Helper::Switch;
use Exporter;

our $VERSION = 0.03;
our @ISA = qw(Exporter);
our @EXPORT= qw(switch case case_lt case_gt default when test where trase next_test no_return);


sub is_number($)
{
	#print "Check $_[0]\n";
	!(($_[0] + 0) ne $_[0])
}


# switch { <case, ...>} @list
# 1. Во всех встореных ф-циях доступна переменная $_.
# Это - элемент исхоного массива. Ее изменение _ВЛИЯЕТ_ на 
# этот массив.
# 2. Во всех блоках 'then' можно юзать 'next', тогда оно передаст 
# управление на следующий блок.
# 3. Каждый then, который не кочается next, может возвращать значение.
# Результатом switch станет их список (или  последний из них, если нужен
# скаляр).
# 4. Внутри блока switch перед case можно изпользовать любой код 
# инициализации - в том числе объявления my.
# Они будут доступны во вложенных ф-циях. В if - секциях case сохраняются 
# _СТАТИЧЕСКИЕ_ значиния на момент начала обработки. Во вложенных блоках 
# значения динамические (могут меняться).
# 5. Помимо 'next' доступны другие методы-"перехватчики" с похожим эффектом.
# Реализованы через goto между ф-циями.


sub switch(&@)
{
	my @switch = &{shift()};
	my @ret = ();
	
	#print "SWITH: @_\n";
	foreach(@_)
	{
		my $next = 0;
		#print "SWITH 1: $next $_\n";
		foreach my $case(@switch)
		{
			#print "\tSWITH 2: $next $case\n";
			if($next || &{$case->{if}})
			{
				$next = 1;
				push @ret, &{$case->{then}};
				last;
			}
			NEXT_OK: # перехватчик вызывается, если нужен next с конторлем проверок
			$next = 0;
		}
		NO_RETURN: # перехватчик для эмуляции неработающего (почему???) last
	}
	return wantarray ? @ret : pop @ret;
}



# case-обработчки.
# Должны возвращать первым элементом списка ссылку на две ф-ции
# 1. if - возвращает булево значение.
# 2. then - оно выполняется если if и 
# switch для этого элемента массива закончен.

# when{<testing>} sub {<use it>},

sub when(&&@)
{
	my $ref =
	{
		if=>shift(),
		then=>shift()
	};
	($ref,@_)
}


# case <value> => sub {<use it>},

sub case($&@)
{
	my $name=shift;
	
	#print "CASE: $name\n";
	my $ref = 
	{
		if=>sub 
		{
			#print "CASE SUB: $_ ?? $name\n";
			
			(ref($name) eq 'Regexp')
			? m/$name/ 
			: (is_number($name) && is_number($_))
			? $name == $_ 
			: $name eq $_
		}, 
		then=>shift()
	};
	($ref,@_)
}

# default {<use it>},

sub default(&@)
{
	my $ref =
	{
		if=>sub {return 1},
		then=>shift()
	};
	($ref,@_)
}


# variants of 'case'...

sub case_gt($&@)
{
	my $name=shift;
	my $ref = 
	{
		if=>sub 
		{
			(is_number($name) && is_number($_))
			? $name < $_ 
			: $name lt $_
		}, 
		then=>shift()
	};
	($ref,@_)
}

sub case_lt($&@)
{
	my $name=shift;
	my $ref = 
	{
		if=>sub 
		{
			(is_number($name) && is_number($_))
			? $name > $_ 
			: $name gt $_
		}, 
		then=>shift()
	};
	($ref,@_)
}



# test 'foo',
# test 'bar',
# test 'baz',
# where { do_some() }
#
# equ
#
# case 'foo' => sub{ next; },
# case 'bar' => sub{ next; },
# case 'baz' => sub{ do_some() }


sub test($@)
{
	my $name=shift;
	&case($name, sub{ next }, @_)
}

sub where(&@)
{
	my $ref =
	{
		if=>sub {return 0},
		then=>shift()
	};
	($ref,@_)
}



# trase { foo() }
# equ
# default { foo(); next_test }

sub trase(&@)
{
	my $then = shift;
	&default(sub{ &$then; next_test() }, @_);
}


# Перехватчики. Вызаваются из then-функций, дают им управление над 
# дальнейшим ходом цикла.

# Аналог next, но возбновляет проверку. Т.е. управление передается не 
# следующей а первой совпавшей.

sub next_test()
{
	goto NEXT_OK;
}

# Не дает сохранить значение в выходной массив

sub no_return()
{
	goto NO_RETURN;
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
