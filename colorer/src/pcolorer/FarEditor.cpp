
#include<stdlib.h>
#include<unicode/UnicodeTools.h>
#include"FarEditor.h"

FarEditor::FarEditor(PluginStartupInfo *info, ParserFactory *pf){
  parserFactory = pf;
  baseEditor = new BaseEditor(parserFactory, this);

  this->info = info;
  info->EditorControl(ECTL_GETINFO, &ei);
  oldFAR = info->StructSize < sizeof(PluginStartupInfo);

  cursorRegion = null;
  unicodeEncodingIndex = -1;
  farEncodingIndex = -3;
  prevLinePosition = 0;
  blockTopPosition = -1;
  inRedraw = false;
  inHandler = false;
  ignoreChange = false;

  ret_str = null;
  ret_strNumber = -1;

  maxLineLength = 0;
  fullBackground = true;
  showHorizontalCross = true;
  showVerticalCross   = true;
  crossZOrder    = 0;
  horzCrossColor = 0x0E;
  vertCrossColor = 0x0E;
  newback = newfore = -1;
  rdBackground = null;
  visibleLevel = 100;

  const Region *def_Outlined = pf->getHRCParser()->getRegion(&DString("def:Outlined"));
  const Region *def_Error = pf->getHRCParser()->getRegion(&DString("def:Error"));
  structOutliner = new Outliner(def_Outlined);
  errorOutliner = new Outliner(def_Error);
  baseEditor->addRegionHandler(structOutliner);
  baseEditor->addRegionHandler(errorOutliner);
};

FarEditor::~FarEditor(){
  delete cursorRegion;
  delete baseEditor;
  delete structOutliner;
  delete errorOutliner;
};


void FarEditor::endJob(int lno){
  delete ret_str;
  ret_str = null;
};
String *FarEditor::getLine(int lno){
EditorGetString es;
int len = 0;
  if (ret_strNumber == lno && ret_str != null) return ret_str;
  ret_strNumber = lno;
  if (inHandler){
    EditorSetPosition esp;
    es.StringNumber = -1;
    es.StringText = null;
    esp.CurLine = lno;
    esp.CurPos = esp.CurTabPos = esp.TopScreenLine = esp.LeftPos = esp.Overtype = -1;
    info->EditorControl(ECTL_SETPOSITION, &esp);
    if (info->EditorControl(ECTL_GETSTRING, &es)) len = es.StringLength;
  }else{
    es.StringNumber = lno;
    es.StringText = null;
    if (info->EditorControl(ECTL_GETSTRING, &es)) len = es.StringLength;
  };
  if (len > maxLineLength && maxLineLength > 0) len = maxLineLength;
  delete ret_str;
  ret_str = new DString(es.StringText, 0, len, unicodeEncodingIndex);
  return ret_str;
};



void FarEditor::setFileType(FileType *ftype){
  baseEditor->setFileType(ftype);

  HRCParser *hrcParser = parserFactory->getHRCParser();
  FileType *def = hrcParser->getFileType(&DString("def"));
  if (def == null) throw Exception(DString("No 'def' file type found"));

  int backparse = 2000;
  UnicodeTools::getNumber(def->getParameter(DString("backparse")), &backparse);
  UnicodeTools::getNumber(def->getParameter(DString("maxlinelength")), &maxLineLength);
  UnicodeTools::getNumber(def->getParameter(DString("default-fore")), &newfore);
  UnicodeTools::getNumber(def->getParameter(DString("default-back")), &newback);
  const String *value;
  value = def->getParameter(DString("fullback"));
  if (value != null && value->equals("no")) fullBackground = false;
  value = def->getParameter(DString("show-cross"));
  if (value != null && value->equals("none")){
    showHorizontalCross = false;
    showVerticalCross   = false;
  };
  if (value != null && value->equals("vertical")){
    showHorizontalCross = false;
    showVerticalCross   = true;
  };
  if (value != null && value->equals("horizontal")){
    showHorizontalCross = true;
    showVerticalCross   = false;
  };
  if (value != null && value->equals("both")){
    showHorizontalCross = true;
    showVerticalCross   = true;
  };
  value = def->getParameter(DString("cross-zorder"));
  if (value != null && value->equals("top")) crossZOrder = 1;

  // installs custom file properties
  UnicodeTools::getNumber(ftype->getParameter(DString("backparse")), &backparse);
  UnicodeTools::getNumber(ftype->getParameter(DString("maxlinelength")), &maxLineLength);
  UnicodeTools::getNumber(ftype->getParameter(DString("default-fore")), &newfore);
  UnicodeTools::getNumber(ftype->getParameter(DString("default-back")), &newback);
  value = ftype->getParameter(DString("fullback"));
  if (value != null && value->equals("no")) fullBackground = false;
  value = ftype->getParameter(DString("show-cross"));
  if (value != null && value->equals("none")){
    showHorizontalCross = false;
    showVerticalCross   = false;
  };
  if (value != null && value->equals("vertical")){
    showHorizontalCross = false;
    showVerticalCross   = true;
  };
  if (value != null && value->equals("horizontal")){
    showHorizontalCross = true;
    showVerticalCross   = false;
  };
  if (value != null && value->equals("both")){
    showHorizontalCross = true;
    showVerticalCross   = true;
  };
  value = ftype->getParameter(DString("cross-zorder"));
  if (value != null && value->equals("top")) crossZOrder = 1;

  baseEditor->setBackParse(backparse);
};

