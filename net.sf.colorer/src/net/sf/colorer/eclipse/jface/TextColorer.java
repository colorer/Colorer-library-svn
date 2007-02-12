package net.sf.colorer.eclipse.jface;

import java.util.Timer;
import java.util.TimerTask;

import net.sf.colorer.FileType;
import net.sf.colorer.eclipse.editors.ColorerEditor;
import net.sf.colorer.editor.BaseEditor;
import net.sf.colorer.editor.PairMatch;
import net.sf.colorer.handlers.LineRegion;
import net.sf.colorer.handlers.RegionDefine;
import net.sf.colorer.handlers.RegionMapper;
import net.sf.colorer.handlers.StyledRegion;
import net.sf.colorer.impl.Logger;
import net.sf.colorer.swt.ColorManager;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.DocumentEvent;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IDocumentExtension4;
import org.eclipse.jface.text.IDocumentListener;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextInputListener;
import org.eclipse.jface.text.ITextListener;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.ITextViewerExtension5;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.TextEvent;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.presentation.IPresentationDamager;
import org.eclipse.jface.text.presentation.IPresentationReconciler;
import org.eclipse.jface.text.presentation.IPresentationRepairer;
import org.eclipse.jface.text.reconciler.IReconcilingStrategy;
import org.eclipse.jface.text.source.projection.ProjectionViewer;
import org.eclipse.swt.custom.LineBackgroundEvent;
import org.eclipse.swt.custom.LineBackgroundListener;
import org.eclipse.swt.custom.StyleRange;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.events.TraverseEvent;
import org.eclipse.swt.events.TraverseListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.ScrollBar;

/**
 * JFace based syntax highlighting implementation using
 * Colorer library. <a href='http://colorer.sf.net/'>http://colorer.sf.net/</a>
 */
public class TextColorer implements IAdaptable
{

