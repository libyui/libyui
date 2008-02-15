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

  File:	      YQPkgChangeLogView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgChangeLogView_h
#define YQPkgChangeLogView_h

#include <zypp/Changelog.h>
#include "YQPkgGenericDetailsView.h"


using std::list;
using std::string;


/**
 * @short Display a pkg's file list
 **/
class YQPkgChangeLogView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgChangeLogView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgChangeLogView();

    /**
     * Show details for the specified package:
     * In this case the package description.
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showDetails( ZyppSel selectable );

protected:

    /**
     * Format a change log list in HTML
     **/
    QString changeLogTable( const zypp::Changelog & changeLog ) const;
};


#endif // ifndef YQPkgChangeLogView_h
