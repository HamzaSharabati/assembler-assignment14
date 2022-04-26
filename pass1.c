/* Contains major function that are related to the first pass */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "utility2.h"
#include "utility1.h"
#include "instructions.h"
#include "pass1.h"


/**
 * first pass process
 * @param line source line
 * @param i index
 * @param ic pointer to Instruction counter
 * @param code_img The code image array
 * @return if succeeded 
 */
static bool process_code(line_info line, int i, long *ic, machine_word **code_img);

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
                        table *symbol_table) {
	int i, j;
	char symbol[MAX_LINE_LENGTH];
	instruction instruction;

	i = 0;

	SKIP_WHITE(line.content, i) 
	if (!line.content[i] || line.content[i] == '\n' || line.content[i] == EOF || line.content[i] == ';')
		return TRUE; /* Empty or documentation line  */

	if (!find_label(line, symbol)) {
		return FALSE;
	}

	/* if illegal label */
	if (symbol[0] && !is_valid_label_name(symbol)) {
		print_error(line, "Illegal label name: %s", symbol);
		return FALSE;
	}
	if (symbol[0] != '\0') {
		for (; line.content[i] != ':'; i++); 
		i++;
	}

	SKIP_WHITE(line.content, i) 

	if (line.content[i] == '\n') return TRUE; /* Label-only line*/

	/* if label  already defined  */
	if (find_by_types(*symbol_table, symbol, 3, EXTERNAL_SYMBOL, DATA_SYMBOL, CODE_SYMBOL)) {
		print_error(line, "Symbol %s is already defined.", symbol);
		return FALSE;
	}

	/* Check if it's an instruction  */
	instruction = find_instruction(line, &i);

	if (instruction == ERROR_INST)
		return FALSE;
	

	SKIP_WHITE(line.content, i)

	
	if (instruction != NONE_INST) {
		if ((instruction == DATA_INST || instruction == STRING_INST) && symbol[0] != '\0')
			add_table_item(symbol_table, symbol, *DC, DATA_SYMBOL); /*add to the table */

		/*  encode string or data into data image and increase dc */
		if (instruction == STRING_INST)
			return process_string_instruction(line, i, data_img, DC);
		else if (instruction == DATA_INST)
			return process_data_instruction(line, i, data_img, DC);
			/* if extern add to table */
		else if (instruction == EXTERN_INST) {
			SKIP_WHITE(line.content, i)

			for (j = 0; line.content[i] && line.content[i] != '\n' && line.content[i] != '\t' && line.content[i] != ' ' && line.content[i] != EOF; i++, j++) {
				symbol[j] = line.content[i];
			}
			symbol[j] = '\0';
			/* If invalid external label name, it's an error */
			if (!is_valid_label_name(symbol)) {
				print_error(line, "Invalid external label name: %s", symbol);
				return TRUE;
			}
			add_table_item(symbol_table, symbol, 0, EXTERNAL_SYMBOL); 
		}
			
		else if (instruction == ENTRY_INST && symbol[0] != '\0') {
			print_error(line, "Can't define a label to an entry instruction.");
			return FALSE;
		}
		
	}
		
	else {
		
		if (symbol[0] != '\0')
			add_table_item(symbol_table, symbol, *IC, CODE_SYMBOL);
		
		return process_code(line, i, IC, code_img);
	}
	return TRUE;
}

/**
 * builds the data inside the additional code word by  given operand,
 * @param code_img The current code image
 * @param ic The current instruction counter
 * @param operand The operand to check
 */
static void build_extra_codeword_fpass(machine_word **code_img, long *ic, char *operand);


static bool process_code(line_info line, int i, long *ic, machine_word **code_img) {
	char operation[4]; 
	char *operands[2]; 
	opcode curr_opcode; 
	funct curr_funct;
	
	data_op_word *codeword; /*the first word (opcode and ARE onle) */
	code_prop *codeword2 ; /* second code  word (funct ,addres ...) */
	long ic_before;
	int j, operand_count;
	machine_word *word_to_write1;
	machine_word *word_to_write2;
	
	SKIP_WHITE(line.content, i)

	
	for (j = 0; line.content[i] && line.content[i] != '\t' && line.content[i] != ' ' && line.content[i] != '\n' && line.content[i] != EOF && j < 6; i++, j++) {
		operation[j] = line.content[i];
	}
	operation[j] = '\0'; /* string terminater */
	/* Get opcode & funct  */
	get_opcode_func(operation, &curr_opcode, &curr_funct);
	/* If invalid operation  */
	if (curr_opcode == NONE_OP) {
		print_error(line, "Unrecognized instruction: %s.", operation);
		return FALSE;
	}

	
	if (!analyze_operands(line, i, operands, &operand_count, operation))  {
	
		return FALSE;
	}

	
	if ((codeword= get_code_word(line, curr_opcode)) == NULL ) {
			
		if (operands[0]) {
			free(operands[0]);
			if (operands[1]) {
				free(operands[1]);
			}
		}
		
		return FALSE;
	}
	
    codeword2= get_code_prop(line, curr_opcode, curr_funct, operand_count, operands) ;
	
	ic_before = *ic;

	
	word_to_write1 = (machine_word *) my_malloc(sizeof(machine_word));
	(word_to_write1->word).data = codeword;
	word_to_write1->flag=0 ;
	word_to_write1->length=0 ;
	code_img[(*ic) - IC_INIT_VALUE] = word_to_write1; 
			 (*ic)++;
    word_to_write2 = (machine_word *) my_malloc(sizeof(machine_word));
	(word_to_write2->word).code = codeword2;
	word_to_write2->flag=1 ;
	word_to_write2->length=0 ;
	if(codeword2!=NULL) /*if there is need for additional word */
	{
		
	    code_img[(*ic) - IC_INIT_VALUE] = word_to_write2;
		(*ic)++;
	}
   
	if (operand_count--) { 
		build_extra_codeword_fpass(code_img, ic, operands[0]);
		free(operands[0]);
		if (operand_count) { 
			build_extra_codeword_fpass(code_img, ic, operands[1]);
			free(operands[1]);
		}
	}

   

	/* Add  length to the first  code word struct: */
	code_img[ic_before - IC_INIT_VALUE]->length = (*ic) - ic_before;

	return TRUE; 
}

static void build_extra_codeword_fpass(machine_word **code_img, long *ic, char *operand) {
	addressing_type operand_addressing = get_addressing_type(operand);
	
	/* if NONE or REGISTER no aditional words */
	if (operand_addressing != NONE_ADDR && operand_addressing != REGISTER_ADDR) {
		
		/* for IMMEDIATE the data encoded imedietely in an additional word */
		if (operand_addressing == IMMEDIATE_ADDR) {
			
			char *ptr;
			machine_word *word_to_write;
			long value = strtol(operand + 1, &ptr, 10);
			word_to_write = (machine_word *) my_malloc(sizeof(machine_word));
			word_to_write->length = 0; 
			word_to_write->flag=0 ;
			(word_to_write->word).data = build_data_op_word(IMMEDIATE_ADDR, value, FALSE);
			code_img[(*ic) - IC_INIT_VALUE] = word_to_write;
			(*ic)++;
			
		}
	    if (operand_addressing == DIRECT_ADDR || operand_addressing == INDEX_ADDR)
        (*ic)=(*ic)+2 ; /* two additional words requierad for direct and index for the base and offset */
	}
}