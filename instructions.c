#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility1.h"
#include "globals.h"

/* finds first instruction . if no such one, returns NONE */
instruction find_instruction(line_info line, int *index) {
	char temp[MAX_LINE_LENGTH];
	int j;
	instruction result;

	SKIP_WHITE(line.content, *index) /* skip space */
	if (line.content[*index] != '.') return NONE_INST;

	for (j = 0; line.content[*index] && line.content[*index] != '\t' && line.content[*index] != ' '; (*index)++, j++) {
		temp[j] = line.content[*index];
	}
	temp[j] = '\0'; /* End of string */
	/* if invalid instruction  */
	if ((result = find_instruction_by_name(temp+1)) != NONE_INST) return result;
	print_error(line, "Invalid instruction name: %s", temp);
	return ERROR_INST; 
}


data_op_word *get_data_word( int data) {
	
	data_op_word *codeword;
	
	/* Create the code word by the data: */
	
	codeword = (data_op_word *) my_malloc(sizeof(data_op_word));

	codeword->data = data;
	codeword->ARE = 4 ;  
	codeword->lastbit = 0 ;
	
	
	return codeword;
}


bool process_string_instruction(line_info line, int index, machine_word **data_img, long *dc) {
     
	 
	char temp_str[MAX_LINE_LENGTH];
	char *last_quote = strrchr(line.content, '"');
	SKIP_WHITE(line.content, index)
	if (line.content[index] != '"') {
		
		print_error(line, "Missing opening quote ");
		return FALSE;
	} else if (&line.content[index] == last_quote) { 
		print_error(line, "Missing closing quote ");
		return FALSE;
	} else {
		int i;
		/* Copy the string  until end of line */
		for (i = 0;line.content[index] && line.content[index] != '\n' &&
		       line.content[index] != EOF; index++,i++) {
				temp_str[i] = line.content[index];
		}
		
		/*building word from the copied string */
		for(i = 1;temp_str[i] && temp_str[i] != '"'; i++) {
           
		    machine_word *word_to_write ;
            word_to_write = (machine_word *) my_malloc(sizeof(machine_word));
			(word_to_write->word).data = get_data_word(temp_str[i]);
	        word_to_write->flag=0 ;
			data_img[*dc] = word_to_write ;
			(*dc)++;
		}
		/*adding string terminater */
		  {machine_word *word_to_write ;
            word_to_write = (machine_word *) my_malloc(sizeof(machine_word));
			(word_to_write->word).data = get_data_word('\0');
	        word_to_write->flag=0 ;
			data_img[*dc] = word_to_write ;
			(*dc)++;
		  }
	
}
return TRUE;
}

bool process_data_instruction(line_info line, int index, machine_word **data_img, long *dc) 
{  
	char temp[80], *temp_ptr;
	int value;
	int i;
	machine_word *word_to_write ;
	SKIP_WHITE(line.content, index) 
	if (line.content[index] == ',') {
		print_error(line, "Unexpected comma after .data instruction"); 
	}
	do {
		for (i = 0;
		     line.content[index] && line.content[index] != EOF && line.content[index] != '\t' &&
		     line.content[index] != ' ' && line.content[index] != ',' &&
		     line.content[index] != '\n'; index++, i++) {
			temp[i] = line.content[index];
		}
		temp[i] = '\0'; /*  string terminater */
		if (!is_int(temp)) {
			print_error(line, "Expected integer for .data instruction (got '%s')", temp);
			return FALSE;
		}
		
		    value = strtol(temp, &temp_ptr, 10);
            /*building machine word from integer */
		  
            word_to_write = (machine_word *) my_malloc(sizeof(machine_word));
			(word_to_write->word).data = get_data_word(value);
	        word_to_write->flag=0 ;
			data_img[*dc] = word_to_write ;

		

		(*dc)++; 
		SKIP_WHITE(line.content, index)
		if (line.content[index] == ',') index++;
		else if (!line.content[index] || line.content[index] == '\n' || line.content[index] == EOF)
			break; 
		SKIP_WHITE(line.content, index)
		if (line.content[index] == ',') {
			print_error(line, "Multiple commas.");
			return FALSE;
		} else if (line.content[index] == EOF || line.content[index] == '\n' || !line.content[index]) {
			print_error(line, "Missing data after comma");
			return FALSE;
		}
	} while (line.content[index] != '\n' && line.content[index] != EOF);
	return TRUE;
}
