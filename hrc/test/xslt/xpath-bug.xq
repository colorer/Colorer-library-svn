xquery version "1.0" encoding "windows-1251";

declare namespace def = "colorer://hrc/lib/default.hrc";

declare variable 
	(: ���� �� ������� (: �� hrc :) ����� :)
	$def:test1 as def:empty := aaa/bbb/ccc; 
declare variable $def:test2 := aaa/bbb/ccc + ccc; (: ����������... :)
declare variable $def:test3:=aaa/bbb;
declare variable $test4:=aaa/bbb; (: ����� :)

declare function def:foo ($bar as xs:string) as def:Text
{
	let $xx:= aaa/bbb, (: ����� :)
	let $aa as empty()? := aaa/bbb,
		$bb := aaa/bbb,
		$cc:= aaa/bbb (: ������ �����... :)
	
	<aaa>
	{
		for $aa in $n return xxx (: � ��� �����... 
��� ����������� ����� ��������� ���-������ � ������ ���... :) 
	}
	</aaa>
};

<foo>
{
	def:foo("&lt;�����&gt;")/bar
}
</foo>
