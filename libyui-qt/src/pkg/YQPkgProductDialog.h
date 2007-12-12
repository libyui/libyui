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

  File:	      YQPkgProductDialog.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgProductDialog_h
#define YQPkgProductDialog_h

#include <QDialog>


class QTabWidget;
class YQPkgProductList;
class YQPkgDependenciesView;


/**
 * Products list with dependencies details view
 **/
class YQPkgProductDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * Static convenience method: Post a dialog with all products
     **/
    static void showProductDialog();

    /**
     * Returns the preferred size.
     *
     * Reimplemented from QWidget to limit the dialog to the screen dimensions.
     **/
    virtual QSize sizeHint () const;

    /**
     * Delayed initialization after the dialog is fully created.
     *
     * Reimplemented from QWidget.
     **/
    virtual void polish();


protected:

    /**
     * Constructor: Creates a description dialog for all packages that match 'pkgName'.
     **/
    YQPkgProductDialog( QWidget * parent );


    // Data members

    YQPkgProductList *		_productList;
    QTabWidget *		_detailsViews;
    YQPkgDependenciesView *	    _dependenciesView;
};


#endif // ifndef YQPkgProductDialog_h
