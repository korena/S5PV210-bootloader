#include<stdio.h>
#include<stdint.h>
#include <errno.h>
#include <string.h>

#define ZIMAGE_MAGIC_OFFSET 0x24
#define ZIMAGE_START_OFFSET 0x28
#define ZIMAGE_END_OFFSET   0x2c



int main(int argc, char** argv){

	FILE* zImage = NULL;
	
	uint32_t buffer[2];
	uint32_t start_addr = 0;
	uint32_t end_addr = 0;
	uint32_t ret = 0;
	if(argc != 2){
		printf("Kernel zImage not supplied, exiting ...\n");
		return 1;
	}
	zImage = fopen(argv[1],"r");	
	if(zImage != NULL){
		// seek into zImage ....
		ret = fseek(zImage,ZIMAGE_START_OFFSET,0);
		if(ret == 0){
			// read address data ...
			ret = fread(buffer,4,2,zImage);
			if(ret == 2){
				printf("zImage start address is %#0x\n",buffer[0]);	
				printf("zImage end address is %#0x\n",buffer[1]);	
				printf("zImage length is %#0x\n",buffer[1] - buffer[0]);	
				if(zImage != NULL)
					fclose(zImage);	
				return 0;
			}else{
				printf("fread returned unexpected result: %d\n",ret);
				printf("error: %s\n",strerror(errno));
				if(zImage != NULL)
					fclose(zImage);	
				return errno;
			}	
		}else{
		if(zImage != NULL)
			fclose(zImage);	
		printf("error seeking file, fseek returned: %d\n",ret);
		return 1;
		}
	}else{
		printf("error opening file, exiting ...\n");
		printf("error: %s\n",strerror(errno));
		return errno;
	}

return 0;
}
