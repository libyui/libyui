#
# Makefile.am for qt/src/include/icons
#

# pkgincludedir = $(includedir)/y2qt/icons

noinst_HEADERS =				\
	autodel.xpm				\
	autodel_disabled.xpm			\
	autoinstall.xpm				\
	autoinstall_disabled.xpm		\
	autoupdate.xpm				\
	autoupdate_disabled.xpm			\
	checklist_off.xpm			\
	checklist_on.xpm			\
	del.xpm					\
	del_disabled.xpm			\
	delete_conflict.xpm			\
	install.xpm				\
	install_disabled.xpm			\
	keepinstalled.xpm			\
	keepinstalled_disabled.xpm		\
	noinst.xpm				\
	noinst_disabled.xpm			\
	normal_conflict.xpm			\
	protected.xpm				\
	protected_disabled.xpm			\
	sel_autodel.xpm				\
	sel_autodel_disabled.xpm		\
	sel_autoinstall.xpm			\
	sel_autoinstall_disabled.xpm		\
	sel_autoupdate.xpm			\
	sel_autoupdate_disabled.xpm		\
	select_conflict.xpm			\
	stop.xpm				\
	taboo.xpm				\
	taboo_disabled.xpm			\
	unresolvable_conflict.xpm		\
	update.xpm				\
	update_disabled.xpm



# Those icons are used at runtime by the online help system.
# For everything else, they are included in the source code -
# no need to install all the other icons as well.
#
# -sh 2003-01-14

helpimagesdir = $(imagedir)

helpimages_DATA =				\
	autodel.xpm				\
	autoinstall.xpm				\
	autoupdate.xpm				\
	del.xpm					\
	install.xpm				\
	keepinstalled.xpm			\
	noinst.xpm				\
	protected.xpm				\
	sel_autodel.xpm				\
	sel_autoinstall.xpm			\
	sel_autoupdate.xpm			\
	taboo.xpm				\
	update.xpm

EXTRA_DIST = $(helpimages_DATA)

CLEANFILES = y2qt.kdoc

#<INSTALL-HEADER-TARGET>

# Automatically inserted by insert_install_target
# DO NOT REMOVE THE TAGS!

# Install header files only if different to version already installed -
# prevent unnecessary installations to keep make dependency changes
# and thus rebuilds for dependent packages as low as possible.

###	install-pkgincludeHEADERS: $(pkginclude_HEADERS)
###		@$(NORMAL_INSTALL)
###		$(mkinstalldirs) $(DESTDIR)$(pkgincludedir)
###		@list='$(pkginclude_HEADERS)';							\
###		for p in $$list; do								\
###			if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; 			\
###			if ! cmp -s $$d$$p $(DESTDIR)$(pkgincludedir)/$$p; then			\
###				echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p";	\
###				$(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p;		\
###		  	fi									\
###		done

#</INSTALL-HEADER-TARGET>
