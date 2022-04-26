/* Output files related functions */
#ifndef _output_files_H
#define _output_files_H
#include "globals.h"
#include "table.h"

/**
 * Writes the output files 
 * @param code_img 
 * @param data_img
 * @param icf  instruction counter
 * @param dcf  data counter
 * @param filename The filename without the extension
 * @param sympol_table
 * @return Whether succeeded
 */
int write_output_files(machine_word **code_img, machine_word **data_img, long icf, long dcf, char *filename,
                       table symbol_table);

#endif