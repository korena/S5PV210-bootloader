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

    strcpy(params->u.cmdline.cmdline,line); /* place commandline into tag */

    params = tag_next(params);              /* move pointer to next tag */
}

static void
setup_end_tag(void)
{
    params->hdr.tag = ATAG_NONE;            /* Empty tag ends list */
    params->hdr.size = 0;                   /* zero length */
}

#define MACH_SMDKV210 2456
#define DRAM_BASE 0x10000000
#define ZIMAGE_LOAD_ADDRESS DRAM_BASE + 0x8000
#define INITRD_LOAD_ADDRESS DRAM_BASE + 0x800000

static void
setup_tags(uint32_t *parameters)
{
    setup_core_tag(parameters, 4096);       /* standard core tag 4k pagesize */
    setup_mem_tag(DRAM_BASE, 0x4000000);    /* 64Mb at 0x10000000 */
    setup_mem_tag(DRAM_BASE + 0x8000000, 0x4000000); /* 64Mb at 0x18000000 */
    setup_ramdisk_tag(4096);                /* create 4Mb ramdisk */ 
    setup_initrd2_tag(INITRD_LOAD_ADDRESS, 0x100000); /* 1Mb of compressed data placed 8Mb into memory */
    setup_cmdline_tag("root=/dev/ram0");    /* commandline setting root device */
    setup_end_tag();                    /* end of tags */
}

int
start_linux(char *name,char *rdname)
{
    void (*theKernel)(int zero, int arch, uint32_t params);
    uint32_t exec_at = (uint32_t)-1;
    uint32_t parm_at = (uint32_t)-1;
    uint32_t machine_type;

    exec_at = ZIMAGE_LOAD_ADDRESS;
    parm_at = DRAM_BASE + 0x100;

    load_image(name, exec_at);              /* copy image into RAM */

    load_image(rdname, INITRD_LOAD_ADDRESS);/* copy initial ramdisk image into RAM */

    setup_tags((uint32_t*)parm_at);                    /* sets up parameters */

    machine_type = 2456;	              /* get machine type */

//    irq_shutdown();                         /* stop irq */

//    cpu_op(CPUOP_MMUCHANGE, NULL);          /* turn MMU off */

    theKernel = (void (*)(int, int, uint32_t))exec_at; /* set the kernel address */

    theKernel(0, machine_type, parm_at);    /* jump to kernel with register set */

    return 0;
}
