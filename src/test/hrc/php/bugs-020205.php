
<?
echo <<< END
test
END
.'test';
?>

<?
/* ⠪�� ����୮ ��ࠡ��뢠���� ��ப� � ������� ����窠� */
$var1 = '1';
echo "${var1}test{$var1}test${'va'."r1"}<br>\n";
$a['test'] = 'hello';
echo "test$a[test]test<br>\n";
class a { }
$a = new a;
$a->ddd = 'c';
echo "var is [$a->ddd]test<br>\n";

?>
/*
    ��������� ���ᢥ⪠ ��� ����:
    <script language="php">
    php script class{}
    </script>
*/

/*
    �� ���ᢥ稢����� ���� keyword'�, ���訥�� � PHP5.
<?
    try, catch, trhow, public, private � �.�.
?>
*/
