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

  File:		YQWizard.cc

  Author:	Stefan Hundhammer <sh@suse.de>

  Textdomain	"packages-qt"

/-*/

#include "YQWizard.h"
#define y2log_component "qt-wizard"
#include <ycp/y2log.h>

// For the command parser

#include <string>
#include <YShortcut.h>

#include <QDialog>
#include <QSvgRenderer>
#include <QPainter>
#include <QStackedWidget>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qmenudata.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>

#include "QY2ListView.h"
#include "QY2Styler.h"
#include "QY2HelpDialog.h"
#include <QGridLayout>
#include <qevent.h>

#include "utf8.h"
#include "YQi18n.h"
#include "YQUI.h"
#include "YQApplication.h"
#include "YQDialog.h"
#include "YQAlignment.h"
#include "YQReplacePoint.h"
#include "YQEmpty.h"
#include "YQLabel.h"
#include "YQWizardButton.h"
#include "YQIconPool.h"
#include "YQWidgetFactory.h"
#include "YQSignalBlocker.h"
#include "YEvent.h"

using std::string;

#ifdef TEXTDOMAIN
#    undef TEXTDOMAIN
#endif

#define TEXTDOMAIN "packages-qt"

#define USE_ICON_ON_HELP_BUTTON		0


YQWizard::YQWizard( YWidget *		parent,
		    const string & 	backButtonLabel,
		    const string & 	abortButtonLabel,
		    const string & 	nextButtonLabel,
		    YWizardMode 	wizardMode )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YWizard( parent,
	       backButtonLabel,
	       abortButtonLabel,
	       nextButtonLabel,
	       wizardMode )
    , _backButtonLabel( backButtonLabel )
    , _abortButtonLabel( abortButtonLabel )
    , _nextButtonLabel( nextButtonLabel )
{
    setObjectName( "wizard" );

    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setSpacing( 0 );
    layout->setMargin( 0 );

    setWidgetRep( this );

    _stepsEnabled = (wizardMode == YWizardMode_Steps);
    _treeEnabled  = (wizardMode == YWizardMode_Tree);

    _stepsDirty		= false;
    _direction		= YQWizard::Forward;

    _sideBar		= 0;
    _stepsPanel		= 0;
    _helpButton		= 0;
    _stepsButton	= 0;
    _treeButton		= 0;
    _releaseNotesButton = 0;
    _treePanel		= 0;
    _tree		= 0;
    _clientArea		= 0;
    _menuBar		= 0;
    _dialogIcon		= 0;
    _dialogHeading	= 0;
    _contents		= 0;
    _backButton		= 0;
    _abortButton	= 0;
    _nextButton		= 0;
    _sendButtonEvents	= true;
    _contentsReplacePoint = 0;

    YQUI::setTextdomain( TEXTDOMAIN );

    //layoutTitleBar( this );

    layout->addLayout( layoutSideBar( this ) );
    layout->addWidget( layoutWorkArea( this ) );

    QY2Styler::self()->registerWidget( this );
}



YQWizard::~YQWizard()
{
    deleteSteps();
}



void YQWizard::layoutTitleBar( QWidget * parent )
{
    QFrame * titleBar = new QFrame( parent );
    YUI_CHECK_NEW( titleBar );

    QHBoxLayout *layout = new QHBoxLayout( titleBar );
    titleBar->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert

    //
    // Left logo
    //

    QLabel * left = new QLabel( titleBar );
    layout->addWidget( left );
    left->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert
    left->setObjectName( "titleBar-left" );

    //
    // Center stretch space
    //

    layout->addStretch( 10 );


    //
    // Right logo
    //

    QLabel * right = new QLabel( titleBar );
    YUI_CHECK_NEW( right );

    layout->addWidget( right );
    right->setObjectName( "titleBar-right" );
}



QLayout *YQWizard::layoutSideBar( QWidget * parent )
{
    _sideBar = new QStackedWidget( parent );
    YUI_CHECK_NEW( _sideBar );
    _sideBar->setMinimumWidth( YQUI::ui()->defaultSize( YD_HORIZ ) / 5 );
    _sideBar->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) ); // hor/vert
    _sideBar->setObjectName( QString( "_sideBar-%1" ).arg( long( this ) ) );

    QVBoxLayout *vbox = new QVBoxLayout( );
    vbox->addWidget( _sideBar );

    if ( _treeEnabled )
    {
	layoutTreePanel();
	showTree();
    }
    else if ( _stepsEnabled )
    {
	layoutStepsPanel();
	showSteps();
    } else
        _sideBar->hide();

    return vbox;
}

