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
	strcpy(colorer_hrd_string, names[i]);
	
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
        edit_cursor_move(wedit, wedit->curs1 - target_pos);
        wedit->force |= REDRAW_PAGE;
    }
    colorer_free_outline_items(wedit, items);
}


static int
exec_edit_syntax_dialog (const char **names) {
/*    int i;

    Listbox *syntaxlist = create_listbox_window (MAX_ENTRY_LEN, LIST_LINES,
	_(" Choose syntax highlighting "), NULL);
    LISTBOX_APPEND_TEXT (syntaxlist, 'A', _("< Auto >"), NULL);
    LISTBOX_APPEND_TEXT (syntaxlist, 'R', _("< Reload Current Syntax >"), NULL);

    for (i = 0; names[i]; i++) {
	LISTBOX_APPEND_TEXT (syntaxlist, 0, names[i], NULL);
	if (! option_auto_syntax && option_syntax_type &&
	    (strcmp (names[i], option_syntax_type) == 0))
    	    listbox_select_by_number (syntaxlist->list, i + N_DFLT_ENTRIES);
    }

    return run_listbox (syntaxlist);
*/
}


void edit_syntax_colorer_dialog (void)
{
/*    int idx;
    char **groups;
    int groups_num = 0;

    colorer_types_list *types_list = colorer_enumerate_types();

    if (types_list == NULL) {
        return;
    }

    groups = g_malloc (sizeof(char*) * types_list[0].items_number);
    
    for (idx = 0; idx < types_list[0].items_number; idx++) {
        for (int j = 0; j < groups_num; j++) {
            if (strcmp(groups[j], types_list[idx].group) != 0)
                continue;
        }
        groups[groups_num++] = types_list[idx].group;
    }


    names = (char**) g_malloc (sizeof (char*));
    names[0] = NULL;
    while (names[count++] != NULL);
    qsort(names, count - 1, sizeof(char*), pstrcmp);

    if ((syntax = exec_edit_syntax_dialog ((const char**) names)) < 0) {
	for (i = 0; names[i]; i++) {
	    g_free (names[i]);
	}
	g_free (names);
	return;
    }

    old_auto_syntax = option_auto_syntax;
    old_syntax_type = g_strdup (option_syntax_type);

*/
}
