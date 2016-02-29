#include <stdarg.h>
#include <terminal.h>
#include <stdarg.h>
//#include <stdlib.h>




/*
 * (probably unsafe) implementation of printf 
 */
int printf(const char* str, ...){
	va_list ap;
	int i=0,j=0;
	int arg;
	va_start(ap,str);
	char out[512]; // size over cpu cycles tradeoff...
	char charInt[12]; // maximum size of printable variable
	while(str != NULL && *str != '\0'){
		if(*str == '%'){
			switch(*++str){
				case 'x':{
						 arg = va_arg(ap, int);
						 snprintf(charInt,12,"%x",arg);
						 for(j=0;j<12;j++){
							 if(charInt[j] != '\0'){
								 out[i++] = charInt[j];
							 }else{
								 str++;
								 break;
							 }
						 }
					 }
					 break;
				case 'd':{
						 arg = va_arg(ap, int);
						 snprintf(charInt,12,"%d",arg);
						 for(j=0;j<12;j++){
							 if(charInt[j] != '\0'){
								 out[i++] = charInt[j];
							 }else{
								 str++;
								 break;
							 }
						 }
					 }
					 break;
				default:{
						out[i++]='%';
						out[i++]=*str++;
					}
					break;
			}
		}else{
			if(i<512){
				out[i++]=*str++;
			}
		}
	}
	//better late than never !
	if(i < 512 && (out+i) != NULL && *(out+i) != '\0'){
		*(out+i)='\0';
	}
	uart_print(out);
}




char *strcpy(char *strDest, const char *strSrc)
{
	char *temp = strDest;
	while(*strDest++ = *strSrc++); // or while((*strDest++=*strSrc++) != '\0');
	return temp;
}


int snprintf (char *__restrict__ s, size_t maxlen, const char *__restrict__ format, ...){
//TODO: implement me!


}


