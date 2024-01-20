/*	TOS.C:		Operating specific I/O and Spawning functions
			for the ATARI ST operating system (GEMDOS)
			for MicroEMACS 4.00
			(C)Copyright 1995 by Daniel M. Lawrence
*/

#include        <stdio.h>
#include	<errno.h>
#include	"estruct.h"
#include	"eproto.h"
#if	TOS
#include        "edef.h"
#include	"elang.h"
#include	"osbind.h"
//#include	"stat.h"	/* DMABUFFER is here */
#include	"errno.h"

#define	EXEC		0x4b	/* Exec off a process */

/*
 * This function is called once to set up the terminal device streams.
 * On VMS, it translates TT until it finds the terminal, then assigns
 * a channel to it and sets it raw. On CPM it is a no-op.
 */
ttopen()
{
	/* on all screens we are not sure of the initial position
	   of the cursor					*/
	ttrow = 999;
	ttcol = 999;
}

/*
 * This function gets called just before we go back home to the command
 * interpreter. On VMS it puts the terminal back in a reasonable state.
 * Another no-operation on CPM.
 */
ttclose()
{
}

/*
 * Write a character to the display. On VMS, terminal output is buffered, and
 * we just put the characters in the big array, after checking for overflow.
 * On CPM terminal I/O unbuffered, so we just write the byte out. Ditto on
 * MS-DOS (use the very very raw console output routine).
 */
ttputc(c)

char c;

{
}

/*
 * Flush terminal buffer. Does real work where the terminal output is buffered
 * up. A no-operation on systems where byte at a time terminal I/O is done.
 */
ttflush()
{
}

/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all. More complex in VMS that almost anyplace else, which figures. Very
 * simple on CPM, because the system can do exactly what you want.
 */
ttgetc()
{
}

#if	TYPEAH
typahead()

{
	int rv;	/* return value from BIOS call */

	/* get the status of the console */
	rv = Bconstat(DEV_CONSOLE);

	/* end return the results */
	if (rv == 0)
		return(FALSE);
	else
		return(TRUE);
}
#endif

/*
 * Create a subjob with a copy of the command intrepreter in it. When the
 * command interpreter exits, mark the screen as garbage so that you do a full
 * repaint. Bound to "^X C". The message at the start in VMS puts out a newline.
 * Under some (unknown) condition, you don't get one free when DCL starts up.
 */
spawncli(f, n)

{
#if	ENVFUNC
	char *shell;	/* ptr to the name of the shell to execute */
#endif

	/* don't allow this command if restricted */
	if (restflag)
		return(resterr());

#if	ENVFUNC
	shell = getenv("SHELL");
	if (shell == (char *)NULL)
		return(FALSE);

	/* Prepare to Jump.... */
	mlerase();
        TTflush();
	TTkclose();

	/* Execute the shell */
	system(shell);

	/* Re-enable EMACS's environment, flag junked screen */
	TTkopen();
        sgarbf = TRUE;
        return(TRUE);
#else
	return(FALSE);
#endif
}

/*
 * Run a one-liner in a subjob. When the command returns, wait for a single
 * character to be typed, then mark the screen as garbage so a full repaint is
 * done. Bound to "C-X !".
 */
spawn(f, n)
{
        register int    s;
        char            line[NLINE];

	/* don't allow this command if restricted */
	if (restflag)
		return(resterr());

        if ((s=mlreply("!", line, NLINE)) != TRUE)
                return(s);
	mlerase();
	TTkclose();
        system(line);
	TTkopen();
	/* if we are interactive, pause here */
	if (clexec == FALSE) {
	        mlputs(TEXT6);
/*                     "\r\n\n[End]" */
        	tgetc();
        }
        sgarbf = TRUE;
        return (TRUE);
}

/*
 * Run an external program with arguments. When it returns, wait for a single
 * character to be typed, then mark the screen as garbage so a full repaint is
 * done. Bound to "C-X $".
 */

execprg(f, n)

{
        register int    s;
        char            line[NLINE];

	/* don't allow this command if restricted */
	if (restflag)
		return(resterr());

        if ((s=mlreply("!", line, NLINE)) != TRUE)
                return(s);
	mlerase();
	TTkclose();
        system(line);
	TTkopen();
	/* if we are interactive, pause here */
	if (clexec == FALSE) {
	        mlputs(TEXT6);
/*                     "\r\n\n[End]" */
        	tgetc();
        }
        sgarbf = TRUE;
        return (TRUE);
}

/*
 * Pipe a one line command into a window
 * Bound to ^X @
 */
