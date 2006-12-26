

#include "xmlbin.h"

class BinaryXML_hrc : public BinaryXMLWriter {

public:


#define TOKEN_VALUE_region  0
#define TOKEN_VALUE_region0  1
#define TOKEN_VALUE_region1  2
#define TOKEN_VALUE_region2  3
#define TOKEN_VALUE_region3  4
#define TOKEN_VALUE_region4  5
#define TOKEN_VALUE_region5  6
#define TOKEN_VALUE_region6  7
#define TOKEN_VALUE_region7  8
#define TOKEN_VALUE_region8  9
#define TOKEN_VALUE_region9  10
#define TOKEN_VALUE_regiona  11
#define TOKEN_VALUE_regionb  12
#define TOKEN_VALUE_regionc  13
#define TOKEN_VALUE_regiond  14
#define TOKEN_VALUE_regione  15
#define TOKEN_VALUE_regionf  16
#define TOKEN_VALUE_hrc  17
#define TOKEN_VALUE_annotation  18
#define TOKEN_VALUE_prototype  19
#define TOKEN_VALUE_package  20
#define TOKEN_VALUE_type  21
#define TOKEN_VALUE_version  22
#define TOKEN_VALUE_appinfo  23
#define TOKEN_VALUE_documentation  24
#define TOKEN_VALUE_contributors  25
#define TOKEN_VALUE_location  26
#define TOKEN_VALUE_name  27
#define TOKEN_VALUE_description  28
#define TOKEN_VALUE_targetNamespace  29
#define TOKEN_VALUE_filename  30
#define TOKEN_VALUE_firstline  31
#define TOKEN_VALUE_parameters  32
#define TOKEN_VALUE_param  32
#define TOKEN_VALUE_value  32
#define TOKEN_VALUE_group  35
#define TOKEN_VALUE_link  36
#define TOKEN_VALUE_weight  37
#define TOKEN_VALUE_import  38
#define TOKEN_VALUE_entity  39
#define TOKEN_VALUE_scheme  40
#define TOKEN_VALUE_regexp  41
#define TOKEN_VALUE_block  42
#define TOKEN_VALUE_keywords  43
#define TOKEN_VALUE_inherit  44
#define TOKEN_VALUE_if  45
#define TOKEN_VALUE_unless  46
#define TOKEN_VALUE_parent  47
#define TOKEN_VALUE_match  48
#define TOKEN_VALUE_priority  49
#define TOKEN_VALUE_start  50
#define TOKEN_VALUE_end  51
#define TOKEN_VALUE_content_priority  52
#define TOKEN_VALUE_inner_region  53
#define TOKEN_VALUE_region00  54
#define TOKEN_VALUE_region01  55
#define TOKEN_VALUE_region02  56
#define TOKEN_VALUE_region03  57
#define TOKEN_VALUE_region04  58
#define TOKEN_VALUE_region05  59
#define TOKEN_VALUE_region06  60
#define TOKEN_VALUE_region07  61
#define TOKEN_VALUE_region08  62
#define TOKEN_VALUE_region09  63
#define TOKEN_VALUE_region0a  64
#define TOKEN_VALUE_region0b  65
#define TOKEN_VALUE_region0c  66
#define TOKEN_VALUE_region0d  67
#define TOKEN_VALUE_region0e  68
#define TOKEN_VALUE_region0f  69
#define TOKEN_VALUE_region10  70
#define TOKEN_VALUE_region11  71
#define TOKEN_VALUE_region12  72
#define TOKEN_VALUE_region13  73
#define TOKEN_VALUE_region14  74
#define TOKEN_VALUE_region15  75
#define TOKEN_VALUE_region16  76
#define TOKEN_VALUE_region17  77
#define TOKEN_VALUE_region18  78
#define TOKEN_VALUE_region19  79
#define TOKEN_VALUE_region1a  80
#define TOKEN_VALUE_region1b  81
#define TOKEN_VALUE_region1c  82
#define TOKEN_VALUE_region1d  83
#define TOKEN_VALUE_region1e  84
#define TOKEN_VALUE_region1f  85
#define TOKEN_VALUE_virtual  86
#define TOKEN_VALUE_subst_scheme  87
#define TOKEN_VALUE_word  88
#define TOKEN_VALUE_symb  89
#define TOKEN_VALUE_ignorecase  90
#define TOKEN_VALUE_worddiv  91
#define TOKEN_VALUE_MAX 92

    const static char* TOKEN_VALUES[];

