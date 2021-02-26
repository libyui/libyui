/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      YQPkgChangesDialog.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgChangesDialog_h
#define YQPkgChangesDialog_h

#include <qdialog.h>
#include <qregexp.h>


class YQPkgList;


/**
 * Changes dialog: Show a dialog with a list of packages that are changed.
 * By default, only packages with an "auto" status ( not set via selections )
 * are displayed.
 **/
class YQPkgChangesDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * Static convenience method: Post a changes dialog with text
     * 'message', a list of changed packages and one ( default ) or two buttons.
     *
     * Returns 'true' if the user accepted ( i.e. clicked the 'accept' button )
     * and 'false' if the user rejected ( i.e. clicked the 'reject' button or
     * the window manager close button ).
     *
     * If the list is empty ( i.e., there are no packages with an "auto"
     * status ), the dialog is not shown at all ( and returns 'true' ) - unless
     *'showIfListEmpty' is 'true'.
     **/
    static bool showChangesDialog( QWidget *		parent,
				   const QString & 	message,
				   const QString &	acceptButtonLabel,
				   const QString &	rejectButtonLabel = QString::null,
				   bool			showIfListEmpty   = false	);


    /**
     * Static convenience method: Post a changes dialog with text 'message', a
     * list of changed packages whose names match the specified regular
     * expression 'regexp' and one ( default ) or two buttons.
     *
     * Returns 'true' if the user accepted ( i.e. clicked the 'accept' button )
     * and 'false' if the user rejected ( i.e. clicked the 'reject' button or
     * the window manager close button ).
     *
     * If the list is empty ( i.e., there are no packages with an "auto"
     * status ), the dialog is not shown at all ( and returns 'true' ) - unless
     *'showIfListEmpty' is 'true'.
     **/
    static bool showChangesDialog( QWidget *		parent,
				   const QString & 	message,
				   const QRegExp & 	regexp,
				   const QString &	acceptButtonLabel,
				   const QString &	rejectButtonLabel = QString::null,
				   bool			showIfListEmpty   = false	);

    /**
     * Returns the preferred size.
     *
     * Reimplemented from QWidget to limit the dialog to the screen dimensions.
     **/
    virtual QSize sizeHint () const;

protected:

    /**
     * Constructor: Creates a changes dialog with text 'message' on
     * top, a list packages with an "auto" status that is not set via selections
     * and one ( default ) or two buttons.
     *
     * Not meant for public use. Applications should use the static
     * 'showChangesDialog' method instead.
     *
     * This constructor does not call filter() yet - this is the caller's
     * responsibility.
     **/
    YQPkgChangesDialog( QWidget *		parent,
			const QString & 	message,
			const QString &		acceptButtonLabel,
			const QString &		rejectButtonLabel = QString::null );

    /**
     * Apply the filter criteria: Fill the pkg list with pkgs that have a
     * "modify" status ( install, update, delete ) set by automatic ( i.e. via the
     * dependency solver ), by application ( i.e. via software selections ) or
     * manually by the user.
     **/
    void filter( bool byAuto	= true,
		 bool byApp	= false,
		 bool byUser	= false );

    /**
     * Apply the filter criteria: Fill the pkg list with pkgs that have a
     * "modify" status ( install, update, delete ) set by automatic ( i.e. via the
     * dependency solver ), by application ( i.e. via software selections ) or
     * manually by the user and whose name matches 'regexp'.
     **/
    void filter( const QRegExp & regexp,
		 bool byAuto	= true,
		 bool byApp	= false,
		 bool byUser	= false );

    /**
     * Returns 'true' if the pkg list is empty.
     * This is only meaningful after calling 'filter()' !
     **/
    bool isEmpty() const;


    // Data members

    YQPkgList *		_pkgList;
};


#endif // ifndef YQPkgChangesDialog_h