pipecmd(f, n)
{
        register int    s;	/* return status from CLI */
	register EWINDOW *wp;	/* pointer to new window */
	register BUFFER *bp;	/* pointer to buffer to zot */
        char	line[NLINE];	/* command line send to shell */
	static char bname[] = "command";
	FILE *fp;

	static char filnam[NSTRING] = "command";

	/* don't allow this command if restricted */
	if (restflag)
		return(resterr());

	/* get the command to pipe in */
        if ((s=mlreply("@", line, NLINE)) != TRUE)
                return(s);

	/* get rid of the command output buffer if it exists */
        if ((bp=bfind(bname, FALSE, 0)) != (BUFFER *)NULL) {
		/* try to make sure we are off screen */
		wp = wheadp;
		while (wp != NULL) {
			if (wp->w_bufp == bp) {
				onlywind(FALSE, 1);
				break;
			}
			wp = wp->w_wndp;
		}
		if (zotbuf(bp) != TRUE)

			return(FALSE);
	}

	strcat(line," >>");
	strcat(line,filnam);
	movecursor(term.t_nrow - 1, 0);
	TTkclose();
	system(line);
	TTkopen();
        sgarbf = TRUE;
	if ((fp = fopen(filnam, "r")) == NULL) {
		s = FALSE;
	} else {
		fclose(fp);
		s = TRUE;
	}

	if (s != TRUE)
		return(s);

	/* split the current window to make room for the command output */
	if (splitwind(FALSE, 1) == FALSE)
			return(FALSE);

	/* and read the stuff in */
	if (getfile(filnam, FALSE) == FALSE)
		return(FALSE);

	/* make this window in VIEW mode, update all mode lines */
	curwp->w_bufp->b_mode |= MD_READ_ONLY;
	upmode();

	/* and get rid of the temporary file */
	unlink(filnam);
	return(TRUE);
}

/*
 * filter a buffer through an external DOS program
 * Bound to ^X #
 */
filter(f, n)

{
        register int    s;	/* return status from CLI */
	register BUFFER *bp;	/* pointer to buffer to zot */
        char line[NLINE];	/* command line send to shell */
	char tmpnam[NFILEN];	/* place to store real file name */
	static char bname1[] = "fltinp";

	static char filnam1[] = "fltinp";
	static char filnam2[] = "fltout";

	/* don't allow this command if restricted */
	if (restflag)
		return(resterr());

	if (curbp->b_mode&MD_READ_ONLY)	/* don't allow this command if	*/
		return(rdonly());	/* we are in read only mode	*/

	/* get the filter name and its args */
        if ((s=mlreply("#", line, NLINE)) != TRUE)
                return(s);

	/* setup the proper file names */
	bp = curbp;
	strcpy(tmpnam, bp->b_fname);	/* save the original name */
	strcpy(bp->b_fname, bname1);	/* set it to our new one */

	/* write it out, checking for errors */
	if (writeout(filnam1, "w") != TRUE) {
		mlwrite(TEXT2);
/*                      "[Cannot write filter file]" */
		strcpy(bp->b_fname, tmpnam);
		return(FALSE);
	}

	strcat(line," <fltinp >fltout");
	movecursor(term.t_nrow, 0);
	TTkclose();
	system(line);
	TTkopen();
        sgarbf = TRUE;
	s = TRUE;

	/* on failure, escape gracefully */
	if (s != TRUE || (readin(filnam2,FALSE) == FALSE)) {
		mlwrite(TEXT3);
/*                      "[Execution failed]" */
		strcpy(bp->b_fname, tmpnam);
		unlink(filnam1);
		unlink(filnam2);
		return(s);
	}

	/* reset file name */
	strcpy(bp->b_fname, tmpnam);	/* restore name */
	bp->b_flag |= BFCHG;		/* flag it as changed */
	upmode();

	/* and get rid of the temporary file */
	unlink(filnam1);
	unlink(filnam2);
	return(TRUE);
}


/* return a system dependant string with the current time */

char *timeset()

{
	register char *sp;	/* temp string pointer */
	char buf[16];		/* time data buffer */

	time(buf);
	sp = (char*)ctime(buf);
	sp[strlen(sp)-1] = 0;
	return(sp);
}

/*	FILE Directory routines		*/

static _DTA info;		/* Info about the file */
char xpath[NFILEN];		/* path of file to find */
char rbuf[NFILEN];		/* return file buffer */

/*	do a wild card directory search (for file name completion) */

char *getffile(fspec)

char *fspec;	/* file to match */

