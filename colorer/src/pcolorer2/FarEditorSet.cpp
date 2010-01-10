#include<unicode/Encodings.h>
#include<common/Logging.h>
#include<colorer/viewer/TextConsoleViewer.h>

#include"FarEditorSet.h"

#define REG_DISABLED        L"disabled"
#define REG_HRD_NAME        L"hrdName"
#define REG_CATALOG         L"catalog"
#define REG_MAXTIME         L"maxTime"
#define REG_CROSSDRAW   L"CrossDraw"
#define REG_PAIRSDONTDRAW   L"pairsDontDraw"
#define REG_SYNTAXDONTDRAW  L"syntaxDontDraw"
#define REG_OLDOUTLINE      L"oldOutlineView"

wchar_t* rtrim(wchar_t* str)
{
	wchar_t* ptr = str;
	str += wcslen(str);

	while (iswspace(*(--str))) *str = 0;

	return ptr;
}

wchar_t* ltrim(wchar_t* str)
{
	while (iswspace(*(str++)));

	return str - 1;
}

wchar_t* trim(wchar_t* str)
{
	return ltrim(rtrim(str));
}

FarEditorSet::FarEditorSet(PluginStartupInfo *fedi)
{
	info = fedi;
	wchar_t key[255];
	_snwprintf(key, 255, L"%s\\colorer", info->RootKey);
	hPluginRegistry = rOpenKey(HKEY_CURRENT_USER, key);
	rDisabled = rGetValue(hPluginRegistry, REG_DISABLED) != 0;
	parserFactory = null;
	regionMapper = null;
	FullReloadBase();
}

FarEditorSet::~FarEditorSet()
{
	dropAllEditors();
	RegCloseKey(hPluginRegistry);
	delete regionMapper;
	delete parserFactory;
}


