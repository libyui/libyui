/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|			   contributed Qt objects                      |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      QY2CharValidator.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  This is a pure Qt object - it can be used independently of YaST2.

/-*/


#include <stdio.h>
#include <qvalidator.h>
#include "QY2CharValidator.h"



QY2CharValidator::QY2CharValidator( const QString &	initialValidChars,
				    QObject *		parent,
				    const char *	name )
    : QValidator( parent )
    , _validChars( initialValidChars )
{
  setObjectName(name);
}


QY2CharValidator::~QY2CharValidator()
{
    // NOP
}


QValidator::State
QY2CharValidator::validate( QString & fieldContents, int & pos ) const
{
    if ( validChars().isEmpty() || fieldContents.isEmpty() )
	return QValidator::Acceptable;


    // Check the entire field contents.
    //
    // There might be more than one new character - the user might have copied
    // some longer text via the X clipboard.

    for ( int i=0; i < fieldContents.length(); i++ )
    {
	if ( ! validChars().contains( fieldContents[i] ) )
	    return QValidator::Invalid;
    }

    return QValidator::Acceptable;
}


#include "QY2CharValidator.moc"
