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

#include "utf8.h"
#include "YQUI.h"
#include "YQReplacePoint.h"
#include "YQEmpty.h"
#include "QY2LayoutUtils.h"
#include "YEvent.h"



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

    // Official SuSE CD colors
    // They all look horrible on-screen.

    QColor suseColorLinux ( (int) (.45*255), (int) (.75*255), (int) (.10*255) );
    QColor suseColorOcean ( (int) (0),       (int) (.30*255), (int) (.25*255) );
    QColor suseColorJungle( (int) (.10*255), (int) (.60*255), (int) (.20*255) );
    QColor suseColorOlive ( (int) (.20*255), (int) (.35*255), (int) (.10*255) );
    
    QColor bg( suseColorOlive );

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

    //
    // Left pane
    //

    QHBox * hbox = new QHBox( this );
    CHECK_PTR( hbox );

    QLabel * left_pane = new QLabel( "\n\n- Step 1\n- Step 2\n- Step 3\n  ...\n- Step n", hbox );
    CHECK_PTR( left_pane );
    left_pane->setPaletteBackgroundColor( bg );
    left_pane->setFont( QFont( "Helvetica", 12 ) );
    left_pane->setMargin( 30 );
    left_pane->setMinimumWidth( 200 );
    left_pane->setAlignment( Qt::AlignLeft | Qt::AlignTop );
    left_pane->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) ); // hor/vert


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

    
    //
    // Initial YEmpty widget contents of replace point
    //

    YQEmpty * empty =new YQEmpty( _contentsReplacePoint, widgetOpt );
    empty->setParent( _contentsReplacePoint );
    _contentsReplacePoint->addChild( empty );


    //
    // Button box
    //

    _buttonBox = new QHBox( work_area );
    CHECK_PTR( _buttonBox );
    _buttonBox->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) ); // hor/vert
    _buttonBox->setMargin( 5 );
    layoutButtonBox();
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
}


#include "YQWizard.moc"
