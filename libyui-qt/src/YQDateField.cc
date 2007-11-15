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

  File:	      	YQDateField.cc

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include <qdatetimeedit.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQDateField.h"
#include "YQWidgetCaption.h"


YQDateField::YQDateField( YWidget * parent, const string & label )
    : QVBox( (QWidget *) parent->widgetRep() )
    , YDateField( parent, label )
{
    setWidgetRep( this );
    setSpacing( YQWidgetSpacing );
    setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, fromUTF8( label ) );
    YUI_CHECK_NEW( _caption );

    _qt_dateEdit = new QDateEdit( this );
    YUI_CHECK_NEW( _qt_dateEdit );

    _qt_dateEdit->setAutoAdvance( true );
    _qt_dateEdit->setOrder( QDateEdit::DMY );
    _caption->setBuddy( _qt_dateEdit );
}


YQDateField::~YQDateField()
{
    // NOP
}


string YQDateField::value()
{
    return toUTF8( _qt_dateEdit->date().toString( Qt::ISODate ) );
}


void YQDateField::setValue( const string & newValue )
{
    _qt_dateEdit->setDate( QDate::fromString( fromUTF8( newValue ), Qt::ISODate ) );

}


void YQDateField::setLabel( const string & newLabel )
{
    _caption->setText( fromUTF8( newLabel ) );
    YDateField::setLabel( newLabel );
}


void YQDateField::setEnabled( bool enabled )
{
    QVBox::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQDateField::preferredWidth()
{
    return sizeHint().width();
}


int YQDateField::preferredHeight()
{
    return sizeHint().height();
}


void YQDateField::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQDateField::setKeyboardFocus()
{
    _qt_dateEdit->setFocus();

    return true;
}



#include "YQDateField.moc"