    DString *getToken(int id)
    {
        if (id < 0 || id >= TOKEN_VALUE_MAX) return null;
        return new DString(TOKEN_VALUES[id]);
    }

protected:

void toBinary_hrc(Element *el){

    Element *child = (Element*)el;

    

        putInteger(TOKEN_VALUE_hrc);

    
        toBinaryType_hrc((Element*)child);
    

}
  
void toBinaryType_hrc(Element *el){
    


    int attrCount = 0 +1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_version);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_version]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_annotation])){
            

        putInteger(TOKEN_VALUE_annotation);

    
        toBinaryType_annotation((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_prototype])){
            

        putInteger(TOKEN_VALUE_prototype);

    
        toBinaryType_prototype((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_package])){
            

        putInteger(TOKEN_VALUE_package);

    
        toBinaryType_package((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_type])){
            

        putInteger(TOKEN_VALUE_type);

    
        toBinaryType_type((Element*)child);
    
        }
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_annotation(Element *el){
    


    int attrCount = 0 ;

    putInteger(attrCount);

    


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_appinfo])){
            

        putInteger(TOKEN_VALUE_appinfo);

    
    {
      Element *el = (Element*)child;
      


    int attrCount = 0 ;

    putInteger(attrCount);

    


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
    }
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_documentation])){
            

        putInteger(TOKEN_VALUE_documentation);

    
    {
      Element *el = (Element*)child;
      


    int attrCount = 0 ;

    putInteger(attrCount);

    


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
    }
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_contributors])){
            

        putInteger(TOKEN_VALUE_contributors);

    
    {
      Element *el = (Element*)child;
      


    int attrCount = 0 ;

    putInteger(attrCount);

    


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
    }
    
        }
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_package(Element *el){
    


    int attrCount = 0 +1+1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_name);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_name]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_description);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_description]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_targetNamespace);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_targetNamespace]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_annotation])){
            

        putInteger(TOKEN_VALUE_annotation);

    
        toBinaryType_annotation((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_location])){
            

        putInteger(TOKEN_VALUE_location);

    
        toBinaryType_location((Element*)child);
    
        }
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_prototype(Element *el){
    


    int attrCount = 0 +1+1+1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_name);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_name]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_description);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_description]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_group);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_group]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_targetNamespace);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_targetNamespace]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_annotation])){
            

        putInteger(TOKEN_VALUE_annotation);

    
        toBinaryType_annotation((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_location])){
            

        putInteger(TOKEN_VALUE_location);

    
        toBinaryType_location((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_filename])){
            

        putInteger(TOKEN_VALUE_filename);

    
        toBinaryType_filename((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_firstline])){
            

        putInteger(TOKEN_VALUE_firstline);

    
        toBinaryType_firstline((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_parameters])){
            

        putInteger(TOKEN_VALUE_parameters);

    
    {
      Element *el = (Element*)child;
      


    int attrCount = 0 ;

    putInteger(attrCount);

    


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_param])){
            

        putInteger(TOKEN_VALUE_param);

    
    {
      Element *el = (Element*)child;
      


    int attrCount = 0 +1+1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_name);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_name]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_value);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_value]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_description);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_description]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
    }
    
        }
        
        child = child->getNextSibling();
    }
  
    }
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_param])){
            

        putInteger(TOKEN_VALUE_param);

    
    {
      Element *el = (Element*)child;
      


    int attrCount = 0 +1+1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_name);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_name]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_value);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_value]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_description);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_description]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
    }
    
        }
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_location(Element *el){
    


    int attrCount = 0 +1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_link);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_link]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_filename(Element *el){
    


    int attrCount = 0 
    aa
  ;

    putInteger(attrCount);

    


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_firstline(Element *el){
    


    int attrCount = 0 
    aa
  ;

    putInteger(attrCount);

    


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_type(Element *el){
    


    int attrCount = 0 +1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_name);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_name]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_annotation])){
            

        putInteger(TOKEN_VALUE_annotation);

    
        toBinaryType_annotation((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_import])){
            

        putInteger(TOKEN_VALUE_import);

    
        toBinaryType_import((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_region])){
            

        putInteger(TOKEN_VALUE_region);

    
        toBinaryType_region((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_entity])){
            

        putInteger(TOKEN_VALUE_entity);

    
        toBinaryType_entity((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_scheme])){
            

        putInteger(TOKEN_VALUE_scheme);

    
        toBinaryType_scheme((Element*)child);
    
        }
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_scheme(Element *el){
    


    int attrCount = 0 +1+1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_name);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_name]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_if);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_if]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_unless);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_unless]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_annotation])){
            

        putInteger(TOKEN_VALUE_annotation);

    
        toBinaryType_annotation((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_regexp])){
            

        putInteger(TOKEN_VALUE_regexp);

    
        toBinaryType_regexp((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_block])){
            

        putInteger(TOKEN_VALUE_block);

    
        toBinaryType_block((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_keywords])){
            

        putInteger(TOKEN_VALUE_keywords);

    
        toBinaryType_keywords((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_inherit])){
            

        putInteger(TOKEN_VALUE_inherit);

    
        toBinaryType_inherit((Element*)child);
    
        }
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_import(Element *el){
    


    int attrCount = 0 +1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_type);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_type]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_entity(Element *el){
    


    int attrCount = 0 +1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_name);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_name]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_value);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_value]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_region(Element *el){
    


    int attrCount = 0 +1+1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_name);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_name]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_parent);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_parent]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_description);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_description]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_regexp(Element *el){
    


    int attrCount = 0 
    aa
  ;

    putInteger(attrCount);

    


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_block(Element *el){
    


    int attrCount = 0 +1+1+1+1+1+1+1ag;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_start);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_start]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_end);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_end]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_scheme);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_scheme]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_priority);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_priority]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_content_priority);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_content_priority]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_inner_region);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_inner_region]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_start])){
            

        putInteger(TOKEN_VALUE_start);

    
        toBinaryType_blockInner((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_end])){
            

        putInteger(TOKEN_VALUE_end);

    
        toBinaryType_blockInner((Element*)child);
    
        }
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_blockInner(Element *el){
    


    int attrCount = 0 
    aa
  ;

    putInteger(attrCount);

    


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_inherit(Element *el){
    


    int attrCount = 0 +1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_scheme);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_scheme]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_virtual])){
            

        putInteger(TOKEN_VALUE_virtual);

    
        toBinaryType_virtual((Element*)child);
    
        }
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_virtual(Element *el){
    


    int attrCount = 0 +1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_scheme);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_scheme]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_subst_scheme);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_subst_scheme]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_keywords(Element *el){
    


    int attrCount = 0 +1+1+1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_ignorecase);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_ignorecase]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_region);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_region]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_priority);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_priority]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_worddiv);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_worddiv]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_word])){
            

        putInteger(TOKEN_VALUE_word);

    
        toBinaryType_word((Element*)child);
    
        }
        
        if (child->getNodeType() == Node::ELEMENT_NODE &&
            child->getNodeName()->equals(TOKEN_VALUES[TOKEN_VALUE_symb])){
            

        putInteger(TOKEN_VALUE_symb);

    
        toBinaryType_symb((Element*)child);
    
        }
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_symb(Element *el){
    


    int attrCount = 0 +1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_name);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_name]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_region);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_region]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  
