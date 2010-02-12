#include"FarEditorSet.h"

FarEditorSet::FarEditorSet()
{
	wchar_t key[255];
	_snwprintf(key,255, L"%s\\colorer", Info.RootKey);
  
  DWORD res =rOpenKey(HKEY_CURRENT_USER, key, hPluginRegistry);
  if (res == REG_CREATED_NEW_KEY)
  {
    SetDefaultSettings();
  }
  
	rEnabled = !!rGetValueDw(hPluginRegistry, cRegEnabled, cEnabledDefault);
	parserFactory = NULL;
	regionMapper = NULL;
	ReloadBase();
}

FarEditorSet::~FarEditorSet()
{
	dropAllEditors(false);
	RegCloseKey(hPluginRegistry);
  delete sHrdName;
  delete sCatalogPath;
  delete sCatalogPathExp;
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
	
	try
	{
		if (!rEnabled)
		{
			menuElements[0].Text = GetMsg(mConfigure);
			menuElements[0].Selected = 1;

			if (Info.Menu(Info.ModuleNumber, -1, -1, 0, FMENU_WRAPMODE, GetMsg(mName), 0, L"menu", NULL, NULL, menuElements, 1) == 0)
      {
        ReadSettings();
        configure();
      }

			return;
		};

		for (int i = sizeof(iMenuItems) / sizeof(iMenuItems[0]) - 1; i >= 0; i--)
		{
			if (iMenuItems[i] == -1)
				menuElements[i].Separator = 1;
			else
				menuElements[i].Text = GetMsg(iMenuItems[i]);
		};

		menuElements[0].Selected = 1;
    
    // т.к. теоритически функция getCurrentEditor может вернуть NULL, то будем 
    // проверять на это. Но ситуация возврата NULL не нормальна, ошибка где то в другом месте
    FarEditor *editor = getCurrentEditor();
		switch (Info.Menu(Info.ModuleNumber, -1, -1, 0, FMENU_WRAPMODE, GetMsg(mName), 0, L"menu", NULL, NULL,
		                   menuElements, sizeof(iMenuItems) / sizeof(iMenuItems[0])))
		{
			case 0:
        if (editor)
          chooseType();
				break;
			case 1:
        if (editor)
          editor->matchPair();
				break;
			case 2:
        if (editor)
          editor->selectBlock();
				break;
			case 3:
        if (editor)
          editor->selectPair();
				break;
			case 4:
        if (editor)
          editor->listFunctions();
				break;
			case 5:
        if (editor)
          editor->listErrors();
				break;
			case 6:
        if (editor)
          editor->selectRegion();
				break;
			case 7:
        if (editor)
          editor->locateFunction();
				break;
			case 9:
        if (editor)
          editor->updateHighlighting();
				break;
			case 10:
				ReloadBase();
				break;
			case 11:
				configure();
				break;
		};
	}
	catch (Exception &e)
	{
		const wchar_t* exceptionMessage[5];
		exceptionMessage[0] = GetMsg(mName);
		exceptionMessage[1] = GetMsg(mCantLoad);
		exceptionMessage[3] = GetMsg(mDie);
		StringBuffer msg("openMenu: ");
		exceptionMessage[2] = (msg+e.getMessage()).getWChars();

		if (getErrorHandler())
			getErrorHandler()->error(*e.getMessage());

		Info.Message(Info.ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
		disableColorer();
	};
}


void FarEditorSet::viewFile(const String &path)
{
	try
	{
		if (!rEnabled) throw Exception(DString("Colorer is disabled"));

		String *newPath = NULL;

		if (path[0] == '\"') newPath = new DString(path, 1, path.length()-2);
		else newPath = new DString(path);

		// Creates store of text lines
		TextLinesStore textLinesStore;
		textLinesStore.loadFile(newPath, NULL, true);
		// HRC loading
		//we need this?
		//HRCParser *hrcParser = parserFactory->getHRCParser();
		// Base editor to make primary parse
		BaseEditor baseEditor(parserFactory, &textLinesStore);
		baseEditor.setRegionMapper(regionMapper);
		baseEditor.chooseFileType(newPath);
		// initial event
		baseEditor.lineCountEvent(textLinesStore.getLineCount());
		// computing background color
		int background = 0x1F;
		const StyledRegion *rd = StyledRegion::cast(regionMapper->getRegionDefine(DString("def:Text")));

		if (rd != NULL && rd->bfore && rd->bback) background = rd->fore + (rd->back<<4);

		// File viewing in console window
		TextConsoleViewer viewer(&baseEditor, &textLinesStore, background, -1);
		viewer.view();
		delete newPath;
	}
	catch (Exception &e)
	{
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
	int num = 0;
	const String *group = NULL;
	FileType *type = NULL;

	for (int idx = 0;; idx++, num++)
	{
		type = hrcParser->enumerateFileTypes(idx);

		if (type == NULL) break;

		if (group != NULL && !group->equals(type->getGroup())) num++;

		group = type->getGroup();
	};

	char MapThis[] = "1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
	FarMenuItem *menuels = new FarMenuItem[num];
	memset(menuels, 0, sizeof(FarMenuItem)*(num));
	group = NULL;

	for (int idx = 0, i = 0;; idx++, i++)
	{
		type = hrcParser->enumerateFileTypes(idx);

		if (type == NULL) break;

		if (group != NULL && !group->equals(type->getGroup()))
		{
			menuels[i].Separator = 1;
			i++;
		};

		group = type->getGroup();

		const wchar_t *groupChars = NULL;

		if (group != NULL) groupChars = group->getWChars();
		else groupChars = L"<no group>";

		menuels[i].Text = new wchar_t[255];
		_snwprintf((wchar_t*)menuels[i].Text, 255, L"%c. %s: %s", idx < 36?MapThis[idx]:'x', groupChars, type->getDescription()->getWChars());

		if (type == fe->getFileType()) menuels[i].Selected = 1;
	};

	wchar_t bottom[20];
	int i;
	_snwprintf(bottom, 20, GetMsg(mTotalTypes), num);
	i = Info.Menu(Info.ModuleNumber, -1, -1, 0, FMENU_WRAPMODE | FMENU_AUTOHIGHLIGHT,
	               GetMsg(mSelectSyntax), bottom, L"contents", NULL, NULL, menuels, num);

	for (int idx = 0; idx < num; idx++)
		if (menuels[idx].Text) delete[] menuels[idx].Text;

	delete[] menuels;

	if (i == -1) return;

	i++;
	type = NULL;
	group = NULL;

	for (int ti = 0; i; i--, ti++)
	{
		type = hrcParser->enumerateFileTypes(ti);

		if (!type) break;

		if (group != NULL && !group->equals(type->getGroup())) i--;

		group = type->getGroup();
	};

	if (type != NULL) fe->setFileType(type);
}

const String *FarEditorSet::getHRDescription(const String &name)
{
  const String *descr = NULL;
	if (parserFactory != NULL)
	{
		descr = parserFactory->getHRDescription(DString("console"), name);
	}

	if (descr == NULL)
	{
		descr = &name;
	}

	return descr;
}

LONG_PTR WINAPI SettingDialogProc(HANDLE hDlg, int Msg, int Param1, LONG_PTR Param2) 
{
  FarEditorSet *fes = (FarEditorSet *)Info.SendDlgMessage(hDlg,DM_GETDLGDATA,0,0);; 

  switch (Msg)
  {
    case DN_BTNCLICK:
      switch (Param1)
      {
        case IDX_HRD_SELECT:
          {
            SString *tempSS = new SString(fes->chooseHRDName(fes->sTempHrdName));
            delete fes->sTempHrdName;
            fes->sTempHrdName=tempSS;
            const String *descr=fes->getHRDescription(*fes->sTempHrdName);
            Info.SendDlgMessage(hDlg,DM_SETTEXTPTR,IDX_HRD_SELECT,(LONG_PTR)descr->getWChars());
            return true;
          }
          break;
        case IDX_RELOAD_ALL:
          {
            Info.SendDlgMessage(hDlg,DM_SHOWDIALOG , false,0);
            wchar_t *catalog = trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_CATALOG_EDIT,0));
            fes->TestLoadBase(fes->sTempHrdName->getWChars(),catalog,true);
            Info.SendDlgMessage(hDlg,DM_SHOWDIALOG , true,0);
            return true;
          }
          break;
        case IDX_OK:
          const wchar_t *temp = (const wchar_t*)trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_CATALOG_EDIT,0));
          int k = (int)Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_ENABLED, 0);

          if (fes->GetCatalogPath()->compareTo(DString(temp))|| (!fes->GetPluginStatus() && k)) 
            if (fes->TestLoadBase(fes->sTempHrdName->getWChars(),temp,false))
              return false;
            else
              return true;

          return false;
          break;
      }
  }

  return Info.DefDlgProc(hDlg, Msg, Param1, Param2);
}

