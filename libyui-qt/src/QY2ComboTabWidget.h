/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|			   contributed Qt widgets		       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      QY2ComboTabWidget.h

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt widget - it can be used independently of YaST2.

/-*/


#ifndef QY2ComboTabWidget_h
#define QY2ComboTabWidget_h

#include <QString>
#include <QHash>

class QComboBox;
class QLabel;
class QStackedWidget;
class QWidget;


/**
 * This widget is very much like a QTabWidget, but it uses a combo box above
 * the tab pages rather than a ( scrolled ) one-line row of tabs.
 **/
class QY2ComboTabWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor. 'combo_box_label' is the user-visible label of the combo
     * box that is used to switch between the different tab pages.
     **/
    QY2ComboTabWidget( const QString &	combo_box_label,
		       QWidget *	parent = 0,
		       const char *	name   = 0 );

    /**
     * Destructor.
     **/
    virtual ~QY2ComboTabWidget();

    /**
     * Add a page. 'page_label' will be the user-visible combo box entry for
     * that page.
     **/
    void addPage( const QString & page_label, QWidget * page );

    
signals:

    /**
     * Emitted when the current page changes.
     * NOT emitted initially for the very first page that is shown.
     **/
    void currentChanged( QWidget * newCurrentPage );

    
public slots:

    /**
     * Show a page. Updates the combo box contents accordingly.
     * This is an expensive operation: All combo box items are searched for the
     * item that corresponds to this page.
     **/
    void showPage( QWidget * page );


protected slots:

    /**
     * Show a page identified by its index. Does NOT update the combo box
     * contents.
     **/
    void showPageIndex( int index );


protected:

    QComboBox		* combo_box;
    QLabel		* combo_label;
    QStackedWidget 	* widget_stack;
    QHash<int, QWidget *>	  pages;
};

#endif // QY2ComboTabWidget_h
