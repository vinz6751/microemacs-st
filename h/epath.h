/*	EPATH:	This file contains certain info needed to locate the
		MicroEMACS files on a system dependant basis.

									*/

/*	possible names and paths of help files under different OSs	*/

NOSHARE CONST char *pathname[] =

#if	TOS
{
	"emacs.rc",
	"emacs.hlp",
	"\\",
	"\\bin\\",
	"\\util\\",
	""
};
#endif
 

#if	V7 | BSD | FREEBSD | USG | AIX | SMOS | HPUX8 | HPUX9 | XENIX | SUN | AVIION
{
	".emacsrc",
	"emacs.hlp",
	"/usr/local/",
	"/usr/lib/",
	""
};
#endif


#define	NPNAMES	(sizeof(pathname)/sizeof(char *))
