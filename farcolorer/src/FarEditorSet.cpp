#include"FarEditorSet.h"

#define REG_DISABLED        "disabled"
#define REG_HRD_NAME        "hrdName"
#define REG_CATALOG         "catalog"
#define REG_CROSSDRAW				"CrossDraw"
#define REG_PAIRSDONTDRAW  "pairsDontDraw"
#define REG_SYNTAXDONTDRAW "syntaxDontDraw"
#define REG_OLDOUTLINE     "oldOutlineView"

char* rtrim(char* str)
{
	char* ptr = str;
	str += strlen(str);
	if (ptr != str)
		while (isspace(*(--str))) *str = 0;
	return ptr;
}

char* ltrim(char* str)
{
	while (isspace(*(str++)));
	return str - 1;
}

char* trim(char* str)
{
	return ltrim(rtrim(str));
}

FarEditorSet::FarEditorSet(PluginStartupInfo *fedi)
{
	info = fedi;

	char key[255];
	_snprintf(key, 255, "%s\\colorer", info->RootKey);
	hPluginRegistry = rOpenKey(HKEY_CURRENT_USER, key);

	parserFactory = null;
	regionMapper = null;
	reloadBase();
}

FarEditorSet::~FarEditorSet(){
	dropAllEditors();
	RegCloseKey(hPluginRegistry);
	delete regionMapper;
	delete parserFactory;
}


void FarEditorSet::openMenu(){
	int iMenuItems[] = {
		mListTypes, mMatchPair, mSelectBlock, mSelectPair,
		mListFunctions, mFindErrors, mSelectRegion, mLocateFunction, -1,
		mUpdateHighlight, mChooseEncoding, mConfigure
	};
	FarMenuItem menuElements[sizeof(iMenuItems) / sizeof(iMenuItems[0])];
	memset(menuElements, 0, sizeof(menuElements));

	try{
		if (rDisabled){
			strcpy(menuElements[0].Text, GetMsg(mConfigure));
			menuElements[0].Selected = 1;
			if(info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE, GetMsg(mName), 0, "menu", NULL, NULL, menuElements, 1) == 0)
				configure();
			return;
		};
		for (int i = sizeof(iMenuItems) / sizeof(iMenuItems[0]) - 1; i >= 0; i--){
			if (iMenuItems[i] == -1)
				menuElements[i].Separator = 1;
			else
				strcpy(menuElements[i].Text, GetMsg(iMenuItems[i]));
		};
		menuElements[0].Selected = 1;

		switch(info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE, GetMsg(mName), 0, "menu", NULL, NULL,
			menuElements, sizeof(iMenuItems) / sizeof(iMenuItems[0]))){
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
				getCurrentEditor()->selectEncoding();
				break;
			case 11:
				configure();
				break;
		};
	}catch(Exception &e){
		const char* exceptionMessage[5];
		exceptionMessage[0] = GetMsg(mName);
		exceptionMessage[1] = GetMsg(mCantLoad);
		exceptionMessage[3] = GetMsg(mDie);
		StringBuffer msg("openMenu: ");
		exceptionMessage[2] = (msg+e.getMessage()).getChars();
		if (getErrorHandler())
			getErrorHandler()->error(*e.getMessage());
		info->Message(info->ModuleNumber, FMSG_WARNING, "exception", &exceptionMessage[0], 4, 1);
		disableColorer();
	};
}


void FarEditorSet::viewFile(const String &path){
	try{
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
	}catch(Exception &e){
		const char* exceptionMessage[5];
		exceptionMessage[0] = GetMsg(mName);
		exceptionMessage[1] = GetMsg(mCantOpenFile);
		exceptionMessage[3] = GetMsg(mDie);
		exceptionMessage[2] = e.getMessage()->getChars();
		info->Message(info->ModuleNumber, FMSG_WARNING, "exception", &exceptionMessage[0], 4, 1);
	};
}

