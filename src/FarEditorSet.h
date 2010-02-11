#ifndef _FAREDITORSET_H_
#define _FAREDITORSET_H_

#include<colorer/handlers/FileErrorHandler.h>
#include<colorer/handlers/LineRegionsSupport.h>
#include<colorer/handlers/StyledHRDMapper.h>
#include<colorer/viewer/TextConsoleViewer.h>
#include<common/Logging.h>
#include<unicode/Encodings.h>

#include "pcolorer.h"
#include "tools.h"
#include "FarEditor.h"

//registry keys
const wchar_t cRegEnabled[] = L"Enabled";
const wchar_t cRegHrdName[] = L"HrdName";
const wchar_t cRegCatalog[] = L"Catalog";
const wchar_t cRegCrossDraw[] = L"CrossDraw";
const wchar_t cRegPairsDraw[] = L"PairsDraw";
const wchar_t cRegSyntaxDraw[] = L"SyntaxDraw";
const wchar_t cRegOldOutLine[] = L"OldOutlineView";

//values of registry keys by default
const bool cEnabledDefault = true;
const wchar_t cHrdNameDefault[] = L"default";
const wchar_t cCatalogDefault[] = L"";
const int cCrossDrawDefault = 2;
const bool cPairsDrawDefault = true;
const bool cSyntaxDrawDefault = true;
const bool cOldOutLineDefault = true;

enum
{ IDX_BOX, IDX_ENABLED, IDX_CROSS, IDX_PAIRS, IDX_SYNTAX, IDX_OLDOUTLINE,
  IDX_CATALOG, IDX_CATALOG_EDIT, IDX_HRD, IDX_HRD_SELECT, IDX_RELOAD_ALL,
  IDX_OK, IDX_CANCEL};

LONG_PTR WINAPI SettingDialogProc(HANDLE hDlg, int Msg, int Param1, LONG_PTR Param2);

/**
 * FAR Editors container.
 * Manages all library resources and creates FarEditor class instances.
 * @ingroup far_plugin
 */
class FarEditorSet
{
	public:
		/** Creates set and initialises it with PluginStartupInfo structure */
		FarEditorSet();
		/** Standard destructor */
		~FarEditorSet();

		/** Shows editor actions menu */
		void openMenu();
		/** Shows plugin's configuration dialog */
		void configure();
		/** Views current file with internal viewer */
		void viewFile(const String &path);

		/** Dispatch editor event in the opened editor */
		int  editorEvent(int Event, void *Param);
		/** Dispatch editor input event in the opened editor */
		int  editorInput(const INPUT_RECORD *ir);

    /** Get the description of HRD, or parameter name if description=null */
		const String *getHRDescription(const String &name);
    /** Shows dialog with HRD scheme selection */
		const String *chooseHRDName(const String *current);
		
    /** Reads all registry settings into variables */
		void ReadSettings();
		/**
		 * trying to load the database on the specified path
		 */
		bool TestLoadBase(const wchar_t *hrdName, const wchar_t *catalogPath, const int full);
    SString *GetCatalogPath() {return sCatalogPath;}
    bool GetPluginStatus() {return rEnabled;}

    SString *sTempHrdName;
	private:
		/** Returns current global error handler. */
		ErrorHandler *getErrorHandler();
    /** Returns new editor. */
    FarEditor *addNewEditor();
		/** Returns currently active editor. */
		FarEditor *getCurrentEditor();
		/**
		 * Reloads HRC database.
		 * Drops all currently opened editors and their
		 * internal structures. Prepares to work with newly
		 * loaded database. Read settings from registry
		 */
		void ReloadBase();

		/** Shows dialog of file type selection */
		void chooseType();
		/** FAR localized messages */
		const wchar_t *GetMsg(int msg);
    /** Applies the current settings for editors*/
		void ApplySettingsToEditors();
		/** writes the default settings in the registry*/
    void SetDefaultSettings();
    void SaveSettings();

		/** Kills all currently opened editors*/
		void dropAllEditors();
		/** Disables all plugin processing*/
		void disableColorer();

		Hashtable<FarEditor*> farEditorInstances;
		ParserFactory *parserFactory;
		RegionMapper *regionMapper;
		HRCParser *hrcParser;

		HKEY hPluginRegistry;

		/** registry settings */
		bool rEnabled; // status plugin
		int drawCross;
		bool drawPairs; 
    bool drawSyntax;
		bool oldOutline;
    SString *sHrdName;
	  SString *sCatalogPath;
    SString *sCatalogPathExp;
};

#endif
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