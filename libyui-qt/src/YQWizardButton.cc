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

  File:	      YQWizardButton.cc

  Author:     Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qpushbutton.h>
#include <qsize.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQWizardButton.h"
#include "YQWizard.h"


YQWizardButton::YQWizardButton( YQWizard *		wizard,
				YQDialog *		dialog,
				QWidget *		buttonParent,
				const YCPString &	label,
				const YCPValue & 	id )
    : YQGenericButton( wizard, dialog, YWidgetOpt(), label )
    , _wizard( wizard )
{
    QPushButton * button = new QPushButton( fromUTF8( label->value() ), buttonParent );
    CHECK_PTR( button );
    
    setQPushButton( button );
    setWidgetRep( button );
    setId( id );
    button->setFont( YQUI::ui()->currentFont() );

    // The parent dialog makes a wizard button the default button if the rest
    // of the dialog doesn't have a default button, so make sure this button
    // doesn't interfere with that.
    setDefault( false );

    connect( button, SIGNAL( clicked() ),
	     this,   SIGNAL( clicked() ) );

    
    // This widget itself will never be visible, only its button - which is not
    // a child of this widget.
    QWidget::hide();
}


YQWizardButton::~YQWizardButton()
{
    // Don't delete qPushButton() here - its parent (buttonParent) will take
    // care of that!
}


void YQWizardButton::hide()
{
    if ( qPushButton() )
	qPushButton()->hide();
}


void YQWizardButton::show()
{
    if ( qPushButton() )
	qPushButton()->show();
}


bool YQWizardButton::isShown() const
{
    if ( qPushButton() )
	return qPushButton()->isShown();
    else
	return false;
}

bool YQWizardButton::isHidden() const
{
    return ! isShown();
}


#include "YQWizardButton.moc"
