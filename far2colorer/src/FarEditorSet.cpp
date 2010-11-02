#include"FarEditorSet.h"

int _snwprintf_s (wchar_t *string, size_t sizeInWords, size_t count, const wchar_t *format, ...)
{
  va_list arglist;
  va_start(arglist, format);
  return _vsnwprintf(string, count, format, arglist);
}

FarEditorSet::FarEditorSet()
{
  wchar_t key[255];
  _snwprintf(key,255, L"%s\\colorer", Info.RootKey);

  DWORD res =rOpenKey(HKEY_CURRENT_USER, key, hPluginRegistry);
  if (res == REG_CREATED_NEW_KEY){
    SetDefaultSettings();
  }

  rEnabled = !!rGetValueDw(hPluginRegistry, cRegEnabled, cEnabledDefault);
  parserFactory = NULL;
  regionMapper = NULL;
  hrcParser = NULL;
  sHrdName = NULL;
  sHrdNameTm = NULL;
  sCatalogPath = NULL;
  sCatalogPathExp = NULL;
  sTempHrdName = NULL;
  sTempHrdNameTm = NULL;
  sUserHrdPath = NULL;
  sUserHrdPathExp = NULL;
  sUserHrcPath = NULL;
  sUserHrcPathExp = NULL;

  ReloadBase();
  SetBgEditor();
  viewFirst = 0;
}

FarEditorSet::~FarEditorSet()
{
  dropAllEditors(false);
  RegCloseKey(hPluginRegistry);
  delete sHrdName;
  delete sHrdNameTm;
  delete sCatalogPath;
  delete sUserHrdPath;
  delete sUserHrdPathExp;
  delete sUserHrcPath;
  delete sUserHrcPathExp;
  delete regionMapper;
  delete parserFactory;
}


