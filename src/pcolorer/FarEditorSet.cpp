#include<stdlib.h>
#include<farplugin/farcolor.hpp>
#include<misc/registry.cpp>
#include<unicode/Encodings.h>
#include<colorer/viewer/TextConsoleViewer.h>

#include"FarEditorSet.h"

#define REG_DISABLED        "disabled"
#define REG_HRD_NAME        "hrdName"
#define REG_CATALOG         "catalog"
#define REG_MAXTIME         "maxTime"
#define REG_CROSSDONTDRAW  "crossDontDraw"
#define REG_PAIRSDONTDRAW  "pairsDontDraw"
#define REG_SYNTAXDONTDRAW "syntaxDontDraw"

FarEditorSet::FarEditorSet(PluginStartupInfo *fedi)
{
  info = fedi;

  char key[200];
  sprintf(key, "%s\\colorer", info->RootKey);
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
    mListFunctions, mFindErrors, mSelectRegion, -1,
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
      case 8:
        getCurrentEditor()->updateHighlighting();
        break;
      case 9:
        getCurrentEditor()->selectEncoding();
        break;
      case 10:
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
    info->Message(info->ModuleNumber, 0, "exception", &exceptionMessage[0], 4, 1);
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
    HRCParser *hrcParser = parserFactory->getHRCParser();
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
    info->Message(info->ModuleNumber, 0, "exception", &exceptionMessage[0], 4, 1);
  };
}


void FarEditorSet::chooseType()
{
  FarEditor *fe = getCurrentEditor();

  int num = 0;
  int i = 0;
  int idx = 0;
  const String *group = null;
  FileType *type = null;

  for(idx = 0;; idx++, num++){
    type = hrcParser->enumerateFileTypes(idx);
    if (type == null) break;
    if (group != null && !group->equals(type->getGroup())) num++;
    group = type->getGroup();
  };

  char MapThis[] = "1234567890QWERTYUIOPASDFGHJKLZXCVBNMxx";
  FarMenuItem *menuels = new FarMenuItem[num+2];
  memset(menuels, 0, sizeof(FarMenuItem)*(num+2));

  group = null;
  for(idx = 0, i = 0;; idx++, i++){
    type = hrcParser->enumerateFileTypes(idx);
    if (type == null) break;


    if (group != null && !group->equals(type->getGroup())){
      menuels[i].Separator = 1;
      i++;
    };
    group = type->getGroup();

    const char *groupChars = null;
    if (group != null) groupChars = group->getChars();
    sprintf(menuels[i].Text, "%c. %s: %s", idx < 37?MapThis[idx]:'x', groupChars, type->getDescription()->getChars());
    if(type == fe->getFileType()) menuels[i].Selected = 1;
  };

  char bottom[20];
  sprintf(bottom, GetMsg(mTotalTypes), idx);

  i = info->Menu(info->ModuleNumber, -1, -1, 0, FMENU_WRAPMODE | FMENU_AUTOHIGHLIGHT,
                     GetMsg(mSelectSyntax), bottom, "contents", null, null, menuels, i);
  delete[] menuels;

  if (i == -1) return;
  i++;
  type = null;
  group = null;
  for(int ti = 0; i; i--, ti++){
    type = hrcParser->enumerateFileTypes(ti);
    if (group != null && !group->equals(type->getGroup())) i--;
    group = type->getGroup();
    if (!type) break;
  };
  if (type != null) fe->setFileType(type);
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
      IDX_CATALOG,
      IDX_CATALOG_EDIT,
      IDX_HRD,
      IDX_HRD_SELECT,
      IDX_TIME,
      IDX_TIME_EDIT,
      IDX_RELOAD,//8
      IDX_RELOAD_ALL,
      IDX_OK,
      IDX_CANCEL,
    };

    FarDialogItem fdi[] = {
      { DI_DOUBLEBOX,3,1,49,17,0,0,0,0,""},

      { DI_CHECKBOX,6,3,0,0,TRUE,0,0,0,""},

      { DI_CHECKBOX,6,5,0,0,FALSE,0,0,0,""},
      { DI_CHECKBOX,19,5,0,0,FALSE,0,0,0,""},
      { DI_CHECKBOX,32,5,0,0,FALSE,0,0,0,""},

      { DI_TEXT,6,7,0,0,FALSE,0,0,0,""},
      { DI_EDIT,10,8,40,5,FALSE,(DWORD)"catalog",DIF_HISTORY,0,""},
      { DI_TEXT,6,9,0,0,FALSE,0,0,0,""},    // hrd
      { DI_BUTTON,10,10,0,0,FALSE,0,0,0,""}, // hrd button
      { DI_TEXT,6,11,0,0,FALSE,0,0,0,""},
      { DI_EDIT,10,12,25,5,FALSE,(DWORD)"clr_time",DIF_HISTORY,0,""},
      { DI_BUTTON,6,14,0,0,FALSE,0,0,0,""},    // reload
      { DI_BUTTON,26,14,0,0,FALSE,0,0,0,""},   // all
      { DI_BUTTON,30,16,0,0,FALSE,0,0,TRUE,""}, // ok
      { DI_BUTTON,38,16,0,0,FALSE,0,0,0,""},   // cancel
    }; // type, x1, y1, x2, y2, focus, sel, fl, def, data

    strcpy(fdi[IDX_BOX].Data, GetMsg(mSetup));
    strcpy(fdi[IDX_DISABLED].Data, GetMsg(mTurnOff));
    fdi[IDX_DISABLED].Selected = !rGetValue(hPluginRegistry, REG_DISABLED);

    strcpy(fdi[IDX_CROSS].Data, GetMsg(mCross));
    strcpy(fdi[IDX_PAIRS].Data, GetMsg(mPairs));
    strcpy(fdi[IDX_SYNTAX].Data, GetMsg(mSyntax));
    fdi[IDX_CROSS].Selected = !rGetValue(hPluginRegistry, REG_CROSSDONTDRAW);
    fdi[IDX_PAIRS].Selected = !rGetValue(hPluginRegistry, REG_PAIRSDONTDRAW);
    fdi[IDX_SYNTAX].Selected = !rGetValue(hPluginRegistry, REG_SYNTAXDONTDRAW);

    strcpy(fdi[IDX_CATALOG].Data, GetMsg(mCatalogFile));
    rGetValue(hPluginRegistry, REG_CATALOG, fdi[IDX_CATALOG_EDIT].Data, 512);
    strcpy(fdi[IDX_HRD].Data, GetMsg(mHRDName));

    char hrdName[32];
    hrdName[0] = 0;
    rGetValue(hPluginRegistry, REG_HRD_NAME, hrdName, 32);
    DString shrdName;
    if (hrdName[0]){
      shrdName = DString(hrdName);
    }else{
      shrdName = DString("default");
    }
    const String *descr = null;
    if (parserFactory != null){
      descr = parserFactory->getHRDescription(DString("console"), shrdName);
    }
    if (descr == null){
      descr = &shrdName;
    }
    strcpy(fdi[IDX_HRD_SELECT].Data, descr->getChars());

    strcpy(fdi[IDX_TIME].Data, GetMsg(mMaxTime));

    rGetValue(hPluginRegistry, REG_MAXTIME, fdi[IDX_TIME_EDIT].Data, 512);

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

      oName[0] = 0;
      rGetValue(hPluginRegistry, REG_MAXTIME, oName, 512);
      if (stricmp(oName, fdi[IDX_TIME_EDIT].Data)) i = IDX_RELOAD;

      if (rDisabled != !fdi[IDX_DISABLED].Selected){
        i = IDX_RELOAD;
      }
    }

    rSetValue(hPluginRegistry, REG_CROSSDONTDRAW, !fdi[IDX_CROSS].Selected);
    rSetValue(hPluginRegistry, REG_PAIRSDONTDRAW, !fdi[IDX_PAIRS].Selected);
    rSetValue(hPluginRegistry, REG_SYNTAXDONTDRAW, !fdi[IDX_SYNTAX].Selected);

    rSetValue(hPluginRegistry, REG_DISABLED, !fdi[IDX_DISABLED].Selected);

    int len;
    len = strlen(fdi[IDX_CATALOG_EDIT].Data);
    rSetValue(hPluginRegistry, REG_CATALOG, REG_SZ, fdi[IDX_CATALOG_EDIT].Data, len);
    len = strlen(fdi[IDX_TIME_EDIT].Data);
    rSetValue(hPluginRegistry, REG_MAXTIME, REG_SZ, fdi[IDX_TIME_EDIT].Data, len);

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
        StringBuffer tname(type->getGroup());
        tname.append(DString(": ")).append(type->getDescription());
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
    info->Message(info->ModuleNumber, 0, "exception", &exceptionMessage[0], 4, 1);
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
    editor = getCurrentEditor();
    if (Event != EE_REDRAW) return 0;
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
    info->Message(info->ModuleNumber, 0, "exception", &exceptionMessage[0], 4, 1);
    disableColorer();
  };
  return 0;
}



