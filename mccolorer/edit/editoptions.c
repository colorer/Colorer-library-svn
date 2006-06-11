/* editor options dialog box

   Copyright (C) 1996, 1997 the Free Software Foundation

   Authors: 1996, 1997 Paul Sheer

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

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

#include <stdlib.h>

#include "../src/global.h"

#include "edit.h"
#include "usermap.h"
#include "../src/dialog.h"	/* B_CANCEL */
#include "../src/wtools.h"	/* QuickDialog */
#include "../src/setup.h"

#include "edit-widget.h"
#include "syntax-colorer.h"

#ifdef USE_COLORER
#define OPT_DLG_H 18
#else
#define OPT_DLG_H 17
#endif

#define OPT_DLG_W 72

#ifndef USE_INTERNAL_EDIT
#define USE_INTERNAL_EDIT 1
#endif

static const char *key_emu_str[] =
{N_("Intuitive"), N_("Emacs"), N_("User-defined"), NULL};

static const char *wrap_str[] =
{N_("None"), N_("Dynamic paragraphing"), N_("Type writer wrap"), NULL};

static void
i18n_translate_array (const char *array[])
{
    while (*array!=NULL) {
	*array = _(*array);
        array++;
    }
}

void
edit_options_dialog (void)
{
    char wrap_length[32], tab_spacing[32], *p, *q;
    int wrap_mode = 0;
    int old_syntax_hl;
    int tmp_index;
#ifdef USE_COLORER
    int old_syntax_colorer;
    int tedit_syntax_colorer = option_syntax_colorer;
#endif
    int tedit_key_emulation = edit_key_emulation;
    int toption_fill_tabs_with_spaces = option_fill_tabs_with_spaces;
    int toption_save_position = option_save_position;
    int tedit_confirm_save = edit_confirm_save;
    int tedit_syntax_highlighting = option_syntax_highlighting;
    int toption_return_does_auto_indent = option_return_does_auto_indent;
    int toption_backspace_through_tabs = option_backspace_through_tabs;
    int toption_fake_half_tabs = option_fake_half_tabs;
    static int i18n_flag = 0;

    QuickWidget quick_widgets[] = {
	/* 0 */
	{quick_button, 6, 10, OPT_DLG_H - 3, OPT_DLG_H, N_("&Cancel"), 0,
	 B_CANCEL, 0, 0, NULL},
	/* 1 */
	{quick_button, 2, 10, OPT_DLG_H - 3, OPT_DLG_H, N_("&OK"), 0,
	 B_ENTER, 0, 0, NULL},
	/* 2 */
	{quick_label, OPT_DLG_W / 2, OPT_DLG_W, OPT_DLG_H - 5, OPT_DLG_H,
	 N_("Word wrap line length: "), 0, 0, 0, 0, NULL},
	/* 3 */
	{quick_input, OPT_DLG_W / 2 + 24, OPT_DLG_W, OPT_DLG_H - 5,
	 OPT_DLG_H, "", OPT_DLG_W / 2 - 4 - 24, 0, 0, 0, "edit-word-wrap"},
	/* 4 */
	{quick_label, OPT_DLG_W / 2, OPT_DLG_W, OPT_DLG_H - 6, OPT_DLG_H,
	 N_("Tab spacing: "), 0, 0, 0, 0, NULL},
	/* 5 */
	{quick_input, OPT_DLG_W / 2 + 24, OPT_DLG_W, OPT_DLG_H - 6,
	 OPT_DLG_H, "", OPT_DLG_W / 2 - 4 - 24, 0, 0, 0,
	 "edit-tab-spacing"},
	/* 6 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 8,
	 OPT_DLG_H, N_("Synta&x highlighting"), 8, 0, 0, 0, NULL},
#ifdef USE_COLORER
	/* 7 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 9,
	 OPT_DLG_H, N_("&Colorer syntax engine"), 0, 0, 0, 0, NULL},
	/* 8 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 10,
	 OPT_DLG_H, N_("Save file &position"), 0, 0, 0, 0, NULL},
	/* 9 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 11,
	 OPT_DLG_H, N_("Confir&m before saving"), 6, 0, 0, 0, NULL},
	/* 10 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 12,
	 OPT_DLG_H, N_("Fill tabs with &spaces"), 0, 0, 0, 0, NULL},
	/* 11 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 13,
	 OPT_DLG_H, N_("&Return does autoindent"), 0, 0, 0, 0, NULL},
	/* 12 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 14,
	 OPT_DLG_H, N_("&Backspace through tabs"), 0, 0, 0, 0, NULL},
	/* 13 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 15,
	 OPT_DLG_H, N_("&Fake half tabs"), 0, 0, 0, 0, NULL},
	/* 14 */
	{quick_radio, 5, OPT_DLG_W, OPT_DLG_H - 7, OPT_DLG_H, "", 3, 0, 0,
	 const_cast(char **, wrap_str), "wrapm"},
	/* 15 */
	{quick_label, 4, OPT_DLG_W, OPT_DLG_H - 8, OPT_DLG_H,
	 N_("Wrap mode"), 0, 0,
	 0, 0, NULL},
	/* 16 */
	{quick_radio, 5, OPT_DLG_W, OPT_DLG_H - 14, OPT_DLG_H, "", 2, 0, 0,
	 const_cast(char **, key_emu_str), "keyemu"},
	/* 17 */
	{quick_label, 4, OPT_DLG_W, OPT_DLG_H - 15, OPT_DLG_H,
	 N_("Key emulation"), 0, 0, 0, 0, NULL},
	/* 18 */
	NULL_QuickWidget
#else
	/* 7 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 9,
	 OPT_DLG_H, N_("Save file &position"), 0, 0, 0, 0, NULL},
	/* 8 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 10,
	 OPT_DLG_H, N_("Confir&m before saving"), 6, 0, 0, 0, NULL},
	/* 9 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 11,
	 OPT_DLG_H, N_("Fill tabs with &spaces"), 0, 0, 0, 0, NULL},
	/* 10 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 12,
	 OPT_DLG_H, N_("&Return does autoindent"), 0, 0, 0, 0, NULL},
	/* 11 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 13,
	 OPT_DLG_H, N_("&Backspace through tabs"), 0, 0, 0, 0, NULL},
	/* 12 */
	{quick_checkbox, OPT_DLG_W / 2 + 1, OPT_DLG_W, OPT_DLG_H - 14,
	 OPT_DLG_H, N_("&Fake half tabs"), 0, 0, 0, 0, NULL},
	/* 13 */
	{quick_radio, 5, OPT_DLG_W, OPT_DLG_H - 7, OPT_DLG_H, "", 3, 0, 0,
	 const_cast(char **, wrap_str), "wrapm"},
	/* 14 */
	{quick_label, 4, OPT_DLG_W, OPT_DLG_H - 8, OPT_DLG_H,
	 N_("Wrap mode"), 0, 0,
	 0, 0, NULL},
	/* 15 */
	{quick_radio, 5, OPT_DLG_W, OPT_DLG_H - 13, OPT_DLG_H, "", 3, 0, 0,
	 const_cast(char **, key_emu_str), "keyemu"},
	/* 16 */
	{quick_label, 4, OPT_DLG_W, OPT_DLG_H - 14, OPT_DLG_H,
	 N_("Key emulation"), 0, 0, 0, 0, NULL},
	NULL_QuickWidget
#endif
    };

    QuickDialog Quick_options =
	{ OPT_DLG_W, OPT_DLG_H, -1, 0, N_(" Editor options "), "", 0, 0 };

    if (!i18n_flag) {
	i18n_translate_array (key_emu_str);
	i18n_translate_array (wrap_str);
	i18n_flag = 1;
    }

    g_snprintf (wrap_length, sizeof (wrap_length), "%d",
		option_word_wrap_line_length);
    g_snprintf (tab_spacing, sizeof (tab_spacing), "%d",
		option_tab_spacing);

    quick_widgets[3].text = wrap_length;
    quick_widgets[3].str_result = &p;
    quick_widgets[5].text = tab_spacing;
    quick_widgets[5].str_result = &q;
    quick_widgets[6].result = &tedit_syntax_highlighting;

    tmp_index = 7;
#ifdef USE_COLORER
    quick_widgets[tmp_index++].result = &tedit_syntax_colorer;
#endif
    quick_widgets[tmp_index++].result = &toption_save_position;
    quick_widgets[tmp_index++].result = &tedit_confirm_save;
    quick_widgets[tmp_index++].result = &toption_fill_tabs_with_spaces;
    quick_widgets[tmp_index++].result = &toption_return_does_auto_indent;
    quick_widgets[tmp_index++].result = &toption_backspace_through_tabs;
    quick_widgets[tmp_index++].result = &toption_fake_half_tabs;

    if (option_auto_para_formatting)
	wrap_mode = 1;
    else if (option_typewriter_wrap)
	wrap_mode = 2;
    else
	wrap_mode = 0;
#ifdef USE_COLORER
    tmp_index = 14;
#else
    tmp_index = 13;
#endif

    quick_widgets[tmp_index].result = &wrap_mode;
    quick_widgets[tmp_index].value = wrap_mode;

    quick_widgets[tmp_index+2].result = &tedit_key_emulation;
    quick_widgets[tmp_index+2].value = tedit_key_emulation;

    Quick_options.widgets = quick_widgets;

    if (quick_dialog (&Quick_options) == B_CANCEL)
	return;

    old_syntax_hl = option_syntax_highlighting;
#ifdef USE_COLORER
    old_syntax_colorer = option_syntax_colorer;
#endif

    if (p) {
	option_word_wrap_line_length = atoi (p);
	g_free (p);
    }
    if (q) {
	option_tab_spacing = atoi (q);
	if (option_tab_spacing <= 0)
	    option_tab_spacing = 8;
	g_free (q);
    }

    option_syntax_highlighting = tedit_syntax_highlighting;
#ifdef USE_COLORER
    option_syntax_colorer = tedit_syntax_colorer;
#endif
    edit_confirm_save = tedit_confirm_save;
    option_save_position = toption_save_position;
    option_fill_tabs_with_spaces = toption_fill_tabs_with_spaces;
    option_return_does_auto_indent = toption_return_does_auto_indent;
    option_backspace_through_tabs = toption_backspace_through_tabs;
    option_fake_half_tabs = toption_fake_half_tabs;

    if (wrap_mode == 1) {
	option_auto_para_formatting = 1;
	option_typewriter_wrap = 0;
    } else if (wrap_mode == 2) {
	option_auto_para_formatting = 0;
	option_typewriter_wrap = 1;
    } else {
	option_auto_para_formatting = 0;
	option_typewriter_wrap = 0;
    }

    /* Reload menu if key emulation has changed */
    if (edit_key_emulation != tedit_key_emulation) {
	edit_key_emulation = tedit_key_emulation;
	edit_reload_menu ();
    }

    /* Load or unload syntax rules if the option has changed */
    if (option_syntax_highlighting != old_syntax_hl
#ifdef USE_COLORER
    || option_syntax_colorer != old_syntax_colorer
#endif
    )
    {
#ifdef USE_COLORER
        edit_reload_menu();
	edit_free_syntax_rules (wedit);
#endif
 	edit_load_syntax (wedit, NULL, option_syntax_type);
    /* Load usermap if it's needed */
    edit_load_user_map (wedit);
    }
}


#if USE_COLORER
void
edit_colorer_options_dialog (void)
{
    int i;
    char **names, **descr;
    Listbox *syntaxlist;
    
    if (!option_syntax_colorer) {
	return;
    }
    
    colorer_get_color_styles(&names, &descr);
    if (names == NULL || descr == NULL) {
        return;
    }
        
    syntaxlist = create_listbox_window (40, 18, _(" Choose Color Style "), NULL);
    
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


#endif


