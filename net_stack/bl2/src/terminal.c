#include <stdint.h>
#include <stdarg.h>
#include <terminal.h>


/*
 * Lazy implementation of printf 
 */
void printf(char* str, ...){
	va_list ap;
	int i=0;
	char chr;

	while(str != NULL && *str != '\0'){
		if(*str == '%')
		str++;
		switch(chr){
		case "x":{
			 //TODO: convert the corresponding int argument
			 //into a string, and copy it to the output string 
			 }	
		}		
		str++;	
	}
}