void FarEditorSet::configure()
{
	try
	{
		FarDialogItem fdi[] =
		{
			{ DI_DOUBLEBOX,3,1,51,17,0,0,0,0,L""},                                 //IDX_BOX,
			{ DI_CHECKBOX,5,3,0,0,TRUE,0,0,0,L""},                                 //IDX_DISABLED,
			{ DI_CHECKBOX,5,5,0,0,FALSE,0,DIF_3STATE,0,L""},                       //IDX_CROSS,
			{ DI_CHECKBOX,18,5,0,0,FALSE,0,0,0,L""},                               //IDX_PAIRS,
			{ DI_CHECKBOX,31,5,0,0,FALSE,0,0,0,L""},                               //IDX_SYNTAX,
			{ DI_CHECKBOX,5,7,0,0,FALSE,0,0,0,L""},                                //IDX_OLDOUTLINE,
			{ DI_TEXT,5,9,0,0,FALSE,0,0,0,L""},                                    //IDX_CATALOG,
			{ DI_EDIT,6,10,47,5,FALSE,(DWORD_PTR)L"catalog",DIF_HISTORY,0,L""},   //IDX_CATALOG_EDIT
			{ DI_TEXT,5,12,0,0,FALSE,0,0,0,L""},                                   //IDX_HRD,
			{ DI_BUTTON,20,12,0,0,FALSE,0,0,0,L""},                                //IDX_HRD_SELECT,
			{ DI_BUTTON,5,14,0,0,FALSE,0,0,0,L""},                                //IDX_RELOAD_ALL,
			{ DI_BUTTON,29,16,0,0,FALSE,0,0,TRUE,L""},                             //IDX_OK,
			{ DI_BUTTON,37,16,0,0,FALSE,0,0,0,L""},                                //IDX_CANCEL,
		};// type, x1, y1, x2, y2, focus, sel, fl, def, data

		fdi[IDX_BOX].PtrData = GetMsg(mSetup);
		fdi[IDX_ENABLED].PtrData = GetMsg(mTurnOff);
		fdi[IDX_ENABLED].Selected = rEnabled;
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
		fdi[IDX_HRD].PtrData = GetMsg(mHRDName);

		const String *descr = NULL;
    sTempHrdName =new SString(sHrdName); 
		descr=getHRDescription(*sTempHrdName);

		fdi[IDX_HRD_SELECT].PtrData = descr->getWChars();
		fdi[IDX_RELOAD_ALL].PtrData = GetMsg(mReloadAll);
		fdi[IDX_OK].PtrData = GetMsg(mOk);
		fdi[IDX_CANCEL].PtrData = GetMsg(mCancel);
		/*
		 * Dialog activation
		 */
    HANDLE hDlg = Info.DialogInit(Info.ModuleNumber, -1, -1, 55, 19, L"config", fdi, ARRAY_SIZE(fdi), 0, 0, SettingDialogProc, (LONG_PTR)this);
    int i = Info.DialogRun(hDlg);

    if (i == IDX_OK)
    {
      fdi[IDX_CATALOG_EDIT].PtrData = (const wchar_t*)trim((wchar_t*)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_CATALOG_EDIT,0));
      //check whether or not to reload the base
      int k = false;

      if (sCatalogPath->compareTo(DString(fdi[IDX_CATALOG_EDIT].PtrData))) 
        k = true;
      if (!k)
        if (sHrdName->compareTo(*sTempHrdName))
          k = true;

      fdi[IDX_ENABLED].Selected = (int)Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_ENABLED, 0);
      drawCross = (int)Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_CROSS, 0);
      drawPairs = !!Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_PAIRS, 0);
      drawSyntax = !!Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_SYNTAX, 0);
      oldOutline = !!Info.SendDlgMessage(hDlg, DM_GETCHECK, IDX_OLDOUTLINE, 0);
      delete sHrdName;
      delete sCatalogPath;
      sHrdName = sTempHrdName;
      sCatalogPath = new SString(DString(fdi[IDX_CATALOG_EDIT].PtrData));

      // if the plugin has been enable, and we will disable
      if (rEnabled && !fdi[IDX_ENABLED].Selected)
      {
        rEnabled = false;
        SaveSettings();
        disableColorer();
      }
      else
        if ((!rEnabled && fdi[IDX_ENABLED].Selected) || k)
        {
          rEnabled = true;
          SaveSettings();
          enableColorer();
        }
        else
        {
          SaveSettings();
          ApplySettingsToEditors();
        }
    }

    Info.DialogFree(hDlg);

	}
	catch (Exception &e)
	{
		const wchar_t* exceptionMessage[5];
		exceptionMessage[0] = GetMsg(mName);
		exceptionMessage[1] = GetMsg(mCantLoad);
		exceptionMessage[2] = 0;
		exceptionMessage[3] = GetMsg(mDie);
		StringBuffer msg("configure: ");
		exceptionMessage[2] = (msg+e.getMessage()).getWChars();

		if (getErrorHandler() != NULL)
			getErrorHandler()->error(*e.getMessage());

		Info.Message(Info.ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
		disableColorer();
	};
}