void YQWizard::layoutStepsPanel()
{
    // Steps
    _stepsPanel = new QFrame( _sideBar );
    _sideBar->addWidget( _stepsPanel );
    _sideBar->setObjectName( "steps" );
    QY2Styler::self()->registerWidget( _sideBar );
    _stepsPanel->setProperty( "class", "steps QFrame" );

    // Steps panel bottom buttons ("Help", "Release Notes")

    // Layouts for the buttons

    _releaseNotesButton = new QPushButton( _( "Release Notes..." ), _stepsPanel );
    _releaseNotesButton->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum ) ); // hor/vert

    connect( _releaseNotesButton,	SIGNAL( clicked()  ),
	     this,			SLOT  ( releaseNotesClicked() ) );

    _releaseNotesButton->hide();	// hidden until showReleaseNotesButton() is called

    _stepsDirty = true; // no layout yet
}



void YQWizard::addStep( const string & text, const string & id )
{
    QString qId = fromUTF8( id );

    if ( _stepsIDs[ qId ] )
    {
	y2error( "Step ID \"%s\" (\"%s\") already used for \"%s\"",
		 id.c_str(),
		 text.c_str(),
		 qPrintable( _stepsIDs[ qId ]->name() ) );
	return;
    }

    if ( !_stepsList.empty() && _stepsList.last()->name() == fromUTF8( text ) )
    {
	// Consecutive steps with the same name will be shown as one single step.
	//
	// Since steps are always added at the end of the list, it is
	// sufficient to check the last step of the list. If the texts are the
	// same, the other with the same text needs to get another (additional)
	// ID to make sure setCurrentStep() works as it should.
	_stepsList.last()->addID( qId );
    }
    else
    {
	_stepsList.append( new YQWizard::Step( fromUTF8( text ), qId ) );
	_stepsDirty = true;
    }

    _stepsIDs.insert( qId, _stepsList.last() );
}



void YQWizard::addStepHeading( const string & text )
{
    _stepsList.append( new YQWizard::StepHeading( fromUTF8( text ) ) );
    _stepsDirty = true;
}



void YQWizard::updateSteps()
{
    if ( ! _stepsPanel )
	return;

    // Create a grid layout for the steps
    delete _stepsPanel->layout();
    QVBoxLayout *_stepsVBox = new QVBoxLayout( _stepsPanel );

    QGridLayout *_stepsGrid = new QGridLayout( );
    _stepsGrid->setObjectName( QString( "_stepsGrid_%1" ).arg(  long( this ) ) );
    YUI_CHECK_NEW( _stepsGrid );
    _stepsVBox->addLayout( _stepsGrid );
    _stepsGrid->setColumnMinimumWidth( 0, 10 );
    _stepsGrid->setRowStretch( 0, 1 );
    _stepsGrid->setRowStretch( 1, 1 );
    _stepsGrid->setRowStretch( 2, 99 );

    const int statusCol = 1;
    const int nameCol	= 2;

    int row = 0;

    //
    // Create widgets for all steps and step headings in the internal list
    //

    for ( QList<Step*>::iterator i = _stepsList.begin(); i != _stepsList.end(); ++i)
    {
        YQWizard::Step * step = *i;

        step->deleteLabels();

	if ( step->isHeading() )
	{
	    //
	    // Heading
	    //

            qDebug( "%p - add Stepheading %s", this, qPrintable( step->name() ) );
	    QLabel * label = new QLabel( step->name(), _stepsPanel );
	    YUI_CHECK_NEW( label );
            label->setObjectName( step->name() );
	    label->setAlignment( Qt::AlignLeft | Qt::AlignTop );
            label->setProperty( "class", "steps_heading" );

	    step->setNameLabel( label );
	    _stepsGrid->addWidget( label,
                                   row, statusCol,
                                   1, nameCol - statusCol + 1);
	}
	else	// No heading - ordinary step
	{
	    //
	    // Step status
	    //

            qDebug( "%p - add Step %s", this, qPrintable( step->name() ) );
	    QLabel * statusLabel = new QLabel( _stepsPanel );
	    YUI_CHECK_NEW( statusLabel );

	    step->setStatusLabel( statusLabel );
            statusLabel->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	    _stepsGrid->addWidget( statusLabel, row, statusCol );

	    //
	    // Step name
	    //

	    QLabel * nameLabel = new QLabel( step->name(), _stepsPanel );
	    YUI_CHECK_NEW( nameLabel );
	    nameLabel->setAlignment( Qt::AlignLeft | Qt::AlignTop );
            nameLabel->setObjectName( step->name() );

	    step->setNameLabel( nameLabel );
	    _stepsGrid->addWidget( nameLabel, row, nameCol );
        }

	row++;
    }

    _stepsVBox->addStretch( 99 );
    QVBoxLayout *rbl = new QVBoxLayout();
    rbl->addWidget( _releaseNotesButton, 0, Qt::AlignCenter );

    _stepsVBox->addLayout( rbl );
    _stepsVBox->addStretch( 29 );

    _stepsDirty = false;
}


