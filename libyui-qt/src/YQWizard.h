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


#ifndef YQWizard_h
#define YQWizard_h

#include <qvbox.h>
#include <qpixmap.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qdict.h>
#include "QY2ListView.h"
#include <YWizard.h>
#include "YQWizardButton.h"

#include <string>
#include <vector>

class QGridLayout;
class QHBox;
class QLabel;
class QMenuBar;
class QPopupMenu;
class QPushButton;
class QSpacerItem;
class QTextBrowser;
class QToolButton;
class QWidgetStack;
class YReplacePoint;

class YQAlignment;
class YQReplacePoint;
class QY2ListView;


class YQWizard : public QVBox, public YWizard
{
    Q_OBJECT

protected:
    class Step;
    class TreeItem;

public:
    /**
     * Constructor.
     **/
    YQWizard( YWidget * 	parent,
	      const string &	backButtonLabel,
	      const string & 	abortButtonLabel,
	      const string & 	nextButtonLabel,
	      YWizardMode	wizardMode = YWizardMode_Standard );

    /**
     * Destructor.
     **/
    virtual ~YQWizard();

    /**
     * Returns a descriptive label of this dialog instance for debugging.
     *
     * Reimplemented from YWidget.
     **/
    virtual string debugLabel();

    enum Direction { Forward, Backward };

    /**
     * Returns the current direction of wizard operations - going forward or
     * going backward. This can be used to maintain a consistent direction when
     * assigning default buttons to a dialog.
     **/
    Direction direction() const { return _direction; }

    //
    // Wizard basics
    //

    /**
     * Return internal widgets.
     *
     * Implemented from YWizard.
     **/
    virtual YQWizardButton * backButton()  const { return _backButton;  }
    virtual YQWizardButton * abortButton() const { return _abortButton; }
    virtual YQWizardButton * nextButton()  const { return _nextButton;  }

    virtual YReplacePoint * contentsReplacePoint() const { return _contentsReplacePoint; }

    /**
     * Set the label of one of the wizard buttons (backButton(), abortButton(),
     * nextButton() ) if that button is non-null. 
     *
     * Implemented from YWizard.
     **/
    virtual void setButtonLabel( YPushButton * button, const string & newLabel );
    
    /**
     * Set the help text.
     *
     * Implemented from YWizard.
     **/
    virtual void setHelpText( const string & helpText );

    /**
     * Set the dialog icon. An empty icon name clears the current icon.
     *
     * Implemented from YWizard.
     **/
    virtual void setDialogIcon( const string & iconName );

    /**
     * Set the dialog heading.
     *
     * Implemented from YWizard.
     **/
    virtual void setDialogHeading( const string & headingText );


    //
    // Steps handling
    //

    /**
     * Add a step for the steps panel on the side bar.
     * This only adds the step to the internal list of steps.
     * The display is only updated upon calling updateSteps().
     *
     * Implemented from YWizard.
     **/
    virtual void addStep( const string & text, const string & id );

    /**
     * Add a step heading for the steps panel on the side bar.
     * This only adds the heading to the internal list of steps.
     * The display is only updated upon calling updateSteps().
     *
     * Implemented from YWizard.
     **/
    virtual void addStepHeading( const string & text );

    /**
     * Delete all steps and step headings from the internal lists.
     * The display is only updated upon calling updateSteps().
     *
     * Implemented from YWizard.
     **/
    virtual void deleteSteps();

    /**
     * Set the current step. This also triggers updateSteps() if necessary.
     *
     * Implemented from YWizard.
     **/
    virtual void setCurrentStep( const string & id );

    /**
     * Update the steps display: Reflect the internal steps and heading lists
     * in the layout.
     *
     * Implemented from YWizard.
     **/
    virtual void updateSteps();


    //
    // Tree handling
    //