    /**
     * Reconcyler, merged with damager and repairer.
     * To be run incrementally in background
     */
    class AsyncReconcyler implements IPresentationReconciler,
                    ITextListener, ITextInputListener, Runnable {

        static final int ASYNC_DELAY = 1000;
        static final int MAX_SINGLE_PARSE_SIZE = 30000;
        static final int INCREMENTAL_PARSE_SIZE = 5000;
        
        private IRegion fDamage;
        private Display fDisplay;
        
        public IPresentationDamager getDamager(String contentType) {
            return null;
        }
        public IPresentationRepairer getRepairer(String contentType) {
            return null;
        }

        public void install(ITextViewer viewer) {
            if (Logger.TRACE){
                Logger.trace("CDR", "install");
            }
            fViewer = viewer;
            if (fViewer instanceof ITextViewerExtension5) {
                fProjectionViewer = (ProjectionViewer)fViewer;
            }
            fViewer.addTextListener(this);
            fViewer.addTextInputListener(this);
            
            fDisplay = Display.getCurrent();
            new Thread(this).start();
        }

        public void uninstall() {
            if (fViewer != null){
                fViewer.removeTextListener(this);
                fViewer.removeTextInputListener(this);
            }
            fViewer = null;
        }
        
        void setDocument(IDocument document)
        {
            if (Logger.TRACE){
                Logger.trace("CDR", "setDocument");
            }
            fDocument = document;
            
            if (fDocument == null) return;
            
//            fDocument.addDocumentListener(this);
//            documentChanged(null);
            
            prevStamp = -1;
            fColorManager = (ColorManager)fEditor.getAdapter(ColorManager.class);

            fBaseEditor = (BaseEditor)fEditor.getAdapter(BaseEditor.class);
            fBaseEditor.setRegionCompact(true);

            attach(fViewer.getTextWidget());
        }

        /**
         *  Forms damage region from damage start till end of the text,
         *  Sends document change notifications to baseEditor parser.
         */ 
        IRegion getDamageRegion(DocumentEvent event)
        {
            long modStamp = -1;
            
            if (event == null) return null;
            
            modStamp = event.getModificationStamp();
            if (modStamp != prevStamp){
                try {        
                    if (Logger.TRACE){
                        Logger.trace("CDR", "getDamageRegion: sending modify event: "+ fDocument.getLineOfOffset(event.getOffset()));
                    }
                    fBaseEditor.modifyEvent(fDocument.getLineOfOffset(event.getOffset()));
                    fBaseEditor.lineCountEvent(fDocument.getNumberOfLines());
                }catch(BadLocationException e){};
            }
            try{
                prevStamp = event.getModificationStamp();
                int soffset = fDocument.getLineInformationOfOffset(event.getOffset()).getOffset();
                // Join with already existing damage
                if (fDamage != null && soffset > fDamage.getOffset()) {
                    soffset = fDamage.getOffset();
                }
                // Create new damage region
                if (Logger.TRACE){
                    Logger.trace("CDR", "getDamageRegion "+soffset+":"+(fDocument.getLength() - soffset));
                }
                return new Region(soffset, fDocument.getLength() - soffset);
            }catch(BadLocationException e){
                Logger.error("CDR", "getDamageRegion", e);
            }
            return null;
        }

        /**
         * Creates presentation for the passed in damage region.
         * @param presentation Object to fillup with styles
         * @param damage Region to recover (line-based)
         */
        void createPresentation(TextPresentation presentation, IRegion damage)
        {
            if (Logger.TRACE){
                Logger.trace("CDR", "createPresentation "+damage.getOffset()+":"+damage.getLength());
            }
            int fpos = -1;
            try{
                int l_start = fDocument.getLineOfOffset(damage.getOffset());
                int l_end = fDocument.getLineOfOffset(damage.getOffset()+damage.getLength());
                // Minus 1 to exclude last line which is out of region
                l_end--;
                Logger.trace("CDR", "createPresentation: filling "+l_start+":"+l_end);
            
                for (int lno = l_start; lno <= l_end; lno++) {
                    LineRegion[] lrarr = fBaseEditor.getLineRegions(lno);
                    
                    for (int idx = 0; idx < lrarr.length; idx++) {
                        LineRegion lr = lrarr[idx];
                        StyledRegion rdef = (StyledRegion) lr.rdef;
                        if (rdef == null) continue;
                        if (lr.special) continue;
                        
                        IRegion lineinfo = fDocument.getLineInformation(lno);

                        int start = lr.start;
                        int end = lr.end;
                        if (end == -1) end = lineinfo.getLength();
                        int length = end - start;
                        start = lineinfo.getOffset() + start;
                        
                        if (length == 0) {
                            continue;
                        }
                        
                        if (fpos > start) {
                            Logger.error("CDR", "------------ createPresentation inconsistent");
                        }
                        fpos = start+length;

                        StyleRange sr = new StyleRange(start, length,
                                fColorManager.getColor(rdef.bfore, rdef.fore),
                                fColorManager.getColor(rdef.bback, rdef.back),
                                rdef.style);
                        presentation.addStyleRange(sr);
                    }
                }
                Logger.trace("CDR", "createPresentation: filled");
            }catch(BadLocationException e){
                Logger.error("CDR", "StyleRange fill error", e);
            };
        }
        
        
        /**
         * Runs reconciling process for the damaged document.
         */
        public void textChanged(TextEvent event) {
            if (fDocument == null) return;
            
            if (Logger.TRACE){
                Logger.trace("TextColorer", "textChanged");
            }
            
            IRegion newDamage = getDamageRegion(event.getDocumentEvent());

            if (newDamage != null)
            {
                fDamage = newDamage;
            }
            
            repairPresentation(true);
            
            fModTimestamp = System.currentTimeMillis();
        }

        /**
         * Activates asynchronous presentation repair.
         */
        public void run()
        {
            while(fViewer != null && !fDisplay.isDisposed())
            {
                fDisplay.asyncExec(new Runnable() {
                    public void run() {
                        if (System.currentTimeMillis() > fModTimestamp+ASYNC_DELAY-200)
                            repairPresentation(false);
                    }
                });

                try{
                    Thread.sleep(ASYNC_DELAY);
                }catch(Exception e){}
                
            }
        }
        
        public void repairPresentation(boolean visual)
        {
            if (fDamage == null) return;

            if (Logger.TRACE){
                Logger.trace("CDR", "Presentation change started");
            }
            
            try{
                int newlen = fViewer.getBottomIndexEndOffset() - fDamage.getOffset();
                // Damage is below visible screen - stop repair if only visual part requested
                if (visual && newlen <= 0){
                    return; 
                }
                // Damage is below visible screen - parse incrementally
                if (newlen <= 0) newlen = INCREMENTAL_PARSE_SIZE;
                // Just to be sure
                if (newlen+10 > fDamage.getLength()) newlen = fDamage.getLength();
                // Clip long damages to process them incrementally
                newlen = Math.min(newlen, MAX_SINGLE_PARSE_SIZE);

                // Align length by line's end
                int endline = fDocument.getLineOfOffset(fDamage.getOffset()+newlen);
                newlen = fDocument.getLineOffset(endline)+fDocument.getLineLength(endline)-fDamage.getOffset();

                IRegion visibleDamage = new Region(fDamage.getOffset(), newlen);
                if (visibleDamage.getLength() == 0) return;

                TextPresentation presentation = new TextPresentation(visibleDamage, 10+newlen/10);

                createPresentation(presentation, visibleDamage);

//                if (true) return;

                // Clear range!
                IRegion widgetDamage = fProjectionViewer.modelRange2WidgetRange(visibleDamage);
                text.setStyleRanges(widgetDamage.getOffset(), widgetDamage.getLength(), null, null);

                int newstart = visibleDamage.getOffset()+visibleDamage.getLength();
                fDamage = new Region(newstart, fDamage.getOffset()+fDamage.getLength()-newstart);
                
                if (fDamage.getLength() == 0){
                    fDamage = null;
                }

                fViewer.changeTextPresentation(presentation, false);

                if (Logger.TRACE){
                    Logger.trace("CDR", "Presentation change finished");
                }

            }catch(Exception e){
                Logger.error("CDR", "runnable repairer failed", e);
            }
        }
        
        /**
         * @see org.eclipse.jface.text.ITextInputListener#inputDocumentAboutToBeChanged(org.eclipse.jface.text.IDocument, org.eclipse.jface.text.IDocument)
         */
        public void inputDocumentAboutToBeChanged(IDocument oldInput, IDocument newInput) {
        }
        /**
         * @see org.eclipse.jface.text.ITextInputListener#inputDocumentChanged(org.eclipse.jface.text.IDocument, org.eclipse.jface.text.IDocument)
         */
        public void inputDocumentChanged(IDocument oldInput, IDocument newInput) {
            if (newInput != null){
                setDocument(newInput);
            }
        }
        public IReconcilingStrategy getReconcilingStrategy(String contentType) {
            return null;
        }
    }



