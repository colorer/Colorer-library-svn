#ifdef _DEBUG
#include<vld.h>
#endif
#include "xmlprint.h"
#include <windows.h>
#include <wchar.h>
#include<common/io/FileInputSource.h>

#define COLOR(c) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),c)
#define BR    0x0E
#define TAG   0x0F
#define NORM  0x07
#define PAR   0x0C
#define PLAIN 0x09

void PrintInConsole(SString *inFile, bool color)
{
  FileInputSource fis = FileInputSource(inFile, null);
  try{
    DocumentBuilder *db = new DocumentBuilder();
    db->setIgnoringComments(false);
    db->setIgnoringElementContentWhitespace(true);
    Document *doc = db->parse(&fis);
    if (color){
      printLevelColor(doc, 0);
    }else{
      wprintf(doc->toString(0,3)->getWChars());
    }

    db->free(doc);
    delete db;
  }catch(Exception &e){
    wprintf(L"\nException: %s\n\n", e.getMessage()->getWChars());
  }catch(...){
    wprintf(L"Unknown exception\n");
  }

}

void printLevelColor(Node *node, int lev)
{
  int i;
  Node *tmp = node;

  do{
    switch (tmp->getNodeType()){
      case Node::DOCUMENT_NODE:
        {
          COLOR(PLAIN);
          wprintf(L"<?xml version=\"%s\" encoding=\"%s\"?>\n", ((Document*)tmp)->getXmlVersion()->getWChars(),((Document*)tmp)->getXmlEncoding()->getWChars());
          COLOR(NORM);

          Node *elem = ((Document*)tmp)->getFirstChild();
          if (elem){
            printLevelColor(elem, lev);
          }
        }
        break;
      case Node::ELEMENT_NODE:
        {
          Element *elem = (Element*)tmp;

          for(i = 0; i < lev*3; i++){
            wprintf(L" ");
          }
          COLOR(BR);
          wprintf(L"<");
          COLOR(TAG);
          wprintf(L"%s", elem->getNodeName()->getWChars());
          COLOR(NORM);

          const Vector<const String*> *attrs = elem->getAttributes();
          for (i = 0; i < attrs->size(); i++){
            COLOR(PAR);
            wprintf(L" %s=", attrs->elementAt(i)->getWChars());
            COLOR(NORM);
            wprintf(L"\"%s\"", elem->getAttribute(attrs->elementAt(i))->getWChars());
          };
          COLOR(BR);
          wprintf(L">\n");

          if (tmp->hasChildNodes()){
            printLevelColor(elem->getFirstChild(), lev+1);
          };
          for(i = 0; i < lev*3; i++) wprintf(L" ");

          COLOR(BR);
          wprintf(L"</");
          COLOR(TAG);
          wprintf(L"%s", elem->getNodeName()->getWChars());
          COLOR(BR);
          wprintf(L">\n");
          COLOR(NORM);
        }
        break;
      case Node::TEXT_NODE:
        {
          Text *elem = (Text*)tmp;
          COLOR(PLAIN);
          wprintf(L"%s", elem->getData()->getWChars());
          COLOR(NORM);
        }
        break;
      case Node::PROCESSING_INSTRUCTION_NODE:
        {
          ProcessingInstruction *elem = (ProcessingInstruction*)tmp;
          COLOR(PLAIN);
          wprintf(L"<?%s %s?>\n", elem->getTarget()->getWChars(), elem->getData()->getWChars());
          COLOR(NORM);
        }
        break;
      case Node::COMMENT_NODE:
        {
          Comment *elem = (Comment*)tmp;
          for(i = 0; i < lev*3; i++) wprintf(L" ");
          COLOR(PLAIN);
          wprintf(L"<!--%s-->\n", elem->getData()->getWChars());
          elem->getLength();
          COLOR(NORM);
        }
        break;
    }

    tmp = tmp->getNextSibling();
  }while(tmp);
};


void SaveToFile(SString *inFile, SString *outFile)
{
  FileInputSource fis = FileInputSource(inFile, null);
  try{
    DocumentBuilder *db = new DocumentBuilder();
    db->setIgnoringComments(false);
    db->setIgnoringElementContentWhitespace(true);
    Document *doc = db->parse(&fis);
    doc->saveToFile(outFile);
    db->free(doc);
    delete db;
  }catch(Exception &e){
    wprintf(L"\nException: %s\n\n", e.getMessage()->getWChars());
  }catch(...){
    wprintf(L"Unknown exception\n");
  }

}