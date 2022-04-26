#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utility1.h"
#include "table.h"



/**
 * Writes the code and data image into an .ob file
 * @param code_img The code image
 * @param data_img The data image
 * @param icf The final instruction counter
 * @param dcf The final data counter
 * @param filename The filename, without the extension
 * @return Whether succeeded
 */
static bool write_ob(machine_word **code_img, machine_word **data_img, long icf, long dcf, char *filename);

/**
 * Writes the .ext file from the sympol table
 * @param tab The symbol table to write
 * @param filename The filename without the extension
 * @return Whether succeeded
 */
static bool write_table_to_ext(table tab, char *filename);

/**
 * Writes the .ent file from the sympol table
 * @param tab The symbol table to write
 * @param filename The filename without the extension
 * @return Whether succeeded
 */
static bool write_table_to_ent(table tab, char *filename);


bool write_output_files(machine_word **code_img, machine_word **data_img, long icf, long dcf, char *filename,
                       table symbol_table) {
						   
	bool result;
	table externals = filter_table_by_type(symbol_table, EXTERNAL_REFERENCE);
	table entries = filter_table_by_type(symbol_table, ENTRY_SYMBOL);
	
	
	result = write_ob(code_img, data_img, icf, dcf, filename)  &&
	        
	        write_table_to_ext(externals, filename) &&
	         write_table_to_ent(entries, filename); 
	
	/* Release filtered tables */
	free_table(externals);
	free_table(entries);
	return result;
}

static bool write_ob(machine_word **code_img, machine_word **data_img, long icf, long dcf, char *filename) {
	int i;
	unsigned int A , B , C ,D , E ; /*for saving the values of each 4 bits */
	long val;
	FILE *file_desc;
	/* add extension of file to open */
	char *output_filename = stralloc(filename, ".ob");
	/* Try to open the file for writing */
	file_desc = fopen(output_filename, "w");
	free(output_filename);
	if (file_desc == NULL) {
		printf("Can't create or rewrite to file %s.", output_filename);
		return FALSE;
	}
    	
	/* print data/code word count on top */
	fprintf(file_desc, "%ld %ld", icf - IC_INIT_VALUE, dcf);

	/* starting from index 0, not IC_INIT_VALUE as icf, so we have to subtract it. */
	for (i = 0; i<icf-IC_INIT_VALUE; i++) {
		
		if(code_img[i]!=NULL) { /* making sure not to get "core dumped error " */
		
		if (code_img[i]->flag == 0) { /*if its an opp code word */
                  
			      val =( (code_img[i]->word.data->data) /* opp code is in place no need to shift */
				         | (code_img[i]->word.data->ARE << 16) /*shifting bits to there places */
						 | (code_img[i]->word.data->lastbit << 19));
		} else {    /*if it's second data word with funct and addressing */

			 val = ((code_img[i]->word.code->dest_addressing)|
			         (code_img[i]->word.code->dest_register << 2) | /*shifting bits to there places */
				    ( code_img[i]->word.code->src_addressing << 6) |
					( code_img[i]->word.code->src_register << 8 )|
					( code_img[i]->word.code->funct << 12) |
					(code_img[i]->word.code->ARE << 16 ) |
					(code_img[i]->word.code->lastbit << 19) )  ;

		}
		/*using musks to take the values of each for bits in the word */
        E=val & 0x0F ;
		D=(val>>4) & 0x0F ;
		C=(val>>8) & 0x0F ;
		B=(val>>12) & 0x0F ;
		A=(val>>16) & 0x0F ;

		/* Write the value to the file - first */
		fprintf(file_desc, "\n%.4d \tA%1x-B%1x-C%1x-D%1x-E%1x  ", i + 100,  A , B , C ,D , E);
	}}
	
    
	/* data same as with code */
	for (i = 0; i < dcf; i++) {
		
		 val =( (data_img[i]->word.data->data)  /*shifting bits to there places */
			  | (data_img[i]->word.data->ARE << 16)
			  | (data_img[i]->word.data->lastbit << 19));

		  E=val & 0x0F ;
		  D=(val>>4) & 0x0F ;
		  C=(val>>8) & 0x0F ;
		  B=(val>>12) & 0x0F ;
		  A=(val>>16) & 0x0F ;	  

		fprintf(file_desc, "\n%.4ld \tA%1x-B%1x-C%1x-D%1x-E%1x  ", icf + i, A , B , C ,D , E);
	}

	/* Close the file */
	fclose(file_desc);
	return TRUE;
}

static bool write_table_to_ent(table tab, char *filename) {
	
	FILE *file_desc;
	/* concatenate filename & extension, and open the file for writing: */
	char *full_filename = stralloc(filename,".ent");
	/* nothing to write */
	if (tab == NULL) return TRUE;
	
	file_desc = fopen(full_filename, "w");
	free(full_filename);
	
	if (file_desc == NULL) {
		printf("Can't create or rewrite to file %s.", full_filename);
		return FALSE;
	}
	
    
	/* writing base  for each address by dividing by 16(as int) then multiplying by 16 , then getting the offset by substracting the base from the original value */

	fprintf(file_desc, "%s %.ld, %.ld ", tab->key, (((tab->value)/16)*16) , ((tab->value)-(((tab->value)/16)*16)) );
	while ((tab = tab->next) != NULL) {
		fprintf(file_desc, "\n%s %.ld, %.ld ", tab->key, (((tab->value)/16)*16) , ((tab->value)-(((tab->value)/16)*16)) );
	}
	fclose(file_desc);
	return TRUE;
}

static bool write_table_to_ext(table tab, char *filename) {

	FILE *file_desc;
	
	char *full_filename = stralloc(filename,".ext");
	if (tab == NULL) return TRUE;

	file_desc = fopen(full_filename, "w");
	free(full_filename);
	
	if (file_desc == NULL) {
		printf("Can't create or rewrite to file %s.", full_filename);
		return FALSE;
	}
	
    /* writing base  for each address by dividing by 16(as int) then multiplying by 16 , then getting the offset by substracting the base from the original value */
	
	fprintf(file_desc, "%s BASE   %.1ld \n%s OFFSET %.1ld ", tab->key, (((tab->value)/16)*16) ,tab->key , ((tab->value)-(((tab->value)/16)*16)) );
	while ((tab = tab->next) != NULL) {
		fprintf(file_desc, "\n%s BASE   %.1ld \n%s OFFSET %.1ld ", tab->key, (((tab->value)/16)*16) ,tab->key , ((tab->value)-(((tab->value)/16)*16)) );
	}
	fclose(file_desc);
	return TRUE;
}