void FarEditorSet::openMenu()
{
  int iMenuItems[] =
  {
    mListTypes, mMatchPair, mSelectBlock, mSelectPair,
    mListFunctions, mFindErrors, mSelectRegion, mLocateFunction, -1,
    mUpdateHighlight, mReloadBase, mConfigure
  };
  FarMenuItem menuElements[sizeof(iMenuItems) / sizeof(iMenuItems[0])];
  memset(menuElements, 0, sizeof(menuElements));

  try{
    if (!rEnabled){
      menuElements[0].Text = GetMsg(mConfigure);
      menuElements[0].Selected = 1;

      if (Info.Menu(Info.ModuleNumber, -1, -1, 0, FMENU_WRAPMODE, GetMsg(mName), 0, L"menu", NULL, NULL, menuElements, 1) == 0){
        ReadSettings();
        configure(true);
      }

      return;
    };

    for (int i = sizeof(iMenuItems) / sizeof(iMenuItems[0]) - 1; i >= 0; i--){
      if (iMenuItems[i] == -1){
        menuElements[i].Separator = 1;
      }
      else{
        menuElements[i].Text = GetMsg(iMenuItems[i]);
      }
    };

    menuElements[0].Selected = 1;

    // т.к. теоритически функция getCurrentEditor может вернуть NULL, то будем 
    // проверять на это. Но ситуация возврата NULL не нормальна, ошибка где то в другом месте
    FarEditor *editor = getCurrentEditor();
    switch (Info.Menu(Info.ModuleNumber, -1, -1, 0, FMENU_WRAPMODE, GetMsg(mName), 0, L"menu", NULL, NULL,
      menuElements, sizeof(iMenuItems) / sizeof(iMenuItems[0])))
    {
    case 0:
      if (editor){
        chooseType();
      }
      break;
    case 1:
      if (editor){
        editor->matchPair();
      }
      break;
    case 2:
      if (editor){
        editor->selectBlock();
      }
      break;
    case 3:
      if (editor){
        editor->selectPair();
      }
      break;
    case 4:
      if (editor){
        editor->listFunctions();
      }
      break;
    case 5:
      if (editor){
        editor->listErrors();
      }
      break;
    case 6:
      if (editor){
        editor->selectRegion();
      }
      break;
    case 7:
      if (editor){
        editor->locateFunction();
      }
      break;
    case 9:
      if (editor){
        editor->updateHighlighting();
      }
      break;
    case 10:
      ReloadBase();
      break;
    case 11:
      configure(true);
      break;
    };
  }
  catch (Exception &e){
    const wchar_t* exceptionMessage[5];
    exceptionMessage[0] = GetMsg(mName);
    exceptionMessage[1] = GetMsg(mCantLoad);
    exceptionMessage[3] = GetMsg(mDie);
    StringBuffer msg("openMenu: ");
    exceptionMessage[2] = (msg+e.getMessage()).getWChars();

    if (getErrorHandler()){
      getErrorHandler()->error(*e.getMessage());
    }

    Info.Message(Info.ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
    disableColorer();
  };
}


void FarEditorSet::viewFile(const String &path)
{
  if (viewFirst==0) viewFirst=1;
  try{
    if (!rEnabled){
      throw Exception(DString("Colorer is disabled"));
    }

    // Creates store of text lines
    TextLinesStore textLinesStore;
    textLinesStore.loadFile(&path, NULL, true);
    // Base editor to make primary parse
    BaseEditor baseEditor(parserFactory, &textLinesStore);
    RegionMapper *regionMap=parserFactory->createStyledMapper(&DConsole, sHrdName);
    baseEditor.setRegionMapper(regionMap);
    baseEditor.chooseFileType(&path);
    // initial event
    baseEditor.lineCountEvent(textLinesStore.getLineCount());
    // computing background color
    int background = 0x1F;
    const StyledRegion *rd = StyledRegion::cast(regionMap->getRegionDefine(DString("def:Text")));

    if (rd != NULL && rd->bfore && rd->bback){
      background = rd->fore + (rd->back<<4);
    }

    // File viewing in console window
    TextConsoleViewer viewer(&baseEditor, &textLinesStore, background, -1);
    viewer.view();
  }
  catch (Exception &e){
    const wchar_t* exceptionMessage[4];
    exceptionMessage[0] = GetMsg(mName);
    exceptionMessage[1] = GetMsg(mCantOpenFile);
    exceptionMessage[3] = GetMsg(mDie);
    exceptionMessage[2] = e.getMessage()->getWChars();
    Info.Message(Info.ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
  };
}


void FarEditorSet::chooseType()
{
  FarEditor *fe = getCurrentEditor();
  if (!fe){
    return;
  }

  int num = 0;
  const String *group = NULL;
  FileType *type = NULL;

  for (int idx = 0;; idx++, num++){
    type = hrcParser->enumerateFileTypes(idx);

    if (type == NULL){
      break;
    }

    if (group != NULL && !group->equals(type->getGroup())){
      num++;
    }

    group = type->getGroup();
  };

  char MapThis[] = "1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
  FarMenuItem *menuels = new FarMenuItem[num];
  memset(menuels, 0, sizeof(FarMenuItem)*(num));
  group = NULL;

  for (int idx = 0, i = 0;; idx++, i++){
    type = hrcParser->enumerateFileTypes(idx);

    if (type == NULL){
      break;
    }

    if (group != NULL && !group->equals(type->getGroup())){
      menuels[i].Separator = 1;
      i++;
    };

    group = type->getGroup();

    const wchar_t *groupChars = NULL;

    if (group != NULL){
      groupChars = group->getWChars();
    }
    else{
      groupChars = L"<no group>";
    }

    menuels[i].Text = new wchar_t[255];
    _snwprintf((wchar_t*)menuels[i].Text, 255, L"%c. %s: %s", idx < 36?MapThis[idx]:'x', groupChars, type->getDescription()->getWChars());

    if (type == fe->getFileType()){
      menuels[i].Selected = 1;
    }
  };

  wchar_t bottom[20];
  int i;
  _snwprintf(bottom, 20, GetMsg(mTotalTypes), num);
  i = Info.Menu(Info.ModuleNumber, -1, -1, 0, FMENU_WRAPMODE | FMENU_AUTOHIGHLIGHT,
    GetMsg(mSelectSyntax), bottom, L"contents", NULL, NULL, menuels, num);

  for (int idx = 0; idx < num; idx++){
    if (menuels[idx].Text){
      delete[] menuels[idx].Text;
    }
  }

  delete[] menuels;

  if (i == -1){
    return;
  }

  i++;
  type = NULL;
  group = NULL;

  for (int ti = 0; i; i--, ti++){
    type = hrcParser->enumerateFileTypes(ti);

    if (!type){
      break;
    }

    if (group != NULL && !group->equals(type->getGroup())){
      i--;
    }

    group = type->getGroup();
  };

  if (type != NULL){
    fe->setFileType(type);
  }
}

const String *FarEditorSet::getHRDescription(const String &name, DString _hrdClass )
{
  const String *descr = NULL;
  if (parserFactory != NULL){
    descr = parserFactory->getHRDescription(_hrdClass, name);
  }

  if (descr == NULL){
    descr = &name;
  }

  return descr;
}

LONG_PTR WINAPI SettingDialogProc(HANDLE hDlg, int Msg, int Param1, LONG_PTR Param2) 
{
  FarEditorSet *fes = (FarEditorSet *)Info.SendDlgMessage(hDlg,DM_GETDLGDATA,0,0);; 

  switch (Msg){
  case DN_BTNCLICK:
    switch (Param1){
  case IDX_HRD_SELECT:
    {
      SString *tempSS = new SString(fes->chooseHRDName(fes->sTempHrdName, DConsole));
      delete fes->sTempHrdName;
      fes->sTempHrdName=tempSS;
      const String *descr=fes->getHRDescription(*fes->sTempHrdName,DConsole);
      Info.SendDlgMessage(hDlg,DM_SETTEXTPTR,IDX_HRD_SELECT,(LONG_PTR)descr->getWChars());
      return true;
    }
    break;
  case IDX_HRD_SELECT_TM:
    {
      SString *tempSS = new SString(fes->chooseHRDName(fes->sTempHrdNameTm, DRgb));
      delete fes->sTempHrdNameTm;
      fes->sTempHrdNameTm=tempSS;
      const String *descr=fes->getHRDescription(*fes->sTempHrdNameTm,DRgb);
      Info.SendDlgMessage(hDlg,DM_SETTEXTPTR,IDX_HRD_SELECT_TM,(LONG_PTR)descr->getWChars());
      return true;
    }
    break;
  case IDX_RELOAD_ALL:
    {
      Info.SendDlgMessage(hDlg,DM_SHOWDIALOG , false,0);
      wchar_t *catalog = trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_CATALOG_EDIT,0));
      wchar_t *userhrd = trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_USERHRD_EDIT,0));
      wchar_t *userhrc = trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_USERHRC_EDIT,0));
      fes->TestLoadBase(catalog, userhrd, userhrc, true);
      Info.SendDlgMessage(hDlg,DM_SHOWDIALOG , true,0);
      return true;
    }
    break;
  case IDX_OK:
    const wchar_t *temp = (const wchar_t*)trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_CATALOG_EDIT,0));
    const wchar_t *userhrd = (const wchar_t*)trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_USERHRD_EDIT,0));
    const wchar_t *userhrc = (const wchar_t*)trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_USERHRC_EDIT,0));
    int k = (int)Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_ENABLED, 0);

    if (fes->GetCatalogPath()->compareTo(DString(temp))|| fes->GetUserHrdPath()->compareTo(DString(userhrd)) || (!fes->GetPluginStatus() && k)){ 
      if (fes->TestLoadBase(temp, userhrd, userhrc, false)){
        return false;
      }
      else{
        return true;
      }
    }

    return false;
    break;
    }
  }

  return Info.DefDlgProc(hDlg, Msg, Param1, Param2);
}

