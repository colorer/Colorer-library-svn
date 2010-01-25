#include<unicode/Encodings.h>
#include<common/Logging.h>
#include<colorer/viewer/TextConsoleViewer.h>

#include"FarEditorSet.h"

#define REG_DISABLED        L"disabled"
#define REG_HRD_NAME        L"hrdName"
#define REG_CATALOG         L"catalog"
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
	_snwprintf_s(key,255, 255, L"%s\\colorer", info->RootKey);
	hPluginRegistry = rOpenKey(HKEY_CURRENT_USER, key);
	rDisabled = rGetValue(hPluginRegistry, REG_DISABLED) != 0;
	parserFactory = NULL;
	regionMapper = NULL;
	ReloadBase();
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
		mUpdateHighlight, mReloadBase, mConfigure
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

		info->Message(info->ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
		disableColorer();
	};
}


void FarEditorSet::viewFile(const String &path)
{
	try
	{
		if (rDisabled) throw Exception(DString("Colorer is disabled"));

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
	i = info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE | FMENU_AUTOHIGHLIGHT,
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

int FarEditorSet::getHRDescription(const String &name,const String *&descr)
{
	int i = true;
	if (parserFactory != NULL)
	{
		descr = parserFactory->getHRDescription(DString("console"), name);
	}
	else
		i = false;

	if (descr == NULL)
	{
		descr = &name;
	}

	return i;
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
			IDX_RELOAD,
			IDX_RELOAD_ALL,
			IDX_OK,
			IDX_CANCEL,
		};

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
			{ DI_TEXT,5,11,0,0,FALSE,0,0,0,L""},                                   //IDX_HRD,
			{ DI_BUTTON,11,12,0,0,FALSE,0,0,0,L""},                                //IDX_HRD_SELECT,
			{ DI_BUTTON,5,14,0,0,FALSE,0,0,0,L""},                                 //IDX_RELOAD,
			{ DI_BUTTON,26,14,0,0,FALSE,0,0,0,L""},                                //IDX_RELOAD_ALL,
			{ DI_BUTTON,29,16,0,0,FALSE,0,0,TRUE,L""},                             //IDX_OK,
			{ DI_BUTTON,37,16,0,0,FALSE,0,0,0,L""},                                //IDX_CANCEL,
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
		
		wchar_t *tempCatalogEdit = rGetValueSz(hPluginRegistry, REG_CATALOG);
		fdi[IDX_CATALOG_EDIT].PtrData = tempCatalogEdit;
		
		fdi[IDX_HRD].PtrData = GetMsg(mHRDName);
		
		wchar_t *hrdName = rGetValueSz(hPluginRegistry, REG_HRD_NAME);
		if (!hrdName)  // with a blank line is written '\0'
		{
			hrdName = new wchar_t[7];
			wcscpy(hrdName, L"default");
		}

		SString *hrdNameSS;
		hrdNameSS = new SString(DString(hrdName));
		delete[] hrdName;

		const String *descr = NULL;
		int getDescr;
		getDescr = getHRDescription(*hrdNameSS,descr);

		fdi[IDX_HRD_SELECT].PtrData = descr->getWChars();

		fdi[IDX_RELOAD].PtrData = GetMsg(mReload);
		fdi[IDX_RELOAD_ALL].PtrData = GetMsg(mReloadAll);
		fdi[IDX_OK].PtrData = GetMsg(mOk);
		fdi[IDX_CANCEL].PtrData = GetMsg(mCancel);
		/*
		 * Dialog activation
		 */
		HANDLE hDlg = info->DialogInit(info->ModuleNumber, -1, -1, 55, 19, L"config", fdi, ARRAY_SIZE(fdi), 0, 0, info->DefDlgProc, 0);
		int i = info->DialogRun(hDlg);

		while ((i == IDX_HRD_SELECT)||(i == IDX_RELOAD)||(i == IDX_RELOAD_ALL))
		{
			if (i == IDX_HRD_SELECT)
			{
				if (parserFactory != NULL)
				{
					const String *p=chooseHRDName(hrdNameSS);
					SString *tempSS;
					tempSS = new SString(p);
					delete hrdNameSS;
					hrdNameSS=tempSS;

					getHRDescription(*hrdNameSS,descr);

					info->SendDlgMessage(hDlg,DM_SETTEXTPTR,IDX_HRD_SELECT,(LONG_PTR)descr->getWChars());
				}
			}

			if ((i == IDX_RELOAD))
			{
				wchar_t *catalog = trim((wchar_t*)info->SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_CATALOG_EDIT,0));
				TestLoadBase(hrdNameSS->getWChars(),catalog,false);
			}

			if (i == IDX_RELOAD_ALL)
			{
				wchar_t *catalog = trim((wchar_t*)info->SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,IDX_CATALOG_EDIT,0));
				TestLoadBase(hrdNameSS->getWChars(),catalog,true);
			}

			i = info->DialogRun(hDlg);
		};

		if (i == IDX_CANCEL || i == -1)
		{
			info->DialogFree(hDlg);
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

			int k = false;
			wchar_t *a = rGetValueSz(hPluginRegistry, REG_CATALOG);
			if (a!=NULL)
			{
				if (wcsicmp(a, fdi[IDX_CATALOG_EDIT].PtrData)) 
					k = true;
			}
			else
				if (wcslen(fdi[IDX_CATALOG_EDIT].PtrData)>0)
					k=true;
			delete[] a;

			if (!k)
			{
				wchar_t *b = rGetValueSz(hPluginRegistry, REG_HRD_NAME);
				if (b!=NULL)
				{
					if (wcsicmp(b, hrdNameSS->getWChars()))
						k = true;
				}
				else
					if (wcslen(hrdNameSS->getWChars())>0)
						k=true;
				delete[] b;
			}

			rSetValue(hPluginRegistry, REG_CROSSDRAW, fdi[IDX_CROSS].Selected);
			rSetValue(hPluginRegistry, REG_PAIRSDONTDRAW, !fdi[IDX_PAIRS].Selected);
			rSetValue(hPluginRegistry, REG_SYNTAXDONTDRAW, !fdi[IDX_SYNTAX].Selected);
			rSetValue(hPluginRegistry, REG_OLDOUTLINE, fdi[IDX_OLDOUTLINE].Selected);
			rSetValue(hPluginRegistry, REG_DISABLED, !fdi[IDX_DISABLED].Selected);
			rSetValue(hPluginRegistry, REG_CATALOG, REG_SZ, fdi[IDX_CATALOG_EDIT].PtrData, (DWORD)(2 *(wcslen(fdi[IDX_CATALOG_EDIT].PtrData)+1)));
			rSetValue(hPluginRegistry, REG_HRD_NAME, REG_SZ, hrdNameSS->getWChars(), (DWORD)(2 *(hrdNameSS->length()+1)));

			// if the plugin has been included, and we will disable
			if ((!rDisabled && !fdi[IDX_DISABLED].Selected))
			{
				disableColorer();
			}
			else
			if ((rDisabled && fdi[IDX_DISABLED].Selected)||(k))
			{
				rDisabled = false;
				ReloadBase();
			}
		}
		info->DialogFree(hDlg);

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

		info->Message(info->ModuleNumber, FMSG_WARNING, L"exception", &exceptionMessage[0], 4, 1);
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
		FarEditor *editor = NULL;

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