void YQWizard::updateStepStates()
{
    if ( _stepsDirty )
	updateSteps();

    YQWizard::Step * currentStep = findStep( _currentStepID );
    QList<YQWizard::Step*>::iterator step = _stepsList.begin();

    if ( currentStep )
    {
	// Set status icon and color for the current step
	currentStep->setStatus( Step::Current );

	//
	// Set all steps before the current to "done"
	//

	while ( step != _stepsList.end() && *step != currentStep )
	{
	    ( *step )->setStatus( Step::Done );
	    step++;
	}

	// Skip the current step - continue with the step after it

	if ( step != _stepsList.end() )
	    step++;
    }

    //
    // Set all steps after the current to "to do"
    //

    while ( step != _stepsList.end() )
    {
	( *step )->setStatus( Step::Todo );
	step++;
    }
}

void YQWizard::setCurrentStep( const string & id )
{
    _currentStepID = fromUTF8( id );
    updateStepStates();
}


void YQWizard::deleteSteps()
{
    qDeleteAll(_stepsList);
    _stepsList.clear();
    _stepsIDs.clear();
}


YQWizard::Step * YQWizard::findStep( const QString & id )
{
    if ( id.isEmpty() )
	return 0;

    return _stepsIDs[ id ];
}


void YQWizard::layoutTreePanel()
{
    _treePanel = new QFrame( _sideBar );
    YUI_CHECK_NEW( _treePanel );
    QHBoxLayout *layout = new QHBoxLayout( _treePanel );
    _sideBar->addWidget( _treePanel );

    QVBoxLayout * vbox = new QVBoxLayout();
    YUI_CHECK_NEW( vbox );
    layout->addLayout( vbox );

    // Selection tree

    _tree = new QY2ListView( _treePanel );
    YUI_CHECK_NEW( _tree );
    vbox->addWidget( _tree );

    //FIXME
//     _tree->addColumn( "" );
//     _tree->header()->hide();

    _tree->setRootIsDecorated( true );
    _tree->setSortByInsertionSequence( true );

    connect( _tree,	SIGNAL( itemSelectionChanged     ( void ) ),
	     this,	SLOT  ( treeSelectionChanged ( void ) ) );

    connect( _tree,	SIGNAL( itemActivated  ( QTreeWidgetItem *, int ) ),
	     this,	SLOT  ( sendTreeEvent ( QTreeWidgetItem * ) ) );

    connect( _tree,	SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int ) ),
	     this,	SLOT  ( sendTreeEvent ( QTreeWidgetItem * ) ) );

}