void FarEditorSet::configure(bool fromEditor)
{
  try{
    FarDialogItem fdi[] =
    {
      { DI_DOUBLEBOX,3,1,55,21,0,0,0,0,L""},                                 //IDX_BOX,
      { DI_CHECKBOX,5,2,0,0,TRUE,0,0,0,L""},                                 //IDX_DISABLED,
      { DI_CHECKBOX,5,3,0,0,FALSE,0,DIF_3STATE,0,L""},                       //IDX_CROSS,
      { DI_CHECKBOX,5,4,0,0,FALSE,0,0,0,L""},                               //IDX_PAIRS,
      { DI_CHECKBOX,5,5,0,0,FALSE,0,0,0,L""},                               //IDX_SYNTAX,
      { DI_CHECKBOX,5,6,0,0,FALSE,0,0,0,L""},                                //IDX_OLDOUTLINE,
      { DI_CHECKBOX,5,7,0,0,TRUE,0,0,0,L""},                                 //IDX_CHANGE_BG,
      { DI_TEXT,5,8,0,0,FALSE,0,0,0,L""},                                   //IDX_HRD,
      { DI_BUTTON,20,8,0,0,FALSE,0,0,0,L""},                                //IDX_HRD_SELECT,
      { DI_TEXT,5,9,0,0,FALSE,0,0,0,L""},                                    //IDX_CATALOG,
      { DI_EDIT,6,10,52,5,FALSE,(DWORD_PTR)L"catalog",DIF_HISTORY,0,L""},   //IDX_CATALOG_EDIT
      { DI_TEXT,5,11,0,0,FALSE,0,0,0,L""},                                    //IDX_USERHRC,
      { DI_EDIT,6,12,52,5,FALSE,(DWORD_PTR)L"userhrc",DIF_HISTORY,0,L""},   //IDX_USERHRC_EDIT
      { DI_TEXT,5,13,0,0,FALSE,0,0,0,L""},                                    //IDX_USERHRD,
      { DI_EDIT,6,14,52,5,FALSE,(DWORD_PTR)L"userhrd",DIF_HISTORY,0,L""},   //IDX_USERHRD_EDIT
      { DI_SINGLEBOX,4,16,54,16,TRUE,0,0,0,L""},                                //IDX_TM_BOX,
      { DI_CHECKBOX,5,17,0,0,TRUE,0,0,0,L""},                                //IDX_TRUEMOD,
      { DI_TEXT,20,17,0,0,TRUE,0,0,0,L""},                                //IDX_TMMESSAGE,
      { DI_TEXT,5,18,0,0,FALSE,0,0,0,L""},                                   //IDX_HRD_TM,
      { DI_BUTTON,20,18,0,0,FALSE,0,0,0,L""},                                //IDX_HRD_SELECT_TM,
      { DI_SINGLEBOX,4,19,54,19,TRUE,0,0,0,L""},                                //IDX_TM_BOX_OFF,
      { DI_BUTTON,29,20,0,0,FALSE,0,0,0,L""},                                //IDX_RELOAD_ALL,
      { DI_BUTTON,5,20,0,0,FALSE,0,0,TRUE,L""},                             //IDX_OK,
      { DI_BUTTON,13,20,0,0,FALSE,0,0,0,L""},                                //IDX_CANCEL,
    };// type, x1, y1, x2, y2, focus, sel, fl, def, data

    fdi[IDX_BOX].PtrData = GetMsg(mSetup);
    fdi[IDX_ENABLED].PtrData = GetMsg(mTurnOff);
    fdi[IDX_ENABLED].Selected = rEnabled;
    fdi[IDX_TRUEMOD].PtrData = GetMsg(mTrueMod);
    fdi[IDX_TRUEMOD].Selected = TrueModOn;
    fdi[IDX_CROSS].PtrData = GetMsg(mCross);
    fdi[IDX_CROSS].Selected = drawCross;
    fdi[IDX_PAIRS].PtrData = GetMsg(mPairs);
    fdi[IDX_PAIRS].Selected = drawPairs;
    fdi[IDX_SYNTAX].PtrData = GetMsg(mSyntax);
    fdi[IDX_SYNTAX].Selected = drawSyntax;
    fdi[IDX_OLDOUTLINE].PtrData = GetMsg(mOldOutline);
    fdi[IDX_OLDOUTLINE].Selected = oldOutline;
    fdi[IDX_CATALOG].PtrData = GetMsg(mCatalogFile);
    fdi[IDX_CATALOG_EDIT].PtrData = sCatalogPath->getWChars();
    fdi[IDX_USERHRC].PtrData = GetMsg(mUserHrcFile);
    fdi[IDX_USERHRC_EDIT].PtrData = sUserHrcPath->getWChars();
    fdi[IDX_USERHRD].PtrData = GetMsg(mUserHrdFile);
    fdi[IDX_USERHRD_EDIT].PtrData = sUserHrdPath->getWChars();
    fdi[IDX_HRD].PtrData = GetMsg(mHRDName);

    const String *descr = NULL;
    sTempHrdName =new SString(sHrdName); 
    descr=getHRDescription(*sTempHrdName,DConsole);

    fdi[IDX_HRD_SELECT].PtrData = descr->getWChars();

    const String *descr2 = NULL;
    sTempHrdNameTm =new SString(sHrdNameTm); 
    descr2=getHRDescription(*sTempHrdNameTm,DRgb);

    fdi[IDX_HRD_TM].PtrData = GetMsg(mHRDNameTrueMod);
    fdi[IDX_HRD_SELECT_TM].PtrData = descr2->getWChars();
    fdi[IDX_CHANGE_BG].PtrData = GetMsg(mChangeBackgroundEditor);
    fdi[IDX_CHANGE_BG].Selected = ChangeBgEditor;
    fdi[IDX_RELOAD_ALL].PtrData = GetMsg(mReloadAll);
    fdi[IDX_OK].PtrData = GetMsg(mOk);
    fdi[IDX_CANCEL].PtrData = GetMsg(mCancel);
    fdi[IDX_TM_BOX].PtrData = GetMsg(mTrueModSetting);

    if (!checkConsoleAnnotationAvailable() && fromEditor){
      fdi[IDX_HRD_SELECT_TM].Flags = DIF_DISABLE;
      fdi[IDX_TRUEMOD].Flags = DIF_DISABLE;
      if (!checkFarTrueMod()){
        if (!checkConEmu()){
          fdi[IDX_TMMESSAGE].PtrData = GetMsg(mNoFarTMConEmu);
        }
        else{
          fdi[IDX_TMMESSAGE].PtrData = GetMsg(mNoFarTM);
        }
      }
      else{
        if (!checkConEmu()){
          fdi[IDX_TMMESSAGE].PtrData = GetMsg(mNoConEmu);
        }
      }
    }
    /*
    * Dialog activation
    */
    HANDLE hDlg = Info.DialogInit(Info.ModuleNumber, -1, -1, 58, 23, L"config", fdi, ARRAY_SIZE(fdi), 0, 0, SettingDialogProc, (LONG_PTR)this);
    int i = Info.DialogRun(hDlg);

    if (i == IDX_OK){
      fdi[IDX_CATALOG_EDIT].PtrData = (const wchar_t*)trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_CATALOG_EDIT,0));
      fdi[IDX_USERHRD_EDIT].PtrData = (const wchar_t*)trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_USERHRD_EDIT,0));
      fdi[IDX_USERHRC_EDIT].PtrData = (const wchar_t*)trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_USERHRC_EDIT,0));
      //check whether or not to reload the base
      int k = false;

      if (sCatalogPath->compareTo(DString(fdi[IDX_CATALOG_EDIT].PtrData)) || 
          sUserHrdPath->compareTo(DString(fdi[IDX_USERHRD_EDIT].PtrData)) || 
          sUserHrcPath->compareTo(DString(fdi[IDX_USERHRC_EDIT].PtrData)) || 
          sHrdName->compareTo(*sTempHrdName) ||
          sHrdNameTm->compareTo(*sTempHrdNameTm))
      { 
        k = true;
      }

      fdi[IDX_ENABLED].Selected = (int)Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_ENABLED, 0);
      drawCross = (int)Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_CROSS, 0);
      drawPairs = !!Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_PAIRS, 0);
      drawSyntax = !!Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_SYNTAX, 0);
      oldOutline = !!Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_OLDOUTLINE, 0);
      ChangeBgEditor = !!Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_CHANGE_BG, 0);
      fdi[IDX_TRUEMOD].Selected = !!Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_TRUEMOD, 0);
      delete sHrdName;
      delete sHrdNameTm;
      delete sCatalogPath;
      delete sUserHrdPath;
      delete sUserHrcPath;
      sHrdName = sTempHrdName;
      sHrdNameTm = sTempHrdNameTm;
      sCatalogPath = new SString(DString(fdi[IDX_CATALOG_EDIT].PtrData));
      sUserHrdPath = new SString(DString(fdi[IDX_USERHRD_EDIT].PtrData));
      sUserHrcPath = new SString(DString(fdi[IDX_USERHRC_EDIT].PtrData));

      // if the plugin has been enable, and we will disable
      if (rEnabled && !fdi[IDX_ENABLED].Selected){
        rEnabled = false;
        TrueModOn = !!(fdi[IDX_TRUEMOD].Selected);
        SaveSettings();
        disableColorer();
      }
      else{
        if ((!rEnabled && fdi[IDX_ENABLED].Selected) || k){
          rEnabled = true;
          TrueModOn = !!(fdi[IDX_TRUEMOD].Selected);
          SaveSettings();
          enableColorer(fromEditor);
          SetBgEditor();
        }
        else{
          if (TrueModOn !=!!fdi[IDX_TRUEMOD].Selected){
            TrueModOn = !!(fdi[IDX_TRUEMOD].Selected);
            SaveSettings();
            ReloadBase();
          }
          else{
            SaveSettings();
            ApplySettingsToEditors();
            SetBgEditor();
          }
        }
      }
    }

    Info.DialogFree(hDlg);

  }
  catch (Exception &e){
    const wchar_t* exceptionMessage[5];
    exceptionMessage[0] = GetMsg(mName);
    exceptionMessage[1] = GetMsg(mCantLoad);
    exceptionMessage[2] = 0;
    exceptionMessage[3] = GetMsg(mDie);
    StringBuffer msg("configure: ");
    exceptionMessage[2] = (msg+e.getMessage()).getWChars();

    if (getErrorHandler() != NULL){
      getErrorHandler()->error(*e.getMessage());
    }

    Info.Message(Info.ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
    disableColorer();
  };
}

