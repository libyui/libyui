##   -*- autoconf -*-

dnl  Include file for "configure.in" for the YaST2 Qt UI
dnl  See file COPYING for license
dnl
dnl  Author:	Stefan Hundhammer <sh@suse.de>
dnl  Updated:	2001-12-14
dnl


dnl AC_MIN_QT_VERSION(numeric-qt-version, text_qt_version)
dnl Checks for Qt version as specified or higher
dnl
AC_DEFUN([AC_MIN_QT_VERSION],
[
AC_MSG_CHECKING([for Qt $2 or higher])
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -I$QTDIR/include"

AC_TRY_COMPILE([
#include <qglobal.h>
],
[
#if QT_VERSION < $1
#error Qt $2 or higher required!
#endif
],
[AC_MSG_RESULT([yes])],
[AC_MSG_ERROR([Qt $2 or higher required!

Check whether the Qt base packages and the Qt development packages 
are installed and QTDIR is set correctly.
])]
)
CFLAGS="$save_CFLAGS"
])

