#include"FarEditor.h"

FarEditor::FarEditor(PluginStartupInfo *info, ParserFactory *pf)
{
  parserFactory = pf;
  baseEditor = new BaseEditor(parserFactory, this);
  this->info = info;
  info->EditorControl(CurrentEditor, ECTL_GETINFO, NULL, &ei);
  cursorRegion = NULL;
  prevLinePosition = 0;
  blockTopPosition = -1;
  inRedraw = false;
  idleCount = 0;
  ret_str = NULL;
  ret_strNumber = -1;
  maxLineLength = 0;
  fullBackground = true;
  drawCross = 2;
  showHorizontalCross = showVerticalCross = false;
  crossZOrder    = 0;
  horzCrossColor = color(); //0x0E;
  vertCrossColor = color(); //0x0E;
  drawPairs = drawSyntax = true;
  oldOutline = false;
  newback = newfore = -1;
  rdBackground = NULL;
  visibleLevel = 100;
  const Region *def_Outlined = pf->getHRCParser()->getRegion(&DString("def:Outlined"));
  const Region *def_Error = pf->getHRCParser()->getRegion(&DString("def:Error"));
  structOutliner = new Outliner(baseEditor, def_Outlined);
  errorOutliner = new Outliner(baseEditor, def_Error);
  TrueMod=true;
}

FarEditor::~FarEditor()
{
  delete cursorRegion;
  delete structOutliner;
  delete errorOutliner;
  delete baseEditor;
  delete ret_str;
}

void FarEditor::endJob(int lno)
{
  delete ret_str;
  ret_str = NULL;
}

String *FarEditor::getLine(int lno)
{
  if (ret_strNumber == lno && ret_str != NULL){
    return ret_str;
  }

  EditorGetString es;
  int len = 0;
  ret_strNumber = lno;

  es.StringNumber = lno;
  es.StringText = NULL;

  if (info->EditorControl(CurrentEditor, ECTL_GETSTRING, NULL, &es)){
    len = es.StringLength;
  }

  if (len > maxLineLength && maxLineLength > 0){
    len = maxLineLength;
  }

  delete ret_str;
  ret_str = new DString(es.StringText, 0, len);
  return ret_str;
}

void FarEditor::chooseFileType(String *fname)
{
  FileType *ftype = baseEditor->chooseFileType(fname);
  setFileType(ftype);
}

void FarEditor::setFileType(FileType *ftype)
{
  baseEditor->setFileType(ftype);
  reloadTypeSettings();
}

void FarEditor::reloadTypeSettings()
{
  FileType *ftype = baseEditor->getFileType();
  HRCParser *hrcParser = parserFactory->getHRCParser();
  FileType *def = hrcParser->getFileType(&DString("default"));

  if (def == NULL){
    throw Exception(DString("No 'default' file type found"));
  }

  int backparse = def->getParamValueInt(DBackparse, 2000);
  maxLineLength = def->getParamValueInt(DMaxLen, 0);
  newfore = def->getParamValueInt(DDefFore, -1);
  newback = def->getParamValueInt(DDefBack, -1);
  const String *value;
  value = def->getParamValue(DFullback);

  if (value != NULL && value->equals(&DNo)){
    fullBackground = false;
  }

  value = def->getParamValue(DShowCross);
  if (drawCross==2 && value != NULL){
    if (value->equals(&DNone)){
      showHorizontalCross = false;
      showVerticalCross   = false;
    };

    if (value->equals(&DVertical)){
      showHorizontalCross = false;
      showVerticalCross   = true;
    };

    if (value->equals(&DHorizontal)){
      showHorizontalCross = true;
      showVerticalCross   = false;
    };

    if (value->equals(&DBoth)){
      showHorizontalCross = true;
      showVerticalCross   = true;
    };
  }

  value = def->getParamValue(DCrossZorder);

  if (value != NULL && value->equals(&DTop)){
    crossZOrder = 1;
  }

  // installs custom file properties
  backparse = ftype->getParamValueInt(DBackparse, backparse);
  maxLineLength = ftype->getParamValueInt(DMaxLen, maxLineLength);
  newfore = ftype->getParamValueInt(DDefFore, newfore);
  newback = ftype->getParamValueInt(DDefBack, newback);
  value = ftype->getParamValue(DFullback);

  if (value != NULL && value->equals(&DNo)){
    fullBackground = false;
  }

  value = ftype->getParamValue(DShowCross);

  if (drawCross==2 && value != NULL){
    if (value->equals(&DNone)){
      showHorizontalCross = false;
      showVerticalCross   = false;
    };

    if (value->equals(&DVertical)){
      showHorizontalCross = false;
      showVerticalCross   = true;
    };

    if (value->equals(&DHorizontal)){
      showHorizontalCross = true;
      showVerticalCross   = false;
    };

    if (value->equals(&DBoth)){
      showHorizontalCross = true;
      showVerticalCross   = true;
    };
  }

  value = ftype->getParamValue(DCrossZorder);

  if (value != NULL && value->equals(&DTop)){
    crossZOrder = 1;
  }

  baseEditor->setBackParse(backparse);
}