const String *FarEditorSet::chooseHRDName(const String *current, DString _hrdClass )
{
  if (parserFactory == NULL){
    return current;
  }

  int count = parserFactory->countHRD(_hrdClass);
  FarMenuItem *menuElements = new FarMenuItem[count];
  memset(menuElements, 0, sizeof(FarMenuItem)*count);

  for (int i = 0; i < count; i++){
    const String *name = parserFactory->enumerateHRDInstances(_hrdClass, i);
    const String *descr = parserFactory->getHRDescription(_hrdClass, *name);

    if (descr == NULL){
      descr = name;
    }

    menuElements[i].Text = descr->getWChars();

    if (current->equals(name)){
      menuElements[i].Selected = 1;
    }
  };

  int result = Info.Menu(Info.ModuleNumber, -1, -1, 0, FMENU_WRAPMODE|FMENU_AUTOHIGHLIGHT,
    GetMsg(mSelectHRD), 0, L"hrd", NULL, NULL, menuElements, count);
  delete[] menuElements;

  if (result == -1){
    return current;
  }

  return parserFactory->enumerateHRDInstances(_hrdClass, result);
}

int FarEditorSet::editorInput(const INPUT_RECORD *ir)
{
  if (rEnabled){
    FarEditor *editor = getCurrentEditor();
    if (editor){
      return editor->editorInput(ir);
    }
  }
  return 0;
}

