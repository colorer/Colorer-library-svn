
#include<common/io/InputSource.h>

#include<colorer/FileType.h>
#include<colorer/Region.h>

#include<colorer/parsers/StaticBuilder.h>

void StaticBuilder::buildRE()
{
    printf("char *relist[] = {\n");
    printf("NULL,\n");
    for(int idx = 0; idx < reVector.size(); idx++)
    {
        printf("\"%s\", //%04d\n", reVector.elementAt(idx)->getChars(Encodings::ENC_UTF8), idx+1);
    }
    printf("};\n");
}

void StaticBuilder::buildRegionNodes()
{
    printf("\n\nregionNodes = {\n\n");
    for(int idx = 0; idx < regionNodes->size(); idx++)
    {
        printf("  %d, //%d\n", regionNodes->elementAt(idx)->region, idx);
    }
    printf("}\n\n");
}

void StaticBuilder::buildRegions()
{
    printf("\n\nregionIDs = {\n\n");
    for(int idx = 0; ; idx++)
    {
        const Region *r = hrcParser->getRegion(idx);
        if (!r) break;
        printf(" %s, // %d\n", r->getName()->getChars(), idx);
    }
    printf("}\n\n");
}

void StaticBuilder::onStart(HRCParser *hrcParser)
{
    this->hrcParser = hrcParser;

    DynamicBuilder::onStart(hrcParser);
}

char *pn_types[] = {
    "PN_RE",
    "PN_KW",
    "PN_BLOCK",
    "PN_BLOCK_DIRTY",
    "PN_SCHEME_END",
};

void StaticBuilder::onFinish()
{
    DynamicBuilder::onFinish();

    printf("int grammars[] = {\n");

    for (int idx = 0; idx < parserNodes->size(); idx++)
    {
        ParserNode *pn = parserNodes->elementAt(idx);
        
        printf("{%16s, %05d, %05d, %05d, %05d}, //%d\n", pn_types[pn->type], pn->target_node, pn->region_node, pn->start_node, pn->end_node, idx);

    }
    
    printf("};\n");
    
    buildRE();

    buildRegionNodes();

    buildRegions();

    printf("\n\nTOTAL: %d\n", parserNodes->size()*sizeof(ParserNode) + reVector.size()*50 + regionNodes->size() * sizeof(RegionNode) );

}

void StaticBuilder::schemeStart(String *schemeName, Scheme *scheme)
{
    DynamicBuilder::schemeStart(schemeName, scheme);
}

void StaticBuilder::schemeEnd(String *schemeName, Scheme *scheme)
{
    DynamicBuilder::schemeEnd(schemeName, scheme);
}

void StaticBuilder::visitRE(String *re, SchemeNode *node)
{
    DynamicBuilder::visitRE(re, node);
}

void StaticBuilder::visitBlock(String *scheme, SchemeNode *node)
{
    DynamicBuilder::visitBlock(scheme, node);
}
    
void StaticBuilder::visitKeywords(SchemeNode *node)
{
    DynamicBuilder::visitKeywords(node);
}

