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

  File:	      YQWizard.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include "YQWizard.h"
#define y2log_component "qt-wizard"
#include <ycp/y2log.h>

#include <qlabel.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qtextbrowser.h>
#include <qobjectlist.h>
#include <qwidgetstack.h>
#include <qpixmap.h>

#include "utf8.h"
#include "YQi18n.h"
#include "YQUI.h"
#include "YQReplacePoint.h"
#include "YQEmpty.h"
#include "YQLabel.h"
#include "QY2LayoutUtils.h"
#include "YEvent.h"

#define IMAGE_DIR	HELPIMAGEDIR	// FIXME

#define IMAGE_HELP_BG	"wizard-help-bg.png"


YQWizard::YQWizard( QWidget *		parent,
		    YWidgetOpt &	opt,
		    const YCPValue & 	backButtonId,	const YCPString & backButtonLabel,
		    const YCPValue & 	abortButtonId,	const YCPString & abortButtonLabel,
		    const YCPValue & 	nextButtonId,	const YCPString & nextButtonLabel  )
    : QVBox( parent )
    , YWizard( opt,
	       backButtonId,	backButtonLabel,
	       abortButtonId,	abortButtonLabel,
	       nextButtonId,	nextButtonLabel  )
    , _backButtonLabel ( backButtonLabel  )
    , _abortButtonLabel( abortButtonLabel )
    , _nextButtonLabel ( nextButtonLabel  )
    , _sideBar( 0 )
    , _stepsPanel( 0 )
    , _helpPanel( 0 )
    , _abortButton( 0 )
    , _backButton( 0 )
    , _nextButton( 0 )
{
    QHBox * hbox = 0;
    
    setWidgetRep( this );

    // The SuSE web site version of the corporate design colors
    QColor webGreenBG( 0x9c, 0xce, 0x9c );
    QColor webGreenFG( 0x31, 0x65, 0x00 );

    QColor kenGrey( 0xE6, 0xE6, 0xE6 );

    QColor bg( kenGrey );
    QColor lightGreen( 0xA4, 0xC2, 0x6A );


    //
    // Top decoration
    //

    QLabel * top = new QLabel( "SUSE Linux", this );
    CHECK_PTR( top );
    top->setPaletteBackgroundColor( bg );
    top->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    top->setFont( QFont( "Helvetica", 14, QFont::Bold ) );
    top->setMargin( 15 );
    top->setMinimumHeight( 80 );
    top->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) ); // hor/vert
    top->setPaletteForegroundColor( webGreenFG );


    //
    // Center part - side bar and work area
    //

    QHBox * outerHBox = new QHBox( this );
    CHECK_PTR( outerHBox );


    //
    // Side Bar
    //

    _sideBar = new QWidgetStack( outerHBox );
    CHECK_PTR( _sideBar );

    _sideBar->setMinimumWidth( YQUI::ui()->defaultSize( YD_HORIZ ) / 5 );
    _sideBar->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) ); // hor/vert
    _sideBar->setPaletteBackgroundColor( bg );

    
    _sideBar->setMargin( 0 );


    //
    // Steps
    //

    _stepsPanel = new QVBox( _sideBar );
    CHECK_PTR( _stepsPanel );

    _sideBar->addWidget( _stepsPanel );

    QLabel * steps = new QLabel(
				"<font size=3 color=#629900>"
				"<b>Base Installation</b>"
				"<ul>"
				"<li>Language<br>"
				"<li>Installation Settings<br>"
				"<li>Perform Installation<br>"
				"</ul>"
				"<b>Configuration</b>"
				"<ul>"
				"<li>Root Password"
				"<li>Network"
				"<li>Online Update"
				"<li>Users"
				"<li>Clean Up"
				"<li>Release Notes"
				"<li>Device Configuration"
				"</ul>"
				"</font>",
				_stepsPanel );
    CHECK_PTR( steps );

    steps->setPaletteBackgroundColor( bg );
    steps->setFont( QFont( "Helvetica", 10 ) );

    steps->setFont( QFont( "Helvetica", 12 ) );
    steps->setMargin( 10 );

    steps->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    steps->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) ); // hor/vert

    addVStretch( _stepsPanel );
    
    hbox = new QHBox( _stepsPanel );
    CHECK_PTR( hbox );

    addHStretch( hbox );

    _helpButton = new QPushButton( _( "Help" ), hbox );
    CHECK_PTR( _helpButton );

    connect( _helpButton, SIGNAL( clicked()  ),
	     this,        SLOT  ( showHelp() ) );

    addHStretch( hbox );
    addVSpacing( _stepsPanel );

    //
    // Help
    //

    _helpPanel = new QVBox( _sideBar );
    CHECK_PTR( _helpPanel );

    _sideBar->addWidget( _helpPanel );


    hbox = new QHBox( _helpPanel );
    CHECK_PTR( hbox );
    
    addHelpMarginColumn( hbox );


    // Help browser
    
    _helpBrowser = new QTextBrowser( hbox );
    CHECK_PTR( _helpBrowser );

    _helpBrowser->setMimeSourceFactory( 0 );
    _helpBrowser->setFont( YQUI::ui()->currentFont() );
    _helpBrowser->installEventFilter( this );
    _helpBrowser->setTextFormat( Qt::RichText );
    _helpBrowser->setMargin( 4 );

    QPixmap bgPixmap( QString( HELPIMAGEDIR ) + "/" + IMAGE_HELP_BG );

    if ( ! bgPixmap.isNull() )
	_helpBrowser->setPaletteBackgroundPixmap( bgPixmap );

    _helpBrowser->setText( "<p>This is a help text.</p><p>It should be helpful.</p>" );

    addHelpMarginColumn( hbox );
    
    QWidget * spacer = addVSpacing( _helpPanel );
    CHECK_PTR( spacer );


    // "Steps" button


    hbox = new QHBox( _helpPanel );
    CHECK_PTR( hbox );
    
    addHStretch( hbox );
    
    _stepsButton = new QPushButton( _( "Steps" ), hbox );
    CHECK_PTR( _stepsButton );

    
    connect( _stepsButton, SIGNAL( clicked()   ),
	     this,         SLOT  ( showSteps() ) );

    addHStretch( hbox );
    addVSpacing( _helpPanel );


    //
    // Work area (contains client area and button box)
    //

    QVBox * workAreaVBox = new QVBox( outerHBox );
    CHECK_PTR( workAreaVBox );
    
    _workArea = new QVBox( workAreaVBox );
    CHECK_PTR( _workArea );

    // _workArea->setFrameStyle( QFrame::TabWidgetPanel | QFrame::Sunken );
    _workArea->setFrameStyle( QFrame::Box | QFrame::Sunken );
    _workArea->setMargin( 4 );
    

    //
    // Client area
    //

    _clientArea = new QVBox( _workArea );
    CHECK_PTR( _clientArea );
    _clientArea->setMargin( 4 );

    
    //
    // Replace point for wizard contents
    //

    YWidgetOpt widgetOpt;
    _contentsReplacePoint = new YQReplacePoint( _clientArea, widgetOpt );
    CHECK_PTR( _contentsReplacePoint );

    _contentsReplacePoint->setId( YCPSymbol( YWizardContentsReplacePointID ) ); // `id(`contents)
    addChild( _contentsReplacePoint );
    _contentsReplacePoint->installEventFilter( this );


    //
    // Initial YEmpty widget contents of replace point
    //

