/*	ST52.C:	Atari ST1040/520 screen support functions
/*	ST52.C:	Atari ST1040/520 screen support functions
		written by lots of people (see below)
	tabsize=8

		Daniel Lawrence
		James Turner
		Jeff Lomicka
		J. C. Benoist
		Vincent Barrilliot

	Modification History:
	31-dec-87	Jeff Lomicka
	- massive changes/additions for accessories and mouse
	20-jan-87	Daniel Lawrence
	- changed code in domousekey() to pass five bytes, two zeros
	  (to tell input.c that this is a mouse event), x/ypos
	  and the mouse event type.
	may 88		Jeff Lomicka and Dan Lawrence
	- a lot of changes.  Through out aline.h, use the MWC and
	  ATARI names for things now.
	- moving the mouse to the extreme upper left brings up
	  the desk accessory menu.  EMACS won't replot on the
	  way out yet.
	- cleaned up behavior of the mouse and the cursor on exit
	26-feb-89	Daniel Lawrence
	- rewote input layer to conform to new specs in input.c
	01-may-91	Daniel Lawrence/Suggested by Allan Pratt
	- renamed atari source files to TOS.C and ST52.c
	- changed symbols ATARI => ST52 and ST520 => TOS
	22-feb-21	Vincent Barrilliot modernized for GCC 4.6.4,
			GEMlib and libcmini
*/

#define termdef 1		/* don't define "term" external */

/* These routines provide support for the ATARI 1040ST and 520ST using the virtual VT52 Emulator */
/* This is for GCC 4.6.4 MiNTlib and GEMlib */
#include	<gem.h>
#include	<osbind.h>
#include	<stdio.h>
#include	<stdint.h>
#include	"estruct.h"
#include	"eproto.h"
#include	"edef.h"
#include	"elang.h"
#define __MFDB__ /* Prevent collision between gem and osbind declaration of the MFDB type */
#include	<mint/linea.h>


#if	ST52

#define NROW	50	/* Screen size. 		*/
#define NCOL	80	/* Edit if you want to. 	*/
#define MARGIN	8	/* size of minimim margin and	*/
#define SCRSIZ	64	/* scroll size for extended lines */
#define NPAUSE	300	/* # times thru update to pause */
#define BIAS	0x20	/* Origin 0 coordinate bias.	*/
#define ESC	'\033'	/* ESC character.		*/
#define ESCS	"\033"	/* ESC character string.	*/
#define SCRFONT 2	/* index of 8x16 monochrome system default font */
#define DENSIZE 50	/* # of lines in a dense screen */


/*	Palette color definitions	*/

#define LOWPAL	"000700070770007707077777333733373773337737377777"
#define MEDPAL	"000700007777"
#define HIGHPAL "111000"


/*	ST Global definitions		*/

static uint16_t initrez;		/* initial screen resolution */
static uint16_t currez;			/* current screen resolution */
static uint16_t gemflag;		/* were we called from the desktop? */
static uint16_t mouseon;		/* True if mouse is on */
static char  resolution_name[][8] = {		/* screen resolution names */
	"LOW", "MEDIUM", "HIGH", "DENSE"
};
static uint16_t spalette[16];		/* original color palette settings */
static uint16_t palette[16];		/* current palette settings */
static uint16_t scursor;		/* original text cursor mode */

#ifdef LIBCMINI
__LINEA *__aline;
__FONT  **__fonts;
short  (**__funcs) (void);

#ifdef __mcoldfire__
	// On ColdFire V4e, the standard Line A opcodes
	// conflict with some valid MAC instructions.
	// Fortunately, the following range is always invalid
	// and triggers the standard Line A exception.
	// The ColdFire OS will keep only the last 4 bits
	#define LINEA_OPCODE_BASE 0xa920
#else
	#define LINEA_OPCODE_BASE 0xa000
#endif
	#define ASM_LINEA3(opcode) ".word	" #opcode
	#define ASM_LINEA2(opcode) ASM_LINEA3(opcode)
	#define ASM_LINEA(n) ASM_LINEA2(LINEA_OPCODE_BASE+n)

