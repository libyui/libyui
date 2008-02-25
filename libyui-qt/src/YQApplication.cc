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

  File:		YQApplication.cc

  Author:	Stefan Hundhammer <sh@suse.de>

  Textdomain	"packages-qt"
/-*/


#include <unistd.h>	// access()

#include <QApplication>
#include <QLocale>
#include <QRegExp>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QMessageBox>

#define YUILogComponent "qt-ui"
#include "YUILog.h"
#include "YUISymbols.h"
#include "YQUI.h"

#include "utf8.h"
#include "YQi18n.h"

#include "YQApplication.h"
#include "YQPackageSelectorPluginStub.h"


YQApplication::YQApplication()
    : YApplication()
    , _currentFont( 0 )
    , _headingFont( 0 )
    , _boldFont( 0 )
    , _fontFamily( "Sans Serif" )
    , _langFonts( 0 )
    , _qtTranslations( 0 )
    , _autoFonts( false )
    , _autoNormalFontSize( -1 )
    , _autoHeadingFontSize( -1 )
      , _leftHandedMouse( false )
    , _askedForLeftHandedMouse( false )
{
    yuiDebug() << "YQApplication constructor start" << endl;

    setIconBasePath( ICONDIR "/icons/22x22/apps/" );
    loadPredefinedQtTranslations();

    yuiDebug() << "YQApplication constructor end" << endl;
}


YQApplication::~YQApplication()
{
    if ( _langFonts )
	delete _langFonts;

    if ( _qtTranslations )
	delete _qtTranslations;

    deleteFonts();
}


void
YQApplication::setLanguage( const string & language,
			    const string & encoding )
{
    YApplication::setLanguage( language, encoding );
    loadPredefinedQtTranslations();
    setLangFonts( language, encoding );
}



void
YQApplication::loadPredefinedQtTranslations()
{
    QString path = QT_LOCALEDIR;
    QString language = QLocale::system().name();

    QString transFile = QString( "qt_%1.qm")
              .arg( language.toLower().replace('_','-') );

    if ( path.isEmpty() )
    {
	yuiWarning() << "Qt locale directory not set - "
		     << "no translations for predefined Qt dialogs"
		     << endl;
	return;
    }

    if ( ! _qtTranslations )
	_qtTranslations = new QTranslator();

    _qtTranslations->load( transFile, path );

    if ( _qtTranslations->isEmpty() )
    {
	// try fallback
	transFile = QString( "qt_%1.qm").arg( language.toLower().left(2) );
	_qtTranslations->load( transFile, path );
    }

    if ( _qtTranslations->isEmpty() )
    {
	yuiWarning() << "Can't load translations for predefined Qt dialogs from "
		     << path << "/" << transFile << endl;
    }
    else
    {
	yuiMilestone() << "Loaded translations for predefined Qt dialogs from "
		       << path << "/" << transFile << endl;

	qApp->installTranslator( _qtTranslations );
    }


    // Force reverse layout for Arabic and Hebrew

    if ( ( language.startsWith( "ar" ) ||	// Arabic
	   language.startsWith( "he" ) )	// Hebrew
	 && ! (qApp->layoutDirection() == Qt::RightToLeft) )
    {
	yuiWarning() << "Using fallback rule for reverse layout for language \""
		     << language << "\"" << endl;

	qApp->setLayoutDirection( Qt::RightToLeft );
    }
}


void
YQApplication::setLangFonts( const string & language, const string & encoding )
{
    QString oldFontFamily = _fontFamily;

    if ( ! _langFonts )
    {
	_langFonts = new QY2Settings( LANG_FONTS_FILE );
	Q_CHECK_PTR( _langFonts );

	if ( _langFonts->readError() )
	    yuiError() << "Error reading " << _langFonts->fileName() << endl;
	else
	    yuiMilestone() <<  _langFonts->fileName() << " read OK" << endl;
    }

    QString lang = language.c_str();

    if ( ! encoding.empty() )
	lang += QString( "." ) + encoding.c_str();

    QString key;

    if ( ! _langFonts->hasKey( fontKey( lang ) ) )	// Try with encoding ("zh_CN.UTF8" etc.)
    {
	lang = language.c_str();			// Try without encoding ("zh_CN")

	if ( ! _langFonts->hasKey( fontKey( lang ) ) )
	    lang.replace( QRegExp( "_.*$" ), "" );	// Cut off trailing country ("_CN")
    }

    if ( _langFonts->hasKey( fontKey( lang ) ) )
    {
	_fontFamily = _langFonts->get( fontKey( lang ), "Sans Serif" );
	yuiMilestone() << fontKey( lang ) << " = \"" << _fontFamily << "\"" << endl;
    }
    else
    {
	_fontFamily = _langFonts->get( fontKey( "" ), "Sans Serif" );
	yuiMilestone() << "Using fallback for " << lang
		       << ": font = \"" << _fontFamily << "\""
		       << endl;
    }

    if ( _fontFamily != oldFontFamily && ! _fontFamily.isEmpty() )
    {
	yuiMilestone() << "New font family: " << _fontFamily << endl;
	deleteFonts();
	int size = qApp->font().pointSize();
	QFont font( _fontFamily );
	font.setPointSize( size );
	qApp->setFont(font);	// font, informWidgets
	yuiMilestone() << "Reloading fonts - now using \"" << font.toString() << "\"" << endl;
    }
    else
    {
	yuiDebug() << "No font change" << endl;
    }
}