    /**
     * General StyledText widget helper. Links StyledText with colorer's
     * BaseEditor API
     */
    class WidgetEventHandler implements KeyListener,
            DisposeListener, LineBackgroundListener,
            PaintListener, TraverseListener, ControlListener,
            MouseListener, SelectionListener
    {
    
        public void widgetDisposed(DisposeEvent e) {
            detach();
        };
    
        public void lineGetBackground(LineBackgroundEvent e) {
            int lno = text.getLineAtOffset(e.lineOffset);
            int caret = text.getCaretOffset();
            int length = e.lineText.length();
    
            if (horzCrossColor != null && horzCross && lineHighlighting
                    && e.lineOffset <= caret && caret <= e.lineOffset + length) {
                e.lineBackground = fColorManager.getColor(
                        ((StyledRegion) horzCrossColor).bback,
                        ((StyledRegion) horzCrossColor).back);
                return;
            }
            if (!fullBackground)
                return;
            LineRegion[] lr = fBaseEditor.getLineRegions(lno);
            for (int idx = 0; idx < lr.length; idx++) {
                StyledRegion rdef = (StyledRegion) lr[idx].rdef;
                if (lr[idx].end == -1 && rdef != null)
                    e.lineBackground = fColorManager.getColor(rdef.bback, rdef.back);
            }
        }
    
        public void paintControl(PaintEvent e) {
            stateChanged();
            if (!pairsHighlighting)
                return;
            pairsDraw(e.gc, currentPair);
        }
    
        // ------------------
        public void keyPressed(KeyEvent e) {
            stateChanged();
        }
    
        public void keyReleased(KeyEvent e) {
        }
    
        public void keyTraversed(TraverseEvent e) {
            stateChanged();
        }
    
        // ------------------
    
        public void controlMoved(ControlEvent e) {
        }
    
        public void controlResized(ControlEvent e) {
            stateChanged();
        }
    
        // ------------------
    
        public void mouseDoubleClick(MouseEvent e) {
        }
    
        public void mouseDown(MouseEvent e) {
            stateChanged();
        }
    
        public void mouseUp(MouseEvent e) {
        }
    
        // ------------------
    
        public void widgetDefaultSelected(SelectionEvent e) {
        }
        public void widgetSelected(SelectionEvent e) {
            stateChanged();
        }
    }


