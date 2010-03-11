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

void ColorPrintInConsole(SString *testFile)
{
  FileInputSource fis = FileInputSource(testFile, null);
  try{
    DocumentBuilder *db = new DocumentBuilder();
    Document *doc = db->parse(&fis);

    printLevel(doc, 0);

    db->free(doc);
    delete db;
  }catch(Exception &e){
    wprintf(L"\nException: %s\n\n", e.getMessage()->getWChars());
  }catch(...){
    wprintf(L"Unknown exception\n");
  }

}
void printLevel(Node *node, int lev)
{
  int i;
  Node *tmp = node;

  do{

    if (tmp->getNodeType() == Node::DOCUMENT_NODE){
      Node *elem = ((Document*)tmp)->getFirstChild();
      if (elem){
        printLevel(elem, lev);
      }
    }

    if (tmp->getNodeType() == Node::ELEMENT_NODE){
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
        printLevel(elem->getFirstChild(), lev+1);
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

    if (tmp->getNodeType() == Node::TEXT_NODE){
      Text *elem = (Text*)tmp;
      COLOR(PLAIN);
      wprintf(L"%s", elem->getData()->getWChars());
      COLOR(NORM);

      CharacterData *elem1 = (CharacterData*)elem;
      elem1->getData();
    };

    if (tmp->getNodeType() == Node::PROCESSING_INSTRUCTION_NODE){
      ProcessingInstruction *elem = (ProcessingInstruction*)tmp;
      COLOR(PLAIN);
      wprintf(L"<?%s %s?>", elem->getTarget()->getWChars(), elem->getData()->getWChars());
      COLOR(NORM);

      CharacterData *elem1 = (CharacterData*)elem;
      elem1->getData();

    };

    if (tmp->getNodeType() == Node::COMMENT_NODE){
      Comment *elem = (Comment*)tmp;
      COLOR(PLAIN);
      wprintf(L"<!--%s-->", elem->getData()->getWChars());
      elem->getLength();
      COLOR(NORM);
    };

    tmp = tmp->getNextSibling();
  }while(tmp);
};
