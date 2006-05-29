
#ifdef __cplusplus
extern "C" {
#endif


void colorer_load_syntax (WEdit * edit, char **names, const char *type);

void colorer_free_syntax_rules (WEdit * edit);

void colorer_get_syntax_color (WEdit * edit, long byte_index, int *color);

/**
 * Searches screen visible bracket under cursor and its pair.
 */
int colorer_find_visible_bracket(WEdit *edit);

/**
 * Searches globally over the text paired bracket.
 */
int colorer_get_bracket(WEdit *edit);

int colorer_select_bracket(WEdit *edit);

int colorer_select_bracket_content(WEdit *edit);

/**
 * Called when modification happens in editor
 */
void colorer_edit_modified(WEdit * edit);


#ifdef __cplusplus
}
#endif
