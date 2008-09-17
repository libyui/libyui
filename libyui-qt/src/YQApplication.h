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

  File:	      YQApplication.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YQApplication_h
#define YQApplication_h

#include <Qt/qobject.h>
#include <Qt/qtranslator.h>
#include <qfont.h>

#include "YQPackageSelectorPluginStub.h"
#include "YApplication.h"

class QSettings;
class YQPackageSelectorPlugin;


class YQApplication: public QObject, public YApplication
{
    Q_OBJECT

protected:

    friend class YQUI;

    /**
     * Constructor.
     *
     * Use YUI::app() to get the singleton for this class.
     **/
    YQApplication();

    /**
     * Destructor.
     **/
    virtual ~YQApplication();


public:

    /**
     * Set language and encoding for the locale environment ($LANG).
     *
     * Load UI-internal translations (e.g. for predefined dialogs like
     * file selection) and fonts.
     *
     * 'language' is the ISO short code ("de_DE", "en_US", ...).
     *
     * 'encoding' an (optional) encoding ("utf8", ...) that will be appended if
     *  present.
     *
     * Reimplemented from YApplication.
     **/
    virtual void setLanguage( const string & language,
			      const string & encoding = string() );

    /**
     * Load translations for Qt's predefined dialogs like file selection box
     * etc.
     **/
    void loadPredefinedQtTranslations();

    /**
     * Set the layout direction (left-to-right or right-to-left) from
     * 'language'.
     **/
    void setLayoutDirection( const string & language );

    /**
     * Set fonts according to the specified language and encoding.
     *
     * This is most important for some Asian languages that have overlaps in
     * the Unicode table, like Japanese vs. Chinese.
     **/
    void setLangFonts( const string & language,
		       const string & encoding = string() );

    /**
     * Returns the application's default font.
     **/
    const QFont & currentFont();

    /**
     * Returns the application's default bold font.
     **/
    const QFont & boldFont();

    /**
     * Returns the application's heading font.
     **/
    const QFont & headingFont();

    /**
     * Delete the fonts so they will be reloaded upon their next usage.
     **/
    void deleteFonts();

    /**
     * Determine good fonts based on defaultsize geometry and set
     * _auto_normal_font_size and _auto_heading_font_size accordingly.
     * Caches the values, so it's safe to call this repeatedly.
     **/
    void pickAutoFonts();

    /**
     * Returns 'true' if the UI  automatically picks fonts, disregarding Qt
     * standard settings.
     *
     * This makes sense during system installation system where the display DPI
     * cannot reliably be retrieved and thus Qt uses random font sizes based on
     * that random DPI.
     **/
    bool autoFonts() const { return _autoFonts; }

    /**
     * Set whether or not fonts should automatically be picked.
     **/
    void setAutoFonts( bool useAutoFonts );

    /**
     * Return a string for a named glyph.
     *
     * Using this is discouraged in new applications.
     *
     * Reimplemented from YApplication.
     **/
    virtual string glyph( const string & glyphSymbolName );

    /**
     * Open a directory selection box and prompt the user for an existing
     * directory.
     *
     * 'startDir' is the initial directory that is displayed.
     *
     * 'headline' is an explanatory text for the directory selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected directory name
     * or an empty string if the user canceled the operation.
     *
     * Implemented from YApplication.
     **/
    virtual string askForExistingDirectory( const string & startDir,
					    const string & headline );

    /**
     * Open a file selection box and prompt the user for an existing file.
     *
     * 'startWith' is the initial directory or file.
     *
     * 'filter' is one or more blank-separated file patterns, e.g.
     * "*.png *.jpg"
     *
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected file name
     * or an empty string if the user canceled the operation.
     *
     * Implemented from YApplication.
     **/
    virtual string askForExistingFile( const string & startWith,
				       const string & filter,
				       const string & headline );

