#include<stdio.h>
#include<stdint.h>


/*=========complimentary defines=========*/
#define REDTEXT  "\x1B[31m"
#define YELLOWTEXT  "\x1B[33m"

/*===========Macro defines==============*/
#define	max_BL1_length	16384


#define BL1_LENGTH	(16*1024)
#define DEBUG		1

/*===========Prototypes================*/
void checksum_calc(uint32_t *checksum,uint32_t *length);
int makeImage(uint32_t *checksum,uint32_t *length);
void showResults(char *inputFileName,char *outputFileName);
/*===========GLOBALS==================*/
char* INPUT,*OUTPUT;


int main(int argc,char** argv){

  FILE *input = NULL;
  uint32_t checksum = 0;
  uint32_t length = 0;


if(argc < 2){
printf("no input file was passed! What do you think you're doing?\n\n");
return 1;
}else{
INPUT = argv[1];
OUTPUT = argv[2];
}
    /*read in BL1.bin,calculate checksum, and get the length while you're at it ...*/
    checksum_calc(&checksum,&length);
  /*check if the file is too big to fit ...*/
  if(length > BL1_LENGTH){
   printf("dude, this thing wont fit! (Size is %d)\n\n",length); 
    return 2;
  }
  /*create the output image*/
  makeImage(&checksum,&length);
  
#ifdef DEBUG
showResults(INPUT,OUTPUT);  
#endif  
  return 0;
}

void checksum_calc(uint32_t *checksum,uint32_t *length){
  int guard = 1;
  uint32_t data = 0;
  FILE* file = fopen(INPUT, "rb");
  
  if(file == NULL){
    printf("error opening file %s\n",INPUT);
    return;
  }
  
  	while (guard = fread(&data, sizeof(uint32_t), 1, file))
	{
		*length += 4;
		*checksum += ((data >> 0) & 0xff);
		*checksum += ((data >> 8) & 0xff);
		*checksum += ((data >> 16) & 0xff);
		*checksum += ((data >> 24) & 0xff);	
	}
  printf("Input File Length: %d Bytes\n", *length);
  printf("Input File Checksum: %d\n", *checksum);
  if (file != NULL)	fclose(file);
}
int makeImage(uint32_t *checksum,uint32_t *length){
  FILE *output = NULL;
  FILE *input = NULL;
  int guard = 1;
  uint32_t data = 0;
  uint32_t actualLength = 0;
  
  input=fopen(INPUT, "rb");
  output = fopen(OUTPUT,"wb");
  
  if(output == NULL || input == NULL){
    printf("Error reading file ( input: %s or output: %s\n\n",INPUT,OUTPUT);
    return 1;
  }
	*length = BL1_LENGTH;
	fwrite(length, sizeof(uint32_t), 1, output);
	fwrite(&data, sizeof(uint32_t), 1, output);
	fwrite(checksum, sizeof(uint32_t), 1, output);
	fwrite(&data, sizeof(uint32_t), 1, output);
	printf("wrote BL1 expected header:\n");
	printf("BL1 length: %d\n",*length);
	printf("Reserved:   %d\n",data);
	printf("BL1 CS:     %d\n",*checksum);
	printf("Reserved:   %d\n",data);
	actualLength = 16;
	while (guard = fread(&data, sizeof(uint32_t), 1, input))
	{
		actualLength += 4;
		fwrite(&data, sizeof(uint32_t), 1, output);
	}
	printf("wrote BL1 data...\n");
	
	data = 0;
	for (; actualLength < (BL1_LENGTH); actualLength += 4)
	{
		fwrite(&data, sizeof(uint32_t), 1, output);
	}
	printf("padding inserted ...\n");
	
if (input != NULL)	fclose(input);
if (output != NULL)	fclose(output);
	
return 0;
}

void showResults(char *inputFileName,char *outputFileName){
  
 FILE *input = NULL;
 FILE *output = NULL;
  uint32_t data;
  int guard = 10;
  int lim = 1;
  input = fopen(inputFileName,"rb");
  output = fopen(outputFileName,"rb");
  
  if(input == NULL || output == NULL){
    printf("ERROR reading files from showResults function ...\n");
    return;
  }
  
  printf("showing the first 10 words of each file ...\n");
  
  printf("%s contents:\n",INPUT);
  while(guard && lim){
    lim = fread(&data,sizeof(uint32_t),1,input);
    printf("0x%08X\n",data);
    guard -= 1;
    }
    guard = 10;
    lim = 1;
  printf("%s contents:\n",OUTPUT);
  while(guard && lim){
    lim = fread(&data,sizeof(uint32_t),1,output);
    printf("0x%08X\n",data);
    guard -= 1;
    }
    fclose(input);
    fclose(output);
}