int FarEditorSet::getCountFileTypeAndGroup()
{
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
  return num;
}
FileTypeImpl* FarEditorSet::getFileTypeByIndex(int idx)
{
  FileType *type = NULL;
  const String *group = NULL;

  for (int i = 0; idx>=0; idx--, i++){
    type = hrcParser->enumerateFileTypes(i);

    if (!type){
      break;
    }

    if (group != NULL && !group->equals(type->getGroup())){
      idx--;
    }
    group = type->getGroup();
  };

  return (FileTypeImpl*)type;
}

void FarEditorSet::chooseType()
{
  FarEditor *fe = getCurrentEditor();
  if (!fe){
    return;
  }

  int num = getCountFileTypeAndGroup();
  const String *group = NULL;
  FileType *type = NULL;

  char MapThis[] = "1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
  FarMenuItem *menuels = new FarMenuItem[num];
  memset(menuels, 0, sizeof(FarMenuItem)*(num));
  group = null;
  for (int idx = 0, i = 0;; idx++, i++){
    type = hrcParser->enumerateFileTypes(idx);

    if (type == NULL){
      break;
    }

    if (group != null && !group->equals(type->getGroup())){
      menuels[i].Separator = 1;
      i++;
    };

    group = type->getGroup();

    const char *groupChars = NULL;

    if (group != NULL){
      groupChars = group->getChars();
    }
    else{
      groupChars = "<no group>";
    }

    sprintf(menuels[i].Text, "%c. %s: %s", idx < 36?MapThis[idx]:'x', groupChars, type->getDescription()->getChars());

    if (type == fe->getFileType()){
      menuels[i].Selected = 1;
    }
  };
  char bottom[20];
  int i;
  sprintf(bottom, GetMsg(mTotalTypes), hrcParser->getFileTypesCount());

  i = info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE | FMENU_AUTOHIGHLIGHT,
    GetMsg(mSelectSyntax), bottom, "contents", null, null, menuels, num);
  delete[] menuels;

  if (i == -1){
    return;
  }
  type = getFileTypeByIndex(i);
  if (type != NULL){
    fe->setFileType(type);
  }

}


