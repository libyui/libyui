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

  File:	      YQPkgSelectionsFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgSelectionsFilterView_h
#define YQPkgSelectionsFilterView_h

#include <qvbox.h>
#include <zypp/Selection.h>


class YQPkgSelList;


class YQPkgSelectionsFilterView : public QVBox
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgSelectionsFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgSelectionsFilterView();


    /**
     * Returns this view's selections list.
     **/
    YQPkgSelList * selList() const { return _selList; }


protected:


    // Data members

    YQPkgSelList	* _selList;
};



#endif // ifndef YQPkgSelectionsFilterView_h