void FarEditorSet::openMenu()
{
	int iMenuItems[] =
	{
		mListTypes, mMatchPair, mSelectBlock, mSelectPair,
		mListFunctions, mFindErrors, mSelectRegion, mLocateFunction, -1,
		mUpdateHighlight, mConfigure
	};
	FarMenuItem menuElements[sizeof(iMenuItems) / sizeof(iMenuItems[0])];
	memset(menuElements, 0, sizeof(menuElements));

	try
	{
		if (rDisabled)
		{
			menuElements[0].Text = GetMsg(mConfigure);
			menuElements[0].Selected = 1;

			if (info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE, GetMsg(mName), 0, L"menu", NULL, NULL, menuElements, 1) == 0)
				configure();

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

		switch (info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE, GetMsg(mName), 0, L"menu", NULL, NULL,
		                   menuElements, sizeof(iMenuItems) / sizeof(iMenuItems[0])))
		{
			case 0:
				chooseType();
				break;
			case 1:
				getCurrentEditor()->matchPair();
				break;
			case 2:
				getCurrentEditor()->selectBlock();
				break;
			case 3:
				getCurrentEditor()->selectPair();
				break;
			case 4:
				getCurrentEditor()->listFunctions();
				break;
			case 5:
				getCurrentEditor()->listErrors();
				break;
			case 6:
				getCurrentEditor()->selectRegion();
				break;
			case 7:
				getCurrentEditor()->locateFunction();
				break;
			case 9:
				getCurrentEditor()->updateHighlighting();
				break;
			case 10:
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

		info->Message(info->ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
		disableColorer();
	};
}


void FarEditorSet::viewFile(const String &path)
{
	try
	{
		if (rDisabled) throw Exception(DString("Colorer is disabled"));

		String *newPath = null;

		if (path[0] == '\"') newPath = new DString(path, 1, path.length()-2);
		else newPath = new DString(path);

		// Creates store of text lines
		TextLinesStore textLinesStore;
		textLinesStore.loadFile(newPath, null, true);
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

		if (rd != null && rd->bfore && rd->bback) background = rd->fore + (rd->back<<4);

		// File viewing in console window
		TextConsoleViewer viewer(&baseEditor, &textLinesStore, background, -1);
		viewer.view();
		delete newPath;
	}
	catch (Exception &e)
	{
		const wchar_t* exceptionMessage[5];
		exceptionMessage[0] = GetMsg(mName);
		exceptionMessage[1] = GetMsg(mCantOpenFile);
		exceptionMessage[3] = GetMsg(mDie);
		exceptionMessage[2] = e.getMessage()->getWChars();
		info->Message(info->ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
	};
}


void FarEditorSet::chooseType()
{
	FarEditor *fe = getCurrentEditor();
	int num = 0;
	const String *group = null;
	FileType *type = null;

	for (int idx = 0;; idx++, num++)
	{
		type = hrcParser->enumerateFileTypes(idx);

		if (type == null) break;

		if (group != null && !group->equals(type->getGroup())) num++;

		group = type->getGroup();
	};

	char MapThis[] = "1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
	FarMenuItem *menuels = new FarMenuItem[num];
	memset(menuels, 0, sizeof(FarMenuItem)*(num));
	group = null;

	for (int idx = 0, i = 0;; idx++, i++)
	{
		type = hrcParser->enumerateFileTypes(idx);

		if (type == null) break;

		if (group != null && !group->equals(type->getGroup()))
		{
			menuels[i].Separator = 1;
			i++;
		};

		group = type->getGroup();

		const wchar_t *groupChars = null;

		if (group != null) groupChars = group->getWChars();
		else groupChars = L"<no group>";

		menuels[i].Text = new wchar_t[255];
		_snwprintf((wchar_t*)menuels[i].Text, 255, L"%c. %s: %s", idx < 36?MapThis[idx]:'x', groupChars, type->getDescription()->getWChars());

		if (type == fe->getFileType()) menuels[i].Selected = 1;
	};

	wchar_t bottom[20];
	int i;
	_snwprintf(bottom, 20, GetMsg(mTotalTypes), num);
	i = info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE | FMENU_AUTOHIGHLIGHT,
	               GetMsg(mSelectSyntax), bottom, L"contents", null, null, menuels, num);

	for (int idx = 0; idx < num; idx++)
		if (menuels[idx].Text) delete[] menuels[idx].Text;

	delete[] menuels;

	if (i == -1) return;

	i++;
	type = null;
	group = null;

	for (int ti = 0; i; i--, ti++)
	{
		type = hrcParser->enumerateFileTypes(ti);

		if (!type) break;

		if (group != null && !group->equals(type->getGroup())) i--;

		group = type->getGroup();
	};

	if (type != null) fe->setFileType(type);
}


void FarEditorSet::configure()
{
	try
	{
		enum
		{
			IDX_BOX,
			IDX_DISABLED,
			IDX_CROSS,
			IDX_PAIRS,
			IDX_SYNTAX,
			IDX_OLDOUTLINE,
			IDX_CATALOG,
			IDX_CATALOG_EDIT,
			IDX_HRD,
			IDX_HRD_SELECT,
			IDX_TIME,
			IDX_TIME_EDIT,
			IDX_RELOAD,
			IDX_RELOAD_ALL,
			IDX_OK,
			IDX_CANCEL,
		};

		FarDialogItem fdi[] =
		{
			{ DI_DOUBLEBOX,3,1,49,19,0,0,0,0,L""},                                 //IDX_BOX,
			{ DI_CHECKBOX,6,3,0,0,TRUE,0,0,0,L""},                                 //IDX_DISABLED,
			{ DI_CHECKBOX,6,5,0,0,FALSE,0,DIF_3STATE,0,L""},                       //IDX_CROSS,
			{ DI_CHECKBOX,19,5,0,0,FALSE,0,0,0,L""},                               //IDX_PAIRS,
			{ DI_CHECKBOX,32,5,0,0,FALSE,0,0,0,L""},                               //IDX_SYNTAX,
			{ DI_CHECKBOX,6,7,0,0,FALSE,0,0,0,L""},                                //IDX_OLDOUTLINE,
			{ DI_TEXT,6,9,0,0,FALSE,0,0,0,L""},                                    //IDX_CATALOG,
			{ DI_EDIT,10,10,40,5,FALSE,(DWORD_PTR)L"catalog",DIF_HISTORY,0,L""},   //IDX_CATALOG_EDIT
			{ DI_TEXT,6,11,0,0,FALSE,0,0,0,L""},                                   //IDX_HRD,
			{ DI_BUTTON,12,12,0,0,FALSE,0,0,0,L""},                                //IDX_HRD_SELECT,
			{ DI_TEXT,6,13,0,0,FALSE,0,0,0,L""},                                   //IDX_TIME,
			{ DI_EDIT,10,14,25,5,FALSE,(DWORD_PTR)L"clr_time",DIF_HISTORY,0,L""},  //IDX_TIME_EDIT,
			{ DI_BUTTON,6,16,0,0,FALSE,0,0,0,L""},                                 //IDX_RELOAD,
			{ DI_BUTTON,26,16,0,0,FALSE,0,0,0,L""},                                //IDX_RELOAD_ALL,
			{ DI_BUTTON,30,18,0,0,FALSE,0,0,TRUE,L""},                             //IDX_OK,
			{ DI_BUTTON,38,18,0,0,FALSE,0,0,0,L""},                                //IDX_CANCEL,
		};// type, x1, y1, x2, y2, focus, sel, fl, def, data

		fdi[IDX_BOX].PtrData = GetMsg(mSetup);
		fdi[IDX_DISABLED].PtrData = GetMsg(mTurnOff);
		fdi[IDX_DISABLED].Selected = !rGetValueDw(hPluginRegistry, REG_DISABLED);
		fdi[IDX_CROSS].PtrData = GetMsg(mCross);
		fdi[IDX_CROSS].Selected = rGetValueDw(hPluginRegistry, REG_CROSSDRAW);
		fdi[IDX_PAIRS].PtrData = GetMsg(mPairs);
		fdi[IDX_PAIRS].Selected = !rGetValueDw(hPluginRegistry, REG_PAIRSDONTDRAW);
		fdi[IDX_SYNTAX].PtrData = GetMsg(mSyntax);
		fdi[IDX_SYNTAX].Selected = !rGetValueDw(hPluginRegistry, REG_SYNTAXDONTDRAW);
		fdi[IDX_OLDOUTLINE].PtrData = GetMsg(mOldOutline);
		fdi[IDX_OLDOUTLINE].Selected = rGetValueDw(hPluginRegistry, REG_OLDOUTLINE);
		fdi[IDX_CATALOG].PtrData = GetMsg(mCatalogFile);
		
		wchar_t *tempCatalogEdit = null;
		rGetValueSz(hPluginRegistry, REG_CATALOG, tempCatalogEdit);
		fdi[IDX_CATALOG_EDIT].PtrData = tempCatalogEdit;
		
		fdi[IDX_HRD].PtrData = GetMsg(mHRDName);
		
		wchar_t *hrdName = null;
		DString shrdName; // используетс€ дл€ хранени€ hrd_name по всей функции
		int len=rGetValueSz(hPluginRegistry, REG_HRD_NAME, hrdName);
		if (len<=1)  // при пустой строке записываетс€ '\0' 
			hrdName = L"default";
		
		shrdName = DString(hrdName);
		const String *descr = null;
		int getDescr = true;
		if (parserFactory != null)
		{
			descr = parserFactory->getHRDescription(DString("console"), shrdName);
		}
		else
			getDescr = false;

		if (descr == null)
		{
			descr = &shrdName;
		}

		fdi[IDX_HRD_SELECT].PtrData = descr->getWChars();
		fdi[IDX_TIME].PtrData = GetMsg(mMaxTime);
		
		wchar_t *tempMaxTime = null;
		rGetValueSz(hPluginRegistry, REG_MAXTIME, tempMaxTime);
		fdi[IDX_TIME_EDIT].PtrData = tempMaxTime;

		fdi[IDX_RELOAD].PtrData = GetMsg(mReload);
		fdi[IDX_RELOAD_ALL].PtrData = GetMsg(mReloadAll);
		fdi[IDX_OK].PtrData = GetMsg(mOk);
		fdi[IDX_CANCEL].PtrData = GetMsg(mCancel);
		/*
		 * Dialog activation
		 */
		HANDLE hDlg = info->DialogInit(info->ModuleNumber, -1, -1, 53, 21, L"config", fdi, ARRAY_SIZE(fdi), 0, 0, info->DefDlgProc, 0);
		int i = info->DialogRun(hDlg);

		while ((i == IDX_HRD_SELECT)||(i == IDX_RELOAD)||(i == IDX_RELOAD_ALL))
		{
			if (i == IDX_HRD_SELECT)
			{
				if (parserFactory != null)
				{
					shrdName=chooseHRDName(&shrdName);

					// сохран€ем значение shrdName, иначе при ReloadBase оно затираетс€
					delete[] hrdName;
					hrdName= new wchar_t[shrdName.length()];
					wcscpy(hrdName,shrdName.getWChars());
					shrdName=DString(hrdName);

					descr = parserFactory->getHRDescription(DString("console"), shrdName);

					if (descr == null)
					{
						descr = &shrdName;
					}
					info->SendDlgMessage(hDlg,DM_SETTEXTPTR,IDX_HRD_SELECT,(LONG_PTR)descr->getWChars());
				}
			}

			if ((i == IDX_RELOAD)||(i == IDX_RELOAD_ALL))
			{
				wchar_t *catalog = trim((wchar_t*)info->SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_CATALOG_EDIT,0));
				QuickReloadBase(hrdName,catalog);

				//случай когда плагин был отключен и название цветовой схемы имеет не то название 
				if (!getDescr)
				{
					descr = null;
					if (parserFactory != null)
					{
						descr = parserFactory->getHRDescription(DString("console"), shrdName);
					}
					
					if (descr == null)
					{
						descr = &shrdName;
					}
					info->SendDlgMessage(hDlg,DM_SETTEXTPTR,IDX_HRD_SELECT,(LONG_PTR)descr->getWChars());
					getDescr = true;
				}
			}

			if (i == IDX_RELOAD_ALL){
				const wchar_t *marr[2] = { GetMsg(mName), GetMsg(mReloading) };

				for (int idx = 0;; idx++)
				{
					FileType *type = hrcParser->enumerateFileTypes(idx);

					if (type == null) break;

					StringBuffer tname;

					if (type->getGroup() != null)
					{
						tname.append(type->getGroup());
						tname.append(DString(": "));
					}

					tname.append(type->getDescription());
					marr[1] = tname.getWChars();
					HANDLE scr = info->SaveScreen(0, 0, -1, -1);
					info->Message(info->ModuleNumber, 0, null, &marr[0], 2, 0);
					type->getBaseScheme();
					info->RestoreScreen(scr);
				};
			}

			i = info->DialogRun(hDlg);
		};

		if (i == IDX_CANCEL || i == -1)
		{
			info->DialogFree(hDlg);
			// нужно перезагрузить все настройки на случай попыток подгрузить другую базу
			FullReloadBase();
			return;
		}

		if (i == IDX_OK)
		{
			fdi[IDX_DISABLED].Selected = (int)info->SendDlgMessage(hDlg, DM_GETCHECK, IDX_DISABLED, 0);
			fdi[IDX_CROSS].Selected = (int)info->SendDlgMessage(hDlg, DM_GETCHECK, IDX_CROSS, 0);
			fdi[IDX_PAIRS].Selected = (int)info->SendDlgMessage(hDlg, DM_GETCHECK, IDX_PAIRS, 0);
			fdi[IDX_SYNTAX].Selected = (int)info->SendDlgMessage(hDlg, DM_GETCHECK, IDX_SYNTAX, 0);
			fdi[IDX_OLDOUTLINE].Selected = (int)info->SendDlgMessage(hDlg, DM_GETCHECK, IDX_OLDOUTLINE, 0);
			fdi[IDX_CATALOG_EDIT].PtrData = (const wchar_t*)trim((wchar_t*)info->SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_CATALOG_EDIT,0));
			fdi[IDX_TIME_EDIT].PtrData = (const wchar_t*)trim((wchar_t*)info->SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_TIME_EDIT,0));

			rSetValue(hPluginRegistry, REG_CROSSDRAW, fdi[IDX_CROSS].Selected);
			rSetValue(hPluginRegistry, REG_PAIRSDONTDRAW, !fdi[IDX_PAIRS].Selected);
			rSetValue(hPluginRegistry, REG_SYNTAXDONTDRAW, !fdi[IDX_SYNTAX].Selected);
			rSetValue(hPluginRegistry, REG_OLDOUTLINE, fdi[IDX_OLDOUTLINE].Selected);
			rSetValue(hPluginRegistry, REG_DISABLED, !fdi[IDX_DISABLED].Selected);
			rSetValue(hPluginRegistry, REG_CATALOG, REG_SZ, fdi[IDX_CATALOG_EDIT].PtrData, (DWORD)(2 *(wcslen(fdi[IDX_CATALOG_EDIT].PtrData)+1)));
			rSetValue(hPluginRegistry, REG_MAXTIME, REG_SZ, fdi[IDX_TIME_EDIT].PtrData, (DWORD)(2 *(wcslen(fdi[IDX_TIME_EDIT].PtrData)+1)));
			rSetValue(hPluginRegistry, REG_HRD_NAME, REG_SZ, shrdName.getWChars(), (DWORD)(2 *(shrdName.length()+1)));

			if (!rDisabled && !fdi[IDX_DISABLED].Selected)
			{
				disableColorer();
			}
			else
			if (rDisabled && fdi[IDX_DISABLED].Selected)
			{
				rDisabled = false;
			}
		}
		info->DialogFree(hDlg);
		FullReloadBase();
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

		if (getErrorHandler() != null)
			getErrorHandler()->error(*e.getMessage());

		info->Message(info->ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
		disableColorer();
	};
}

