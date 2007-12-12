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

  File:	      YQSimplePatchSelector.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQSimplePatchSelector_h
#define YQSimplePatchSelector_h

#include <QColor>

#include "YQPackageSelectorBase.h"


class QPushButton;

class YQPkgPatchFilterView;
class YQPkgPatchList;
class YQWizard;


class YQSimplePatchSelector : public YQPackageSelectorBase
{
    Q_OBJECT

public:

    YQSimplePatchSelector( YWidget * parent, long modeFlags );


protected slots:

    /**
     * User clicked on "Details..." - start the detailed package selection.
     * This will return from UI::RunPkgSelection() with `details .
     **/
    void detailedPackageSelection();

    /**
     * Debugging
     **/
    void debugTrace();


protected:

    // Layout methods - create and layout widgets

    void	basicLayout();
    void	layoutButtons	( QWidget * parent );

    /**
     * Find the wizard in the current dialog, if there is any.
     * Returns 0 if there is none.
     **/
    YQWizard *	findWizard() const;

    /**
     * Establish Qt signal / slot connections.
     **/
    void makeConnections();


    // Data members

    YQPkgPatchFilterView *	_patchFilterView;
    YQPkgPatchList *		_patchList;
    YQWizard *			_wizard;
};



#endif // YQSimplePatchSelector_h
