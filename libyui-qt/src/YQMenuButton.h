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
#include <ycp/YCPString.h>

#include "YMenuButton.h"


class QPushButton;

class YQMenuButton : public QWidget, public YMenuButton
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    YQMenuButton( QWidget * 		parent,
		  const YWidgetOpt &	opt,
		  YCPString 		label );

    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     */
    void setEnabling( bool enabled );

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize( YUIDimension dim );

    /**
     * Sets the new size of the widget.
     */
    void setSize( long newWidth, long newHeight );

    /**
     * Changes the label of the button
     */
    void setLabel( const YCPString & label );

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Actually create the menu structure.
     *
     * Reimplemented - inherited from YMenuButton.
     */
    virtual void createMenu();

    /**
     * Recursively create the menu structure.
     */
    void createMenu( YMenuItem * ymenu, QPopupMenu * _qt_menu );
    
protected slots:

    /**
     * Triggered when any menu item is activated.
     */
    void menuEntryActivated( int menu_item_index );

    /**
     * Triggered via menuEntryActivated() by zero timer to get back in sync
     * with the Qt event loop. 
     */
    void returnNow();

    
protected:

    //
    // Data members
    //
    
    QPushButton *	_qt_pushbutton;
    int			_selected_item_index;
};

#endif // YQMenuButton_h
