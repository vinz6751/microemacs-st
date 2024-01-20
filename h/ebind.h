/*	EBIND:		Initial default key to function bindings for
			MicroEMACS 4.00
*/

/*
 * Command table.
 * This table  is *roughly* in ASCII order, left to right across the
 * characters of the command. It is being searched sequentially for
 * keystrokes and the &bind function. Thus I am placing more frequent
 * keys at the top . . .
 */
 KEYTAB	keytab[NBINDS] = {

#if	MOUSE
	{MOUS|'m',		BINDFNC,	mmove},
	{MOUS|CTRL|'m',		BINDFNC,	mmove},
	{MOUS|SHFT|'m',		BINDFNC,	mmove},
	{MOUS|CTRL|SHFT|'m',	BINDFNC,	mmove},
	{MOUS|'a',		BINDFNC,	movemd},
	{MOUS|'b',		BINDFNC,	movemu},
	{MOUS|'e',		BINDFNC,	mregdown},
	{MOUS|'f',		BINDFNC,	mregup},
	{MOUS|'1',		BINDFNC,	resizm},
#endif 

	{CTRL|'A',		BINDFNC,	goto_begining_of_line},
	{CTRL|'B',		BINDFNC,	back_char},
	{CTRL|'C',		BINDFNC,	insspace},
	{CTRL|'D',		BINDFNC,	forwdel},
	{CTRL|'E',		BINDFNC,	goto_end_of_line},
	{CTRL|'F',		BINDFNC,	forward_char},
	{CTRL|'G',		BINDFNC,	ctrlg},
	{CTRL|'H',		BINDFNC,	backdel},
	{CTRL|'I',		BINDFNC,	tab},
	{CTRL|'J',		BINDFNC,	indent},
	{CTRL|'K',		BINDFNC,	killtext},
	{CTRL|'L',		BINDFNC,	refresh},
	{CTRL|'M',		BINDFNC,	newline},
	{CTRL|'N',		BINDFNC,	forward_line},
	{CTRL|'O',		BINDFNC,	openline},
	{CTRL|'P',		BINDFNC,	backward_line},
	{CTRL|'Q',		BINDFNC,	quote},
	{CTRL|'R',		BINDFNC,	backsearch},
	{CTRL|'S',		BINDFNC,	forwsearch},
	{CTRL|'T',		BINDFNC,	twiddle},
	{CTRL|'U',		BINDFNC,	unarg},
	{CTRL|'V',		BINDFNC,	forward_page},
	{CTRL|'W',		BINDFNC,	killregion},
	{CTRL|'X',		BINDFNC,	cex},
	{CTRL|'Y',		BINDFNC,	yank},
	{CTRL|'Z',		BINDFNC,	backward_page},
	{CTRL|'[',		BINDFNC,	meta},
	{CTRL|'\\',		BINDFNC,	forwsearch},
	{CTRL|'^',		BINDFNC,	quote},
	{CTRL|'_',		BINDFNC,	undo},
	{CTLX|CTRL|'A',		BINDFNC,	file_append},
	{CTLX|CTRL|'B', 	BINDFNC,	listbuffers},
	{CTLX|CTRL|'C', 	BINDFNC,	quit},
	{CTLX|CTRL|'D', 	BINDFNC,	detab},
	{CTLX|CTRL|'E', 	BINDFNC,	entab},
	{CTLX|CTRL|'F', 	BINDFNC,	find_file},
	{CTLX|CTRL|'I', 	BINDFNC,	insert_file},
	{CTLX|CTRL|'K',		BINDFNC,	macrotokey},
	{CTLX|CTRL|'L', 	BINDFNC,	lowerregion},
	{CTLX|CTRL|'M', 	BINDFNC,	delmode},
	{CTLX|CTRL|'N', 	BINDFNC,	mvdnwind},
	{CTLX|CTRL|'O', 	BINDFNC,	deblank},
	{CTLX|CTRL|'P', 	BINDFNC,	mvupwind},
	{CTLX|CTRL|'R', 	BINDFNC,	file_read},
	{CTLX|CTRL|'S', 	BINDFNC,	filesave},
	{CTLX|CTRL|'T', 	BINDFNC,	trim},
	{CTLX|CTRL|'U', 	BINDFNC,	upperregion},
	{CTLX|CTRL|'V', 	BINDFNC,	view_file},
	{CTLX|CTRL|'W', 	BINDFNC,	file_write},
	{CTLX|CTRL|'X', 	BINDFNC,	swap_mark},
	{CTLX|CTRL|'Z', 	BINDFNC,	shrinkwind},
	{CTLX|CTRL|'\\', 	BINDFNC,	filesave},
	{CTLX|'?',		BINDFNC,	deskey},
	{CTLX|'!',		BINDFNC,	spawn},
	{CTLX|'@',		BINDFNC,	pipecmd},
	{CTLX|'#',		BINDFNC,	filter},
	{CTLX|'$',		BINDFNC,	execprg},
	{CTLX|'=',		BINDFNC,	showcpos},
	{CTLX|'(',		BINDFNC,	ctlxlp},
	{CTLX|')',		BINDFNC,	ctlxrp},
	{CTLX|'<',		BINDFNC,	narrow},
	{CTLX|'>',		BINDFNC,	widen},
	{CTLX|'^',		BINDFNC,	enlargewind},
	{CTLX|' ',		BINDFNC,	remove_mark},
	{CTLX|'0',		BINDFNC,	delwind},
	{CTLX|'1',		BINDFNC,	onlywind},
	{CTLX|'2',		BINDFNC,	splitwind},
	{CTLX|'A',		BINDFNC,	setvar},
	{CTLX|'B',		BINDFNC,	usebuffer},
	{CTLX|'C',		BINDFNC,	spawncli},
#if	BSD || FREEBSD || VMS || SUN || HPUX8 || HPUX9 || AVIION
	{CTLX|'D',		BINDFNC,	bktoshell},
#endif
	{CTLX|'E',		BINDFNC,	ctlxe},
	{CTLX|'G',		BINDFNC,	dispvar},
	{CTLX|'K',		BINDFNC,	killbuffer},
	{CTLX|'M',		BINDFNC,	setmod},
	{CTLX|'N',		BINDFNC,	filename},
	{CTLX|'O',		BINDFNC,	nextwind},
	{CTLX|'P',		BINDFNC,	prevwind},
#if	ISRCH
	{CTLX|'R',		BINDFNC,	risearch},
	{CTLX|'S',		BINDFNC,	fisearch},
#endif
	{CTLX|'U',		BINDFNC,	undo_list},
	{CTLX|'W',		BINDFNC,	resize},
	{CTLX|'X',		BINDFNC,	nextbuffer},
	{CTLX|'Y',		BINDFNC,	cycle_ring},
	{CTLX|'Z',		BINDFNC,	enlargewind},
	{META|CTRL|'C', 	BINDFNC,	wordcount},
	{META|CTRL|'E', 	BINDFNC,	execproc},
	{META|CTRL|'F', 	BINDFNC,	getfence},
	{META|CTRL|'G', 	BINDFNC,	goto_mark},
	{META|CTRL|'H', 	BINDFNC,	delbword},
	{META|CTRL|'K', 	BINDFNC,	unbindkey},
	{META|CTRL|'L', 	BINDFNC,	reposition},
	{META|CTRL|'M', 	BINDFNC,	delgmode},
	{META|CTRL|'N', 	BINDFNC,	namebuffer},
	{META|CTRL|'R', 	BINDFNC,	qreplace},
	{META|CTRL|'S', 	BINDFNC,	execfile},
	{META|CTRL|'V', 	BINDFNC,	nextdown},
	{META|CTRL|'U', 	BINDFNC,	undo_delete},
	{META|CTRL|'W', 	BINDFNC,	killpara},
	{META|CTRL|'X', 	BINDFNC,	execcmd},
	{META|CTRL|'Y',		BINDFNC,	clear_ring},
	{META|CTRL|'Z', 	BINDFNC,	nextup},
	{META|CTRL|'\\', 	BINDFNC,	execfile},
	{META|' ',		BINDFNC,	set_mark},
	{META|'?',		BINDFNC,	help},
	{META|'!',		BINDFNC,	reposition},
	{META|')',		BINDFNC,	indent_region},
	{META|'(',		BINDFNC,	undent_region},
	{META|'.',		BINDFNC,	set_mark},
	{META|'>',		BINDFNC,	goto_end_of_buffer},
	{META|'<',		BINDFNC,	goto_begining_of_buffer},
	{META|'~',		BINDFNC,	unmark},
	{META|'A',		BINDFNC,	apro},
	{META|'B',		BINDFNC,	backword},
	{META|'C',		BINDFNC,	capword},
	{META|'D',		BINDFNC,	delfword},
#if	CRYPT
	{META|'E',		BINDFNC,	setekey},
#endif
	{META|'F',		BINDFNC,	forwword},
	{META|'G',		BINDFNC,	goto_line},
	{META|'K',		BINDFNC,	bindtokey},
	{META|'L',		BINDFNC,	lowerword},
	{META|'M',		BINDFNC,	setgmode},
	{META|'N',		BINDFNC,	goto_end_of_paragraph},
	{META|'P',		BINDFNC,	goto_begining__of_paragraph},
	{META|'Q',		BINDFNC,	fillpara},
	{META|'R',		BINDFNC,	sreplace},
#if	BSD || FREEBSD || HPUX8 || HPUX9 || VMS || SUN || AVIION
	{META|'S',		BINDFNC,	bktoshell},
#endif
	{META|'U',		BINDFNC,	upperword},
	{META|'V',		BINDFNC,	backward_page},
	{META|'W',		BINDFNC,	copyregion},
	{META|'X',		BINDFNC,	namedcmd},
	{META|'Y',		BINDFNC,	yank_pop},
	{META|'Z',		BINDFNC,	quickexit},
/*	{META|CTRL|'?',		BINDFNC,	delbword}, ??? */

	{ALTD|'B',		BINDFNC,	list_screens},
	{ALTD|'C',		BINDFNC,	cycle_screens},
	{ALTD|'D',		BINDFNC,	delete_screen},
	{ALTD|'F',		BINDFNC,	find_screen},
	{ALTD|'N',		BINDFNC,	rename_screen},
	{ALTD|'R',		BINDFNC,	backhunt},
	{ALTD|'S',		BINDFNC,	forwhunt},

	{SPEC|'<',		BINDFNC,	goto_begining_of_buffer},	/* Home */
	{SPEC|'P',		BINDFNC,	backward_line},	/* up */
	{SPEC|'Z',		BINDFNC,	backward_page},	/* PgUp */
	{SPEC|'B',		BINDFNC,	back_char},	/* left */

	{SPEC|'L',		BINDFNC,	reposition},	/* NP5 */

	{SPEC|'F',		BINDFNC,	forward_char},	/* right */
	{SPEC|'>',		BINDFNC,	goto_end_of_buffer},	/* End */
	{SPEC|'N',		BINDFNC,	forward_line},	/* down */
	{SPEC|'V',		BINDFNC,	forward_page},	/* PgDn */
	{SPEC|'C',		BINDFNC,	insspace},	/* Ins */
	{SPEC|'D',		BINDFNC,	forwdel},	/* Del */

	{SPEC|CTRL|'B', 	BINDFNC,	backword},	/* ctrl left */
	{SPEC|CTRL|'F', 	BINDFNC,	forwword},	/* ctrl right */
	{SPEC|CTRL|'Z', 	BINDFNC,	goto_begining__of_paragraph},	/* ctrl PgUp */
	{SPEC|CTRL|'V', 	BINDFNC,	goto_end_of_paragraph},	/* ctrl PgDn */

#if ATKBD | OS2

/* The following keys are some of the extra ones that can be returned by an
   AT-style keyboard.  By default we bind them to the same functions as the
   equivalent PC keys.  However they, and the ones not mentioned here, are
   available to the user for rebinding to functions of choice.

   See keyboard.c for a full list of available keys.
*/
	{SPEC|'a',		BINDFNC,	goto_begining_of_buffer},	/* grey Home */
	{SPEC|'b',		BINDFNC,	backward_line},	/* grey up */
	{SPEC|'c',		BINDFNC,	backward_page},	/* grey PgUp */
	{SPEC|'d',		BINDFNC,	back_char},	/* grey left */
	{SPEC|'e',		BINDFNC,	reposition},	/* grey center */
	{SPEC|'f',		BINDFNC,	forward_char},	/* grey right */
	{SPEC|'g',		BINDFNC,	goto_end_of_buffer},	/* grey End */
	{SPEC|'h',		BINDFNC,	forward_line},	/* grey down */
	{SPEC|'i',		BINDFNC,	forward_page},	/* grey PgDn */
	{SPEC|'j',		BINDFNC,	insspace},	/* grey Ins */
	{SPEC|'k',		BINDFNC,	forwdel},	/* grey Del */
	{SPEC|':',		BINDFNC,	execcmd},	/* pause */

	{SPEC|CTRL|'c',		BINDFNC,	goto_begining__of_paragraph},	/* ctrl grey PgUp */
	{SPEC|CTRL|'d',		BINDFNC,	backword},	/* ctrl grey left */
	{SPEC|CTRL|'f',		BINDFNC,	forwword},	/* ctrl grey right */
	{SPEC|CTRL|'i',		BINDFNC,	goto_end_of_paragraph},	/* ctrl grey PgDn */

#endif

#if	TOS
	{SPEC|CTRL|'5',		BINDFNC,	help},
#endif
 
	{CTRL|'?',		BINDFNC,	backdel},

	{0,			BINDNUL,	NULL}
};