    public final static int HLS_NONE = 0;
    public final static int HLS_XOR = 1;
    public final static int HLS_OUTLINE = 2;
    public final static int HLS_OUTLINE2 = 3;

    int highlightStyle = HLS_XOR;

    boolean fullBackground = false;
    boolean vertCross = false;
    boolean horzCross = false;

    RegionDefine vertCrossColor = null;
    RegionDefine horzCrossColor = null;

    PairMatch currentPair = null;

    int prevLine = 0;
    int visibleStart, visibleEnd;
    long prevStamp = IDocumentExtension4.UNKNOWN_MODIFICATION_STAMP;
    long fModTimestamp;

    boolean lineHighlighting = true;
    boolean pairsHighlighting = true;

    private IDocument fDocument;

    private ColorManager fColorManager;
    private BaseEditor fBaseEditor;
    private ColorerEditor fEditor;
    private StyledText text;

    private ITextViewer fViewer;
    private ProjectionViewer fProjectionViewer;

    private WidgetEventHandler fHandler = new WidgetEventHandler();
    private AsyncReconcyler fReconciler = new AsyncReconcyler();

    
    /**
     * Common TextColorer creation constructor. Creates TextColorer object,
     * which is to be attached to the StyledText widget.
     * 
     * @param pf Parser factory, used to create all coloring text parsers.
     * @param cm Color Manager, used to store cached color objects
     */
    public TextColorer(ColorerEditor editor)
    {
        fEditor = editor;
    }


    /**
     * Installs this highlighter into the specified StyledText object. Client
     * can manually call detach() method, when wants to destroy this object.
     */
    void attach(StyledText parent) {

        text = parent;
        text.addDisposeListener(fHandler);
        text.addLineBackgroundListener(fHandler);
        text.addPaintListener(fHandler);

        text.addControlListener(fHandler);
        text.addKeyListener(fHandler);
        text.addTraverseListener(fHandler);
        text.addMouseListener(fHandler);
        text.addSelectionListener(fHandler);

        ScrollBar sb = text.getVerticalBar();
        if (sb != null)
            sb.addSelectionListener(fHandler);
        
        updateViewport();
    }

    /**
     * Removes this object from the corresponding StyledText widget. Object
     * can't be used after this call, until another attach. This method is
     * called automatically, when StyledText widget is disposed
     */
    void detach() {
        if (text == null)
            return;
        text.removeDisposeListener(fHandler);
        text.removeLineBackgroundListener(fHandler);
        text.removePaintListener(fHandler);
        text.removeControlListener(fHandler);
        text.removeKeyListener(fHandler);
        text.removeTraverseListener(fHandler);
        text.removeMouseListener(fHandler);
        text.removeSelectionListener(fHandler);
        ScrollBar sb = text.getVerticalBar();
        if (sb != null)
            sb.removeSelectionListener(fHandler);
        text = null;
    }

    public void relink() {
        fBaseEditor = (BaseEditor)fEditor.getAdapter(BaseEditor.class);
        updateViewport();
        invalidateSyntax();
    }

    public void invalidateSyntax() {
        fBaseEditor.modifyEvent(0);
        prevStamp = IDocumentExtension4.UNKNOWN_MODIFICATION_STAMP;
        fViewer.invalidateTextPresentation();
    }
    
    void checkActive() {
        if (text == null) {
            throw new RuntimeException("Object is not attached to StyledText");
        }
    }

    /**
     * Selects and installs coloring style (filetype) according to filename
     * string and current first line of text.
     * 
     * @param filename
     *            File name to be used to autodetect filetype
     */
    public FileType chooseFileType(String filename) {
        checkActive();
        FileType selected = fBaseEditor.chooseFileType(filename);
        invalidateSyntax();
        return selected;
    }

    /**
     * Selects and installs specified file type.
     * 
     * @param typename
     *            Name or description of HRC filetype.
     */
    public void setFileType(FileType typename) {
        checkActive();
        fBaseEditor.setFileType(typename);
        invalidateSyntax();
    }

    /**
     * Returns currently used file type.
     */
    public FileType getFileType() {
        checkActive();
        return fBaseEditor.getFileType();
    }

    /**
     * Returns current low-level BaseEditor object implementation
     */
    public BaseEditor getBaseEditor() {
        return fBaseEditor;
    }

