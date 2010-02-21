rem todo: build.xml for ant...

perl ../main/dtd2xml.pl dtdxml.dxc
java -jar %ANT_HOME%\lib\saxon9.jar -o dtdxml.xsd dtdxml-dtd.xml ..\dtd2xsd\dtd2xsd.xsl
