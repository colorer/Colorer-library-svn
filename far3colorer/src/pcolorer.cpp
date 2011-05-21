#include"pcolorer.h"
#include"tools.h"
#include"FarEditorSet.h"

FarEditorSet *editorSet = NULL;
PluginStartupInfo Info;
FarStandardFunctions FSF;
StringBuffer *PluginPath;

extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )  
{
  switch( fdwReason ) 
  { 
  case DLL_PROCESS_ATTACH:
    {
      // obtain the path to the folder plugin, without being attached to the file name
      wchar_t path[MAX_PATH];
      if (!GetModuleFileName(hinstDLL, path, MAX_PATH)){
        return false;
      }
      DString module(path, 0);
      int pos = module.lastIndexOf('\\');
      pos = module.lastIndexOf('\\',pos);
      PluginPath=new StringBuffer(DString(module, 0, pos));
    }
    break;

  case DLL_PROCESS_DETACH:
    delete PluginPath;
    break;
  }

  return true;  
}
/**
  Returns message from FAR current language.
*/
const wchar_t *GetMsg(int msg)
{
  return(Info.GetMsg(&MainGuid, msg));
};

/**
  Global information about the plugin
*/
void WINAPI GetGlobalInfoW(struct GlobalInfo *Info)
{
  Info->StructSize=sizeof(GlobalInfo);
  Info->MinFarVersion=FARMANAGERVERSION;
  Info->Version=MAKEFARVERSION(1,0,3,1,VS_BETA);// need fix
  Info->Guid=MainGuid;
  Info->Title=L"FarColorer";
  Info->Description=L"Syntax highlighting in Far editor";
  Info->Author=L"Igor Ruskih, Dobrunov Aleksey, Eugene Efremov";
}

/**
  Plugin initialization and creation of editor set support class.
*/
void WINAPI SetStartupInfoW(const struct PluginStartupInfo *fei)
{
  Info = *fei;
  FSF = *fei->FSF;
  Info.FSF = &FSF;
};

/**
  Plugin strings in FAR interface.
*/
void WINAPI GetPluginInfoW(struct PluginInfo *nInfo)
{
  static wchar_t* PluginMenuStrings;
  memset(nInfo, 0, sizeof(*nInfo));
  nInfo->Flags = PF_EDITOR | PF_DISABLEPANELS;
  nInfo->StructSize = sizeof(*nInfo);
  nInfo->PluginConfig.Count = 1;
  nInfo->PluginMenu.Count = 1;
  PluginMenuStrings = (wchar_t*)GetMsg(mName);
  nInfo->PluginConfig.Strings = &PluginMenuStrings;
  nInfo->PluginMenu.Strings = &PluginMenuStrings;
  nInfo->PluginConfig.Guids=&PluginConfig;
  nInfo->PluginMenu.Guids=&PluginMenu;
  nInfo->CommandPrefix = L"clr";
};

/**
  On FAR exit. Destroys all internal structures.
*/
void WINAPI ExitFARW(const struct ExitInfo *Info)
{
  if (editorSet){
    delete editorSet;
  }
};

/**
  Open plugin configuration of actions dialog.
*/
HANDLE WINAPI OpenW(const struct OpenInfo *Info)
{
  if (Info->OpenFrom == OPEN_EDITOR){
    editorSet->openMenu();
  }
  else
    if (Info->OpenFrom == OPEN_COMMANDLINE){
      //file name, which we received
      wchar_t *file = (wchar_t*)Info->Data;

      wchar_t *nfile = PathToFull(file,true);
      if (nfile){
        if (!editorSet){
          editorSet = new FarEditorSet();
        }
        editorSet->viewFile(DString(nfile));
      }

      delete[] nfile;
    }

  return INVALID_HANDLE_VALUE;
};

/**
  Configures plugin.
*/
int WINAPI ConfigureW(const GUID* Guid)
{
  if (!editorSet){
    editorSet = new FarEditorSet();
  }
  // ReadSettings need for plugin off mode
  editorSet->ReadSettings();
  editorSet->configure(false);
  return 1;
};

/**
  Processes FAR editor events and
  makes text colorizing here.
*/
int WINAPI ProcessEditorEventW(int Event, void *Param)
{
  if (!editorSet){
    editorSet = new FarEditorSet();
  }
  return editorSet->editorEvent(Event, Param);
};

int WINAPI ProcessEditorInputW(const ProcessEditorInputInfo *Rec)
{
  return editorSet->editorInput(Rec);
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