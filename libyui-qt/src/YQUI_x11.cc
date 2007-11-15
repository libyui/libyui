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

  Textdomain "packages-qt"

/-*/


#include <qcursor.h>
#include <qwidgetstack.h>
#include <qvbox.h>
#include <qwidgetlist.h>
#include <qtextcodec.h>
#include <qregexp.h>
#include <qlocale.h>
#include <qmessagebox.h>


#define y2log_component "qt-ui"
#include <ycp/y2log.h>

#include "YEvent.h"
#include "YQDialog.h"
#include "YQi18n.h"
#include "QY2Settings.h"
#include "YQUI.h"

// Include low-level X headers AFTER Qt headers:
// X.h pollutes the global namespace (!!!) with pretty useless #defines
// like "Above", "Below" etc. that clash with some Qt headers.
#include <X11/Xlib.h>


int YQUI::getDisplayWidth()
{
    return qApp->desktop()->width();
}


int YQUI::getDisplayHeight()
{
    return qApp->desktop()->height();
}


int YQUI::getDisplayDepth()
{
    return QColor::numBitPlanes();
}


long YQUI::getDisplayColors()
{
    return 1L << QColor::numBitPlanes();
}


int YQUI::getDefaultWidth()
{
    return _default_size.width();
}


int YQUI::getDefaultHeight()
{
    return _default_size.height();
}


int YQUI::defaultSize(YUIDimension dim) const
{
    if ( haveWM() )
	return dim == YD_HORIZ ? _default_size.width() : _default_size.height();
    else
	return dim == YD_HORIZ ? qApp->desktop()->width() : qApp->desktop()->height();
}


QWidget* YQUI::mainWidget()
{
    return _main_win;
};


void YQUI::beep()
{
    qApp->beep();
}


void
YQUI::busyCursor( void )
{
    qApp->setOverrideCursor( Qt::busyCursor );
}


void
YQUI::normalCursor( void )
{
    if ( _busy_cursor_timer.isActive() )
	_busy_cursor_timer.stop();

    while ( qApp->overrideCursor() )
	qApp->restoreOverrideCursor();
}


const QFont &YQUI::currentFont()
{
    /**
     * Brute force approach to make sure we'll really get a complete Unicode font:
     * Explicitly load the one font that we made sure to contain all required
     * characters, including Latin1, Latin2, Japanese, Korean, and the
     * characters used for glyphs.
     *
     * There are many fonts that claim to be Unicode, but most of them contain
     * just a sorry excuse for a complete Unicode character set. Qt can't know
     * how complete a font is, so it chooses one that might be better in otherf
     * aspects, but lacks necessary characters.
     **/

    if ( ! _loaded_current_font )
    {
	if ( autoFonts() )
	{
	    pickAutoFonts();

	    _current_font = QFont( _font_family );
	    _current_font.setPixelSize( _auto_normal_font_size );
	    _current_font.setWeight( QFont::Normal );

	    y2milestone( "Loaded %d pixel font: %s", _auto_normal_font_size,
			 (const char *) _current_font.toString() );

	    qApp->setFont( _current_font, true );	// font, informWidgets
	}
	else
	{
	    _current_font = qApp->font();
	}

	_loaded_current_font = true;
    }

    return _current_font;
}


const QFont &YQUI::boldFont()
{
    if ( ! _loaded_bold_font )
    {
	_bold_font = currentFont();
	_bold_font.setBold( true );
	_loaded_bold_font = true;
    }

    return _bold_font;
}


const QFont &YQUI::headingFont()
{
    /**
     * Brute force load the heading font - see currentFont() above for more.
     **/

    if ( ! _loaded_heading_font )
    {
	if ( autoFonts() )
	{
	    pickAutoFonts();

	    _heading_font = QFont( _font_family );
	    _heading_font.setPixelSize( _auto_heading_font_size );
	    _heading_font.setWeight( QFont::Bold );

	    y2milestone( "Loaded %d pixel bold font: %s", _auto_heading_font_size,
			 (const char *) _heading_font.toString() );
	}
	else
	{
	    _heading_font = QFont( _font_family, 14, QFont::Bold );
	}
	_loaded_heading_font = true;
    }

    return _heading_font;
}