    /**
     * Changes style/coloring scheme into the specified.
     * 
     * @param regionMapper
     *            External RegionMapper object
     * @param useBackground
     *            If true, native HRD background properties would be assigned to
     *            colored StyledText.
     */
    public void setRegionMapper(RegionMapper regionMapper, boolean useBackground) {
        fBaseEditor.setRegionMapper(regionMapper);

        StyledRegion sr = (StyledRegion) fBaseEditor.getBackground();
        text.setForeground(null);
        text.setBackground(null);
        if (useBackground) {
            text.setForeground(fColorManager.getColor(sr.bfore, sr.fore));
            text.setBackground(fColorManager.getColor(sr.bback, sr.back));
        };
        setCross(vertCross, horzCross);
        invalidateSyntax();
    }

    /**
     * Changes style/coloring scheme into the specified.
     * 
     * @param name
     *            Name of color scheme (HRD name).
     * @param useBackground
     *            If true, native HRD background properties would be assigned to
     *            colored StyledText.
     */
    public void setRegionMapper(String hrdName, boolean useBackground) {
        fBaseEditor.setRegionMapper(StyledRegion.HRD_RGB_CLASS, hrdName);

        StyledRegion sr = (StyledRegion) fBaseEditor.getBackground();
        text.setForeground(null);
        text.setBackground(null);
        if (useBackground) {
            text.setForeground(fColorManager.getColor(sr.bfore, sr.fore));
            text.setBackground(fColorManager.getColor(sr.bback, sr.back));
        };
        setCross(vertCross, horzCross);
        invalidateSyntax();
    }

    /**
     * Inlined languages background coloring.
     * 
     * @param full
     *            If true, background color of other language insertions (jsp,
     *            php) would be painted till end of line. If false, only text
     *            will be painted with marked color.
     */
    public void setFullBackground(boolean full) {
        fullBackground = full;
        invalidateSyntax();
    }

    /**
     * Specifies visibility of cross at the cursor position.
     * 
     * @param vert
     *            Not used
     */
    public void setCross(boolean horz, boolean vert) {
        horzCross = horz;
        vertCross = vert;
        vertCrossColor = null;
        horzCrossColor = null;
        if (horzCross)
            horzCrossColor = fBaseEditor.getHorzCross();
        if (vertCross)
            vertCrossColor = fBaseEditor.getVertCross();
        fViewer.invalidateTextPresentation();
    }

    /**
     * Paint paired constructions or not.
     * 
     * @param paint
     *            Paint Matched pairs or not.
     * @param style
     *            One of TextColorer.HLS_XOR, TextColorer.HLS_OUTLINE or
     *            TextColorer.HLS_OUTLINE2
     */
    public void setPairsPainter(int style) {
        highlightStyle = style;
        text.redraw();
    }

    /** Checks if caret positioned on highlighted pair. */
    public boolean pairAvailable() {
        return currentPair != null;
    }

    /** Moves caret to the position of currently active pair. */
    public boolean matchPair() {
        if (currentPair == null)
            return false;
        try{
            int caret = fProjectionViewer.widgetOffset2ModelOffset(text.getCaretOffset());
            int lno = fDocument.getLineOfOffset(caret);
            PairMatch cp = fBaseEditor.getPairMatch(lno, caret-
                    fDocument.getLineOffset(lno));
            fBaseEditor.searchGlobalPair(cp);
            if (cp.end == null)
                return false;
            int position = fDocument.getLineOffset(cp.eline);
            if (cp.topPosition)
                position += cp.end.end;
            else
                position += cp.end.start;
            fEditor.selectAndReveal(position, 0);
            return true;
        }catch(BadLocationException e){
            Assert.isTrue(false, "Never reach");
        }
        return false;
    }

    /** Selects a content of the currently positioned pair. */
    public boolean selectPair() {
        if (currentPair == null)
            return false;
        try{
            int caret = fProjectionViewer.widgetOffset2ModelOffset(text.getCaretOffset());
            int lno = fDocument.getLineOfOffset(caret);
            PairMatch cp = fBaseEditor.getPairMatch(lno, caret-
                    fDocument.getLineOffset(lno));
            fBaseEditor.searchGlobalPair(cp);
            if (cp.end == null)
                return false;
            if (cp.topPosition)
                fEditor.selectAndReveal(fDocument.getLineOffset(cp.sline)+
                        cp.start.start,
                        fDocument.getLineOffset(cp.eline)+
                        cp.end.end);
            else
                fEditor.selectAndReveal(fDocument.getLineOffset(cp.eline)+
                        cp.end.start,
                        fDocument.getLineOffset(cp.sline)+
                        cp.start.end);
            return true;
        }catch(BadLocationException e){
            Assert.isTrue(false, "Never reach");
        }
        return false;
    }

