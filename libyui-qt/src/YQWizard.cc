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
    , _backButton( 0 )
    , _abortButton( 0 )
    , _nextButton( 0 )
{
    setWidgetRep( this );

    // Official SuSE colors according to the corporate design;
    // they all look horrible on-screen.

    QColor suseColorLinux ( (int) (.45*255), (int) (.75*255), (int) (.10*255) );
    QColor suseColorOcean ( (int) (0),       (int) (.30*255), (int) (.25*255) );
    QColor suseColorJungle( (int) (.10*255), (int) (.60*255), (int) (.20*255) );
    QColor suseColorOlive ( (int) (.20*255), (int) (.35*255), (int) (.10*255) );

    // The SuSE web site version of the corporate design colors
    QColor webGreenBG( 0x9c, 0xce, 0x9c );
    QColor webGreenFG( 0x31, 0x65, 0x00 );

    QColor bg( webGreenBG );


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

    QHBox * hbox = new QHBox( this );
    CHECK_PTR( hbox );


    //
    // Side Bar
    //

    _sideBar = new QTabWidget( hbox );
    CHECK_PTR( _sideBar );
    _sideBar->setTabPosition( QTabWidget::Bottom );
    _sideBar->setMinimumWidth( YQUI::ui()->defaultSize( YD_HORIZ ) / 5 );
    _sideBar->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) ); // hor/vert
    _sideBar->setMargin( 8 );
    
    _widgetStack = findWidgetStack( _sideBar );

    if ( _widgetStack )
    {
	_widgetStack->setPaletteBackgroundColor( bg );
	// Postponing widgetStack->setFrameStyle( QFrame::NoFrame );
	// until the last tab is added - each call to QTabWidget::addTab()
	// restores the 3D effects that we don't want here.
    }



    //
    // Steps
    //

    QLabel * steps = new QLabel(
				"<font size=3>"
				"<b>Base Installation</b>"
				"<ul>"
				"<li>Language Selection<br>"
				"<li>Basic Settings<br>"
				"<li>Software Installation<br>"
				"</ul>"
				"<b>Configuration</b>"
				"<ul>"
				"<li>Root Password"
				"<li>Network"
				"<li>Online Update"
				"<li>User"
				"<li>Clean up"
				"<li>Release Notes"
				"<li>Device Configuration"
				"</ul>"
				"</font>",
				_sideBar );
    CHECK_PTR( steps );

    _sideBar->addTab( steps, _( "Steps" ) );
    steps->setPaletteBackgroundColor( bg );
    steps->setFont( QFont( "Helvetica", 10 ) );

    // steps->setPaletteForegroundColor( webGreenFG );
    steps->setFont( QFont( "Helvetica", 12 ) );
    steps->setMargin( 10 );

    steps->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    steps->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) ); // hor/vert

    //
    // Help
    //

    QTextBrowser * _helpBrowser = new QTextBrowser( _sideBar );
    CHECK_PTR( _helpBrowser );

    _sideBar->addTab( _helpBrowser, _( "Help" ) );

    _helpBrowser->setMimeSourceFactory( 0 );
    _helpBrowser->setFont( YQUI::ui()->currentFont() );
    _helpBrowser->installEventFilter( this );
    _helpBrowser->setTextFormat( Qt::RichText );
    _helpBrowser->setMargin( 4 );

    QPixmap bgPixmap( QString( HELPIMAGEDIR ) + "/" + IMAGE_HELP_BG );

    if ( ! bgPixmap.isNull() )
	_helpBrowser->setPaletteBackgroundPixmap( bgPixmap );

    _helpBrowser->setText( "<p>This is a help text.</p><p>It should be helpful.</p>" );


    // Disable 3D effects (they ruin the overall appearance) - but only after
    // the last tab is added: Each call to QTabWidget::addTab() restores the 3D
    // effects.

    if ( _widgetStack )
	_widgetStack->setFrameStyle( QFrame::NoFrame );


    //
    // Work area (contains client area and button box)
    //

    QVBox * work_area = new QVBox( hbox );

    //
    // Client area
    //

    _clientArea = new QVBox( work_area );
    CHECK_PTR( _clientArea );

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
    YQEmpty * empty =new YQEmpty( _contentsReplacePoint, widgetOpt );
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

    _buttonBox = new QHBox( work_area );
    CHECK_PTR( _buttonBox );
    _buttonBox->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) ); // hor/vert
    _buttonBox->setMargin( 5 );
    layoutButtonBox();
}


QWidgetStack * YQWizard::findWidgetStack( QTabWidget * tab )
{
    // The author of QTabBar saw fit not to allow disabling or changing of any
    // 3D effects or margins.
    //
    // But Qt generally provides access to any child widgets, so let's do the
    // brute force approach and search the one and only QWidgetStack (derived
    // from QFrame) widget that is responsible for those 3D effects. We can't
    // use them here in this wizard - they would destroy the overall effect.
    // Yes, strictly spoken, this is an ugly hack...

    QWidgetStack * widgetStack = 0;
    QObjectList * childrenList = tab->queryList( "QWidgetStack" );

    if ( childrenList )
    {
	widgetStack = (QWidgetStack *) childrenList->first();
	delete childrenList;
    }

    return widgetStack;
}


void YQWizard::layoutButtonBox()
{
    destroyButtons();

    //
    // "Back" button
    //

    _backButton  = new QPushButton( fromUTF8( _backButtonLabel->value()  ), _buttonBox );
    CHECK_PTR( _backButton );
    connect( _backButton,	SIGNAL( clicked()      	),
	     this,	  	SLOT  ( backClicked()	) );

    addHStretch( _buttonBox );


    //
    // "Abort" button
    //

    _abortButton = new QPushButton( fromUTF8( _abortButtonLabel->value() ), _buttonBox );
    CHECK_PTR( _abortButton );
    connect( _abortButton,	SIGNAL( clicked()     	),
	     this,	  	SLOT  ( abortClicked() ) );

    addHStretch( _buttonBox );


    //
    // "Next" button
    //

    _nextButton  = new QPushButton( fromUTF8( _nextButtonLabel->value()  ), _buttonBox );
    CHECK_PTR( _nextButton );
    connect( _nextButton,	SIGNAL( clicked()	),
	     this,	  	SLOT  ( nextClicked()	) );
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
    QSize contentsSize = _clientArea->size();
    _contentsReplacePoint->setSize( contentsSize.width(), contentsSize.height() );
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