void linea0(void)
{
        register __LINEA *__xaline __asm__ ("a0");
        register __FONT **__xfonts __asm__ ("a1");
        register short (**__xfuncs) (void) __asm__ ("a2");

        __asm__ volatile
        (
		ASM_LINEA(0x0)
        : "=g"(__xaline), "=g"(__xfonts), "=g"(__xfuncs)  /* outputs */
        :                                                 /* inputs  */
        : __CLOBBER_RETURN("a0") __CLOBBER_RETURN("a1") __CLOBBER_RETURN("a2") "d0", "d1", "d2", "cc"       /* clobbered regs */
	  AND_MEMORY
        );

        __aline = __xaline;
        __fonts = __xfonts;
        __funcs = __xfuncs;
}


#include <time.h>

char *ctime(const time_t *timer)
{
	return "date/heure";
}
#endif /* LIBCMINI */

static __FONT *system_font;	/* pointer to default system font */
static __FONT *small_font;	/* pointer to small font */


/*	Some useful information about our environment */
static uint16_t
	g_wchar, g_hchar,	/* VDI's idea of current font size */
	junk, wkid_physical, wkid;	/* Graphics ID, workstation ID */

static uint16_t oldbut = 0;	/* Previous state of mouse buttons */
static uint16_t mctrl;		/* current BEG_MOUSE state */
static struct { char *norm, *shift, *caps; } *kt;	/* Keyboard mapping */
static uint16_t mousehidden;

/*
 *	This object tree is for displaying the desk accessory menu.
 *	Actual positions are written into this structure when the
 *	screen size is known.
 */
OBJECT menu[] = {
    -1, 1, 4,	G_IBOX,   OF_NONE,  OS_NORMAL, 0x0L,	   0, 0, 0, 0,/* Root */
     4, 2, 2,	 G_BOX,   OF_NONE,  OS_NORMAL, 0x10F0L,    0, 0, 1, 1,/* BAR */
     1, 3, 3,	G_IBOX,   OF_NONE,  OS_NORMAL, 0x0L,	   0, 0, 1, 1,/* Active */
     2,-1,-1,  G_TITLE,   OF_NONE,  OS_NORMAL, (OBSPEC)"", 0, 0, 1, 1,/* title */
     0, 5, 5,	G_IBOX,   OF_NONE,  OS_NORMAL, 0x0L,	   0, 0, 0, 0,/* screen */
     4, 6,13,	 G_BOX,   OF_NONE,  OS_NORMAL, 0xFF1100L,  0, 0, 0, 0,/* box */
     7,-1,-1, G_STRING,   OF_NONE,  OS_NORMAL, (OBSPEC)TEXT179,0,0,168,	16,
/*                                       "  About MicroEmacs" */
     8,-1,-1, G_STRING,   OF_NONE,  OS_DISABLED, (OBSPEC)"---------------------",
						           0, 16, 168, 16,
     9,-1,-1, G_STRING,   OF_NONE,  OS_NORMAL, (OBSPEC)"", 0, 32, 168, 16,
    10,-1,-1, G_STRING,   OF_NONE,  OS_NORMAL, (OBSPEC)"", 0, 48, 168, 16,
    11,-1,-1, G_STRING,   OF_NONE,  OS_NORMAL, (OBSPEC)"", 0, 64, 168, 16,
    12,-1,-1, G_STRING,   OF_NONE,  OS_NORMAL, (OBSPEC)"", 0, 80, 168, 16,
    13,-1,-1, G_STRING,   OF_NONE,  OS_NORMAL, (OBSPEC)"", 0, 96, 168, 16,
     5,-1,-1, G_STRING, OF_LASTOB,  OS_NORMAL, (OBSPEC)"", 0,112, 168, 16
};

/* In stmouse.s */
extern void mymouse(void*);	/* .s file for calling two mouse handlers */
extern void (*mousevec_tos)(void*);	/* Address of second handler */