const String *FarEditorSet::chooseHRDName(const String *current)
{
	int count = 0;

	if (parserFactory == NULL) return current;

	while (parserFactory->enumerateHRDInstances(DString("console"), count) != NULL) count++;

	FarMenuItem *menuElements = new FarMenuItem[count];
	memset(menuElements, 0, sizeof(FarMenuItem)*count);

	for (int i = 0; i < count; i++)
	{
		const String *name = parserFactory->enumerateHRDInstances(DString("console"), i);
		const String *descr = parserFactory->getHRDescription(DString("console"), *name);

		if (descr == NULL) descr = name;

		menuElements[i].Text = descr->getWChars();

		if (current->equals(name)) menuElements[i].Selected = 1;
	};

	int result = Info.Menu(Info.ModuleNumber, -1, -1, 0, FMENU_WRAPMODE|FMENU_AUTOHIGHLIGHT,
	                        GetMsg(mSelectHRD), 0, L"hrd",
	                        NULL, NULL, menuElements, count);
	delete[] menuElements;

	if (result == -1) return current;

	return parserFactory->enumerateHRDInstances(DString("console"), result);
}

int FarEditorSet::editorInput(const INPUT_RECORD *ir)
{
	if (rEnabled)
  {
    FarEditor *editor = getCurrentEditor();
    if (editor)
      return editor->editorInput(ir);
  }
  return 0;
}

