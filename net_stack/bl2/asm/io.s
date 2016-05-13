/*
* these are globalized in io.h, because I will forget where they are in three days.
*/

.text
.code 32

.global _raw_writeb
.global _raw_writehw
.global _raw_writel
.global _raw_readb
.global _raw_readhw
.global _raw_readl

_raw_writeb:
       push	{lr}
       strb	r0,[r1]
       pop 	{pc}

_raw_writehw:
       push	{lr}
       strh	r0,[r1]
       pop 	{pc}

_raw_writel:
       push	{lr}
       str	r0,[r1]
       pop 	{pc}

_raw_readb:
       push	{lr}
       ldrb	r0,[r0]
       pop 	{pc}

_raw_readhw:
       push	{lr}
       ldrh	r0,[r0]
       pop 	{pc}

_raw_readl:
	push	{lr}
	ldr	r0,[r0]
	pop 	{pc}