FileType *FarEditor::getFileType(){
  return baseEditor->getFileType();
};

void FarEditor::setRegionMapper(RegionMapper *rs){
  baseEditor->setRegionMapper(rs);

  rdBackground = StyledRegion::cast(baseEditor->rd_def_Text);
  horzCrossColor = convert(StyledRegion::cast(baseEditor->rd_def_HorzCross));
  vertCrossColor = convert(StyledRegion::cast(baseEditor->rd_def_VertCross));
  if (horzCrossColor == 0) horzCrossColor = 0x0E;
  if (vertCrossColor == 0) vertCrossColor = 0x0E;
};




void FarEditor::matchPair()
{
EditorSetPosition esp;
  enterHandler();
  PairMatch *pm = baseEditor->getPairMatch(ei.CurLine, ei.CurPos);
  if (pm == null) return;
  baseEditor->searchGlobalPair(pm);
  leaveHandler();

  if (pm->eline == -1) return;

  esp.CurTabPos = -1;
  esp.LeftPos = -1;
  esp.Overtype = -1;
  esp.TopScreenLine = -1;
  esp.CurLine = pm->eline;
  esp.CurPos = pm->end->end-1;
  if (!pm->topPosition) esp.CurPos = pm->end->start;
  if (esp.CurLine < ei.TopScreenLine || esp.CurLine > ei.TopScreenLine+ei.WindowSizeY){
     esp.TopScreenLine = pm->eline - ei.WindowSizeY/2;
     if (esp.TopScreenLine < 0) esp.TopScreenLine = 0;
  };
  info->EditorControl(ECTL_SETPOSITION, &esp);
  baseEditor->releasePairMatch(pm);
  ignoreChange = true;
};

void FarEditor::selectPair(){
EditorSelect es;
int X1, X2, Y1, Y2;
  enterHandler();
  PairMatch *pm = baseEditor->getPairMatch(ei.CurLine, ei.CurPos);
  if (pm == null) return;
  baseEditor->searchGlobalPair(pm);
  leaveHandler();
  if (pm->eline == -1) return;

  X1 = pm->start->end;
  X2 = pm->end->start-1;
  Y1 = pm->sline;
  Y2 = pm->eline;
  if (!pm->topPosition){
    X2 = pm->start->start-1;
    X1 = pm->end->end;
    Y2 = pm->sline;
    Y1 = pm->eline;
  };
  es.BlockType = BTYPE_STREAM;
  es.BlockStartLine = Y1;
  es.BlockStartPos = X1;
  es.BlockHeight = Y2 - Y1 + 1;
  es.BlockWidth = X2 - X1 + 1;
  info->EditorControl(ECTL_SELECT, &es);

  baseEditor->releasePairMatch(pm);
  ignoreChange = true;
};
void FarEditor::selectBlock(){
EditorSelect es;
int X1, X2, Y1, Y2;
  enterHandler();
  PairMatch *pm = baseEditor->getPairMatch(ei.CurLine, ei.CurPos);
  if (pm == null) return;
  baseEditor->searchGlobalPair(pm);
  leaveHandler();

  if (pm->eline == -1) return;

  X1 = pm->start->start;
  X2 = pm->end->end-1;
  Y1 = pm->sline;
  Y2 = pm->eline;
  if (!pm->topPosition){
    X2 = pm->start->end-1;
    X1 = pm->end->start;
    Y2 = pm->sline;
    Y1 = pm->eline;
  };
  es.BlockType = BTYPE_STREAM;
  es.BlockStartLine = Y1;
  es.BlockStartPos = X1;
  es.BlockHeight = Y2 - Y1 + 1;
  es.BlockWidth = X2 - X1 + 1;
  info->EditorControl(ECTL_SELECT, &es);

  baseEditor->releasePairMatch(pm);
  ignoreChange = true;
};