void toBinaryType_word(Element *el){
    


    int attrCount = 0 +1+1;

    putInteger(attrCount);

    
    {
        putInteger(TOKEN_VALUE_name);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_name]));
        putString(attr);
    }
  
    {
        putInteger(TOKEN_VALUE_region);
        const String *attr = el->getAttribute(&DString(TOKEN_VALUES[TOKEN_VALUE_region]));
        putString(attr);
    }
  


    int childCount = 0;

    Node *child = el->getFirstChild();
    while(child != null){
        if (child->getNodeType() == Node::ELEMENT_NODE){
            childCount++;
        }
        child = child->getNextSibling();
    }

    putInteger(childCount);

    child = el->getFirstChild();
    while(child != null){
        
        child = child->getNextSibling();
    }
  
}
  

   void writeElementCount(Document *doc){
       int elcount = 0;
       bool skip_childred = false;

       Node *rmnext = doc->getDocumentElement();
       while(rmnext != doc && rmnext != null)
       {
         if (!skip_childred){
           while(rmnext->getFirstChild() != null){
             rmnext = rmnext->getFirstChild();
           }
         };
         skip_childred = false;

         Node *el = rmnext->getNextSibling();
         if(el == null){
           el = rmnext->getParent();
           skip_childred = true;
         }
         elcount++;
         rmnext = el;
       }
       putInteger(elcount);
   }

public:

void toBinary(Document *doc, Writer *writer){
   setWriter(writer);

//   writeElementCount(doc);

   toBinary_hrc(doc->getDocumentElement());

   completed();
}

};

const char* BinaryXML_hrc::TOKEN_VALUES[] = {

"region",
"region0",
"region1",
"region2",
"region3",
"region4",
"region5",
"region6",
"region7",
"region8",
"region9",
"regiona",
"regionb",
"regionc",
"regiond",
"regione",
"regionf",
"hrc",
"annotation",
"prototype",
"package",
"type",
"version",
"appinfo",
"documentation",
"contributors",
"location",
"name",
"description",
"targetNamespace",
"filename",
"firstline",
"parameters",
"param",
"value",
"group",
"link",
"weight",
"import",
"entity",
"scheme",
"regexp",
"block",
"keywords",
"inherit",
"if",
"unless",
"parent",
"match",
"priority",
"start",
"end",
"content-priority",
"inner-region",
"region00",
"region01",
"region02",
"region03",
"region04",
"region05",
"region06",
"region07",
"region08",
"region09",
"region0a",
"region0b",
"region0c",
"region0d",
"region0e",
"region0f",
"region10",
"region11",
"region12",
"region13",
"region14",
"region15",
"region16",
"region17",
"region18",
"region19",
"region1a",
"region1b",
"region1c",
"region1d",
"region1e",
"region1f",
"virtual",
"subst-scheme",
"word",
"symb",
"ignorecase",
"worddiv",
};

  