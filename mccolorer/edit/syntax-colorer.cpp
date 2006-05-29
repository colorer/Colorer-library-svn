
//#define COLORER_FEATURE_LOGLEVEL  COLORER_FEATURE_LOGLEVEL_FULL
#define COLORER_FEATURE_LOGLEVEL  COLORER_FEATURE_LOGLEVEL_QUIET

#include"syntax-colorer-legacy-log.h"

#include"syntax-colorer-wrapper.h"

#include<colorer/editor/BaseEditor.h>
#include<common/Logging.h>
#include<assert.h>

char * win_colors[] = {

"black",
"blue",
"green",
"cyan",
"red",
"magenta",
"brown",
"lightgray",

"gray",
"brightblue",
"brightgreen",
"brightcyan",
"brightred",
"brightmagenta",
"yellow",
"white",

};

int colorer_color_map[16*16];



/**
 * MCString is a String abstraction over simple WEdit ASCII buffer.
 * For now it doesn't handle any unicode related data.
 */
class MCString : public String
{
public:
    MCString(WEdit *edit)
    {
        this->edit = edit;
    }
    ~MCString()
    {
    }
    void setPosition(int pos)
    {
        start = pos;
        //CLR_TRACE("MC", "setPosition start=%d", pos);
        len = edit_eol(edit, pos) - start;
        //CLR_TRACE("MC", "setPosition len=%d", len);
        assert(len >= 0);
    }
  
    int length() const{
        return len;
    }
    
    wchar operator[](int idx) const
    {
        if (idx < 0 || idx >= len) throw OutOfBoundException(StringBuffer("MCString: ")+SString(idx));
        return colorer_edit_get_byte(edit, start+idx);
    }
private:
    WEdit *edit;
    int start, len;
};

/**
 * Line information source from original WEdit buffer.
 * To speedup operation this object eliminates real String object
 * creation and returns alwayse the same object (cline).
 * This is ok since parser requires this object
 * to be valid only until next call to getLine() method.
 */
class MCLineSource : public LineSource
{
    WEdit *edit;
    MCString *cline;
public:
    MCLineSource(WEdit *edit)
    {
        cline = new MCString(edit);
        this->edit = edit;
    }
    ~MCLineSource()
    {
    	delete cline;
    	cline = null;
    }
    void startJob(int lno)
    {
        CLR_TRACE("MC", "startJob %d",lno);
    }
    void endJob(int lno)
    {
        CLR_TRACE("MC", "endJob %d", lno);
    }
    
    String *getLine(int lno)
    {
        int pos = colorer_get_line_pos(edit, lno);
        //CLR_TRACE("MC", "getLine:%03d - pos=%d", lno, pos);
	if (pos == -1){
            CLR_ERROR("MC", "getLine: %d returns null", lno);
	    return null;
	}
	cline->setPosition(pos);
        return cline;
    }
};


class MCEditColorer
{
public:
    MCEditColorer(WEdit *edit)
    {
        if (pf == null){
            try {
                pf = new ParserFactory();
            } catch(Exception &e) {
                //notify about disabled colorer
                option_syntax_colorer = 0;
                return;
            }

            // cleanup color index map
            for (int idx = 0; idx < 16*16; idx++){
                colorer_color_map[idx] = -1;
            }
        }
        this->edit = edit;
        colorer_store_handle(edit, this);

        lineSource = new MCLineSource(edit);
        
        baseEditor = new BaseEditor(pf, lineSource);
        baseEditor->chooseFileType(&DString(colorer_get_filename(edit)));
        
        try {
            if (*colorer_hrd_string == 0) {
                strcpy(colorer_hrd_string, "default");
            }
            baseEditor->setRegionMapper(&DString("console"), &DString(colorer_hrd_string));
        } catch (Exception &e) {
            baseEditor->setRegionMapper(&DString("console"), &DString("default"));
        }

        baseEditor->setRegionCompact(true);

        CLR_TRACE("MC", "filename: %s", colorer_get_filename(edit));
        CLR_TRACE("MC", "filetype: %s", baseEditor->getFileType()->getName()->getChars());
        def_fore = StyledRegion::cast(baseEditor->rd_def_Text)->fore;
        def_back = StyledRegion::cast(baseEditor->rd_def_Text)->back;
        def_mc   = colorer_convert_color(StyledRegion::cast(baseEditor->rd_def_Text));
	CLR_TRACE("MC", "def_fore=%d, def_back=%d, def_mc=%d", def_fore, def_back, def_mc);
	pairMatch = null;
    }