void (*first_mhandler)(void*);	/* Address of first handler */
static _KBDVECS *keyboard_vectors;		/* OS's keyboard vector table */
static int msevntsiz = 0;	/* Events in private mouse event queue */
static uint16_t msevntq[16];	/* State of mouse keys at event */
static int msevntin = 0;	/* Queue input ptr */
static int msevntout = 0;	/* Queue output ptr */
static int mousekeys1 = 0;	/* Last mouse key event */
static int mousecol = HUGE;	/* current mouse column */
static int mouserow = HUGE;	/* current mouse row */

/* Forward references. */
int ttopen();	
int ttgetc();
int ttputc();
int ttflush();
int ttclose();
static int stmove();
static int steeol();
static int steeop();
static int stbeep();
static int stopen();
static int stclose();
static int stgetc();
static int stputc(int);
static int stputs(const char *s, size_t count); /* Local convenience */
static int strev();
static int strez();
static int stkopen();
static int stkclose();
#if COLOR
static int stfcol();
static int stbcol();
#endif

static int extcode(uint16_t special_keys, uint16_t scancode, uint16_t key);

/* Convenience definitions */
#define cursor_on() Cursconf(1, 0)
#define cursor_off() Cursconf(0, 0)
#define cursor_blink_on() Cursconf(2, 0)
#define cursor_blink_off() Cursconf(3, 0)

#if 0
/* Only use for debugging */
static void debugmsg(char *msg) {
	FILE *f = fopen("debug.txt","a");
	if (f == NULL) {
		fopen("debug.txt","w");
	}
	fputs(msg, f);
	fflush(f);
	fclose(f);
}
#endif

/*
 * Dispatch table. All the
 * hard fields just point into the
 * terminal I/O code.
 */
TERM	term = {
	NROW-1,
	NROW-1,
	NCOL,
	NCOL,
	0, 0,
	MARGIN,
	SCRSIZ,
	NPAUSE,
	&stopen,
	&stclose,
	&stkopen,
	&stkclose,
	&stgetc,
	&stputc,
	&ttflush,
	&stmove,
	&steeol,
	&steeop,
	&steeop,
	&stbeep,
	&strev,
	&strez
#if	COLOR
	, &stfcol,
	&stbcol
#endif
};

/*	Input buffers and pointers	*/

#define	IBUFSIZE	64	/* this must be a power of 2 */

static uint8_t in_buf[IBUFSIZE];	/* input character buffer */
static int in_next = 0;		/* pos to retrieve next input character */
static int in_last = 0;		/* pos to place most recent input character */

static void in_init()	/* initialize the input buffer */
{
	in_next = in_last = 0;
}

static int in_check()	/* is the input buffer non-empty? */
{
	return in_next == in_last ? FALSE : TRUE;
}

static in_put(int event)
{
	in_buf[in_last++] = event;
	in_last &= (IBUFSIZE - 1);
}

static int in_get()	/* get an event from the input buffer */
{
	int event;	/* event to return */

	event = in_buf[in_next++];
	in_next &= (IBUFSIZE - 1);
	return(event);
}

static int switch_font(__FONT *font)
{
	V_CEL_HT = font->size -1;
	V_CEL_WR = __aline->_VWRAP * font->size;
	V_CEL_MY = (V_Y_MAX / font->size) - 1;
	V_CEL_MX = (V_X_MAX / font->max_cell_width) - 1;
	V_FNT_WR = font->form_width;
	V_FNT_ST = font->first_ade;
	V_FNT_ND = font->last_ade;
	V_OFF_AD = (char*)font->off_table;
	V_FNT_AD = font->dat_table;
}

static int stmove(int row, int col)
{
	char buf[] = { ESC, 'Y', BIAS, BIAS };
	buf[2] += row;
	buf[3] += col;
	stputs(buf, sizeof(buf));
}

static int steeol(void)
{
	char buf[] = { ESC, 'K' };
	stputs(buf, sizeof(buf));
}

int steeop(void)
{
	char buf[] = { ESC, 'J' };

#if	COLOR
	stfcol(gfcolor);
	stbcol(gbcolor);
#endif
	stputs(buf, sizeof(buf));
}

static int strev(int status)	/* set the reverse video state */
/* status: TRUE = reverse video, FALSE = normal video */
{
	if (currez > 1) {
		char buf[] = { ESC, 'q' };
		if (status)
			buf[1] = 'p';
		stputs(buf, sizeof(buf));
	}
}

