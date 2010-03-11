#include<common/io/FileInputSource.h>
#include<xml/xmldom.h>
#include<windows.h>
#include<stdio.h>
#include "xmlprint.h"

int main(int argc, char *argv[])
{
  FileInputSource fis = FileInputSource(&DString(argv[1]), null);
  printf("FILE:\t%s\t\t\t\t", argv[1]);
  try{

    int t1 = GetTickCount();

    DocumentBuilder *db = new DocumentBuilder();

    Document *doc = db->parse(&fis);

    int t2 = GetTickCount()-t1;

    printf("TIMING:\t%d\n", t2);

    printLevel(doc, 0);

    db->free(doc);
    delete db;
  }catch(Exception &e){
    printf("\n\nEXC: %s\n\n", e.getMessage()->getChars());
  }catch(...){
    printf("UNK\n");
  }
  return 0;
};