void FarEditor::selectRegion(){
EditorSelect es;
EditorGetString egs;

  egs.StringNumber = ei.CurLine;
  info->EditorControl(ECTL_GETSTRING, &egs);
  int end = cursorRegion->end;
  if (end == -1) end = egs.StringLength;

  if (cursorRegion != null && end - cursorRegion->start > 0){
    es.BlockType = BTYPE_STREAM;
    es.BlockStartLine = ei.CurLine;
    es.BlockStartPos = cursorRegion->start;
    es.BlockHeight = 1;
    es.BlockWidth = end - cursorRegion->start;
    info->EditorControl(ECTL_SELECT, &es);
  };
  ignoreChange = true;
};

void FarEditor::listFunctions(){
  enterHandler();
  baseEditor->validate(-1);
  leaveHandler();
  showOutliner(structOutliner);
  ignoreChange = true;
};
void FarEditor::listErrors(){
  enterHandler();
  baseEditor->validate(-1);
  leaveHandler();
  showOutliner(errorOutliner);
  ignoreChange = true;
};

void FarEditor::updateHighlighting(){
  baseEditor->validate(ei.TopScreenLine);
};

void FarEditor::selectEncoding(){
  int ecount = 0;
  while(Encodings::getEncodingName(ecount) != null) ecount++;

  FarMenuItem *menuElements = new FarMenuItem[ecount];
  memset(menuElements, 0, sizeof(FarMenuItem)*ecount);

  for (int i = 0; i < ecount; i++){
    strcpy(menuElements[i].Text, Encodings::getEncodingName(i));
  };
  if (unicodeEncodingIndex < ecount) menuElements[unicodeEncodingIndex].Selected = 1;

  int result = info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE, GetMsg(mSelectEncoding), 0, "encodings",
                          NULL, NULL, menuElements, ecount);
  delete[] menuElements;
  if (result != -1){
    unicodeEncodingIndex = result;
    baseEditor->modifyEvent(0);
  };
  ignoreChange = true;
};

