#ifndef _XMLPRINT_H_
#define _XMLPRINT_H_

#include "xml/xmldom.h"

void PrintInConsole(SString *inFile,bool color);
void printLevelColor(Node *node, int lev);

#endif _XMLPRINT_H_