int FarEditorSet::editorEvent(int Event, void *Param)
{
  // check whether all the editors cleaned
  if (!rEnabled && farEditorInstances.size() && Event==EE_GOTFOCUS){
    dropCurrentEditor(true);
    return 0;
  }

  if (!rEnabled){
    return 0;
  }

  try{
    FarEditor *editor = NULL;
    switch (Event){
    case EE_REDRAW:
      {
        editor = getCurrentEditor();
        if (editor){
          return editor->editorEvent(Event, Param);
        }
        else{
          return 0;
        }
      }
      break;
    case EE_GOTFOCUS:
      {
        if (!getCurrentEditor()){
          addCurrentEditor();
        }
        return 0;
      }
      break;
    case EE_READ:
      {
        addCurrentEditor();
        return 0;
      }
      break;
    case EE_CLOSE:
      {
        editor = farEditorInstances.get(&SString(*((int*)Param)));
        farEditorInstances.remove(&SString(*((int*)Param)));
        delete editor;
        return 0;
      }
      break;
    }
  }
  catch (Exception &e){
    const wchar_t* exceptionMessage[5];
    exceptionMessage[0] = GetMsg(mName);
    exceptionMessage[1] = GetMsg(mCantLoad);
    exceptionMessage[2] = 0;
    exceptionMessage[3] = GetMsg(mDie);
    StringBuffer msg("editorEvent: ");
    exceptionMessage[2] = (msg+e.getMessage()).getWChars();

    if (getErrorHandler()){
      getErrorHandler()->error(*e.getMessage());
    }

    Info.Message(Info.ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
    disableColorer();
  };

  return 0;
}

bool FarEditorSet::TestLoadBase(const wchar_t *catalogPath, const wchar_t *userHrdPath, const wchar_t *userHrcPath, const int full)
{
  bool res = true;
  const wchar_t *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
  HANDLE scr = Info.SaveScreen(0, 0, -1, -1);
  Info.Message(Info.ModuleNumber, 0, NULL, &marr[0], 2, 0);

  ParserFactory *parserFactoryLocal = NULL;
  RegionMapper *regionMapperLocal = NULL;
  HRCParser *hrcParserLocal = NULL;

  SString *catalogPathS = PathToFullS(catalogPath,false);
  SString *userHrdPathS = PathToFullS(userHrdPath,false);
  SString *userHrcPathS = PathToFullS(userHrcPath,false);

  SString *tpath;
  if (!catalogPathS || !catalogPathS->length()){
    StringBuffer *path=new StringBuffer(PluginPath);
    path->append(DString(FarCatalogXml));
    tpath = path;
  }
  else{
    tpath=catalogPathS;
  }

  try{
    parserFactoryLocal = new ParserFactory(tpath);
    hrcParserLocal = parserFactoryLocal->getHRCParser();
    FarHrcSettings p(parserFactoryLocal);
    p.readProfile();
    p.readUserProfile();
    LoadUserHrd(userHrdPathS, parserFactoryLocal);
    LoadUserHrc(userHrcPathS, parserFactoryLocal);

    try{
      regionMapperLocal = parserFactoryLocal->createStyledMapper(&DConsole, sTempHrdName);
    }
    catch (ParserFactoryException &e)
    {
      if ((parserFactoryLocal != NULL)&&(parserFactoryLocal->getErrorHandler()!=NULL)){
        parserFactoryLocal->getErrorHandler()->error(*e.getMessage());
      }
      regionMapperLocal = parserFactoryLocal->createStyledMapper(&DConsole, NULL);
    };

    delete regionMapperLocal;
    regionMapperLocal=NULL;
    try{
      regionMapperLocal = parserFactoryLocal->createStyledMapper(&DRgb, sTempHrdNameTm);
    }
    catch (ParserFactoryException &e)
    {
      if ((parserFactoryLocal != NULL)&&(parserFactoryLocal->getErrorHandler()!=NULL)){
        parserFactoryLocal->getErrorHandler()->error(*e.getMessage());
      }
      regionMapperLocal = parserFactoryLocal->createStyledMapper(&DRgb, NULL);
    };

    Info.RestoreScreen(scr);
    if (full){
      for (int idx = 0;; idx++){
        FileType *type = hrcParserLocal->enumerateFileTypes(idx);

        if (type == NULL){
          break;
        }

        StringBuffer tname;

        if (type->getGroup() != NULL){
          tname.append(type->getGroup());
          tname.append(DString(": "));
        }

        tname.append(type->getDescription());
        marr[1] = tname.getWChars();
        scr = Info.SaveScreen(0, 0, -1, -1);
        Info.Message(Info.ModuleNumber, 0, NULL, &marr[0], 2, 0);
        type->getBaseScheme();
        Info.RestoreScreen(scr);
      }
    }
  }
  catch (Exception &e){
    const wchar_t *errload[5] = { GetMsg(mName), GetMsg(mCantLoad), 0, GetMsg(mFatal), GetMsg(mDie) };

    errload[2] = e.getMessage()->getWChars();

    if ((parserFactoryLocal != NULL)&&(parserFactoryLocal->getErrorHandler()!=NULL)){
      parserFactoryLocal->getErrorHandler()->error(*e.getMessage());
    }

    Info.Message(Info.ModuleNumber, FMSG_WARNING, NULL, &errload[0], 5, 1);
    Info.RestoreScreen(scr);
    res = false;
  };

  delete regionMapperLocal;
  delete parserFactoryLocal;

  return res;
}

void FarEditorSet::ReloadBase()
{
  if (!rEnabled){
    return;
  }

  const wchar_t *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
  HANDLE scr = Info.SaveScreen(0, 0, -1, -1);
  Info.Message(Info.ModuleNumber, 0, NULL, &marr[0], 2, 0);

  dropAllEditors(false);
  delete regionMapper;
  delete parserFactory;
  parserFactory = NULL;
  regionMapper = NULL;

  ReadSettings();
  consoleAnnotationAvailable=checkConsoleAnnotationAvailable() && TrueModOn;
  if (consoleAnnotationAvailable){
    hrdClass = DRgb;
    hrdName = sHrdNameTm;
  }
  else{
    hrdClass = DConsole;
    hrdName = sHrdName;
  }

  SString *tpath;
  if (!sCatalogPathExp || !sCatalogPathExp->length()){
    StringBuffer *path=new StringBuffer(PluginPath);
    path->append(DString(FarCatalogXml));
    tpath = path;
  }
  else{
    tpath=sCatalogPathExp;
  }

  try{
    parserFactory = new ParserFactory(tpath);
    hrcParser = parserFactory->getHRCParser();
    FarHrcSettings p(parserFactory);
    p.readProfile();
    p.readUserProfile();
    LoadUserHrd(sUserHrdPathExp, parserFactory);
    LoadUserHrc(sUserHrcPathExp, parserFactory);

    try{
      regionMapper = parserFactory->createStyledMapper(&hrdClass, &hrdName);
    }
    catch (ParserFactoryException &e){
      if (getErrorHandler() != NULL){
        getErrorHandler()->error(*e.getMessage());
      }
      regionMapper = parserFactory->createStyledMapper(&hrdClass, NULL);
    };
  }
  catch (Exception &e){
    const wchar_t *errload[5] = { GetMsg(mName), GetMsg(mCantLoad), 0, GetMsg(mFatal), GetMsg(mDie) };

    errload[2] = e.getMessage()->getWChars();

    if (getErrorHandler() != NULL){
      getErrorHandler()->error(*e.getMessage());
    }

    Info.Message(Info.ModuleNumber, FMSG_WARNING, NULL, &errload[0], 5, 1);

    disableColorer();
  };

  Info.RestoreScreen(scr);
}

ErrorHandler *FarEditorSet::getErrorHandler()
{
  if (parserFactory == NULL){
    return NULL;
  }

  return parserFactory->getErrorHandler();
}

FarEditor *FarEditorSet::addCurrentEditor()
{
  if (viewFirst==1){
    viewFirst=2;
    ReloadBase();
  }

  EditorInfo ei;
  Info.EditorControl(ECTL_GETINFO, &ei);

  FarEditor *editor = new FarEditor(&Info, parserFactory);
  farEditorInstances.put(&SString(ei.EditorID), editor);
  LPWSTR FileName=NULL;
  size_t FileNameSize=Info.EditorControl(ECTL_GETFILENAME,NULL);

  if (FileNameSize){
    FileName=new wchar_t[FileNameSize];

    if (FileName){
      Info.EditorControl(ECTL_GETFILENAME,FileName);
    }
  }

  DString fnpath(FileName);
  int slash_idx = fnpath.lastIndexOf('\\');

  if (slash_idx == -1){
    slash_idx = fnpath.lastIndexOf('/');
  }

  DString fn = DString(fnpath, slash_idx+1);
  editor->chooseFileType(&fn);
  delete[] FileName;
  editor->setTrueMod(consoleAnnotationAvailable);
  editor->setRegionMapper(regionMapper);
  editor->setDrawCross(drawCross);
  editor->setDrawPairs(drawPairs);
  editor->setDrawSyntax(drawSyntax);
  editor->setOutlineStyle(oldOutline);

  return editor;
}

FarEditor *FarEditorSet::getCurrentEditor()
{
  EditorInfo ei;
  Info.EditorControl(ECTL_GETINFO, &ei);
  FarEditor *editor = farEditorInstances.get(&SString(ei.EditorID));

  return editor;
}

const wchar_t *FarEditorSet::GetMsg(int msg)
{
  return(Info.GetMsg(Info.ModuleNumber, msg));
}

void FarEditorSet::enableColorer(bool fromEditor)
{
  rEnabled = true;
  rSetValue(hPluginRegistry, cRegEnabled, rEnabled);
  ReloadBase();
  if (fromEditor){
    addCurrentEditor();
  }
}

void FarEditorSet::disableColorer()
{
  rEnabled = false;
  rSetValue(hPluginRegistry, cRegEnabled, rEnabled);

  dropCurrentEditor(true);

  delete regionMapper;
  delete parserFactory;
  parserFactory = NULL;
  regionMapper = NULL;
}

void FarEditorSet::ApplySettingsToEditors()
{
  for (FarEditor *fe = farEditorInstances.enumerate(); fe != NULL; fe = farEditorInstances.next()){
    fe->setTrueMod(consoleAnnotationAvailable);
    fe->setDrawCross(drawCross);
    fe->setDrawPairs(drawPairs);
    fe->setDrawSyntax(drawSyntax);
    fe->setOutlineStyle(oldOutline);
  }
}

void FarEditorSet::dropCurrentEditor(bool clean)
{
  EditorInfo ei;
  Info.EditorControl(ECTL_GETINFO, &ei);
  FarEditor *editor = farEditorInstances.get(&SString(ei.EditorID));
  if (editor){
    if (clean){
      editor->cleanEditor();
    }
    farEditorInstances.remove(&SString(ei.EditorID));
    delete editor;
  }
  Info.EditorControl(ECTL_REDRAW, NULL);
}

void FarEditorSet::dropAllEditors(bool clean)
{
  for (FarEditor *fe = farEditorInstances.enumerate(); fe != NULL; fe = farEditorInstances.next()){
    if (clean){
      fe->cleanEditor();
    }
    delete fe;
  };

  farEditorInstances.clear();
}

void FarEditorSet::ReadSettings()
{
  wchar_t *hrdName = rGetValueSz(hPluginRegistry, cRegHrdName, cHrdNameDefault);
  wchar_t *hrdNameTm = rGetValueSz(hPluginRegistry, cRegHrdNameTm, cHrdNameTmDefault);
  wchar_t *catalogPath = rGetValueSz(hPluginRegistry, cRegCatalog, cCatalogDefault);
  wchar_t *userHrdPath = rGetValueSz(hPluginRegistry, cRegUserHrdPath, cUserHrdPathDefault);
  wchar_t *userHrcPath = rGetValueSz(hPluginRegistry, cRegUserHrcPath, cUserHrcPathDefault);

  delete sHrdName;
  delete sHrdNameTm;
  delete sCatalogPath;
  delete sCatalogPathExp;
  delete sUserHrdPath;
  delete sUserHrdPathExp;
  delete sUserHrcPath;
  delete sUserHrcPathExp;
  sHrdName = NULL;
  sCatalogPath = NULL;
  sCatalogPathExp = NULL;
  sUserHrdPath = NULL;
  sUserHrdPathExp = NULL;
  sUserHrcPath = NULL;
  sUserHrcPathExp = NULL;

  sHrdName = new SString(hrdName);
  sHrdNameTm = new SString(hrdNameTm);
  sCatalogPath = new SString(catalogPath);
  sCatalogPathExp = PathToFullS(catalogPath,false);
  sUserHrdPath = new SString(userHrdPath);
  sUserHrdPathExp = PathToFullS(userHrdPath,false);
  sUserHrcPath = new SString(userHrcPath);
  sUserHrcPathExp = PathToFullS(userHrcPath,false);

  delete[] hrdName;
  delete[] hrdNameTm;
  delete[] catalogPath;
  delete[] userHrdPath;
  delete[] userHrcPath;

  // two '!' disable "Compiler Warning (level 3) C4800" and slightly faster code
  rEnabled = !!rGetValueDw(hPluginRegistry, cRegEnabled, cEnabledDefault);
  drawCross = rGetValueDw(hPluginRegistry, cRegCrossDraw, cCrossDrawDefault);
  drawPairs = !!rGetValueDw(hPluginRegistry, cRegPairsDraw, cPairsDrawDefault);
  drawSyntax = !!rGetValueDw(hPluginRegistry, cRegSyntaxDraw, cSyntaxDrawDefault);
  oldOutline = !!rGetValueDw(hPluginRegistry, cRegOldOutLine, cOldOutLineDefault);
  TrueModOn = !!rGetValueDw(hPluginRegistry, cRegTrueMod, cTrueMod);
  ChangeBgEditor = !!rGetValueDw(hPluginRegistry, cRegChangeBgEditor, cChangeBgEditor);
}

void FarEditorSet::SetDefaultSettings()
{
  rSetValue(hPluginRegistry, cRegEnabled, cEnabledDefault); 
  rSetValue(hPluginRegistry, cRegHrdName, REG_SZ, cHrdNameDefault, static_cast<DWORD>(sizeof(wchar_t)*(wcslen(cHrdNameDefault)+1)));
  rSetValue(hPluginRegistry, cRegHrdNameTm, REG_SZ, cHrdNameTmDefault, static_cast<DWORD>(sizeof(wchar_t)*(wcslen(cHrdNameTmDefault)+1)));
  rSetValue(hPluginRegistry, cRegCatalog, REG_SZ, cCatalogDefault, static_cast<DWORD>(sizeof(wchar_t)*(wcslen(cCatalogDefault)+1)));
  rSetValue(hPluginRegistry, cRegCrossDraw, cCrossDrawDefault); 
  rSetValue(hPluginRegistry, cRegPairsDraw, cPairsDrawDefault); 
  rSetValue(hPluginRegistry, cRegSyntaxDraw, cSyntaxDrawDefault); 
  rSetValue(hPluginRegistry, cRegOldOutLine, cOldOutLineDefault); 
  rSetValue(hPluginRegistry, cRegTrueMod, cTrueMod); 
  rSetValue(hPluginRegistry, cRegChangeBgEditor, cChangeBgEditor); 
  rSetValue(hPluginRegistry, cRegUserHrdPath, REG_SZ, cUserHrdPathDefault, static_cast<DWORD>(sizeof(wchar_t)*(wcslen(cUserHrdPathDefault)+1)));
  rSetValue(hPluginRegistry, cRegUserHrcPath, REG_SZ, cUserHrcPathDefault, static_cast<DWORD>(sizeof(wchar_t)*(wcslen(cUserHrcPathDefault)+1)));
}

void FarEditorSet::SaveSettings()
{
  rSetValue(hPluginRegistry, cRegEnabled, rEnabled); 
  rSetValue(hPluginRegistry, cRegHrdName, REG_SZ, sHrdName->getWChars(), sizeof(wchar_t)*(sHrdName->length()+1));
  rSetValue(hPluginRegistry, cRegHrdNameTm, REG_SZ, sHrdNameTm->getWChars(), sizeof(wchar_t)*(sHrdNameTm->length()+1));
  rSetValue(hPluginRegistry, cRegCatalog, REG_SZ, sCatalogPath->getWChars(), sizeof(wchar_t)*(sCatalogPath->length()+1));
  rSetValue(hPluginRegistry, cRegCrossDraw, drawCross); 
  rSetValue(hPluginRegistry, cRegPairsDraw, drawPairs); 
  rSetValue(hPluginRegistry, cRegSyntaxDraw, drawSyntax); 
  rSetValue(hPluginRegistry, cRegOldOutLine, oldOutline); 
  rSetValue(hPluginRegistry, cRegTrueMod, TrueModOn); 
  rSetValue(hPluginRegistry, cRegChangeBgEditor, ChangeBgEditor); 
  rSetValue(hPluginRegistry, cRegUserHrdPath, REG_SZ, sUserHrdPath->getWChars(), sizeof(wchar_t)*(sUserHrdPath->length()+1));
  rSetValue(hPluginRegistry, cRegUserHrcPath, REG_SZ, sUserHrcPath->getWChars(), sizeof(wchar_t)*(sUserHrcPath->length()+1));
}

bool FarEditorSet::checkConEmu()
{
  bool conemu;
  wchar_t shareName[255];
  wsprintf(shareName, AnnotationShareName, sizeof(AnnotationInfo), GetConsoleWindow());

  HANDLE hSharedMem = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, shareName);
  conemu = (hSharedMem != 0) ? 1 : 0;
  CloseHandle(hSharedMem);
  return conemu;
}

