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

  File:	      YQComboBox.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <qstring.h>
#include <qlabel.h>
#include <qcombobox.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "QY2CharValidator.h"
#include "YQComboBox.h"

#define SPACING			4	// between subwidgets


YQComboBox::YQComboBox( QWidget * parent, YWidgetOpt & opt,
		       const YCPString & label)
    : QVBox(parent)
    , YComboBox(opt, label)
    , _validator( 0 )
{
    setWidgetRep( this );
    setSpacing( SPACING );
    setMargin( YQWIDGET_BORDER );

    _qt_label = new QLabel(fromUTF8(label->value() ), this );
    _qt_label->setTextFormat(QLabel::PlainText );
    _qt_label->setFont(YUIQt::ui()->currentFont() );

    _qt_combo_box = new QComboBox(opt.isEditable.value(), this );
    _qt_combo_box->setFont(YUIQt::ui()->currentFont() );

    _qt_label->setBuddy(_qt_combo_box );

    connect( _qt_combo_box, SIGNAL( highlighted( int) ), this, SLOT( slotSelected(int) ) );
    connect( _qt_combo_box, SIGNAL( activated( const QString & ) ), this, SLOT(textChanged(const QString & ) ) );
}


void YQComboBox::setLabel( const YCPString & label)
{
    _qt_label->setText(fromUTF8(label->value() ) );
    YComboBox::setLabel( label );
}


void YQComboBox::setValidChars( const YCPString & newValidChars )
{
    if ( _validator )
    {
	_validator->setValidChars( fromUTF8( newValidChars->value() ) );
    }
    else
    {
	_validator = new QY2CharValidator( fromUTF8( newValidChars->value() ), this );
	_qt_combo_box->setValidator( _validator );

	// No need to delete the validator in the destructor - Qt will take
	// care of that since it's a QObject with a parent!
    }

    YComboBox::setValidChars( newValidChars );
}


long YQComboBox::nicesize( YUIDimension dim)
{
    if (dim == YD_HORIZ) return sizeHint().width();
    else			return sizeHint().height();
}


void YQComboBox::setSize( long newWidth, long newHeight)
{
    resize(newWidth, newHeight );
}


void YQComboBox::setEnabling( bool enabled)
{
    _qt_label->setEnabled(enabled );
    _qt_combo_box->setEnabled(enabled );
}


void YQComboBox::itemAdded( const YCPString & string, int index, bool selected)
{
    _qt_combo_box->insertItem(fromUTF8(string->value() ) );

    if (selected)
	setValue(string );
}


YCPString YQComboBox::getValue() const
{
    return YCPString( toUTF8( _qt_combo_box->currentText() ) );
}


void YQComboBox::setValue( const YCPString & new_value)
{
    _qt_combo_box->setEditText( fromUTF8( new_value->value() ) );
}


void YQComboBox::setCurrentItem( int index)
{
    _qt_combo_box->setCurrentItem(index );
}


bool YQComboBox::setKeyboardFocus()
{
    _qt_combo_box->setFocus();

    return true;
}


// slots

void YQComboBox::slotSelected( int i)
{
    if (getNotify() )
	YUIQt::ui()->returnNow( YUIInterpreter::ET_WIDGET, this );
}


void YQComboBox::textChanged( const QString &new_text)
{
    if (getNotify() )
	YUIQt::ui()->returnNow( YUIInterpreter::ET_WIDGET, this );
}


#include "YQComboBox.moc.cc"
