
#ifdef __cplusplus
extern "C" {
#endif


void colorer_load_syntax (WEdit * edit, char ***names, const char *type);

void colorer_free_syntax_rules (WEdit * edit);

/* Requests for a list of available color styles */
void colorer_get_color_styles (char ***names, char ***descr);

/* Frees previously returned arrays */
void colorer_free_color_styles (char **names, char **descr);

/**
 * Base entrance point to request color from highlighting system
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
