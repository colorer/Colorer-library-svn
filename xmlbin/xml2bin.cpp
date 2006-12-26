
#include<windows.h>
#include<common/io/InputSource.h>
#include<common/io/StreamWriter.h>

#include"xmlbin.h"

int main(int argc, char *argv[])
{

  try{
    DocumentBuilder db;
    Document *doc;
    if (!argv[1] || !argv[2]){
      printf("Usage: xml2bin input output");
    }

    InputSource *is = InputSource::newInstance(&DString(argv[1]));
    db.setIgnoringElementContentWhitespace(true);
    db.setIgnoringComments(true);
    doc = db.parse(is);

    BinaryXMLWriter binxml;
    binxml.toBinary(doc, null);

    db.free(doc);
  }catch(Exception &e){
    printf(e.getMessage()->getChars());
  };

}