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
#include "qpixmap.h"

class QGridLayout;
class QHBox;
class QLabel;
class QPushButton;
class QTextBrowser;
class QToolButton;
class QWidgetStack;

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

    /**
     * Set a dialog icon. 0 is a valid value - it clears the current icon.
     **/
    void setDialogIcon( const char * iconName );

    /**
     * Set a dialog heading. 0 is a valid value - it clears the old text.
     **/
    void setDialogHeading( const QString & headingText );

    
public slots:


    /**
     * Set the help text. 0 is a valid value - it clears the old text.
     **/
    void setHelpText( const QString & helpText );

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

    /**
     * Set a widget's background pixmap to a gradient pixmap and set the
     * widget's height (fixed) to that pixmap's height.
     **/
    static void setGradient( QWidget * widget, const QPixmap & pixmap );

    /**
     * Set a widget's background to the lower portion (the bottom
     * 'croppedHeight' pixels) of a pixmap and set the widget's height (fixed)
     * to that 'croppedHeight'.
     **/
    static void setBottomCroppedGradient( QWidget * widget,
					  const QPixmap & pixmap,
					  int croppedHeight );

    /**
     * Bottom-crop a pixmap: Return a pixmap with the bottom 'croppedHeight'
     * pixels. 
     **/
    static QPixmap bottomCropPixmap( const QPixmap & pixmap, int croppedHeight );

    /**
     * Return the color of pixel( x, y ) of a pixmap.
     * This is a _very_ expensive operation!
     **/
    static QColor pixelColor( const QPixmap & pixmap, int x, int y );
    
    
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

    void layoutTitleBar	( QWidget * parent );
    void layoutSideBar	( QWidget * parent );
    void layoutStepsPanel();
    void layoutHelpPanel();
    void layoutWorkArea	( QHBox * parentHBox );
    void layoutClientArea( QWidget * parent );
    void layoutButtonBox();

    /**
     * Load gradient pixmaps
     **/
    void YQWizard::loadGradientPixmaps();


    /**
     * Destroy the button box's buttons
     **/
    void destroyButtons();

    /**
     * Add a (left or right) margin of the specified width to a widget,
     * consisting of a fixed height top gradient , a flexible center part (in
     * the gradient center color) and a fixed height bottom gradient.
     *
     * The bottom gradient widget is returned as a reference for other
     * background pixmaps.
     **/
    QWidget * addGradientColumn( QWidget * parent, int width = 8 );

    /**
     * Add a grid layout to a parent widget that centers its (only) child at
     * the parent's bottom, maintaining the specified margin from all edges
     * (including the bottom).
     *
     * Returns the newly created grid layout.
     **/
    QGridLayout * centerAtBottom( QWidget * parent, QWidget * child, int margin );

    /**
     * Send a wizard event with the specified ID.
     **/
    void sendEvent( YCPValue id );


    // Data members

    YCPString	_backButtonLabel;
    YCPString	_abortButtonLabel;
    YCPString	_nextButtonLabel;

    
    QPixmap	_titleBarGradientPixmap;
    QPixmap	_topGradientPixmap;
    QColor	_gradientCenterColor;
    QPixmap	_bottomGradientPixmap;


    QWidgetStack *	_sideBar;
    QVBox *		    _stepsPanel;
    QPushButton	*		_helpButton;
    QHBox *		    _helpPanel;
    QTextBrowser *		_helpBrowser;
    QPushButton *		_stepsButton;

    QVBox *		_clientArea;
    QLabel *		    _dialogIcon;
    QLabel *		    _dialogHeading;
    YQReplacePoint *	    _contentsReplacePoint;
    QHBox *		_buttonBox;
    QPushButton *	    _abortButton;
    QPushButton *	    _backButton;
    QPushButton *	    _nextButton;
};

#endif // YQWizard_h
