
#ifdef __cplusplus
extern "C" {
#endif


void colorer_load_syntax (WEdit * edit, char ***names, const char *type);

void colorer_free_syntax_rules (WEdit * edit);

typedef struct
{
    const char *name;
    const char *descr;
    const char *group;
    int items_number;
} colorer_types_list;

colorer_types_list *colorer_enumerate_types (void);

void colorer_free_types_list(colorer_types_list *types_list);

typedef struct {
    const char *item;
    int line, pos;
} colorer_outline_items;

colorer_outline_items *colorer_get_outline_items (WEdit *edit);

void colorer_free_outline_items(WEdit *edit, colorer_outline_items *outline_items);

/**
 * Requests for a list of available color styles
 */
void colorer_get_color_styles (const char ***names, const char ***descr);

/**
 * Frees previously returned arrays
 */
void colorer_free_color_styles (const char **names, const char **descr);

const char *colorer_get_current_type (WEdit *edit);

int colorer_set_current_type (WEdit *edit, const char *type);



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
