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


#ifndef YQComboBox_h
#define YQComboBox_h

#include <QFrame>
#include "YComboBox.h"

class YQWidgetCaption;
class QComboBox;
class QY2CharValidator;


class YQComboBox : public QFrame, public YComboBox
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQComboBox( YWidget * parent, const string & label, bool editable );

    /**
     * Destructor.
     **/
    ~YQComboBox();

    /**
     * Add one item. This widget assumes ownership of the item object and will
     * delete it in its destructor.
     *
     * Reimplemented from YComboBox.
     **/
    virtual void addItem( YItem * item );

    /**
     * Delete all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();

    /**
     * Change the label text.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void setLabel( const string & label );

    /**
     * Change the valid input characters.
     *
     * Reimplemented from YComboBox.
     **/
    virtual void setValidChars( const string & validChars );

    /**
     * Specify the amount of characters which can be inserted.
     *
     * Reimplemented from YComboBox.
     **/
    virtual void setInputMaxLength( int numberOfChars );

    /**
     * Returns 'true' if the given text is valid according to the current
     * setting of ValidChars.
     **/
    bool isValidText( const QString & txt ) const;

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
     * Tells the ui that an item has been selected. This is only
     * interesting if the `notify option is set.
     **/
    void slotSelected( int i );

    /**
     * Tells the ui that the user has edited the text ( if the
     * 'editable' option is set ).
     * This is only interesting if the `notify option is set.
     **/
    void textChanged( QString );


protected:

    /**
     * Return this ComboBox's current value as text.
     *
     * Called internally from value(), selectedItem() and related.
     *
     * Implemented from YComboBox.
     **/
    virtual string text();

    /**
     *
     * Set this ComboBox's current value as text.
     *
     * Called internally whenever the content is to change
     * programmatically. Don't call setValue() or selectItem() from here.
     *
     * Implemented from YComboBox.
     **/
    virtual void setText( const string & newText );


    //
    // Data members
    //

    YQWidgetCaption *	_caption;
    QComboBox * 	_qt_comboBox;
    QY2CharValidator *	_validator;
};


#endif // YQLabel_h
