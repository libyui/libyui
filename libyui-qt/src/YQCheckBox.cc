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

  File:	      YQCheckBox.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qcheckbox.h>
#include <qlayout.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YQCheckBox.h"


// +----+----------------------------------+----+
// |	|[X] Checkboxlabel		   |	|
// +----+----------------------------------+----+
// <----> SPACING			   <---->

#define SPACING 8


YQCheckBox::YQCheckBox( QWidget *parent, YWidgetOpt &opt,
		       const YCPString& label, bool checked)
    : QGroupBox(parent)
    , YCheckBox(opt, label)
    , dont_care(false)
{
    setWidgetRep( this );
    setFrameStyle(NoFrame);

    QBoxLayout *layout = new QBoxLayout(this, QBoxLayout::LeftToRight);

    qt_checkbox = new QCheckBox( fromUTF8(label->value()), this);
    layout->addSpacing(SPACING);
    layout->addWidget(qt_checkbox);
    layout->addSpacing(SPACING);
    qt_checkbox->setFont(YUIQt::ui()->currentFont());
    qt_checkbox->setChecked(checked);

    connect ( qt_checkbox, SIGNAL ( toggled ( bool ) ),
	      this, SLOT ( changed ( bool ) ) );
}


long YQCheckBox::nicesize(YUIDimension dim)
{
    if (dim == YD_HORIZ) return 2 * SPACING + qt_checkbox->sizeHint().width();
    else return qt_checkbox->sizeHint().height();
}


void YQCheckBox::setSize(long newWidth, long newHeight)
{
    qt_checkbox->resize(newWidth - 2*SPACING, newHeight);
    resize(newWidth, newHeight);
}


YCPValue YQCheckBox::getValue()
{
    switch ( qt_checkbox->state() )
    {
	case QButton::On:	return YCPBoolean( true  );
	case QButton::Off:	return YCPBoolean( false );
	case QButton::NoChange:	return YCPVoid();	// nil
    }
    
    y2error( "Unknown QCheckBox state: %d", (int) qt_checkbox->state() );
    return YCPVoid();
}


void YQCheckBox::setValue(const YCPValue & val)
{
    if ( val->isBoolean() )
    {
	setTristate(false);
	qt_checkbox->setChecked(val->asBoolean()->value());
    }
    else	// "Nil" -> set TriState: neither on nor off
    {
	setTristate(true);
    }
}


bool YQCheckBox::isTristate()
{
    return dont_care;
}

void YQCheckBox::setTristate(bool tristate)
{
    dont_care = tristate;
    qt_checkbox->setTristate(tristate);

    if ( tristate )
	qt_checkbox->setNoChange();
}

void YQCheckBox::setLabel(const YCPString &label)
{
    qt_checkbox->setText(fromUTF8(label->value()));
    YCheckBox::setLabel(label);
}


void YQCheckBox::setEnabling(bool enabled)
{
    qt_checkbox->setEnabled(enabled);
}


bool YQCheckBox::setKeyboardFocus()
{
    qt_checkbox->setFocus();

    return true;
}



// slots

void YQCheckBox::changed( bool newState )
{
    if (getNotify())
	YUIQt::ui()->returnNow(YUIInterpreter::ET_WIDGET, this);
}


#include "YQCheckBox.moc.cc"