    ~MCEditColorer()
    {
        colorer_store_handle(edit, null);
        delete pairMatch;
        delete baseEditor;
        delete lineSource;
    }
    
    void modifyEvent(int line)
    {
        baseEditor->lineCountEvent(colorer_total_lines(edit));
        baseEditor->visibleTextEvent(colorer_top_line(edit), colorer_height(edit));
        baseEditor->modifyEvent(line);
    }

    int get_color(int byte_index)
    {
        int cur_pos = colorer_edit_get_cursor(edit);
	
        if (byte_index == -1) {
            return def_mc;
        }

        baseEditor->lineCountEvent(colorer_total_lines(edit));
        baseEditor->visibleTextEvent(colorer_top_line(edit), colorer_height(edit));

	/*
	static int tl = 0;
	static int topl = 0;
	static int h = 0;
	if (tl != colorer_total_lines(edit) ||
	    topl != colorer_top_line(edit) ||
	    h != colorer_height(edit)+1)
	{
	    tl = colorer_total_lines(edit);
	    topl = colorer_top_line(edit);
	    h = colorer_height(edit)+1;
            //CLR_TRACE("MC", "colorer_total_lines: %d", tl);
	    //CLR_TRACE("MC", "colorer_top: %d, height=%d", topl, h);
	}
	*/

	int line = colorer_get_line(edit, byte_index);
	//CLR_TRACE("MC", "byte=%d, line=%d", byte_index, line);

        LineRegion *lr = baseEditor->getLineRegions(line);
        //CLR_TRACE("MC", "lr=%p", lr);

        int line_offset = byte_index - edit_bol(edit, byte_index);
        
        if (pairMatch != null) {
            if (line == pairMatch->sline &&
                line_offset >= pairMatch->start->start &&
                line_offset < pairMatch->start->end)
            {
                return colorer_convert_color(pairMatch->start->styled());
            }
            if (line == pairMatch->eline &&
                line_offset >= pairMatch->end->start &&
                line_offset < pairMatch->end->end)
            {
                return colorer_convert_color(pairMatch->end->styled());
            }
        }

        for (LineRegion *next = lr; next != null; next = next->next){
            //CLR_TRACE("MC", "next:%d-%d rd=%s", next->start, next->end, next->region->getName()->getChars());
            if (next->start <= line_offset && (next->end > line_offset || next->end == -1)
                && next->rdef != null && !next->special) {
		return colorer_convert_color(next->styled());
            }
        }
        return def_mc;
    }
    
    int find_visible_bracket(WEdit *edit) 
    {
    	bool pairDrop = false;
    	int cpos = colorer_edit_get_cursor(edit);
    	cpos = cpos - edit_bol(edit, cpos);
    	
        if (pairMatch != null) {
            baseEditor->releasePairMatch(pairMatch);
            pairDrop = true;
        }

        // update state
        baseEditor->lineCountEvent(colorer_total_lines(edit));
        baseEditor->visibleTextEvent(colorer_top_line(edit), colorer_height(edit));

        pairMatch = baseEditor->searchLocalPair(colorer_edit_cursline(edit), cpos);
        if (pairMatch != null || pairDrop) {
            //CLR_TRACE("MC", "find_visible_bracket");
            return 1;
        }
        return 0;
    }
    
    int get_bracket(WEdit *edit)
    {
    	int cpos = colorer_edit_get_cursor(edit);
    	cpos = cpos - edit_bol(edit, cpos);

        if (pairMatch != null) {
            baseEditor->releasePairMatch(pairMatch);
        }
        pairMatch = baseEditor->searchGlobalPair(colorer_edit_cursline(edit), cpos);
        if (pairMatch != null && pairMatch->eline != -1) {
            //CLR_TRACE("MC", "find bracket:%d", pairMatch->eline);
            return colorer_get_line_pos(edit, pairMatch->eline) +
                   (pairMatch->topPosition ? pairMatch->end->end : pairMatch->end->start);
        }
        return -1;
    }
    
