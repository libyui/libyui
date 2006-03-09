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

  File:	      YQPkgPatchFilterView.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgPatchFilterView_h
#define YQPkgPatchFilterView_h

#include "YQZypp.h"
#include <qvbox.h>


class YQPkgPatchList;
class YQPkgDescriptionView;
class QComboBox;
class QLabel;
class QSplitter;
class QTabWidget;


/**
 * @short Display a list of zypp::Patch objects and ( below ) details about the
 * currently selected YOU patch.
 **/
class YQPkgPatchFilterView : public QVBox
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgPatchFilterView( QWidget * parent );

    /**
     * Destructor
     **/
    virtual ~YQPkgPatchFilterView();


    /**
     * Returns this view's selections list.
     **/
    YQPkgPatchList * youPatchList() const { return _patchList; }

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
     * Fill the YOU patch list with regard to the _patchCategory
     * combo box.
     **/
    void fillPatchList();


protected:


    // Data members

    QSplitter *			_splitter;
    YQPkgPatchList *		_patchList;
    QComboBox *			_patchCategory;
    QTabWidget *		_detailsViews;
    YQPkgDescriptionView *	_descriptionView;
    QLabel *			_totalDownloadSize;
};



#endif // ifndef YQPkgPatchFilterView_h