void YQUI::setAllFontsDirty()
{
    _loaded_current_font = false;
    _loaded_heading_font = false;
    _loaded_bold_font    = false;
}



void YQUI::pickAutoFonts()
{
    if ( _auto_normal_font_size >= 0 )	// Use cached values
	return;

    int x = _default_size.width();
    int y = _default_size.height();

    int normal  = 10;
    int heading	= 12;

    if ( x >= 800 && y >= 600 )
    {
	normal	= 10;
	heading	= 12;
    }

    if ( x >= 1024 && y >= 768 )
    {
	normal	= 12;
	heading	= 14;
    }

    if ( x >= 1280 && y >= 1024 )
    {
	normal	= 14;
	heading	= 18;
    }

    if ( x >= 1400 )
    {
	normal	= 16;
	heading	= 20;
    }

    if ( x >= 1600 )
    {
	normal	= 18;
	heading	= 24;
    }

    if ( x >= 2048 )	// Sounds futuristic? Just wait one or two years...
    {
	normal	= 20;
	heading	= 28;
    }

    _auto_normal_font_size  = normal;
    _auto_heading_font_size = heading;

    y2milestone( "Selecting auto fonts - normal: %d, heading: %d (bold)",
		 _auto_normal_font_size, _auto_heading_font_size );
}


void YQUI::toggleVisionImpairedPalette()
{
    if ( _usingVisionImpairedPalette )
    {
	qApp->setPalette( normalPalette(),
			  true );  // informWidgets

	_usingVisionImpairedPalette = false;
    }
    else
    {
	qApp->setPalette( visionImpairedPalette(),
			  true );  // informWidgets

	_usingVisionImpairedPalette = true;
    }
}


QPalette YQUI::visionImpairedPalette()
{
    const QColor dark  ( 0x20, 0x20, 0x20 );

    QColorGroup activeCg;	// for the active window (the one with the keyboard focus)

    activeCg.setColor( QColorGroup::Background,		Qt::black 	);
    activeCg.setColor( QColorGroup::Foreground,		Qt::cyan	);
    activeCg.setColor( QColorGroup::Text,		Qt::cyan	);
    activeCg.setColor( QColorGroup::Base,		dark		);
    activeCg.setColor( QColorGroup::Button,		dark		);
    activeCg.setColor( QColorGroup::ButtonText,		Qt::green	);
    activeCg.setColor( QColorGroup::Highlight,		Qt::yellow	);
    activeCg.setColor( QColorGroup::HighlightedText,	Qt::black	);

    QColorGroup inactiveCg;	// for other windows (those that don't have the keyboard focus)

    inactiveCg.setColor( QColorGroup::Background,	Qt::black 	);
    inactiveCg.setColor( QColorGroup::Foreground,	Qt::cyan	);
    inactiveCg.setColor( QColorGroup::Text,		Qt::cyan	);
    inactiveCg.setColor( QColorGroup::Base,		dark		);
    inactiveCg.setColor( QColorGroup::Button,		dark		);
    inactiveCg.setColor( QColorGroup::ButtonText,	Qt::green	);

    QColorGroup disabledCg;	// for disabled widgets

    disabledCg.setColor( QColorGroup::Background,	Qt::black 	);
    disabledCg.setColor( QColorGroup::Foreground,	Qt::gray	);
    disabledCg.setColor( QColorGroup::Text,		Qt::gray	);
    disabledCg.setColor( QColorGroup::Base,		dark		);
    disabledCg.setColor( QColorGroup::Button,		dark		);
    disabledCg.setColor( QColorGroup::ButtonText,	Qt::gray	);

    QPalette pal( activeCg, disabledCg, inactiveCg );

    return pal;
}


