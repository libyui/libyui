/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:       YQPkgFilterTab.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef YQPkgFilterTab_h
#define YQPkgFilterTab_h

#include <QWidget>
#include <QHash>
#include <QTabWidget>
#include "ImplPtr.h"

class YQPkgFilterTabPrivate;
class YQPkgFilterPage;
class YQPkgDiskUsageList;
class QAction;


/**
 * Widget for "tabbed browsing" in packages:
 *
 *               /------\/------\/------\
 *     [View v]  | Tab1 || Tab2 || Tab3 |               [Close]
 *     +-----------------+------------------------------------+
 *     |                 |                                    |
 *     | QStackedWidget: |    Right pane                      |
 *     |                 |                                    |
 *     | Filter pages    |    (application defined)           |
 *     |                 |                                    |
 *     |                 |                                    |
 *     |                 |                                    |
 *     .                 .                                    .
 *     .                 .                                    .
 *     .                 .                                    .
 *     |                 |                                    |
 *     +-----------------+------------------------------------+
 *
 * Each filter page corresponds to one tab and a number of widgets in a
 * QStackedWidget in the left filter pane. When tabs are switched, the
 * corresponding filter page is raised to the top of the widget stack.
 * The right pane, however, remains unchanged.
 *
 * Only a small numbers of filter pages is displayed as open tabs right
 * away. Each of the other filter pages is shown in a new tabs when the user
 * requests it via the pop-up menu on [View] button. Similarly, the tabs for all
 * but the last filter pages can be closed with the [Close] button.
 *
 * The left (filter page) and right panes are separated with a user-moveable
 * splitter.
 **/
class YQPkgFilterTab: protected QTabWidget
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQPkgFilterTab( QWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQPkgFilterTab();

    /**
     * Add a page with a user-visible "pageLabel", a widget with the page
     * content and an internal name (or ID). If 'showAlways' is true, the
     * corresponding tab will be opened immediately, otherwise the page will
     * just be added to the pop-up menu on the [New] button.
     *
     * 'pageContent' will be reparented to a subwidget of this class.
     **/
    void addPage( const QString &       pageLabel,
                  QWidget *             pageContent,
                  const QString &       internalName = QString(),
                  bool                  showAlways   = false );

    /**
     * Return the right pane.
     **/
    QWidget * rightPane() const;

    /**
     * Return the disk usage list widget or 0 if there is none.
     **/
    YQPkgDiskUsageList * diskUsageList() const;

    
signals:

    /**
     * Emitted when the current page changes.
     * NOT emitted initially for the very first page that is shown.
     **/
    void currentChanged( QWidget * newPageContent );


public slots:

    /**
     * Show a page. Create a tab for that page if it doesn't already exist.
     **/
    void showPage( QWidget * page );
    void showPage( const QString & internalName );

    /**
     * Close the current page unless this is the last visible page.
     **/
    void closeCurrentPage();
						 
protected slots:

    /**
     * Show the page with the specified tab index.
     **/
    void showPage( int tabIndex );

    /**
     * Show the page with the widget of this action's data().
     **/
    void showPage( QAction * action );


protected:

    /**
     * Show a page.
     **/
    void showPage( YQPkgFilterPage * page );

    /**
     * Find a filter page by its content widget (the widget that was passed
     * to addPage() ).
     * Return 0 if there is no such page.
     **/
    YQPkgFilterPage * findPage( QWidget * pageContent );

    /**
     * Find a filter page by its internal name.
     * Return 0 if there is no such page.
     **/
    YQPkgFilterPage * findPage( const QString & internalName );

    /**
     * Find a filter page by its tab index.
     * Return 0 if there is no such page.
     **/
    YQPkgFilterPage * findPage( int tabIndex );


private:

    ImplPtr<YQPkgFilterTabPrivate> priv;
};



/**
 * Helper class for filter pages
 **/
struct YQPkgFilterPage
{
    YQPkgFilterPage( const QString &	pageLabel,
		     QWidget *		content,
		     const QString &	internalName = QString(),
		     bool		showAlways   = false )
	: content( content )
	, label( pageLabel )
	, id( internalName )
	, showAlways( showAlways )
	, tabIndex( -1 )
	{}

    QWidget *	content;
    QString	label;		// user visible text
    QString	id;		// internal name
    bool	showAlways;
    int		tabIndex;	// index of the corresponding tab or -1 if none
};


#endif // YQPkgFilterTab_h