int FarEditor::editorEvent(int event, void *param)
{
  // ignore event
  if (event != EE_REDRAW || inRedraw) return 0;

  enterHandler();

  if (farEncodingIndex != ei.TableNum - ei.AnsiMode){
    static CharTableSet cts;
    farEncodingIndex = ei.TableNum - ei.AnsiMode;
    if (farEncodingIndex == -1) unicodeEncodingIndex = Encodings::getEncodingIndex("cp866");
    else if (farEncodingIndex == -2) unicodeEncodingIndex = Encodings::getEncodingIndex("cp1251");
    else{
      info->CharTable(farEncodingIndex, (char*)&cts, sizeof(cts));
      unicodeEncodingIndex = Encodings::getEncodingIndex(cts.TableName);
    };
    if (unicodeEncodingIndex == -1) unicodeEncodingIndex = Encodings::getDefaultEncodingIndex();
    // Here we have to invalidate full editor content
    baseEditor->modifyEvent(0);
  };
  if (WindowSizeX != ei.WindowSizeX || WindowSizeY != ei.WindowSizeY){
    WindowSizeX = ei.WindowSizeX;
    WindowSizeY = ei.WindowSizeY;
  };

  baseEditor->visibleTextEvent(ei.TopScreenLine, WindowSizeY);
  baseEditor->lineCountEvent(ei.TotalLines);

  // Hack against FAR's bad EEREDRAW_CHANGE events
  if (param == EEREDRAW_CHANGE && ignoreChange == true){
    param = EEREDRAW_ALL;
    ignoreChange = false;
  };

  if (oldFAR || param == EEREDRAW_CHANGE){
    int ml = (prevLinePosition < ei.CurLine ? prevLinePosition : ei.CurLine)-1;
    if (ml < 0) ml = 0;
    if (blockTopPosition != -1 && ml > blockTopPosition) ml = blockTopPosition;
    baseEditor->modifyEvent(ml);
  };
  prevLinePosition = ei.CurLine;
  blockTopPosition = -1;
  if (ei.BlockType != BTYPE_NONE) blockTopPosition = ei.BlockStartLine;

  // hack against tabs in FAR's editor
  EditorConvertPos ecp, ecp_cl;

  ecp.StringNumber = -1;
  ecp.SrcPos = ei.CurPos;
  info->EditorControl(ECTL_REALTOTAB, &ecp);

  delete cursorRegion;
  cursorRegion = null;

  if (rdBackground == null) throw Exception(DString("HRD Background region 'def:Text' not found"));

  for(int lno = ei.TopScreenLine; lno < ei.TopScreenLine + WindowSizeY; lno++){
    if (lno >= ei.TotalLines) break;
    LineRegion *l1 = baseEditor->getLineRegions(lno);

    addFARColor(lno, -1, 0, 0);

    EditorGetString egs;
    egs.StringNumber = lno;
    info->EditorControl(ECTL_GETSTRING, &egs);
    int llen = egs.StringLength;
    const char *lptr = egs.StringText;

    // fills back
    if (lno == ei.CurLine && showHorizontalCross)
      addFARColor(lno, 0, ei.LeftPos + ei.WindowSizeX, horzCrossColor);
    else
      addFARColor(lno, 0, ei.LeftPos + ei.WindowSizeX, convert(null));

    if (showVerticalCross){
      ecp_cl.StringNumber = lno;
      ecp_cl.SrcPos = ecp.DestPos;
      info->EditorControl(ECTL_TABTOREAL, &ecp_cl);
      addFARColor(lno, ecp_cl.DestPos, ecp_cl.DestPos+1, vertCrossColor);
    };
    bool vertCrossDone = false;

    int syns = 0;
    int syne = 0;
    for(; l1; l1 = l1->next){
      if (l1->special) continue;
      if (l1->start == l1->end) continue;
      if (l1->start > ei.LeftPos+ei.WindowSizeX) continue;
      if (l1->end != -1 && l1->end < ei.LeftPos) continue;

      if ((lno != ei.CurLine || !showHorizontalCross || crossZOrder == 0)){
        int color = convert(l1->styled());
        if (lno == ei.CurLine && showHorizontalCross){
          if (foreDefault(color)) color = (color&0xF0) + (horzCrossColor&0xF);
          if (backDefault(color)) color = (color&0xF) + (horzCrossColor&0xF0);
        };
        if (!color) continue;

        int lend = l1->end;
        if (lend == -1) lend = fullBackground ? ei.LeftPos+ei.WindowSizeX : llen;
        addFARColor(lno, l1->start, lend, color);

        if (lno == ei.CurLine && (l1->start <= ei.CurPos) && (ei.CurPos <= lend)){
          delete cursorRegion;
          cursorRegion = new LineRegion(*l1);
        };
        // column
        if (showVerticalCross && crossZOrder == 0 && l1->start <= ecp_cl.DestPos && ecp_cl.DestPos < lend) {
          color = convert(l1->styled());
          if (foreDefault(color)) color = (color&0xF0) + (vertCrossColor&0xF);
          if (backDefault(color)) color = (color&0xF) + (vertCrossColor&0xF0);
          ecp_cl.StringNumber = lno;
          ecp_cl.SrcPos = ecp.DestPos;
          info->EditorControl(ECTL_TABTOREAL, &ecp_cl);
          addFARColor(lno, ecp_cl.DestPos, ecp_cl.DestPos+1, color);
          vertCrossDone = true;
        };

      };
    };

    if (showVerticalCross && !vertCrossDone){
      ecp_cl.StringNumber = lno;
      ecp_cl.SrcPos = ecp.DestPos;
      info->EditorControl(ECTL_TABTOREAL, &ecp_cl);
      addFARColor(lno, ecp_cl.DestPos, ecp_cl.DestPos+1, vertCrossColor);
    };
  };


  /// pair brackets
  PairMatch *pm = baseEditor->getPairMatch(ei.CurLine, ei.CurPos);
  if (pm != null){
    int color = convert(pm->start->styled());
    if (showHorizontalCross){
      if (foreDefault(color)) color = (color&0xF0) + (horzCrossColor&0xF);
      if (backDefault(color)) color = (color&0xF) + (horzCrossColor&0xF0);
    };
    addFARColor(ei.CurLine, pm->start->start, pm->start->end, color);
    if (showVerticalCross && !showHorizontalCross && pm->start->start <= ei.CurPos && ei.CurPos < pm->start->end){
      color = convert(pm->start->styled());
      if (foreDefault(color)) color = (color&0xF0) + (vertCrossColor&0xF);
      if (backDefault(color)) color = (color&0xF) + (vertCrossColor&0xF0);
      addFARColor(pm->sline, ei.CurPos, ei.CurPos+1, color);
    };

    baseEditor->searchLocalPair(pm);

    if (pm->eline != -1){
      color = convert(pm->end->styled());
      if (showHorizontalCross && pm->eline == ei.CurLine){
        if (foreDefault(color)) color = (color&0xF0) + (horzCrossColor&0xF);
        if (backDefault(color)) color = (color&0xF) + (horzCrossColor&0xF0);
      };
      addFARColor(pm->eline, pm->end->start, pm->end->end, color);

      ecp.StringNumber = pm->eline;
      ecp.SrcPos = ecp.DestPos;
      info->EditorControl(ECTL_TABTOREAL, &ecp);
      if (showVerticalCross && pm->end->start <= ecp.DestPos && ecp.DestPos < pm->end->end) {
        color = convert(pm->end->styled());
        if (foreDefault(color)) color = (color&0xF0) + (vertCrossColor&0xF);
        if (backDefault(color)) color = (color&0xF) + (vertCrossColor&0xF0);
        addFARColor(pm->eline, ecp.DestPos, ecp.DestPos+1, color);
      };
    };
    baseEditor->releasePairMatch(pm);
  };

  leaveHandler();

  if (param != EEREDRAW_ALL){
    inRedraw = true;
    info->EditorControl(ECTL_REDRAW, EEREDRAW_ALL);
    inRedraw = false;
  };
  return true;
}

