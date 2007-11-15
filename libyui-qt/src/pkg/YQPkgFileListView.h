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

  File:	      YQPkgFileListView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQPkgFileListView_h
#define YQPkgFileListView_h

#include "YQPkgGenericDetailsView.h"


using std::list;
using std::string;


/**
 * @short Display a pkg's file list
 **/
class YQPkgFileListView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgFileListView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgFileListView();

    /**
     * Show details for the specified package:
     * In this case the package description.
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showDetails( ZyppSel selectable );

protected:

    /**
     * Format a file list in HTML
     **/
    QString formatFileList( const list<string> & fileList ) const;
};


#endif // ifndef YQPkgFileListView_h