    /** Selects an internal part of the currently selected paired content */
    public boolean selectContentPair() {
        if (currentPair == null)
            return false;
        
        try{
            int caret = fProjectionViewer.widgetOffset2ModelOffset(text.getCaretOffset());
            int lno = fDocument.getLineOfOffset(caret);
            PairMatch cp = fBaseEditor.getPairMatch(lno, caret-
                    fDocument.getLineOffset(lno));
            fBaseEditor.searchGlobalPair(cp);
            if (cp.end == null)
                return false;
            if (cp.topPosition)
                fEditor.selectAndReveal(fDocument.getLineOffset(cp.sline)+
                        cp.start.end,
                        fDocument.getLineOffset(cp.eline)+
                        cp.end.start);
            else
                fEditor.selectAndReveal(fDocument.getLineOffset(cp.eline)+
                        cp.end.end,
                        fDocument.getLineOffset(cp.sline)+
                        cp.start.start);
            return true;
        }catch(BadLocationException e){
            Assert.isTrue(false, "Never reach");
        }
        return false;
   }

    /**
     * Retrieves current LineRegion under caret.
     * 
     * @return LineRegion currently under Caret
     */
    public LineRegion getCaretRegion() {
        LineRegion caretRegion = null;
        try{
            int caret = fProjectionViewer.widgetOffset2ModelOffset(text.getCaretOffset());
            int lno = fDocument.getLineOfOffset(caret);
        
            int linepos = caret - fDocument.getLineOffset(lno);

            LineRegion[] arr = fBaseEditor.getLineRegions(lno);
            if (arr == null) {
                return null;
            }
            for (int idx = 0; idx < arr.length; idx++) {
                if (arr[idx].start <= linepos && arr[idx].end > linepos
                        && !arr[idx].special) {
                    caretRegion = arr[idx];
                }
            }
            return caretRegion;
        }catch(BadLocationException e){
            Assert.isTrue(false, "Never reach");
        }
        return null;
    }

    /**
     * Informs colorer about visible state change of the editor
     */
    void stateChanged()
    {
        
        
        fModTimestamp = System.currentTimeMillis();

        if (Logger.TRACE){
            Logger.trace("TextColorer", "stateChanged");
        }
        
        updateViewport();

        try{
            int curOffset = fProjectionViewer.widgetOffset2ModelOffset(text.getCaretOffset());
            int curLine = fDocument.getLineOfOffset(curOffset);

            if (lineHighlighting && text.getSelectionRange().y != 0) {
                lineHighlighting = false;
                drawLine(prevLine);
                pairsHighlighting = false;
                pairsDraw(null, currentPair);
                return;
            }
            if (text.getSelectionRange().y != 0) {
                return;
            }
            /* do not analize pairs if cursor is out of visible area */
            if (curLine < visibleStart || curLine > visibleEnd) {
                return;
            }
            if (!lineHighlighting) {
                // drawing current line
                lineHighlighting = true;
                drawLine(curLine);
            } else if (curLine != prevLine) {
                drawLine(prevLine);
                drawLine(curLine);
                prevLine = curLine;
            }
            // drawing current pairs
            if (!pairsHighlighting) {
                pairsHighlighting = true;
                pairsDraw(null, currentPair);
            } else {
                int lineOffset = fDocument.getLineOffset(curLine);
                PairMatch newmatch = fBaseEditor.getPairMatch(curLine,
                        curOffset - lineOffset);
                if (newmatch != null)
                    fBaseEditor.searchLocalPair(newmatch);
                if ((newmatch == null && currentPair != null)
                        || (newmatch != null && !newmatch.equals(currentPair))) {
                    pairsDraw(null, currentPair);
                    pairsDraw(null, newmatch);
                }
                currentPair = newmatch;
            }
        }catch(BadLocationException e){
            Assert.isTrue(false, "Never reach");
        }
    }