#if	COLOR
static int mapcol(int color)	/* medium rez color translation */
/* clr: emacs color number to translate */
{
	static int mctable[] = { 0, 1, 2, 3, 2, 1, 2, 3};

	if (currez != 1)
		return(color);
	else
		return(mctable[color]);
}

static int stfcol(int color)	/* set the foreground color */
{
	if (currez < 2) {
		char buf[] = { ESC, 'b', mapcol(color) };
		stputs(buf, sizeof(buf));
	}
}

static int stbcol(int color)	/* set the background color */
{
	if (currez < 2) {
		stputc(ESC);
		stputc('c');
		stputc(mapcol(color));
	}
}
#endif

static char beep[] = {
	0x00, 0x00,
	0x01, 0x01,
	0x02, 0x01,
	0x03, 0x01,
	0x04, 0x02,
	0x05, 0x01,
	0x07, 0x38,
	0x08, 0x10,
	0x09, 0x10,
	0x0A, 0x10,
	0x0B, 0x00,
	0x0C, 0x30,
	0x0D, 0x03,
	0xFF, 100,
	0x09, 0x00,
	0x0A, 0x00,
	0xFF, 0x00
};

static int stbeep()
{
	Dosound(beep);
}

static void mouse_on()	/* turn the gem mouse ON */
{
	if (!mouseon) {
		graf_mouse(M_ON, 0L);
		graf_mouse(ARROW, 0L);
		mouseon = 1;
	}
}

static void mouse_off()	/* turn the gem mouse OFF */
{
	if (mouseon) {
	    graf_mouse(M_OFF, 0L);
	    mouseon = 0;
	}
	
	if (!mousehidden)
		;//stbeep();
}

/*
	me_mh - MicroEmacs Mouse interrupt Handler.  This handler is called,
	in addition to the regular mouse handler, in order for microemacs
	to have access to some more information about the mouse events.

	What it does is:

	- Queue a copy of the mouse button state at the actual moment of
	the event, rather than after GEM is finished playing with timers.

	- Convert all right-mouse-button events into left-mouse-button
	events BEFORE GEM's mouse interrupt handler, so that GEM will generate
	a left-mouse-button event.
*/
static void me_mh(void *b)
{
	unsigned mousekeys;
	char *a = (char*)b;
	
	mousekeys = a[0] & 7;
	if (mousekeys != 0)
		a[0] = 0xFA;	 /* Any key down means button 1 down */
	else
		a[0] = 0xf8;	/* Otherwise button 1 up */
	if (msevntsiz < 16) {
		/* Previous event was processed, look for next one */
		if (mousekeys != mousekeys1) {
			/* A button change is detected, insert it in queue */
			msevntq[msevntin++] = mousekeys;
			msevntin &= 15;
			msevntsiz++;
			mousekeys1 = mousekeys;
		}
	}
}


static int stkopen()	/* open the keyboard (and mouse) */
{
	/* Set up an interrupt handler for the mouse that performs both
	 * me_mh() and the default mouse handling.  The .s file "stmouse.s"
	 * contains some special code for this purpose. */
	first_mhandler = me_mh;
	keyboard_vectors = Kbdvbase();
	mousevec_tos = keyboard_vectors->mousevec;
	keyboard_vectors->mousevec = mymouse;
}

