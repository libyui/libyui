/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|			   contributed Qt widgets		       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      QY2ComboTabWidget.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt widget - it can be used independently of YaST2.

/-*/


#include <qhbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qwidgetstack.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "QY2ComboTabWidget.h"


#define SPACING			6	// between subwidgets
#define MARGIN			4	// around the widget



QY2ComboTabWidget::QY2ComboTabWidget( const QString &	label,
				      QWidget *		parent,
				      const char *	name )
    : QVBox( parent, name )
{
    setFrameStyle( QFrame::Panel | QFrame::Raised );
    setLineWidth(2);
    setMidLineWidth(2);
    setSpacing( SPACING );
    setMargin ( MARGIN  );

    
    QHBox * hbox = new QHBox( this );
    CHECK_PTR( hbox );
    hbox->setSpacing( SPACING );
    hbox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert
    

    combo_label = new QLabel( label, hbox );
    CHECK_PTR( combo_label );
    
    combo_box = new QComboBox( hbox );
    CHECK_PTR( combo_box );
    
    combo_label->setBuddy( combo_box );
    combo_box->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert
    connect( combo_box, SIGNAL( activated( int ) ),
	     this,	SLOT  ( showPage ( int ) ) );
    
    widget_stack = new QWidgetStack( this );
    CHECK_PTR( widget_stack );
}



QY2ComboTabWidget::~QY2ComboTabWidget()
{
    
}


void
QY2ComboTabWidget::addPage( const QString & page_label, QWidget * new_page )
{
    pages.insert( combo_box->count(), new_page );
    combo_box->insertItem( page_label );
    widget_stack->addWidget( new_page );

    if ( ! widget_stack->visibleWidget() )
	widget_stack->raiseWidget( new_page );
}


void
QY2ComboTabWidget::showPage( int index )
{
    QWidget * page = pages[ index ];

    if ( page )
    {
	widget_stack->raiseWidget( page );
	// y2debug( "Changing current page" );
	emit currentChanged( page );
    }
    else
    {
	qWarning( "QY2ComboTabWidget: Page #%d not found", index );
	return;
    }
}


void
QY2ComboTabWidget::showPage( QWidget * page )
{
    widget_stack->raiseWidget( page );

    if ( page == pages[ combo_box->currentItem() ] )
    {
	// Shortcut: If the requested page is the one that belongs to the item
	// currently selected in the combo box, don't bother searching the
	// correct combo box item.
	return;
    }

    
    // Search the dict for this page
    
    QIntDictIterator<QWidget> it( pages );

    while ( it.current() )
    {
	if ( page == it.current() )
	{
	    combo_box->setCurrentItem( it.currentKey() );
	    return;
	}

	++it;
    }

    // If we come this far, that page isn't present in the dict.

    qWarning( "QY2ComboTabWidget: Page not found" );
}



#include "QY2ComboTabWidget.moc.cc"
