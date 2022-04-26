#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "utility1.h"
#include"macro.h"

m_table tab=NULL ; /* table to hold macros */
bool pre_process(char *filename) {
	
	
	bool in_macro = FALSE;
	
	char *input_filename;
	char *output_filename;
	char macro_text[MAX_LINE_LENGTH]={'\0'} ;
	char temp_line[MAX_LINE_LENGTH + 2];
	char temp_key[MAX_LINE_LENGTH]  ;
	FILE *input;
	FILE *output;
	input_filename = stralloc(filename, ".as");
	input = fopen(input_filename, "r");
	
	if (input == NULL) {
		/* if file couldn't be opened, write to stderr. */
		printf("Error  file \"%s.as\" is inaccessible for reading. skipping it.\n", filename);
		free(input_filename);   
	   
		return FALSE;
	}

	output_filename = stralloc(filename, ".am");
	output = fopen(output_filename, "w");
	
	if (output == NULL) {
		/* if file couldn't be opened, write to stderr. */
		printf("Error: file \"%s.am\" is inaccessible for writing or cant be opened. skipping it.\n", filename);
		free(input_filename); 
		free(output_filename) ;  
	
		return FALSE;
	}
	
	while(fgets(temp_line,MAX_LINE_LENGTH,input)!=NULL)
		{
		 
		 if(!in_macro){
		 int i=0 ;
		 int j=0 ;
		 char temp_word[MAX_LINE_LENGTH] ;
		 char *temp_text ;
		 SKIP_WHITE(temp_line, i)
		 while(temp_line[i]!='\n'&& temp_line[i]!='\t'&& temp_line[i]!=' ') 
		 {temp_word[j]=temp_line[i] ;
		  j++ ;
		  i++ ;
		 }
		 /* adding string terminater to word */
		 temp_word[j]='\0' ;
		 if((strcmp("macro",temp_word))==0) 
		  { 
			j=0 ;
			SKIP_WHITE(temp_line, i)
			/* copying key to temp key */
			while(temp_line[i]!='\n'&& temp_line[i]!='\t'&& temp_line[i]!=' '&& temp_line[i]!=EOF) 
		    {temp_key[j]=temp_line[i] ;
		     j++ ;
		     i++ ;

		    }
			/* adding string terminater at the end of key */
			temp_key[j]='\0' ;
			in_macro=TRUE ;
		  }
		  /* if not a macro difinition looking if the word is pre-difined macro */ 
		  else if ((temp_text=find_macro(tab,temp_word))) {
           
			  int k=0 ;
			  
              while(temp_text[k]!='\0')
			  {
              fputc(temp_text[k],output) ;
			  k++ ;
			  }
		  } /* if not a macro copy it to output file */
		  else  fputs(temp_line,output) ;
		  
		}else { int i=0 ;
		        int j=0 ;
		        char temp_word[MAX_LINE_LENGTH] ;
		        SKIP_WHITE(temp_line, i)
				/* adding macro to the table */
		        while(temp_line[i]!='\n'&& temp_line[i]!='\t'&& temp_line[i]!=' ') 
		        {temp_word[j]=temp_line[i] ;
		         j++ ;
		         i++ ;
		         }
		         temp_word[j]='\0' ;
		        if((strcmp("endm",temp_word))==0){ 
					add_macro(&tab,temp_key,macro_text) ;
					
					in_macro=FALSE ;}
				else  strcat(macro_text,temp_line) ;

			}

		  }

       

		
    
	fclose(input) ;
	fclose(output) ;

    free_m_table( tab) ;
    return TRUE ;
}

void add_macro(m_table *tab, char *key, char *text) {
	
	
	char *temp_key;
	char *temp_text;

	m_table new_entry;
	
    new_entry = (m_table) my_malloc(sizeof(m_table));
	
	temp_key = (char *) my_malloc(strlen(key) + 1);
	temp_text = (char *) my_malloc(strlen(text) + 1);
	strcpy(temp_key, key);
	strcpy(temp_text, text);
	new_entry->key= temp_key;
	new_entry->text = temp_text;
	new_entry->next = (*tab);
	(*tab) = new_entry;
	
	
	
}

char *find_macro (m_table tab, char *key)
{  

   if((tab!=NULL)&&((strcmp(key,tab->key))==0))
   {
    return (tab->text) ;
   }
return NULL ;

}

void free_m_table(m_table tab) {
	m_table prev_entry, curr_entry = tab;
	while (curr_entry != NULL) {
	prev_entry = curr_entry;
	curr_entry = curr_entry->next;
	free(prev_entry->key);
	free(prev_entry->text) ;
	free(prev_entry);
	}
}