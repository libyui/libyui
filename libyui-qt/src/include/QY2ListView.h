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

  File:	      QY2ListView.h

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt widget - it can be used independently of YaST2.
  
/-*/

// -*- c++ -*-


#ifndef QY2ListView_h
#define QY2ListView_h

#include <qlistview.h>
#include <qpoint.h>
#include <qcolor.h>
#include <vector>


class QY2ListViewItem;	// shorter name than "QY2ListViewItem"


/**
 * @short Enhanced QListView
 **/
class QY2ListView : public QListView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    QY2ListView( QWidget *parent );

    /**
     * Destructor
     **/
    virtual ~QY2ListView();


public slots:

    /**
     * Select a list entry (if there is any).
     * Usually this will be the first list entry, but don't rely on that - this
     * might change without notice. Emits signal selectionChanged().
     **/
    virtual void selectSomething();

    /**
     * Reimplemented from QListView:
     * Adjust header sizes after clearing contents.
     **/
    virtual void clear();

    /**
     * Update the status display of all toplevel list entries:
     * Call QY2ListViewItem::updateStatus() for each item.
     * This is an expensive operation.
     **/
    void updateToplevelItemStates();

    /**
     * Update the status display of all toplevel list entries:
     * Call QY2ListViewItem::updateData() for each item.
     * This is an expensive operation.
     **/
    void updateToplevelItemData();

    /**
     * Save the current column widths.
     **/
    void saveColumnWidths();

    /**
     * Restore the column widths to what was saved previously with
     * saveColumnWidths().   
     **/
    void restoreColumnWidths();

    
signals:
    
    /**
     * Emitted for mouse clicks on a package
     **/
    void columnClicked		( int			button,
				  QListViewItem *	item,
				  int			col,
				  const QPoint &	pos );

    /**
     * Emitted for mouse double clicks on a package
     **/
    void columnDoubleClicked	( int			button,
				  QListViewItem *	item,
				  int			col,
				  const QPoint &	pos );

public:

    /**
     * Returns the next free serial number for items that want to be ordered in
     * insertion sequence. 
     **/
    int nextSerial() { return _nextSerial++; }

    
protected slots:

    /**
     * Internal: Handle manual column resize.
     * Save the user's preferred sizes so they don't get overwritten each time
     * the list is cleared and filled with new contents.
     **/
    void columnWidthChanged( int col, int oldSize, int newSize );

protected:

    /**
     * Handle mouse clicks.
     * Reimplemented from QScrollView.
     **/
    virtual void contentsMousePressEvent( QMouseEvent * e );

    /**
     * Handle mouse clicks.
     * Reimplemented from QScrollView.
     **/
    virtual void contentsMouseReleaseEvent( QMouseEvent* );

    /**
     * Handle mouse clicks.
     * Reimplemented from QScrollView.
     **/
    virtual void contentsMouseDoubleClickEvent( QMouseEvent* );


    
    // Data members

    QListViewItem *	_mousePressedItem;
    int			_mousePressedCol;
    int			_mousePressedButton;
    
    std::vector<int> 	_savedColumnWidth;
    int			_nextSerial;
};



/**
 * Enhanced QListViewItem
 **/
class QY2ListViewItem: public QListViewItem
{
public:

    /**
     * Constructor for toplevel items.
     *
     * 'sortByInsertionSequence' indicates if this item keeps the insertion
     * order (true) or leaves sorting to the user (false - sort-by-click on
     * column headers).
     **/
    QY2ListViewItem( QY2ListView * 		parentListView,
		     const QString &		text = QString::null,
		     bool 			sortByInsertionSequence = false );


    /**
     * Constructor for deeper level items.
     *
     * 'sortByInsertionSequence' indicates if this item keeps the insertion
     * order (true) or leaves sorting to the user (false - sort-by-click on
     * column headers).
     **/
    QY2ListViewItem( QListViewItem * 		parentItem,
		     const QString &		text = QString::null,
		     bool 			sortByInsertionSequence = false );

    /**
     * Destructor
     **/
    virtual ~QY2ListViewItem();

    /**
     * Update this item's status.
     * Triggered by QY2ListView::updateAllItemStates().
     * Derived classes should overwrite this.
     * This default implementation does nothing.
     **/
    virtual void updateStatus() {}

    /**
     * Update this item's data completely.
     * Triggered by QY2ListView::updateAllItemData().
     * Derived classes should overwrite this.
     * This default implementation does nothing.
     **/
    virtual void updateData() {}

    /**
     * Comparison function used for sorting the list.
     * Returns:
     * -1 if this <  other
     *	0 if this == other
     * +1 if this >  other
     *
     * Reimplemented from QListViewItem
     **/
    virtual int compare( QListViewItem *	other,
			 int			col,
			 bool			ascending ) const;

    /**
     * Return this item's serial number.
     * Useful for comparison functions that order by insertion sequence.
     **/
    int serial() const { return _serial; }