QString
YQApplication::fontKey( const QString & lang )
{
    if ( lang.isEmpty() )
	return "font";
    else
	return QString( "font[%1]").arg( lang );
}


const QFont &
YQApplication::currentFont()
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

    if ( ! _currentFont )
    {
	if ( autoFonts() )
	{
	    pickAutoFonts();

	    _currentFont = new QFont( _fontFamily );
	    _currentFont->setPixelSize( _autoNormalFontSize );
	    _currentFont->setWeight( QFont::Normal );

	    yuiMilestone() << "Loaded " <<  _autoNormalFontSize
			   << " pixel font: " << _currentFont->toString()
			   << endl;

	    qApp->setFont( * _currentFont);	// font, informWidgets
	}
	else
	{
	    // yuiDebug() << "Copying QApplication::font()" << endl;
	    _currentFont = new QFont( qApp->font() );
	}
    }

    return * _currentFont;
}


const QFont &
YQApplication::boldFont()
{
    if ( ! _boldFont )
    {
	_boldFont = new QFont( currentFont() );
	_boldFont->setBold( true );
    }

    return * _boldFont;
}


const QFont &
YQApplication::headingFont()
{
    /**
     * Brute force load the heading font - see currentFont() above for more.
     **/

    if ( ! _headingFont )
    {
	if ( autoFonts() )
	{
	    pickAutoFonts();

	    _headingFont = new QFont( _fontFamily );
	    _headingFont->setPixelSize( _autoHeadingFontSize );
	    _headingFont->setWeight( QFont::Bold );

	    yuiMilestone() << "Loaded " << _autoHeadingFontSize
			   << " pixel bold font: " << _headingFont->toString()
			   << endl;
	}
	else
	{
	    _headingFont = new QFont( _fontFamily, 14, QFont::Bold );
	}
    }

    return * _headingFont;
}


void
YQApplication::deleteFonts()
{
    if ( _currentFont )
	delete _currentFont;

    if ( _headingFont )
	delete _headingFont;

    if ( _boldFont )
	delete _boldFont;

    _currentFont = 0;
    _headingFont = 0;
    _boldFont	 = 0;
}


void
YQApplication::setAutoFonts( bool useAutoFonts )
{
    _autoFonts = useAutoFonts;
}


void
YQApplication::pickAutoFonts()
{
    if ( _autoNormalFontSize >= 0 )	// Use cached values
	return;

    int x = defaultWidth();
    int y = defaultHeight();

    int normal	= 10;
    int heading = 12;

    if ( x >= 800 && y >= 600 )
    {
	normal	= 10;
	heading = 12;
    }

    if ( x >= 1024 && y >= 768 )
    {
	normal	= 12;
	heading = 14;
    }

    if ( x >= 1280 && y >= 1024 )
    {
	normal	= 14;
	heading = 18;
    }

    if ( x >= 1400 )
    {
	normal	= 16;
	heading = 20;
    }

    if ( x >= 1600 )
    {
	normal	= 18;
	heading = 24;
    }

    if ( x >= 2048 )	// Sounds futuristic? Just wait one or two years...
    {
	normal	= 20;
	heading = 28;
    }

    _autoNormalFontSize	 = normal;
    _autoHeadingFontSize = heading;

    yuiMilestone() << "Selecting auto fonts - normal: " << _autoNormalFontSize
		   << ", heading: " <<  _autoHeadingFontSize  << " (bold)"
		   << endl;
}


string
YQApplication::glyph( const string & sym )
{
    QChar unicodeChar;

    // Hint: Use the 'xfd' program to view characters available in the Unicode font.

    if      ( sym == YUIGlyph_ArrowLeft         )       unicodeChar = QChar( 0x2190 );
    else if ( sym == YUIGlyph_ArrowRight        )       unicodeChar = QChar( 0x2192 );
    else if ( sym == YUIGlyph_ArrowUp           )       unicodeChar = QChar( 0x2191 );
    else if ( sym == YUIGlyph_ArrowDown         )       unicodeChar = QChar( 0x2193 );
    else if ( sym == YUIGlyph_CheckMark         )       unicodeChar = QChar( 0x2714 );
    else if ( sym == YUIGlyph_BulletArrowRight  )       unicodeChar = QChar( 0x279c );
    else if ( sym == YUIGlyph_BulletCircle      )       unicodeChar = QChar( 0x274d );
    else if ( sym == YUIGlyph_BulletSquare      )       unicodeChar = QChar( 0x274f );
    else return "";

    return toUTF8( QString( unicodeChar ) );
}


