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

  File:	      YQMultiSelectionBox.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQMultiSelectionBox_h
#define YQMultiSelectionBox_h

#include <QFrame>
#include <QColorGroup>
#include <QTreeWidget>

#include "YMultiSelectionBox.h"

class YQWidgetCaption;
class YQMultiSelectionBoxItem;


class YQMultiSelectionBox : public QFrame, public YMultiSelectionBox
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQMultiSelectionBox( YWidget * parent, const string & label );

    /**
     * Destructor.
     **/
    ~YQMultiSelectionBox();

    /**
     * Change the label text.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void setLabel( const string & label );

    /**
     * Add an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void addItem( YItem * item );

    /**
     * Select or deselect an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void selectItem( YItem * item, bool selected = true );

    /**
     * Deselect all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deselectAllItems();

    /**
     * Delete all items.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void deleteAllItems();

    /**
     * Return the the item that currently has the keyboard focus
     * or 0 if no item currently has the keyboard focus.
     *
     * Reimplemented from YMultiSelectionBox.
     **/
    virtual YItem * currentItem();

    /**
     * Set the keyboard focus to the specified item.
     * 0 means clear the keyboard focus.
     *
     * Reimplemented from YMultiSelectionBox.
     **/
    virtual void setCurrentItem( YItem * item );

    /**
     * Set enabled/disabled state.
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


signals:
    /**
     * Emitted upon when an item changes its state.
     **/
    void valueChanged();


protected slots:
    /**
     * Send SelectionChanged event if `opt(`notify) is set.
     **/
    void slotSelected();

    /**
     * Send ValueChanged event if `opt(`notify) is set.
     **/
    void slotValueChanged();

    void slotItemChanged( QTreeWidgetItem*, int );

protected:
    /**
     * Find the counterpart of 'item'. Return 0 if there is none.
     **/
    YQMultiSelectionBoxItem * findItem( YItem * item );


    YQWidgetCaption *	_caption;
    QTreeWidget * 	_qt_listView;
};



class YQMultiSelectionBoxItem: public QTreeWidgetItem
{
public:
    /**
     * Constructor.
     **/
    YQMultiSelectionBoxItem( YQMultiSelectionBox * 	parent,
			     QTreeWidget * 		listView,
			     YItem *			yItem );

    /**
     * Destructor.
     **/
    virtual ~YQMultiSelectionBoxItem() {}

    /**
     * Return the corresponding YItem.
     **/
    YItem * yItem() const { return _yItem; }


protected:

    //
    // Data members
    //

    YItem *			_yItem;
    int 			_serial;
    YQMultiSelectionBox * 	_multiSelectionBox;


    /**
     * The next serial number to use
     **/
    static int _item_count;
};

#endif // YQLabel_h
