
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



