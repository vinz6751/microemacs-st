|	stmouse.s, a handler that preserves A0 across the call, and calls
|	two mouse routines.
|
|	31-Dec-1987	Jeff Lomicka
|	10-Feb-2021 	Vincent Barrilliot Updated for GCC 4.6.4
|
|	mymouse( hdr1, hdr2) calls both handlers with A0 and stack parameter
|
	.global	_mymouse
	.global	_first_mhandler
	.global	_mousevec_tos

	| XBRA header
	.dc.l   0x58425241 | 'XBRA'
	.dc.l	0x55454d58 | 'UEMX', short for MicroEmacs
_mousevec_tos:
	.dc.l	0
_mymouse:				| Replacement mouse handler
	move.l	a0,-(sp)		| Save value for second handler
	move.l	a0,-(sp)		| Pass to first handler
	movea.l	_first_mhandler,a1	| first_mhandler(a0)
	jsr	(a1)			| Call first handler
	addq.l	#4,a7			| Recover stack
	movea.l	(sp),a0			| Recover A0
	movea.l	_mousevec_tos,a1	| Pick up second handler
	jsr	(a1)
	addq.l	#4,a7			| Recover stack
	rts