/*
void FarEditor::showOutliner(Outliner *outliner)
{
FarMenuItem *menu;

  int outputEnc = Encodings::getEncodingIndex("cp866");
  menu = new FarMenuItem[outliner->itemCount()];

  for(int i = 0; i < outliner->itemCount(); i++){
    OutlineItem *item = outliner->getItem(i);
    char *item_chars = item->token->getChars(outputEnc);
    int item_length = strlen(item_chars);
    if (item_length > 100) item_length = 100;
    int si = sprintf(menu[i].Text, "%3d: ",item->lno);
    strncpy(menu[i].Text+si, item_chars, item_length);
    (menu[i].Text+si)[item_length] = 0;
    *(OutlineItem**)(&menu[i].Text[124]) = item;
    // position on nearest top function
    if (!itemSelected && ei.CurLine >= item->lno && (i+1 == items_num || ei.CurLine < outliner->getItem(i+1)->lno)){
      menu[menu_size].Selected = 1;
      itemSelected = true;
    };
  };
  showMenu();
  delete menu;
}
*/

void FarEditor::showOutliner(Outliner *outliner)
{
FarMenuItem *menu;
EditorSetPosition esp;
bool moved = false;
int i, code = 0;

const int FILTER_SIZE = 40;

  int breakKeys[] =
  {
    VK_BACK, VK_RETURN, 0xBA, 0xBD, VK_TAB,
    (PKF_CONTROL<<16)+VK_UP, (PKF_CONTROL<<16)+VK_DOWN,
    (PKF_CONTROL<<16)+VK_LEFT, (PKF_CONTROL<<16)+VK_RIGHT,
    '0','1','2','3','4','5','6','7','8','9',
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z',' ', 0,
  };

  int keys_size = 0;
  while (breakKeys[keys_size] !=0) keys_size++;

  int outputEnc = Encodings::getEncodingIndex("cp866");

  char prefix[FILTER_SIZE+1];
  char autofilter[FILTER_SIZE+1];
  char filter[FILTER_SIZE+1];
  int  flen = 0, autoIncremented = 0;
  *filter = 0;

  int maxLevel = -1;

  bool stopMenu = false;
  int items_num = outliner->itemCount();
  if (items_num == 0) stopMenu = true;
  menu = new FarMenuItem[items_num];

  while (!stopMenu){
    memset(menu, 0, sizeof(FarMenuItem)*items_num);

    // items in FAR's menu;
    int menu_size = 0;
    int selectedItem = 0;

    Vector<int> treeStack;
    for(i = 0; i < items_num; i++){
      OutlineItem *item = outliner->getItem(i);
      if (item->token->indexOfIgnoreCase(DString(filter)) != -1){

        int treeLevel = Outliner::manageTree(treeStack, item->level);
        if (maxLevel < treeLevel) maxLevel = treeLevel;
        if (treeLevel > visibleLevel) continue;

        int si = sprintf(menu[menu_size].Text, "%4d ", item->lno);
        for(int lIdx = 0; lIdx < treeLevel; lIdx++){
          menu[menu_size].Text[si++] = ' ';
          menu[menu_size].Text[si++] = ' ';
        };
        const String *region = item->region->getName();
        char cls = (char)Character::toLowerCase((*region)[region->indexOf(':')+1]);
        si += sprintf(menu[menu_size].Text+si, "%c ", cls);

        int labelLength = item->token->length();
        if (labelLength+si > 110) labelLength = 110;
        strncpy(menu[menu_size].Text+si, item->token->getChars(outputEnc), labelLength);
        menu[menu_size].Text[si+labelLength] = 0;

        *(OutlineItem**)(&menu[menu_size].Text[124]) = item;
        // set position on nearest top function

        if (ei.CurLine >= item->lno) selectedItem = menu_size;
        menu_size++;
      };
    }
    if (selectedItem > 0) menu[selectedItem].Selected = 1;

    if (menu_size == 0 && flen > 0){
      flen--;
      filter[flen] = 0;
      continue;
    }

    int aflen = flen;
    // Find same function prefix
    bool same = true;
    int plen = 0;
    strcpy(autofilter, filter);

    while (code != 0 && menu_size > 1 && same && plen < FILTER_SIZE){
      plen = aflen + 1;
      int auto_ptr  = DString(menu[0].Text).indexOfIgnoreCase(DString(autofilter));
      if (int(strlen(menu[0].Text)-auto_ptr) < plen) break;
      strncpy(prefix, menu[0].Text+auto_ptr, plen);
      prefix[plen] = 0;
      for (int j = 1 ; j < menu_size ; j++ ){
        if (DString(menu[j].Text).indexOfIgnoreCase(DString(prefix)) == -1){
          same = false;
          break;
        }
      }
      if (same){
        aflen++;
        strcpy(autofilter, prefix);
      }
    }
    autoIncremented = strlen(autofilter) - strlen(filter);


    char top[128];
    const char *topline = GetMsg(mOutliner);
    char captionfilter[FILTER_SIZE+1];
    strncpy(captionfilter, filter, flen);
    captionfilter[flen] = 0;
    if (aflen > flen){
      strcat(captionfilter, "?");
      strncat(captionfilter, autofilter+flen, aflen-flen);
      captionfilter[aflen+1] = 0;
    }
    sprintf(top, topline, captionfilter);

    int sel = 0;
    sel = info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_SHOWAMPERSAND|FMENU_WRAPMODE,
                  top, GetMsg(mChoose), "add", breakKeys, &code, menu, menu_size);

    switch (code){
      case -1:
        stopMenu = true;
        break;
      case 0: // VK_BACK
        if (flen > 0) flen--;
        filter[flen] = 0;
        break;
      case 1:{ // VK_RETURN
        if (menu_size == 0) break;
        esp.CurTabPos = esp.LeftPos = esp.Overtype = esp.TopScreenLine = -1;
        OutlineItem *item = *(OutlineItem**)(&menu[sel].Text[124]);
        esp.CurLine = item->lno;
        esp.CurPos = item->pos;
        esp.TopScreenLine = esp.CurLine - ei.WindowSizeY/2;
        if (esp.TopScreenLine < 0) esp.TopScreenLine = 0;
        info->EditorControl(ECTL_SETPOSITION, &esp);
        stopMenu = true;
        moved = true;
        break;
      }
      case 2: // :
        if (flen == FILTER_SIZE) break;
        filter[flen]= ':';
        filter[++flen]= 0;
        break;

      case 3: // _
        if (flen == FILTER_SIZE) break;
        filter[flen]= '_';
        filter[++flen]= 0;
        break;

      case 4: // VK_TAB
        strcpy(filter, autofilter);
        flen = aflen;
        break;

      case 5:{ // ctrl-up
        if (menu_size == 0) break;
        if (sel == 0) sel = menu_size-1;
        else sel--;
        esp.CurTabPos = esp.LeftPos = esp.Overtype = esp.TopScreenLine = -1;
        OutlineItem *item = *(OutlineItem**)(&menu[sel].Text[124]);
        esp.CurLine = item->lno;
        esp.CurPos = item->pos;
        esp.TopScreenLine = esp.CurLine - ei.WindowSizeY/2;
        if (esp.TopScreenLine < 0) esp.TopScreenLine = 0;
        info->EditorControl(ECTL_SETPOSITION, &esp);
        info->EditorControl(ECTL_REDRAW, EEREDRAW_ALL);
        info->EditorControl(ECTL_GETINFO, &ei);
        break;
      }
      case 6:{ // ctrl-down
        if (menu_size == 0) break;
        if (sel == menu_size-1) sel = 0;
        else sel++;
        esp.CurTabPos = esp.LeftPos = esp.Overtype = esp.TopScreenLine = -1;
        OutlineItem *item = *(OutlineItem**)(&menu[sel].Text[124]);
        esp.CurLine = item->lno;
        esp.CurPos = item->pos;
        esp.TopScreenLine = esp.CurLine - ei.WindowSizeY/2;
        if (esp.TopScreenLine < 0) esp.TopScreenLine = 0;
        info->EditorControl(ECTL_SETPOSITION, &esp);
        info->EditorControl(ECTL_REDRAW, EEREDRAW_ALL);
        info->EditorControl(ECTL_GETINFO, &ei);
        break;
      }
      case 7:{ // ctrl-left
        if (visibleLevel > maxLevel) visibleLevel = maxLevel-1;
        else if (visibleLevel > 0) visibleLevel--;
        if (visibleLevel < 0) visibleLevel = 0;
        break;
      }
      case 8:{ // ctrl-right
        visibleLevel++;
        break;
      }
      default:
        if (flen == FILTER_SIZE || code > keys_size) break;
        filter[flen] = (char)Character::toLowerCase(breakKeys[code]);
        filter[++flen] = 0;
        break;
    };
  };
  delete[] menu;
  if (!moved){
    // restoring position
    esp.CurLine = ei.CurLine;
    esp.CurPos = ei.CurPos;
    esp.CurTabPos = ei.CurTabPos;
    esp.TopScreenLine = ei.TopScreenLine;
    esp.LeftPos = ei.LeftPos;
    esp.Overtype = ei.Overtype;
    info->EditorControl(ECTL_SETPOSITION, &esp);
  };
  if (items_num == 0){
    const char *msg[2] = { GetMsg(mNothingFound), GetMsg(mGotcha) };
    info->Message(info->ModuleNumber, 0, 0, msg, 2, 1);
  };
}

