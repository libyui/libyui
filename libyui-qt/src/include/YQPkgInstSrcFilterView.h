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

  File:	      YQPkgInstSrcFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgInstSrcFilterView_h
#define YQPkgInstSrcFilterView_h

#include <qvbox.h>


class YQPkgInstSrcList;


class YQPkgInstSrcFilterView : public QVBox
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgInstSrcFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgInstSrcFilterView();


    /**
     * Returns this view's installation source list.
     **/
    YQPkgInstSrcList * instSrcList() const { return _instSrcList; }


protected:


    // Data members

    YQPkgInstSrcList * _instSrcList;
};



#endif // ifndef YQPkgInstSrcFilterView_h
