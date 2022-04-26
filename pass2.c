#include <stdio.h>
#include <stdlib.h>
#include "pass2.h"
#include "utility2.h"
#include "utility1.h"
#include "string.h"

int process_spass_operand(line_info line, long *curr_ic, long *ic, char *operand, machine_word **code_img,
                          table *symbol_table);


bool second_pass(line_info line, long *ic, machine_word **code_img, table *symbol_table) {
	char *indexOfColon;
	char *token;
	long i = 0;
	SKIP_WHITE(line.content, i)
	/* Empty/Comment line - no processing needed! */
	if (line.content[i] == ';' || line.content[i] == '\n') return TRUE;
	indexOfColon = strchr(line.content, ':');
	/*check for label */
	if (indexOfColon != NULL) {
		i = indexOfColon - line.content;
		i++;
	}
	SKIP_WHITE(line.content, i)

	/* .instruction */
	if (line.content[i] == '.') {
		/*if it's entry we add it to the symbol table*/
		if (strncmp(".entry", line.content, 6) == 0) {
			i += 6;
			SKIP_WHITE(line.content, i)
			token = strtok(line.content + i, " \n\t");
			/* if label is already marked as entry, ignore. */
			if (token == NULL) {
				print_error(line, "You have to specify a label name for .entry instruction.");
				return FALSE;
			}
			if (find_by_types(*symbol_table, token, 1, ENTRY_SYMBOL) == NULL) {
				table_entry *entry;
				token = strtok(line.content + i, "\n"); /*get name of label*/
	
				/* if symbol is not defined as data/code */
				if ((entry = find_by_types(*symbol_table, token, 2, DATA_SYMBOL, CODE_SYMBOL)) == NULL) {
					/* if defined as external print error */
					if ((entry = find_by_types(*symbol_table, token, 1, EXTERNAL_SYMBOL)) != NULL) {
						print_error(line, "The symbol %s can be either external or entry, but not both.",
						                  entry->key);
						return FALSE;
					}
					
					print_error(line, "The symbol %s for .entry is undefined.", token);
					return FALSE;
				}
				add_table_item(symbol_table, token, entry->value, ENTRY_SYMBOL);
			}
		}
		return TRUE;
	}
	return add_symbols_to_code(line, ic, code_img, symbol_table);
}

/**
 * Find the symbol that need replacment in a code line, and replacing them by the address in the symbol table.
 * @param line The current code line that is being processed
 * @param ic A pointer to the current instruction counter
 * @param code_img The machine code image array
 * @param symbol_table The  symbol table

 * @return whether succeeded
 */
bool add_symbols_to_code(line_info line, long *ic, machine_word **code_img, table *symbol_table) {
	char temp[80];
	char *operands[2];
	int i = 0, operand_count;
	bool isvalid = TRUE;
	long curr_ic = *ic; 
	/* Get the total word length of current code text line in code binary image */
	int length = code_img[(*ic) - IC_INIT_VALUE]->length;
	/* if the length is 1 or 2, then there's only the code word, no data. */
	if (length > 2) {
		/* Now, we need to skip command, and get the operands themselves: */
		SKIP_WHITE(line.content, i)
		find_label(line, temp);
		if (temp[0] != '\0') { 
			for (; line.content[i] && line.content[i] != '\n' && line.content[i] != EOF && line.content[i] != ' ' &&
			       line.content[i] != '\t'; i++);
			i++;
		}
		SKIP_WHITE(line.content, i)
		
		for (; line.content[i] && line.content[i] != ' ' && line.content[i] != '\t' && line.content[i] != '\n' &&
		       line.content[i] != EOF; i++);
		SKIP_WHITE(line.content, i)
		
		analyze_operands(line, i, operands, &operand_count, NULL);
		/* Process operands, if needed. if failed return failure. otherwise continue */
		if (operand_count--) {
			isvalid = process_spass_operand(line, &curr_ic, ic, operands[0], code_img, symbol_table);
			free(operands[0]);
			if (!isvalid) return FALSE;
			if (operand_count) {
				isvalid = process_spass_operand(line, &curr_ic, ic, operands[1], code_img, symbol_table);
				free(operands[1]);
				if (!isvalid) return FALSE;
			}
		}
	}
	/* Make the current pass IC as the next line ic */
	(*ic) = (*ic) + length ;
	return TRUE;
}

/**
 * Builds the additional data word for operand in the second pass, if needed.
 * @param line current line
 * @param curr_ic  instruction pointer of source code line start
 * @param ic
 * @param operand The operand string
 * @param code_img The code image array
 * @param symbol_table The symbol table
 * @return Whether succeeded
 */
int process_spass_operand(line_info line, long *curr_ic, long *ic, char *operand, machine_word **code_img,
                          table *symbol_table) {
	addressing_type addr = get_addressing_type(operand);
	machine_word *word_to_write1;
	machine_word *word_to_write2;
	
	
	
    
	if (DIRECT_ADDR == addr || INDEX_ADDR == addr) {
	    
		int i ;
	    char label[32] ;
		long base;
		long hist ;
		table_entry *entry ;
		/*processing only the label without the index if its an index operand */
	    for(i=0 ;i<strlen(operand)  && operand[i]!= '[' ; i++ ) ; 
	    memcpy(label,operand,i) ;
	    label[i]='\0';
		

		 entry = find_by_types(*symbol_table, label, 3, DATA_SYMBOL, CODE_SYMBOL, EXTERNAL_SYMBOL);
		if (entry == NULL) {
			print_error(line, "The symbol %s not found", label);
			return FALSE;
		}
		/*calculating base and hist */
		base = ((entry->value)/16)*16 ;
		hist = (entry->value) - base ;
	  

		/* Add to externals reference table if it's an external. increase ic because it's the next data word */
		if (entry->type == EXTERNAL_SYMBOL) {
			add_table_item(symbol_table, operand, (*curr_ic) + 2, EXTERNAL_REFERENCE);
		}
       
		/*encoding the addreses of the labels (base and hist )*/
		word_to_write1 = (machine_word *) my_malloc(sizeof(machine_word));
		word_to_write1->length = 0;
		word_to_write1->flag=0 ;
		word_to_write1->word.data = build_data_op_word(addr, base, entry->type == EXTERNAL_SYMBOL);
		/* +2 because there is 2 words fo the code itself */
		(*curr_ic)=(*curr_ic)+2;
		code_img[((*curr_ic)) - IC_INIT_VALUE] = word_to_write1;
		
		word_to_write2 = (machine_word *) my_malloc(sizeof(machine_word));
		word_to_write2->length = 0;
		word_to_write2->flag=0 ;
		word_to_write2->word.data = build_data_op_word(addr, hist, entry->type == EXTERNAL_SYMBOL);
		(*curr_ic)++ ;
		code_img[((*curr_ic)) - IC_INIT_VALUE] = word_to_write2;
		
	

	}
	return TRUE;
}