bool YQUI::close()
{
    sendEvent( new YCancelEvent() );
    return true;
}


bool YQUI::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::Close )
    {
	// Handle WM_CLOSE - but only if it comes from a dialog that is managed by the UI,
	// not from an independent Qt dialog (such as the package selector popups)

	QWidget * objDialog = 0;

	if ( obj && obj->isWidgetType() )
	{
	    objDialog = (QWidget *) obj;
	    objDialog = objDialog->topLevelWidget();
	}

	if ( objDialog &&
	     ( objDialog == mainWidget() ||
	       objDialog == (QObject *) YDialog::currentDialog()->widgetRep() ) )
	{
	    emit wmClose();

	    if ( ! _wm_close_blocked )
	    {
		// Don't simply close the application window, return from UserInput()
		// with `id(`cancel) and let the YCP application decide how to handle
		// that (e.g., ask for confirmation).

		y2debug( "Caught window close event - returning with `cancel" );
		sendEvent( new YCancelEvent() );
	    }

	    return true;	// Event processed
	}
    }
    else if ( ev->type() == QEvent::Show )
    {
	if ( obj == _main_win )
	{
	    if ( _main_dialog_id > 0 )
	    {
		// Work around QWidgetStack bug: The last raiseWidget() call
		// (from closeDialog() ) might have failed if the widget was
		// invisible at that time, e.g., because the user had switched to
		// some other virtual desktop (bugzilla bug #11310)

		_widget_stack->raiseWidget( _main_dialog_id );
	    }
	}
	else
	{
	    return showEventFilter( obj, ev );
	}
    }

    return false;	// Don't stop event processing
}


bool YQUI::showEventFilter( QObject * obj, QEvent * ev )
{
    if ( ! haveWM() )
    {
	// Make sure newly opened windows get the keyboard focus even without a
	// window manager. Otherwise the app might be unusable without a mouse.

	QWidget * widget = dynamic_cast<QWidget *> (obj);

	if ( widget )
	    widget->setActiveWindow();
    }

    return false;	// Don't stop event processing
}


void YQUI::loadPredefinedQtTranslations()
{
    QString path = QT_LOCALEDIR;
    QString language = QLocale::system().name();

    QString trans_file = QString( "qt_%1.qm")
              .arg( language.lower().replace('_','-') );

    if ( path.isEmpty() )
    {
	y2warning( "Qt locale directory not set - "
		   "no translations for predefined Qt dialogs" );
	return;
    }

    _qtTranslations.load( trans_file, path );

    if ( _qtTranslations.isEmpty() )
    {
	// try fallback
	trans_file = QString( "qt_%1.qm").arg( language.lower().left(2) );
	_qtTranslations.load( trans_file, path );
    }

    if ( _qtTranslations.isEmpty() )
    {
	y2warning( "Can't load translations for predefined Qt dialogs from %s/%s",
		   (const char *) path, (const char *) trans_file );
    }
    else
    {
	y2milestone( "Loaded translations for predefined Qt dialogs from %s/%s",
		     (const char *) path, (const char *) trans_file );

	qApp->installTranslator( & _qtTranslations );
    }


    // Force reverse layout for Arabic and Hebrew

    if ( ( language.startsWith( "ar" ) ||	// Arabic
	   language.startsWith( "he" ) )	// Hebrew
	 && ! reverseLayout() )
    {
	y2warning( "Using fallback rule for reverse layout for language '%s'",
		   (const char *) language );

	qApp->setReverseLayout( true );
    }
}


