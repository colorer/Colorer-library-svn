
#include<common/io/InputSource.h>

#include<colorer/FileType.h>
#include<colorer/Region.h>

#include<colorer/parsers/DynamicBuilder.h>

static Hashtable <int> schemeHash;

static Hashtable <int> reHash;
static Vector<String*> reVector;
static int reHashIndex = 1;



DynamicBuilder::DynamicBuilder()
{
    parserNodes = new DynamicArray<ParserNode>(10000);
    regionNodes = new DynamicArray<RegionNode>(10000);
    regionDescriptions = new DynamicArray<RegionDescription>(2000);
}

DynamicBuilder::~DynamicBuilder()
{
    delete parserNodes;
    delete regionNodes;
    delete regionDescriptions;
}

void DynamicBuilder::onStart()
{

}

void DynamicBuilder::onFinish()
{
    for (int idx = 0; idx < parserNodes->size(); idx++)
    {
        ParserNode *pn = parserNodes->elementAt(idx);
        if (pn->type == PN_BLOCK_DIRTY) {
            String *schemeName = (String*)pn->target_node;
            pn->target_node = schemeHash.get(schemeName) - 1;
            assert(pn->target_node != -1);
            // removes qualified name here
            delete schemeName;
            pn->type = PN_BLOCK;
        }
    }
}

void DynamicBuilder::schemeStart(String *schemeName, Scheme *scheme)
{
    schemeHash.put(schemeName, parserNodes->size() + 1);
}

void DynamicBuilder::schemeEnd(String *schemeName, Scheme *scheme)
{
    ParserNode *pn = parserNodes->addElement();
    pn->type = PN_SCHEME_END;
}

void DynamicBuilder::visitRE(String *re, SchemeNode *node)
{
    ParserNode *pn = parserNodes->addElement();
    pn->type = PN_RE;
    pn->target_node = 0;

    int reindex = reHash.get(re);
    if (reindex == 0) {
        reindex = reHashIndex++;
        reHash.put(re, reindex);
        reVector.addElement(re);
    }

    pn->start_node = reindex;

}

void DynamicBuilder::visitBlock(String *scheme, SchemeNode *node)
{
    ParserNode *pn = parserNodes->addElement();
    pn->type = PN_BLOCK_DIRTY;
    // scheme pointer should be deleted
    pn->target_node = (int)scheme;
    pn->start_node = 0;
    pn->end_node = 0;
    pn->region_node = 0;
}
    
void DynamicBuilder::visitKeywords(SchemeNode *node)
{
    ParserNode *pn = parserNodes->addElement();
    pn->type = PN_KW;
    pn->target_node = 0;
    pn->region_node = 0;
}
