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

  File:	      YQSelectionBox.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQSelectionBox_h
#define YQSelectionBox_h

#include <QFrame>
#include <QTimer>

#include "YSelectionBox.h"

class YQWidgetCaption;
class QListWidget;
class QListWidgetItem;


class YQSelectionBox : public QFrame, public YSelectionBox
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQSelectionBox( YWidget * parent, const string & label );

    /**
     * Destructor.
     **/
    virtual ~YQSelectionBox();

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
     * Add multiple items.
     *
     * Reimplemented for efficiency from YSelectionWidget.
     **/
    virtual void addItems( const YItemCollection & itemCollection );

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
     *
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();

    /**
     * Event filter.
     *
     * Reimplemented from QWidget.
     **/
    virtual bool eventFilter( QObject * obj, QEvent * ev );


protected slots:

    /**
     * Notification that an item has been selected.
     * This is only relevant if `opt(`notify ) is set.
     **/
    void slotSelectionChanged();

    /**
     * Notification that an item has been activated (double clicked).
     **/
    void slotActivated( QListWidgetItem * item );

    /**
     * Return after some millseconds delay - collect multiple events.
     * This is only relevant if `opt( `notify ) is set.
     **/
    void returnDelayed();

    /**
     * Return immediately.
     * This is only relevant if `opt( `notify ) is set.
     **/
    void returnImmediately();


protected:

    /**
     * Select an item by index.
     **/
    void selectItem( int index );

    /**
     * Internal addItem() method that will not do expensive operations in batch
     * mode. 
     **/
    void addItem( YItem * item, bool batchMode );
    

    //
    // Data members
    //

    YQWidgetCaption *	_caption;
    QListWidget *	_qt_listWidget;
    QTimer 		_timer;
};

#endif // YQLabel_h