void YQWizard::addTreeItem( const string & parentID, const string & text, const string & id )
{
    QString qId = fromUTF8( id );

    if ( ! _tree )
    {
	y2error( "YQWizard widget not created with `opt(`treeEnabled) !" );
	return;
    }

    YQWizard::TreeItem * item	= 0;
    YQWizard::TreeItem * parent = 0;

    if ( ! parentID.empty() )
    {
	parent = findTreeItem( parentID );
    }

    if ( parent )
    {
	item = new YQWizard::TreeItem( parent, fromUTF8( text ), qId );
	YUI_CHECK_NEW( item );
    }
    else
    {
	item = new YQWizard::TreeItem( _tree, fromUTF8( text ), qId );
	YUI_CHECK_NEW( item );
    }

    if ( ! qId.isEmpty() )
	_treeIDs.insert( qId, item );
}



void YQWizard::deleteTreeItems()
{
    if ( _tree )
	_tree->clear();

    _treeIDs.clear();
}



YQWizard::TreeItem * YQWizard::findTreeItem( const string & id )
{
    if ( id.empty() )
	return 0;

    return _treeIDs[ fromUTF8( id ) ];
}


void YQWizard::selectTreeItem( const string & id )
{
    if ( _tree )
    {
	YQWizard::TreeItem * item = findTreeItem( id );

	if ( item )
	{
	    YQSignalBlocker sigBlocker( _tree );

      _tree->setCurrentItem(item);
	    _tree->scrollToItem(item);
	}
    }
}


void YQWizard::sendTreeEvent( QTreeWidgetItem * listViewItem )
{
    if ( listViewItem )
    {
	YQWizard::TreeItem * item = dynamic_cast<YQWizard::TreeItem *> ( listViewItem );

	if ( item && ! item->id().isEmpty() )
	    sendEvent( toUTF8( item->id() ) );
    }
}


void YQWizard::treeSelectionChanged()
{ //FIXME is currentItem correct or selected.first
    if ( _tree )
	sendTreeEvent( _tree->currentItem() );
}


string YQWizard::currentTreeSelection()
{
    if ( _tree )
    {
	QTreeWidgetItem * sel = _tree->currentItem();

	if ( sel )
	{
	    YQWizard::TreeItem * item = dynamic_cast<YQWizard::TreeItem *> (sel);

	    if ( item && ! item->id().isEmpty() )
		return toUTF8( item->id() );
	}
    }

    return string();
}



QWidget *YQWizard::layoutWorkArea( QWidget * parent )
{
    QFrame *workArea = new QFrame( parent );
    workArea->setObjectName( "work_area" );

    QY2Styler::self()->registerWidget( workArea );

    QVBoxLayout *vbox = new QVBoxLayout( workArea );

    _menuBar = new QMenuBar( workArea );
    YUI_CHECK_NEW( _menuBar );

    _menuBar->hide(); // will be made visible when menus are added
    vbox->addWidget( _menuBar );

    //
    // Dialog icon and heading
    //

    QHBoxLayout * headingHBox = new QHBoxLayout();
    YUI_CHECK_NEW( headingHBox );
    //headingHBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert
    vbox->addLayout( headingHBox );

    _dialogIcon = new QLabel( workArea );
    YUI_CHECK_NEW( _dialogIcon );
    headingHBox->addWidget( _dialogIcon );
    _dialogIcon->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) ); // hor/vert
    _dialogIcon->setObjectName( "DialogIcon" );

    _dialogHeading = new QLabel( workArea );
    YUI_CHECK_NEW( _dialogHeading );
    headingHBox->addWidget( _dialogHeading );
    _dialogHeading->setAlignment( Qt::AlignLeft );
    _dialogHeading->setWordWrap( true );
    _dialogHeading->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert
    _dialogHeading->setObjectName( "DialogHeading" );

    //
    // Client area (the part that belongs to the YCP application)
    //

    layoutClientArea( workArea );
    vbox->addWidget( _clientArea );

    //
    // Button box
    //

    QLayout *bb = layoutButtonBox( workArea );
    vbox->addLayout( bb );

    return workArea;
}