static int stopen()	/* open the screen */
{
	int i;

	strcpy(os, "TOS");

	linea0();
	system_font = __fonts[SCRFONT];
	small_font = __fonts[1];
	
	/* save if the current mouse is hidden (ie we are not in GEM) */
	gemflag = (mousehidden == 0);


	/* Setup AES and VDI */
	appl_init();
	wkid_physical = graf_handle(&g_wchar, &g_hchar, &junk, &junk);
#if	0
	/* We never use the VDI, but if you do, turn this back on */
	v_opnvwk(worki, &wkid, worko);
#endif


	/* In order to have both the mouse cursor and the text cursor on the
	 * screen at the same time, we have to flash it ourselves, turning
	 * the mouse cursor on and off for each flash.
	 * The cursors are both off whenever we are not in an input wait. */
	cursor_blink_off();	/* Stop text cursor from flashing */   
	cursor_off();		/* Turn off text cursor off */
	mouseon = 1;		/* Assume the mouse is on */
	mouse_off();		/* to force the turn off */

	/* Set up the menu bar's coordinates to match the font and screen size
	 * for this screen resolution */
	wind_get(0, WF_CURRXYWH,	/* Fetch actual screen size for menu */
		&menu[0].ob_x, &menu[0].ob_y,
		&menu[0].ob_width, &menu[0].ob_height);
	menu[1].ob_x = menu[0].ob_width - 1;	/* Locate "desk" box */
	menu[4].ob_width = menu[0].ob_width;
	menu[4].ob_height = 8*g_hchar;
	menu[5].ob_width = 21*g_wchar;
	menu[5].ob_height = 8*g_hchar;
	menu[5].ob_x = menu[0].ob_width - menu[5].ob_width;
	for( i=6; i<14; i++) {
		/* Desk accessory items */
		menu[i].ob_y = (i-6)*g_hchar;
		menu[i].ob_width = menu[5].ob_width;
		menu[i].ob_height = g_hchar;
	}
	menu_bar(menu, 1);

	/* Shut off GEM's user interface until we enter an input wait.
	 * Note that unless we claim the mouse with BEG_MCTRL, we will not
	 * get scheduled to run any time the left mouse button goes down while
	 * the mouse is on the top 16 pixels of the screen.  We keep Emacs
	 * "hung" whenever MCTRL is given to desk accessories or GEM, and keep
	 * GEM hung whenever we have control. */
	wind_update(BEG_UPDATE);	/* Shuts off GEM drawing */
	wind_update(BEG_MCTRL);		/* Shuts off GEM use of mouse */

	mctrl = 0;			/* Flag that we have mouse control */
	kt = Keytbl( -1L, -1L, -1L);

	ttopen();
	eolexist = TRUE;

	/* save the current color palette */
	for (i=0; i<16; i++)
		spalette[i] = Setcolor(i, -1);

	/* and find the current resolution */
	initrez = currez = Getrez();
	strcpy(sres, resolution_name[currez]);

	/* set up the screen size and palette */
	switch (currez) {
		case 0: term.t_mrow = 25 - 1;
			term.t_nrow = 25 - 1;
			term.t_ncol = 40;
			strcpy(palstr, LOWPAL);
			break;

		case 1: term.t_mrow = 25 - 1;
			term.t_nrow = 25 - 1;
			strcpy(palstr, MEDPAL);
			break;

		case 2: term.t_mrow = DENSIZE - 1;
			term.t_nrow = 25 - 1;
			strcpy(palstr, HIGHPAL);
	}

	/* and set up the default palette */
	spal(palstr);

	stputs(ESCS "w", 2); /* automatic overflow off */

	/* initialize the input buffer */
	in_init();
}

static int stkclose()	/* close the keyboard (and mouse) */
{
	keyboard_vectors->mousevec = mousevec_tos;	/* Restore original handler */

	mouse_on();		/* turn the mouse on */
	if (gemflag != 0) {
		/* coming from gem..... we */
		cursor_off();		/* turn the cursor off */
	} else {
		/* coming from a shell, we */
		cursor_on();		/* turn the cursor on */
		cursor_blink_on();		/* Turn text cursor blinking */
		// FIXME hidemouse();		/* Turn mouse off for shell */
	}
}

static int stclose()
{
	stputs(ESCS "v", 2);	/* auto overflow on */

	/* restore the original screen resolution */
	if (currez == 3)
		switch_font(system_font);
	strez(resolution_name[initrez]);

	/* restore the original palette settings */
	Setpalette(spalette);

	ttclose();

	wind_update(END_MCTRL);		/* Return GEM's control of screen */
	wind_update(END_UPDATE);
#if	0
	v_clsvwk(wkid);			/* Clean up GEM */
#endif
	appl_exit();
}

