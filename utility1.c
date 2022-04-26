#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "utility1.h"
#include "utility2.h" 



char *stralloc(char *s0, char* s1) {
	char *str = (char *)my_malloc(strlen(s0) + strlen(s1) + 1);
	strcpy(str, s0);
	strcat(str, s1);
	return str;
}



bool find_label(line_info line, char *symbol_dest) {
	int j, i;
	i = j = 0;

	/* Skip white chars at the beginning anyway */
	SKIP_WHITE(line.content, i)

	
	for (; line.content[i] && line.content[i] != ':' && line.content[i] != EOF && i <= MAX_LINE_LENGTH; i++, j++) {
		symbol_dest[j] = line.content[i];
	}
	symbol_dest[j] = '\0'; /* End of string */

	/* if it was a try to define label, print errors if needed. */
	if (line.content[i] == ':') {
		if (!is_valid_label_name(symbol_dest)) {
			print_error(line,
			                  "Invalid label name - cannot be longer than 32 chars, may only start with letter be alphanumeric.");
			symbol_dest[0] = '\0';
			return FALSE; /* No valid symbol, and no try to define one */
		}
		return TRUE;
	}
	symbol_dest[0] = '\0';
	return TRUE; /* There was no error */
}


struct instruction_lookup_item {
	char *name;
	instruction value;
};

static struct instruction_lookup_item
		instructions_lookup_table[] = {
		{"string", STRING_INST},
		{"data",   DATA_INST},
		{"entry",  ENTRY_INST},
		{"extern", EXTERN_INST},
		{NULL, NONE_INST}
};

instruction find_instruction_by_name(char *name) {
	struct instruction_lookup_item *curr_item;
	for (curr_item = instructions_lookup_table; curr_item->name != NULL; curr_item++) {
		if (strcmp(curr_item->name, name) == 0) {
			return curr_item->value;
		}
	}
	return NONE_INST;
}

bool is_int(char *string) {
	int i = 0;
	if (string[0] == '-' || string[0] == '+') string++; /* if string starts with +/-, it's OK */
	for (; string[i]; i++) { /* Just make sure that everything is a digit until the end */
		if (!isdigit(string[i])) {
			return FALSE;
		}
	}
	return i > 0; /* if i==0 then it was an empty string! */
}

void *my_malloc(long size) {
	void *ptr = malloc(size);
	if (ptr == NULL) {
		printf(" Memory allocation failed.");
		exit(1);
	}
	return ptr;
}

bool is_valid_label_name(char *name) {
	/* Check length, first char is alpha and all the others are alphanumeric, and not saved word */
	return name[0] && strlen(name) <= 31 && isalpha(name[0]) && is_alphanumeric_str(name + 1) &&
	       !is_reserved_word(name);
}

bool is_label_index(char *operand) {
    
	 int i ;
	 char label[32] ;
	 for(i=0 ;i<strlen(operand)  && operand[i]!= '[' ; i++ ) ;
	 
	 memcpy(label,operand,i) ;
	 label[i]='\0';
	 i++ ;
    /* checking that there is reg name after '[' */
	 if (is_valid_label_name(label))
	      if (((operand[i] == 'r' )&&
	           (operand[i+1]>='0')    &&
	           (operand[i+1]<='9')    &&
	           (operand[i+2]==']')    &&
			   (operand[i+3]=='\0'))  ||
	          ((operand[i] == 'r' )   &&
	           (operand[i+1]=='1')    &&
	           (operand[i+2]>='0')    &&
	           (operand[i+2]<='5')    &&
	           (operand[i+3]==']')    &&
			   (operand[i+4]=='\0')))
	      return TRUE;
	 
	 return FALSE ;

}

bool is_alphanumeric_str(char *string) {
	int i;
	/*check for every char in string if it is non alphanumeric char if it is function returns true*/
	for (i = 0; string[i]; i++) {
		if (!isalpha(string[i]) && !isdigit(string[i])) return FALSE;
	}
	return TRUE;
}

bool is_reserved_word(char *name) {
	int fun, opc;
	/* check if register or command */
	get_opcode_func(name, &opc, (funct *) &fun);
	if (opc != NONE_OP || get_register_by_name(name) != NONE_REG || find_instruction_by_name(name) != NONE_INST) return TRUE;

	return FALSE;
}

int print_error(line_info line, char *message, ...) { 
	int result;
	va_list args; /* for formatting */
	/* Print file+line */
	fprintf(stderr,"Error In %s:%ld: ", line.file_name, line.line_number);

	/* use vprintf to call printf from variable argument function (from stdio.h) with message + format */
	va_start(args, message);
	result = vfprintf(stderr, message, args);
	va_end(args);

	fprintf(stderr, "\n");
	return result;
}


