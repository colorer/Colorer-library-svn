
#include<common/io/InputSource.h>

#include<colorer/FileType.h>
#include<colorer/Region.h>

#include<colorer/parsers/DynamicBuilder.h>

DynamicBuilder::DynamicBuilder()
{
    parserNodes = new DynamicArray<ParserNode>(10000);
    regionNodes = new DynamicArray<RegionNode>(10000);
//    regionDescriptions = new DynamicArray<RegionDescription>(2000);
}

DynamicBuilder::~DynamicBuilder()
{
    delete parserNodes;
    delete regionNodes;
//    delete regionDescriptions;
}

void DynamicBuilder::onStart(HRCParser *hrcParser)
{
    reHashIndex = 1;
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
    pn->target_node = pn->region_node = pn->end_node = pn->start_node = 0;
}

void DynamicBuilder::visitRE(String *re, SchemeNode *node)
{
    ParserNode *pn = parserNodes->addElement();
    pn->type = PN_RE;
    pn->target_node = 0;
    pn->end_node = 0;

    pn->start_node = getREindex(re);
    pn->region_node = getRegionIndex(node);

}

void DynamicBuilder::visitBlock(String *scheme, SchemeNode *node)
{
    ParserNode *pn = parserNodes->addElement();
    pn->type = PN_BLOCK_DIRTY;
    // scheme pointer should be deleted
    pn->target_node = (int)scheme;
    pn->start_node = getREindex(node->start_re);
    pn->end_node = getREindex(node->end_re);
    pn->region_node = getRegionIndex(node);
}
    
void DynamicBuilder::visitKeywords(SchemeNode *node)
{
    ParserNode *pn = parserNodes->addElement();
    pn->type = PN_KW;
    pn->target_node = 0;
    pn->region_node = getRegionIndex(node);
    pn->end_node = pn->start_node = 0;
}


int DynamicBuilder::getREindex(String *re)
{
    if (re == null) return -1;

    int reindex = reHash.get(re);
    if (reindex == 0) {
        reindex = reHashIndex++;
        reHash.put(re, reindex);
        reVector.addElement(re);
    }
    return reindex;
}

bool requals(RegionNode *rnode, SchemeNode *node)
{
    return rnode->region == (node->region ? node->region->getID() : 0);
}

int DynamicBuilder::getRegionIndex(SchemeNode *node)
{
    //search
    for (int idx = 0; idx < regionNodes->size(); idx++) {
        if (requals(regionNodes->elementAt(idx), node)){
            return idx;
        }
    }
    
    // not found
    RegionNode *el = regionNodes->addElement();
    
    el->region = node->region ? node->region->getID() : 0;
    
    //el->regions = node->regions;
    //el->regionsn = node->regionsn;
    //el->regione = node->regione;
    //el->regionen = node->regionen;
    
    return regionNodes->size();

}
