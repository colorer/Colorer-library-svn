/*
   Colorer syntax engine - C++ bridge

   Copyright (C) 1996, 1997 the Free Software Foundation

   Authors: 2006 Igor Russkih <irusskih at gmail dot com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

//#define COLORER_FEATURE_LOGLEVEL  COLORER_FEATURE_LOGLEVEL_FULL
#define COLORER_FEATURE_LOGLEVEL  COLORER_FEATURE_LOGLEVEL_QUIET

#include"syntax-colorer-legacy-log.h"

#include"syntax-colorer-wrapper.h"
#include"syntax-colorer.h"

#include<colorer/editor/BaseEditor.h>
#include<colorer/editor/Outliner.h>
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
 * creation and returns always the same object (cline).
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
        try{
            if (pf == null) {
                pf = new ParserFactory();
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
            
            /* Loading color style */
            try {
                if (*colorer_hrd_string == 0) {
                    strcpy(colorer_hrd_string, "default");
                }
                baseEditor->setRegionMapper(&DString("console"), &DString(colorer_hrd_string));
            } catch (Exception &e) {
                baseEditor->setRegionMapper(&DString("console"), &DString("default"));
            }

            baseEditor->setRegionCompact(true);
            
            const Region *def_Outlined = pf->getHRCParser()->getRegion(&DString("def:Outlined"));
            const Region *def_Error = pf->getHRCParser()->getRegion(&DString("def:Error"));
            structOutliner = new Outliner(baseEditor, def_Outlined);
            //errorOutliner = new Outliner(baseEditor, def_Error);

            CLR_TRACE("MC", "filename: %s", colorer_get_filename(edit));
            CLR_TRACE("MC", "filetype: %s", baseEditor->getFileType()->getName()->getChars());

            def_fore = StyledRegion::cast(baseEditor->rd_def_Text)->fore;
            def_back = StyledRegion::cast(baseEditor->rd_def_Text)->back;
            def_mc   = colorer_convert_color(StyledRegion::cast(baseEditor->rd_def_Text));
