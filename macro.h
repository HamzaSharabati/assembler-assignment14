#ifndef _MACRO_H
#define _MACRO_H






typedef struct m_entry* m_table;

/** macros table */
typedef struct m_entry {
	/** Next entry in table */
	m_table next;
	
	char *key;
	/**text of the macro */
	char *text;
	
	
} m_table_entry;

/**
 * replaces the macros in the file 
 * @param filename 
 * @return if succeded 
 */
bool pre_process(char *filename) ;
/**
 * Adds macro to the table .
 * @param tab A pointer to the table
 * @param key The key of the macro
 * @param value The text of the macro
 */
void add_macro(m_table *tab, char *key, char *text)  ;
/**
 * finds macro in the table
 * @param tab A pointer to the table
 * @param key The key 
 * @return The text if found, NULL if not found
 */
char *find_macro (m_table tab, char *key);
void free_m_table(m_table tab) ;

#endif