    /**
     * Add a tree item. If "parentID" is an empty string, it will be a root
     * item. 'text' is the text that will be displayed in the tree, 'id' the ID
     * with which this newly created item can be referenced - and that will be
     * returned when the user clicks on a tree item.
     *
     * Implemented from YWizard.
     **/
    virtual void addTreeItem( const string & parentID,
			      const string & text,
			      const string & id	);

    /**
     * Select the tree item with the specified ID, if such an item exists.
     *
     * Implemented from YWizard.
     **/
    virtual void selectTreeItem( const string & id );

    /**
     * Returns the current tree selection or an empty string if nothing is
     * selected or there is no tree.
     *
     * Implemented from YWizard.
     **/
    virtual string currentTreeSelection();

    /**
     * Delete all tree items.
     *
     * Implemented from YWizard.
     **/
    virtual void deleteTreeItems();


    //
    // Menu handling
    //

    /**
     * Add a menu to the menu bar. If the menu bar is not visible yet, it will
     * be made visible. 'text' is the user-visible text for the menu bar
     * (including keyboard shortcuts marked with '&'), 'id' is the menu ID for
     * later addMenuEntry() etc. calls.
     *
     * Implemented from YWizard.
     **/
    virtual void addMenu( const string & text,
			  const string & id );

    /**
     * Add a submenu to the menu with ID 'parentMenuID'.
     *
     * Implemented from YWizard.
     **/
    virtual void addSubMenu( const string & parentMenuID,
			     const string & text,
			     const string & id );

    /**
     * Add a menu entry to the menu with ID 'parentMenuID'. 'id' is what will
     * be returned by UI::UserInput() etc. when a user activates this menu entry.
     *
     * Implemented from YWizard.
     **/
    virtual void addMenuEntry( const string & parentMenuID,
			       const string & text,
			       const string & id );

    /**
     * Add a menu separator to a menu.
     *
     * Implemented from YWizard.
     **/
    virtual void addMenuSeparator( const string & parentMenuID );

    /**
     * Delete all menus and hide the menu bar.
     *
     * Implemented from YWizard.
     **/
    virtual void deleteMenus();


    //
    // Misc
    //

    /**
     * Show a "Release Notes" button above the "Help" button in the steps panel
     * with the specified label that will return the specified id to
     * UI::UserInput() when clicked.
     *
     * The button (or the wizard) will assume ownership of the id and delete it
     * in the destructor.
     *
     * Implemented from YWizard.
     **/
    virtual void showReleaseNotesButton( const string & label,
					 const string & id );

    /**
     * Hide an existing "Release Notes" button.
     *
     * Implemented from YWizard.
     **/
    virtual void hideReleaseNotesButton();

    /**
     * Retranslate internal buttons that are not accessible from the outside:
     * - [Help]
     * - [Steps]
     * - [Tree]
     *
     * Implemented from YWizard.
     **/
    virtual void retranslateInternalButtons();

    /**
     * Event filter.
     *
     * Reimplemented from QWidget.
     **/
    virtual bool eventFilter( QObject * obj, QEvent * ev );

    //
    // Geometry management
    //

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );


signals:

    /**
     * Emitted when the "Back" or "Cancel" button is clicked.
     **/
    void backClicked();

    /**
     * Emitted when the "Abort" button is clicked.
     **/
    void abortClicked();

    /**
     * Emitted when the "Next" or "OK" button is clicked.
     *
     * Notice: As long as this signal is connected, the wizard will no longer
     * send button events to the UI. Rather, the connected QObject has to take
     * care to propagate those events.
     * This is used in YQPatternSelector, for example.
     **/
    void nextClicked();


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

    /**
     * Show the current selection tree in the side panel, if there is any. If
     * there is none, nothing happens.
     **/
    void showTree();

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
     * If pixmap is null defaultColor is returned.
     * This is a _very_ expensive operation!
     **/
    static QColor pixelColor( const QPixmap & pixmap, int x, int y, const QColor & defaultColor );