void FarEditorSet::configure()
{
	try{
		enum{
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
			IDX_RELOAD,//8
			IDX_RELOAD_ALL,
			IDX_OK,
			IDX_CANCEL,
		};

		FarDialogItem fdi[] = {
			{ DI_DOUBLEBOX,3,1,49,17,0,0,0,0,""},

			{ DI_CHECKBOX,6,3,0,0,TRUE,0,0,0,""},

			{ DI_CHECKBOX,6,5,0,0,FALSE,0,DIF_3STATE,0,""},
			{ DI_CHECKBOX,19,5,0,0,FALSE,0,0,0,""},
			{ DI_CHECKBOX,32,5,0,0,FALSE,0,0,0,""},

			{ DI_CHECKBOX,6,7,0,0,FALSE,0,0,0,""},

			{ DI_TEXT,6,9,0,0,FALSE,0,0,0,""},
			{ DI_EDIT,10,10,40,5,FALSE,(DWORD)"catalog",DIF_HISTORY,0,""},
			{ DI_TEXT,6,11,0,0,FALSE,0,0,0,""},    // hrd
			{ DI_BUTTON,12,12,0,0,FALSE,0,0,0,""}, // hrd button
			{ DI_BUTTON,6,14,0,0,FALSE,0,0,0,""},    // reload
			{ DI_BUTTON,26,14,0,0,FALSE,0,0,0,""},   // all
			{ DI_BUTTON,30,16,0,0,FALSE,0,0,TRUE,""}, // ok
			{ DI_BUTTON,38,16,0,0,FALSE,0,0,0,""},   // cancel
		}; // type, x1, y1, x2, y2, focus, sel, fl, def, data

		strcpy(fdi[IDX_BOX].Data, GetMsg(mSetup));
		strcpy(fdi[IDX_DISABLED].Data, GetMsg(mTurnOff));
		fdi[IDX_DISABLED].Selected = !rGetValue(hPluginRegistry, REG_DISABLED);

		strcpy(fdi[IDX_CROSS].Data, GetMsg(mCross));
		fdi[IDX_CROSS].Selected = rGetValue(hPluginRegistry, REG_CROSSDRAW);

		strcpy(fdi[IDX_PAIRS].Data, GetMsg(mPairs));
		fdi[IDX_PAIRS].Selected = !rGetValue(hPluginRegistry, REG_PAIRSDONTDRAW);

		strcpy(fdi[IDX_SYNTAX].Data, GetMsg(mSyntax));
		fdi[IDX_SYNTAX].Selected = !rGetValue(hPluginRegistry, REG_SYNTAXDONTDRAW);

		strcpy(fdi[IDX_OLDOUTLINE].Data, GetMsg(mOldOutline));
		fdi[IDX_OLDOUTLINE].Selected = rGetValue(hPluginRegistry, REG_OLDOUTLINE);

		strcpy(fdi[IDX_CATALOG].Data, GetMsg(mCatalogFile));

		char tempCatalogEdit[512] = {0};
		rGetValue(hPluginRegistry, REG_CATALOG, tempCatalogEdit, 512);
		strcpy(fdi[IDX_CATALOG_EDIT].Data, trim(tempCatalogEdit));
		strcpy(fdi[IDX_HRD].Data, GetMsg(mHRDName));

		char hrdName[32]  = {0};
		int len=rGetValue(hPluginRegistry, REG_HRD_NAME, hrdName, 32);
		DString shrdName = DString("default");
		if (len > 1) {
			char* temp=trim(hrdName);
			if (strlen(temp)>1)
				shrdName = DString(temp); 
		}
		const String *descr = null;
		if (parserFactory != null){
			descr = parserFactory->getHRDescription(DString("console"), shrdName);
		}
		if (descr == null){
			descr = &shrdName;
		}
		strcpy(fdi[IDX_HRD_SELECT].Data, descr->getChars());

		strcpy(fdi[IDX_RELOAD].Data, GetMsg(mReload));
		strcpy(fdi[IDX_RELOAD_ALL].Data, GetMsg(mReloadAll));
		strcpy(fdi[IDX_OK].Data, GetMsg(mOk));
		strcpy(fdi[IDX_CANCEL].Data, GetMsg(mCancel));

		/*
		* Dialog activation
		*/
		int i = info->Dialog(info->ModuleNumber, -1, -1, 53, 19, "config", fdi, ARRAY_SIZE(fdi));

		if (i == IDX_CANCEL || i == -1){
			return;
		}

		if (i == IDX_OK){
			char oName[512];

			oName[0] = 0;
			rGetValue(hPluginRegistry, REG_CATALOG, oName, 512);
			if (stricmp(oName, fdi[IDX_CATALOG_EDIT].Data)) i = IDX_RELOAD;

			if (rDisabled != !fdi[IDX_DISABLED].Selected){
				disableColorer();
				i = IDX_RELOAD;
			}
		}

		rSetValue(hPluginRegistry, REG_CROSSDRAW, fdi[IDX_CROSS].Selected);
		rSetValue(hPluginRegistry, REG_PAIRSDONTDRAW, !fdi[IDX_PAIRS].Selected);
		rSetValue(hPluginRegistry, REG_SYNTAXDONTDRAW, !fdi[IDX_SYNTAX].Selected);
		rSetValue(hPluginRegistry, REG_OLDOUTLINE, fdi[IDX_OLDOUTLINE].Selected);

		rSetValue(hPluginRegistry, REG_DISABLED, !fdi[IDX_DISABLED].Selected);

		strcpy(fdi[IDX_CATALOG_EDIT].Data,trim(fdi[IDX_CATALOG_EDIT].Data));
		len = strlen(fdi[IDX_CATALOG_EDIT].Data)+1;
		rSetValue(hPluginRegistry, REG_CATALOG, REG_SZ, fdi[IDX_CATALOG_EDIT].Data, len);

		readRegistry();

		if (i == IDX_HRD_SELECT){
			const String *newname = chooseHRDName(&shrdName);
			rSetValue(hPluginRegistry, REG_HRD_NAME, REG_SZ, (char*)newname->getChars(), newname->length());
			configure();
			i = IDX_RELOAD;
		};
		if (i == IDX_RELOAD){
			reloadBase();
		}
		if (i == IDX_RELOAD_ALL){
			reloadBase();
			if (rDisabled){
				return;
			}
			const char *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
			for(int idx = 0;; idx++){
				FileType *type = hrcParser->enumerateFileTypes(idx);
				if (type == null) break;
				StringBuffer tname;
				if (type->getGroup() != null) {
					tname.append(type->getGroup());
					tname.append(DString(": "));
				}
				tname.append(type->getDescription());
				marr[1] = tname.getChars();
				HANDLE scr = info->SaveScreen(0, 0, -1, -1);
				info->Message(info->ModuleNumber, 0, null, &marr[0], 2, 0);
				type->getBaseScheme();
				info->RestoreScreen(scr);
			};
		};
	}catch(Exception &e){
		const char* exceptionMessage[5];
		exceptionMessage[0] = GetMsg(mName);
		exceptionMessage[1] = GetMsg(mCantLoad);
		exceptionMessage[2] = 0;
		exceptionMessage[3] = GetMsg(mDie);
		StringBuffer msg("configure: ");
		exceptionMessage[2] = (msg+e.getMessage()).getChars();
		if (getErrorHandler() != null)
			getErrorHandler()->error(*e.getMessage());
		info->Message(info->ModuleNumber, FMSG_WARNING, "exception", &exceptionMessage[0], 4, 1);
		disableColorer();
	};

}

