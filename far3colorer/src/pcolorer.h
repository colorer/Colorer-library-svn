#ifndef _PCOLORER_H_
#define _PCOLORER_H_

/**
  Visual Leak Detector http://sites.google.com/site/dmoulding/vld .
  Optional 
*/
#ifdef _DEBUG
#ifndef _WIN64
//#include<vld.h>
#endif
#endif

#include<plugin.hpp>
#include<farcolor.hpp>
#include<farkeys.hpp>
#include"ConsoleAnnotation.h"

#include<wctype.h>
#include<wchar.h>
#include<windows.h>
#include "registry_wide.h"
#include<unicode/StringBuffer.h>

#include <initguid.h>
// {D2F36B62-A470-418d-83A3-ED7A3710E5B5}
DEFINE_GUID(MainGuid,0xd2f36b62, 0xa470, 0x418d, 0x83, 0xa3, 0xed, 0x7a, 0x37, 0x10, 0xe5, 0xb5);
// {45453CAC-499D-4b37-82B8-0A77F7BD087C}
DEFINE_GUID(PluginMenu, 0x45453cac, 0x499d, 0x4b37, 0x82, 0xb8, 0xa, 0x77, 0xf7, 0xbd, 0x8, 0x7c);
// {87C92249-430D-4334-AC33-05E7423286E9}
DEFINE_GUID(PluginConfig, 0x87c92249, 0x430d, 0x4334, 0xac, 0x33, 0x5, 0xe7, 0x42, 0x32, 0x86, 0xe9);

extern PluginStartupInfo Info;
extern FarStandardFunctions FSF;
extern StringBuffer *PluginPath;

/** FAR .lng file identifiers. */
enum
{
  mName, mSetup, mTurnOff, mTrueMod,
  mCross, mPairs, mSyntax, mOldOutline,
  mOk, mReloadAll, mCancel,
  mCatalogFile, mHRDName,mHRDNameTrueMod,
  mListTypes, mMatchPair, mSelectBlock, mSelectPair,
  mListFunctions, mFindErrors, mSelectRegion, mLocateFunction,
  mUpdateHighlight, mReloadBase, mConfigure,
  mTotalTypes, mSelectSyntax, mOutliner, mNothingFound,
  mGotcha, mChoose,
  mReloading, mCantLoad, mCantOpenFile, mDie, mTry,
  mFatal, mSelectHRD, mChangeBackgroundEditor, mTrueModSetting,
  mNoFarTM, mNoConEmu, mNoFarTMConEmu, mUserHrdFile, mUserHrcFile, mUserHrcSetting,
  mUserHrcSettingDialog, mListSyntax, mParamList,mParamValue
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
 * Cail Lomecb <irusskih at gmail.com>.
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