#if 1
    YQEmpty * empty = new YQEmpty( _contentsReplacePoint, widgetOpt );
    empty->setParent( _contentsReplacePoint );
    _contentsReplacePoint->addChild( empty );
#else
    YQLabel * label =new YQLabel( _contentsReplacePoint, widgetOpt,
				  YCPString( "Initial placeholder\nfor the wizard replace point")  );
    label->setParent( _contentsReplacePoint );
    _contentsReplacePoint->addChild( label );

    label->setPaletteBackgroundColor( white );
#endif


    //
    // Button box
    //

    _buttonBox = new QHBox( _workArea );
    CHECK_PTR( _buttonBox );
    _buttonBox->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) ); // hor/vert
    _buttonBox->setMargin( 5 );
    layoutButtonBox();

    
    //
    // Spacers (purely decorative) at the bottom and right of the client area
    //

    QWidget * bottomSpacer = addVSpacing( workAreaVBox, 10 );
    CHECK_PTR( bottomSpacer );
    bottomSpacer->setPaletteBackgroundColor( bg );

    
    QWidget * rightSpacer = addHSpacing( outerHBox, 10 );
    CHECK_PTR( rightSpacer );
    rightSpacer->setPaletteBackgroundColor( bg );

    

    showSteps();
}


void YQWizard::layoutButtonBox()
{
    destroyButtons();


    addHSpacing( _buttonBox, 4 );
    
    //
    // "Abort" button
    //

    _abortButton = new QPushButton( fromUTF8( _abortButtonLabel->value() ), _buttonBox );
    CHECK_PTR( _abortButton );
    connect( _abortButton,	SIGNAL( clicked()     	),
	     this,	  	SLOT  ( abortClicked() ) );

    addHStretch( _buttonBox );

    
    //
    // "Back" button
    //

    _backButton  = new QPushButton( fromUTF8( _backButtonLabel->value()  ), _buttonBox );
    CHECK_PTR( _backButton );
    connect( _backButton,	SIGNAL( clicked()      	),
	     this,	  	SLOT  ( backClicked()	) );

    addHSpacing( _buttonBox );

    
    //
    // "Next" button
    //

    _nextButton  = new QPushButton( fromUTF8( _nextButtonLabel->value()  ), _buttonBox );
    CHECK_PTR( _nextButton );
    connect( _nextButton,	SIGNAL( clicked()	),
	     this,	  	SLOT  ( nextClicked()	) );

    
    addHSpacing( _buttonBox, 4 );
}