FileType *FarEditor::getFileType()
{
  return baseEditor->getFileType();
}

void FarEditor::setDrawCross(int _drawCross)
{
  drawCross=_drawCross;
  switch (drawCross){
  case 0:
    showHorizontalCross = false;
    showVerticalCross   = false;
    break;
  case 1:
    showHorizontalCross = true;
    showVerticalCross   = true;
    break;
  case 2:
    reloadTypeSettings();
    break;
  }
}

void FarEditor::setDrawPairs(bool drawPairs)
{
  this->drawPairs = drawPairs;
}

void FarEditor::setDrawSyntax(bool drawSyntax)
{
  this->drawSyntax = drawSyntax;
}

void FarEditor::setOutlineStyle(bool oldStyle)
{
  this->oldOutline = oldStyle;
}

void FarEditor::setTrueMod(bool _TrueMod)
{
  this->TrueMod = _TrueMod;
}

void FarEditor::setRegionMapper(RegionMapper *rs)
{
  baseEditor->setRegionMapper(rs);
  rdBackground = StyledRegion::cast(baseEditor->rd_def_Text);
  horzCrossColor = convert(StyledRegion::cast(baseEditor->rd_def_HorzCross));
  vertCrossColor = convert(StyledRegion::cast(baseEditor->rd_def_VertCross));

  //TODO
  if (horzCrossColor.concolor == 0) horzCrossColor.concolor = 0x0E;
  if (vertCrossColor.concolor == 0) vertCrossColor.concolor = 0x0E;
}

void FarEditor::matchPair()
{
  EditorSetPosition esp;
  enterHandler();
  PairMatch *pm = baseEditor->searchGlobalPair(ei.CurLine, ei.CurPos);

  if ((pm == NULL)||(pm->eline == -1)){
    baseEditor->releasePairMatch(pm);
    return;
  }

  esp.CurTabPos = -1;
  esp.LeftPos = -1;
  esp.Overtype = -1;
  esp.TopScreenLine = -1;
  esp.CurLine = pm->eline;

  if (!pm->topPosition){
    esp.CurPos = pm->end->start;
  }
  else{
    esp.CurPos = pm->end->end-1;
  }

  if (esp.CurLine < ei.TopScreenLine || esp.CurLine > ei.TopScreenLine+ei.WindowSizeY){
    esp.TopScreenLine = pm->eline - ei.WindowSizeY/2;

    if (esp.TopScreenLine < 0){
      esp.TopScreenLine = 0;
    }
  };

  info->EditorControl(CurrentEditor, ECTL_SETPOSITION, NULL, &esp);
  baseEditor->releasePairMatch(pm);
}

