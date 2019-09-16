/*
  Copyright (C) 2019 SUSE LLC
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:	      YQItemSelector.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>
#include <yui/YEvent.h>
#include "utf8.h"
#include "YQItemSelector.h"
#include "YQSignalBlocker.h"
#include "YQUI.h"

#define VERBOSE_SELECTION       0

using std::string;


YQItemSelector::YQItemSelector( YWidget *	parent,
                                bool            enforceSingleSelection )
    : QScrollArea( (QWidget *) parent->widgetRep() )
    , YItemSelector( parent, enforceSingleSelection )
{
    setWidgetRep( this );

    setWidgetResizable( true );
    setSizeAdjustPolicy( QAbstractScrollArea::AdjustToContentsOnFirstShow );

    _itemContainer = new QWidget( this );
    _itemContainer->setObjectName( "YQItemSelectorItemContainer" );
    YUI_CHECK_NEW( _itemContainer );

    QVBoxLayout * outerVBox = new QVBoxLayout( _itemContainer );
    YUI_CHECK_NEW( outerVBox );

    _itemLayout = new QVBoxLayout();
    outerVBox->addLayout( _itemLayout );
    outerVBox->addStretch( 1000 ); // this takes up any excess space

    this->QScrollArea::setWidget( _itemContainer );
}


YQItemSelector::~YQItemSelector()
{
    // NOP
}


void YQItemSelector::addItemWidget( YQSelectorItemWidget * itemWidget )
{
    _itemLayout->addWidget( itemWidget );
}


void YQItemSelector::addItem( YItem * item )
{
    YUI_CHECK_PTR( item );
    YItemSelector::addItem( item );

    YQSelectorItemWidget * itemWidget = new YQSelectorItemWidget( this, item );
    YUI_CHECK_NEW( itemWidget );

    _itemWidgets[ item ] = itemWidget;

    connect( itemWidget,        &pclass( itemWidget )::selectionChanged,
             this,              &pclass( this       )::slotSelectionChanged );

    if ( item->selected() && enforceSingleSelection() )
        deselectOtherItems( item );
}


void YQItemSelector::addItems( const YItemCollection & itemCollection )
{
    for ( YItem * item: itemCollection )
        addItem( item );
}


void YQItemSelector::selectItem( YItem * item, bool selected )
{
    YQSelectorItemWidget * itemWidget = _itemWidgets.value( item );

    if ( ! itemWidget )
	YUI_THROW( YUIException( "Can't find selected item" ) );

    itemWidget->setSelected( selected );

    if ( enforceSingleSelection() )
    {
        if ( selected )
            deselectOtherItems( item );
    }
}


void YQItemSelector::deselectAllItems()
{
    foreach ( YQSelectorItemWidget * itemWidget, _itemWidgets )
        itemWidget->setSelected( false );

    YItemSelector::deselectAllItems();
}


void YQItemSelector::deselectOtherItems( YItem * selectedItem )
{
    for ( QMap<YItem *, YQSelectorItemWidget *>::iterator it = _itemWidgets.begin();
          it != _itemWidgets.end();
          ++it )
    {
        if ( it.key() != selectedItem )
        {
            it.key()->setSelected( false );      // The YItem
            it.value()->setSelected( false );    // ...and the corresponding widget
        }
    }
}


void YQItemSelector::deleteAllItems()
{
    YQSignalBlocker sigBlocker( this );

    qDeleteAll( _itemWidgets.values() );
    _itemWidgets.clear();

    YItemSelector::deleteAllItems();
}


void YQItemSelector::setEnabled( bool enabled )
{
    // TO DO

}


int YQItemSelector::preferredWidth()
{
    // TO DO
    return 42;
}


int YQItemSelector::preferredHeight()
{
    // TO DO
    return 42;
}


void YQItemSelector::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
}


bool YQItemSelector::setKeyboardFocus()
{
    // TO DO
    return false;
}


void YQItemSelector::slotSelectionChanged( YQSelectorItemWidget *   itemWidget,
                                           bool                     selected )
{
    YUI_CHECK_PTR( itemWidget );

    YItem * item = itemWidget->item();
    item->setSelected( selected );

    if ( selected )
    {
#if VERBOSE_SELECTION
        yuiMilestone() << "Selected " << item->label() << endl;
#endif

        if ( enforceSingleSelection() )
            deselectOtherItems( item );
    }
#if VERBOSE_SELECTION
    else
        yuiMilestone() << "Deselected " << item->label() << endl;
#endif

#if VERBOSE_SELECTION
    dumpItems();
#endif

    if ( notify() && ( selected || ! enforceSingleSelection() ) )
        YQUI::ui()->sendEvent( new YWidgetEvent( this, YEvent::ValueChanged ) );
}


//-----------------------------------------------------------------------------


YQSelectorItemWidget::YQSelectorItemWidget( YQItemSelector	* parent,
					    YItem               * item )
    : QFrame( parent->itemContainer() )
    , _parent( parent )
    , _item( item )
{
    string description;
    YDescribedItem * describedItem = dynamic_cast<YDescribedItem *>(item);

    if ( describedItem )
        description = describedItem->description();

    createWidgets( item->label(),
                   description,
                   item->iconName(),
                   item->selected() );
}


YQSelectorItemWidget::~YQSelectorItemWidget()
{
    // NOP
}


void YQSelectorItemWidget::createWidgets( const string  & label,
                                          const string	& description,
                                          const string	& iconName,
                                          bool		  selected )
{
    /*
     *	   this (QFrame)
     *	     _hBox
     *	       _vBox
     *           _headingToggle  (_headingRadioButton or _headingCheckBox)
     *		 _descriptionLabel
     *	       _iconLabel
     *
     *	   +--------------------------------------------------+	  QFrame (this)
     *	   |  ( )  Heading			    xx	  xx  |
     *	   |					     xx	 xx   |
     *	   |	   Description text		      Icon    |
     *	   |	   Description text		     xx	 xx   |
     *	   |	   ...				    xx	  xx  |
     *	   |	   Description text			      |
     *	   +--------------------------------------------------+
     */

    _headingRadioButton = 0;
    _headingCheckBox	= 0;
    _descriptionLabel	= 0;
    _iconLabel		= 0;

    yuiMilestone() << "Creating item for " << label << endl;


    // Parts initially generated with Qt Designer

    QSizePolicy sizePol( QSizePolicy::Preferred, QSizePolicy::Fixed );
    sizePol.setHorizontalStretch( 0 );
    sizePol.setVerticalStretch( 0 );
    sizePol.setHeightForWidth( sizePolicy().hasHeightForWidth() );
    setSizePolicy( sizePol );

    setFrameShape( QFrame::StyledPanel );
    setFrameShadow( QFrame::Raised );

    _hBox = new QHBoxLayout( this );	// outer layout
    _hBox->setSpacing( 6 );
    _hBox->setContentsMargins( -1, 6, 6, 6 );

    _vBox = new QVBoxLayout();		// inner layout
    _vBox->setSpacing( 6 );
    _vBox->setContentsMargins( 0, 0, 0, 0 );     // don't let margins accumulate


    //
    // Heading (QRadioButton or QCheckBox)
    //

    if ( singleSelection() )
    {
	_headingToggle = _headingRadioButton = new QRadioButton( fromUTF8( label ), this );
    }
    else
    {
	_headingToggle = _headingCheckBox = new QCheckBox( fromUTF8( label ), this );
    }

    YUI_CHECK_NEW( _headingToggle );

    _headingToggle->setObjectName( "YQSelectorItemHeading" );  // for QSS style sheets
    _headingToggle->setChecked( selected );
    
    QFont font( _headingToggle->font() );
    font.setBold( true );
    _headingToggle->setFont( font );

    connect( _headingToggle,    &pclass( _headingToggle )::toggled,
             this,              &pclass( this )::slotSelectionChanged );
    
    _vBox->addWidget( _headingToggle );
    _hBox->addLayout( _vBox );


    //
    // Description (body text)
    //

    if ( ! description.empty() )
    {
	_descriptionLabel = new QLabel( fromUTF8( description ), this );
	YUI_CHECK_NEW( _descriptionLabel );
	_descriptionLabel->setObjectName( "YQSelectorItemDescription" ); // for QSS
        _descriptionLabel->setIndent( 20 ); // Compensate for QRadioButton icon

        _vBox->addWidget( _descriptionLabel );
    }


    //
    // Icon
    //

    if ( ! iconName.empty() )
    {
	_iconLabel = new QLabel( "", this );
        YUI_CHECK_NEW( _iconLabel );

        // FIXME use icon loader
	_iconLabel->setPixmap( QPixmap( fromUTF8( iconName ) ) );
        // FIXME use icon loader

	_descriptionLabel->setObjectName( "YQSelectorItemIcon" );       // for QSS
        _iconLabel->setIndent(0);

        QSizePolicy sizePol( _iconLabel->sizePolicy() );
        sizePol.setHorizontalStretch( 0 );
        sizePol.setVerticalStretch( 0 );
        _iconLabel->setSizePolicy( sizePol );

        _hBox->addWidget( _iconLabel );
    }

    YUI_CHECK_PTR( _parent );
    _parent->addItemWidget( this );
}


bool YQSelectorItemWidget::singleSelection() const
{
    return _parent && _parent->enforceSingleSelection();
}


bool YQSelectorItemWidget::selected() const
{
    return _headingToggle->isChecked();
}


void YQSelectorItemWidget::setSelected( bool sel )
{
    YQSignalBlocker sigBlocker( this );
    _headingToggle->setChecked( sel );
}


void YQSelectorItemWidget::slotSelectionChanged( bool selected )
{
    emit selectionChanged( this, selected );
}
