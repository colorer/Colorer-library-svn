
#include<stdio.h>
#include<fcntl.h>
#include<io.h>
#include<xml/xmldom.h>
#include<unicode/String.h>

#define ELEMENT_NODE_MASK (1 << 12)
#define TEXT_NODE_MASK    (2 << 12)
#define NODE_MASK         (0xF000)

#define MAGIC_WORD        'blmx'

class BinaryXMLWriter {
public:
  /**
   * Parses input stream and creates DOM tree.
   */
  Document *parse(InputSource *is)
  {
      data = is->openStream();
      datalen = is->length();

      doc = db.newDocument();

      dataptr = 0;

      if (getInteger() != MAGIC_WORD) {
        throw Exception(DString("Not a Binary XML format!"));
      }

      int tokenCount = getInteger();
      while(tokenCount--){
        tokenVector.addElement(getString()); // free() vector!
      }

      parseElement(doc, getShort());

      is->closeStream();

      return doc;
  }

  void free(Document *doc)
  {
      db.free(doc);
  }


  void toBinary(Document *doc, Writer *writer)
  {
      setWriter(writer);

      putInteger(MAGIC_WORD);

      tokenCount = 0;
      collectNodes(doc->getDocumentElement());
      putInteger(tokenVector.size());
      for(int idx = 0; idx < tokenVector.size(); idx++){
        putString(tokenVector.elementAt(idx));
      }

      serialize(doc);

      completed();
  }

  void serialize(Document *doc) {
      serialize(doc->getDocumentElement());
  }


protected:
  const byte *data;
  int datalen;
  int dataptr;

  int fid;

  Hashtable<int> tokens;
  int tokenCount;
  Vector<const String*> tokenVector;
  DocumentBuilder db;
  Document *doc;


  virtual String *getToken(int id)
  {
    return new DString(tokenVector.elementAt(id-1));
  }

  void parseElement(Node *parent, int nametag)
  {
    String *tag = getToken(nametag & ~NODE_MASK);

    Element *elem = parent->getOwnerDocument()->createElement(tag);
    parent->appendChild(elem);

    int attrCount = getShort();

    while(attrCount--) {
      String *attrName = getToken(getShort());
      String *attrValue = getString();

      if (attrValue) {
        elem->setAttribute(attrName, attrValue);
      };
    }

    int childCount = getShort();
    while(childCount--) {
      int type = getShort();
      if ((type&NODE_MASK) == ELEMENT_NODE_MASK){
        parseElement(elem, type);
      }
      if ((type&NODE_MASK) == TEXT_NODE_MASK){
        parseText(elem);
      }
    }
  }

  void parseText(Node *parent)
  {
    Text *text = parent->getOwnerDocument()->createTextNode(getString());
    parent->appendChild(text);
  }


  int getInteger() {
    int val = *(int*)(data+dataptr);
    dataptr += sizeof(int);
    return val;
  }

  int getShort() {
    int val = *(int*)(data+dataptr) & 0xFFFF;
    dataptr += 2;
    return val;
  }

  String *getString() {
    int len = getShort();
    if (len == 0){
      return null;
    }
    len--;
    dataptr += len*sizeof(wchar);
    return new SString(DString((const wchar*)(data+dataptr-len*sizeof(wchar)), 0, len));
    //return new DString((const byte*)(data+dataptr-len), len, Encodings::ENC_UTF8);
  }




  void addToken(const String *token) {
    if (tokens.get(token) == 0){
      tokenCount++;
      tokens.put(token, tokenCount);
      tokenVector.addElement(token);
    }
  }

  int getTokenID(const String *token) {
    if (tokens.get(token) == 0) throw Exception(DString("Bad token"));
    return tokens.get(token);
  }




  void collectNodes(Element *el) {

    addToken(el->getNodeName());

    const Vector<const String*> *attrs = el->getAttributes();

    for(int idx = 0; idx < attrs->size(); idx++){
      const String *aname = attrs->elementAt(idx);
      const String *aval = el->getAttribute(aname);

      addToken(aname);
    }

    for(Node *next = el->getFirstChild(); next != null; next = next->getNextSibling()){
      if (next->getNodeType() == Node::ELEMENT_NODE){
        collectNodes((Element*)next);
      }
    }

  }


  void serialize(Element *el)
  {
    int eltype = getTokenID(el->getNodeName()) | ELEMENT_NODE_MASK;
    putShort(eltype);

    const Vector<const String*> *attrs = el->getAttributes();
    putShort(attrs->size());

    for(int idx = 0; idx < attrs->size(); idx++){
      const String *aname = attrs->elementAt(idx);
      const String *aval = el->getAttribute(aname);

      putShort(getTokenID(aname));
      putString(aval);
    }

    Node *next;
    int childCount = 0;

    for(next = el->getFirstChild(); next != null; next = next->getNextSibling()){
      if (next->getNodeType() == Node::ELEMENT_NODE ||
          next->getNodeType() == Node::TEXT_NODE){
        childCount++;
      }
    }
    putShort(childCount);

    for(next = el->getFirstChild(); next != null; next = next->getNextSibling()){
      if (next->getNodeType() == Node::ELEMENT_NODE){
        serialize((Element*)next);
      }
      if (next->getNodeType() == Node::TEXT_NODE){
        serialize((Text*)next);
      }
    }
  }

  void serialize(Text *el)
  {
    putShort(TEXT_NODE_MASK);
    putString(el->getData());
  }


  void setWriter(Writer *writer){
    //this->writer = writer;

    fid = open("output", _O_BINARY|_O_CREAT|_O_RDWR);

  }

  void completed() {
    close(fid);
  }

  void putInteger(int val) {
    write(fid, &val, 4);
  }
  void putShort(int val) {
    if (val < 0 || val >= 0x10000){
      throw Exception(DString("XMLB Short storage overflow"));
    }
    write(fid, &val, 2);
  }

  void putString(const String *val) {
    if (val == null){
      putShort(0);
    }else{
      putShort(val->length()+1);

      for(int i = 0; i < val->length(); i++){
        wchar cval = (*val)[i];
        write(fid, &cval, sizeof(wchar));
      }
    }
  }

};
