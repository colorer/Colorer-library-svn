package net.sf.colorer.eclipse.editors;

import java.util.ArrayList;
import java.util.List;

import net.sf.colorer.eclipse.EclipsecolorerPlugin;
import net.sf.colorer.eclipse.PreferencePage;

import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.text.source.SourceViewerConfiguration;

public class ColorerSourceViewerConfiguration extends SourceViewerConfiguration {

  /** Copied from org.eclipse.jdt.ui.text.JavaSourceViewerConfiguration */
  public String[] getIndentPrefixes(ISourceViewer sourceViewer, String contentType) {
	  List list = new ArrayList();
	  // prefix[0] is either '\t' or ' ' x tabWidth, depending on useSpaces
    IPreferenceStore prefStore = EclipsecolorerPlugin.getDefault().getPreferenceStore();
    int tabWidth = prefStore.getInt(PreferencePage.TAB_WIDTH);
    boolean useSpaces = prefStore.getBoolean(PreferencePage.SPACES_FOR_TABS);
		for (int i= 0; i <= tabWidth; i++) {
		    StringBuffer prefix = new StringBuffer();
			if (useSpaces) {
			    for (int j = 0; j + i < tabWidth; j++)
			    	prefix.append(' ');
		    	
				if (i != 0)
		    		prefix.append('\t');				
			} else {    
			    for (int j = 0; j < i; j++)
			    	prefix.append(' ');
				if (i != tabWidth)
		    		prefix.append('\t');
			}
			list.add(prefix.toString());
		}
		list.add("");
		return (String[]) list.toArray(new String[list.size()]);
	}

	public int getTabWidth(ISourceViewer sourceViewer) {
		return EclipsecolorerPlugin.getDefault().getPreferenceStore().getInt(PreferencePage.TAB_WIDTH);
	}

	public ColorerSourceViewerConfiguration() {
		super();
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