protected slots:

    /**
     * Internal notification that the "Back" button has been clicked.
     **/
    void slotBackClicked();

    /**
     * Internal notification that the "Abort" button has been clicked.
     **/
    void slotAbortClicked();

    /**
     * Internal notification that the "Next" button has been clicked.
     **/
    void slotNextClicked();

    /**
     * Propagate button clicked event of release notes button to the
     * application.
     **/
    void releaseNotesClicked();

    /**
     * Internal notification that [Space] or [Return] has been pressed on a
     * tree item.
     * If the item has an ID, that ID will be returned to UI::UserInput().
     **/
    void sendTreeEvent( QListViewItem * item );

    /**
     * Internal notification that the tree selection has changed.
     *
     * If the currently selected item has an ID, that ID will be returned to
     * UI::UserInput().
     **/
    void treeSelectionChanged();

    /**
     * Internal notification that a menu item with numeric ID 'numID' has been
     * activated.
     **/
    void sendMenuEvent( int numID );


protected:

    // Layout functions

    void layoutTitleBar		( QWidget * parent );
    void layoutSideBar		( QWidget * parent );
    void layoutSideBarButtonBox	( QWidget * parent, QPushButton * button );
    void layoutStepsPanel();
    void layoutHelpPanel();
    void layoutTreePanel();
    void layoutWorkArea	( QHBox * parentHBox );
    void layoutClientArea( QWidget * parent );
    void layoutButtonBox( QWidget * parent );


    /**
     * Load gradient pixmaps
     **/
    void loadGradientPixmaps();

    /**
     * Load step status icons
     **/
    void loadStepsIcons();

    /**
     * Destroy the button box's buttons
     **/
    void destroyButtons();

    /**
     * Update all step - use appropriate icons and colors
     **/
    void updateStepStates();

    /**
     * Add a (left or right) margin of the specified width to a widget,
     * consisting of a fixed height top gradient , a flexible center part (in
     * the gradient center color) and a fixed height bottom gradient.
     *
     * The bottom gradient widget is returned as a reference for other
     * background pixmaps.
     **/
    void addGradientColumn( QWidget * parent, int width = 8 );

    /**
     * Send a wizard event with the specified ID.
     **/
    void sendEvent( const string & id );

    /**
     * Returns 'true' if the application is running on a high-color display,
     * i.e., on an X visual with more than 8 bit depth.
     **/
    bool highColorDisplay() const;

    /**
     * Notification that a signal is being connected.
     *
     * Reimplemented from QObject.
     **/
    void connectNotify ( const char * signal );

    /**
     * Notification that a signal is being disconnected.
     *
     * Reimplemented from QObject.
     **/
    void disconnectNotify ( const char * signal );

    /**
     * Set a button's label.
     **/
    void setButtonLabel( YQWizardButton * button, const QString & newLabel );

    /**
     * Enable or disable a button.
     **/
    void enableButton( YQWizardButton * button, bool enabled );

    /**
     * Set the keyboard focus to a button.
     **/
    void setButtonFocus( YQWizardButton * button );

    /**
     * Set text color and status icon for one wizard step
     **/
    void setStepStatus( YQWizard::Step * step, const QPixmap & icon, const QColor & color );

    /**
     * Find a step with the specified ID. Returns 0 if there is no such step.
     **/
    YQWizard::Step * findStep( const QString & id );

    /**
     * Find a tree item with the specified ID. Tree items without IDs cannot be
     * found at all.
     * Returns the item or 0 if no such item found.
     **/
    YQWizard::TreeItem * findTreeItem( const string & id );


    //
    // Data members
    //

    string	_backButtonLabel;
    string	_abortButtonLabel;
    string	_nextButtonLabel;
    
    bool	_stepsEnabled;
    bool	_treeEnabled;
    bool	_protectNextButton;
    bool	_stepsDirty;
    bool	_sendButtonEvents;
    Direction	_direction;

    QPixmap	_titleBarGradientPixmap;
    QPixmap	_topGradientPixmap;
    QColor	_gradientCenterColor;
    QColor	_gradientTopColor;
    QPixmap	_bottomGradientPixmap;

    QPixmap	_stepCurrentIcon;
    QPixmap	_stepToDoIcon;
    QPixmap	_stepDoneIcon;

    QColor	_stepCurrentColor;
    QColor	_stepToDoColor;
    QColor	_stepDoneColor;

    QString	_currentStepID;


    QWidgetStack *	_sideBar;
    QVBox *		    _stepsPanel;
    QVBox *			_stepsBox;
    QGridLayout *		    _stepsGrid;
    QPushButton *		_releaseNotesButton;
    string			    _releaseNotesButtonId;
    QPushButton *		_helpButton;
    QHBox *		    _helpPanel;
    QTextBrowser *		_helpBrowser;
    QPushButton *		_stepsButton;
    QPushButton *		_treeButton;
    QHBox *		    _treePanel;
    QY2ListView *		_tree;

    QVBox *		_clientArea;
    QWidget *		    _menuBarBox;
    QMenuBar *		        _menuBar;
    QLabel *		    _dialogIcon;
    QLabel *		    _dialogHeading;
    YQAlignment *    	    _contents;
    YQWizardButton *	    _backButton;
    QSpacerItem *	    _backButtonSpacer;
    YQWizardButton *	    _abortButton;
    YQWizardButton *	    _nextButton;
    YReplacePoint *	_contentsReplacePoint;

    QPtrList<YQWizard::Step> 	_stepsList;
    QDict<YQWizard::Step>	_stepsIDs;
    QDict<YQWizard::TreeItem>	_treeIDs;
    QDict<QPopupMenu>		_menuIDs;
    vector<string>		_menuEntryIDs;


