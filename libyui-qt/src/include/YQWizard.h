/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						     (c) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:	      YQWizard.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-

#ifndef YQWizard_h
#define YQWizard_h

#include "YWizard.h"
#include "qvbox.h"

class QHBox;
class QPushButton;
class QTabWidget;
class QTextBrowser;
class QWidgetStack;
class QToolButton;

class YQReplacePoint;


class YQWizard : public QVBox, public YWizard
{
    Q_OBJECT

public:
    /**
     * Constructor
     */
    YQWizard( QWidget * 	parent,
	      YWidgetOpt & 	opt,
	      const YCPValue & 	backButtonId,	const YCPString & backButtonLabel,
	      const YCPValue & 	abortButtonId,	const YCPString & abortButtonLabel,
	      const YCPValue & 	nextButtonId,	const YCPString & nextButtonLabel  );

    /**
     * Notification that a child widget has been added.
     * Reimplemented from YContainerWidget.
     **/
    void addChild( YWidget * child );

    /**
     * Minimum size the widget should have to make it look and feel
     * nice.
     * @dim Dimension, either YD_HORIZ or YD_VERT
     */
    long nicesize( YUIDimension dim );

    /**
     * Sets the new size of the widget.
     **/
    void setSize( long newWidth, long newHeight );

    /**
     * Event filter - inherited from QWidget
     **/
    bool eventFilter( QObject * obj, QEvent * ev );

    
public slots:

    /**
     * Adapt the size of the client area (the ReplacePoint(`id(`contents)) to
     * fit in its current space. 
     **/
    void resizeClientArea();

    /**
     * Show the current help text.
     *
     * This is useful only if it is obscured by any wizard steps, but it can
     * safely be called at any time.
     **/
    void showHelp();

    /**
     * Show the current wizard steps, if there are any. If there are none,
     * nothing happens.
     **/
    void showSteps();

    
protected slots:

    /**
     * Internal notification that the "Back" button has been clicked.
     **/
    void backClicked();

    /**
     * Internal notification that the "Abort" button has been clicked.
     **/
    void abortClicked();

    /**
     * Internal notification that the "Next" button has been clicked.
     **/
    void nextClicked();


protected:

    // Layout functions
    
    void layoutStepsPanel();
    void layoutHelpPanel();
    void layoutButtonBox();

    /**
     * Destroy the button box's buttons
     **/
    void destroyButtons();

    /**
     * Add a (left or right) margin common to the help panel.
     **/
    void addHelpMarginColumn( QWidget * parent );
    
    /**
     * Send a wizard event with the specified ID.
     **/
    void sendEvent( YCPValue id );

    
    // Data members

    YCPString	_backButtonLabel;
    YCPString	_abortButtonLabel;
    YCPString	_nextButtonLabel;

    QColor	_bg;

    QWidgetStack *	_sideBar;
    QVBox *		    _stepsPanel;
    QPushButton	*		_helpButton;
    QVBox *		    _helpPanel;
    QTextBrowser *		_helpBrowser;
    QPushButton *		_stepsButton;

    QVBox *		_workArea;
    QVBox *		    _clientArea;
    YQReplacePoint *	        _contentsReplacePoint;
    QHBox *		    _buttonBox;
    QPushButton *	        _abortButton;
    QPushButton *	        _backButton;
    QPushButton *	        _nextButton;
};

#endif // YQWizard_h