void FarEditor::enterHandler(){
  inHandler = true;
  info->EditorControl(ECTL_GETINFO, &ei);
  ret_strNumber = -1;
};
void FarEditor::leaveHandler(){
  // restoring position
  EditorSetPosition esp;
  esp.CurLine = ei.CurLine;
  esp.CurPos = ei.CurPos;
  esp.TopScreenLine = ei.TopScreenLine;
  esp.LeftPos = ei.LeftPos;
  esp.CurTabPos = -1;
  esp.Overtype = -1;
  info->EditorControl(ECTL_SETPOSITION, &esp);
  inHandler = false;
};

int FarEditor::convert(const StyledRegion *rd){
  int color;
  if (rdBackground == null) return 0;
  if (rd != null) color = rd->fore + (rd->back << 4);
  int fore = (newfore != -1)?newfore:rdBackground->fore;
  int back = (newback != -1)?newback:rdBackground->back;
  if (rd == null || !rd->bfore) color = (color&0xF0) + fore;
  if (rd == null || !rd->bback) color = (color&0xF) + (back << 4);
  return color;
}

bool FarEditor::foreDefault(int color){
  return (color&0xF) == rdBackground->fore;
}

bool FarEditor::backDefault(int color){
  return (color>>4) == rdBackground->back;
}

void FarEditor::addFARColor(int lno, int s, int e, int col)
{
EditorColor ec;
  ec.StringNumber = lno;
  ec.StartPos = s;
  ec.EndPos = e-1;
  ec.Color = col;
  info->EditorControl(ECTL_ADDCOLOR, &ec);
};

const char *FarEditor::GetMsg(int msg){
  return(info->GetMsg(info->ModuleNumber, msg));
};

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Colorer Library.
 *
 * The Initial Developer of the Original Code is
 * Cail Lomecb <cail@nm.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2003
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */