/*
Adapted from http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html 
*/

#include<stdint.h>
#include<string.h>

/* list of possible tags */
#define ATAG_NONE       0x00000000
#define ATAG_CORE       0x54410001
#define ATAG_MEM        0x54410002
#define ATAG_VIDEOTEXT  0x54410003
#define ATAG_RAMDISK    0x54410004
#define ATAG_INITRD2    0x54420005
#define ATAG_SERIAL     0x54410006
#define ATAG_REVISION   0x54410007
#define ATAG_VIDEOLFB   0x54410008
#define ATAG_CMDLINE    0x54410009



typedef uint32_t (*copy_mmc_to_mem)(uint32_t  channel, uint32_t  start_block, uint16_t block_size,
		                                            uint32_t  *target, uint32_t  init);


/* structures for each atag */
typedef struct atag_header {
        uint32_t size; /* length of tag in words including this header */
        uint32_t tag;  /* tag type */
} header_tag;

struct atag_core {
        uint32_t flags;
        uint32_t pagesize;
        uint32_t rootdev;
};

struct atag_mem {
        uint32_t     size;
        uint32_t     start;
};

struct atag_videotext {
        uint8_t              x;
        uint8_t              y;
        uint16_t             video_page;
        uint8_t              video_mode;
        uint8_t              video_cols;
        uint16_t             video_ega_bx;
        uint8_t              video_lines;
        uint8_t              video_isvga;
        uint16_t             video_points;
};

struct atag_ramdisk {
        uint32_t flags;
        uint32_t size;
        uint32_t start;
};

struct atag_initrd2 {
        uint32_t start;
        uint32_t size;
};

struct atag_serialnr {
        uint32_t low;
        uint32_t high;
};

struct atag_revision {
        uint32_t rev;
};

struct atag_videolfb {
        uint16_t             lfb_width;
        uint16_t             lfb_height;
        uint16_t             lfb_depth;
        uint16_t             lfb_linelength;
        uint32_t             lfb_base;
        uint32_t             lfb_size;
        uint8_t              red_size;
        uint8_t              red_pos;
        uint8_t              green_size;
        uint8_t              green_pos;
        uint8_t              blue_size;
        uint8_t              blue_pos;
        uint8_t              rsvd_size;
        uint8_t              rsvd_pos;
};

struct atag_cmdline {
        char    cmdline[1];
};

struct atag {
        struct atag_header hdr;
        union {
                struct atag_core         core;
                struct atag_mem          mem;
                struct atag_videotext    videotext;
                struct atag_ramdisk      ramdisk;
                struct atag_initrd2      initrd2;
                struct atag_serialnr     serialnr;
                struct atag_revision     revision;
                struct atag_videolfb     videolfb;
                struct atag_cmdline      cmdline;
        } u;
};


#define tag_next(t)     ((struct atag *)((uint32_t *)(t) + (t)->hdr.size))
#define tag_size(type)  ((sizeof(header_tag) + sizeof(struct type)) >> 2)
static struct atag *params; /* used to point at the current tag */


static void debug_print(char*str){
	uart_print(str);
}
static void uart_print_address_contents(uint32_t *address){
	uart_print_hex(*address);
}

char * because_ive_had_it_with_gcc_linker_strcpy(char *strDest, const char *strSrc)
{
	   // assert(strDest!=NULL && strSrc!=NULL);  // NO.
	    char *temp = strDest;
	    while(*strDest++ = *strSrc++); // or while((*strDest++=*strSrc++) != '\0');
	    return temp;
}



