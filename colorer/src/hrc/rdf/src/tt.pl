#!perl 
while(<>)
{
 s{\s*<rdf:(\w+).+?node(\d).*}{
   $a=(($2==1)?'nodeElement':'propertyElt');
   "<xs:element name='$1' type='$a'/>"
 }ge;
 print;
}