SString *FarEditorSet::ExpandEnvironment(const wchar_t *catalogPath)
{
	SString *S = NULL;
	if ((catalogPath)&&(wcslen(catalogPath)))
	{
		wchar_t *temp = NULL;
		int i;
		// ��������� �� ���������� ���������
		i=ExpandEnvironmentStrings(catalogPath,temp,0);
		if (i)
		{
			temp=new wchar_t[i];
			ExpandEnvironmentStrings(catalogPath,temp,i);
			S = new SString(DString(temp));
		}
		else
			S = new SString(DString(catalogPath));
		delete[] temp;
	}
	return S;
}

void FarEditorSet::TestLoadBase(const wchar_t *hrdName, const wchar_t *catalogPath, const int full)
{
	const wchar_t *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
	HANDLE scr = info->SaveScreen(0, 0, -1, -1);
	info->Message(info->ModuleNumber, 0, NULL, &marr[0], 2, 0);

	ParserFactory *parserFactoryLocal = NULL;
	RegionMapper *regionMapperLocal = NULL;
	HRCParser *hrcParserLocal = NULL;

	SString *hrdNameS = NULL;
	if (hrdName)
		hrdNameS = new SString(DString(hrdName));

	SString *catalogPathS = NULL;
	catalogPathS=ExpandEnvironment(catalogPath);

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

		info->RestoreScreen(scr);
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
				scr = info->SaveScreen(0, 0, -1, -1);
				info->Message(info->ModuleNumber, 0, NULL, &marr[0], 2, 0);
				type->getBaseScheme();
				info->RestoreScreen(scr);
			}
		}
	}
	catch (Exception &e)
	{
		const wchar_t *errload[5] = { GetMsg(mName), GetMsg(mCantLoad), 0, GetMsg(mFatal), GetMsg(mDie) };

		errload[2] = e.getMessage()->getWChars();

		if ((parserFactoryLocal != NULL)&&(parserFactoryLocal->getErrorHandler()!=NULL))
				parserFactoryLocal->getErrorHandler()->error(*e.getMessage());

		info->Message(info->ModuleNumber, FMSG_WARNING, NULL, &errload[0], 5, 1);
		info->RestoreScreen(scr);

	};

	delete regionMapperLocal;
	delete parserFactoryLocal;
}

