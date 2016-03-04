#include <stdarg.h>
#include <terminal.h>
#include <stdint.h>
#include <string.h>


static char out[512] = {0};


/*
 * (probably unsafe) implementation of printf 
 */
int printf(const char* str, ...){
	va_list ap;
	int i=0,j=0;
	int arg;
	va_start(ap,str);
	char charInt[12] ={0}; // maximum size of printable variable
	
	memset(out, 0, sizeof(out));

	while(str != NULL && *str != '\0' && i < 512){
		if(*str == '%'){
			switch(*++str){
				case 'x':{
						 arg = va_arg(ap, int);
						 printnum(charInt,12,"%x",arg);
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
						 printnum(charInt,12,"%d",arg);
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
			out[i++]=*str++;
		}
	}
	//better late than never !
	if(i < 512 && (out+i) != NULL && *(out+i) != '\0'){
		*(out+i)='\0';
	}
	uart_print(out);
	return 0;
}




char *strcpy(char *strDest, const char *strSrc)
{
	char *temp = strDest;
	while(*strDest++ = *strSrc++); // or while((*strDest++=*strSrc++) != '\0');
	return temp;
}


/**
 * snprintf substitute,barely tested, no error checking, blind trust.
 */
int printnum (char *__restrict__ s, size_t maxlen, const char *__restrict__ format, uint32_t num){
int i=0;
uint32_t base =1;

for (;(i<maxlen) && (s != NULL);i++)
        s[i]=0;



while(format != NULL && *format != '\0'){
        if(*format == '%' && *(format+1) != '\0'){
                switch(*++format){
                case 'x':{
                         // print in hex format into *s  
                         
			// in case zero was passed :
			if(num == 0){
				*s = '0';
				 if(++s != NULL){
					 *s='\0';
				 	 return 0;
				 } else{
					 *--s='\0';
				 	 return -1;
				 }
			}
                         for(i=7;i>=0;i--){
                                 if(((num >> i*4) & 0xF) != 0)
                                         break; // skipping leading zeros
                         }
                         for(;i>=0;i--){
                                 if(maxlen != 0 && s != NULL){
                                         if(((num >> i*4)& 0xF) <= 0x9){
                                                 *s =(char) (((num >> i*4) & 0xF) + '0');
                                         }else{
                                                 *s =(char) ((((num >> i*4) & 0xF) + 'A')-10);
                                         }
                                         maxlen--;
                                         s++;
                                 } else{
                                         return -1;
                                 }
                         }
			 // terminate string ...
			 if(maxlen !=0 && s != NULL){
				 *s = '\0';
			 }else{
				 return -1;
			 }
			 return 0;
			 }break;
		case 'd':{
				 // print in decimal format into *s  
			// in case zero was passed :
			if(num == 0){
				*s = '0';
				 if(++s != NULL){
					 *s='\0';
				 	 return 0;
				 } else{
					 *--s='\0';
				 	 return -1;
				 }
			}
				 while(base < num){
					 base = (base << 3)+(base << 1);
				 }
				 base /= 10;
				 while((base >= 1) && (maxlen != 0) && (s != NULL)){
					 *s = (char)(num/base + '0');
					 num -= ((num/base)*base);
					 maxlen--;
					 s++;
					 base /= 10;
				 }
				 // terminate string ...
				 if(maxlen !=0 && s != NULL){
					 *s = '\0';
				 }else{
					 return -1;
				 }
				 return 0;
			 }break;

		default: {
				 return -2;
			 }
		}

	}

}
return -1;
}

