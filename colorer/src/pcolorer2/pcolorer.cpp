#include"pcolorer.h"
#include<windows.h>
#include"FarEditorSet.h"

static struct PluginStartupInfo Info;
static struct FarStandardFunctions FSF;
FarEditorSet *editorSet = null;

/**
 * Returns message from FAR current language.
 */
const wchar_t *GetMsg(int msg)
{
	return(Info.GetMsg(Info.ModuleNumber, msg));
};

/**
 * Plugin initialization and creation of editor set support class.
 */
void WINAPI SetStartupInfoW(const struct PluginStartupInfo *fei)
{
	Info = *fei;
	FSF = *fei->FSF;
	Info.FSF = &FSF;
	editorSet = new FarEditorSet(&Info);
};

/**
 * Plugin strings in FAR interface.
 */
void WINAPI GetPluginInfoW(struct PluginInfo *nInfo)
{
	static wchar_t* PluginMenuStrings;
	memset(nInfo, 0, sizeof(*nInfo));
	nInfo->Flags = PF_EDITOR | PF_DISABLEPANELS;
	nInfo->StructSize = sizeof(*nInfo);
	nInfo->PluginConfigStringsNumber = 1;
	nInfo->PluginMenuStringsNumber = 1;
	PluginMenuStrings = (wchar_t*)GetMsg(mName);
	nInfo->PluginConfigStrings = &PluginMenuStrings;
	nInfo->PluginMenuStrings = &PluginMenuStrings;
	nInfo->CommandPrefix = L"clr";
};

/**
 * On FAR exit. Destroys all internal structures.
 */
void WINAPI ExitFARW()
{
	delete editorSet;
};

/**
 * Open plugin configuration of actions dialog.
 */
HANDLE WINAPI OpenPluginW(int OpenFrom, INT_PTR Item)
{
	if (OpenFrom == OPEN_EDITOR)
	{
		editorSet->openMenu();
	}
	else if (OpenFrom == OPEN_COMMANDLINE)
	{
		//имя файла, которое нам передали
		wchar_t *file = (wchar_t*)Item;
		int l=wcslen(file);
		
		wchar_t *file_exp=(wchar_t*)calloc(l,sizeof(wchar_t));
		// убираем кавычки, если присутствуют
		// ориентируясь по первому символу - если он " то убираем и первый и последний.
		// если первый кавычка,  а последний нет - ну это не наши проблемы, и так и так ошибка
		if ((l>0)&&(*file==L'"'))
		{
			wcsncpy_s(file_exp,l, &file[1],l-2);
			wcscpy_s(file,l,file_exp);
		}
		// заменяем переменные окружения на их значения
		int k=ExpandEnvironmentStrings(file,file_exp,l);
		if (!k) 
			wcscpy_s(file_exp,l,file);
		else
			if (k>=l) 
			{
				l=k;
				file_exp=(wchar_t*)realloc(file_exp,l*sizeof(wchar_t));
				if (!ExpandEnvironmentStrings(file,file_exp,l)) wcscpy_s(file_exp,l,file);
			}

		// получаем полный путь до файла, преобразовывая всякие ../ ./  и т.п.
		wchar_t *temp=NULL;
		int p=FSF.ConvertPath(CPM_FULL,file_exp,temp,0);
		temp=(wchar_t*)calloc(p,sizeof(wchar_t));
		FSF.ConvertPath(CPM_FULL,file_exp,temp,p);
		
		// для нормальной работы с длинными путями, путь нужно преобразовать в UNC
		if (wcsstr(temp,L"\\\\?\\")==NULL){
			file_exp=(wchar_t*)realloc(file_exp,(p+7)*sizeof(wchar_t));
			wcscpy(file_exp,L"\\\\?\\");
			wcscat(file_exp,temp);
			editorSet->viewFile(DString(file_exp));
		}
		else
			editorSet->viewFile(DString(temp));

		free(temp);
		free(file_exp);
	}
	else
		editorSet->configure();

	return INVALID_HANDLE_VALUE;
};

/**
 * Configures plugin.
 */
int WINAPI ConfigureW(int ItemNumber)
{
	editorSet->configure();
	return 1;
};

/**
 * Processes FAR editor events and
 * makes text colorizing here.
 */
int WINAPI ProcessEditorEventW(int Event, void *Param)
{
	return editorSet->editorEvent(Event, Param);
};
int WINAPI ProcessEditorInputW(const INPUT_RECORD*ir)
{
	return editorSet->editorInput(ir);
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