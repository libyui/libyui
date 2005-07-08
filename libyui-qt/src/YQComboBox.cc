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


#define SEND_SELECTION_CHANGED_EVENT 0

#include <qstring.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "QY2CharValidator.h"
#include "YQComboBox.h"



YQComboBox::YQComboBox( QWidget * 		parent,
			const YWidgetOpt & 	opt,
			const YCPString &	label )
    : QVBox( parent )
    , YComboBox( opt, label )
    , _validator(0)
{
    setWidgetRep( this );
    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );

    _qt_label = new QLabel( fromUTF8(label->value() ), this );
    _qt_label->setTextFormat( QLabel::PlainText );
    _qt_label->setFont( YQUI::ui()->currentFont() );

    if ( _qt_label->text().isEmpty() )
	_qt_label->hide();

    _qt_combo_box = new QComboBox( opt.isEditable.value(), this );
    _qt_combo_box->setFont( YQUI::ui()->currentFont() );

    _qt_label->setBuddy( _qt_combo_box );

#if SEND_SELECTION_CHANGED_EVENT
    connect( _qt_combo_box,	SIGNAL( highlighted (int) ),
	     this,		SLOT  ( slotSelected(int) ) );
#endif

    connect( _qt_combo_box,	SIGNAL( activated  ( const QString & ) ),
	     this,		SLOT  ( textChanged( const QString & ) ) );
}


void YQComboBox::setLabel( const YCPString & label )
{
    _qt_label->setText( fromUTF8(label->value() ) );
    
    if ( _qt_label->text().isEmpty() )
    {
	if ( _qt_label->isVisible() )
	    _qt_label->hide();
    }
    else
    {
	if ( _qt_label->isHidden() )
	    _qt_label->show();
    }
    
    YComboBox::setLabel( label );
}


void YQComboBox::setValidChars( const YCPString & newValidChars )
{
    if ( ! _qt_combo_box->editable() )
    {
	y2warning( "Setting ValidChars is useless on a combo box that isn't editable! (%s)",
		   debugLabel().c_str() );
	return;
    }
    
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
    
    if ( ! isValidText( _qt_combo_box->currentText() ) )
    {
	y2error( "Old value \"%s\" of %s \"%s\" invalid according to ValidChars \"%s\" - deleting",
		 (const char *) _qt_combo_box->currentText(),
		 widgetClass(), debugLabel().c_str(),
		 newValidChars->value().c_str() );
	_qt_combo_box->setCurrentText( "" );
    }

    YComboBox::setValidChars( newValidChars );
}


long YQComboBox::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ ) 	return sizeHint().width();
    else			return sizeHint().height();
}


void YQComboBox::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void YQComboBox::setEnabling( bool enabled )
{
    _qt_label->setEnabled( enabled );
    _qt_combo_box->setEnabled( enabled );
}


void YQComboBox::itemAdded( const YCPString & string, int index, bool selected )
{
    _qt_combo_box->insertItem( fromUTF8(string->value() ) );

    if ( selected )
	setValue( string );
}


int YQComboBox::getCurrentItem() const
{
    return _qt_combo_box->currentItem();
}


YCPString YQComboBox::getValue() const
{
    return YCPString( toUTF8( _qt_combo_box->currentText() ) );
}


bool YQComboBox::isValidText( const QString & txt ) const
{
    if ( ! _validator )
	return true;

    int pos = 0;
    QString text( txt );	// need a non-const QString &
    
    return _validator->validate( text, pos ) == QValidator::Acceptable;
}


void YQComboBox::setValue( const YCPString & ytext )
{
    QString text = fromUTF8( ytext->value() );
    
    if ( isValidText( text ) )
    {
	_qt_combo_box->blockSignals( true );
	_qt_combo_box->setCurrentText( text );
	_qt_combo_box->blockSignals( false );
    }
    else
    {
	y2error( "%s \"%s\": Rejecting invalid value \"%s\"",
		 widgetClass(), debugLabel().c_str(), ytext->value().c_str() );
    }
}


void YQComboBox::setCurrentItem( int index )
{
    _qt_combo_box->setCurrentItem( index );
}


bool YQComboBox::setKeyboardFocus()
{
    _qt_combo_box->setFocus();

    return true;
}


// slots

void YQComboBox::slotSelected( int i )
{
    if ( getNotify() )
    {
	if ( ! YQUI::ui()->eventPendingFor( this ) )
	{
	    // Avoid overwriting a (more important) ValueChanged event with a SelectionChanged event

	    YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::SelectionChanged ) );
	}
    }
}


void YQComboBox::textChanged( const QString & new_text )
{
    if ( getNotify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQComboBox::deleteAllItems()
{
    _qt_combo_box->blockSignals( true );
    _qt_combo_box->clear();
    YSelectionWidget::deleteAllItems();
    _qt_combo_box->blockSignals( false );
}

void YQComboBox::setInputMaxLength( const YCPInteger & numberOfChars)
{
	_qt_combo_box->lineEdit()->setMaxLength(numberOfChars->asInteger()->value());
}

#include "YQComboBox.moc"
