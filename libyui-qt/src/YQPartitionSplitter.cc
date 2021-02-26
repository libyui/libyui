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

  File:	      YQPartitionSplitter.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YEvent.h"
#include "YQPartitionSplitter.h"


YQPartitionSplitter::YQPartitionSplitter( QWidget *		parent,
					  const YWidgetOpt &	opt,
					  int			arg_usedSize,
					  int			arg_totalFreeSize,
					  int			arg_newPartSize,
					  int			arg_minNewSize,
					  int			arg_minFreeSize,
					  const YCPString &	arg_usedLabel,
					  const YCPString &	arg_freeLabel,
					  const YCPString &	arg_newPartLabel,
					  const YCPString &	arg_freeFieldLabel,
					  const YCPString &	arg_newPartFieldLabel )
    : QVBox( parent )
    , YPartitionSplitter( opt,
			  arg_usedSize,
			  arg_totalFreeSize,
			  arg_newPartSize,
			  arg_minNewSize,
			  arg_minFreeSize,
			  arg_usedLabel,
			  arg_freeLabel,
			  arg_newPartLabel,
			  arg_freeFieldLabel,
			  arg_newPartFieldLabel )
{
    setWidgetRep( this );

    _countShowDelta = opt.countShowDelta.value();

    setSpacing( YQWidgetSpacing );
    setMargin( YQWidgetMargin );


    // BarGraph

    _barGraph = new QY2BarGraph( this );
    _barGraph->setSegments(3);
    _barGraph->setLabel( 0, fromUTF8( usedLabel()->value()    ) );
    _barGraph->setLabel( 1, fromUTF8( freeLabel()->value()    ) );
    _barGraph->setLabel( 2, fromUTF8( newPartLabel()->value() ) );


    // upper inner HBox for the labels

    _labels_hbox = new QHBox( this );
    _labels_hbox->setSpacing( YQWidgetMargin );


    // Label for the free size

    _qt_freeFieldLabel = new QLabel( fromUTF8( freeFieldLabel()->value() ),
				    _labels_hbox );
    _qt_freeFieldLabel->setTextFormat( QLabel::PlainText );
    _qt_freeFieldLabel->setFont( YQUI::ui()->currentFont() );
    _qt_freeFieldLabel->setAlignment( Qt::AlignLeft );


    // Label for the new partition size

    _qt_newPartFieldLabel = new QLabel( fromUTF8( newPartFieldLabel()->value() ),
				       _labels_hbox );
    _qt_newPartFieldLabel->setTextFormat( QLabel::PlainText );
    _qt_newPartFieldLabel->setFont( YQUI::ui()->currentFont() );
    _qt_newPartFieldLabel->setAlignment( Qt::AlignRight );


    // lower inner HBox for the fields and the slider

    _fields_hbox = new QHBox( this );
    _fields_hbox->setSpacing( YQWidgetMargin );


    // SpinBox for the free size

    _qt_freeSizeField = new QSpinBox( minFreeSize(), maxFreeSize(),
				     1, // step
				     _fields_hbox );
    _qt_freeSizeField->setFont( YQUI::ui()->currentFont() );
    _qt_freeFieldLabel->setBuddy( _qt_freeSizeField );


    // Slider for the free size

    _qt_freeSizeSlider = new QSlider( minFreeSize(), maxFreeSize(),
				     1, // pageStep
				     remainingFreeSize(),	// initial value
				     QSlider::Horizontal, _fields_hbox );
    _qt_freeSizeSlider->setFont( YQUI::ui()->currentFont() );


    // SpinBox for the new partition size

    _qt_newPartSizeField = new QSpinBox( minNewPartSize(), maxNewPartSize(),
					1, // step
					_fields_hbox );
    _qt_newPartSizeField->setFont( YQUI::ui()->currentFont() );
    _qt_newPartFieldLabel->setBuddy( _qt_newPartSizeField );

    // Initialize all fields and the bar graph

    setValue( newPartSize() );


    // Connect signals

    connect( _qt_freeSizeSlider,	SIGNAL( valueChanged   (int) ),
	     this,		SLOT  ( setFreeSizeSlot(int) ) );

    connect( _qt_freeSizeField,	SIGNAL( valueChanged   (int) ),
	     this,		SLOT  ( setFreeSizeSlot(int) ) );

    connect( _qt_newPartSizeField,SIGNAL( valueChanged	  (int) ),
	     this,		SLOT  ( setNewPartSizeSlot(int) ) );
}


void YQPartitionSplitter::setEnabling( bool enabled )
{
    _qt_freeFieldLabel->setEnabled	( enabled );
    _qt_newPartFieldLabel->setEnabled	( enabled );
    _qt_freeSizeSlider->setEnabled	( enabled );
    _qt_freeSizeField->setEnabled		( enabled );
    _qt_freeSizeField->setEnabled		( enabled );
    _qt_newPartSizeField->setEnabled	( enabled );
}


long YQPartitionSplitter::nicesize( YUIDimension dim )
{
    if ( dim == YD_HORIZ )	return sizeHint().width();
    else			return sizeHint().height();
}


void YQPartitionSplitter::setSize( long newWidth, long newHeight )
{
    resize( newWidth, newHeight );
}


void YQPartitionSplitter::setValue( int new_newPartSize )
{
    YPartitionSplitter::setValue( new_newPartSize );

    if ( ! _barGraph )
	return;

    _barGraph->blockSignals( true );
    _qt_freeSizeField->blockSignals( true );
    _qt_freeSizeSlider->blockSignals( true );
    _qt_newPartSizeField->blockSignals( true );
    
    _barGraph->setValue( 0, usedSize() );
    _barGraph->setValue( 1, remainingFreeSize() );
    _barGraph->setValue( 2, newPartSize() );
    _barGraph->update();
    _qt_freeSizeField->setValue ( remainingFreeSize() );
    _qt_freeSizeSlider->setValue( remainingFreeSize() );
    _qt_newPartSizeField->setValue( newPartSize() );
    
    _barGraph->blockSignals( false );
    _qt_freeSizeField->blockSignals( false );
    _qt_freeSizeSlider->blockSignals( false );
    _qt_newPartSizeField->blockSignals( false );
    
}


void YQPartitionSplitter::setFreeSizeSlot( int newFreeSize )
{
    int newPartSize = totalFreeSize() - newFreeSize;
    if( _countShowDelta )
	newPartSize += usedSize();

    setValue( newPartSize );

    if ( getNotify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


void YQPartitionSplitter::setNewPartSizeSlot( int newPartSize )
{

    if( _countShowDelta )
	setValue( remainingFreeSize() - totalFreeSize() );
    else
	setValue( newPartSize );
	
    if ( getNotify() )
	YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


bool YQPartitionSplitter::setKeyboardFocus()
{
    _qt_newPartSizeField->setFocus();

    return true;
}


#include "YQPartitionSplitter.moc"
