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

  File:	      YQRadioButton.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qradiobutton.h>
#include <qlayout.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YQRadioButton.h"


// +----+----------------------------------+----+
// |	|(o) RadioButtonlabel		   |	|
// +----+----------------------------------+----+
// <----> SPACING			   <---->

#define SPACING 8


YQRadioButton::YQRadioButton(YUIQt *yuiqt,
			     QWidget *parent,
			     YWidgetOpt &opt,
			     YRadioButtonGroup *rbg,
			     const YCPString& label, bool checked)
    : QGroupBox(parent)
    , YRadioButton(opt, label, rbg)
    , yuiqt(yuiqt)
{
    setWidgetRep(this);

    setFrameStyle(NoFrame);

    QBoxLayout *layout = new QBoxLayout(this, QBoxLayout::LeftToRight);

    qt_radiobutton = new QRadioButton( fromUTF8(label->value()), this);
    layout->addSpacing(SPACING);
    layout->addWidget(qt_radiobutton);
    layout->addSpacing(SPACING);
    qt_radiobutton->setFont(yuiqt->currentFont());
    qt_radiobutton->setChecked(checked);

    connect ( qt_radiobutton, SIGNAL ( toggled ( bool ) ),
	      this, SLOT ( changed ( bool ) ) );
}


long YQRadioButton::nicesize(YUIDimension dim)
{
    if (dim == YD_HORIZ) return 2 * SPACING + qt_radiobutton->sizeHint().width();
    else return qt_radiobutton->sizeHint().height();
}


void YQRadioButton::setSize(long newwidth, long newheight)
{
    qt_radiobutton->resize(newwidth - 2*SPACING, newheight);
    resize(newwidth, newheight);
}


YCPBoolean YQRadioButton::getValue()
{
    return YCPBoolean(qt_radiobutton->isChecked());
}


void YQRadioButton::setValue(const YCPBoolean &c)
{
    qt_radiobutton->setChecked(c->value());
}


void YQRadioButton::setLabel(const YCPString &label)
{
    qt_radiobutton->setText(fromUTF8(label->value()));
    YRadioButton::setLabel(label);
}


void YQRadioButton::setEnabling(bool enabled)
{
    qt_radiobutton->setEnabled(enabled);
}


QRadioButton *YQRadioButton::getQtButton()
{
    return qt_radiobutton;
}


bool YQRadioButton::setKeyboardFocus()
{
    qt_radiobutton->setFocus();

    return true;
}


// slots

void YQRadioButton::changed( bool newState )
{
    if (getNotify() && newState)
	yuiqt->returnNow(YUIInterpreter::ET_WIDGET, this);
}




#include "YQRadioButton.moc.cc"
