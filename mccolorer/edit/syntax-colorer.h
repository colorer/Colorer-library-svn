/*
   Colorer syntax engine - C++ bridge header

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
#ifndef MC_EDIT_COLORER_H
#define MC_EDIT_COLORER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Colorer's side allocation/deallocation
 */
void colorer_load_syntax (WEdit * edit, char ***names, const char *type);

void colorer_free_syntax_rules (WEdit * edit);


/**
 * HRC Types enumeration API
 */
typedef struct
{
    const char *name;
    const char *descr;
    const char *group;
    int items_number;
} colorer_types_list;

colorer_types_list *colorer_enumerate_types (void);

void colorer_free_types_list(colorer_types_list *types_list);

/**
 * Outliner API
 */
typedef struct
{
    const char *item;
    int line, pos;
} colorer_outline_items;

colorer_outline_items *colorer_get_outline_items (WEdit *edit);

void colorer_free_outline_items(WEdit *edit, colorer_outline_items *outline_items);

/**
 * General Reload, reloads HRC, HRD bases
 */
void colorer_reload(WEdit *edit);

/**
 * Requests for a list of available color styles
 */
void colorer_get_color_styles (const char ***names, const char ***descr);

/**
 * Frees previously returned arrays
 */
void colorer_free_color_styles (const char **names, const char **descr);

/**
 * Retrieves current type name in the specified editor
 */
const char *colorer_get_current_type (WEdit *edit);

/**
 * Changes current type name in the specified editor
 */
int colorer_set_current_type (WEdit *edit, const char *type);


/**
 *
 * Base entrance point to request color from highlighting system
 *
 */
void colorer_get_syntax_color (WEdit * edit, long byte_index, int *color);

/**
 * Searches screen visible bracket under cursor and its pair.
 */
int colorer_find_visible_bracket(WEdit *edit);

/**
 * Searches globally over the text paired bracket.
 */
int colorer_get_bracket(WEdit *edit);

/**
 * Searches globally over the text paired bracket
 * and makes a mark block over it.
 */
int colorer_select_bracket(WEdit *edit);

/**
 * Searches globally over the text paired bracket
 * and makes a mark block over its content excluding brackets.
 */
int colorer_select_bracket_content(WEdit *edit);

/**
 * Called when modification happens in editor
 */
void colorer_edit_modified(WEdit * edit);

/**
 * Requests default background color for current editor
 */
int colorer_get_default_color(WEdit * edit);

#ifdef __cplusplus
}
#endif

#endif
