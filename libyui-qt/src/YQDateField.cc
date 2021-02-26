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


#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include <qdatetimeedit.h>
#include <QVBoxLayout>

#include "utf8.h"
#include "YQUI.h"
#include "YQDateField.h"
#include "YQWidgetCaption.h"


YQDateField::YQDateField( YWidget * parent, const string & label )
    : QFrame( (QWidget *) parent->widgetRep() )
    , YDateField( parent, label )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    setWidgetRep( this );
    layout->setSpacing( YQWidgetSpacing );
    layout->setMargin ( YQWidgetMargin  );

    _caption = new YQWidgetCaption( this, fromUTF8( label ) );
    YUI_CHECK_NEW( _caption );
    layout->addWidget( _caption );

    _qt_dateEdit = new QDateEdit( this );
    YUI_CHECK_NEW( _qt_dateEdit );
    layout->addWidget( _qt_dateEdit );

    //_qt_dateEdit->setAutoAdvance( true );
    _qt_dateEdit->setDisplayFormat( "yyyy-MM-dd" );
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
    QFrame::setEnabled( enabled );
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