//            def_mc_bg= try_alloc_color_pair(null, win_colors[def_back]);

    	    CLR_TRACE("MC", "def_fore=%d, def_back=%d, def_mc=%d", def_fore, def_back, def_mc);
    	    CLR_TRACE("MC", "def_mc_bg=%d", def_mc_bg);

	    pairMatch = null;

        } catch(Exception &e) {
            // TODO: notify about disabled colorer
    	    CLR_TRACE("MC", "Exception in MCEditColorer: %s", e.getMessage()->getChars());
            option_syntax_colorer = 0;
            return;
        }

    }

    ~MCEditColorer()
    {
        colorer_store_handle(edit, null);
        delete pairMatch;
        delete structOutliner;
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

	int line = colorer_get_line(edit, byte_index);

        LineRegion *lr = baseEditor->getLineRegions(line);

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
    
    int find_visible_bracket()
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
    
    int get_bracket()
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
    
    int select_bracket()
    {
    	if (get_bracket() == -1 || pairMatch == null) {
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
    
    int select_bracket_content()
    {
    	if (get_bracket() == -1 || pairMatch == null) {
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

    int get_default_color()
    {
        return def_mc;
    }

    static colorer_types_list *enumerate_types()
    {
        colorer_types_list *types_list = null;
        Vector<FileType *> types_vector;

        if (pf == null) {
            return null;
        }

        int idx = 0;
        while(true) {
            FileType *type = pf->getHRCParser()->enumerateFileTypes(idx++);
            if (type == null) break;
            types_vector.addElement(type);
        }
        types_list = new colorer_types_list[types_vector.size()];
        for (idx = 0; idx < types_vector.size(); idx++) {
            types_list[idx].name = types_vector.elementAt(idx)->getName()->getChars();
            types_list[idx].descr = types_vector.elementAt(idx)->getDescription()->getChars();
            types_list[idx].group = types_vector.elementAt(idx)->getGroup()->getChars();
        }
        types_list[0].items_number = idx;
        return types_list;
    }

    static void get_color_styles(const char ***names, const char ***descr)
    {
        ParserFactory *pf = MCEditColorer::pf;
    
	if (pf == null) {
	    *names = null;
	    *descr = null;
	    return;
	}
    
	Vector<const char *> hrd_styles;
	int i = 0;
	while (true){
	    const String * hrd = pf->enumerateHRDInstances(DString("console"), i++);
	    if (hrd == null) break;
	    hrd_styles.addElement(hrd->getChars());
	    hrd_styles.addElement(pf->getHRDescription(DString("console"), *hrd)->getChars());
	}
	const char **hrd_names = new const char*[hrd_styles.size()/2 + 1];
	const char **hrd_descr = new const char*[hrd_styles.size()/2 + 1];
	for (i = 0; i < hrd_styles.size()/2; i++) {
	    hrd_names[i] = hrd_styles.elementAt(i*2);
	    hrd_descr[i] = hrd_styles.elementAt(i*2+1);
	}
	hrd_names[i] = null;
	hrd_descr[i] = null;
	*names = hrd_names;
	*descr = hrd_descr;
    }
    
    static void free_color_styles(const char **names, const char **descr)
    {
        delete[] names;
        delete[] descr;
    }
    
    colorer_outline_items *get_outline_items()
    {
        colorer_outline_items *outline_items;
        
        baseEditor->validate(-1, false);
        
        outline_items = new colorer_outline_items[structOutliner->itemCount()+1];
        
        for (int idx = 0; idx < structOutliner->itemCount(); idx++) {
            outline_items[idx].item = structOutliner->getItem(idx)->token->getChars();
            outline_items[idx].line = structOutliner->getItem(idx)->lno;
            outline_items[idx].pos  = structOutliner->getItem(idx)->pos;
        }
        outline_items[structOutliner->itemCount()].item = null;

	return outline_items;
    }

    const char *get_current_type()
    {
        if (baseEditor->getFileType() == null) {
            return null;
        }
        return baseEditor->getFileType()->getName()->getChars();
    }

    int set_current_type (const char *type)
    {
        if (type == null) {
            baseEditor->chooseFileType(null);
            return true;
        }
        return baseEditor->setFileType(DString(type)) != null ? true : false;
    }

    static void reload()
    {
        delete pf;
        pf = null;
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
            CLR_TRACE("MC", "map[%s:%s - %d]=%d", win_colors[fg], win_colors[bg], bg + (fg<<4), colorer_color_map[bg + (fg<<4)]);
        }
        return ret;
    }

private:    
    WEdit        *edit;
    BaseEditor   *baseEditor;
    Outliner     *structOutliner;
    MCLineSource *lineSource;
    int          def_fore, def_back, def_mc, def_mc_bg;
    PairMatch    *pairMatch;
    static ParserFactory *pf;
};


ParserFactory *MCEditColorer::pf = null;




extern "C" {


void colorer_load_syntax (WEdit * edit, char ***names, const char *type)
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
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer == null) {
        return;
    }
    *color = mccolorer->get_color(byte_index);
}

void colorer_edit_modified(WEdit * edit)
{
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer == null) {
        return;
    }
    mccolorer->modifyEvent(colorer_edit_cursline(edit));
}

int colorer_find_visible_bracket(WEdit *edit)
{
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer == null) {
        return -1;
    }
    return mccolorer->find_visible_bracket();
}

int colorer_get_bracket(WEdit *edit)
{
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer == null) {
        return -1;
    }
    return mccolorer->get_bracket();    
}

int colorer_select_bracket(WEdit *edit)
{
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer == null) {
        return 0;
    }
    return mccolorer->select_bracket();
}

int colorer_select_bracket_content(WEdit *edit)
{
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer == null) {
        return 0;
    }
    return mccolorer->select_bracket_content();
}

int colorer_get_default_color(WEdit * edit)
{
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer == null) {
        return -1;
    }
    return mccolorer->get_default_color();
}

colorer_types_list *colorer_enumerate_types()
{
    return MCEditColorer::enumerate_types();
}

void colorer_free_types_list(colorer_types_list *types_list)
{
    delete[] types_list;
    return;
}

void colorer_get_color_styles(const char ***names, const char ***descr)
{
    MCEditColorer::get_color_styles(names, descr);
}

void colorer_free_color_styles(const char **names, const char **descr)
{
    MCEditColorer::free_color_styles(names, descr);
}

const char *colorer_get_current_type (WEdit *edit)
{
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer == null) {
        return null;
    }
    return mccolorer->get_current_type();
}

int colorer_set_current_type (WEdit *edit, const char *type)
{
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer == null) {
        return -1;
    }
    return mccolorer->set_current_type(type);
}


colorer_outline_items *colorer_get_outline_items(WEdit * edit)
{
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer == null) {
        return null;
    }
    return mccolorer->get_outline_items();
}

void colorer_free_outline_items(WEdit * edit, colorer_outline_items *outline_items)
{
    delete[] outline_items;
}

void colorer_reload(WEdit *edit)
{
    MCEditColorer *mccolorer = (MCEditColorer*)colorer_get_handle(edit);
    if (mccolorer != null) {
        delete mccolorer;
    }
    MCEditColorer::reload();
}


}
