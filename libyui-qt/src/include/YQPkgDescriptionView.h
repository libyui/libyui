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

  File:	      YQPkgDescriptionView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgDescriptionView_h
#define YQPkgDescriptionView_h

#include "YQPkgGenericDetailsView.h"


class YQPkgDescriptionView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgDescriptionView( QWidget *parent );

    
    /**
     * Destructor
     **/
    virtual ~YQPkgDescriptionView();


protected:

    /**
     * Show details for the specified package:
     * In this case the package description.
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showPkgDetails( PMPackagePtr pkg );
};


#endif // ifndef YQPkgDescriptionView_h
