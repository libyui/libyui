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

  File:	      YQPkgGenericDetailsView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgGenericDetailsView_h
#define YQPkgGenericDetailsView_h

#include <qtextbrowser.h>
#include <y2pm/PMObject.h>
#include <y2util/Date.h>


class QTabWidget;
using std::string;


/**
 * @short Abstract base class for details views. Handles generic stuff like
 * HTML formatting, Qt slots and display only if this view is visible at all
 * ( it may be hidden if it's part of a QTabWidget).
 **/
class YQPkgGenericDetailsView : public QTextBrowser
{
    Q_OBJECT

protected:

    /**
     * Constructor.
     **/
    YQPkgGenericDetailsView( QWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQPkgGenericDetailsView();

public:

    /**
     * Returns the minimum size required for this widget.
     * Inherited from QWidget.
     **/
    virtual QSize minimumSizeHint() const;

    /**
     * Returns a uniform heading in HTML format for the specified package:
     * Package name and summary
     **/
    static QString htmlHeading( PMObjectPtr pmObj );

    /**
     * Escapes characters special to HTML in a ( plain text) string, such as:
     * '<'   ->   '&lt;'
     * '>'   ->   '&gt;'
     * '&'   ->   '&amp;'
     *
     * Returns the escaped string.
     **/
    static QString htmlEscape( const QString & plainText );

    /**
     * Returns a string containing a HTML table with 'contents'.
     **/
    static QString table( const QString & contents );

    /**
     * Returns a string containing a HTML table row with 'contents'.
     **/
    static QString row( const QString & contents );

    /**
     * Returns a string containing a HTML table cell with 'contents'.
     **/
    static QString cell( QString contents		);
    static QString cell( int contents 			);
    static QString cell( const string & contents	);
    static QString cell( const Date & date		);

    /**
     * Returns a string containing a HTML table cell with 'contents'
     * for table headers.
     **/
    static QString hcell( QString contents );

    /**
     * Format a date.
     **/
    static QString formatDate( const Date & date );


public slots:

    /**
     * Show details for the specified package.
     * Delayed ( optimized) display if this is embedded into a QTabWidget
     * parent: In this case, wait until this page becomes visible.
     **/
    void showDetailsIfVisible( PMObjectPtr pmObj );

    // slot clear() inherited from QTextEdit


protected slots:

    /**
     * Show data for the last package.
     **/
    void reload( QWidget * newCurrent );


protected:

    /**
     * Show details for the specified package.
     * Reimplement this in derived classes.
     **/
    virtual void showDetails( PMObjectPtr pmObj ) = 0;

    /**
     * Workaround for Bugzilla bug #19419: Y2Pkg hangs on middle mouse click.
     * Ignore all those events. We don't want them.
     *
     * Reimplemented from QTextEdit / QScrollView.
     **/
    virtual void contentsMousePressEvent   ( QMouseEvent * )  {}
    virtual void contentsMouseReleaseEvent ( QMouseEvent * )  {}
    virtual void contentsDragEnterEvent ( QDragEnterEvent * ) {}
    virtual void contentsDragLeaveEvent ( QDragLeaveEvent * ) {}
    virtual void contentsDragMoveEvent 	( QDragMoveEvent *  ) {}
    virtual void contentsDropEvent 	( QDropEvent *      ) {}


    // Data members

    QTabWidget	* 	_parentTab;
    PMObjectPtr		_pmObj;
};


#endif // ifndef YQPkgGenericDetailsView_h