void YQWizard::addHelpMarginColumn( QWidget * parent )
{
    if ( ! parent )
	return;
    
    QVBox * vbox = new QVBox( parent );
    CHECK_PTR( vbox );



    // TO DO: upper (flexible) and lower (gradient) margin
    
    QWidget * spacer = addHSpacing( vbox );
    CHECK_PTR( spacer );

    spacer->setPaletteBackgroundColor( parent->paletteBackgroundColor() );
}



void YQWizard::destroyButtons()
{
    if ( _backButton  )
    {
	delete _backButton;
	_backButton = 0;
    }

    if ( _abortButton )
    {
	delete _abortButton;
	_abortButton = 0;
    }

    if ( _nextButton  )
    {
	delete _nextButton;
	_nextButton = 0;
    }
}


void YQWizard::addChild( YWidget * ychild )
{
    if ( ! ychild )
    {
	y2error( "Cannot add NULL child" );
	return;
    }

#warning FIXME delete any old child first

    QWidget * child = (QWidget *) ychild->widgetRep();
    CHECK_PTR( child );
    child->reparent( _clientArea, 0, QPoint( 0, 0 ) ); // parent, wflags, pos

    YContainerWidget::addChild( ychild );
}


void YQWizard::backClicked()
{
    sendEvent( _backButtonId );
}


void YQWizard::abortClicked()
{
    sendEvent( _abortButtonId );
}


void YQWizard::nextClicked()
{
    sendEvent( _nextButtonId );
}


void YQWizard::showHelp()
{
    if ( _sideBar && _helpPanel )
    {
	_sideBar->raiseWidget( _helpPanel );
    }
}


void YQWizard::showSteps()
{
    if ( _sideBar && _stepsPanel )
    {
	_sideBar->raiseWidget( _stepsPanel );
    }
}


void YQWizard::sendEvent( YCPValue id )
{
    // Wizard events are sent as menu events - the semantics are similar.
    //
    // Widget events wouldn't do since they use their widget's ID as the ID to
    // return (which would be inappropriate since that would be the ID of the
    // wizard widget). Another type of event (WizardEvent) could be introduced,
    // but it would add little more information (if any) than MenuEvent.
    //
    // YQPackageSelector uses the same approach. After all, one widget that can
    // return multiple IDs is roughly the semantics of MenuEvents.

    YQUI::ui()->sendEvent( new YMenuEvent( id ) );
}


long YQWizard::nicesize( YUIDimension dim )
{
    return dim == YD_HORIZ ? sizeHint().width() : sizeHint().height();
}


void YQWizard::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
    resizeClientArea();
}


void YQWizard::resizeClientArea()
{
    // y2debug( "resizing client area" );
    QRect contentsRect = _clientArea->contentsRect();
    _contentsReplacePoint->setSize( contentsRect.width(), contentsRect.height() );
}


bool YQWizard::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::Resize && obj == _contentsReplacePoint )
    {
	resizeClientArea();
	return true;		// Event handled
    }

    return QWidget::eventFilter( obj, ev );
}


#include "YQWizard.moc"