const String *FarEditorSet::chooseHRDName(const String *current)
{
	int count = 0;

	if (parserFactory == null) return current;

	while (parserFactory->enumerateHRDInstances(DString("console"), count) != null) count++;

	FarMenuItem *menuElements = new FarMenuItem[count];
	memset(menuElements, 0, sizeof(FarMenuItem)*count);

	for (int i = 0; i < count; i++)
	{
		const String *name = parserFactory->enumerateHRDInstances(DString("console"), i);
		const String *descr = parserFactory->getHRDescription(DString("console"), *name);

		if (descr == null) descr = name;

		menuElements[i].Text = descr->getWChars();

		if (current->equals(name)) menuElements[i].Selected = 1;
	};

	int result = info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE|FMENU_AUTOHIGHLIGHT,
	                        GetMsg(mSelectHRD), 0, L"hrd",
	                        NULL, NULL, menuElements, count);
	delete[] menuElements;

	if (result == -1) return current;

	return parserFactory->enumerateHRDInstances(DString("console"), result);
}

int FarEditorSet::editorInput(const INPUT_RECORD *ir)
{
	if (rDisabled)
	{
		return 0;
	}

	return getCurrentEditor()->editorInput(ir);
}

int FarEditorSet::editorEvent(int Event, void *Param)
{
	if (rDisabled)
	{
		return 0;
	}

	try
	{
		FarEditor *editor = null;

		if (Event == EE_CLOSE)
		{
			editor = farEditorInstances.get(&SString(*((int*)Param)));
			farEditorInstances.remove(&SString(*((int*)Param)));
			delete editor;
			return 0;
		};

		if (Event != EE_REDRAW) return 0;

		editor = getCurrentEditor();
		return editor->editorEvent(Event, Param);
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

		info->Message(info->ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
		disableColorer();
	};

	return 0;
}

void FarEditorSet::QuickReloadBase(const wchar_t *hrdName, const wchar_t *catalogPath)
{
	const wchar_t *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
	HANDLE scr = info->SaveScreen(0, 0, -1, -1);
	info->Message(info->ModuleNumber, 0, null, &marr[0], 2, 0);
	
	dropAllEditors();
	ReloadBase(hrdName,catalogPath, false);
	
	info->RestoreScreen(scr);

}

void FarEditorSet::ReloadBase(const wchar_t *hrdName, const wchar_t *catalogPath, int dis)
{
	delete regionMapper;
	delete parserFactory;
	parserFactory = null;
	regionMapper = null;

	SString *hrdNameS = null;
	if ((hrdName)&&(wcslen(hrdName)))
		hrdNameS = new SString(DString(hrdName));

	// читаем путь до catalog.xml
	SString *catalogPathS = null;
	if ((catalogPath)&&(wcslen(catalogPath)))
	{
		wchar_t *temp = null;
		int i;
		// провер€ем на переменные окружени€
		i=ExpandEnvironmentStrings(catalogPath,temp,0);
		if (i)
		{
			temp=new wchar_t[i];
			ExpandEnvironmentStrings(catalogPath,temp,i);
			catalogPathS = new SString(DString(temp));
		}
		else
			catalogPathS = new SString(DString(catalogPath));
		delete[] temp;
	}
	
	try
	{
		parserFactory = new ParserFactory(catalogPathS);
		hrcParser = parserFactory->getHRCParser();

		try
		{
			regionMapper = parserFactory->createStyledMapper(&DString("console"), hrdNameS);
		}
		catch (ParserFactoryException &e)
		{
			if (getErrorHandler() != null) getErrorHandler()->error(*e.getMessage());
			regionMapper = parserFactory->createStyledMapper(&DString("console"), null);
		};
	}
	catch (Exception &e)
	{
		const wchar_t *errload[5] = { GetMsg(mName), GetMsg(mCantLoad), 0, GetMsg(mFatal), GetMsg(mDie) };

		errload[2] = e.getMessage()->getWChars();

		if (getErrorHandler() != null) getErrorHandler()->error(*e.getMessage());

		info->Message(info->ModuleNumber, FMSG_WARNING, null, &errload[0], 5, 1);
		if (dis)
			disableColorer();
	};

}

void FarEditorSet::FullReloadBase()
{
	if (rDisabled) return;
	
	const wchar_t *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
	HANDLE scr = info->SaveScreen(0, 0, -1, -1);
	info->Message(info->ModuleNumber, 0, null, &marr[0], 2, 0);
	
	dropAllEditors();
	readRegistry();
	
	wchar_t *hrd = null;
	rGetValueSz(hPluginRegistry,REG_HRD_NAME,hrd);
	
	wchar_t *catalog = null;
	rGetValueSz(hPluginRegistry,REG_CATALOG,catalog);

	ReloadBase(hrd, catalog,true);
	
	delete[] catalog;
	delete[] hrd;

	info->RestoreScreen(scr);
}


ErrorHandler *FarEditorSet::getErrorHandler()
{
	if (parserFactory == null) return null;

	return parserFactory->getErrorHandler();
}


FarEditor *FarEditorSet::getCurrentEditor()
{
	EditorInfo ei;
	info->EditorControl(ECTL_GETINFO, &ei);
	FarEditor *editor = farEditorInstances.get(&SString(ei.EditorID));

	if (editor == null)
	{
		editor = new FarEditor(info, parserFactory);
		farEditorInstances.put(&SString(ei.EditorID), editor);
		LPWSTR FileName=NULL;
		size_t FileNameSize=info->EditorControl(ECTL_GETFILENAME,NULL);

		if (FileNameSize)
		{
			FileName=new wchar_t[FileNameSize];

			if (FileName)
			{
				info->EditorControl(ECTL_GETFILENAME,FileName);
			}
		}

		DString fnpath(FileName);
		int slash_idx = fnpath.lastIndexOf('\\');

		if (slash_idx == -1) slash_idx = fnpath.lastIndexOf('/');

		DString fn = DString(fnpath, slash_idx+1);
		editor->chooseFileType(&fn);
		editor->setRegionMapper(regionMapper);
		editor->setDrawCross(drawCross);
		editor->setDrawPairs(drawPairs);
		editor->setDrawSyntax(drawSyntax);
		editor->setOutlineStyle(oldOutline);
		editor->setMaxTime(rMaxTime);
		delete[] FileName;
	};

	return editor;
}


const wchar_t *FarEditorSet::GetMsg(int msg)
{
	return(info->GetMsg(info->ModuleNumber, msg));
}


void FarEditorSet::disableColorer()
{
	rDisabled = true;
	rSetValue(hPluginRegistry, REG_DISABLED, rDisabled);
	
	for (FarEditor *fe = farEditorInstances.enumerate(); fe != null; fe = farEditorInstances.next())
	{
		fe->cleanEditor();
		delete fe;
	};

	farEditorInstances.clear();
	
	delete regionMapper;
	delete parserFactory;
	parserFactory = null;
	regionMapper = null;
}


void FarEditorSet::readRegistry()
{
	wchar_t mt[64];
	rDisabled = rGetValue(hPluginRegistry, REG_DISABLED) != 0;
	drawCross = rGetValue(hPluginRegistry, REG_CROSSDRAW);
	drawPairs = !rGetValue(hPluginRegistry, REG_PAIRSDONTDRAW);
	drawSyntax = !rGetValue(hPluginRegistry, REG_SYNTAXDONTDRAW);
	oldOutline = rGetValue(hPluginRegistry, REG_OLDOUTLINE) == TRUE;
	rMaxTime = 3000;
	int len = rGetValue(hPluginRegistry, REG_MAXTIME, mt, 64);

	if (len>0)
	{
		wchar_t* temp=trim(mt);

		if (wcslen(temp)>0)
			rMaxTime = _wtoi(mt);
	}

	for (FarEditor *fe = farEditorInstances.enumerate(); fe != null; fe = farEditorInstances.next())
	{
		fe->setDrawCross(drawCross);
		fe->setDrawPairs(drawPairs);
		fe->setDrawSyntax(drawSyntax);
		fe->setMaxTime(rMaxTime);
		fe->setOutlineStyle(oldOutline);
	}
}

void FarEditorSet::dropAllEditors()
{
	for (FarEditor *fe = farEditorInstances.enumerate(); fe != null; fe = farEditorInstances.next())
	{
		delete fe;
	};

	farEditorInstances.clear();
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