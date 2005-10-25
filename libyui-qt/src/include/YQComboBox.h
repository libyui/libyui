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

  File:	      YQComboBox.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQComboBox_h
#define YQComboBox_h

#include <qvbox.h>
#include <ycp/YCPString.h>

#include "YComboBox.h"


class QLabel;
class QComboBox;
class QY2CharValidator;


class YQComboBox : public QVBox, public YComboBox
{
    Q_OBJECT

public:

    YQComboBox( QWidget * parent, const YWidgetOpt & opt, const YCPString & label );

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
     * Change the label text.
     */
    void setLabel( const YCPString & label );

    /**
     * Change the valid input characters.
     *
     * Reimplemented from @ref YComboBox
     */
    void setValidChars( const YCPString & validChars );

    /**
     * Returns 'true' if the given text is valid according to the current
     * setting of ValidChars. 
     **/
    bool isValidText( const QString & txt ) const;
    
    /**
     * Notification that a new item has been added.
     *
     * Reimplemented from YComboBox.
     */
    void itemAdded( const YCPString & itemText, int itemIndex, bool selected );

    /**
     * Inherited from YComboBox:
     * Returns the ComboBox value.
     */
    YCPString getValue() const;

    /**
     * Inherited from YComboBox:
     * Sets the ComboBox value to a random value that is not already in
     * the item list. Will be called for editable ComboBox widgets only.
     */
    void setValue( const YCPString & new_value );

    /**
     * Inherited from YComboBox:
     * Returns the index of the currently selected item (from 0 on)
     * or -1 if no item is selected.
     **/
    int getCurrentItem() const;
    
    /**
     * Inherited from YComboBox:
     * Selects an item from the list. Notice there intentionally is no
     * corresponding getCurrentItem() method - use getValue() instead.
     */
    void setCurrentItem( int index );

    /**
     * Accept the keyboard focus.
     */
    virtual bool setKeyboardFocus();

    /**
     * Delete all items.
     *
     * Reimplemented from YSelectionWidget
     */
    virtual void deleteAllItems();

    /**
     * Specify the amount of characters which can be inserted.
     *
     * Reimplemented from @ref YTextEntry.
     */
    void setInputMaxLength( const YCPInteger & numberOfChars );


protected slots:

    /**
     * Tells the ui that an item has been selected. This is only
     * interesting if the `notify option is set.
     */
    void slotSelected( int i );

    /**
     * Tells the ui that the user has edited the text ( if the
     * 'editable' option is set ).
     * This is only interesting if the `notify option is set.
     */
    void textChanged( const QString &new_text );

    
protected:

    //
    // Data members
    //
    
    QLabel * 		_qt_label;
    QComboBox * 	_qt_combo_box;
    QY2CharValidator *	_validator;
};


#endif // YQLabel_h