const String *FarEditorSet::chooseHRDName(const String *current){
	int count = 0;
	if (parserFactory == null) return current;
	while(parserFactory->enumerateHRDInstances(DString("console"), count) != null) count++;

	FarMenuItem *menuElements = new FarMenuItem[count];
	memset(menuElements, 0, sizeof(FarMenuItem)*count);

	for (int i = 0; i < count; i++){
		const String *name = parserFactory->enumerateHRDInstances(DString("console"), i);
		const String *descr = parserFactory->getHRDescription(DString("console"), *name);
		if (descr == null) descr = name;
		strcpy(menuElements[i].Text, descr->getChars());
		if (current->equals(name)) menuElements[i].Selected = 1;
	};
	int result = info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE|FMENU_AUTOHIGHLIGHT,
		GetMsg(mSelectHRD), 0, "hrd",
		NULL, NULL, menuElements, count);
	delete[] menuElements;
	if (result == -1) return current;
	return parserFactory->enumerateHRDInstances(DString("console"), result);
}




int FarEditorSet::editorInput(const INPUT_RECORD *ir)
{
	if (rDisabled){
		return 0;
	}
	return getCurrentEditor()->editorInput(ir);
}

int FarEditorSet::editorEvent(int Event, void *Param)
{
	if (rDisabled){
		return 0;
	}
	try{
		FarEditor *editor = null;
		if (Event == EE_CLOSE){
			editor = farEditorInstances.get(&SString(*((int*)Param)));
			farEditorInstances.remove(&SString(*((int*)Param)));
			delete editor;
			return 0;
		};
		if (Event != EE_REDRAW) return 0;
		editor = getCurrentEditor();
		return editor->editorEvent(Event, Param);
	}catch(Exception &e){
		const char* exceptionMessage[5];
		exceptionMessage[0] = GetMsg(mName);
		exceptionMessage[1] = GetMsg(mCantLoad);
		exceptionMessage[2] = 0;
		exceptionMessage[3] = GetMsg(mDie);
		StringBuffer msg("editorEvent: ");
		exceptionMessage[2] = (msg+e.getMessage()).getChars();
		if (getErrorHandler())
			getErrorHandler()->error(*e.getMessage());
		info->Message(info->ModuleNumber, FMSG_WARNING, "exception", &exceptionMessage[0], 4, 1);
		disableColorer();
	};
	return 0;
}



