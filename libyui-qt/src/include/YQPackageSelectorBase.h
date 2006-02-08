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

  File:	      YQPackageSelectorBase.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQPackageSelectorBase_h
#define YQPackageSelectorBase_h

#include <qvbox.h>
#include "YPackageSelector.h"


class QY2ComboTabWidget;

class YQPkgConflictDialog;
class YQPkgDiskUsageList;


/**
 * Abstract base class for package selectors.
 **/
class YQPackageSelectorBase : public QVBox, public YPackageSelector
{
    Q_OBJECT

protected:

    /**
     * Constructor.
     *
     * Will initialize package and selection managers and create conflict
     * dialogs. 
     **/
    YQPackageSelectorBase( QWidget * parent, const YWidgetOpt & opt );

    
public:
    
    /**
     * Inherited from YWidget: Sets the enabled state of the
     * widget. All new widgets are enabled per definition. Only
     * enabled widgets can take user input.
     **/
    void setEnabling( bool enabled );

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     **/
    long nicesize( YUIDimension dim );

    /**
     * Sets the new size of the widget.
     **/
    void setSize( long newWidth, long newHeight );

    /**
     * Accept the keyboard focus.
     **/
    virtual bool setKeyboardFocus();


public slots:

    /**
     * Resolve package dependencies (unconditionally).
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
     int resolvePackageDependencies();

    /**
     * Check for disk overflow and post a warning dialog if necessary.
     * The user can choose to override this warning.
     *
     * Returns QDialog::Accepted if no warning is necessary or if the user
     * wishes to override the warning, QDialog::Rejected otherwise.
     **/
    int checkDiskUsage();

    /**
     * Display a list of automatically selected packages
     * (excluding packages contained in any selections that are to be installed)
     **/
    void showAutoPkgList();

    /**
     * Close processing and abandon changes
     **/
    void reject();

    /**
     * Close processing and accept changes
     **/
    void accept();

    /**
     * Inform user about a feature that is not implemented yet.
     * This should NEVER show up in the final version.
     **/
    void notImplemented();

    /**
     * Post beta version warning dialog
     **/
    void postBetaWarning();

    
signals:

    /**
     * Emitted when package resolving is started.
     * This can be used for some kind of "busy display".
     **/
    void resolvingStarted();

    /**
     * Emitted when package resolving is finished.
     **/
    void resolvingFinished();

    
protected:

    /**
     * Event handler for keyboard input - for debugging and testing.
     *
     * Reimplemented from QWidget.
     */
    virtual void keyPressEvent( QKeyEvent * ev );


    // Data members

    bool				_youMode;
    bool				_showChangesDialog;
    YQPkgConflictDialog *		_pkgConflictDialog;
    YQPkgDiskUsageList *		_diskUsageList;
};



#endif // YQPackageSelectorBase_h