protected:

    /**
     * Helper class to represent a wizard step internally
     **/
    class Step
    {
    public:

	Step( const QString & name = "", const QString & id = "" )
	    : _name( name )
	    , _statusLabel( 0 )
	    , _nameLabel(0)
	    , _enabled( true )
	    , _idList( id )
	{}

	/**
	 * Destructor. Intentionally not deleting the widgets.
	 **/
	virtual ~Step() {}

	virtual bool isHeading() const { return false; }

	QString  name()		const { return _name;		}
	QLabel * statusLabel()	const { return _statusLabel;	}
	QLabel * nameLabel()	const { return _nameLabel;	}
	bool	 isEnabled() 	const { return _enabled;	}
	const QStringList & id() const { return _idList;	}
	void addID( const QString & id ) { _idList.append( id ); }
	virtual bool hasID( const QString & id ) { return _idList.find( id ) != _idList.end(); }

	void setStatusLabel( QLabel * label )	{ _statusLabel = label; }
	void setNameLabel  ( QLabel * label )	{ _nameLabel   = label; }
	void setEnabled( bool enabled )		{ _enabled = enabled; }

    protected:

	QString		_name;
	QLabel * 	_statusLabel;
	QLabel *	_nameLabel;
	bool		_enabled;
	QStringList	_idList;
    };


    /**
     * Helper class to represent a wizard step heading internally
     **/
    class StepHeading: public Step
    {
    public:

	StepHeading( const QString & name = "" )
	    : Step( name, "" )
	    {}

	virtual ~StepHeading() {}
	virtual bool isHeading() const { return true; }
	virtual bool hasID( const QString & id ) { return false; }
    };


    /**
     * Helper class for wizard tree item
     **/
    class TreeItem: public QY2ListViewItem
    {
    public:
	TreeItem( QY2ListView *		parent,
		  const QString & 	text,
		  const QString & 	id )
	    : QY2ListViewItem( parent, text )
	    , _id( id )
	    {}

	TreeItem( YQWizard::TreeItem * 	parent,
		  const QString & 	text,
		  const QString & 	id )
	    : QY2ListViewItem( parent, text )
	    , _id( id )
	    {}

	QString text() const { return QListViewItem::text(0); }
	QString id()   const { return _id; }

    private:
	QString _id;
    };

}; // class YQWizard



#endif // YQWizard_h