{
	register int index;		/* index into various strings */
	register int point;		/* index into other strings */
	register int extflag;		/* does the file have an extention? */
	char fname[NFILEN];		/* file/path for DOS call */

	/* first parse the file path off the file spec */
	strcpy(xpath, fspec);
	index = strlen(xpath) - 1;
	while (index >= 0 && (xpath[index] != '/' &&
				xpath[index] != '\\' && xpath[index] != ':'))
		--index;
	xpath[index+1] = 0;

	/* check for an extension */
	point = strlen(fspec) - 1;
	extflag = FALSE;
	while (point >= index) {
		if (fspec[point] == '.') {
			extflag = TRUE;
			break;
		}
		point--;
	}

	/* construct the composite wild card spec */
	strcpy(fname, xpath);
	strcat(fname, &fspec[index+1]);
	strcat(fname, "*");
	if (extflag == FALSE)
		strcat(fname, ".*");

	/* and call for the first file */
	Fsetdta(&info);		/* Initialize buffer for our search */
	if (Fsfirst(fname, 0xF7))
		return(NULL);

	/* return the first file name! */
	strcpy(rbuf, xpath);
	strcat(rbuf, info.dta_name);
	mklower(rbuf);
	if (info.dta_attribute & 0x10)
		strcat(rbuf, DIRSEPSTR);
	return(rbuf);
}

char *getnfile()

{

	/* and call for the first file */
	if (Fsnext())
		return(NULL);

	/* return the first file name! */
	strcpy(rbuf, xpath);
	strcat(rbuf, info.dta_name);
	mklower(rbuf);
	if (info.dta_attribute & 0x10)
		strcat(rbuf, DIRSEPSTR);
	return(rbuf);
}

#if	LATTICE
system(cmd)	/* call the system to execute a new program */

char *cmd;	/* command to execute */

{
	char *pptr;			/* pointer into program name */
	char pname[NSTRING];		/* name of program to execute */
	char tail[NSTRING];		/* command tail */

	/* scan off program name.... */
	pptr = pname;
	while (*cmd && (*cmd != ' ' && *cmd != '\t'))
		*pptr++ = *cmd++;
	*pptr = 0;

	/* create program name length/string */
	tail[0] = strlen(cmd);
	strcpy(&tail[1], cmd);

	/* go do it! */
	return(gemdos(		(int)EXEC,
				(int)0,
				(char *)pname,
				(char *)tail,
				(char *)NULL));
}
#elif	GCC

system(cmd)	/* call the system to execute a new program */
const char *cmd;	/* command to execute */
{
	char *pptr;			/* pointer into program name */
	char pname[NSTRING];		/* name of program to execute */
	char tail[NSTRING];		/* command tail */
	char *path;

	/* scan off program name.... */
	pptr = pname;
	while (*cmd && (*cmd != ' ' && *cmd != '\t'))
		*pptr++ = *cmd++;
	*pptr = 0;

	/* Check if it's an absolute path */
	pptr = (char*)cmd;
	if (strlen(cmd) > 2 && (cmd[1] == ':' || cmd[0] == DIRSEPCHAR)) {
		/* Do nothing */
	}
	else {
#if ENVFUNC
		if (ffropen(pname) == FIOSUC) {
			ffclose();
		}
		else {
			// Try find the program in the PATH
			char *sp;	/* pointer into path spec */
			int i; 	/* index */
			static char fspec[NFILEN];	/* full path spec to search */
			static char path_comp[NFILEN];	/* one of the paths in the PATH environment variable */

			path = getenv("PATH");
			if (path != NULL) {
				while (*path) {
					char *start_of_path_component = path;
					int path_len;
					while (*path && *path != PATHCHR)
						path++;
				
					path_len = path - start_of_path_component;
					if (path_len == 0)
						goto next_path_component;

					strncpy(path_comp, start_of_path_component, path_len);
					
					/* ensure path ends with directory separator */
					if (path_comp[path_len-1] != DIRSEPCHAR)
						path_comp[path_len++] = DIRSEPCHAR;
					
					path_comp[path_len] = '\0'; /* force end of string */

					const char *exts[] = { "", ".TTP", ".TOS", ".PRG", ".APP", ".GTP" };
					int i,found = 0;
					for (i=0; !found && i<5; i++) {
						strcpy(fspec,path_comp);
						strcat(fspec,pname); /* TODO: check for length */
						strcat(fspec,exts[i]);
						found = (ffropen(fspec) == FIOSUC);
					}
					if (found) {
						ffclose();
						strcpy(pname,fspec); // TODO: Should check for lengths...
						break;
					}
next_path_component:
					/* next path component */
					if (*path == PATHCHR)
						++path;
				}
			}
		}
#endif
	}

	/* create program name length/string */
	tail[0] = strlen(cmd);
	strcpy(&tail[1], cmd);

	/* go do it! */
	return(Pexec(
				(int)0,
				(char *)pname,
				(char *)tail,
				(char *)NULL));
}

#endif

#else
TOShello()
{
}
#endif