    void updateViewport() {
        checkActive();
        
        if (Logger.TRACE){
            Logger.trace("TextColorer", "updateViewport");
        }

        visibleStart = 0;
        try {
            visibleStart = fViewer.getTopIndex() - 1;
        } catch (Exception e) {
            e.printStackTrace(System.out);
        }
        if (visibleStart < 0) visibleStart = 0;
        visibleEnd = fViewer.getBottomIndex();
        int lc = fDocument.getNumberOfLines();
        if (visibleEnd > lc) visibleEnd = lc;

        fBaseEditor.visibleTextEvent(visibleStart, visibleEnd - visibleStart);
        fBaseEditor.lineCountEvent(fDocument.getNumberOfLines());
        
        fReconciler.repairPresentation(true);
    }

    void pairDraw(GC gc, StyledRegion sr, int start, int end) {
        if (start > text.getCharCount() || end > text.getCharCount())
            return;
        if (gc != null) {
            Point left = text.getLocationAtOffset(start);
            Point right = text.getLocationAtOffset(end);
            if (sr != null) {
                if (highlightStyle == HLS_XOR) {
                    int resultColor = sr.fore
                            ^ fColorManager.getColor(text.getBackground());
                    if (text.getLineAtOffset(text.getCaretOffset()) == text.getLineAtOffset(start)
                            && horzCross
                            && horzCrossColor != null
                            && ((StyledRegion) horzCrossColor).bback)
                        resultColor = sr.fore ^ ((StyledRegion) horzCrossColor).back;
                    Color color = fColorManager.getColor(sr.bfore, resultColor);
                    gc.setBackground(color);
                    gc.setXORMode(true);
                    gc.fillRectangle(left.x, left.y, right.x - left.x, gc.getFontMetrics().getHeight());
                } else if (highlightStyle == HLS_OUTLINE) {
                    Color color = fColorManager.getColor(sr.bfore, sr.fore);
                    gc.setForeground(color);
                    gc.drawRectangle(left.x, left.y, right.x - left.x - 1, gc.getFontMetrics().getHeight() - 1);
                } else if (highlightStyle == HLS_OUTLINE2) {
                    Color color = fColorManager.getColor(sr.bfore, sr.fore);
                    gc.setForeground(color);
                    gc.setLineWidth(2);
                    gc.drawRectangle(left.x + 1, left.y + 1, right.x - left.x - 2, gc.getFontMetrics().getHeight() - 2);
                }
            }
        } else {
            text.redrawRange(start, end - start, true);
        }
    }

    void pairsDraw(GC gc, PairMatch pm) {
        if (pm == null)
            return;
        try{
            if (pm.start != null) {
                /* Do not draw pairs if currently invisible */
                if (pm.sline < visibleStart || pm.sline > visibleEnd) return;
    
                int lineOffset = fDocument.getLineOffset(pm.sline);
                int drawStart = fProjectionViewer.modelOffset2WidgetOffset(lineOffset);
                if (drawStart == -1) return;
                pairDraw(gc, (StyledRegion) pm.start.rdef, pm.start.start
                        + drawStart, pm.start.end + drawStart);
            }
            if (pm.end != null) {
                if (pm.eline < visibleStart || pm.eline > visibleEnd) return;
    
                int lineOffset = fDocument.getLineOffset(pm.eline);
                int drawStart = fProjectionViewer.modelOffset2WidgetOffset(lineOffset);
                if (drawStart == -1) return;
                pairDraw(gc, (StyledRegion) pm.end.rdef, pm.end.start + drawStart,
                        pm.end.end + drawStart);
            }
        }catch(BadLocationException e){
            Assert.isTrue(false, "Never reach");
        }
    }

    void drawLine(int lno) {
        if (lno < 0 || lno >= fDocument.getNumberOfLines())
            return;
        int widgetLine = fProjectionViewer.modelLine2WidgetLine(lno);
        if (widgetLine == -1) return;
        int y = text.getLocationAtOffset(text.getOffsetAtLine(widgetLine)).y;
        int height = 0;
        if (text.getLineCount() > widgetLine+1)
            height = text.getLocationAtOffset(text.getOffsetAtLine(widgetLine+1)).y - y;
        else
            height = text.getLocationAtOffset(text.getCharCount()).y
                    + text.getLineHeight();
        int width = text.getClientArea().width + text.getHorizontalPixel();
        text.redraw(0, y, width, height, false);
    }

    
    
    
    
    
    public Object getAdapter(Class adapter)
    {
        if (adapter == IPresentationReconciler.class) {
            return fReconciler;
        }
        return null;
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