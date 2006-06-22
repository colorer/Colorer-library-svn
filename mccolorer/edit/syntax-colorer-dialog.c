/*
   Colorer syntax engine - MC Edit dialogs support: syntax, style selection and outline

   Copyright (C) 2006 Igor Russkih <irusskih at gmail dot com>

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

#include <config.h>
#include "../src/global.h"
#include "../src/dialog.h"
#include "../src/wtools.h"
#include "../src/setup.h"
#include "edit.h"
#include "edit-widget.h"
#include "syntax-colorer.h"


void edit_colorer_options_dialog (void)
{
    int i;
    const char **names, **descr;
    Listbox *syntaxlist;
    
    if (!option_syntax_colorer) {
	return;
    }
    
    syntaxlist = create_listbox_window (40, 18, _(" Choose Color Style "), NULL);
    
    colorer_get_color_styles(&names, &descr);

    for (i = 0; names[i]; i++) {
	LISTBOX_APPEND_TEXT (syntaxlist, 0, descr[i], NULL);
	if (colorer_hrd_string != NULL && (strcmp (names[i], colorer_hrd_string) == 0))
	    listbox_select_by_number (syntaxlist->list, i);
    }

    i =  run_listbox (syntaxlist);
    if (i >= 0) {
	int slen = strlen(names[i]);
	if (slen >= COLORER_HRD_STRING_LENGTH) {
	    slen = COLORER_HRD_STRING_LENGTH-1;
	}
	strncpy(colorer_hrd_string, names[i], slen);
	colorer_hrd_string[slen] = 0;
	
	/* reload instance to apply changes */
	edit_free_syntax_rules(wedit);
	edit_load_syntax(wedit, NULL, option_syntax_type);
    }
    colorer_free_color_styles(names, descr);
}


void edit_colorer_outline_dialog (void)
{
    int i, cpos;
    colorer_outline_items *items;
    Listbox *syntaxlist;
    
    if (!option_syntax_colorer) {
	return;
    }
    
    items = colorer_get_outline_items(wedit);
    if (items == NULL) {
        return;
    }
        
    syntaxlist = create_listbox_window (40, 18, _(" Outline "), NULL);
    
    cpos = wedit->curs_line;
     
    for (i = 0; items[i].item; i++) {
	LISTBOX_APPEND_TEXT (syntaxlist, 0, items[i].item, NULL);
	if (cpos >= items[i].line && (items[i+1].item == NULL || cpos < items[i+1].line) )
	    listbox_select_by_number (syntaxlist->list, i);
    }

    i =  run_listbox (syntaxlist);
    if (i >= 0) {
        int target_pos = edit_find_line(wedit, items[i].line) + items[i].pos;
        edit_cursor_move(wedit,  target_pos - wedit->curs1);
        wedit->force |= REDRAW_PAGE;
    }
    colorer_free_outline_items(wedit, items);
}


static int
exec_edit_syntax_dialog (const char **names, int current, const char *titlepart)
{
    int i;
    char title[255];

    sprintf(title, "%s[ %s ]", _(" Choose syntax highlighting "), titlepart);

    Listbox *syntaxlist = create_listbox_window (24, 18, title, NULL);

    LISTBOX_APPEND_TEXT (syntaxlist, 'A', _("< Auto >"), NULL);
    LISTBOX_APPEND_TEXT (syntaxlist, 'R', _("< Reload Current Syntax >"), NULL);

    for (i = 0; names[i]; i++) {
	LISTBOX_APPEND_TEXT (syntaxlist, 0, names[i], NULL);
    }
    listbox_select_by_number (syntaxlist->list, current+2);

    return run_listbox (syntaxlist) - 2;
}


void edit_syntax_colorer_dialog (void)
{
    int idx, j, cgroupitem = 0, ctypeitem = 0;
    const char **groups = NULL;
    const char **types = NULL;
    const char *ctype, *cdescr, *cgroup;
    int groups_num = 0, types_num = 0;

    colorer_types_list *types_list = colorer_enumerate_types();

    if (types_list == NULL) {
        return;
    }

    ctype = colorer_get_current_type(wedit);

    groups = g_malloc (sizeof(char*) * types_list[0].items_number);
    
    for (idx = 0; idx < types_list[0].items_number; idx++) {
        int exists = FALSE;
        for (j = 0; j < groups_num; j++) {
            if (strcmp(groups[j], types_list[idx].group) == 0) {
                exists = TRUE;
            }
            if (strcmp(cgroup, groups[j]) == 0) {
                cgroupitem = j;
            }
        }
        if (strcmp(ctype, types_list[idx].name) == 0) {
            cdescr = types_list[idx].descr;
            cgroup = types_list[idx].group;
        }
        if (!exists) {
            groups[groups_num] = types_list[idx].group;
            groups_num++;
        }
    }
    groups[groups_num] = NULL;


    while (1) {
        
        cgroupitem = exec_edit_syntax_dialog(groups, cgroupitem, cdescr);
        
        if (cgroupitem < 0) {
            break;
        }

        cgroup = groups[cgroupitem];

        types_num = 0;
        ctypeitem = 0;

        g_free(types);
        types = g_malloc(sizeof(char*) * types_list[0].items_number * 2);

        for (idx = 0; idx < types_list[0].items_number; idx++) {
            if (strcmp(cgroup, types_list[idx].group) == 0) {
                types[types_num] = types_list[idx].descr;
                types[types_list[0].items_number * 2 - types_num] = types_list[idx].name;
                if (strcmp(ctype, types_list[idx].name) == 0) {
                    ctypeitem = types_num;
                }
                types_num++;
            }
        }
        types[types_num] = NULL;
        
        ctypeitem = exec_edit_syntax_dialog(types, ctypeitem, cdescr);

        if (ctypeitem == -3) {
            continue;
        } else if (ctypeitem < 0) {
            cgroupitem = ctypeitem;
            break;
        } else {
            colorer_set_current_type(wedit, types[types_list[0].items_number * 2 - ctypeitem]);
            break;
        }
    }
    g_free(types);
    g_free(groups);
    colorer_free_types_list(types_list);

    if (cgroupitem == -2) {
	edit_free_syntax_rules (wedit);
 	edit_load_syntax (wedit, NULL, option_syntax_type);
    }

    if (cgroupitem == -1) {
        colorer_reload(wedit);
 	edit_load_syntax (wedit, NULL, option_syntax_type);
    }

}