/*	spal(pstr):	reset the current palette according to a
			"palette string" of the form

	000111222333444555666777

	which contains the octal values for the palette registers
*/
int spal(char *pstr)
/* pstr: palette string */
{
	int pal;	/* current palette position */
	int clr;	/* current color value */
	int i;

	for (pal = 0; pal < 16; pal++) {
		if (*pstr == 0)
			break;

		/* parse off a color */
		clr = 0;
		for (i = 0; i < 3; i++)
			if (*pstr)
				clr = clr * 16 + (*pstr++ - '0');
		palette[pal] = clr;
	};

	/* and now set it */
	Setpalette(palette);
}

static void domousekey(int newbut, int special_keys)
{
	register int k;

	for (k=1; k!=4; k = k<<1) {	/* k is butnum, k is bit num */ 
		/* For each button on the mouse */
		if ((oldbut&k) != (newbut&k)) {
			/* This button changed, generate an event */
			in_put(0);		/* escape indicator */
			in_put(MOUS >> 8);	/* mouse event */
			in_put(mousecol);	/* x-position */
			in_put(mouserow);	/* y-position */
			in_put(((newbut&k) ? 0 : 1)
				+ (k==1 ? 4 : 0)
				+ ((special_keys&3) ? 'A' : 'a'));
		}
	}
	oldbut = newbut;
}

static int stgetc()	/* get a char from the keyboard */
{
	static uint16_t bexpected = 1;	/* Expected next mouse state */
	int      flashcounter;		/* Counter for flashing cursor */
	uint16_t ev_which;		/* Event returned */
	uint16_t mx, my, bstate, special_keys, key, mc;/* Event parameters */
	uint16_t mes[8];

	/* Working out a multi-byte input sequence */
	if (in_check())
		return(in_get());

	/* Turn on text cursor, mouse and give control to GEM while we're waiting */
	cursor_on();
	mouse_on();
	wind_update(END_UPDATE);
	if (mctrl)
		wind_update(END_MCTRL);
	
	flashcounter = 0;
	
	for(;;) {
		if (!mctrl) {
			/* We have control, so it is okay to flash cursor */
			if (flashcounter == 6) {
				mouse_off();
				cursor_off(); /* After 60ms, turn it off */
				mouse_on();
			} else if (flashcounter == 10) {
				/* Wrapped around one cycle of cursor flashing,
				   turn it on */
				mouse_off();
				cursor_on();
				mouse_on();
				flashcounter = 0;
			}
		}

		/* Wait for events */
		ev_which = evnt_multi(
			MU_TIMER | MU_MESAG | MU_KEYBD | MU_BUTTON | MU_M1,
			1,		/* Maximum clicks to wait for */
			1,		/* Buttons that generate events */
			bexpected,	/* Button states that generate events */
			0, menu[1].ob_x, menu[1].ob_y, menu[1].ob_width, menu[1].ob_height,
			0, 0           , 0           , 0               , 0, /* enter/exit, x, y, w, h for rect 2 */
			mes,		/* Buffer to receive mesasge */
			/* Low and high order miliseconds of counter */
			100L,
			&mx, &my,	/* Mouse location */
			&bstate,	/* State of the mouse buttons */
			&special_keys,	/* State of the special keys */
			&key,		/* Key pressed */
			&mc);		/* Actual number of clicks */

		if (ev_which & MU_KEYBD) {
			uint16_t scancode; /* Keyboard scan code */

			/* Keyboard events cause keystrokes, add SPC prefix to fn keys */
			if (mctrl)
				wind_update(BEG_MCTRL);
			Giaccess(0, 9+128);
			Giaccess(0, 10+128);

			/* Cancel any double-clicking */
			nclicks = 0;
			mctrl = 0;

	 	        scancode = key >> 8;
		        key &= 0xff;
			
			extcode(special_keys, scancode, key);
		} else if (ev_which & MU_BUTTON) {
			/* Mouse event, look at our actual mouse event */
			uint16_t top, junk;
			
			wind_get(0, WF_TOP, &top, &junk, &junk, &junk);
			if (top == 0) {
				/* Desktop is top windo, allow Emacs to continue */
				bexpected = (~bstate) & 1;
				mousecol = mx / g_wchar;
				mouserow = my / g_hchar;
				
				while(msevntsiz > 0) {
					/* Process each queued event */
					domousekey(msevntq[msevntout++], special_keys);
					msevntout &= 15;
					msevntsiz--;
				}
				
				if (mctrl)
					wind_update(BEG_MCTRL);
				mctrl = 0;
			}
		} else if (ev_which & MU_M1) {
			/* Mouse entered desk menu, allow it to appear */
			if (!mctrl)
				wind_update(END_MCTRL);
			mctrl = 1;
			sgarbf = TRUE;
		} else if (ev_which & MU_TIMER) {
			flashcounter++;
		} else {
			/* Most likely is the about message */
		}

		/* is there now a pending event? */
		if (in_check()) {
			key = in_get();
			break;
		}
	}

	/* Disable input (mouse, cursor) while we're processing it */
	if (mctrl)
		wind_update(BEG_MCTRL);
	wind_update(BEG_UPDATE);
	mouse_off();
	cursor_off();			/* Turn text cursor off */
	
	return(key & 0xFF);
}

