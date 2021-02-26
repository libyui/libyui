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

  File:	      	YUIQt_x11.cc

  Author:     	Stefan Hundhammer <sh@suse.de>

  Textdomain "qt"

/-*/

#include <QEvent>
#include <QCursor>
#include <QWidget>
#include <QTextCodec>
#include <QRegExp>
#include <QLocale>
#include <QMessageBox>
#include <QColorGroup>
#include <QDesktopWidget>


#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "YEvent.h"
#include "YQDialog.h"
#include "YQi18n.h"
#include "YQUI.h"

// Include low-level X headers AFTER Qt headers:
// X.h pollutes the global namespace (!!!) with pretty useless #defines
// like "Above", "Below" etc. that clash with some Qt headers.
#include <X11/Xlib.h>


int YQUI::defaultSize(YUIDimension dim) const
{
    return dim == YD_HORIZ ? _defaultSize.width() : _defaultSize.height();
}


void YQUI::busyCursor()
{
    qApp->setOverrideCursor( Qt::BusyCursor );
}


void YQUI::normalCursor()
{
    if ( _busyCursorTimer->isActive() )
	_busyCursorTimer->stop();

    while ( qApp->overrideCursor() )
	qApp->restoreOverrideCursor();
}


void YQUI::toggleVisionImpairedPalette()
{
    if ( _usingVisionImpairedPalette )
    {
	qApp->setPalette( normalPalette());  // informWidgets

	_usingVisionImpairedPalette = false;
    }
    else
    {
	qApp->setPalette( visionImpairedPalette() );  // informWidgets

	_usingVisionImpairedPalette = true;
    }
}


QPalette YQUI::visionImpairedPalette()
{
    const QColor dark  ( 0x20, 0x20, 0x20 );
    QPalette pal;

    // for the active window (the one with the keyboard focus)
    pal.setColor( QPalette::Active, QPalette::Background,	Qt::black 	);
    pal.setColor( QPalette::Active, QPalette::Foreground,	Qt::cyan	);
    pal.setColor( QPalette::Active, QPalette::Text,		Qt::cyan	);
    pal.setColor( QPalette::Active, QPalette::Base,		dark		);
    pal.setColor( QPalette::Active, QPalette::Button,		dark		);
    pal.setColor( QPalette::Active, QPalette::ButtonText,	Qt::green	);
    pal.setColor( QPalette::Active, QPalette::Highlight,	Qt::yellow	);
    pal.setColor( QPalette::Active, QPalette::HighlightedText,	Qt::black	);

    // for other windows (those that don't have the keyboard focus)
    pal.setColor( QPalette::Inactive, QPalette::Background,	Qt::black 	);
    pal.setColor( QPalette::Inactive, QPalette::Foreground,	Qt::cyan	);
    pal.setColor( QPalette::Inactive, QPalette::Text,		Qt::cyan	);
    pal.setColor( QPalette::Inactive, QPalette::Base,		dark		);
    pal.setColor( QPalette::Inactive, QPalette::Button,		dark		);
    pal.setColor( QPalette::Inactive, QPalette::ButtonText,	Qt::green	);

    // for disabled widgets
    pal.setColor( QPalette::Disabled, QPalette::Background,	Qt::black 	);
    pal.setColor( QPalette::Disabled, QPalette::Foreground,	Qt::gray	);
    pal.setColor( QPalette::Disabled, QPalette::Text,		Qt::gray	);
    pal.setColor( QPalette::Disabled, QPalette::Base,		dark		);
    pal.setColor( QPalette::Disabled, QPalette::Button,		dark		);
    pal.setColor( QPalette::Disabled, QPalette::ButtonText,	Qt::gray	);

    return pal;
}


// FIXME: Does this still do anything now that YQUI is no longer a QObject?
bool YQUI::close()
{
    sendEvent( new YCancelEvent() );
    return true;
}



// EOF
