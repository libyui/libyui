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

  File:	      YQPkgLangFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgLangFilterView_h
#define YQPkgLangFilterView_h

#include <qvbox.h>
#include <y2pm/PMSelection.h>


class YQPkgLangSelList;


class YQPkgLangFilterView : public QVBox
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgLangFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgLangFilterView();


    /**
     * Returns this view's language selections list.
     **/
    YQPkgLangSelList * langSelList() const { return _langSelList; }


protected:


    // Data members

    YQPkgLangSelList	* _langSelList;
};



#endif // ifndef YQPkgLangFilterView_h