static stputc(int c)	/* output char c to the screen */
{
	Bconout(DEV_CONSOLE, c);
}

static int stputs(const char *s, size_t count) /* Local convenience */
{
	while (count--)
		stputc(*s++);
}

static int strez(const char *newrez)	/* change screen resolution */
{
	int nrez;	/* requested new resolution */

	/* first, decode the resolution name */
	for (nrez = 0; nrez < 4; nrez++)
		if (strcmp(newrez, resolution_name[nrez]) == 0)
			break;
	if (nrez == 4) {
		mlwrite(TEXT180); /* "%%No such resolution" */
		return(FALSE);
	}

	/* next, make sure this resolution is legal for this monitor */
	if ((currez < 2 && nrez > 1) || (currez > 1 && nrez < 2)) {
		mlwrite(TEXT181); /* "%%Resolution illegal for this monitor" */
		return(FALSE);
	}

	/* eliminate non-changes */
	if (currez == nrez)
		return(TRUE);

	/* finally, make the change */
	switch (nrez) {
		case 0: /* low resolution - 16 colors */
			newwidth(TRUE, 40);
			strcpy(palstr, LOWPAL);
			Setscreen(-1L, -1L, 0);
			g_wchar = g_hchar = 8;
			break;

		case 1: /* medium resolution - 4 colors */
			newwidth(TRUE, 80);
			strcpy(palstr, MEDPAL);
			Setscreen(-1L, -1L, 1);
			g_wchar = g_hchar = 8;
			break;

		case 2: /* High resolution - 2 colors - 25 lines */
			newsize(TRUE, 25);
			strcpy(palstr, HIGHPAL);
			switch_font(system_font);
			g_wchar = g_hchar = 16;
			break;

		case 3: /* Dense resolution - 2 colors - 40 lines */
			newsize(TRUE, DENSIZE);
			strcpy(palstr, HIGHPAL);
			switch_font(small_font);
			g_wchar = g_hchar = 8;
			break;
	}

	/* and set up the default palette */
	spal(palstr);
	currez = nrez;
	strcpy(sres, resolution_name[currez]);

	stputs(ESCS "w", 2); /* automatic overflow off */
	return(TRUE);
}

/*	extcode:	resolve Atari-ST extended character codes
			encoding the proper sequences into emacs
			printable character specifications
*/

