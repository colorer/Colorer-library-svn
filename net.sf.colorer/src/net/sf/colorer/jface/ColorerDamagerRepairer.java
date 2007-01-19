package net.sf.colorer.jface;

import java.util.Vector;

import net.sf.colorer.eclipse.ColorerPlugin;
import net.sf.colorer.eclipse.editors.DocumentLineSource;
import net.sf.colorer.editor.BaseEditor;
import net.sf.colorer.handlers.LineRegion;
import net.sf.colorer.handlers.StyledRegion;
import net.sf.colorer.impl.BaseEditorNative;
import net.sf.colorer.impl.Logger;
import net.sf.colorer.swt.ColorManager;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.DocumentEvent;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IDocumentExtension4;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITypedRegion;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.presentation.IPresentationDamager;
import org.eclipse.jface.text.presentation.IPresentationRepairer;
import org.eclipse.swt.custom.StyleRange;

public class ColorerDamagerRepairer implements IPresentationDamager,
        IPresentationRepairer, SyntaxConfiguration {

    private IDocument fDocument;
    private BaseEditor fBaseEditor;
    private ColorManager fColorManager;
    private long prevStamp = IDocumentExtension4.UNKNOWN_MODIFICATION_STAMP;
    
    public BaseEditor getBaseEditor() {
        return fBaseEditor;
    }

    public void setColorManager(ColorManager cm) {
        fColorManager = cm;
    }
    
    public IRegion getDamageRegion(ITypedRegion partition, DocumentEvent event,
            boolean documentPartitioningChanged)
    {
        if (event.getModificationStamp() != prevStamp){
            try {        
                fBaseEditor.modifyEvent(fDocument.getLineOfOffset(event.getOffset()));
                fBaseEditor.visibleTextEvent(0, fDocument.getNumberOfLines());
                fBaseEditor.lineCountEvent(fDocument.getNumberOfLines());
            }catch(BadLocationException e){};
        }

        Logger.trace("CDR", "getDamageRegion "+event.getOffset()+" "+event.getLength()+" "+event.getLength());
        if (fBaseEditor.getFileType() != null){
            prevStamp = event.getModificationStamp();
        }
        // Region from damage start till end of the text 
        return new Region(event.getOffset(), partition.getLength()-event.getOffset());
    }

    public void setDocument(IDocument document) {
        if (fDocument == document) return;
        fDocument = document;
        if (fBaseEditor != null){
            fBaseEditor.dispose();
        }
        fBaseEditor = new BaseEditorNative(ColorerPlugin.getDefaultPF(), new DocumentLineSource(fDocument));
        fBaseEditor.setRegionCompact(true);
    }

    public void createPresentation(TextPresentation presentation,
            ITypedRegion damage)
    {
        Logger.trace("CDR", "createPresentation "+damage.getOffset()+":"+damage.getLength());
        Vector styles = new Vector();
        
        try{
            int l_start = fDocument.getLineOfOffset(damage.getOffset());
            int l_end = fDocument.getLineOfOffset(damage.getOffset()+damage.getLength());
        
            for (int lno = l_start; lno <= l_end; lno++) {
                LineRegion[] lrarr = fBaseEditor.getLineRegions(lno);
                
                for (int idx = 0; idx < lrarr.length; idx++) {
                    LineRegion lr = lrarr[idx];
                    StyledRegion rdef = (StyledRegion) lr.rdef;
                    if (rdef == null)
                        continue;
                    if (lr.special)
                        continue;
                    int start = lr.start;
                    int end = lr.end;
                    if (end == -1)
                        end = fDocument.getLineLength(lno);
                    end = end - start;
                    start = fDocument.getLineOffset(lno) + start;
        
                    StyleRange sr = new StyleRange(start, end,
                            fColorManager.getColor(rdef.bfore, rdef.fore),
                            fColorManager.getColor(rdef.bback, rdef.back),
                            rdef.style);
                    styles.addElement(sr);
                }
            }
        }catch(BadLocationException e){
            Logger.error("CDR", "StyleRange fill error", e);
        };
        presentation.replaceStyleRanges((StyleRange[]) styles.toArray(new StyleRange[] {}));
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
 * The Original Code is the Colorer Library
 *
 * The Initial Developer of the Original Code is
 * Igor Russkih <irusskih at gmail dot com>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2007
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