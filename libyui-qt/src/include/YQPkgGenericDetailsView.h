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


class QTabWidget;


class YQPkgGenericDetailsView : public QTextBrowser
{
    Q_OBJECT

public:

    /**
     * Constructor for QWidget parents
     **/
    YQPkgGenericDetailsView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgGenericDetailsView();

    /**
     * Returns a uniform heading in HTML format for the specified package:
     * Package name and summary
     **/
    static QString htmlHeading( PMObjectPtr pmObj );

    /**
     * Escapes characters special to HTML in a (plain text) string, such as:
     * '<'   ->   '&lt;'
     * '>'   ->   '&gt;'
     * '&'   ->   '&amp;'
     *
     * Returns the escaped string.
     **/
    static QString htmlEscape( const QString & plainText );

    
public slots:

    /**
     * Show details for the specified package.
     * Delayed (optimized) display if this is embedded into a QTabWidget
     * parent: In this case, wait until this page becomes visible.
     **/
    void showDetailsIfVisible( PMObjectPtr pmObj );
    
    // slot clear() inherited from QTextEdit

    
protected slots:
    
    /**
     * Show data for the last package.
     **/
    void reload( QWidget *newCurrent );

    
protected:
    
    /**
     * Show details for the specified package.
     * Reimplement this in derived classes.
     **/
    virtual void showDetails( PMObjectPtr pmObj ) = 0;
    

    // Data members

    QTabWidget	* 	_parentTab;
    PMObjectPtr		_pmObj;
};


#endif // ifndef YQPkgGenericDetailsView_h
