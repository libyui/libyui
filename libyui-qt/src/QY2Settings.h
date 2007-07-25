/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|			   contributed Qt classes		       |
|						     (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:	      QY2Settings.h

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt class - it can be used independently of YaST2.

/-*/



#ifndef QY2Settings_h
#define QY2Settings_h

#include <qstring.h>
#include <qmap.h>
#include <qdict.h>
#include <qstringlist.h>
#include <qtextstream.h>

/**
 * Utility class that handles program settings in .ini file format:
 *
 * 	key="value"
 * 	key2 = value
 * 	; comment
 * 	# comment
 *
 *	[section-title]
 *	key = "value"
 *      ; comment
 *	key2="value"
 *	key3 = _( "message that needs to be translated" )
 *
 *	[section-title2]
 *	key="value"
 *	key2="value"
 *
 * key=value pairs appear one on a line each. Leading and trailing whitespace
 * is disregarded around both key and value. Value should be quoted (but
 * doesn't need to). Quotes in value are escaped with \".
 **/
class QY2Settings
{
public:

    enum AccessMode
    {
	ReadOnly,
	ReadWrite,
	WriteOnly
    };

    /**
     * Constructor. Reads settings from the specified file.
     *
     * Use readError() or readOk() afterwards to check for read errors.
     **/
    QY2Settings( const QString & fileName, AccessMode accessMode=ReadOnly );

    /**
     * Destructor.
     *
     * Writes any pending changes back to the file if there are any left
     * if accessMode() is ReadWrite or WriteOnly.
     **/
    ~QY2Settings();

    /**
     * Returns 'true' if the settings couldn't be read from the file specified
     * in the constructor.
     **/
    bool readError() const { return _readError; }

    /**
     * Returns 'true' if the settings were read without problems from the file
     * specified in the constructor. This is simply the opposite of
     * readError().
     **/
    bool readOk() const { return ! _readError; }

    /**
     * Select the specified section in the settings for subsequent get()
     * calls. Until is used, the unnamed default section is used.
     *
     * In the settings file, a section is marked with
     *
     * [section-title]
     *
     * Using a null string (QString::null) switches back to the unnamed default
     * section.
     *
     * Returns 'true' upon success, 'false' if there is no such section (in
     * which case the unnamed default section will be selected).
     **/
    bool selectSection( const QString & section );

    /**
     * Select the default (unnamed) section for subsequent get() calls.
     * This is the default unless selectSection() was called.
     **/
    void selectDefaultSection();

    /**
     * Returns the name of the current section or QString::null if the default
     * section is used.
     **/
    QString currentSection() const { return _currentSectionName; }

    /**
     * Returns a list of all sections.
     * 'includeUnnamed' specifies if the unnamed default section should be
     * included in the list.
     * The list sort order is undefined.
     **/
    QStringList sections( bool includeUnnamed = false ) const;

    /**
     * Returns the value for the specified key in the current section.
     * If there is no such key, the specified fallback value is returned.
     **/
    QString get( const QString & key,
		 const QString & fallback = "") const;

    /**
     * Same as get() with fallback always an empty string
     **/
    QString operator[] ( const QString & key );

    /**
     * Checks if the current section has the specified key.
     **/
    bool hasKey( const QString & key );

    /**
     * Returns a list of all keys in the current section.
     * The list sort order is undefined.
     **/
    QStringList keys() const;

    //
    // Write access
    //
    // All write access functions will fail if the current access mode is not
    // one of ReadWrite / WriteOnly.
    //

    /**
     * Set the specified key to the specified value. Overwrites any existing
     * key-value pair or adds a new one if there is no such key yet.
     **/
    void set( const QString & key, const QString & value );

    /**
     * Add a section with the specified name. If a section with that name
     * already exists, it will only be selected and its old contents will
     * remain untouched.
     * In any case, this section becomes the current section.
     **/
    void addSection( const QString & section );

    /**
     * Clear all key=value pairs from the current section.
     **/
    void clearSection();

    /**
     * Writes changed settings back to the file specified in the constructor if
     * accessMode() is ReadWrite or WriteOnly.
     *
     * Returns 'true' on success.
     **/
    bool save();

    /**
     * Returns 'true' if there are any changes that need to be written.
     *
     * Always returns 'false' if accessMode() is ReadOnly.
     **/
    bool pendingChanges() const { return _dirty; }

    /**
     * Returns the file name.
     **/
    QString fileName() const { return _fileName; }

    /**
     * Returns the access mode - one of ReadOnly, ReadWrite, WriteOnly.
     **/
    AccessMode accessMode() const { return _accessMode; }


protected:

    class Section: public QMap<QString, QString>
    {
    public:
	Section( const QString & name )
	    : QMap<QString, QString>()
	    , _name( name )
	    {}
	QString name() const { return _name; }

    protected:

	QString _name;
    };
    


    /**
     * Initialize the section data
     **/
    void initSections();

    /**
     * Read the settings file. Sets _readError depending on success or failure.
     * Returns 'true' upon success.
     **/
    bool load();

    /**
     * Save one section to a stream
     **/
    void saveSection( QTextStream & stream, Section * section );


    // Data members


    QString		_fileName;
    AccessMode		_accessMode;
    bool		_readError;
    QString		_currentSectionName;
    Section *		_currentSection;
    bool		_dirty;

    Section *		_defaultSection;
    QDict<Section>	_sections;
    
    typedef QDictIterator<Section>	SectionIterator;
};

#endif // QY2Settings_h
