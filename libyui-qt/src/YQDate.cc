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

  File:	      YQDate.cc

  Author:     Anas Nashif <nashif@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQDate.h"


YQDate::YQDate( QWidget * 		parent,
		  const YWidgetOpt &	opt,
		  const YCPString  &	label,
		  const YCPString &	date )
    //: QDateEdit ( parent )
    : QVBox( parent )
    , YDate( opt, label )
{
    setWidgetRep( this );
    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );

    _qt_label = new QLabel( fromUTF8(label->value()), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YQUI::ui()->currentFont() );

    if ( label->value() == "" )
	_qt_label->hide();

    _qt_dateedit = new QDateEdit( this );
    _qt_dateedit->setOrder( QDateEdit::YMD );
    _qt_dateedit->setDate( QDate::fromString(date->value(), Qt::ISODate ));

    _qt_label->setBuddy( _qt_dateedit );
}


YCPString YQDate::getDate()
{
    return YCPString( toUTF8(_qt_dateedit->date().toString(Qt::ISODate) ) );
}


void YQDate::setLabel( const YCPString & label )
{
    _qt_label->setText( fromUTF8(label->value() ) );
    YDate::setLabel( label );
}


void YQDate::setEnabling( bool enabled )
{
    setEnabled( enabled );
}


long YQDate::nicesize( YUIDimension dim )
{
    return dim == YD_HORIZ ? sizeHint().width() : sizeHint().height();
}


void YQDate::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}



void YQDate::setNewDate( const YCPString & date )
{
    _qt_dateedit->setDate(  QDate::fromString(date->value(), Qt::ISODate ));   
    
}



#include "YQDate.moc"
