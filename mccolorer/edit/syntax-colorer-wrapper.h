/*
   Colorer syntax engine - C wrappers for common MC Edit request and operations

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
#ifndef MC_EDIT_COLORER_WRAPPER_H
#define MC_EDIT_COLORER_WRAPPER_H

#ifdef __cplusplus	
extern "C"{
#endif

/* It is not safe generally, but for this file it is OK. */
#include"../src/color.h"

/* Stub definitions for C++ code */
    typedef void WEdit;
    extern char colorer_hrd_string[];
    extern int option_syntax_colorer;

/* Interface functions */

    long edit_bol (WEdit * edit, long current);
    long edit_eol (WEdit * edit, long current);
    long edit_find_line (WEdit *edit, int line);

/* C-C++ layer code */

    void colorer_store_handle(WEdit *edit, void *mcedit);
    void *colorer_get_handle(WEdit *edit);
    
    char *colorer_get_filename(WEdit *edit);
    int colorer_total_lines(WEdit *edit);
    int colorer_top_line(WEdit *edit);
    int colorer_height(WEdit *edit);
    int colorer_get_line (WEdit *edit, int pos);
    int colorer_get_line_pos (WEdit *edit, int line);
    int colorer_edit_get_byte (WEdit * edit, long byte_index);
    int colorer_edit_cursline (WEdit * edit);
    int colorer_edit_get_cursor(WEdit * edit);
    void colorer_mark_text(WEdit * edit, int start, int end);

#ifdef __cplusplus	
}
#endif

#endif
