
#include<windows.h>
#include<common/io/InputSource.h>
#include<common/io/StreamWriter.h>

#include"xmlbin.h"
#include"../../src/test/xmlparser/xmlprint.cpp"

#define BinaryXML_hrc BinaryXMLWriter

int main(int argc, char *argv[])
{

  try{
    int t1;

    DocumentBuilder db;
    Document *doc;
    if (argv[1]){
      InputSource *is = InputSource::newInstance(&DString(argv[1]));
      is->openStream();
      is->closeStream();

      t1 = GetTickCount();
      doc = db.parse(is);
    }
    int t2 = GetTickCount()-t1;
    printf("TIMING-XML:\t%d\n", t2);
    if (argv[1]){
      //StreamWriter *writer = new StreamWriter(stdout, ENC_);

      BinaryXML_hrc binxml;
      binxml.toBinary(doc, null);

    }

    int t3 = GetTickCount()-t1-t2;
    printf("TIMING-SER:\t%d\n", t3);

//
    InputSource *is2 = InputSource::newInstance(&DString("jar:output.zip!output"));
    is2->openStream();
    is2->closeStream();


    t3 = GetTickCount()-t1-t2;

    BinaryXML_hrc binxml2;
    Document *doc2 = binxml2.parse(is2);

    int t4 = GetTickCount()-t1-t2-t3;
    printf("TIMING-BIN:\t%d\n", t4);

//    printLevel(doc2, 0);

    binxml2.free(doc2);

    if (argv[1]){
      db.free(doc);
    }

  }catch(Exception &e){
    printf(e.getMessage()->getChars());
  };

}