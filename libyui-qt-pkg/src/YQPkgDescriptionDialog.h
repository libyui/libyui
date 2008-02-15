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

  File:	      YQPkgDescriptionDialog.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgDescriptionDialog_h
#define YQPkgDescriptionDialog_h

#include <qdialog.h>


class YQPkgList;
class YQPkgDescriptionView;


/**
 * Pkg status and description as a standalone popup dialog.
 **/
class YQPkgDescriptionDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * Static convenience method: Post a description dialog for pkg 'pkgName'.
     **/
    static void showDescriptionDialog( const QString & pkgName );

    /**
     * Returns the preferred size.
     *
     * Reimplemented from QWidget to limit the dialog to the screen dimensions.
     **/
    virtual QSize sizeHint () const;

    
protected:

    /**
     * Constructor: Creates a description dialog for all packages that match 'pkgName'.
     **/
    YQPkgDescriptionDialog( QWidget *		parent,
			    const QString &	pkgName );

    /**
     * Apply the filter criteria: Fill the pkg list with pkgs that match the
     * specified package name.  
     **/
    void filter( const QString & pkgName );

    /**
     * Returns 'true' if the pkg list is empty.
     * This is only meaningful after calling 'filter()' !
     **/
    bool isEmpty() const;


    // Data members

    YQPkgList *			_pkgList;
    YQPkgDescriptionView *	_pkgDescription;
};


#endif // ifndef YQPkgDescriptionDialog_h
