/* Helper functions to process and analyze code */
#ifndef _UTILITY2_H
#define _UTILITY2_H
#include "table.h"
#include "globals.h"

/**
 * Detects the opcode and the funct of a command by it's name
 * @param cmd The command name (string)
 * @param opcode_out The opcode value destination
 * @param funct_out The funct value destination
 */
void get_opcode_func(char* cmd, opcode *opcode_out, funct *funct_out);

/**
 * Returns the addressing type of an operand
 * @param operand The operand's string
 * @return The addressing type of the operand
 */
addressing_type get_addressing_type(char *operand);

/**
 * Validates and Builds 1st code word by the opcode .
 * @param line
 * @param curr_opcode The current opcode
 * @return A pointer to code word struct, which represents the code. if validation fails, returns NULL.
 */
data_op_word *get_code_word(line_info line, opcode curr_opcode) ;
/**
 * Validates and Builds  2nd code word by the opcode , func ,operands.
 * @param line
 * @param curr_opcode The current opcode
 * @param curr_funct The func
 * @param op_count n. of operands
 * @param operands list of operands 
 * @return A pointer to code word struct, which represents the code. if validation fails, returns NULL.
 */
code_prop *get_code_prop (line_info line, opcode curr_opcode, funct curr_funct, int op_count, char *operands[2]) ;

/**
 * Returns the register enum value
 * @param name The name of the register
 * @return The enum value of the register if found. otherwise, returns NONE_REG
 */
reg get_register_by_name(char *name);



/**
 * Returns the register enum value from label index operand
 * @param name 
 * @return The enum value of the register if found. otherwise, returns NONE_REG
 */
reg get_register_from_index(char *name) ;

/**
 * Builds a data word by the operand's addressing type, value and whether the symbol (if it is one) is external.
 * @param addressing The addressing type of the value
 * @param data The value
 * @param is_extern_symbol If the symbol is a label, and it's external
 * @return A pointer to the constructed data word for the data by the specified properties.
 */
data_op_word *build_data_op_word(addressing_type addressing, int data, bool is_extern_symbol);

/**
 * Separates the operands from a certain index, puts each operand into the destination array,
 * and puts the found operand count in operand count argument
 * @param line The command text
 * @param i The index to start analyzing from
 * @param destination At least a 2-cell buffer of strings for the extracted operand strings
 * @param operand_count The destination of the detected operands count
 * @param command The current command string
 * @return Whether analyzing succeeded
 */
bool analyze_operands(line_info line, int i, char **destination, int *operand_count, char *command);

#endif