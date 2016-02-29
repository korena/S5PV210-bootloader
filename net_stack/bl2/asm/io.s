/*
* these are globalized in io.h, because I will forget where they are in three days.
*/

.text
.code 32

.global __raw_writeb
.global __raw_writew
.global __raw_writel
.global __raw_readb
.global __raw_readw
.global __raw_readl

__raw_writeb:
	push	{lr}
	strb	r0,[r1]
	pop 	{pc}

__raw_writew:
	push	{lr}
	strh	r0,[r1]
	pop 	{pc}

__raw_writel:
	push	{lr}
	str	r0,[r1]
	pop 	{pc}

__raw_readb:
	push	{lr}
	ldrb	r0,[r0]
	pop 	{pc}

__raw_readw:
	push	{lr}
	ldrh	r0,[r0]
	pop 	{pc}

__raw_readl:
	push	{lr}
	ldr	r0,[r0]
	pop 	{pc}
