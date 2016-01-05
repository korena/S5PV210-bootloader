#include<stdio.h>
#include<stdint.h>


/*===========Prototypes================*/
void length_calc(uint32_t *length);
int makeBl2Image(uint32_t *length);
/*===========GLOBALS==================*/
char* INPUT,*OUTPUT;


int main(int argc,char** argv){


  FILE *input = NULL;
  uint32_t length = 0;


	if(argc < 2){
		printf("no input file was passed! What do you think you're doing?\n\n");
		return 1;
	}else{
		INPUT = argv[1];
		OUTPUT = argv[2];
	}

	length_calc(&length);
	makeBl2Image(&length);

  return 0;
}


void length_calc(uint32_t *length){
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
	}
  printf("Input File Length: %d Bytes\n", *length);
  if (file != NULL)	fclose(file);
}



int makeBl2Image(uint32_t *length){
  FILE *output = NULL;
  FILE *input = NULL;
  int guard = 1;
  uint32_t data = 0;
  
  input=fopen(INPUT, "rb");
  output = fopen(OUTPUT,"wb");
  
  if(output == NULL || input == NULL){
    printf("Error reading file ( input: %s or output: %s\n\n",INPUT,OUTPUT);
    return 1;
  }
	fwrite(length, sizeof(uint32_t), 1, output);
	printf("wrote BL2 expected header:\n");
	printf("BL2 length: %d\nMoving on to write actual BL2 data ...\n",*length);
	while (guard = fread(&data, sizeof(uint32_t), 1, input))
	{
		fwrite(&data, sizeof(uint32_t), 1, output);
	}
	printf("wrote BL2 data...\n");
	
if (input != NULL)	fclose(input);
if (output != NULL)	fclose(output);
	
return 0;
}
