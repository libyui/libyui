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

  File:	      YQPkgInstSrcList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgInstSrcList_h
#define YQPkgInstSrcList_h

#include <QY2ListView.h>
#include <y2pm/PMPackage.h>
#include <y2pm/InstSrcManager.h>


class YQPkgInstSrcListItem;


typedef enum YQPkgInstSrcStatus
{
    YQPkgInstSrcOn,
    YQPkgInstSrcOff,
    YQPkgInstSRcDisabled
};


/**
 * @short Display a list of PMSelection objects.
 **/
class YQPkgInstSrcList : public QY2ListView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgInstSrcList( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgInstSrcList();


public slots:

    /**
     * Filter according to the view's rules and current selection.
     * Emits those signals:
     *    filterStart()
     *    filterMatch() for each pkg that matches the filter
     *    filterFinished()
     **/
    void filter();

    /**
     * Same as filter(), but only if this widget is currently visible.
     **/
    void filterIfVisible();

    /**
     * Add an inst source to the list. Connect a filter's filterMatch() signal
     * to this slot. Remember to connect filterStart() to clear()
     * (inherited from QListView).
     **/
    void addInstSrc( InstSrcManager::ISrcId instSrcId );
    
    /**
     * Emit an updatePackages() signal.
     **/
    void sendUpdatePackages() { emit updatePackages(); }

    
public:

    // Column numbers

    int statusCol()		const	{ return _statusCol;		}
    int nameCol()		const	{ return _nameCol;		}
    int urlCol()		const	{ return _urlCol;		}

    
    /**
     * Returns the currently selected item or 0 if there is none.
     **/
    YQPkgInstSrcListItem * selection() const;


signals:

    /**
     * Emitted when it's time to update displayed package information,
     * e.g., package states.
     **/
    void updatePackages();

    /**
     * Emitted when the filtering starts. Use this to clear package lists
     * etc. prior to adding new entries.
     **/
    void filterStart();

    /**
     * Emitted during filtering for each pkg that matches the filter.
     **/
    void filterMatch( PMPackagePtr pkg );

    /**
     * Emitted when filtering is finished.
     **/
    void filterFinished();


protected slots:

    /**
     * Fill the language list.
     **/
    void fillList();


private:

    //
    // Data members
    //

    int		_statusCol;
    int		_nameCol;
    int		_urlCol;
    
};



class YQPkgInstSrcListItem: public QY2ListViewItem
{
public:

    /**
     * Constructor
     **/
    YQPkgInstSrcListItem( YQPkgInstSrcList *parentList, InstSrcManager::ISrcId instSrcId );

    /**
     * Destructor
     **/
    virtual ~YQPkgInstSrcListItem();

    /**
     * Returns the original inst source ID within the package manager backend.
     **/
    InstSrcManager::ISrcId instSrcId() { return _instSrcId; }

    /**
     * Returns the parent list
     **/
    const YQPkgInstSrcList * instSrcList() const { return _instSrcList; }

    /**
     * Returns the original object within the package manager backend.
     **/
    const InstSrcManager::ISrcId constInstSrcId() const { return _instSrcId; }

    /**
     * Returns the status of this inst source
     **/
    YQPkgInstSrcStatus status() const { return _status; }
    
    /**
     * Set the inst source status.
     *
     * Emits updatePackages signal for each status change.
     **/
    virtual void setStatus( YQPkgInstSrcStatus newStatus );


    // Columns

    int statusCol()	const	{ return _instSrcList->statusCol();	}
    int nameCol()	const	{ return _instSrcList->nameCol();	}
    int urlCol()	const 	{ return _instSrcList->urlCol(); 	}


protected:

    // Data members

    YQPkgInstSrcList *		_instSrcList;
    InstSrcManager::ISrcId	_instSrcId;
    YQPkgInstSrcStatus		_status;
};


#endif // ifndef YQPkgInstSrcList_h