void YQUI::setLangFonts( const YCPString & language )
{
    QString old_font_family = _font_family;

    if ( ! _lang_fonts )
    {
	_lang_fonts = new QY2Settings( LANG_FONTS_FILE );
	CHECK_PTR( _lang_fonts );

	if ( _lang_fonts->readError() )
	    y2error( "Error reading %s", (const char *) _lang_fonts->fileName() );
	else
	    y2milestone( "%s read OK", (const char *) _lang_fonts->fileName() );
    }

    QString lang = language->value().c_str();
    QString key;

    if ( ! _lang_fonts->hasKey( fontKey( lang ) ) )	// "zh_CN.UTF8" etc.
    {
	lang.replace( QRegExp( "\\..*$" ), "" );	// Cut off trailing encoding (".UTF8")

	if ( ! _lang_fonts->hasKey( fontKey( lang ) ) )
	    lang.replace( QRegExp( "_.*$" ), "" );	// Cut off trailing country ("_CN")
    }

    if ( _lang_fonts->hasKey( fontKey( lang ) ) )
    {
	_font_family = _lang_fonts->get( fontKey( lang ), "Sans Serif" );
	y2milestone( "%s = \"%s\"", (const char *) fontKey( lang ), (const char *) _font_family );
    }
    else
    {
	_font_family = _lang_fonts->get( fontKey( "" ), "Sans Serif" );
	y2milestone( "Using fallback for %s: font = \"%s\"",
		     (const char *) lang, (const char *) _font_family );
    }

    if ( _font_family != old_font_family && ! _font_family.isEmpty() )
    {
	setAllFontsDirty();
	int size = qApp->font().pointSize();
	QFont font( _font_family );
	font.setPointSize( size );
	qApp->setFont( font, true );	// font, informWidgets
	y2milestone( "Reloading fonts - now using \"%s\"",
		     (const char *) font.toString() );
    }
    else
    {
	y2debug( "No font change" );
    }
}


QString YQUI::fontKey( const QString & lang )
{
    if ( lang.isEmpty() )
	return "font";
    else
	return QString( "font[%1]").arg( lang );
}


/**
 * UI-specific conversion from logical layout spacing units (80x25)
 * to device dependent units (640x480).
 **/
int YQUI::deviceUnits( YUIDimension dim, float size )
{
    if ( dim==YD_HORIZ )	size *= ( 640.0/80 );
    else			size *= ( 480.0/25 );

    return (int) ( size + 0.5 );
}


/**
 * Default conversion from device dependent layout spacing units (640x480)
 * to logical layout units (80x25).
 *
 * This default function assumes 80x25 units.
 * Derived UIs may want to reimplement this.
 **/
float YQUI::layoutUnits( YUIDimension dim, int device_units )
{
    float size = (float) device_units;

    if ( dim==YD_HORIZ )	size *= ( 80/640.0 );
    else			size *= ( 25/480.0 );

    return size;
}


void YQUI::maybeLeftHandedUser()
{
    if ( _askedForLeftHandedMouse )
	return;


    QString message =
	_( "You clicked the right mouse button "
	   "where a left-click was expected."
	   "\n"
	   "Switch left and right mouse buttons?"
	   );
    int button = QMessageBox::question( 0,
					// Popup dialog caption
					_( "Unexpected Click" ),
					message,
					QMessageBox::Yes | QMessageBox::Default,
					QMessageBox::No,
					QMessageBox::Cancel | QMessageBox::Escape );

    if ( button == QMessageBox::Yes )
    {

	const char * command = 
	    _leftHandedMouse ?
	    "xmodmap -e \"pointer = 1 2 3\"":	// switch back to right-handed mouse
	    "xmodmap -e \"pointer = 3 2 1\"";	// switch to left-handed mouse

	_leftHandedMouse	 = ! _leftHandedMouse; 	// might be set repeatedly!
	_askedForLeftHandedMouse = false;	// give the user a chance to switch back
	y2milestone( "Switching mouse buttons: %s", command );
	
	system( command );
    }
    else if ( button == 1 )	// No
    {
	_askedForLeftHandedMouse = true;
    }
}



// EOF