void YQWizard::layoutClientArea( QWidget * parent )
{
    _clientArea = new QFrame( parent );
    YUI_CHECK_NEW( _clientArea );
    _clientArea->setObjectName("_clientArea");
    QVBoxLayout *layout = new QVBoxLayout( _clientArea );
    layout->setMargin( 0 );

    //
    // HVCenter for wizard contents
    //

    _contents = new YQAlignment( this, _clientArea, YAlignCenter, YAlignCenter );
    YUI_CHECK_NEW( _contents );
    layout->addWidget( _contents );
    _contents->QObject::setProperty( "class", "Contents" );

    _contents->setStretchable( YD_HORIZ, true );
    _contents->setStretchable( YD_VERT,	 true );
    _contents->installEventFilter( this );
    _contents->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    //
    // Replace point for wizard contents
    //

    _contentsReplacePoint = YUI::widgetFactory()->createReplacePoint( _contents );

    //
    // Initial YEmpty widget contents of replace point
    //

    YUI::widgetFactory()->createEmpty( _contentsReplacePoint );
    _contentsReplacePoint->showChild();

}



QLayout *YQWizard::layoutButtonBox( QWidget * parent )
{
    //
    // QHBoxLayout for the buttons
    //

    QHBoxLayout * hbox = new QHBoxLayout();		// parent, spacing
    YUI_CHECK_NEW( hbox );

    hbox->setSpacing( 0 );
    hbox->setMargin( 0 );

    // Help button - intentionally without keyboard shortcut
    _helpButton = new QPushButton( _( "Help" ), parent );
    YUI_CHECK_NEW( _helpButton );
    _helpButton->setShortcut( Qt::Key_F1 );

    connect( _helpButton, SIGNAL( clicked()  ),
	     this,	 SLOT  ( showHelp() ) );

    hbox->addWidget( _helpButton );

    hbox->addStretch( 10 );

    //
    // "Abort" button
    //

    _abortButton = new YQWizardButton( this, parent, _abortButtonLabel );
    YUI_CHECK_NEW( _abortButton );

    hbox->addWidget( (QWidget *) _abortButton->widgetRep() );
    connect( _abortButton,	SIGNAL( clicked()		),
	     this,		SLOT  ( slotAbortClicked()	) );

    hbox->addSpacing( 10 );

    //
    // "Back" button
    //

    _backButton	 = new YQWizardButton( this, parent, _backButtonLabel );
    YUI_CHECK_NEW( _backButton );

    hbox->addWidget( (QWidget *) _backButton->widgetRep() );
    connect( _backButton,	SIGNAL( clicked()		),
	     this,		SLOT  ( slotBackClicked()	) );

    if ( _backButton->text().isEmpty() )
	_backButton->hide();

    //
    // "Next" button
    //

    hbox->addSpacing( 5 );

    _nextButton	 = new YQWizardButton( this, parent, _nextButtonLabel );
    YUI_CHECK_NEW( _nextButton );

    hbox->addWidget( (QWidget *) _nextButton->widgetRep() );
    connect( _nextButton,	SIGNAL( clicked()		),
	     this,		SLOT  ( slotNextClicked()	) );

    return hbox;
}

void YQWizard::destroyButtons()
{
    delete _backButton;
    _backButton = 0;

    delete _abortButton;
    _abortButton = 0;

    delete _nextButton;
    _nextButton = 0;
}


void YQWizard::connectNotify ( const char * signal )
{
    if ( QString( signal ).contains( "nextClicked()" ) )
    {
	y2debug( "nextClicked connected, no longer directly sending button events" );
	_sendButtonEvents = false;
    }
}


void YQWizard::disconnectNotify ( const char * signal )
{
    if ( QString( signal ).contains( "nextClicked()" ) )
    {
	y2debug( "nextClicked disconnected, directly sending button events again" );
	_sendButtonEvents = true;
    }
}


void YQWizard::setDialogIcon( const string & iconName )
{
    if ( _dialogIcon )
    {
	if ( ! iconName.empty() )
	{
	    QPixmap icon( iconName.c_str() );

	    if ( icon.isNull() )
		y2warning( "Couldn't load dialog icon \"%s\"", iconName.c_str() );
	    else
	    {
		_dialogIcon->setPixmap( icon );
		topLevelWidget()->setWindowIcon( icon );
	    }
	}
	else
	{
	    _dialogIcon->clear();
	    topLevelWidget()->setWindowIcon( QIcon() );
	}
    }
}


void YQWizard::setDialogHeading( const string & headingText )
{
    if ( _dialogHeading )
    {
	if ( ! headingText.empty() )
	    _dialogHeading->setText( fromUTF8( headingText ) );
	else
	    _dialogHeading->clear();
    }
}