    int select_bracket(WEdit *edit)
    {
    	if (get_bracket(edit) == -1 || pairMatch == null) {
    	    return 0;
    	}
    	if (pairMatch->topPosition) {
            colorer_mark_text(edit,
                colorer_get_line_pos(edit, pairMatch->sline) + pairMatch->start->start,
                colorer_get_line_pos(edit, pairMatch->eline) + pairMatch->end->end
            );
    	} else {
            colorer_mark_text(edit,
                colorer_get_line_pos(edit, pairMatch->eline) + pairMatch->end->start,
                colorer_get_line_pos(edit, pairMatch->sline) + pairMatch->start->end
            );
    	}
    	return 1;
    }
    
    int select_bracket_content(WEdit *edit)
    {
    	if (get_bracket(edit) == -1 || pairMatch == null) {
    	    return 0;
    	}
    	if (pairMatch->topPosition) {
            colorer_mark_text(edit,
                colorer_get_line_pos(edit, pairMatch->sline) + pairMatch->start->end,
                colorer_get_line_pos(edit, pairMatch->eline) + pairMatch->end->start
            );
    	} else {
            colorer_mark_text(edit,
                colorer_get_line_pos(edit, pairMatch->eline) + pairMatch->end->end,
                colorer_get_line_pos(edit, pairMatch->sline) + pairMatch->start->start
            );
    	}
    	return 1;
    }
    
private:
    int colorer_convert_color(const StyledRegion *region)
    {
        int fg = region->bfore ? region->fore : def_fore;
        int bg = region->bback ? region->back : def_back;
        int ret = colorer_color_map[bg + (fg<<4)];
        if (ret == -1){
            ret =  try_alloc_color_pair(win_colors[fg], win_colors[bg]);
            colorer_color_map[bg+(fg<<4)] = ret;
            //CLR_TRACE("MC", "map[%s:%s - %d]=%d", win_colors[fg], win_colors[bg], bg + (fg<<4), colorer_color_map[bg + (fg<<4)]);
        }
        //CLR_TRACE("MC", "clr_map [%d-%d]=%d", fg, bg, ret);
        return ret;
    }

private:    
    static ParserFactory *pf;

    WEdit        *edit;
    BaseEditor   *baseEditor;
    MCLineSource *lineSource;
    int          def_fore, def_back, def_mc;
    PairMatch    *pairMatch;
};


ParserFactory *MCEditColorer::pf = null;




extern "C" {


void colorer_load_syntax (WEdit * edit, char **names, const char *type)
{
    //CLR_TRACE("MC", "colorer-load:%s", type);

    if (edit == null){
        return;
    }

    MCEditColorer *mcedit = new MCEditColorer(edit);
}

void colorer_free_syntax_rules (WEdit * edit)
{
    //CLR_TRACE("MC", "colorer-free");
    delete (MCEditColorer*)colorer_get_handle(edit);
}

void colorer_get_syntax_color (WEdit * edit, long byte_index, int *color)
{
    *color = ((MCEditColorer*)colorer_get_handle(edit))->get_color(byte_index);
}

void colorer_edit_modified(WEdit * edit)
{
    //CLR_TRACE("MC", "colorer_edit_modified:%d", colorer_edit_cursline(edit));
    ((MCEditColorer*)colorer_get_handle(edit))->modifyEvent(colorer_edit_cursline(edit));
}

int colorer_find_visible_bracket(WEdit *edit)
{
    return ((MCEditColorer*)colorer_get_handle(edit))->find_visible_bracket(edit);    
}

int colorer_get_bracket(WEdit *edit)
{
    return ((MCEditColorer*)colorer_get_handle(edit))->get_bracket(edit);    
}

int colorer_select_bracket(WEdit *edit)
{
    return ((MCEditColorer*)colorer_get_handle(edit))->select_bracket(edit);    
}

int colorer_select_bracket_content(WEdit *edit)
{
    return ((MCEditColorer*)colorer_get_handle(edit))->select_bracket_content(edit);    
}


}