void FarEditorSet::reloadBase()
{
	const char *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
	const char *errload[5] = { GetMsg(mName), GetMsg(mCantLoad), 0, GetMsg(mFatal), GetMsg(mDie) };

	dropAllEditors();
	readRegistry();

	delete regionMapper;
	delete parserFactory;
	parserFactory = null;
	regionMapper = null;

	if (rDisabled) return;

	HANDLE scr = info->SaveScreen(0, 0, -1, -1);
	info->Message(info->ModuleNumber, 0, null, &marr[0], 2, 0);

	int len;
	char regstring[512];
	len = rGetValue(hPluginRegistry, REG_HRD_NAME, regstring, 512);
	SString *hrdName = null;
	if (len > 1) {
		char* temp=trim(regstring);
		if (strlen(temp)>1)
			hrdName = new SString(temp);
	}

	regstring[0]='\0';

	len = rGetValue(hPluginRegistry, REG_CATALOG, regstring, 512);
	SString *catalogPath = null;
	if (len > 1) {
		char* temp=trim(regstring);
		if (strlen(temp)>1)
			catalogPath = new SString(temp);
	}

	try{
		parserFactory = new ParserFactory(catalogPath);
		hrcParser = parserFactory->getHRCParser();
		try{
			regionMapper = parserFactory->createStyledMapper(&DString("console"), hrdName);
		}catch(ParserFactoryException &e){
			if (getErrorHandler() != null) getErrorHandler()->error(*e.getMessage());
			regionMapper = parserFactory->createStyledMapper(&DString("console"), null);
		};
	}catch(Exception &e){
		errload[2] = e.getMessage()->getChars();
		if (getErrorHandler() != null) getErrorHandler()->error(*e.getMessage());
		info->Message(info->ModuleNumber, 0, null, &errload[0], 5, 1);
		disableColorer();
	};
	delete catalogPath;
	delete hrdName;

	info->RestoreScreen(scr);
}


ErrorHandler *FarEditorSet::getErrorHandler(){
	if (parserFactory == null) return null;
	return parserFactory->getErrorHandler();
}


FarEditor *FarEditorSet::getCurrentEditor(){
	EditorInfo ei;
	info->EditorControl(ECTL_GETINFO, &ei);
	FarEditor *editor = farEditorInstances.get(&SString(ei.EditorID));
	if (editor == null){
		editor = new FarEditor(info, parserFactory);
		farEditorInstances.put(&SString(ei.EditorID), editor);

		DString fnpath(ei.FileName);
		int slash_idx = fnpath.lastIndexOf('\\');
		if (slash_idx == -1) slash_idx = fnpath.lastIndexOf('/');
		DString fn = DString(fnpath, slash_idx+1);
		editor->chooseFileType(&fn);
		editor->setRegionMapper(regionMapper);
		editor->setDrawCross(drawCross);
		editor->setDrawPairs(drawPairs);
		editor->setDrawSyntax(drawSyntax);
		editor->setOutlineStyle(oldOutline);
	};
	return editor;
}


const char *FarEditorSet::GetMsg(int msg){
	return(info->GetMsg(info->ModuleNumber, msg));
}

void FarEditorSet::disableColorer(){
	rDisabled = true;
	for(FarEditor *fe = farEditorInstances.enumerate(); fe != null; fe = farEditorInstances.next()){
		fe->cleanEditor();
		delete fe;  
	};
	farEditorInstances.clear();
	rSetValue(hPluginRegistry, REG_DISABLED, rDisabled);
}


void FarEditorSet::readRegistry()
{
	rDisabled = rGetValue(hPluginRegistry, REG_DISABLED) != 0;

	drawCross = rGetValue(hPluginRegistry, REG_CROSSDRAW);
	drawPairs = !rGetValue(hPluginRegistry, REG_PAIRSDONTDRAW);
	drawSyntax = !rGetValue(hPluginRegistry, REG_SYNTAXDONTDRAW);
	oldOutline = rGetValue(hPluginRegistry, REG_OLDOUTLINE) == TRUE;

	for(FarEditor *fe = farEditorInstances.enumerate(); fe != null; fe = farEditorInstances.next()){
		fe->setDrawCross(drawCross);
		fe->setDrawPairs(drawPairs);
		fe->setDrawSyntax(drawSyntax);
		fe->setOutlineStyle(oldOutline);
	}
}

void FarEditorSet::dropAllEditors(){
	for(FarEditor *fe = farEditorInstances.enumerate(); fe != null; fe = farEditorInstances.next()){
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