string YQWizard::debugLabel()
{
    if ( _dialogHeading )
    {
	QString label = _dialogHeading->text();
	label = label.simplified(); // Replace any embedded newline with a single blank

	if ( ! label.isEmpty() )
	{
	    label.prepend( "YQWizard \"" );
	    label.append( "\"" );

	    return toUTF8( label );
	}
    }

    return "untitled YQWizard";
}

void YQWizard::setHelpText( const string & helpText )
{
    _qHelpText = fromUTF8( helpText );
    _qHelpText.replace( "&product;", YQUI::ui()->productName() );
}


void YQWizard::slotBackClicked()
{
    emit backClicked();

    if ( _sendButtonEvents )
	YQUI::ui()->sendEvent( new YWidgetEvent( _backButton, YEvent::Activated ) );

    _direction = YQWizard::Backward;
}


void YQWizard::slotAbortClicked()
{
    emit abortClicked();

    if ( _sendButtonEvents )
	YQUI::ui()->sendEvent( new YWidgetEvent( _abortButton, YEvent::Activated ) );
}


void YQWizard::slotNextClicked()
{
    emit nextClicked();

    if ( _sendButtonEvents )
	YQUI::ui()->sendEvent( new YWidgetEvent( _nextButton, YEvent::Activated ) );

    _direction = YQWizard::Forward;
}


void YQWizard::showHelp()
{
    QY2HelpDialog helpDlg( _qHelpText, this );
    helpDlg.exec();
}


void YQWizard::releaseNotesClicked()
{
    YQUI::ui()->sendEvent( new YWidgetEvent( _nextButton, YEvent::Activated ) );

    if ( ! _releaseNotesButtonId.empty() )
    {
	y2milestone( "Release Notes button clicked" );
	sendEvent( _releaseNotesButtonId );
    }
}


void YQWizard::showSteps()
{
    if ( _sideBar && _stepsPanel )
    {
	_sideBar->setCurrentWidget( _stepsPanel );
    }
}


void YQWizard::showTree()
{
    if ( _sideBar && _treePanel )
    {
	_sideBar->setCurrentWidget( _treePanel );
    }
}



void YQWizard::addMenu( const string & text,
			const string & id )
{
    if ( _menuBar )
    {
	QMenu * menu = new QMenu( _menuBar );
	YUI_CHECK_NEW( menu );

	_menuIDs.insert( fromUTF8( id ), menu );
	//FIXME _menuBar->insertItem( fromUTF8( text ), menu );

	connect( menu, SIGNAL( activated    ( int ) ),
		 this, SLOT  ( sendMenuEvent( int ) ) );

        _menuBar->show();
    }
}


void YQWizard::addSubMenu( const string & parentMenuID,
			   const string & text,
			   const string & id )
{
    QMenu* parentMenu = _menuIDs[ fromUTF8( parentMenuID ) ];

    if ( parentMenu )
    {
	QMenu * menu = new QMenu( _menuBar );
	YUI_CHECK_NEW( menu );

	_menuIDs.insert( fromUTF8( id ), menu );
	//FIXME parentMenu->insertItem( fromUTF8( text ), menu );

	connect( menu, SIGNAL( activated    ( int ) ),
		 this, SLOT  ( sendMenuEvent( int ) ) );
    }
    else
    {
	y2error( "Can't find menu with ID %s", parentMenuID.c_str() );
    }
}


void YQWizard::addMenuEntry( const string & parentMenuID,
			     const string & text,
			     const string & idString )
{
    QMenu * parentMenu = _menuIDs[ fromUTF8( parentMenuID ) ];

    if ( parentMenu )
    {
	int id = _menuEntryIDs.size();
	_menuEntryIDs.push_back( idString );
	//FIXME parentMenu->insertItem( fromUTF8( text ), id );
    }
    else
    {
	y2error( "Can't find menu with ID %s", parentMenuID.c_str() );
    }
}


void YQWizard::addMenuSeparator( const string & parentMenuID )
{
    QMenu * parentMenu = _menuIDs[ fromUTF8( parentMenuID ) ];

    if ( parentMenu )
    {
	parentMenu->addSeparator();
    }
    else
    {
	y2error( "Can't find menu with ID %s", parentMenuID.c_str() );
    }
}


