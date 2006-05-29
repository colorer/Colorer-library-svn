
#include <config.h>
#include "edit.h"
#include "edit-widget.h"
#include "../src/color.h" /* use_colors */
#include "../src/main.h"  /* mc_home */
#include "../src/wtools.h"  /* message() */
#include "../src/setup.h"

#define COLORER_FEATURE_LOGLEVEL COLORER_FEATURE_LOGLEVEL_FULL
//#define COLORER_FEATURE_LOGLEVEL COLORER_FEATURE_LOGLEVEL_QUIET

#include "syntax-colorer-legacy-log.h"
#include "common/Logging.h"

int option_syntax_colorer = 1;

char colorer_hrd_string[COLORER_HRD_STRING_LENGTH] = { 0 };


void colorer_store_handle(WEdit *edit, void *mcedit)
{
    edit->colorer_handle = mcedit;
    return;
}

void *colorer_get_handle(WEdit *edit)
{
    return edit->colorer_handle;
}


int colorer_edit_get_byte (WEdit * edit, long byte_index)
{
//    CLR_TRACE("MC", "get_byte=%d lastbyte=%d", byte_index, edit->last_byte);
    return edit_get_byte(edit, byte_index);
}

char *colorer_get_filename(WEdit *edit)
{
    return edit->filename;
}

int colorer_total_lines(WEdit *edit)
{
    return edit->total_lines+1;
}

int colorer_top_line(WEdit *edit)
{
    return edit->start_line;
}

int colorer_height(WEdit *edit)
{
    int height = edit->widget.lines;
    if (height == 0) return 1;
    return height;
}

int colorer_get_line_pos (WEdit *edit, int line)
{
//    CLR_TRACE("MC", "get_line_pos total_lines=%d", edit->total_lines);
    if (line < 0 || line > edit->total_lines) return -1;
    return edit_find_line(edit, line);
}

int colorer_edit_cursline(WEdit * edit)
{
    return edit->curs_line;
}


int colorer_get_line (WEdit *edit, int pos)
{
    int i, j = 0;
    int m = 2000000000;
    
    if (!edit->caches_valid)
    {
        for (i = 0; i < N_LINE_CACHES; i++) {
            edit->line_numbers[i] = edit->line_offsets[i] = 0;
        }
/* three offsets that we *know* are line 0 at 0 and these two: */
        edit->line_numbers[1] = edit->curs_line;
        edit->line_offsets[1] = edit_bol (edit, edit->curs1);
        edit->line_numbers[2] = edit->total_lines;
        edit->line_offsets[2] = edit_bol (edit, edit->last_byte);
        edit->caches_valid = 1;
    }

    pos = edit_bol(edit, pos);

    if (pos >= edit->last_byte)
        return edit->line_numbers[2];
    if (pos <= 0)
        return 0;

/* find the closest known point */
    for (i = 0; i < N_LINE_CACHES; i++) {
        int n;
        n = abs (edit->line_offsets[i] - pos);
        if (n < m) {
            m = n;
            j = i;
        }
    }
    if (m == 0)
        return edit->line_numbers[j];   /* know the offset exactly */
    
    if (m == 1 && j >= 3)
        i = j;          /* one line different - caller might be looping, so stay in this cache */
    else
        i = 3 + (rand () % (N_LINE_CACHES - 3));
    
    if (pos > edit->line_offsets[j])
    {
        int offs = edit->line_offsets[j];
        edit->line_numbers[i] = edit->line_numbers[j];
        while (pos > offs){
            edit->line_numbers[i]++;
            offs = edit_eol(edit, offs)+1;
        }
        edit->line_offsets[i] = offs;
    }
    else
    {
        int offs = edit->line_offsets[j];
        edit->line_numbers[i] = edit->line_numbers[j];
        while (pos < offs){
            edit->line_numbers[i]--;
            offs = edit_bol(edit, offs-1);
        }
        edit->line_offsets[i] = offs;
    }
    return edit->line_numbers[i];
}

int colorer_edit_get_cursor(WEdit * edit)
{
    return edit->curs1;
}

void colorer_mark_text(WEdit * edit, int start, int end)
{
    edit->mark1 = min(start, end);
    edit->mark2 = max(start, end);
    edit->column1 = edit->column2 = 0;
    edit->force |= REDRAW_PAGE;
}
