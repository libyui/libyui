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

  File:	      QY2Settings.cpp

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt class - it can be used independently of YaST2.

/-*/



#include "QY2Settings.h"

#include <QFile>
#include <QRegExp>

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;


QY2Settings::QY2Settings( const QString & fileName, AccessMode accessMode )
    : _fileName( fileName )
    , _accessMode( accessMode )
{
    if ( _accessMode == ReadOnly || _accessMode == ReadWrite )
	load();
    else
	initSections();
}

void QY2Settings::initSections()
{
    _defaultSection = new Section( "" );
    Q_CHECK_PTR( _defaultSection );

    _currentSection = _defaultSection;
    _sections.insert( "", _defaultSection );
}



QY2Settings::~QY2Settings()
{
    if ( _dirty && _accessMode != ReadOnly )
	save();
}


bool QY2Settings::selectSection( const QString & sectionName )
{
    _currentSection = _sections[ sectionName ];

    if ( _currentSection )
    {
	return true;
    }
    else
    {
	_currentSection = _defaultSection;
	return false;
    }
}


void QY2Settings::selectDefaultSection()
{
    _currentSection = _defaultSection;
}


QStringList QY2Settings::sections( bool includeUnnamed ) const
{
    QStringList sectionList;
    SectionIterator it( _sections );

    while ( it.hasNext() )
    {
	QString sectionName = (it.value())->name();

	if ( includeUnnamed || ! sectionName.isEmpty() )
	    sectionList.append( sectionName );

	it.next();
    }

    return sectionList;
}


QString QY2Settings::get( const QString & key, const QString & fallback ) const
{
    // Can't use operator[] here since we have a pointer

    Section::const_iterator it = _currentSection->find( key );

    if ( it == _currentSection->constEnd() )
	return fallback;

    return it.value();
}


QString QY2Settings::operator[] ( const QString & key )
{
    return get( key );
}


bool QY2Settings::hasKey( const QString & key )
{
    return _currentSection->contains( key );
}


QStringList QY2Settings::keys() const
{
    QStringList keyList;

    for ( Section::const_iterator it = _currentSection->constBegin();
	  it != _currentSection->constEnd();
	  ++it )
    {
	keyList.append( it.key() );
    }

    return keyList;
}


bool QY2Settings::load()
{
    initSections();
    _readError = false;
    
    if ( _accessMode == WriteOnly )
	return false;

    
    QFile file( _fileName );

    if ( ! file.open( QIODevice::ReadOnly ) )
    {
	cerr << "Can't load settings from " << qPrintable( _fileName )
	     << ": " << qPrintable( file.errorString() )
	     << endl;
	
	_readError = true;
	
	return false;
    }
    
    QTextStream str( &file );
    str.setAutoDetectUnicode(true);
    QString line;
    int lineCount = 0;

    while ( ! file.atEnd() )
    {
	line = str.readLine().trimmed();
	lineCount++;
	
	
	// Skip empty lines
	
	if ( line.isEmpty() ) continue;

	
	// Skip comment lines

	if ( line.startsWith( "#"  ) ) continue;
	if ( line.startsWith( ";"  ) ) continue;
	if ( line.startsWith( "//" ) ) continue;


	if ( line.startsWith( "[" ) )
	{
	    // New section

	    line.replace( QRegExp( "^\\[\\s*" ), "" );
	    line.replace( QRegExp( "\\s*\\].*$" ), "" );
	    addSection( line );
	}
	else if ( line.contains( "=" ) )
	{
	    // key=value pair

	    QString key   = line.section( "=", 0, 0 ).trimmed();
	    QString value = line.section( "=", 1, 1 ).trimmed();

	    value.replace( QRegExp( "^\"" ), "" );	// strip leading "
	    value.replace( QRegExp( "\"$" ), "" );	// strip trailing "
	    value.replace( "\\\"", "\"" );		// un-escape "

	    set( key, value );
	}
	else
	{
	    qWarning( "%s:%d: Syntax error: %s",
		      qPrintable( _fileName ), lineCount, qPrintable( line ) );
	}
    }

    _dirty = false;

    return true;
}


bool QY2Settings::save()
{
    if ( _accessMode == ReadOnly )
	return false;

    QFile file( _fileName );

    if ( ! file.open( QIODevice::WriteOnly ) )
    {
	cerr << "Can't save settings to " << qPrintable( _fileName )
	     << ": " << qPrintable( file.errorString() )
	     << endl;
	
	return false;
    }
    
    QTextStream str( &file );
    str.setAutoDetectUnicode(true);

    // The default section must be saved first since it doesn't have a section
    // name that could be used for a headline
    saveSection( str, _defaultSection );
    
    SectionIterator sectIt( _sections );
    
    while ( sectIt.hasNext() )
    {
	if ( sectIt.value() != _defaultSection )
	    saveSection( str, sectIt.value() );
	    sectIt.next();
    }

    _dirty = false;

    return true;
}


void QY2Settings::saveSection( QTextStream & str, Section * sect )
{
    // Section header

    if ( ! sect->name().isEmpty() )
	str << "[" << sect->name() << "]" << endl;

    // value=key pairs for this section

    for ( Section::iterator it = sect->begin();
	  it != sect->end();
	  ++it )
    {
	QString value = it.value();
	value.replace( "\"", "\\\"" );	// Escape embedded " with \"

	str << it.key() << "= \"" << value << "\"" << endl;
    }

    str << endl;
}


void QY2Settings::set( const QString & key, const QString & value )
{
    _currentSection->insert( key, value );
    _dirty = true;
}


void QY2Settings::addSection( const QString & sectionName )
{
    _currentSection = _sections[ sectionName ]; // already there?

    if ( _currentSection )			// -> use it
	return;

    _currentSection = new Section( sectionName );
    Q_CHECK_PTR( _currentSection );

    _sections.insert( sectionName, _currentSection );
    _dirty = true;
}


void QY2Settings::clearSection()
{
    _currentSection->clear();
    _dirty = true;
}


