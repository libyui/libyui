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

  File:	      YQMenuButton.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQMenuButton_h
#define YQMenuButton_h

#include <qwidget.h>
#include "YMenuButton.h"
#include <QHash>

class QAction;
class QPushButton;
class QMenu;

class YQMenuButton : public QWidget, public YMenuButton
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQMenuButton( YWidget * 		parent,
		  const string &	label );

    /**
     * Destructor.
     **/
    virtual ~YQMenuButton();

    /**
     * Change the label on the button.
     *
     * Reimplemented from YMenuButton / YSelectionWidget.
     **/
    virtual void setLabel( const string & label );

    /**
     * Rebuild the displayed menu tree from the internally stored YMenuItems.
     *
     * Implemented from YMenuButton.
     **/
    virtual void rebuildMenuTree();
    
    /**
     * Set enabled / disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );
    
    /**
     * Accept the keyboard focus.
     **/
    virtual bool setKeyboardFocus();

    
protected slots:

    /**
     * Triggered when any menu item is activated.
     **/
    void menuEntryActivated( QAction * menuItem );

    /**
     * Triggered via menuEntryActivated() by zero timer to get back in sync
     * with the Qt event loop. 
     **/
    void returnNow();

    
protected:

    /**
     * Recursively insert menu items into 'menu' from iterator 'begin' to iterator 'end'.
     **/
    void rebuildMenuTree( QMenu * menu, YItemIterator begin, YItemIterator end );

    
    //
    // Data members
    //
    
    YMenuItem *		_selectedItem;
    QPushButton * 	_qt_button;
    QHash<QAction*,int> _serials;
};

#endif // YQMenuButton_h
