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

  File:	      YQTime.cc

  Author:     Anas Nashif <nashif@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQTime.h"


YQTime::YQTime( QWidget * 		parent,
		  const YWidgetOpt &	opt,
		  const YCPString  &	label,
		  const YCPString &	time )
    //: QTimeEdit ( parent )
    : QVBox( parent )
    , YTime( opt, label )
{
    setWidgetRep( this );
    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );

    _qt_label = new QLabel( fromUTF8(label->value()), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YQUI::ui()->currentFont() );

    if ( label->value() == "" )
	_qt_label->hide();

    _qt_timeedit = new QTimeEdit( this );
    _qt_timeedit->setTime( QTime::fromString(time->value(), Qt::ISODate ));

    _qt_label->setBuddy( _qt_timeedit );
}


YCPString YQTime::getTime()
{
    return YCPString( toUTF8(_qt_timeedit->time().toString(Qt::ISODate) ) );
}


void YQTime::setLabel( const YCPString & label )
{
    _qt_label->setText( fromUTF8(label->value() ) );
    YTime::setLabel( label );
}


void YQTime::setEnabling( bool enabled )
{
    setEnabled( enabled );
}


long YQTime::nicesize( YUIDimension dim )
{
    return dim == YD_HORIZ ? sizeHint().width() : sizeHint().height();
}


void YQTime::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}



void YQTime::setNewTime( const YCPString & time )
{
    _qt_timeedit->setTime(  QTime::fromString(time->value(), Qt::ISODate ));   
    
}



#include "YQTime.moc"