static int extcode(uint16_t special_keys, uint16_t scancode, uint16_t key)
/* special_keys: Shift keys from event */
/* scancode: ST scancode */
/* key: GEMDOS translation of the key */
{
	uint32_t shift;	/* CAPS LOCK flag doesn't come from EVENT_MULTI */
	unsigned code;	/* Build up special function code */


	/* Identify any shift-key codes associated with this keystroke */
	code = 0;

	/* I don't know why, but for some reason the codes for ALT of top row and
	 * for CTRL of left, right and HOME come up wrong, and this fixes them. */
	if (scancode == 0x77) scancode = 0x47;
	else if (scancode == 0x73) scancode = 0x4b;
	else if (scancode == 0x74) scancode = 0x4d;
	else if (scancode > 0x76) scancode -= 0x76;

	/* Bring the shifted function key scan codes back into regular range */
	if (scancode >= 0x54 && scancode <= 0x5d)
		scancode -= (0x54-0x3B);

	if (special_keys & K_ALT) {
		char frkeyb_alts;
		/* French keyboard uses alt and alt-shift */
		switch (scancode) {
			case 0x1a: /* ¨^{[ */
			case 0x1b: /* $*}] */
			case 0x28: /* ù\% */
			case 0x2b: /* #|@~ */
			case 0x29: /* On PC, emulators the backtick ` has alt, but we don't want it on the Atari*/
				frkeyb_alts = -1;
				break;
			default:
				frkeyb_alts = 0;
		}
		
		if (!frkeyb_alts) {
			/* ALT shift requires special handling */
			code |= ALTD;

			/* ALT of ordinary keys always returns '0' for the key.  Look up
			 * the actual key as if ALT weren't there. */
			shift = Kbshift(-1);	/* Get state of shift keys (CAPS lock) */

			/* Map the key to a normal keystroke.  Keypad keys are treated special,
			 * everyone else is mapped using the installed keytables.  This
			 * means it will work right on German and other keyboards. */
			if (special_keys & K_CTRL) {
				code |= CTRL;
				key = kt->caps[scancode];
			}
			else if (special_keys & (K_LSHIFT | K_RSHIFT)) {
				key = kt->shift[scancode];	/* shift */
			}
			else if (shift & K_CAPSLOCK)
				key = kt->caps[scancode];	/* Caps lock */
			else
				key = kt->norm[scancode];
		}
	}

	/* Peel of the numeric keypad keys */
	if (scancode == 0x72) key = 'E';
	if (scancode != 0x66 && (scancode >= 0x63 || scancode == 0x4A || scancode == 0x4E)) { 
		/* Keypad keys are SPEC or CTRL of what's on the key */
		code |= SPEC | CTRL;
	}

	/* Translate function keys into digits */
	if (scancode >= 0x3b && scancode <= 0x5d) { 
		/* This is a F1 through F9 */
		code |= SPEC;
		key = scancode - 0x3b + '1';
		if (key == '9'+1) key = '0';
	}
	if (scancode == 0x62) { code |= SPEC; key = 'H'; }
	if (scancode == 0x61) { code |= SPEC; key = 'X'; }
	if (scancode == 0x52) { code |= SPEC; key = 'C'; }
	if (scancode == 0x47) { code |= SPEC; key = 'D'; }
	if (scancode == 0x48) { code |= SPEC; key = 'P'; }
	if (scancode == 0x4b) { code |= SPEC; key = 'B'; }
	if (scancode == 0x50) { code |= SPEC; key = 'N'; }
	if (scancode == 0x4d) { code |= SPEC; key = 'F'; }

	/* translate CTRL-shifted of keys that don't usually CTRL */
	if ((special_keys & K_CTRL) && (scancode <= 0x0D || key == 0)) {
		/* Control of a non-usually-control key */
		shift = Kbshift(-1);	/* Get state of CAPS lock */
		code |= CTRL;
		if (special_keys & 3) key = kt->shift[ scancode];	/* shift */
		else if (shift & 16) key = kt->caps[ scancode];	/* Caps lock */
		else key = kt->norm[ scancode];
	}
	if (key == 0) key = '@'; /* Catch junk */
	if (code != 0) {
		/* This is a special key */
		if (code & SPEC) {
			/* Get shift and ctrl of function keys */
			if (special_keys & 3) code |= SHFT;
			if (special_keys & K_CTRL) code |= CTRL;
		}
		in_put( 0);
		in_put( code>>8);
		in_put( key);
	}
	else in_put(key);
}

#if	FLABEL
static int fnclabel(int f, int n)		/* label a function key */
/* f,n: default flag, numeric argument [unused] */
{
	/* on machines with no function keys...don't bother */
	return(TRUE);
}
#endif
#else
sthello()
{
}
#endif
