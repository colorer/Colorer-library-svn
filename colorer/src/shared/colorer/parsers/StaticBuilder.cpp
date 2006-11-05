
#include<common/io/InputSource.h>

#include<colorer/FileType.h>
#include<colorer/Region.h>

#include<colorer/parsers/StaticBuilder.h>

static Hashtable <int> reHash;
static Vector<String*> reVector;
static int reHashIndex = 1;

static void buildre()
{
    printf("char *relist[] = {\n");
    printf("NULL,\n");
    for(int idx = 0; idx < reVector.size(); idx++)
    {
        printf("\"%s\", //%04d\n", reVector.elementAt(idx)->getChars(Encodings::ENC_UTF8), idx+1);
    }
    printf("};\n");
}


void StaticBuilder::onStart()
{
    DynamicBuilder::onStart();
}


void StaticBuilder::onFinish()
{
    DynamicBuilder::onFinish();

    printf("int grammars[] = {\n");

    for (int idx = 0; idx < parserNodes->size(); idx++)
    {
        ParserNode *pn = parserNodes->elementAt(idx);
        
        printf("{%d, %d, %d, %d, %d}, //%d\n", pn->type, pn->target_node, pn->region_node, pn->start_node, pn->end_node, idx);

    }
    
    printf("};\n");
    
    buildre();

}

void StaticBuilder::schemeStart(String *schemeName, Scheme *scheme)
{
    DynamicBuilder::schemeStart(schemeName, scheme);
    // printf("char *schemeScheme start: %s\n", schemeName->getChars(Encodings::ENC_UTF8));
}

void StaticBuilder::schemeEnd(String *schemeName, Scheme *scheme)
{
    DynamicBuilder::schemeEnd(schemeName, scheme);
    // printf("Scheme end: %s\n", schemeName->getChars(Encodings::ENC_UTF8));
}

void StaticBuilder::visitRE(String *re, SchemeNode *node)
{
    DynamicBuilder::visitRE(re, node);

    int reindex = reHash.get(re);
    if (reindex == 0) {
        reindex = reHashIndex++;
        reHash.put(re, reindex);
        reVector.addElement(re);
    }
    // printf("  re %d\n", reindex);
}

void StaticBuilder::visitBlock(String *scheme, SchemeNode *node)
{
    DynamicBuilder::visitBlock(scheme, node);
    //printf("  block : // %s\n", scheme->getChars());
}
    
void StaticBuilder::visitKeywords(SchemeNode *node)
{
    DynamicBuilder::visitKeywords(node);
}