void FarEditorSet::reloadBase()
{
  const char *marr[2] = { GetMsg(mName), GetMsg(mReloading) };
  const char *errload[5] = { GetMsg(mName), GetMsg(mCantLoad), 0, GetMsg(mFatal), GetMsg(mDie) };

  readRegistry();

  dropAllEditors();

  dropAllEditors();

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
  if (len > 1) hrdName = new SString(regstring);

  len = rGetValue(hPluginRegistry, REG_CATALOG, regstring, 512);
  SString *catalogPath = null;
  if (len > 1) catalogPath = new SString(regstring);

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
    editor->setMaxTime(rMaxTime);
  };
  return editor;
}


const char *FarEditorSet::GetMsg(int msg){
  return(info->GetMsg(info->ModuleNumber, msg));
}


void FarEditorSet::disableColorer(){
  rDisabled = true;
  rSetValue(hPluginRegistry, REG_DISABLED, rDisabled);
}


void FarEditorSet::readRegistry()
{
  char mt[64];

  rDisabled = rGetValue(hPluginRegistry, REG_DISABLED) != 0;

  drawCross = !rGetValue(hPluginRegistry, REG_CROSSDONTDRAW);
  drawPairs = !rGetValue(hPluginRegistry, REG_PAIRSDONTDRAW);
  drawSyntax = !rGetValue(hPluginRegistry, REG_SYNTAXDONTDRAW);

  rMaxTime = 3000;
  int len = rGetValue(hPluginRegistry, REG_MAXTIME, mt, 64);
  if (len){
    rMaxTime = atoi(mt);
  }

  for(FarEditor *fe = farEditorInstances.enumerate(); fe != null; fe = farEditorInstances.next()){
    fe->setDrawCross(drawCross);
    fe->setDrawPairs(drawPairs);
    fe->setDrawSyntax(drawSyntax);
    fe->setMaxTime(rMaxTime);
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