    /**
     * Open a file selection box and prompt the user for a file to save data
     * to.  Automatically asks for confirmation if the user selects an existing
     * file.
     *
     * 'startWith' is the initial directory or file.
     *
     * 'filter' is one or more blank-separated file patterns, e.g.
     * "*.png *.jpg"
     *
     * 'headline' is an explanatory text for the file selection box.
     * Graphical UIs may omit that if no window manager is running.
     *
     * Returns the selected file name
     * or an empty string if the user canceled the operation.
     *
     * Implemented from YApplication.
     **/
    virtual string askForSaveFileName( const string & startWith,
				       const string & filter,
				       const string & headline );
    /**
     * Lower-level version that works with QStrings and does not change
     * the mouse cursor.
     **/
    static QString askForSaveFileName( const QString & startWith,
				       const QString & filter,
				       const QString & headline );

    /**
     * Return the package selector plugin singleton or creates it
     * (including loading the plugin lib) if it does not exist yet.
     **/
    static YQPackageSelectorPluginStub * packageSelectorPlugin();

    /**
     * A mouse click with the wrong mouse button was detected - e.g., a right
     * click on a push button. The user might be left-handed, but his mouse
     * might not (yet) be configured for left-handed use - e.g., during
     * installation. Ask him if he would like his mouse temporarily configured
     * as a left-handed mouse.
     *
     * This status can be queried with YQApplication::leftHandedMouse().
     **/
    void maybeLeftHandedUser();

    /**
     * Convert logical layout spacing units into device dependent units.
     * A default size dialog is assumed to be 80x25 layout spacing units.
     *
     * Reimplemented from YApplication.
     **/
    virtual int deviceUnits( YUIDimension dim, float layoutUnits );

    /**
     * Convert device dependent units into logical layout spacing units.
     * A default size dialog is assumed to be 80x25 layout spacing units.
     *
     * Reimplemented from YApplication.
     **/
    virtual float layoutUnits( YUIDimension dim, int deviceUnits );

    /**
     * Change the (mouse) cursor to indicate busy status.
     *
     * Reimplemented from YApplication.
     **/
    virtual void busyCursor();

    /**
     * Change the (mouse) cursor back from busy status to normal.
     *
     * Reimplemented from YApplication.
     **/
    virtual void normalCursor();

    /**
     * Make a screen shot and save it to the specified file.
     *
     * Reimplemented from YApplication.
     **/
    virtual void makeScreenShot( const string & fileName );

    /**
     * Beep.
     *
     * Reimplemented from YApplication.
     **/
    virtual void beep();


    // Display information and UI capabilities.
    //
    // All implemented from YApplication.

    virtual int	 displayWidth();
    virtual int	 displayHeight();
    virtual int	 displayDepth();
    virtual long displayColors();

    // Size of main dialogs
    virtual int	 defaultWidth();
    virtual int	 defaultHeight();

    //
    // UI capabilities
    //

    virtual bool isTextMode()			{ return false; }
    virtual bool hasImageSupport()		{ return true; }
    virtual bool hasIconSupport()		{ return true; }
    virtual bool hasAnimationSupport()		{ return true; }
    virtual bool hasFullUtf8Support()		{ return true; }
    virtual bool richTextSupportsTable()	{ return true; }
    virtual bool hasWizardDialogSupport()	{ return true; }
    virtual bool leftHandedMouse();


protected:

    /**
     * Constructs a key for the language specific font file:
     *     "font[lang]"
     * for
     *     font[de_DE] = "Sans Serif"
     *     font[zh] = "ChineseSpecial, something"
     *     font[ja_JP] = "JapaneseSpecial, something"
     *     font = "Sans Serif"
     **/
    QString fontKey( const QString & lang );


    //
    // Data members
    //

    // Fonts

    QFont * _currentFont;
    QFont * _headingFont;
    QFont * _boldFont;

    /**
     * Font family or list of font families to use ("Sans Serif" etc.)
     **/
    QString _fontFamily;

    /**
     * Language-specific font settings
     **/
    QSettings	* _langFonts;

    /**
     * Translator for the predefined Qt dialogs
     **/
    QTranslator * _qtTranslations;

    //
    // Misc
    //

    bool _autoFonts;
    int  _autoNormalFontSize;
    int  _autoHeadingFontSize;

    bool _leftHandedMouse;
    bool _askedForLeftHandedMouse;


};


#endif // YQApplication_h
