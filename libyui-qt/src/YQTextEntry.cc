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

  File:	      YQTextEntry.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include <qlineedit.h>
#include <qlabel.h>
#define y2log_component "qt-ui"
#include <ycp/y2log.h>

using std::max;

#include "utf8.h"
#include "YUIQt.h"
#include "QY2CharValidator.h"
#include "YQTextEntry.h"

#define SPACING			4	// between subwidgets
#define MARGIN			4	// around the widget


YQTextEntry::YQTextEntry( QWidget *parent, YWidgetOpt &opt,
			 const YCPString &label, const YCPString &text)
    : QVBox(parent)
    , YTextEntry(opt, label)
    , _validator( 0 )
{
    setWidgetRep( this );

    setSpacing( SPACING );
    setMargin( MARGIN );

    qt_label = new QLabel( fromUTF8( label->value() ), this );
    qt_label->setTextFormat( QLabel::PlainText );
    qt_label->setFont( YUIQt::ui()->currentFont() );

    if ( label->value() == "" )
	qt_label->hide();

    qt_lineedit = new QLineEdit(this);
    qt_lineedit->setFont(YUIQt::ui()->currentFont());
    qt_lineedit->setText(fromUTF8(text->value()));

    qt_label->setBuddy(qt_lineedit);

    if (opt.passwordMode.value())
	qt_lineedit->setEchoMode(QLineEdit::Password);

    shrinkable = opt.isShrinkable.value();

    connect(qt_lineedit, SIGNAL(textChanged(const QString&)), this, SLOT(changed(const QString &)));
}


void YQTextEntry::setEnabling(bool enabled)
{
    qt_lineedit->setEnabled(enabled);
}


long YQTextEntry::nicesize(YUIDimension dim)
{
    if (dim == YD_HORIZ)
    {
	long minSize = shrinkable ? 15 : 200;
	long hintWidth = qt_label->sizeHint().width() + margin();

	if ( ! qt_label->isVisible() )
	    hintWidth = 0;

	return max( minSize, hintWidth );
    }
    else
    {
	return sizeHint().height();
    }
}


void YQTextEntry::setSize(long newWidth, long newHeight)
{
    resize(newWidth, newHeight);
}

void YQTextEntry::setText(const YCPString &text)
{
    qt_lineedit->setText(fromUTF8(text->value()));
}


YCPString YQTextEntry::getText()
{
    return YCPString(toUTF8(qt_lineedit->text()));
}


void YQTextEntry::setLabel(const YCPString &label)
{
    qt_label->setText(fromUTF8(label->value()));
    YTextEntry::setLabel(label);
}


void YQTextEntry::setValidChars( const YCPString &newValidChars )
{
    if ( _validator )
    {
	_validator->setValidChars( fromUTF8( newValidChars->value() ) );
    }
    else
    {
	_validator = new QY2CharValidator( fromUTF8( newValidChars->value() ), this );
	qt_lineedit->setValidator( _validator );

	// No need to delete the validator in the destructor - Qt will take
	// care of that since it's a QObject with a parent!
    }

    YTextEntry::setValidChars( newValidChars );
}


bool YQTextEntry::setKeyboardFocus()
{
    qt_lineedit->setFocus();
    qt_lineedit->selectAll();

    return true;
}


// slots

void YQTextEntry::changed(const QString&)
{
    if (getNotify())
	YUIQt::ui()->returnNow(YUIInterpreter::ET_WIDGET, this);
}




#include "YQTextEntry.moc.cc"

