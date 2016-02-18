#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>


void processBits(void);

uint8_t bits[64] = {0};
uint8_t maskSize = 32; //default

int main (int argc,char** argv){

uint8_t i = 0;
uint8_t j = 0;
uint8_t k = 0;
uint8_t size =0;
uint8_t character ;
uint8_t hackChar;
bool digit = false;
bool range = false;
bool unit = false;

	if(argc < 2 || argc > 3){
		/* We print argv[0] assuming it is the program name */
		printf( "usage: %s [N]|[N:N]|[N,N,N:N,...] 8|16|24|32|64 \n", argv[0] );
		printf("You screwed up.\n");
		return 1;
	}else{
		// initialize bits array ...
		for (i=0; i<sizeof(bits);i++){
			bits[i] = 255;
		}
		// maskSize first ...
		if(argc == 3 && strlen(argv[2]) > 0){
			if(strlen(argv[2]) > 2){
				goto LENGTH_ERROR;
			}else if(argc == 3 && strlen(argv[2]) == 2) {
				maskSize = ((argv[2][0]-'0') *10) + (argv[2][1]-'0');		
				if(maskSize != 64 && maskSize != 32 && maskSize != 24 && maskSize != 16){
					goto LENGTH_ERROR;
				}
			}else if(argc == 3 && strlen(argv[2]) == 1){
				maskSize = argv[2][0]-'0';
				if(maskSize != 8)goto LENGTH_ERROR;	
			}
		}
		// range and bits 
		size = strlen(argv[1]);
		for(i=0;i<size && (argv[1][i] != '[');i++); // avoiding leading garbage
		for(i;i<size;i++){
			character = argv[1][i];
			if(isdigit(character)){
				if(!range && !unit && (character - '0' < maskSize)){
					if(j<64){
						bits[j] = character - '0';
						j++;
						unit = true;
					}else{
						goto RANGE_ERROR;
					}		
				}else if (!range && unit){
					if(j>0 && j<64){ // paranoid condition
						bits[j-1] = bits[j-1]*10+character-'0';
						if(bits[j-1] > maskSize) goto RANGE_ERROR;
						unit = false;
					}else{
						goto RANGE_ERROR;
					}
						
				}else if(range){
					if(unit) goto RANGE_ERROR;  // this would be odd ...

					// I'm really sorry for this ...
					if(i<(size-1) && isdigit(argv[1][i+1])){
				 		hackChar = ((character-'0')*10)+(argv[1][i+1]-'0');	
						i++;	
					}else{
					hackChar = character-'0';
					}
					if(j>0 && (hackChar < maskSize)){
						if(bits[j-1] < hackChar){
							while(bits[j-1] < hackChar){
								if(j<64){
									bits[j] = bits[j-1]+1;
									j++;
								}else
									goto RANGE_ERROR;		
							}
						}else if(bits[j-1] > hackChar){
							while(bits[j-1] > hackChar){
								if(j<64){
									bits[j] = bits[j-1]-1;
									j++;
								}else
									goto RANGE_ERROR;		
							}
						}
					}else{
						goto RANGE_ERROR;
					}
					range = false;
				}
				
			}else{
				switch(character){
					case ']' :{
						 // call the processing function 
						 processBits();
						 return 0;
						  }break;
					case ':' :{
					         // flush units ...
						unit = false;
						range = true;
						  }break;
					case ',' :{
						 // flush units ...
						 unit = false;
					      	continue;  
						  }break;
					default: {
						 continue; // for clarity, because the rest of this code is extremely readable.
						 }break;
				}
			
			}
		}
	}

RANGE_ERROR: printf("ERROR parcing data (range error)\n");return 1;
LENGTH_ERROR: printf("ERROR parcing data (unsupported data length)\n");return 1;
}


void processBits(void)
{
	uint8_t i = 0;
	unsigned int hexResult = 0;
	for(i;i<64;i++){
		if(bits[i] != 255)
			hexResult |= (1 << bits[i]);		
	}

		printf("Mask: %#0*x\n",(maskSize/4+2),hexResult);
//		switch(maskSize){
//			case 8:
//		printf("Mask: %#0*x\n",4,hexResult);
//		break;
//			case 16:
//		printf("Mask: %#0*x\n",6,hexResult);
//		break;
//			case 24:
//		printf("Mask: %#0*x\n",8,hexResult);
//		break;
//			case 32:
//		printf("Mask: %#0*x\n",10,hexResult);
//		break;
//			case 64:
//		printf("Mask: %#0*x\n",18,hexResult);
//		break;
//		}
}