static void
setup_core_tag(void * address,long pagesize)
{
    params = (struct atag *)address;         /* Initialise parameters to start at given address */

    params->hdr.tag = ATAG_CORE;            /* start with the core tag */
    params->hdr.size = tag_size(atag_core); /* size the tag */

    params->u.core.flags = 1;               /* ensure read-only */
    params->u.core.pagesize = pagesize;     /* systems pagesize (4k) */
    params->u.core.rootdev = 0;             /* zero root device (typicaly overidden from commandline )*/

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_ramdisk_tag(uint32_t size)
{
    params->hdr.tag = ATAG_RAMDISK;         /* Ramdisk tag */
    params->hdr.size = tag_size(atag_ramdisk);  /* size tag */

    params->u.ramdisk.flags = 0;            /* Load the ramdisk */
    params->u.ramdisk.size = size;          /* Decompressed ramdisk size */
    params->u.ramdisk.start = 0;            /* Unused */

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_initrd2_tag(uint32_t start, uint32_t size)
{
    params->hdr.tag = ATAG_INITRD2;         /* Initrd2 tag */
    params->hdr.size = tag_size(atag_initrd2);  /* size tag */

    params->u.initrd2.start = start;        /* physical start */
    params->u.initrd2.size = size;          /* compressed ramdisk size */

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_mem_tag(uint32_t start, uint32_t len)
{
    params->hdr.tag = ATAG_MEM;             /* Memory tag */
    params->hdr.size = tag_size(atag_mem);  /* size tag */

    params->u.mem.start = start;            /* Start of memory area (physical address) */
    params->u.mem.size = len;               /* Length of area */

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_cmdline_tag(const char * line)
{
    int linelen = strlen(line);

    if(!linelen)
        return;                             /* do not insert a tag for an empty commandline */

    params->hdr.tag = ATAG_CMDLINE;         /* Commandline tag */
    params->hdr.size = (sizeof(struct atag_header) + linelen + 1 + 4) >> 2;

    because_ive_had_it_with_gcc_linker_strcpy(params->u.cmdline.cmdline,line); /* place commandline into tag */

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_end_tag(void)
{
    params->hdr.tag = ATAG_NONE;            /* Empty tag ends list */
    params->hdr.size = 0;                   /* zero length */
}

#define MACH_SMDKV210 2456
#define DRAM_BASE 0x20000000
#define ZIMAGE_LOAD_ADDRESS DRAM_BASE + 0x8000  // 32k away from the base address of DRAM
#define ZIMAGE_LOAD_END_ADDRESS 1516384 // TODO: no good, find a better way.
#define INITRD_LOAD_ADDRESS DRAM_BASE + 0x800000
#define INITRD_LOAD_END_ADDRESS 4096  // size of the ramdisk 
static void
setup_tags(uint32_t *parameters)
{
    setup_core_tag(parameters, 4096);       /* standard core tag 4k pagesize */
    setup_mem_tag(DRAM_BASE, 0x1FFFFFFF);    /* 512MB at 0x20000000, only DMC0 is connected in Tiny210 board */
    setup_ramdisk_tag(8192);                /* create 8Mb ramdisk */ 
    setup_initrd2_tag(INITRD_LOAD_ADDRESS, 0x100000); /* 1Mb of compressed data placed 8Mb into memory */
    setup_cmdline_tag("root=/dev/ram0");    /* commandline setting root device */
    setup_end_tag();                    /* end of tags */
}

#define PRNSTS0 0xEB000024   // present state register sd controller ..
#define ARGUMENT0 0xEB000008
#define CMDREG0 0xEB00000E
#define CMD_BIT_MASK 0x00000001 
#define DAT_BITS_MASK 0x00F00000 

static void load_image(uint32_t *dest_address,uint32_t size_in_bytes){
        uint32_t ret = 0;	
//	uart_print_address_contents((uint32_t*)0xEB00002C);// CLKCON0
//	uart_print_address_contents((uint32_t*)(PRNSTS0)) ; // HOSTCTL0

	while((*(uint32_t*)(PRNSTS0)) & CMD_BIT_MASK != CMD_BIT_MASK){
		debug_print("waiting for CMD bit to clear .... \n\r\0");
	}
	while((*(uint32_t*)(PRNSTS0)) & DAT_BIT_MASK != DAT_BIT_MASK){
		// needed ???
		debug_print("waiting for busy lines to clear.... \n\r\0");
	}
//########################## untested code 



//######################### end of untested code	


	if(ret == 1){
		debug_print("copying successful ...\n\r\0");
	}else{
		debug_print("copying failed :-(\n\r\0");
	}
}
int
start_linux(void)
{
    void (*theKernel)(uint32_t zero, uint32_t arch, uint32_t *params);
    uint32_t i = 0;
    uint32_t *exec_at = (uint32_t *) ZIMAGE_LOAD_ADDRESS;
    uint32_t *parm_at = (uint32_t *) DRAM_BASE + 0x100 ;  // 256 bytes away from the base address of DRAM
    uint32_t machine_type;

    debug_print("about to copy linux image ...\n\r\0");

    load_image((uint32_t*) ZIMAGE_LOAD_ADDRESS, ZIMAGE_LOAD_END_ADDRESS);    /* copy image into RAM */

    debug_print("done copying linux image ...\n\r\0");

//    debug_print("about to copy ramdisk image ...");

//    load_image((uint32_t*) INITRD_LOAD_ADDRESS, INITRD_LOAD_END_ADDRESS);/* copy initial ramdisk image into RAM */

//    debug_print("done copying ramdisk image ...");

    debug_print("setting up ATAGS ...\n\r\0");

    setup_tags(parm_at);                    /* sets up parameters */

    //TODO: you should place the atags_list at param_at !! How else would you expect them to be there !?

    machine_type = 2456;	              /* get machine type */

    theKernel = (void (*)(uint32_t, uint32_t, uint32_t*))exec_at; /* set the kernel address */

    
//    for(i;i<20;i++){
//	    uart_print_address_contents((uint32_t*)exec_at+i);
//    }

//    debug_print("jumping to the kernel ... brace yourself!\n\r\0");
   
    // TODO: for debuggin, read the address at exec_at, and see if it contains the first instruciton of the zImage

while(1);
    // theKernel(0, machine_type, parm_at);    /* jump to kernel with register set */

    return 0;
}
