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

  File:	      YQCheckBox.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qcheckbox.h>
#include <QBoxLayout>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "utf8.h"
#include "YQApplication.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQCheckBox.h"


#define SPACING 8


YQCheckBox::YQCheckBox( YWidget *	parent,
			const string & 	label,
			bool 		checked )
    : QCheckBox( fromUTF8( label ), (QWidget *) parent->widgetRep() )
    , YCheckBox( parent, label )
{
    setWidgetRep( this );

    QCheckBox::setChecked( checked );

    connect( this, 	SIGNAL( stateChanged( int ) ),
	     this, 		SLOT  ( stateChanged( int ) ) );
}


YQCheckBox::~YQCheckBox()
{
    // NOP
}


YCheckBoxState
YQCheckBox::value()
{
    switch ( checkState() )
    {
        case Qt::Checked:          return YCheckBox_on;
	case Qt::Unchecked:	   return YCheckBox_off;
	case Qt::PartiallyChecked: return YCheckBox_dont_care;
    }

    return YCheckBox_off;
}


void
YQCheckBox::setValue( YCheckBoxState newValue )
{
    switch ( newValue )
    {
	case YCheckBox_on:
            QCheckBox::setChecked( true );
	    setTristate( false );
	    break;

	case YCheckBox_off:
	    QCheckBox::setChecked( false );
	    setTristate( false );
	    break;

	case YCheckBox_dont_care:
	    QCheckBox::setTristate( true );
	    setCheckState(Qt::PartiallyChecked);
	    break;
    }
}


void YQCheckBox::setLabel( const string & label )
{
    setText( fromUTF8( label ) );
    YCheckBox::setLabel( label );
}


void YQCheckBox::setUseBoldFont( bool useBold )
{
    setFont( useBold ?
			   YQUI::yqApp()->boldFont() :
			   YQUI::yqApp()->currentFont() );

    YCheckBox::setUseBoldFont( useBold );
}


void YQCheckBox::setEnabled( bool enabled )
{
    QCheckBox::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQCheckBox::preferredWidth()
{
    return 2*SPACING + sizeHint().width();
}


int YQCheckBox::preferredHeight()
{
    return sizeHint().height();
}


void YQCheckBox::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQCheckBox::setKeyboardFocus()
{
    setFocus();

    return true;
}


void YQCheckBox::stateChanged( int newState )
{
    // yuiMilestone() << "new state: " << newState << endl;

    if ( notify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


#include "YQCheckBox.moc"