void FarEditorSet::ReloadBase()
{
	if (rDisabled) return;
	
	const wchar_t *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
	HANDLE scr = info->SaveScreen(0, 0, -1, -1);
	info->Message(info->ModuleNumber, 0, NULL, &marr[0], 2, 0);
	
	dropAllEditors();
	readRegistry();
	
	wchar_t *hrdName = rGetValueSz(hPluginRegistry,REG_HRD_NAME);
	wchar_t *catalogPath =rGetValueSz(hPluginRegistry,REG_CATALOG);

	delete regionMapper;
	delete parserFactory;
	parserFactory = NULL;
	regionMapper = NULL;

	SString *hrdNameS = NULL;
	if (hrdName)
		hrdNameS = new SString(DString(hrdName));

	SString *catalogPathS = NULL;
	catalogPathS=ExpandEnvironment(catalogPath);
	
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
			if (getErrorHandler() != NULL) getErrorHandler()->error(*e.getMessage());
			regionMapper = parserFactory->createStyledMapper(&DString("console"), NULL);
		};
	}
	catch (Exception &e)
	{
		const wchar_t *errload[5] = { GetMsg(mName), GetMsg(mCantLoad), 0, GetMsg(mFatal), GetMsg(mDie) };

		errload[2] = e.getMessage()->getWChars();

		if (getErrorHandler() != NULL) getErrorHandler()->error(*e.getMessage());

		info->Message(info->ModuleNumber, FMSG_WARNING, NULL, &errload[0], 5, 1);

		disableColorer();
	};

	delete[] catalogPath;
	delete[] hrdName;

	info->RestoreScreen(scr);
}

ErrorHandler *FarEditorSet::getErrorHandler()
{
	if (parserFactory == NULL) return NULL;

	return parserFactory->getErrorHandler();
}


FarEditor *FarEditorSet::getCurrentEditor()
{
	EditorInfo ei;
	info->EditorControl(ECTL_GETINFO, &ei);
	FarEditor *editor = farEditorInstances.get(&SString(ei.EditorID));

	if (editor == NULL)
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
	
	dropAllEditors();
	
	delete regionMapper;
	delete parserFactory;
	parserFactory = NULL;
	regionMapper = NULL;
}


void FarEditorSet::readRegistry()
{
	rDisabled = rGetValue(hPluginRegistry, REG_DISABLED) != 0;
	drawCross = rGetValue(hPluginRegistry, REG_CROSSDRAW);
	drawPairs = !rGetValue(hPluginRegistry, REG_PAIRSDONTDRAW);
	drawSyntax = !rGetValue(hPluginRegistry, REG_SYNTAXDONTDRAW);
	oldOutline = rGetValue(hPluginRegistry, REG_OLDOUTLINE) == TRUE;

	for (FarEditor *fe = farEditorInstances.enumerate(); fe != NULL; fe = farEditorInstances.next())
	{
		fe->setDrawCross(drawCross);
		fe->setDrawPairs(drawPairs);
		fe->setDrawSyntax(drawSyntax);
		fe->setOutlineStyle(oldOutline);
	}
}

void FarEditorSet::dropAllEditors()
{
	for (FarEditor *fe = farEditorInstances.enumerate(); fe != NULL; fe = farEditorInstances.next())
	{
		fe->cleanEditor();
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