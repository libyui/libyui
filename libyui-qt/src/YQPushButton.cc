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

  File:	      YQPushButton.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qpushbutton.h>
#include <qsize.h>
#include <qevent.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "utf8.h"
#include "YUIQt.h"
#include "YQPushButton.h"
#include "YQDialog.h"


#define BORDER 3
#define BORDERSIZE QSize(BORDER, BORDER)

YQPushButton::YQPushButton(YUIQt *yuiqt,
			   QWidget *parent,
			   YWidgetOpt &opt,
			   YCPString label)
    : QWidget(parent)
    , YPushButton(opt, label)
    , yuiqt(yuiqt)
{
    setWidgetRep((QWidget *)this);
    qt_pushbutton = new QPushButton(fromUTF8(label->value()), this);
    qt_pushbutton->setFont(yuiqt->currentFont());
    qt_pushbutton->setMinimumSize(2,2);
    qt_pushbutton->setAutoDefault( true );
    qt_pushbutton->installEventFilter( this );
    qt_pushbutton->move(BORDER, BORDER);
    setMinimumSize(qt_pushbutton->minimumSize() + 2 * BORDERSIZE);
    connect(qt_pushbutton, SIGNAL(clicked()), this, SLOT(hit()));

    if (opt.isDefaultButton.value()) makeDefaultButton();
}


void YQPushButton::setEnabling(bool enabled)
{
    qt_pushbutton->setEnabled(enabled);
}


long YQPushButton::nicesize(YUIDimension dim)
{
    return 2 * BORDER + (dim == YD_HORIZ
			 ? qt_pushbutton->sizeHint().width()
			 : qt_pushbutton->sizeHint().height());
}


void YQPushButton::setSize(long newwidth, long newheight)
{
    y2debug("Resizing PushButton to %ld,%ld", newwidth, newheight);
    qt_pushbutton->resize(newwidth - 2 * BORDER, newheight - 2 * BORDER);
    resize(newwidth, newheight);
}


void YQPushButton::setLabel(const YCPString& label)
{
    qt_pushbutton->setText(fromUTF8(label->value()));
    YPushButton::setLabel( label );
}


void YQPushButton::makeDefaultButton()
{
    YQDialog *dia;

    if ( yParent() && ( dia = (YQDialog *) yDialog() ) )
    {
	dia->makeDefaultButton( qt_pushbutton );
    }
    else	// the yDialog parent may not be set yet!
    {
	qt_pushbutton->setDefault( true );
    }
    update();
}


void YQPushButton::hit()
{
    makeDefaultButton();
    yuiqt->returnNow(YUIInterpreter::ET_WIDGET, this);
}


bool YQPushButton::eventFilter( QObject *obj, QEvent *event )
{
    if ( event->type() == QEvent::FocusIn )
    {
	// y2milestone( "FocusIn %s", (const char *) qt_pushbutton->text() );
	makeDefaultButton();
	return false;	// event processed?
    }
#if 0
    else if ( event->type() == QEvent::FocusOut )
    {
	y2milestone( "FocusOut %s - new focus in %s",
		     (const char *) qt_pushbutton->text(),
		     focusWidget() ? (const char *) focusWidget()->className() : "nowhere" );
	return false;	// event processed?
    }
#endif
    return QWidget::eventFilter( obj, event );   
}



bool YQPushButton::setKeyboardFocus()
{
    makeDefaultButton();
    qt_pushbutton->setFocus();

    return true;
}


#include "YQPushButton.moc.cc"
