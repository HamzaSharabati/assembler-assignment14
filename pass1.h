#ifndef PASS1_H
#define PASS1_H
/* Processes a code line in first pass */
#include "globals.h"

/**
 * Processes a single line in the first pass
 * @param line source line
 * @param IC pointer to instruction counter
 * @param DC pointer to data counter
 * @param code_img  code image array
 * @param data_img  data image array
 * @param sympol_table
 * @return if succeeded.
 */
bool first_pass(line_info line, long *IC, long *DC, machine_word **code_img, machine_word **data_img,
                        table *symbol_table);

#endif