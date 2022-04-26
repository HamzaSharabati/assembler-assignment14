
/* Hamza sharabati , 302714530 , 14 assignment -( Systems programing lab ) */
 


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "output_files.h"
#include "utility1.h"
#include "pass1.h"
#include "pass2.h"
#include "macro.h"
 


/**
 * Processes source files
 * @param filename filename as typed without extention
 * @return 
 */
static bool process(char *filename);


int main(int argc, char *argv[]) {
	int i;
		
	
	bool succeeded = TRUE;
	
    /*getting files names */
	for (i = 1; i < argc; ++i) {
		
		if (!succeeded) puts("");
		
		
		succeeded = process(argv[i]);
		
		
	}
	return 0;
}



static bool process(char *filename) {
	
	
	int temp_c;
	long ic = IC_INIT_VALUE, dc = 0, icf, dcf; /*memory counters */
	bool is_success = TRUE;
	char *input_filename;
	char temp_line[MAX_LINE_LENGTH + 2];
	FILE *file_des; 
	machine_word *data_img[CODE_ARR_IMG_LENGTH]; /* imgs of data and code words */
	machine_word *code_img[CODE_ARR_IMG_LENGTH];
	table symbol_table = NULL; 
	line_info current_line; /* a struct to hold line details */
    if(!pre_process(filename)) return FALSE ; /* sending file name to pre-process and macro */
	
	input_filename = stralloc(filename, ".am"); /*adding extention to file name */

	file_des = fopen(input_filename, "r");
	if (file_des == NULL) {
		
		printf(" file \"%s.am\" is inaccessible .\n", filename);
		free(input_filename); 
		return FALSE;
	}

	
	current_line.file_name = input_filename;
	current_line.content = temp_line; 

	for (current_line.line_number = 1;
	     fgets(temp_line, MAX_LINE_LENGTH + 2, file_des) != NULL; current_line.line_number++) {
		if (strchr(temp_line, '\n') == NULL && !feof(file_des)) {
			print_error(current_line, "Line too long , Max line length %d.", MAX_LINE_LENGTH);
			is_success = FALSE;
			do {
				temp_c = fgetc(file_des);
			} while (temp_c != '\n' && temp_c != EOF);
		} else {
			/* sending file to first pass */
			if (!first_pass(current_line, &ic, &dc, code_img, data_img, &symbol_table)) {
				if (is_success) {
					icf = -1;
					is_success = FALSE;
				}
			}
		}
	}

	/*final counters value */
	icf = ic;
	dcf = dc;

	/* starting 2nd pass */
	if (is_success) {

	ic = IC_INIT_VALUE;

	/* adding IC to  DC  in data sympbol table */
	add_value_to_type(symbol_table, icf, DATA_SYMBOL);

	
	rewind(file_des); 

	for (current_line.line_number = 1; !feof(file_des); current_line.line_number++) {

		int i = 0;
		
		fgets(temp_line, MAX_LINE_LENGTH, file_des); 
		SKIP_WHITE(temp_line, i)
		if (code_img[ic - IC_INIT_VALUE] != NULL || temp_line[i] == '.' )
			is_success &= second_pass(current_line, &ic, code_img, &symbol_table);
		
	}

		/* Write files */
		if (is_success) 
	    is_success = write_output_files(code_img, data_img, icf, dcf, filename, symbol_table);
		
		
	}
   

	free(input_filename);

	free_table(symbol_table);




	return is_success;
}