void YQWizard::deleteMenus()
{
    if ( _menuBar )
    {
	_menuBar->hide();
	_menuBar->clear();
	_menuIDs.clear();
	_menuEntryIDs.clear();
    }
}


void YQWizard::sendMenuEvent( int numID )
{
    if ( numID >= 0 && numID < (int) _menuEntryIDs.size() )
    {
	sendEvent( _menuEntryIDs[ numID ] );
    }
    else
    {
	y2error( "Invalid menu ID: %d", numID );
    }
}


void YQWizard::sendEvent( const string & id )
{
    YQUI::ui()->sendEvent( new YMenuEvent( id ) );
}


int YQWizard::preferredWidth()
{
    return sizeHint().width();
}


int YQWizard::preferredHeight()
{
    return sizeHint().height();
}


void YQWizard::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    resizeClientArea();
}



void YQWizard::resizeClientArea()
{
    y2debug( "resizing client area" );
    QRect contentsRect = _clientArea->contentsRect();
    _contents->setSize( contentsRect.width(), contentsRect.height() );
}

bool YQWizard::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::Resize && obj == _contents )
    {
	resizeClientArea();
	return true;		// Event handled
    }

    return QWidget::eventFilter( obj, ev );
}


void YQWizard::setButtonLabel( YPushButton * button, const string & newLabel )
{
    button->setLabel( newLabel );
    YDialog::currentDialog()->checkShortcuts();

    YQWizardButton * wizardButton = dynamic_cast<YQWizardButton *> (button);

    if ( wizardButton ) {
        // QWizardButton only implements hide and show, not setVisible
        if ( newLabel.empty() )
            wizardButton->hide();
        else
            wizardButton->show();
    }
}


void YQWizard::showReleaseNotesButton( const string & label, const string & id )
{
    return; // no longer supported!

    if ( ! _releaseNotesButton )
    {
	y2error( "NULL Release Notes button" );

	if ( ! _stepsPanel )
	    y2error( "This works only if there is a \"steps\" panel!" );

	return;
    }

    // no way to check the shortcut, so strip it
    _releaseNotesButton->setText( fromUTF8( YShortcut::cleanShortcutString( label ) ) );
    _releaseNotesButtonId = id;

    _releaseNotesButton->show();
}


void YQWizard::hideReleaseNotesButton()
{
    if ( _releaseNotesButton && !_releaseNotesButton->isHidden() )
	_releaseNotesButton->hide();
}


void YQWizard::retranslateInternalButtons()
{
    YQUI::setTextdomain( TEXTDOMAIN );

    if ( _helpButton )
	// "Help" button - intentionally without keyboard shortcut
	_helpButton->setText( _( "Help" ) );

    if ( _stepsButton )
	// "Steps" button - intentionally without keyboard shortcut
	_stepsButton->setText( _( "Steps" ) );

    if ( _treeButton )
	// "Tree" button - intentionally without keyboard shortcut
	_treeButton->setText( _( "Tree" ) );
}


void YQWizard::Step::deleteLabels()
{
    delete _statusLabel;
    _statusLabel = 0;
    delete _nameLabel;
    _nameLabel = 0;
}

YQWizard::Step::~Step()
{
    deleteLabels();
}

void YQWizard::Step::setStatus( Status s )
{
    if ( !_statusLabel || !_nameLabel || _status == s )
        return;

    _status = s;

    if ( s == Todo )
    {
        _statusLabel->setProperty( "class", "todo-step-status QLabel" );
        _nameLabel->setProperty( "class", "todo-step-name QLabel" );
    }

    if ( s == Done )
    {
        _statusLabel->setProperty( "class", "done-step-status QLabel" );
        _nameLabel->setProperty( "class", "done-step-name QLabel" );
    }

    if ( s == Current )
    {
        _statusLabel->setProperty( "class", "current-step-status QLabel" );
        _nameLabel->setProperty( "class", "current-step-name QLabel" );
    }

    qApp->style()->polish( _statusLabel );
    qApp->style()->polish( _nameLabel );
}

#include "YQWizard.moc"