string
YQApplication::askForExistingDirectory( const string & startDir,
					const string & headline )
{
#if 0
    normalCursor();
#endif

    QString dirName =
	QFileDialog::getExistingDirectory( 0,				// parent
	 				   fromUTF8( headline ) ,	// caption
					   fromUTF8( startDir ));	// dir



#if 0
    busyCursor();
#endif

    return toUTF8( dirName );
}


string
YQApplication::askForExistingFile( const string & startWith,
				   const string & filter,
				   const string & headline )
{
#if 0
    normalCursor();
#endif

    QString fileName =
	QFileDialog::getOpenFileName( 0, 				// parent
				      fromUTF8( headline ) ,		// caption
				      fromUTF8( startWith ),		// dir
				      fromUTF8( filter ));		// filter

#if 0
    busyCursor();
#endif

    return toUTF8( fileName );
}


string
YQApplication::askForSaveFileName( const string & startWith,
				   const string & filter,
				   const string & headline )
{
#if 0
    normalCursor();
#endif

    QString fileName = askForSaveFileName( fromUTF8( startWith ),
					   fromUTF8( filter ),
					   fromUTF8( headline ) );
#if 0
    busyCursor();
#endif

    return toUTF8( fileName );
}



QString
YQApplication::askForSaveFileName( const QString & startWith,
				   const QString & filter,
				   const QString & headline )
{
    QString fileName;
    bool tryAgain = false;

    do
    {
	// Leave the mouse cursor alone - this function might be called from
	// some other widget, not only from UI::AskForSaveFileName().

	fileName = QFileDialog::getSaveFileName( 0, 			// parent
						 headline,		// caption
						 startWith,		// dir
						 filter );		// filter

	if ( fileName.isEmpty() )	// this includes fileName.isNull()
	    return QString::null;


	if ( access( QFile::encodeName( fileName ), F_OK ) == 0 )	// file exists?
	{
	    QString msg;

	    if ( access( QFile::encodeName( fileName ), W_OK ) == 0 )
	    {
		// Confirm if the user wishes to overwrite an existing file
		msg = ( _( "%1 exists! Really overwrite?" ) ).arg( fileName );
	    }
	    else
	    {
		// Confirm if the user wishes to overwrite a write-protected file %1
		msg = ( _( "%1 exists and is write-protected!\nReally overwrite?" ) ).arg( fileName );
	    }

	    int buttonNo = QMessageBox::information( 0,	// parent widget
						     // Translators: Window title for confirmation dialog
						     _( "Confirm"   ),
						     msg,
						     _( "C&ontinue" ),
						     _( "&Cancel"   ) );
	    tryAgain = ( buttonNo != 0 );
	}

    } while ( tryAgain );

    return fileName;
}


int
YQApplication::displayWidth()
{
    return qApp->desktop()->width();
}


int
YQApplication::displayHeight()
{
    return qApp->desktop()->height();
}


int
YQApplication::displayDepth()
{
    return qApp->desktop()->depth();
}


long
YQApplication::displayColors()
{
    return 1L << qApp->desktop()->depth();
}


int
YQApplication::defaultWidth()
{
    return YQUI::ui()->defaultSize( YD_HORIZ );
}


int
YQApplication::defaultHeight()
{
    return YQUI::ui()->defaultSize( YD_VERT );
}


bool
YQApplication::leftHandedMouse()
{
    return _leftHandedMouse;
}


void
YQApplication::maybeLeftHandedUser()
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
	yuiMilestone() << "Switching mouse buttons: " << command << endl;

	system( command );
    }
    else if ( button == 1 )	// No
    {
	_askedForLeftHandedMouse = true;
    }
}


int YQApplication::deviceUnits( YUIDimension dim, float layoutUnits )
{
    if ( dim==YD_HORIZ )	layoutUnits *= ( 640.0/80 );
    else			layoutUnits *= ( 480.0/25 );

    return (int) ( layoutUnits + 0.5 );
}


float YQApplication::layoutUnits( YUIDimension dim, int deviceUnits )
{
    float size = (float) deviceUnits;

    if ( dim==YD_HORIZ )	size *= ( 80/640.0 );
    else			size *= ( 25/480.0 );

    return size;
}


void YQApplication::beep()
{
    qApp->beep();
}


void YQApplication::busyCursor()
{
    YQUI::ui()->busyCursor();
}


void YQApplication::normalCursor()
{
    YQUI::ui()->normalCursor();
}


void YQApplication::makeScreenShot( const string & fileName )
{
    YQUI::ui()->makeScreenShot( fileName );
}


YQPackageSelectorPluginStub *
YQApplication::packageSelectorPlugin()
{
    static YQPackageSelectorPluginStub * plugin = 0;

    if ( ! plugin )
    {
        plugin = new YQPackageSelectorPluginStub();

        // This is a deliberate memory leak: If an application requires a
        // PackageSelector, it is a package selection application by
        // definition. In this case, the ncurses_pkg plugin is intentionally
        // kept open to avoid repeated start-up cost of the plugin and libzypp.
    }

    return plugin;
}



#include "YQApplication.moc"
