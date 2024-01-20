/*	EFUNC.H:	MicroEMACS function declarations and names

		This file list all the C code functions used by MicroEMACS
	and the names to use to bind keys to them. To add functions,
	declare it here in both the extern function list and the name
	binding table.

*/

/*	Name to function binding table

		This table gives the names of all the bindable functions
	end their C function address. These are used for the bind-to-key
	function.
*/

 NBIND	names[] = {
	{"abort-command",		ctrlg},
#if ABBREVIATIONS
	{"add-abbrev",			add_abbrev},
#endif
	{"add-global-mode",		setgmode},
#if UNIX || (VMS && SMG) || MPE
	{"add-keymap",			addkeymap},
#endif
	{"add-mode",			setmod},
	{"append-file",			file_append},
	{"apropos",			apro},
#if	CTAGS
	{"back-from-tag-word",		backtagword},
#endif
	{"backward-character",		back_char},
	{"begin-macro",			ctlxlp},
	{"beginning-of-file",		goto_begining_of_buffer},
	{"beginning-of-line",		goto_begining_of_line},
	{"bind-to-key",			bindtokey},
	{"buffer-position",		showcpos},
	{"case-region-lower",		lowerregion},
	{"case-region-upper",		upperregion},
	{"case-word-capitalize",	capword},
	{"case-word-lower",		lowerword},
	{"case-word-upper",		upperword},
	{"change-file-name",		filename},
	{"change-screen-column",	new_col_org},
	{"change-screen-row",		new_row_org},
	{"change-screen-size",		newsize},
	{"change-screen-width",		newwidth},
	{"clear-and-redraw",		refresh},
	{"clear-message-line",		clrmes},
	{"copy-region",			copyregion},
	{"count-words",			wordcount},
	{"ctlx-prefix",			cex},
	{"cycle-ring",			cycle_ring},
	{"cycle-screens",		cycle_screens},
#if ABBREVIATIONS
	{"define-abbrevs",		def_abbrevs},
	{"delete-abbrev",		del_abbrev},
#endif
	{"delete-blank-lines",		deblank},
	{"delete-buffer",		killbuffer},
	{"delete-global-mode",		delgmode},
	{"delete-kill-ring",		clear_ring},
	{"delete-mode",			delmode},
	{"delete-next-character",	forwdel},
	{"delete-next-word",		delfword},
	{"delete-other-windows",	onlywind},
	{"delete-previous-character",	backdel},
	{"delete-previous-word",	delbword},
	{"delete-screen",		delete_screen},
	{"delete-undos",		undo_delete},
	{"delete-window",		delwind},
#if ABBREVIATIONS
	{"describe-abbrevs",		desc_abbrevs},
#endif
	{"describe-bindings",		desbind},
	{"describe-functions",		desfunc},
	{"describe-key",		deskey},
	{"describe-variables",		desvars},
	{"detab-region",		detab},
	{"display",			dispvar},
	{"end-macro",			ctlxrp},
	{"end-of-file",			goto_end_of_buffer},
	{"end-of-line",			goto_end_of_line},
	{"end-of-word",			endword},
	{"entab-region",		entab},
	{"exchange-point-and-mark",	swap_mark},
	{"execute-buffer",		execbuf},
	{"execute-command-line",	execcmd},
	{"execute-file",		execfile},
	{"execute-macro",		ctlxe},
	{"execute-named-command",	namedcmd},
	{"execute-procedure",		execproc},
	{"execute-program",		execprg},
	{"exit-emacs",			quit},
	{"fill-paragraph",		fillpara},
	{"filter-buffer",		filter},
	{"find-file",			find_file},
	{"find-screen",			find_screen},
	{"forward-character",		forward_char},
	{"global",			global_var},
	{"goto-line",			goto_line},
	{"goto-mark",			goto_mark},
	{"goto-matching-fence",		getfence},
	{"grow-window",			enlargewind},
	{"handle-tab",			tab},
	{"help",			help},
	{"hunt-backward",		backhunt},
	{"hunt-forward",		forwhunt},
	{"i-shell",			spawncli},
#if	ISRCH
	{"incremental-search",		fisearch},
#endif
	{"indent-region",		indent_region},
#if ABBREVIATIONS
	{"insert-abbrevs",		ins_abbrevs},
#endif
	{"insert-file",			insert_file},
	{"insert-space",		insspace},
	{"insert-string",		istring},
#if ABBREVIATIONS
	{"kill-abbrevs",		kill_abbrevs},
#endif
	{"kill-paragraph",		killpara},
	{"kill-region",			killregion},
	{"kill-to-end-of-line",		killtext},
#if	FLABEL
	{"label-function-key",		fnclabel},
#endif
	{"list-buffers",		listbuffers},
#if UNIX || (VMS && SMG) || MPE
	{"list-keymappings",		listkeymaps},
#endif
	{"list-screens",		list_screens},
	{"list-undos",			undo_list},
	{"local",			local_var},
	{"macro-to-key",		macrotokey},
	{"meta-prefix", 		meta},
#if	MOUSE
	{"mouse-move",			mmove},
	{"mouse-move-down",		movemd},
	{"mouse-move-up",		movemu},
	{"mouse-region-down",		mregdown},
	{"mouse-region-up",		mregup},
	{"mouse-resize-screen",		resizm},
#endif
	{"move-window-down",		mvdnwind},
	{"move-window-up",		mvupwind},
	{"name-buffer",			namebuffer},
	{"narrow-to-region",		narrow},
	{"newline",			newline},
	{"newline-and-indent",		indent},
	{"next-buffer",			nextbuffer},
	{"next-line",			forward_line},
	{"next-page",			forward_page},
	{"next-paragraph",		goto_end_of_paragraph},
	{"next-window",			nextwind},
	{"next-word",			forwword},
	{"nop",				nullproc},
	{"open-line",			openline},
	{"overwrite-string",		ovstring},
	{"pipe-command",		pipecmd},
	{"pop-buffer",			popbuffer},
	{"previous-line",		backward_line},
	{"previous-page",		backward_page},
	{"previous-paragraph",		goto_begining__of_paragraph},
	{"previous-window",		prevwind},
	{"previous-word",		backword},
	{"print",			writemsg},
	{"query-replace-string",	qreplace},
	{"quick-exit",			quickexit},
	{"quote-character",		quote},
#if	CTAGS
	{"re-tag-word",			retagword},
#endif
	{"read-file",			file_read},
	{"redraw-display",		reposition},
	{"remove-mark",			remove_mark},
	{"rename-screen",		rename_screen},
	{"replace-string",		sreplace},
	{"resize-window",		resize},
	{"restore-window",		restwnd},
#if	ISRCH
	{"reverse-incremental-search",	risearch},
#endif
	{"run",				execproc},
	{"save-file",			filesave},
	{"save-window",			savewnd},
	{"scroll-next-down",		nextdown},
	{"scroll-next-up",		nextup},
	{"search-forward",		forwsearch},
	{"search-reverse",		backsearch},
	{"select-buffer",		usebuffer},
	{"set",				setvar},
#if	CRYPT
	{"set-encryption-key",		setekey},
#endif
	{"set-mark",			set_mark},
	{"shell-command",		spawn},
	{"show-files",			showfiles},
	{"shrink-window",		shrinkwind},
	{"source",			execfile},
	{"split-current-window",	splitwind},
	{"store-procedure",		storeproc},
#if	BSD || FREEBSD || VMS || SUN || HPUX8 || HPUX9 || AVIION
	{"suspend-emacs",		bktoshell},
#endif
#if	CTAGS
	{"tag-word",			tagword},
#endif
	{"transpose-characters",	twiddle},
	{"trim-region",			trim},
	{"unbind-key",			unbindkey},
	{"undent-region",		undent_region},
	{"undo",			undo},
	{"universal-argument",		unarg},
	{"unmark-buffer",		unmark},
	{"update-screen",		upscreen},
	{"view-file",			view_file},
	{"widen-from-region",		widen},
	{"wrap-word",			wrapword},
	{"write-file",			file_write},
	{"write-message",		writemsg},
	{"yank",			yank},
	{"yank-pop",			yank_pop},

	{"",			NULL}
};

#define	NFUNCS	(sizeof(names)/sizeof(NBIND)) - 1
