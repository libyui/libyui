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
#include "YUIQt.h"
#include "YQPartitionSplitter.h"


#define VSPACING 4
#define	HSPACING 2
#define MARGIN	 4


YQPartitionSplitter::YQPartitionSplitter( 					  QWidget *		parent,
					  YWidgetOpt &		opt,
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
    : QWidget( parent )
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

    countShowDelta = opt.countShowDelta.value();

    // Outer VBox

    vbox = new QVBox( this );
    vbox->setSpacing( VSPACING );
    vbox->setMargin( MARGIN );


    // BarGraph

    barGraph = new QY2BarGraph( vbox );
    barGraph->setSegments(3);
    barGraph->setLabel( 0, fromUTF8( usedLabel()->value()    ) );
    barGraph->setLabel( 1, fromUTF8( freeLabel()->value()    ) );
    barGraph->setLabel( 2, fromUTF8( newPartLabel()->value() ) );


    // upper inner HBox for the labels

    labels_hbox = new QHBox( vbox );
    labels_hbox->setSpacing( HSPACING );


    // Label for the free size

    qt_freeFieldLabel = new QLabel( fromUTF8( freeFieldLabel()->value() ),
				    labels_hbox );
    qt_freeFieldLabel->setTextFormat( QLabel::PlainText );
    qt_freeFieldLabel->setFont( YUIQt::ui()->currentFont() );
    qt_freeFieldLabel->setAlignment( Qt::AlignLeft );


    // Label for the new partition size

    qt_newPartFieldLabel = new QLabel( fromUTF8( newPartFieldLabel()->value() ),
				       labels_hbox );
    qt_newPartFieldLabel->setTextFormat( QLabel::PlainText );
    qt_newPartFieldLabel->setFont( YUIQt::ui()->currentFont() );
    qt_newPartFieldLabel->setAlignment( Qt::AlignRight );


    // lower inner HBox for the fields and the slider

    fields_hbox = new QHBox( vbox );
    fields_hbox->setSpacing( HSPACING );


    // SpinBox for the free size

    qt_freeSizeField = new QSpinBox( minFreeSize(), maxFreeSize(),
				     1, // step
				     fields_hbox );
    qt_freeSizeField->setFont( YUIQt::ui()->currentFont() );
    qt_freeFieldLabel->setBuddy( qt_freeSizeField );


    // Slider for the free size

    qt_freeSizeSlider = new QSlider( minFreeSize(), maxFreeSize(),
				     1, // pageStep
				     remainingFreeSize(),	// initial value
				     QSlider::Horizontal, fields_hbox );
    qt_freeSizeSlider->setFont( YUIQt::ui()->currentFont() );


    // SpinBox for the new partition size

    qt_newPartSizeField = new QSpinBox( minNewPartSize(), maxNewPartSize(),
					1, // step
					fields_hbox );
    qt_newPartSizeField->setFont( YUIQt::ui()->currentFont() );
    qt_newPartFieldLabel->setBuddy( qt_newPartSizeField );

    // Initialize all fields and the bar graph

    setValue( newPartSize() );


    // Connect signals

    connect( qt_freeSizeSlider,	SIGNAL( valueChanged   (int) ),
	     this,		SLOT  ( setFreeSizeSlot(int) ) );

    connect( qt_freeSizeField,	SIGNAL( valueChanged   (int) ),
	     this,		SLOT  ( setFreeSizeSlot(int) ) );

    connect( qt_newPartSizeField,SIGNAL( valueChanged	  (int) ),
	     this,		SLOT  ( setNewPartSizeSlot(int) ) );
}


void YQPartitionSplitter::setEnabling( bool enabled )
{
    qt_freeFieldLabel->setEnabled	( enabled );
    qt_newPartFieldLabel->setEnabled	( enabled );
    qt_freeSizeSlider->setEnabled	( enabled );
    qt_freeSizeField->setEnabled		( enabled );
    qt_freeSizeField->setEnabled		( enabled );
    qt_newPartSizeField->setEnabled	( enabled );
}


long YQPartitionSplitter::nicesize( YUIDimension dim )
{
    if (dim == YD_HORIZ) return vbox->sizeHint().width();
    else			return vbox->sizeHint().height();
}


void YQPartitionSplitter::setSize( long newWidth, long newHeight )
{
    vbox->resize( newWidth, newHeight );
    resize( newWidth, newHeight );
}


void YQPartitionSplitter::setValue( int new_newPartSize )
{
    YPartitionSplitter::setValue( new_newPartSize);

    if ( ! barGraph )
	return;

    barGraph->setValue( 0, usedSize() );
    barGraph->setValue( 1, remainingFreeSize() );
    barGraph->setValue( 2, newPartSize() );
    barGraph->update();
    qt_freeSizeField->setValue ( remainingFreeSize() );
    qt_freeSizeSlider->setValue( remainingFreeSize() );
    qt_newPartSizeField->setValue  ( newPartSize() );
}


void YQPartitionSplitter::setFreeSizeSlot( int newFreeSize )
{
    int newPartSize = totalFreeSize() - newFreeSize;
    if( countShowDelta )
	{
	newPartSize += usedSize();
	}
    setValue( newPartSize );

    if ( getNotify() )
	YUIQt::ui()->returnNow( YUIInterpreter::ET_WIDGET, this );
}


void YQPartitionSplitter::setNewPartSizeSlot( int newPartSize )
{

    if( countShowDelta )
	{
	setValue( remainingFreeSize() - totalFreeSize() );
	}
    else
	{
	setValue( newPartSize );
	}
    if ( getNotify() )
	YUIQt::ui()->returnNow( YUIInterpreter::ET_WIDGET, this );
}


bool YQPartitionSplitter::setKeyboardFocus()
{
    qt_newPartSizeField->setFocus();

    return true;
}


#include "YQPartitionSplitter.moc.cc"