int FarEditorSet::editorEvent(int Event, void *Param)
{
  // check whether all the editors cleaned
  if (!rEnabled && farEditorInstances.size() && Event==EE_GOTFOCUS)
  {
    dropCurrentEditor(true);
    return 0;
  }

	if (!rEnabled)
	{
		return 0;
	}

	try
	{
    FarEditor *editor = NULL;
    switch (Event)
    {
      case EE_REDRAW:
        {
          editor = getCurrentEditor();
          if (editor)
            return editor->editorEvent(Event, Param);
          else return 0;
        }
        break;
      case EE_GOTFOCUS:
        {
          if (!getCurrentEditor())
            addCurrentEditor();
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
	catch (Exception &e)
	{
		const wchar_t* exceptionMessage[5];
		exceptionMessage[0] = GetMsg(mName);
		exceptionMessage[1] = GetMsg(mCantLoad);
		exceptionMessage[2] = 0;
		exceptionMessage[3] = GetMsg(mDie);
		StringBuffer msg("editorEvent: ");
		exceptionMessage[2] = (msg+e.getMessage()).getWChars();

		if (getErrorHandler())
			getErrorHandler()->error(*e.getMessage());

		Info.Message(Info.ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
		disableColorer();
	};

	return 0;
}

bool FarEditorSet::TestLoadBase(const wchar_t *hrdName, const wchar_t *catalogPath, const int full)
{
  bool res = true;
	const wchar_t *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
	HANDLE scr = Info.SaveScreen(0, 0, -1, -1);
	Info.Message(Info.ModuleNumber, 0, NULL, &marr[0], 2, 0);

	ParserFactory *parserFactoryLocal = NULL;
	RegionMapper *regionMapperLocal = NULL;
	HRCParser *hrcParserLocal = NULL;

	SString *hrdNameS = NULL;
	if (hrdName)
		hrdNameS = new SString(DString(hrdName));

	SString *catalogPathS = NULL;
  wchar_t *t=PathToFool(catalogPath,false);
  if (t)
    catalogPathS=new SString(DString(t));
  delete[] t;
 
	try
	{
		parserFactoryLocal = new ParserFactory(catalogPathS);
		hrcParserLocal = parserFactoryLocal->getHRCParser();

		try
		{
			regionMapperLocal = parserFactoryLocal->createStyledMapper(&DString("console"), hrdNameS);
		}
		catch (ParserFactoryException &e)
		{
			if ((parserFactoryLocal != NULL)&&(parserFactoryLocal->getErrorHandler()!=NULL))
				parserFactoryLocal->getErrorHandler()->error(*e.getMessage());
			regionMapperLocal = parserFactoryLocal->createStyledMapper(&DString("console"), NULL);
		};

		Info.RestoreScreen(scr);
		if (full)
		{
			for (int idx = 0;; idx++)
			{
				FileType *type = hrcParserLocal->enumerateFileTypes(idx);

				if (type == NULL) break;

				StringBuffer tname;

				if (type->getGroup() != NULL)
				{
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
	catch (Exception &e)
	{
		const wchar_t *errload[5] = { GetMsg(mName), GetMsg(mCantLoad), 0, GetMsg(mFatal), GetMsg(mDie) };

		errload[2] = e.getMessage()->getWChars();

		if ((parserFactoryLocal != NULL)&&(parserFactoryLocal->getErrorHandler()!=NULL))
				parserFactoryLocal->getErrorHandler()->error(*e.getMessage());

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
	if (!rEnabled) return;
	
	const wchar_t *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
	HANDLE scr = Info.SaveScreen(0, 0, -1, -1);
	Info.Message(Info.ModuleNumber, 0, NULL, &marr[0], 2, 0);
	
	dropAllEditors(false);
	delete regionMapper;
	delete parserFactory;
	parserFactory = NULL;
	regionMapper = NULL;

  ReadSettings();

	try
	{
		parserFactory = new ParserFactory(sCatalogPathExp);
		hrcParser = parserFactory->getHRCParser();

		try
		{
			regionMapper = parserFactory->createStyledMapper(&DString("console"), sHrdName);
		}
		catch (ParserFactoryException &e)
		{
			if (getErrorHandler() != NULL) getErrorHandler()->error(*e.getMessage());
			regionMapper = parserFactory->createStyledMapper(&DString("console"), NULL);
		};
	}
	catch (Exception &e)
	{
		const wchar_t *errload[5] = { GetMsg(mName), GetMsg(mCantLoad), 0, GetMsg(mFatal), GetMsg(mDie) };

		errload[2] = e.getMessage()->getWChars();

		if (getErrorHandler() != NULL) getErrorHandler()->error(*e.getMessage());

		Info.Message(Info.ModuleNumber, FMSG_WARNING, NULL, &errload[0], 5, 1);

		disableColorer();
	};

	Info.RestoreScreen(scr);
}

ErrorHandler *FarEditorSet::getErrorHandler()
{
	if (parserFactory == NULL) return NULL;

	return parserFactory->getErrorHandler();
}

FarEditor *FarEditorSet::addCurrentEditor()
{
  EditorInfo ei;
  Info.EditorControl(ECTL_GETINFO, &ei);

  FarEditor *editor = new FarEditor(&Info, parserFactory);
  farEditorInstances.put(&SString(ei.EditorID), editor);
  LPWSTR FileName=NULL;
  size_t FileNameSize=Info.EditorControl(ECTL_GETFILENAME,NULL);

  if (FileNameSize)
  {
    FileName=new wchar_t[FileNameSize];

    if (FileName)
    {
      Info.EditorControl(ECTL_GETFILENAME,FileName);
    }
  }

  DString fnpath(FileName);
  int slash_idx = fnpath.lastIndexOf('\\');

  if (slash_idx == -1) slash_idx = fnpath.lastIndexOf('/');

  DString fn = DString(fnpath, slash_idx+1);
  editor->chooseFileType(&fn);
  delete[] FileName;
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

void FarEditorSet::enableColorer()
{
  rEnabled = true;
	rSetValue(hPluginRegistry, cRegEnabled, rEnabled);
  ReloadBase();
  addCurrentEditor();
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
	for (FarEditor *fe = farEditorInstances.enumerate(); fe != NULL; fe = farEditorInstances.next())
	{
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
  if (editor)
  {
    if (clean)
      editor->cleanEditor();
    farEditorInstances.remove(&SString(ei.EditorID));
    delete editor;
  }
  Info.EditorControl(ECTL_REDRAW, NULL);
}

void FarEditorSet::dropAllEditors(bool clean)
{
	for (FarEditor *fe = farEditorInstances.enumerate(); fe != NULL; fe = farEditorInstances.next())
	{
    if (clean)
      fe->cleanEditor();
		delete fe;
	};

	farEditorInstances.clear();
}

void FarEditorSet::ReadSettings()
{
  wchar_t *hrdName = rGetValueSz(hPluginRegistry, cRegHrdName, cHrdNameDefault);
	wchar_t *catalogPath = rGetValueSz(hPluginRegistry, cRegCatalog, cCatalogDefault);

  delete sHrdName;
  delete sCatalogPath;
  delete sCatalogPathExp;
  sHrdName = NULL;
  sCatalogPath = NULL;
  sCatalogPathExp = NULL;

  sHrdName = new SString(DString(hrdName));
	sCatalogPath = new SString(DString(catalogPath));
  wchar_t *t=PathToFool(catalogPath,false);
  if (t)
    sCatalogPathExp=new SString(DString(t));
  delete[] t;

  delete[] hrdName;
  delete[] catalogPath;

  // two '!' disable "Compiler Warning (level 3) C4800" and slightly faster code
	rEnabled = !!rGetValueDw(hPluginRegistry, cRegEnabled, cEnabledDefault);
	drawCross = rGetValueDw(hPluginRegistry, cRegCrossDraw, cCrossDrawDefault);
	drawPairs = !!rGetValueDw(hPluginRegistry, cRegPairsDraw, cPairsDrawDefault);
	drawSyntax = !!rGetValueDw(hPluginRegistry, cRegSyntaxDraw, cSyntaxDrawDefault);
	oldOutline = !!rGetValueDw(hPluginRegistry, cRegOldOutLine, cOldOutLineDefault);
}

void FarEditorSet::SetDefaultSettings()
{
  rSetValue(hPluginRegistry, cRegEnabled, cEnabledDefault); 
  rSetValue(hPluginRegistry, cRegHrdName, REG_SZ, cHrdNameDefault, sizeof(wchar_t)*(wcslen(cHrdNameDefault)+1));
  rSetValue(hPluginRegistry, cRegCatalog, REG_SZ, cCatalogDefault, sizeof(wchar_t)*(wcslen(cCatalogDefault)+1));
  rSetValue(hPluginRegistry, cRegCrossDraw, cCrossDrawDefault); 
  rSetValue(hPluginRegistry, cRegPairsDraw, cPairsDrawDefault); 
  rSetValue(hPluginRegistry, cRegSyntaxDraw, cSyntaxDrawDefault); 
  rSetValue(hPluginRegistry, cRegOldOutLine, cOldOutLineDefault); 
}

void FarEditorSet::SaveSettings()
{
  rSetValue(hPluginRegistry, cRegEnabled, rEnabled); 
  rSetValue(hPluginRegistry, cRegHrdName, REG_SZ, sHrdName->getWChars(), sizeof(wchar_t)*(sHrdName->length()+1));
  rSetValue(hPluginRegistry, cRegCatalog, REG_SZ, sCatalogPath->getWChars(), sizeof(wchar_t)*(sCatalogPath->length()+1));
  rSetValue(hPluginRegistry, cRegCrossDraw, drawCross); 
  rSetValue(hPluginRegistry, cRegPairsDraw, drawPairs); 
  rSetValue(hPluginRegistry, cRegSyntaxDraw, drawSyntax); 
  rSetValue(hPluginRegistry, cRegOldOutLine, oldOutline); 
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