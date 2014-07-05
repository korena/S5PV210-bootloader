#include<stdio.h>
#include<stdint.h>




int main (int argc,char** argv){
  FILE *file = NULL;
  FILE *dump = NULL;
  uint32_t data;
  int guard = 1;
  
  if(argc != 2){
    printf("pass me a file dude!\n");
    printf("usage: %s <input binary name>\n",argv[0]);
    return 0;
  }
  
  file = fopen(argv[1],"rb");
  dump = fopen("dumped.contents","w");
  if(file == NULL){
    printf("ERROR reading file ...\n");
    return 1;
  }
  
  
  while(guard = fread(&data,sizeof(uint32_t),1,file)){
    fprintf(dump,"0x%08X\n",data);
    }
    fclose(file);
    fclose(dump);
}