void FarEditor::selectPair()
{
  EditorSelect es;
  int X1, X2, Y1, Y2;
  enterHandler();
  PairMatch *pm = baseEditor->searchGlobalPair(ei.CurLine, ei.CurPos);

  if ((pm == NULL)||(pm->eline == -1)){
    baseEditor->releasePairMatch(pm);
    return;
  }

  if (pm->topPosition){
    X1 = pm->start->end;
    X2 = pm->end->start-1;
    Y1 = pm->sline;
    Y2 = pm->eline;
  }
  else{
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

  info->EditorControl(CurrentEditor, ECTL_SELECT, NULL, &es);

  baseEditor->releasePairMatch(pm);
}

void FarEditor::selectBlock()
{
  EditorSelect es;
  int X1, X2, Y1, Y2;
  enterHandler();
  PairMatch *pm = baseEditor->searchGlobalPair(ei.CurLine, ei.CurPos);

  if ((pm == NULL)||(pm->eline == -1)){
    baseEditor->releasePairMatch(pm);
    return;
  }

  if (pm->topPosition){
    X1 = pm->start->start;
    X2 = pm->end->end-1;
    Y1 = pm->sline;
    Y2 = pm->eline;
  }
  else{
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

  info->EditorControl(CurrentEditor, ECTL_SELECT, NULL, &es);

  baseEditor->releasePairMatch(pm);
}

void FarEditor::selectRegion()
{
  EditorSelect es;
  EditorGetString egs;
  enterHandler();
  egs.StringNumber = ei.CurLine;
  info->EditorControl(CurrentEditor, ECTL_GETSTRING, NULL, &egs);
  if (cursorRegion != NULL){
    int end = cursorRegion->end;

    if (end == -1){
      end = egs.StringLength;
    }

    if (end - cursorRegion->start > 0){
      es.BlockType = BTYPE_STREAM;
      es.BlockStartLine = ei.CurLine;
      es.BlockStartPos = cursorRegion->start;
      es.BlockHeight = 1;
      es.BlockWidth = end - cursorRegion->start;
      info->EditorControl(CurrentEditor, ECTL_SELECT, NULL, &es);
    };
  }
}

void FarEditor::listFunctions()
{
  baseEditor->validate(-1, false);
  showOutliner(structOutliner);
}

void FarEditor::listErrors()
{
  baseEditor->validate(-1, false);
  showOutliner(errorOutliner);
}


void FarEditor::locateFunction()
{
  // extract word
  enterHandler();
  String &curLine = *getLine(ei.CurLine);
  int cpos = ei.CurPos;
  int sword = cpos;
  int eword = cpos;

  while (cpos < curLine.length() &&
    (Character::isLetterOrDigit(curLine[cpos]) || curLine[cpos] != '_')){
      while (Character::isLetterOrDigit(curLine[eword]) || curLine[eword] == '_'){
        if (eword == curLine.length()-1){
          break;
        }

        eword++;
      }

      while (Character::isLetterOrDigit(curLine[sword]) || curLine[sword] == '_'){
        if (sword == 0){
          break;
        }

        sword--;
      }

      SString funcname(curLine, sword+1, eword-sword-1);
      CLR_INFO("FC", "Letter %s", funcname.getChars());
      baseEditor->validate(-1, false);
      EditorSetPosition esp;
      OutlineItem *item_found = NULL;
      OutlineItem *item_last = NULL;
      int items_num = structOutliner->itemCount();

      if (items_num == 0){
        break;
      }

      //search through the outliner
      for (int idx = 0; idx < items_num; idx++){
        OutlineItem *item = structOutliner->getItem(idx);

        if (item->token->indexOfIgnoreCase(DString(funcname)) != -1){
          if (item->lno == ei.CurLine){
            item_last = item;
          }
          else{
            item_found = item;
          }
        }
      }

      if (!item_found){
        item_found = item_last;
      }

      if (!item_found){
        break;
      }

      esp.CurTabPos = esp.LeftPos = esp.Overtype = esp.TopScreenLine = -1;
      esp.CurLine = item_found->lno;
      esp.CurPos = item_found->pos;
      esp.TopScreenLine = esp.CurLine - ei.WindowSizeY/2;

      if (esp.TopScreenLine < 0){
        esp.TopScreenLine = 0;
      }

      info->EditorControl(CurrentEditor, ECTL_SETPOSITION, NULL, &esp);
      info->EditorControl(CurrentEditor, ECTL_REDRAW, NULL, NULL);
      info->EditorControl(CurrentEditor, ECTL_GETINFO, NULL, &ei);
      return;
  };

  const wchar_t *msg[2] = { GetMsg(mNothingFound), GetMsg(mGotcha) };
  info->Message(&MainGuid, 0, 0, msg, 2, 1);
}

void FarEditor::updateHighlighting()
{
  enterHandler();
  baseEditor->validate(ei.TopScreenLine, true);
}

int FarEditor::editorInput(const ProcessEditorInputInfo *ir)
{
  if (ir->Rec.EventType == KEY_EVENT && ir->Rec.Event.KeyEvent.wVirtualKeyCode == 0){

    if (baseEditor->haveInvalidLine()){
     
      idleCount++;
      if (idleCount > 10){
        idleCount = 10;
      }
      baseEditor->idleJob(idleCount*10);
      info->EditorControl(CurrentEditor, ECTL_REDRAW, NULL, NULL);
    }
  }
  else
    if (ir->Rec.EventType == KEY_EVENT){
      idleCount = 0;
    };

  return 0;
}

int FarEditor::editorEvent(int event, void *param)
{
  // ignore event
  if (event != EE_REDRAW || (event == EE_REDRAW && param == EEREDRAW_ALL && inRedraw)){
    return 0;
  }

  enterHandler();
  WindowSizeX = ei.WindowSizeX;
  WindowSizeY = ei.WindowSizeY;

  baseEditor->visibleTextEvent(ei.TopScreenLine, WindowSizeY);

  baseEditor->lineCountEvent(ei.TotalLines);

  if (param == EEREDRAW_CHANGE){
    int ml = (prevLinePosition < ei.CurLine ? prevLinePosition : ei.CurLine)-1;

    if (ml < 0){
      ml = 0;
    }

    if (blockTopPosition != -1 && ml > blockTopPosition){
      ml = blockTopPosition;
    }

    baseEditor->modifyEvent(ml);
  };

  prevLinePosition = ei.CurLine;
  blockTopPosition = -1;

  if (ei.BlockType != BTYPE_NONE){
    blockTopPosition = ei.BlockStartLine;
  }

  // hack against tabs in FAR's editor
  EditorConvertPos ecp, ecp_cl;
  ecp.StringNumber = -1;
  ecp.SrcPos = ei.CurPos;
  info->EditorControl(CurrentEditor, ECTL_REALTOTAB, NULL, &ecp);
  delete cursorRegion;
  cursorRegion = NULL;

  if (rdBackground == NULL){
    throw Exception(DString("HRD Background region 'def:Text' not found"));
  }

  for (int lno = ei.TopScreenLine; lno < ei.TopScreenLine + WindowSizeY; lno++){
    if (lno >= ei.TotalLines){
      break;
    }

    LineRegion *l1 = NULL;

    if (drawSyntax || drawPairs){
      l1 = baseEditor->getLineRegions(lno);
    }
    
    //clean line in far editor
    deleteFarColor(lno,-1);

    EditorGetString egs;
    egs.StringNumber = lno;
    info->EditorControl(CurrentEditor, ECTL_GETSTRING, NULL, &egs);
    int llen = egs.StringLength;

    // fills back
    if (lno == ei.CurLine && showHorizontalCross){
      if (!TrueMod){
        addFARColor(lno, 0, ei.LeftPos + ei.WindowSizeX, horzCrossColor);
      }
      else{
        addFARColor(lno, 0, ei.LeftPos + ei.WindowSizeX, convert(NULL));
      }
    }
    else{
      addFARColor(lno, 0, ei.LeftPos + ei.WindowSizeX, convert(NULL));
    }

    if (showVerticalCross && !TrueMod){
      ecp_cl.StringNumber = lno;
      ecp_cl.SrcPos = ecp.DestPos;
      info->EditorControl(CurrentEditor, ECTL_TABTOREAL, NULL, &ecp_cl);
      vertCrossColor.concolor |= 0x10000;
      addFARColor(lno, ecp_cl.DestPos, ecp_cl.DestPos+1, vertCrossColor);
    };

    bool vertCrossDone = false;

    if (drawSyntax){
      for (; l1; l1 = l1->next){
        if (l1->special){
          continue;
        }
        if (l1->start == l1->end){
          continue;
        }
        if (l1->start > ei.LeftPos+ei.WindowSizeX){
          continue;
        }
        if (l1->end != -1 && l1->end < ei.LeftPos-ei.WindowSizeX){
          continue;
        }

        if ((lno != ei.CurLine || !showHorizontalCross || crossZOrder == 0)){
          color col = convert(l1->styled());

          //TODO
          if (lno == ei.CurLine && showHorizontalCross){
            if (!TrueMod){
              if (foreDefault(col)){
                col.concolor = (col.concolor&0xF0) + (horzCrossColor.concolor&0xF);
              }

              if (backDefault(col)){
                col.concolor = (col.concolor&0xF) + (horzCrossColor.concolor&0xF0);
              }
            }
          };
          if (!col.concolor){
            continue;
          }
          //
          int lend = l1->end;

          if (lend == -1){
            lend = fullBackground ? ei.LeftPos+ei.WindowSizeX : llen;
          }

          addFARColor(lno, l1->start, lend, col);

          if (lno == ei.CurLine && (l1->start <= ei.CurPos) && (ei.CurPos <= lend)){
            delete cursorRegion;
            cursorRegion = new LineRegion(*l1);
          };

          // column
          if (!TrueMod && showVerticalCross && crossZOrder == 0 && l1->start <= ecp_cl.DestPos && ecp_cl.DestPos < lend){
            col = convert(l1->styled());


            if (foreDefault(col)) col.concolor = (col.concolor&0xF0) + (vertCrossColor.concolor&0xF);

            if (backDefault(col)) col.concolor = (col.concolor&0xF) + (vertCrossColor.concolor&0xF0);

            ecp_cl.StringNumber = lno;
            ecp_cl.SrcPos = ecp.DestPos;
            info->EditorControl(CurrentEditor, ECTL_TABTOREAL, NULL, &ecp_cl);
            col.concolor|=0x10000;
            addFARColor(lno, ecp_cl.DestPos, ecp_cl.DestPos+1, col);
            vertCrossDone = true;
          };
        };
      };
    };
    if (!TrueMod && showVerticalCross && !vertCrossDone){
      ecp_cl.StringNumber = lno;
      ecp_cl.SrcPos = ecp.DestPos;
      info->EditorControl(CurrentEditor, ECTL_TABTOREAL ,NULL, &ecp_cl);
      vertCrossColor.concolor |= 0x10000;
      addFARColor(lno, ecp_cl.DestPos, ecp_cl.DestPos+1, vertCrossColor);
    };
  };

  /// pair brackets
  PairMatch *pm = NULL;

  if (drawPairs){
    pm = baseEditor->searchLocalPair(ei.CurLine, ei.CurPos);
  }

  if (pm != NULL){
    color col = convert(pm->start->styled());

    // TODO
    if (!TrueMod && showHorizontalCross){
      if (foreDefault(col)){ 
        col.concolor = (col.concolor&0xF0) + (horzCrossColor.concolor&0xF);
      }

      if (backDefault(col)){
        col.concolor = (col.concolor&0xF) + (horzCrossColor.concolor&0xF0);
      }
    };
    //
    addFARColor(ei.CurLine, pm->start->start, pm->start->end, col);

    // TODO
    if (!TrueMod && showVerticalCross && !showHorizontalCross && pm->start->start <= ei.CurPos && ei.CurPos < pm->start->end){
      col = convert(pm->start->styled());

      if (foreDefault(col)){
        col.concolor = (col.concolor&0xF0) + (vertCrossColor.concolor&0xF);
      }

      if (backDefault(col)){
        col.concolor = (col.concolor&0xF) + (vertCrossColor.concolor&0xF0);
      }

      col.concolor|=0x10000;
      addFARColor(pm->sline, ei.CurPos, ei.CurPos+1, col);
    };
    //
    if (pm->eline != -1){
      col = convert(pm->end->styled());

      //
      if (!TrueMod && showHorizontalCross && pm->eline == ei.CurLine){
        if (foreDefault(col)){
          col.concolor = (col.concolor&0xF0) + (horzCrossColor.concolor&0xF);
        }

        if (backDefault(col)){
          col.concolor = (col.concolor&0xF) + (horzCrossColor.concolor&0xF0);
        }
      };
      //
      addFARColor(pm->eline, pm->end->start, pm->end->end, col);
      ecp.StringNumber = pm->eline;
      ecp.SrcPos = ecp.DestPos;
      info->EditorControl(CurrentEditor, ECTL_TABTOREAL, NULL, &ecp);

      //
      if (!TrueMod && showVerticalCross && pm->end->start <= ecp.DestPos && ecp.DestPos < pm->end->end){
        col = convert(pm->end->styled());

        if (foreDefault(col)){
          col.concolor = (col.concolor&0xF0) + (vertCrossColor.concolor&0xF);
        }

        if (backDefault(col)){
          col.concolor = (col.concolor&0xF) + (vertCrossColor.concolor&0xF0);
        }
        
        col.concolor|=0x10000;
        addFARColor(pm->eline, ecp.DestPos, ecp.DestPos+1, col);
      };
      //
    };

    baseEditor->releasePairMatch(pm);
  };

  if (param != EEREDRAW_ALL){
    inRedraw = true;
    info->EditorControl(CurrentEditor, ECTL_REDRAW, NULL, (INT_PTR)NULL);
    inRedraw = false;
  };

  return true;
}


void FarEditor::showOutliner(Outliner *outliner)
{
  FarMenuItem *menu;
  EditorSetPosition esp;
  bool moved = false;
  int code = 0;
  const int FILTER_SIZE = 40;
  FarKey breakKeys[] =
  {
    {VK_BACK,0}, 
    {VK_RETURN,0} ,{VK_OEM_1,0}, {VK_OEM_MINUS,0}, {VK_TAB,0},
    {VK_UP,LEFT_CTRL_PRESSED},{VK_DOWN,LEFT_CTRL_PRESSED },
    {VK_LEFT,LEFT_CTRL_PRESSED }, {VK_RIGHT,LEFT_CTRL_PRESSED },
    {VK_RETURN,LEFT_CTRL_PRESSED },{VK_OEM_1,LEFT_CTRL_PRESSED }, 
    {VK_OEM_MINUS,LEFT_CTRL_PRESSED },{VK_OEM_3,LEFT_CTRL_PRESSED },
    {VK_NUMPAD0,0},{VK_NUMPAD1,0},{VK_NUMPAD2,0},{VK_NUMPAD3,0},{VK_NUMPAD4,0},
    {VK_NUMPAD5,0},{VK_NUMPAD6,0},{VK_NUMPAD7,0},{VK_NUMPAD8,0},{VK_NUMPAD9,0},
    {'0',0},{'1',0},{'2',0},{'3',0},{'4',0},{'5',0},{'6',0},{'7',0},{'8',0},{'9',0},
    {'A',0},{'B',0},{'C',0},{'D',0},{'E',0},{'F',0},{'G',0},{'H',0},{'I',0},{'J',0},
    {'K',0},{'L',0},{'M',0},{'N',0},{'O',0},{'P',0},{'Q',0},{'R',0},{'S',0},{'T',0},
    {'U',0},{'V',0},{'W',0},{'X',0},{'Y',0},{'Z',0},{VK_SPACE,0}
  };
  int keys_size = sizeof(breakKeys)/sizeof(FarKey);

  //we need this?
  //int outputEnc = Encodings::getEncodingIndex("cp866");
  wchar_t prefix[FILTER_SIZE+1];
  wchar_t autofilter[FILTER_SIZE+1];
  wchar_t filter[FILTER_SIZE+1];
  int  flen = 0;
  *filter = 0;
  int maxLevel = -1;
  bool stopMenu = false;
  int items_num = outliner->itemCount();

  if (items_num == 0){
    stopMenu = true;
  }

  menu = new FarMenuItem[items_num];

  while (!stopMenu){
    int i;
    memset(menu, 0, sizeof(FarMenuItem)*items_num);
    // items in FAR's menu;
    int menu_size = 0;
    int selectedItem = 0;
    Vector<int> treeStack;

    enterHandler();
    for (i = 0; i < items_num; i++){
      OutlineItem *item = outliner->getItem(i);

      if (item->token->indexOfIgnoreCase(DString(filter)) != -1){
        int treeLevel = Outliner::manageTree(treeStack, item->level);

        if (maxLevel < treeLevel){
          maxLevel = treeLevel;
        }

        if (treeLevel > visibleLevel){
          continue;
        }

        wchar_t * menuItem = new wchar_t[255];

        if (!oldOutline){
          int si = _snwprintf(menuItem, 255, L"%4d ", item->lno+1);

          for (int lIdx = 0; lIdx < treeLevel; lIdx++){
            menuItem[si++] = ' ';
            menuItem[si++] = ' ';
          };

          const String *region = item->region->getName();

          wchar_t cls = Character::toLowerCase((*region)[region->indexOf(':')+1]);

          si += _snwprintf(menuItem+si, 255-si, L"%c ", cls);

          int labelLength = item->token->length();

          if (labelLength+si > 110){
            labelLength = 110;
          }

          wcsncpy(menuItem+si, item->token->getWChars(), labelLength);
          menuItem[si+labelLength] = 0;
        }
        else{
          String *line = getLine(item->lno);
          int labelLength = line->length();

          if (labelLength > 110){
            labelLength = 110;
          }

          wcsncpy(menuItem, line->getWChars(), labelLength);
          menuItem[labelLength] = 0;
        }

        *(OutlineItem**)(&menuItem[124]) = item;
        // set position on nearest top function
        menu[menu_size].Text = menuItem;

        if (ei.CurLine >= item->lno){
          selectedItem = menu_size;
        }

        menu_size++;
      };
    }

    if (selectedItem > 0){
      menu[selectedItem].Flags = MIF_SELECTED;
    }

    if (menu_size == 0 && flen > 0){
      flen--;
      filter[flen] = 0;
      continue;
    }

    int aflen = flen;
    // Find same function prefix
    bool same = true;
    int plen = 0;
    wcscpy(autofilter, filter);

    while (code != 0 && menu_size > 1 && same && plen < FILTER_SIZE){
      plen = aflen + 1;
      int auto_ptr  = DString(menu[0].Text).indexOfIgnoreCase(DString(autofilter));

      if (int(wcslen(menu[0].Text)-auto_ptr) < plen){
        break;
      }

      wcsncpy(prefix, menu[0].Text+auto_ptr, plen);
      prefix[plen] = 0;

      for (int j = 1 ; j < menu_size ; j++){
        if (DString(menu[j].Text).indexOfIgnoreCase(DString(prefix)) == -1){
          same = false;
          break;
        }
      }

      if (same){
        aflen++;
        wcscpy(autofilter, prefix);
      }
    }

    wchar_t top[128];
    const wchar_t *topline = GetMsg(mOutliner);
    wchar_t captionfilter[FILTER_SIZE+1];
    wcsncpy(captionfilter, filter, flen);
    captionfilter[flen] = 0;

    if (aflen > flen){
      wcscat(captionfilter, L"?");
      wcsncat(captionfilter, autofilter+flen, aflen-flen);
      captionfilter[aflen+1] = 0;
    }

    _snwprintf(top, 128, topline, captionfilter);
    int sel = 0;
    sel = info->Menu(&MainGuid, -1, -1, 0, FMENU_SHOWAMPERSAND|FMENU_WRAPMODE,
      top, GetMsg(mChoose), L"add", breakKeys, &code, menu, menu_size);

    // handle mouse selection
    if (sel != -1 && code == -1){
      code = 1;
    }

    switch (code){
    case -1:
      stopMenu = true;
      break;
    case 0: // VK_BACK

      if (flen > 0){
        flen--;
      }

      filter[flen] = 0;
      break;
    case 1:  // VK_RETURN
      {
        if (menu_size == 0){
          break;
        }

        esp.CurTabPos = esp.LeftPos = esp.Overtype = esp.TopScreenLine = -1;
        OutlineItem *item = *(OutlineItem**)(&menu[sel].Text[124]);
        esp.CurLine = item->lno;
        esp.CurPos = item->pos;
        esp.TopScreenLine = esp.CurLine - ei.WindowSizeY/2;

        if (esp.TopScreenLine < 0){
          esp.TopScreenLine = 0;
        }

        info->EditorControl(CurrentEditor, ECTL_SETPOSITION, NULL, &esp);
        stopMenu = true;
        moved = true;
        break;
      }
    case 2: // ;

      if (flen == FILTER_SIZE){
        break;
      }

      filter[flen]= ';';
      filter[++flen]= 0;
      break;
    case 3: // -

      if (flen == FILTER_SIZE){
        break;
      }

      filter[flen]= '-';
      filter[++flen]= 0;
      break;
    case 4: // VK_TAB
      wcscpy(filter, autofilter);
      flen = aflen;
      break;
    case 5:  // ctrl-up
      {
        if (menu_size == 0){
          break;
        }

        if (sel == 0){
          sel = menu_size-1;
        }
        else{
          sel--;
        }

        esp.CurTabPos = esp.LeftPos = esp.Overtype = esp.TopScreenLine = -1;
        OutlineItem *item = *(OutlineItem**)(&menu[sel].Text[124]);
        esp.CurLine = item->lno;
        esp.CurPos = item->pos;
        esp.TopScreenLine = esp.CurLine - ei.WindowSizeY/2;

        if (esp.TopScreenLine < 0){
          esp.TopScreenLine = 0;
        }

        info->EditorControl(CurrentEditor, ECTL_SETPOSITION, NULL, &esp);
        info->EditorControl(CurrentEditor, ECTL_REDRAW, NULL, NULL);
        info->EditorControl(CurrentEditor, ECTL_GETINFO, NULL, &ei);
        break;
      }
    case 6:  // ctrl-down
      {
        if (menu_size == 0){
          break;
        }

        if (sel == menu_size-1){
          sel = 0;
        }
        else{
          sel++;
        }

        esp.CurTabPos = esp.LeftPos = esp.Overtype = esp.TopScreenLine = -1;
        OutlineItem *item = *(OutlineItem**)(&menu[sel].Text[124]);
        esp.CurLine = item->lno;
        esp.CurPos = item->pos;
        esp.TopScreenLine = esp.CurLine - ei.WindowSizeY/2;

        if (esp.TopScreenLine < 0){
          esp.TopScreenLine = 0;
        }

        info->EditorControl(CurrentEditor, ECTL_SETPOSITION, NULL, &esp);
        info->EditorControl(CurrentEditor, ECTL_REDRAW, NULL, NULL);
        info->EditorControl(CurrentEditor, ECTL_GETINFO, NULL, &ei);
        break;
      }
    case 7:  // ctrl-left
      {
        if (visibleLevel > maxLevel){
          visibleLevel = maxLevel-1;
        }
        else{
          if (visibleLevel > 0){
            visibleLevel--;
          }
        }

        if (visibleLevel < 0){
          visibleLevel = 0;
        }

        break;
      }
    case 8:  // ctrl-right
      {
        visibleLevel++;
        break;
      }
    case 9:  // ctrl-return
      {
        //read current position
        info->EditorControl(CurrentEditor, ECTL_GETINFO, NULL, &ei);
        //insert text
        OutlineItem *item = *(OutlineItem**)(&menu[sel].Text[124]);
        info->EditorControl(CurrentEditor, ECTL_INSERTTEXT, NULL, (void*)item->token->getWChars());

        //move the cursor to the end of the inserted string
        esp.CurTabPos = esp.LeftPos = esp.Overtype = esp.TopScreenLine = -1;
        esp.CurLine =-1;
        esp.CurPos = ei.CurPos+item->token->length();
        info->EditorControl(CurrentEditor, ECTL_SETPOSITION, NULL, &esp);

        stopMenu = true;
        moved = true;
        break;
      }
    case 10: // :

      if (flen == FILTER_SIZE){
        break;
      }

      filter[flen]= ':';
      filter[++flen]= 0;
      break;
    case 11: // _

      if (flen == FILTER_SIZE){
        break;
      }

      filter[flen]= '_';
      filter[++flen]= 0;
      break;
    case 12: // _

      if (flen == FILTER_SIZE){
        break;
      }

      filter[flen]= '~';
      filter[++flen]= 0;
      break;
    default:

      if (flen == FILTER_SIZE || code > keys_size){
        break;
      }
      if (code<23){
        filter[flen] = (wchar_t)Character::toLowerCase('0'+code-13);
      }else{
        filter[flen] = (wchar_t)Character::toLowerCase(breakKeys[code].VirtualKeyCode);
      }
      filter[++flen] = 0;
      break;
    }
  }

  for (int i = 0; i < items_num; i++){
    delete[] menu[i].Text;
  }

  delete[] menu;

  if (!moved){
    // restoring position
    esp.CurLine = ei.CurLine;
    esp.CurPos = ei.CurPos;
    esp.CurTabPos = ei.CurTabPos;
    esp.TopScreenLine = ei.TopScreenLine;
    esp.LeftPos = ei.LeftPos;
    esp.Overtype = ei.Overtype;
    info->EditorControl(CurrentEditor, ECTL_SETPOSITION, NULL, &esp);
  }

  if (items_num == 0){
    const wchar_t *msg[2] = { GetMsg(mNothingFound), GetMsg(mGotcha) };
    info->Message(&MainGuid, 0, 0, msg, 2, 1);
  }
}

void FarEditor::enterHandler()
{
  info->EditorControl(CurrentEditor, ECTL_GETINFO, NULL, &ei);
  ret_strNumber = -1;
}

color FarEditor::convert(const StyledRegion *rd)
{
  color col;

  if (rdBackground == NULL) return col;

  int fore = (newfore != -1) ? newfore : rdBackground->fore;
  int back = (newback != -1) ? newback : rdBackground->back;

  if (TrueMod)
  {
    if (rd != NULL){
      col.fg = rd->fore;
      col.bk = rd->back;
    }

    if (rd == NULL || !rd->bfore)
      col.fg = fore;

    if (rd == NULL || !rd->bback)
      col.bk = back;

    if (rd != NULL)
      col.style = rd->style;

    return col;

  }else{
    if (rd != NULL){
      col.cfg = rd->fore;
      col.cbk = rd->back;
    }

    if (rd == NULL || !rd->bfore)
      col.cfg = fore;

    if (rd == NULL || !rd->bback)
      col.cbk = back;

    return col;
  }
}

bool FarEditor::foreDefault(color col)
{
  if (TrueMod)
    return col.fg == rdBackground->fore;
  else
    return col.cfg == rdBackground->fore;
}

bool FarEditor::backDefault(color col)
{
  if (TrueMod)
    return col.bk == rdBackground->back;
  else
    return col.cbk == rdBackground->back;
}

void FarEditor::deleteFarColor(int lno, int s)
{
    EditorDeleteColor edc;
    edc.Owner = MainGuid;
    edc.StartPos = s;
    edc.StringNumber = lno;
    edc.StructSize= sizeof (EditorDeleteColor);
    info->EditorControl(CurrentEditor, ECTL_DELCOLOR, NULL, &edc);
}

void FarEditor::addFARColor(int lno, int s, int e, color col)
{
  if (TrueMod){
    AnnotationInfo ai;
    ai.fg_color = ((col.fg>>16)&0xFF) + (col.fg&0x00FF00) + ((col.fg&0xFF)<<16);
    ai.bk_color = ((col.bk>>16)&0xFF) + (col.bk&0x00FF00) + ((col.bk&0xFF)<<16);
    ai.bk_valid = ai.fg_valid = 1;
    ai.style = col.style;
    addAnnotation(lno, s, e, ai);
  }else{
    EditorColor ec;
    ec.StructSize = sizeof(EditorColor);
    ec.Flags = 0;
    ec.StringNumber = lno;
    ec.StartPos = s;
    ec.EndPos = e-1;
    //ec.Color = col.concolor;
    ec.Owner = MainGuid;
    ec.Priority =0;
    ec.Color.Flags = FCF_FG_4BIT|FCF_BG_4BIT;
    ec.Color.BackgroundColor = col.cbk;
    ec.Color.ForegroundColor = col.cfg;
    CLR_TRACE("FarEditor", "line:%d, %d-%d, color:%x", lno, s, e, col);
    info->EditorControl(CurrentEditor, ECTL_ADDCOLOR, NULL, &ec);
    CLR_TRACE("FarEditor", "line %d: %d-%d: color=%x", lno, s, e, col);
  }

}

void FarEditor::addAnnotation(int lno, int s, int e, AnnotationInfo &ai)
{
  EditorAnnotation ea;

  ea.StringNumber = lno;
  ea.StartPos = s;
  ea.EndPos = e-1;
  memcpy(ea.annotation_raw, ai.raw, sizeof(ai.raw));
  info->EditorControl(CurrentEditor, ECTL_ADDANNOTATION, NULL, &ea);
} 	

const wchar_t *FarEditor::GetMsg(int msg)
{
	return(info->GetMsg(&MainGuid, msg));
	return null;
}

void FarEditor::cleanEditor()
{
 /* color col;
  col.concolor = (int)info->AdvControl(&MainGuid,ACTL_GETCOLOR,0,(void *)COL_EDITORTEXT);*/
  enterHandler();
  for (int i=0; i<ei.TotalLines; i++){

    deleteFarColor(i,-1);
 /*   EditorGetString egs;
    egs.StringNumber=i;
    info->EditorControl(CurrentEditor, ECTL_GETSTRING, NULL, &egs);*/

   /* if (ei.LeftPos + ei.WindowSizeX>egs.StringLength){
      addFARColor(i,0,ei.LeftPos + ei.WindowSizeX,col);
    }
    else{
      addFARColor(i,0,egs.StringLength,col);
    }*/
  }
}

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
 * Portions created by the Initial Developer are Copyright (C) 1999-2005
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