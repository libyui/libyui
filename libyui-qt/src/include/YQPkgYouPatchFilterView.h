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

  File:	      YQPkgYouPatchFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgYouPatchFilterView_h
#define YQPkgYouPatchFilterView_h

#include <qvbox.h>
#include <y2pm/PMSelection.h>


class YQPkgYouPatchList;
class YQPkgDescriptionView;
class QCheckBox;
class QLabel;
class QSplitter;
class QTabWidget;


/**
 * @short Display a list of PMYouPatch objects and (below) details about the
 * currently selected YOU patch.
 **/
class YQPkgYouPatchFilterView : public QVBox
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgYouPatchFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgYouPatchFilterView();


    /**
     * Returns this view's selections list.
     **/
    YQPkgYouPatchList * youPatchList() const { return _youPatchList; }

    /**
     * Reimplemented from QWidget:
     * Reserve a reasonable amount of space.
     **/
    virtual QSize sizeHint() const;

    
public slots:

    /**
     * Update the "total download size" field.
     **/
    void updateTotalDownloadSize();

    
protected slots:

    /**
     * Fill the YOU patch list with regard to the _showInstalledPatches
     * check box.
     **/
    void fillPatchList();


protected:


    // Data members

    QSplitter *			_splitter;
    YQPkgYouPatchList *		_youPatchList;
    QCheckBox *			_showInstalledPatches;
    QTabWidget *		_detailsViews;
    YQPkgDescriptionView *	_descriptionView;
    QLabel *			_totalDownloadSize;
};



#endif // ifndef YQPkgYouPatchFilterView_h
