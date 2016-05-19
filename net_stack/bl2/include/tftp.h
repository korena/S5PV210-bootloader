/*
 *	LiMon - BOOTP/TFTP.
 *
 *	Copyright 1994, 1995, 2000 Neil Russell.
 *	Copyright 2011 Comelit Group SpA
 *	               Luca Ceresoli <luca.ceresoli@comelit.it>
 *	(See License)
 */

#ifndef __TFTP_H__
#define __TFTP_H__

/**********************************************************************/
/*
 *	Global functions and variables.
 */

/* tftp.c */
void tftp_start(enum proto_t protocol);	/* Begin TFTP get/put */
extern unsigned long tftp_timeout_ms;
extern int tftp_timeout_count_max;

/**********************************************************************/

#endif /* __TFTP_H__ */