    /**
     * Returns true if this item sorts itself by insertion sequence.
     **/
    bool sortByInsertionSequence() const { return _sortByInsertionSequence; }

    /**
     * Set sort policy: 'true' to sort by insertion sequence, 'false' for user
     * sort-by-click on column headers.
     **/
    void setSortByInsertionSequence( bool doit )
	{ _sortByInsertionSequence = doit; }

    /**
     * Set the text foreground color for all columns.
     * For more specific purposes reimiplement paintCell().
     **/
    void setTextColor( const QColor & col )
	{ _textColor = col; }
    
    /**
     * Set the text background color for all columns.
     * For more specific purposes reimiplement paintCell().
     **/
    void setBackgroundColor( const QColor & col )
	{ _backgroundColor = col; }

    
protected:

    /**
     * Paint method. Reimplemented from @ref QListViewItem so different
     * colors can be used.
     *
     * Reimplemented from QListViewItem.
     **/
    virtual void paintCell( QPainter *		painter,
			    const QColorGroup &	colorGroup,
			    int			column,
			    int			width,
			    int			alignment );

    // Data members
    
    int		_serial;
    bool	_sortByInsertionSequence;
    
    QColor	_textColor;
    QColor	_backgroundColor;
};



/**
 * Enhanced QCheckListItem
 **/
class QY2CheckListItem: public QCheckListItem
{
public:

    /**
     * Constructor for toplevel items.
     *
     * 'sortByInsertionSequence' indicates if this item keeps the insertion
     * order (true) or leaves sorting to the user (false - sort-by-click on
     * column headers).
     **/
    QY2CheckListItem( QY2ListView * 		parentListView,
		      const QString &		text,
		      QCheckListItem::Type	type,
		      bool 			sortByInsertionSequence = false );


    /**
     * Constructor for deeper level items.
     *
     * 'sortByInsertionSequence' indicates if this item keeps the insertion
     * order (true) or leaves sorting to the user (false - sort-by-click on
     * column headers).
     **/
    QY2CheckListItem( QListViewItem * 		parentItem,
		      const QString &		text,
		      QCheckListItem::Type	type,
		      bool 			sortByInsertionSequence = false );


    /**
     * Constructor for deeper level items for QCheckListItem parents.
     *
     * 'sortByInsertionSequence' indicates if this item keeps the insertion
     * order (true) or leaves sorting to the user (false - sort-by-click on
     * column headers).
     **/
    QY2CheckListItem( QCheckListItem * 		parentItem,
		      const QString &		text,
		      QCheckListItem::Type	type,
		      bool 			sortByInsertionSequence = false );

    /**
     * Destructor
     **/
    virtual ~QY2CheckListItem();

    /**
     * Update this item's status.
     * Triggered by QY2ListView::updateAllItemStates().
     * Derived classes should overwrite this.
     * This default implementation does nothing.
     **/
    virtual void updateStatus() {}

    /**
     * Update this item's data completely.
     * Triggered by QY2ListView::updateAllItemData().
     * Derived classes should overwrite this.
     * This default implementation does nothing.
     **/
    virtual void updateData() {}

    /**
     * Comparison function used for sorting the list.
     * Returns:
     * -1 if this <  other
     *	0 if this == other
     * +1 if this >  other
     *
     * Reimplemented from QListViewItem
     **/
    virtual int compare( QListViewItem *	other,
			 int			col,
			 bool			ascending ) const;

    /**
     * Return this item's serial number.
     * Useful for comparison functions that order by insertion sequence.
     **/
    int serial() const { return _serial; }

    /**
     * Returns true if this item sorts itself by insertion sequence.
     **/
    bool sortByInsertionSequence() const { return _sortByInsertionSequence; }

    /**
     * Set sort policy: 'true' to sort by insertion sequence, 'false' for user
     * sort-by-click on column headers.
     **/
    void setSortByInsertionSequence( bool doit )
	{ _sortByInsertionSequence = doit; }

    /**
     * Set the text foreground color for all columns.
     * For more specific purposes reimiplement paintCell().
     **/
    void setTextColor( const QColor & col )
	{ _textColor = col; }
    
    /**
     * Set the text background color for all columns.
     * For more specific purposes reimiplement paintCell().
     **/
    void setBackgroundColor( const QColor & col )
	{ _backgroundColor = col; }
    
    
protected:

    /**
     * Paint method. Reimplemented from @ref QListViewItem so different
     * colors can be used.
     *
     * Reimplemented from QListViewItem.
     **/
    virtual void paintCell( QPainter *		painter,
			    const QColorGroup &	colorGroup,
			    int			column,
			    int			width,
			    int			alignment );

    // Data members

    int		_serial;
    bool	_sortByInsertionSequence;
    
    QColor	_textColor;
    QColor	_backgroundColor;
};


#endif // ifndef QY2ListView_h