bool FarEditorSet::checkFarTrueMod()
{
  EditorAnnotation ea;
  ea.StringNumber = 1;
  ea.StartPos = 1;
  ea.EndPos = 2;
  return !!Info.EditorControl(ECTL_ADDANNOTATION, &ea);
}

bool FarEditorSet::checkConsoleAnnotationAvailable()
{
  return checkConEmu()&&checkFarTrueMod();
}

bool FarEditorSet::SetBgEditor()
{
  if (rEnabled && ChangeBgEditor && !consoleAnnotationAvailable){
    FarSetColors fsc;
    unsigned char c;

    const StyledRegion* def_text=StyledRegion::cast(regionMapper->getRegionDefine(DString("def:Text")));
    c=(def_text->back<<4) + def_text->fore;

    fsc.Flags=FCLR_REDRAW;
    fsc.ColorCount=1;
    fsc.StartIndex=COL_EDITORTEXT;
    fsc.Colors=&c;
    return !!Info.AdvControl(Info.ModuleNumber,ACTL_SETARRAYCOLOR,&fsc);
  }
  return false;
}

void FarEditorSet::LoadUserHrd(const String *filename, ParserFactory *pf)
{
  if (filename && filename->length()){
    DocumentBuilder docbuilder;
    Document *xmlDocument;
    InputSource *dfis = InputSource::newInstance(filename);
    xmlDocument = docbuilder.parse(dfis);
    Node *types = xmlDocument->getDocumentElement();

    if (*types->getNodeName() != "hrd-sets"){
      docbuilder.free(xmlDocument);
      throw Exception(DString("main '<hrd-sets>' block not found"));
    }
    for (Node *elem = types->getFirstChild(); elem; elem = elem->getNextSibling()){
      if (elem->getNodeType() == Node::ELEMENT_NODE && *elem->getNodeName() == "hrd"){
        pf->parseHRDSetsChild(elem);
      }
    };
    docbuilder.free(xmlDocument);
  }

}

void FarEditorSet::LoadUserHrc(const String *filename, ParserFactory *pf)
{
  if (filename && filename->length()){
    HRCParser *hr = pf->getHRCParser();
    InputSource *dfis = InputSource::newInstance(filename, NULL);
    try{
      hr->loadSource(dfis);
      delete dfis;
    }catch(Exception &e){
      delete dfis;
      throw Exception(e);
    }
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